/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
*
* Module: ddFlip.cpp                                                        *
*                                                                           *
*****************************************************************************
*  Content:    DirectDraw HAL surface flipping support
*                                                                           *
* History:                                                                  *
*       schaefer           7/6/99                                           *
*                                                                           *
\***************************************************************************/

#include "nvprecomp.h"
#include "wincommon.h"

extern DWORD __stdcall ConvertOverlay32 ( LPDDRAWI_DDRAWSURFACE_LCL, BOOL waitForCompletion, LPDWORD lpDstYOffset);

extern BOOL pmAddProcess(GLOBALDATA *pDriverData, DWORD processID);
extern BOOL pmDeleteProcess(DWORD processID);

extern BOOL ResetTwinViewState(GLOBALDATA *pDriverData);

//---------------------------------------------------------------------------

inline DWORD getFlipCountNotifier()
{
    // since the notifier buffers are used for alternate flips,
    // each notifier buffer will increment in steps of two.
    // we must check both and return the larger of the two

    // in the multihead case, there are also two dac objects.
    // in clone mode we must wait for both dacs to finish,
    // so we use the smaller of the two dac object notifiers

    if ((pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE) ||
        (pDriverData->dwDesktopState == NVTWINVIEW_STATE_SPAN))
    {
        NvNotification *pNotifierFlat1 = &pDriverData->pDmaFlipPrimaryNotifierFlat[0 * 16]; // head 0
        NvNotification *pNotifierFlat2 = &pDriverData->pDmaFlipPrimaryNotifierFlat[1 * 16]; // head 1
        DWORD dwCount1 = max(pNotifierFlat1[1].info32, pNotifierFlat1[2].info32);
        DWORD dwCount2 = max(pNotifierFlat2[1].info32, pNotifierFlat2[2].info32);

        return min(dwCount1, dwCount2);
    }
    else
    {
        DWORD dwHead = GET_CURRENT_HEAD();
        NvNotification *pNotifierFlat = &pDriverData->pDmaFlipPrimaryNotifierFlat[dwHead * 16];

        return max(pNotifierFlat[1].info32, pNotifierFlat[2].info32);
    }
}

//---------------------------------------------------------------------------

/*
 * getFlipStatusPrimary
 *
 * Checks if the most recent flip on the given primary surface has completed.
 * If the given primary is 0, checks for pending flips on any surface
 */

HRESULT __stdcall getFlipStatusPrimary (FLATPTR fpVidMem)
{

#if (NVARCH >= 0x010) && defined(HWFLIP)

    DWORD dwCurrentIndex, dwIndex;

#if (NVARCH >= 0x020)
    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS | NVCLASS_FAMILY_KELVIN))
#else
    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS))
#endif
    {
        DWORD dwQueuedFlips, dwFlipCountNotifier;

        dwFlipCountNotifier = getFlipCountNotifier();
        if (CURRENT_FLIP >= dwFlipCountNotifier) {
            dwQueuedFlips = CURRENT_FLIP - dwFlipCountNotifier;
        }
        else { // handle the wrap around case
            dwQueuedFlips = dwFlipCountNotifier - CURRENT_FLIP;
        }

        if (dwQueuedFlips && (fpVidMem == FLIP_ANYSURFACE))
            return DDERR_WASSTILLDRAWING;

        dwCurrentIndex = getDC()->flipHistory.dwIndex;
        for (DWORD i = 0; i < dwQueuedFlips; i++) {
            dwIndex = (dwCurrentIndex - i) & FLIP_HISTORY_MASK;
#ifdef CHECK_FLIP_TO
            if ((getDC()->flipHistory.surfacesFrom[dwIndex] == fpVidMem) ||
                (getDC()->flipHistory.surfacesTo[dwIndex]   == fpVidMem)) {
        return (DDERR_WASSTILLDRAWING);
            }
#else
            if (getDC()->flipHistory.surfacesFrom[dwIndex] == fpVidMem) {
                return (DDERR_WASSTILLDRAWING);
            }
#endif
        }

        return (DD_OK);
    }

#endif  // HWFLIP

    /*
     * see if a flip has happened recently
     */
    if (getDC()->flipPrimaryRecord.bFlipFlag &&
        ((fpVidMem == FLIP_ANYSURFACE) || (fpVidMem == getDC()->flipPrimaryRecord.fpFlipFrom)) ) {

        NvNotification *pDmaFlipPrimaryNotifier = pDriverData->pDmaFlipPrimaryNotifierFlat;

        // D3D: if VSYNC is forced off, we always advertise that flips have completed
        //      this can break ddraw apps and maybe even fail WHQL - therefore
        //      we do not recommend VSYNC ever be disabled by default
        if (getDC()->nvD3DRegistryData.regVSyncMode == D3D_REG_VSYNCMODE_FORCEOFF) {
            return DD_OK;
        }

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_5) {
            if (((volatile)pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) ||
                ((volatile)pDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS))
                return (DDERR_WASSTILLDRAWING);
            else {
                pDriverData->CurrentVisibleSurfaceAddress = getDC()->flipPrimaryRecord.fpFlipTo;
                getDC()->flipPrimaryRecord.bFlipFlag = FALSE;
                return (DD_OK);
            }
        }
        else {
            NvNotification *pDmaSyncNotifier = pDriverData->pDmaSyncNotifierFlat;
            /* pDmaSyncNotifier is used for NOVSYNC flips for devices < NV5 */
            if (((volatile)pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) ||
                ((volatile)pDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS) ||
                ((pDriverData->fIsBltStyleFlip) &&
                 (pDmaSyncNotifier->status == NV_IN_PROGRESS)))
                return (DDERR_WASSTILLDRAWING);
            else {
                pDriverData->CurrentVisibleSurfaceAddress = getDC()->flipPrimaryRecord.fpFlipTo;
                getDC()->flipPrimaryRecord.bFlipFlag = FALSE;
                return (DD_OK);
            }
        }
    }

    return (DD_OK);

} // getFlipStatusPrimary

//---------------------------------------------------------------------------

/*
 * getFrameStatusOverlay
 *
 * Checks if the most recent flip on the given overlay surface has occurred
 */

HRESULT __stdcall getFlipStatusOverlay (FLATPTR fpVidMem)
{
    int iIndex;

    if (getDC()->flipOverlayRecord.bFlipFlag &&
        ((fpVidMem == FLIP_ANYSURFACE) ||
         (fpVidMem == getDC()->flipOverlayRecord.fpFlipFrom) ||
         (fpVidMem == getDC()->flipOverlayRecord.fpPreviousFlipFrom)) ) {

        NvNotification *pPioFlipOverlayNotifier = pDriverData->pPioFlipOverlayNotifierFlat;

        if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
            if (fpVidMem == getDC()->flipOverlayRecord.fpPreviousFlipFrom) {
                // only need for one to flush out
                iIndex = pDriverData->vpp.overlayBufferIndex + 1;
                if (pPioFlipOverlayNotifier[iIndex].status == NV_IN_PROGRESS)
                    return (DDERR_WASSTILLDRAWING);
            } else {
                // need to wait for both flips to flush out
                if (((volatile)pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) ||
                    ((volatile)pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                    return (DDERR_WASSTILLDRAWING);
                } else {
                    getDC()->flipOverlayRecord.bFlipFlag = FALSE;
                }
            }
        } else {
            if (fpVidMem == getDC()->flipOverlayRecord.fpPreviousFlipFrom) {
/* The WHQL flip status test doesn't like this conservatism.  Comment out for now.
                LONGLONG timeNow;
                // only need for one to flush out, no way to really determine this on NV10,
                // but if we wait at least one CRTC refresh since it was issued, we can
                // guarantee it has been flushed through
                if ((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) &&
                    (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                    QueryPerformanceCounter((LARGE_INTEGER *) &timeNow);
                    if ((timeNow - getDC()->flipOverlayRecord.llFlipTime) < (getDC()->flipPrimaryRecord.dwFlipDuration * 2)) {
                        return (DDERR_WASSTILLDRAWING);
                    }
                }
*/
            } else {
                if (fpVidMem == getDC()->flipOverlayRecord.fpFlipFrom) {
                    LONGLONG timeNow = 0x7FFFFFFFFFFFFFFF;
                    DWORD    dwTimeoutCount;

                    // We use different timeout values for 640x480 and higher res modes.
                    if (GET_MODE_HEIGHT() <= 480)
                        dwTimeoutCount = 4;
                    else
                        dwTimeoutCount = 16;

                    // need to wait for both flips to flush out
                    if (((volatile)pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) &&
                        ((volatile)pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                        LONGLONG timeNow = 0x7FFFFFFFFFFFFFFF;

                        // If notifiers don't get set for two CRTC refresh times, time out and return success.
                        QueryPerformanceCounter((LARGE_INTEGER *) &timeNow);
                        if ((timeNow - getDC()->flipOverlayRecord.llFlipTime) < (getDC()->flipPrimaryRecord.dwFlipDuration * dwTimeoutCount)) {
                            return (DDERR_WASSTILLDRAWING);
                        }
                        else {
                            getDC()->flipOverlayRecord.bFlipFlag = FALSE;

                            // Reset notifiers that may be stuck.
                            pPioFlipOverlayNotifier[1].status = 0;
                            pPioFlipOverlayNotifier[2].status = 0;
                        }
                    } else {
                        getDC()->flipOverlayRecord.bFlipFlag = FALSE;
                    }
                }
            }
        }
        return (DD_OK);
    }

    return (DD_OK);

} // getFrameStatusOverlay

//---------------------------------------------------------------------------

// update frame status on surface of indeterminate type

HRESULT __stdcall getFlipStatus (FLATPTR fpVidMem, DWORD dwType)
{
    if (dwType & (DDSCAPS_OVERLAY | DDSCAPS_VIDEOPORT)) {
        return getFlipStatusOverlay (fpVidMem);
    }
    else {
        return getFlipStatusPrimary (fpVidMem);
    }
}

//---------------------------------------------------------------------------

// GetFlipStatus32

DWORD __stdcall GetFlipStatus32 (LPDDHAL_GETFLIPSTATUSDATA lpGetFlipStatus)
{
    nvSetDriverDataPtrFromDDGbl (lpGetFlipStatus->lpDD);

    // validate device
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        NvReleaseSemaphore(pDriverData);
        lpGetFlipStatus->ddRVal = DDERR_SURFACELOST;
        return (DDHAL_DRIVER_HANDLED);
    }

    // validate flags
    DWORD dwFlagsAll = (DDGFS_CANFLIP | DDGFS_ISFLIPDONE);
    DWORD dwFlagsSet = lpGetFlipStatus->dwFlags & dwFlagsAll;
    if ((dwFlagsSet == 0) || (dwFlagsSet == dwFlagsAll)) {
        DPF ("invalid params in GetBlitStatus32");
        NvReleaseSemaphore(pDriverData);
        lpGetFlipStatus->ddRVal = DDERR_INVALIDPARAMS;
        return (DDHAL_DRIVER_HANDLED);
    }

    if (dwFlagsSet & DDGFS_CANFLIP) {

        if (lpGetFlipStatus->lpDDSurface->ddsCaps.dwCaps & (DDSCAPS_OVERLAY | DDSCAPS_VIDEOPORT)) {

            // handle overlays and videoports
            LPPROCESSINFO lpProcInfo = pmGetProcess(pDriverData, GetCurrentProcessId());
            if ((IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hOvlFlipEvenEvent3, 0))) &&
                (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hOvlFlipOddEvent3, 0)))) {
                lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
            }
            /*
            NvNotification *pPioFlipOverlayNotifier = (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;
            iIndex = pDriverData->OverlayBufferIndex + 1;
            if (pPioFlipOverlayNotifier[iIndex].status == NV_IN_PROGRESS)
                lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;  */

        }

        else {

            // get flip status for the primary
#if (NVARCH >= 0x010) && defined(HWFLIP)
#if (NVARCH >= 0x020)
            if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS | NVCLASS_FAMILY_KELVIN))
#else
            if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS))
#endif
            {
                // with nv15 (etc), we can always flip
                lpGetFlipStatus->ddRVal = DD_OK;
            }
            else
#endif  // HWFLIP
            {
                // make sure we sync with other channels before writing put
                getDC()->nvPusher.setSyncChannelFlag();
                if (getDC()->nvPusher.isIdle(TRUE)) {
                    lpGetFlipStatus->ddRVal = DD_OK;
                }
                else {
                    nvPusherStart (TRUE);
                    lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
                }
            }
        }
    }

    else {  // DDGFS_ISFLIPDONE

        // simply get flip status
        lpGetFlipStatus->ddRVal = getFlipStatus (lpGetFlipStatus->lpDDSurface->lpGbl->fpVidMem,
                                                 lpGetFlipStatus->lpDDSurface->ddsCaps.dwCaps);

        // OLD CODE FOR OVERLAYS ONLY:
        // (current thinking is that getFrameStatus always works)
        /*
                NvNotification *pPioFlipOverlayNotifier = (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;
                if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
                {
                    if (((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) &&
                        (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) &&
                        ((fpVidMem == getDC()->flipOverlayRecord.fpPreviousFlipFrom) ||
                         (fpVidMem == getDC()->flipOverlayRecord.fpFlipFrom) ||
                         (fpVidMem == getDC()->flipOverlayRecord.fpFlipTo) ||
                         (fpVidMem == NULL)))
                        lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
                }
                else
                {
                    if (((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) ||
                         (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) &&
                        ((fpVidMem == getDC()->flipOverlayRecord.fpPreviousFlipFrom) ||
                         (fpVidMem == getDC()->flipOverlayRecord.fpFlipFrom) ||
                         (fpVidMem == getDC()->flipOverlayRecord.fpFlipTo) ||
                         (fpVidMem == NULL)))
                        lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
                }
    */

    }

    NvReleaseSemaphore(pDriverData);
    return (DDHAL_DRIVER_HANDLED);

} // GetFlipStatus32

//---------------------------------------------------------------------------

// Check how many frames we are ahead
// we want to limit this since we have serious lag effect on fast CPUs.
// if we're too many frames ahead, we wait.
// free count for d3d stuff must be accurate

void nvCheckQueuedFlips (void)
{
    // max queued frames is regPreRenderLimit (defaults to 3 - PC99 spec)
    DWORD dwMaxQueuedFrames = getDC()->nvD3DRegistryData.regPreRenderLimit;

    // get HW frame #
    COMPLETED_FLIP = getFlipCountNotifier();

    // have we progressed too far?
    while (int(CURRENT_FLIP - COMPLETED_FLIP) > int(dwMaxQueuedFrames))
    {
        // kick off buffer
        nvPusherStart (TRUE);
        // wait for HW to catch up
        nvDelay();
        COMPLETED_FLIP = getFlipCountNotifier();
    }
}

//---------------------------------------------------------------------------

void nvUpdateFlipTracker (DWORD dwSurfaceFrom, DWORD dwSurfaceTo)
{
    // make a note of the fact that we've queued a flip or blit involving these surfaces
    getDC()->flipHistory.dwIndex ++;
    getDC()->flipHistory.dwIndex &= FLIP_HISTORY_MASK;
    getDC()->flipHistory.surfacesFrom[getDC()->flipHistory.dwIndex] = dwSurfaceFrom;
#ifdef CHECK_FLIP_TO
    getDC()->flipHistory.surfacesTo[getDC()->flipHistory.dwIndex]   = dwSurfaceTo;
#endif
    // update frame counters
    CURRENT_FLIP ++;
}

//---------------------------------------------------------------------------

// do a blit instead of a flip

void nvFlipBlit (LPDDHAL_FLIPDATA pfd)
{
    NvNotification *pDmaSyncNotifier = pDriverData->pDmaSyncNotifierFlat;

    DWORD dwSrcOffset = (DWORD)pfd->lpSurfTarg->lpGbl->fpVidMem - (DWORD)pDriverData->BaseAddress;
    DWORD dwDstOffset = (DWORD)pfd->lpSurfCurr->lpGbl->fpVidMem - (DWORD)pDriverData->BaseAddress;
    DWORD dwPitch     = (pfd->lpSurfTarg->lpGbl->lPitch  << 16) | pfd->lpSurfCurr->lpGbl->lPitch;
    DWORD dwDims      = (pfd->lpSurfCurr->lpGbl->wHeight << 16) | pfd->lpSurfCurr->lpGbl->wWidth;

    if (dwDstOffset == dwSrcOffset) dwDstOffset = 0;

    if (pDriverData->dwSharedClipChangeCount != pDriverData->dwDDMostRecentClipChangeCount) {

        nvPushData (0,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
        nvPushData (1,NV_DD_IMAGE_BLACK_RECTANGLE);
        nvPushData (2,dDrawSubchannelOffset(NV_DD_SURFACES) + IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000);
        nvPushData (3,0);
        nvPushData (4,asmMergeCoords(NV_MAX_X_CLIP, NV_MAX_Y_CLIP));
        nvPushData (5,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
        nvPushData (6,NV_DD_SURFACES_2D);

        nvPusherAdjust (7);
        nvPusherStart  (TRUE);

        pDriverData->dwSharedClipChangeCount++;

        pDriverData->dwDDMostRecentClipChangeCount = pDriverData->dwSharedClipChangeCount;
    }

    nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP) + SET_ROP_OFFSET | 0x40000);
    nvPushData (1,SRCCOPYINDEX);
    pDriverData->bltData.dwLastRop = SRCCOPYINDEX;

    nvPushData (2,dDrawSubchannelOffset(NV_DD_SURFACES) + SURFACES_2D_PITCH_OFFSET | 0xC0000);
    nvPushData (3,dwPitch);
    nvPushData (4,dwSrcOffset);
    nvPushData (5,dwDstOffset);
    pDriverData->bltData.dwLastCombinedPitch = dwPitch;
    pDriverData->bltData.dwLastSrcOffset = dwSrcOffset;
    pDriverData->bltData.dwLastDstOffset = dwDstOffset;

    nvPushData (6,dDrawSubchannelOffset(NV_DD_BLIT) + BLIT_POINT_IN_OFFSET | 0xC0000);
    nvPushData (7,0);
    nvPushData (8,0);
    nvPushData (9,dwDims);

    nvPusherAdjust (10);

    if (pfd->dwFlags & DDFLIP_NOVSYNC) {

        getDC()->flipPrimaryRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
        getDC()->flipPrimaryRecord.fpFlipTo = pfd->lpSurfTarg->lpGbl->fpVidMem;
        getDC()->flipPrimaryRecord.bFlipFlag = TRUE;
        pDriverData->fIsBltStyleFlip = TRUE;

        pDmaSyncNotifier->status = NV_IN_PROGRESS;

        nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000);
        nvPushData (1,0);
        nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000);
        nvPushData (3,0);

        nvPusherAdjust (4);
    }

    nvUpdateFlipTracker (pfd->lpSurfCurr->lpGbl->fpVidMem, pfd->lpSurfTarg->lpGbl->fpVidMem);

    nvPusherStart (TRUE);

    // flip the vidmem pointers (so ddraw can flip them back)
    pfd->lpSurfCurr->lpGbl->fpVidMem = dwSrcOffset + (DWORD)pDriverData->BaseAddress;
    pfd->lpSurfTarg->lpGbl->fpVidMem = dwDstOffset + (DWORD)pDriverData->BaseAddress;

}

//---------------------------------------------------------------------------

void nvFlipPrimary (LPDDHAL_FLIPDATA pfd, BOOL bNoVSync)
{
    LPDDRAWI_DDRAWSURFACE_GBL   pSurfTarg_gbl;

    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        pfd->ddRVal = DDERR_SURFACELOST;
        return;
    }

    pSurfTarg_gbl = pfd->lpSurfTarg->lpGbl;

    // assume OK unless we find otherwise
    pfd->ddRVal = DD_OK;

#if (NVARCH >= 0x010) && defined(HWFLIP)

#if (NVARCH >= 0x020)

#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
        getDC()->nvFlipper.flip (&(getDC()->nvPusher), &(getDC()->nvPusherDAC),
                                 pfd->lpSurfCurr, pfd->lpSurfTarg, bNoVSync);
    }
    else if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS)) {
#else // !KELVIN_SEMAPHORES
    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS | NVCLASS_FAMILY_KELVIN)) {
#endif // !KELVIN_SEMAPHORES

#else // !(NVARCH >= 0x020)

    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS)) {

#endif // !(NVARCH >= 0x020)

        // tell the HW we want to start writing to the next buffer
        nvglSetNv15CelsiusSyncIncWrite (NV_DD_CELSIUS);

        // a bug in the HW requires a no-op here. otherwise the stall may
        // get processed before the write actually gets done.
        nvglSetNv10CelsiusNoOp (NV_DD_CELSIUS);

        PRIMARY_BUFFER_INDEX ^= 0x1;

        BOOL  bCloneMode = (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE);
        BOOL  bSpanMode  = (pDriverData->dwDesktopState == NVTWINVIEW_STATE_SPAN);
        DWORD dwNumHeads = (bCloneMode || bSpanMode) ? pDriverData->dwHeads : 1;
        DWORD dwFormat   = (NV07C_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31) |
                           (NV07C_SET_IMAGE_FORMAT_FLAGS_COMPLETE_ON_OBJECT_CLEANUP << 25) |
                           pSurfTarg_gbl->lPitch;
        dwFormat |= bNoVSync   ? (NV07C_SET_IMAGE_FORMAT_WHEN_IMMEDIATELY << 20) :
                                 (NV07C_SET_IMAGE_FORMAT_WHEN_NOT_WITHIN_BUFFER << 20);
        dwFormat |= (bCloneMode || bSpanMode) ? (NV07C_SET_IMAGE_FORMAT_MULTIHEAD_SYNC_ENABLED << 24) :
                                                (NV07C_SET_IMAGE_FORMAT_MULTIHEAD_SYNC_DISABLED << 24);

        for (DWORD dwLogicalHead = 0; dwLogicalHead < dwNumHeads; dwLogicalHead++) {
            // calculate offset, the RM will add the pan & scan adjustment from the display driver if needed
            DWORD dwOffset = pSurfTarg_gbl->fpVidMem - pDriverData->BaseAddress;
#ifdef WINNT
            DWORD dwDAC          = NV_DD_VIDEO_LUT_CURSOR_DAC;
            DWORD dwPhysicalHead = ppdev->ulDeviceDisplay[dwLogicalHead];
#else // !WINNT
            DWORD dwDAC          = NV_DD_VIDEO_LUT_CURSOR_DAC + (bCloneMode ? 0 : GET_CURRENT_HEAD());
            DWORD dwPhysicalHead = dwLogicalHead;
#endif // !WINNT

            if (bSpanMode) {
                // add the span mode offset
                dwOffset += GET_SPAN_OFFSET(dwPhysicalHead);
            }

            // program the flip
            nvglSetObject (NV_DD_SURFACES, dwDAC + dwPhysicalHead); // load the DAC object
            nvglSetNv15VideoLUTCursorDACImageData (NV_DD_SURFACES, PRIMARY_BUFFER_INDEX, dwOffset , dwFormat);
        }

        // stall the graphics pipe until we've flipped away from the buffer we want to write to
        nvglSetNv15CelsiusSyncStall (NV_DD_CELSIUS);

        // put back the NV_DD_SURFACES_2D object
        nvglSetObject (NV_DD_SURFACES, NV_DD_SURFACES_2D);
    }

    else
#endif  // HWFLIP
    {
        NvNotification *pDmaSyncNotifier        = pDriverData->pDmaSyncNotifierFlat;
        NvNotification *pDmaFlipPrimaryNotifier = pDriverData->pDmaFlipPrimaryNotifierFlat;

        // find out if any previous flips are still pending
        HRESULT ddrval = getFlipStatusPrimary (FLIP_ANYSURFACE);

        if (ddrval != DD_OK) {
            // flush the push buffer if we haven't yet done so for this particular flip
            if (!(pDriverData->fDmaPushBufferHasBeenFlushed)) {

                // make sure we sync with other channels before writing put
                getDC()->nvPusher.setSyncChannelFlag();

                // Let D3D code know that we have touched NV
                pDriverData->TwoDRenderingOccurred = 1;

                pDmaSyncNotifier->status = NV_IN_PROGRESS;
                nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000);
                nvPushData (1,0);
                nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000);
                nvPushData (3,0);

                nvPusherAdjust (4);
                nvPusherStart  (TRUE);

                pDriverData->fDmaPushBufferHasBeenFlushed = TRUE;
            }

            // if we've been instructed to wait, wait until we can do the flip and proceed
            // otherwise, just return the failure (the app will try again)
            if (pfd->dwFlags & DDFLIP_WAIT) {
                while (ddrval != DD_OK) {
                    NV_SLEEPFLIP;
                    ddrval = getFlipStatusPrimary(FLIP_ANYSURFACE);
                }
            }
            else {
                pfd->ddRVal = ddrval;
                return;
            }

        }

        // make sure we sync with other channels before writing put
        getDC()->nvPusher.setSyncChannelFlag();

        // we do not want this flip to wait for itself
        getDC()->nvPusher.clearSyncFlipFlag();

        PRIMARY_BUFFER_INDEX ^= 0x1;

        if (!bNoVSync) {
            pDmaFlipPrimaryNotifier[PRIMARY_BUFFER_INDEX + 1].status = NV_IN_PROGRESS;
        }

        // Temporarily load VIDEO_LUT_CURSOR_DAC object
        nvPushData (0,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
        nvPushData (1,NV_DD_VIDEO_LUT_CURSOR_DAC);
        nvPushData (2,dDrawSubchannelOffset(NV_DD_SURFACES) + NV067_SET_IMAGE_OFFSET(PRIMARY_BUFFER_INDEX) | 0x80000);
        nvPushData (3,pSurfTarg_gbl->fpVidMem - pDriverData->BaseAddress);
        if (bNoVSync) {
            nvPushData (4,(NV067_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31) |
                          (NV067_SET_IMAGE_FORMAT_WHEN_IMMEDIATELY << 20)  |
                          pSurfTarg_gbl->lPitch);
        }
        else {
            nvPushData (4,(NV067_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31)      |
                          (NV067_SET_IMAGE_FORMAT_WHEN_NOT_WITHIN_BUFFER << 20) |
                          pSurfTarg_gbl->lPitch);
        }
        // put back the NV_DD_SURFACES2D object
        nvPushData (5,dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000);
        nvPushData (6,NV_DD_SURFACES_2D);

        nvPusherAdjust (7);

        getDC()->flipPrimaryRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
        getDC()->flipPrimaryRecord.fpFlipTo   = pSurfTarg_gbl->fpVidMem;
        getDC()->flipPrimaryRecord.bFlipFlag  = TRUE;

        pDriverData->fDmaPushBufferHasBeenFlushed = FALSE;
    }

    // make a note of the fact that we've advanced a frame
    nvUpdateFlipTracker (pfd->lpSurfCurr->lpGbl->fpVidMem, pfd->lpSurfTarg->lpGbl->fpVidMem);

    // get things started
    nvPusherStart (TRUE);
#if (NVARCH >= 0x020)
#ifdef KELVIN_SEMAPHORES
    if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
        (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
        (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
    {
#ifdef NV_PROFILE_PUSHER
        nvpSetLogPusher(false);
#endif
        getDC()->nvPusherDAC.start (TRUE);
#ifdef NV_PROFILE_PUSHER
        nvpSetLogPusher(true);
#endif
    }
#endif
#endif

    DPF_LEVEL (NVDBG_LEVEL_SURFACE, "nvFlipPrimary: RGB Flip request completed successfully");
}

//---------------------------------------------------------------------------

/* NOTES:
 *
 * When flipping overlay surfaces NOT associated with a video port
 * the surface will be flipped according to the following criteria.
 * DDOVER_INTERLEAVED and DDOVER_BOB values used are those set on most
 * recent UpdateOverlay32 call referencing this overlay surface.
 *
 *  DDOVER_INTERLEAVED  DDOVER_BOB  DDFLIP_EVEN DDFLIP_ODD  DISPLAYED
 *
 *          1               1            1          0       Even field
 *          1               1            X          1       Odd field with BOB
 *          1               1            0          0       Full interleaved surface
 *          1               0            1          0       Even field
 *          1               0            X          1       Odd field
 *          1               0            0          0       Full interleaved surface
 *          0               1            X          X       Full non-interleaved surface
 *          0               0            X          X       Full non-interleaved surface
 */

void nvFlipOverlay (LPDDHAL_FLIPDATA pfd)
{
    LPDDRAWI_DDRAWSURFACE_LCL   lpSurfTarg;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurfTarg_gbl;

    DWORD   dwVPPFlags;
#ifndef WINNT
    DWORD   dwSavedNBData;
#endif
    HRESULT ddrval;

    dbgTracePush ("nvFlipOverlay");

    lpSurfTarg    = pfd->lpSurfTarg;
    pSurfTarg_gbl = lpSurfTarg->lpGbl;

    // presume OK unless we find otherwise
    pfd->ddRVal = DD_OK;

#ifdef WINNT
    // WIN2K HACK: DX7 under win2k presents incorrect flags, so manually fix them
    if (pfd->lpSurfCurr) {
        pfd->lpSurfCurr->ddsCaps.dwCaps &= ~(DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER);
        pfd->lpSurfCurr->ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER;
    }
    lpSurfTarg->ddsCaps.dwCaps |= DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER;
    lpSurfTarg->ddsCaps.dwCaps &= ~DDSCAPS_BACKBUFFER;

    // WIN2K HACK: More flags that aren't set properly by Win2K DX7:
    // dwOverlayFlags may have Bob and Interleaved flags set for some overlay buffers in
    // the flip chain but not others.  We need to fix them for Bob mode to work later on.
    if ((lpSurfTarg->lpGbl->ddpfSurface.dwFourCC == FOURCC_NV12) && (lpSurfTarg->lpSurfMore)) {
        if (pfd->dwFlags & (DDFLIP_ODD | DDFLIP_EVEN)) {
            pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags |= DDOVER_INTERLEAVED;
            pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags |= DDOVER_BOB;
        }
        else {
            pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags &= ~DDOVER_INTERLEAVED;
            pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags &= ~DDOVER_BOB;
        }
    }

#endif WINNT

    pDriverData->vpp.dwOverlayLastVisibleSurfaceLCL = (DWORD) lpSurfTarg;
    pDriverData->vpp.dwOverlayLastVisibleFpVidMem = pSurfTarg_gbl->fpVidMem;
    pDriverData->vpp.dwOverlayLastVisiblePitch = pSurfTarg_gbl->lPitch;
    pDriverData->vpp.dwOverlayLastVisiblePixelFormat = pSurfTarg_gbl->ddpfSurface.dwFourCC;

    if (pDriverData->vpp.dwOverlayFSHead == 0xFFFFFFFE &&
        pDriverData->vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pHWCrtcCX == 0) {
        // TwinView state has changed while video mirror enabled, make the app reinstantiate the overlay
        pfd->ddRVal = DDERR_SURFACELOST;
    }

    // Do some DirectShow compatibility stuff
    if (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_NVMC) {
#if (NVARCH >= 0x10)
        dbgTracePop();
        return;
#endif  // NVARCH >= 0x10
    }

    else if ((pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_NV12) || (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_NVDS)) {

        LPNVMCSURFACEFLAGS lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

#if (NVARCH >= 0x10)

        if (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_NVDS) {
            LPNVMCFRAMEDATA lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(pSurfTarg_gbl->fpVidMem);
            DWORD dwIndex = lpNvMCFrameData->bMCDestinationSurfaceIndex;
            if (dwIndex >= 8) {
                dbgTracePop();
                return;
            }
            lpSurfTarg = (LPDDRAWI_DDRAWSURFACE_LCL)(pDriverData->dwMCNV12Surface[dwIndex]);
            pSurfTarg_gbl = lpSurfTarg->lpGbl;

            // Check for mode switches and full screen DOS interruptions
            if (pDriverData->dwMCDestinationSurface == 0) {
                pfd->ddRVal = DDERR_SURFACELOST;
                dbgTracePop();
                return;
            }
        }

        while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
               (lpSurfaceFlags->dwMCSurfaceBase != pSurfTarg_gbl->fpVidMem)) {
            lpSurfaceFlags++;
        }

        // If this is an NV12 surface and DXVA Back End alpha blending is enabled,
        // perform an alpha blend operation on this frame.
        if ((pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_NV12) &&
            (pDriverData->dwDXVAFlags & DXVA_BACK_END_ALPHA_ENABLED) &&
            pDriverData->dwMCNVMCSurface) {

            // Make sure you only blend each frame once -- in Bob mode we get two Flips per frame.
            if (lpSurfaceFlags->bMCFrameIsComposited == FALSE) {
                nvDXVABackEndAlphaBlend(lpSurfTarg->lpGbl->fpVidMem);
                lpSurfaceFlags->bMCFrameIsComposited = TRUE;
            }
        }

        // Make sure the surface has been format converted
        if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) &&
            (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
            nvMoCompConvertSurfaceFormat(pSurfTarg_gbl->fpVidMem, TRUE, 15);
        else if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD) &&
            ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3))
            nvMoCompConvertSurfaceFormat(pSurfTarg_gbl->fpVidMem, TRUE, 3);
        else if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) &&
            (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 12))
            nvMoCompConvertSurfaceFormat(pSurfTarg_gbl->fpVidMem, TRUE, 12);

        if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted == 15) &&
            (lpSurfaceFlags->bMCFrameIsFiltered == FALSE) &&
            (lpSurfaceFlags->bMCFrameIsComposited == FALSE) &&
            (pDriverData->bMCTemporalFilterDisabled == FALSE) &&
            ((pfd->dwFlags & (DDFLIP_ODD | DDFLIP_EVEN)) != 0))
            nvMoCompTemporalFilter(pSurfTarg_gbl->fpVidMem, pDriverData->bMCPercentCurrentField);

#endif  // NVARCH >= 0x10

        if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
            pfd->ddRVal = DDERR_SURFACELOST;
            dbgTracePop();
            return;
        }

        // Wait for the surface to finish format conversion.
        getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                       lpSurfaceFlags->dwMCFormatCnvReferenceCount);

        // If this is a composited surface, wait for it to finish compositing.
        if (lpSurfaceFlags->bMCFrameIsComposited) {
            getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                      lpSurfaceFlags->dwMCCompositeReferenceCount);
        }

        // If this is a filtered surface, wait for it to finish filtering.
        if (lpSurfaceFlags->bMCFrameIsFiltered) {
            getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                           lpSurfaceFlags->dwMCTemporalFilterReferenceCount);
        }

        // Check if filtering is disabled on this surface for this display call only
        if (lpSurfaceFlags->bMCFrameType & TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE) {
            lpSurfaceFlags->bMCFrameIsFiltered = FALSE;
            lpSurfaceFlags->bMCFrameType &= ~TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE;
        }

        // Always display non-filtered portion of non-interleaved frames
        if ((pfd->dwFlags & (DDFLIP_ODD | DDFLIP_EVEN)) == 0) {
            if (lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME)
                lpSurfaceFlags->bMCFrameIsFiltered = FALSE;
        }
    }

    if ((getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) ||
        pDriverData->vpp.dwOverlayFSOvlLost) {
        pfd->ddRVal = DDERR_SURFACELOST;
        dbgTracePop();
        return;
    }

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvDDrawRecover();
        }
        else {
            pfd->ddRVal = DDERR_SURFACEBUSY;
            dbgTracePop();
            return;
        }
    }

#ifndef WINNT
    // check for VIA 82C598 north bridge
    if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
        // turn off write posting
        _asm {
                mov edx, 0cf8h
                mov eax, 80000840h
                out dx, eax
                mov edx, 0cfch
                mov eax, 0
                in  al, dx
                mov dwSavedNBData, eax
                and eax, 0FFFFFF7Fh
                out dx, al
        }
    }
#endif

    /* Limit flips to 1 at a time to work around NV4 RM limitation */
    ddrval = getFlipStatusOverlay (pSurfTarg_gbl->fpVidMem);

    /* Only reject flip requests when flipping frames (but allow fields to proceed) */
    if ((pfd->dwFlags & (DDFLIP_ODD | DDFLIP_EVEN)) == 0) {
        if (ddrval != DD_OK) {
            if (pfd->dwFlags & DDFLIP_WAIT) {
                while (ddrval != DD_OK) {
                    LPPROCESSINFO lpProcInfo = pmGetProcess (pDriverData, GetCurrentProcessId());
                    if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hOvlFlipEvenEvent3, VPP_TIMEOUT_TIME))) {
                        EVENT_ERROR;
                    }
                    if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hOvlFlipOddEvent3, VPP_TIMEOUT_TIME))) {
                        EVENT_ERROR;
                    }
                    ddrval = getFlipStatusOverlay (pSurfTarg_gbl->fpVidMem);
                }
            }

            else {
                pfd->ddRVal = ddrval;
#ifndef WINNT
                // check for VIA 82C598 north bridge
                if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
                    // restore write posting value
                    _asm {
                            mov edx, 0cf8h
                            mov eax, 80000840h
                            out dx, eax
                            mov edx, 0cfch
                            mov eax, dwSavedNBData
                            out dx, al
                    }
                }
#endif
                dbgTracePop();
                return;
            }
        }

    } else if (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_NV12) {
        if (ddrval != DD_OK) {
            NV_SLEEPFLIP;
            pfd->ddRVal = ddrval;
            dbgTracePop();
            return;
        }
    }

    /*
     * make sure we sync with other channels before writing put
     */
    getDC()->nvPusher.setSyncChannelFlag();

    dwVPPFlags = VPP_ALL;
    if ((pfd->lpSurfTarg->lpGbl->ddpfSurface.dwFourCC == FOURCC_NVDS) && (pfd->lpSurfTarg->lpSurfMore)) {
        if (pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_INTERLEAVED) {
            dwVPPFlags |= VPP_INTERLEAVED;
        }
        if (pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_BOB) {
            dwVPPFlags |= VPP_BOB;
        }
    } else if (lpSurfTarg->lpSurfMore) {
        if (lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_INTERLEAVED) {
            dwVPPFlags |= VPP_INTERLEAVED;
        }
        if (lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_BOB) {
            dwVPPFlags |= VPP_BOB;
        }
    }
    if (pfd->dwFlags & DDFLIP_ODD) {
        dwVPPFlags |= VPP_ODD;
    } else if (pfd->dwFlags & DDFLIP_EVEN) {
        dwVPPFlags |= VPP_EVEN;
    }
    if (lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) {
        dwVPPFlags |= VPP_VIDEOPORT;
    }
    if (pfd->dwFlags & DDFLIP_WAIT) {
        dwVPPFlags |= VPP_WAIT;
    }

    // don't call VPP stuff if the Ring0 KMVT stuff is active
    if (pDriverData->bRing0FlippingFlag == FALSE) {
#ifdef DEBUG
#if (NVARCH >= 0x010)
        if ((moCompDebugLevel > 0) && (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_NV12)) {
            if (overlaySurfaceBase == 0)
                overlaySurfaceBase = pSurfTarg_gbl->fpVidMem;

            dwVPPFlags &= ~VPP_BOB;
            dwVPPFlags &= ~VPP_EVEN;
            dwVPPFlags &= ~VPP_ODD;

            if (!VppDoFlip(&(pDriverData->vpp),
                                overlaySurfaceBase - pDriverData->BaseAddress,
                               pSurfTarg_gbl->lPitch,
                               pSurfTarg_gbl->wWidth,
                               pSurfTarg_gbl->wHeight,
                               pSurfTarg_gbl->ddpfSurface.dwFourCC,
                               dwVPPFlags)) {
#ifndef WINNT
                // check for VIA 82C598 north bridge
                if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
                    // restore write posting value
                    _asm {
                            mov edx, 0cf8h
                            mov eax, 80000840h
                            out dx, eax
                            mov edx, 0cfch
                            mov eax, dwSavedNBData
                            out dx, al
                    }

                }
#endif
                pfd->ddRVal = DDERR_WASSTILLDRAWING;
                dbgTracePop();
                return;
            }
        }
        else
#endif  // NVARCH >= 0x010
#endif  // DEBUG
        {
            if (!VppDoFlip(&(pDriverData->vpp),
                               pSurfTarg_gbl->fpVidMem - pDriverData->BaseAddress,
                               pSurfTarg_gbl->lPitch,
                               pSurfTarg_gbl->wWidth,
                               pSurfTarg_gbl->wHeight,
                               pSurfTarg_gbl->ddpfSurface.dwFourCC,
                               dwVPPFlags)) {
#ifndef WINNT
                // check for VIA 82C598 north bridge
                if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
                    // restore write posting value
                    _asm {
                            mov edx, 0cf8h
                            mov eax, 80000840h
                            out dx, eax
                            mov edx, 0cfch
                            mov eax, dwSavedNBData
                            out dx, al
                    }

                }
#endif
                pfd->ddRVal = DDERR_WASSTILLDRAWING;
                dbgTracePop();
                return;
            }
        }
    }

    getDC()->flipOverlayRecord.llPreviousFlipTime = getDC()->flipOverlayRecord.llFlipTime;
    QueryPerformanceCounter((LARGE_INTEGER *) &getDC()->flipOverlayRecord.llFlipTime);
    getDC()->flipOverlayRecord.fpPreviousFlipFrom = getDC()->flipOverlayRecord.fpFlipFrom;
    getDC()->flipOverlayRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
    getDC()->flipOverlayRecord.fpFlipTo = pSurfTarg_gbl->fpVidMem;

    getDC()->flipOverlayRecord.bFlipFlag = TRUE;

#ifndef WINNT
    // check for VIA 82C598 north bridge
    if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
        // restore write posting value
        _asm {
                mov edx, 0cf8h
                mov eax, 80000840h
                out dx, eax
                mov edx, 0cfch
                mov eax, dwSavedNBData
                out dx, al
        }
    }
#endif

    dbgTracePop();
}

//---------------------------------------------------------------------------

// Flip32
// Flip RGB or overlay surfaces.

#ifdef NV_PROFILE_DP2OPS
DWORD flags = 0;
#endif

DWORD __stdcall Flip32 (LPDDHAL_FLIPDATA pfd)
{
    dbgTracePush ("Flip32");

    DDSTARTTICK(SURF4_FLIP);
    DDFLIPTICKS(FEFLIP);
#ifdef CNT_TEX
    g_dwFrames++;
#endif
    nvSetDriverDataPtrFromDDGbl (pfd->lpDD);

#ifdef  STEREO_SUPPORT
    StereoFlipNotify(pfd);
#endif  //STEREO_SUPPORT

#ifdef NV_PROFILE_DP2OPS
    if (pDriverData->pBenchmark == NULL) {
        pDriverData->pBenchmark = new CBenchmark;
        pDriverData->pBenchmark->resetData();
    }
    else
        pDriverData->pBenchmark->outputData(flags);
#endif

    /*
     * NOTES:
     *
     * This callback is invoked whenever we are about to flip from
     * one surface to another.   pfd->lpSurfCurr is the surface we were at,
     * pfd->lpSurfTarg is the one we are flipping to.
     *
     * You should point the hardware registers at the new surface, and
     * also keep track of the surface that was flipped away from, so
     * that if the user tries to lock it, you can be sure that it is done
     * being displayed
     */


    DPF_LEVEL(NVDBG_LEVEL_FLIP,"Flip32: curr=%08x, targ=%08x", pfd->lpSurfCurr, pfd->lpSurfTarg );
    DPF_LEVEL(NVDBG_LEVEL_FLIP,"        vidmem ptrs change: %08x->%08x",
              pfd->lpSurfCurr->lpGbl->fpVidMem,
              pfd->lpSurfTarg->lpGbl->fpVidMem );

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_TWINVIEW_STATE_CHANGE) {
        if (pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
            VppDestroyFSMirror(&pDriverData->vpp);
            VppDestroyOverlay(&pDriverData->vpp);
            ResetTwinViewState(pDriverData);
            VppCreateOverlay(&pDriverData->vpp);
            VppCreateFSMirror(&pDriverData->vpp, pDriverData->vpp.dwOverlayFSSrcWidth, pDriverData->vpp.dwOverlayFSSrcHeight);
        } else {
            ResetTwinViewState(pDriverData);
        }
    }

    // front buffer rendering will fake flip
    if (dbgFrontRender)
    {
        DWORD dwTemp = pfd->lpSurfCurr->lpGbl->fpVidMem;
        NvReleaseSemaphore(pDriverData);
        pfd->lpSurfCurr->lpGbl->fpVidMem = pfd->lpSurfTarg->lpGbl->fpVidMem;
        pfd->lpSurfTarg->lpGbl->fpVidMem = dwTemp;
        pfd->ddRVal = DD_OK;
        DDENDTICK(SURF4_FLIP);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // figure out whether we should vsync
    BOOL bNoVSyncCapable, bNoVSync;
    bNoVSyncCapable = (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_0049_DAC |
                                                                 NVCLASS_0067_DAC |
                                                                 NVCLASS_007C_DAC));
    if (bNoVSyncCapable) {
        switch (getDC()->nvD3DRegistryData.regVSyncMode) {
            case D3D_REG_VSYNCMODE_PASSIVE:
                bNoVSync = (pfd->dwFlags & DDFLIP_NOVSYNC) ? TRUE : FALSE;
                break;
            case D3D_REG_VSYNCMODE_FORCEOFF:
                bNoVSync = TRUE;
                break;
            case D3D_REG_VSYNCMODE_FORCEON:
                bNoVSync = FALSE;
                break;
            default:
                DPF ("unknown VSYNC mode in Flip32");
                dbgD3DError();
                break;
        } // switch
    }
    else {
        bNoVSync = FALSE;
    }

    if (!(pfd->lpSurfTarg->ddsCaps.dwCaps & (DDSCAPS_OVERLAY | DDSCAPS_VIDEOPORT))) {
#if (NVARCH >= 0x010) && defined(HWFLIP)
        if ((bNoVSync)
            ||
            (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS))
#if (NVARCH >= 0x020)
            ||
            (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
#endif  // NVARCH >= 0x020
            )
        {
            nvCheckQueuedFlips ();
        }
#else  // !HWFLIP
        if (bNoVSync)
        {
            nvCheckQueuedFlips ();
        }
#endif  // !HWFLIP
    }

#if (NVARCH >= 0x010)
    // Handle AA: make sure that the regular buffer has updated before flipping
    if (getDC()->dwAAContextCount >= 1)
    {
        CNvObject *pObj = GET_PNVOBJ(pfd->lpSurfTarg);
        if (pObj) {
            PNVD3DCONTEXT pContext;
            if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                pContext = CKelvinAAState::GetContext(pObj->getSimpleSurface());
            } else {
                pContext = nvCelsiusAAFindContext(pObj->getSimpleSurface());
            }

            if (pContext) {
                pContext->aa.Flip(pContext);
                pContext->kelvinAA.Flip();
            }
        }
    }
#endif  // NVARCH >= 0x010

#ifdef  STEREO_SUPPORT
    StereoBackend(pfd);
#endif  //STEREO_SUPPORT

    // perform flip by either flipping at horz retrace or blt if that is not supported
    if ((bNoVSync) &&
        (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_5))
    {
        // do the blit
        nvFlipBlit (pfd);
        // done
        pfd->ddRVal = DD_OK;
        DDENDTICK(SURF4_FLIP);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // everything is OK, do the flip here
    pDriverData->fIsBltStyleFlip = FALSE;

    if (!(pfd->lpSurfTarg->ddsCaps.dwCaps & (DDSCAPS_OVERLAY | DDSCAPS_VIDEOPORT))) {
        nvFlipPrimary (pfd, bNoVSync);
    }

    else if (pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
        nvFlipOverlay (pfd);
    }

    else {  // Video port flips handled by FlipVideoPort32
        pfd->ddRVal = DDERR_INVALIDSURFACETYPE;
    }

/* DO NOT REMOVE - force flip to be slower than a given speed
    if (getDC()->dwD3DContextCount > 0)
    {
               double maxRefresh = 85.0; // set max refresh rate here
               double microWait  = (1e6/maxRefresh);
               double tick2micro = (1e6/500e6);
        static double last       = 0.0;

        if (last)
        {
            double now;

            do
            {
                __int64 i64;
                __asm
                {
                    _emit 0x0f
                    _emit 0x31
                    mov dword ptr [i64],eax
                    mov dword ptr [i64 + 4],edx
                }
                now = ((double)i64) * tick2micro;
            }
            while ((now - last) < microWait);
            last = now;
        }
        else
        {
            __int64 i64;
            __asm
            {
                _emit 0x0f
                _emit 0x31
                mov dword ptr [i64],eax
                mov dword ptr [i64 + 4],edx
            }
            last = ((double)i64) * tick2micro;
        }
    }
//*/
    dbgFlushType (NVDBG_FLUSH_2D);
    DDENDTICK(SURF4_FLIP);

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} // Flip32

