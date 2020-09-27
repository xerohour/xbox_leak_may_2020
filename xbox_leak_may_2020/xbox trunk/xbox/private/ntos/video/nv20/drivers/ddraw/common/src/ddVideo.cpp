/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1997, 1999, 2000 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       ddVideo.cpp
 *  Content:    NVidia Direct Video/Active Movie Direct Draw support
 *              32 bit DirectDraw part.
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#if 0
DWORD __stdcall HandleVideoPortOverlayUpdate(LPDDHAL_UPDATEOVERLAYDATA lpUOData);
#endif

extern BOOL ResetTwinViewState(GLOBALDATA *pDriverData);


// --------------------------------------------------------------------------------------
// TimerProc
//      Timer callback
void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    LPDDRAWI_DDRAWSURFACE_LCL lpSurfLcl = (LPDDRAWI_DDRAWSURFACE_LCL) pDriverData->vpp.dwOverlayLastVisibleSurfaceLCL;
    LPDDRAWI_DDRAWSURFACE_GBL lpSurfGbl;

    if (lpSurfLcl == NULL) {
        return;
    }

    if ((pDriverData->dwTVTunerFlipCount > 15) ||
        (pDriverData->regTVTunerHack == NV4_REG_VBMM_SINGLEOVL) ||
        (pDriverData->regTVTunerHack == NV4_REG_VBMM_FORCE)) {
        DWORD dwVPPFlags;

        lpSurfGbl = lpSurfLcl->lpGbl;
        if (lpSurfGbl == NULL) {
            return;
        }

        dwVPPFlags = VPP_ALL | VPP_WAIT;

        if (lpSurfLcl->lpSurfMore) {
            if (lpSurfLcl->lpSurfMore->dwOverlayFlags & DDOVER_INTERLEAVED) {
                dwVPPFlags |= VPP_INTERLEAVED | VPP_ODD;
            }
            if (lpSurfLcl->lpSurfMore->dwOverlayFlags & DDOVER_BOB) {
                dwVPPFlags |= VPP_BOB | VPP_ODD;
            }
        }
        if (lpSurfLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) {
            dwVPPFlags |= VPP_VIDEOPORT;
        }

        VppDoFlip(&(pDriverData->vpp),
                      lpSurfGbl->fpVidMem - pDriverData->BaseAddress,
                      lpSurfGbl->lPitch,
                      lpSurfGbl->wWidth,
                      lpSurfGbl->wHeight,
                      lpSurfGbl->ddpfSurface.dwFourCC,
                      dwVPPFlags);

        pDriverData->dwTVTunerFlipCount = 16;
    } else {
        pDriverData->dwTVTunerFlipCount++;
    }
}

// --------------------------------------------------------------------------------------
// TVTunerTimerHack
//      Hack used for bus mastering TV tuners which never call flip.  When detected,
//      install a timer to autoflip
#define TV_TUNER_HACK_FRAME_PERIOD 33

BOOL __stdcall TVTunerTimerHack()
{
#ifdef WINNT
    return FALSE;
#else   // Win9x
    if ((pDriverData->dwTVTunerTimer != NULL) ||
        (pDriverData->regTVTunerHack == NV4_REG_VBMM_NOHACK) ||
        (pDriverData->regTVTunerHack == NV4_REG_VBMM_DETECT && pDriverData->vpp.dwOverlaySurfaces != 1) ||
        (pDriverData->regTVTunerHack == NV4_REG_VBMM_SINGLEOVL && pDriverData->vpp.dwOverlaySurfaces != 1)){
        return FALSE;
    }

    pDriverData->dwTVTunerFlipCount = 0;
    pDriverData->dwTVTunerTimer = SetTimer(NULL, NULL, TV_TUNER_HACK_FRAME_PERIOD, TimerProc);
    return TRUE;
#endif  // WINNT
}


/*
 * UpdateOverlay32
 *
 *
 * NOTES:
 *
 * This callback is invoked to update an overlay surface.
 * This is where the src/destination is specified, any effects, and
 * it is shown or hidden
 *
 * For overlay surfaces NOT associated with a video port the surface will
 * be shown (DDOVER_SHOW) according to the following criteria.
 *
 *     DDOVER_INTERLEAVED   DDOVER_BOB      DISPLAYED
 *
 *             1                1           Even field
 *             1                0           Full surface (no special processing)
 *             0                X           Full surface (no special processing)
 *
 */

DWORD __stdcall
UpdateOverlay32( LPDDHAL_UPDATEOVERLAYDATA lpUOData )
{
    NvNotification              *pPioFlipOverlayNotifier;
    DWORD                       index = 0;
    DWORD                       match = 0;
#ifndef WINNT
    DWORD                       dwSavedNBData = 0;
#endif
    long                        srcDeltaX;
    long                        srcDeltaY;
    long                        dwSrcWidth;
    long                        dwSrcHeight;
    long                        dwDstWidth;
    long                        dwDstHeight;
    U032                        bMediaPortControl = FALSE;
    DWORD                       dwOverlayFormat = 0;
    DWORD                       dwVPPFlags;

    LPDDRAWI_DDRAWSURFACE_LCL   srcx = lpUOData->lpDDSrcSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   src = srcx->lpGbl;
    LPDDRAWI_DDRAWSURFACE_LCL   pLastVisibleSurfLcl;

    dbgTracePush ("UpdateOverlay32");

    nvSetDriverDataPtrFromDDGbl (lpUOData->lpDD);

    // set up pointer to DDHALINFO
    LPDDHALINFO pHalInfo = GET_HALINFO();

    pLastVisibleSurfLcl = (LPDDRAWI_DDRAWSURFACE_LCL) pDriverData->vpp.dwOverlayLastVisibleSurfaceLCL;

    pPioFlipOverlayNotifier = pDriverData->pPioFlipOverlayNotifierFlat;

    // subrectangle size may change, reset temporal filter.
    pDriverData->vpp.regOverlayMode &= ~NV4_REG_OVL_MODE_NOTFIRST;

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvDDrawRecover();
        } else {
            lpUOData->ddRVal = DDERR_SURFACEBUSY;
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_TWINVIEW_STATE_CHANGE) {
        VppDestroyFSMirror(&pDriverData->vpp);
        VppDestroyOverlay(&pDriverData->vpp);
        ResetTwinViewState(pDriverData);
        VppCreateOverlay(&pDriverData->vpp);
        VppCreateFSMirror(&pDriverData->vpp, pDriverData->vpp.dwOverlayFSSrcWidth, pDriverData->vpp.dwOverlayFSSrcHeight);
    }

#if 0
        // This code will eventually be used to specially opt out of overlay stuff when video port is in use
    if( srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)  {

        lpUOData->ddRVal = HandleVideoPortOverlayUpdate( lpUOData);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;

    }
#endif //

#ifdef NVPE
    // VPE takes care of overlay !
    if ((srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) || (pDriverData->bRing0FlippingFlag))
    {
        bMediaPortControl = TRUE;
    }

    else
        bMediaPortControl = FALSE;

    // Workaround for 640x480 Panel overlay flip notifier problem.
    // If mode is 480 lines and overlay ends at line 480, reduce it to line 479.
    if ((GET_MODE_HEIGHT() == 480)      &&
        (lpUOData->rDest.bottom == 480) &&
        (lpUOData->rDest.top < 477)     &&
        (pDriverData->vpp.dwNVOverlayFlags & OVERLAY_FLAG_ON_PANEL)) {

        lpUOData->rDest.bottom = 479;
    }

#endif // NVPE

    if ((src->ddpfSurface.dwFourCC != FOURCC_UYVY) &&
        (src->ddpfSurface.dwFourCC != FOURCC_UYNV) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YUY2) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YUNV) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YV12) &&
        (src->ddpfSurface.dwFourCC != FOURCC_NV12) &&
        (src->ddpfSurface.dwFourCC != FOURCC_NVMC) &&
        (src->ddpfSurface.dwFourCC != FOURCC_NVDS) &&
        (src->ddpfSurface.dwFourCC != FOURCC_420i) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IF09) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YVU9) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IV32) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IV31)) {
            NvReleaseSemaphore(pDriverData);
            lpUOData->ddRVal = DDERR_INVALIDSURFACETYPE;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
    }

    if (!(srcx->ddsCaps.dwCaps & DDSCAPS_OVERLAY)) {
            NvReleaseSemaphore(pDriverData);
            lpUOData->ddRVal = DDERR_NOTAOVERLAYSURFACE;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
    }

    while ((index < NV_VPP_MAX_OVERLAY_SURFACES) && (!match))
        if ((DWORD)srcx == pDriverData->vpp.dwOverlaySurfaceLCL[index++])
            match = 1;

    if (!match) {
        // Do some DirectShow compatibility stuff
        if ((src->ddpfSurface.dwFourCC == FOURCC_NVMC) || (src->ddpfSurface.dwFourCC == FOURCC_NVDS)) {
#if (NVARCH >= 0x10)
            BYTE dstSurfaceIndex;
            LPNVMCFRAMEDATA lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(src->fpVidMem);
            dstSurfaceIndex = lpNvMCFrameData->bMCDestinationSurfaceIndex & 0x0F;
            if (pDriverData->dwMCNVMCSurface != 0) {
                if (dstSurfaceIndex >= 8)
                    dstSurfaceIndex = 0;
                srcx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNV12Surface[dstSurfaceIndex];
                if (srcx != NULL)
                    src = srcx->lpGbl;
            } else
                pLastVisibleSurfLcl = NULL;
#endif  // NVARCH >= 0x10
        } else {
            dbgError("VPP: Could not locate overlay in private list of surfaces");
            NvReleaseSemaphore(pDriverData);
            lpUOData->ddRVal = DDERR_OUTOFCAPS;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    getDC()->nvPusher.waitForOtherChannels();

    if ((getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) ||   // Mode switches might occur without warning
        (pDriverData->vpp.dwOverlayFSOvlHead == 0xFFFFFFFF) ||                    // overlay surface has been destroyed
        (pDriverData->vpp.dwOverlayFSOvlLost)) {                                  // not overlay capable any more

        nvDDrawRecover();
        lpUOData->ddRVal = DDERR_SURFACELOST;
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    pDriverData->dwRingZeroMutex = TRUE;

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

//#ifndef WINNT
#if (!defined(WINNT) && !defined(NVPE))
    // check and possibly allocate overlay events for Ring0 functioning
    // note these will only be actually CALLED if write_then_awaken is used
    // so we're safe since ONLY ring0 should be calling that... hopefully heh heh...
    if(!pDriverData->vpp.dwOverlayEventsAllocated ){
      if(( ((vpSurfaces*)(pDriverData->dwMySurfacesPtr))->pNotifyOverlay0NotifyProc != 0 ) &&
           (((vpSurfaces*)(pDriverData->dwMySurfacesPtr))->pNotifyOverlay1NotifyProc != 0 )  ){
        U032 status = 0;
        status = NvRmAllocEvent (   pDriverData->dwRootHandle,
                              NV_VIDEO_OVERLAY,
                              MY_OVERLAY0_BUFFER_EVENT,
                              NV01_EVENT_KERNEL_CALLBACK,
                              NV07A_NOTIFIERS_SET_OVERLAY(0),
                              (void*)(((vpSurfaces*)(pDriverData->dwMySurfacesPtr))->pNotifyOverlay0NotifyProc));
        if( status !=  0x0 /* meaning sucess */ ) {
          DPF("Bad status returned from nvRmAllocEvent. status = %d",status);
        }
        status = NvRmAllocEvent (   pDriverData->dwRootHandle,
                              NV_VIDEO_OVERLAY,
                              MY_OVERLAY1_BUFFER_EVENT,
                              NV01_EVENT_KERNEL_CALLBACK,
                              NV07A_NOTIFIERS_SET_OVERLAY(1),
                              (void*)(((vpSurfaces*)(pDriverData->dwMySurfacesPtr))->pNotifyOverlay1NotifyProc));
        if( status !=  0x0 /* meaning sucess */ ) {
          DPF("Bad status returned from nvRmAllocEvent. status = %d",status);
        }
        pDriverData->vpp.dwOverlayEventsAllocated = TRUE;
      }
    }
#endif  // (!defined(WINNT) && !defined(NVPE))

    index = pDriverData->vpp.overlayBufferIndex;

    dwSrcWidth  = lpUOData->rSrc.right - lpUOData->rSrc.left;
    dwSrcHeight = lpUOData->rSrc.bottom - lpUOData->rSrc.top;
    dwDstWidth  = lpUOData->rDest.right - lpUOData->rDest.left;
    dwDstHeight = lpUOData->rDest.bottom - lpUOData->rDest.top;

    // if hide/show state, field state, size, and colour key state are the same, then we can do a
    // SetOverlayPosition instead
    if (pDriverData->vpp.dwOverlayFlipCount > 2 &&
        pLastVisibleSurfLcl != NULL &&
        pLastVisibleSurfLcl->lpGbl != NULL &&
        src->fpVidMem == pDriverData->vpp.dwOverlayLastVisibleFpVidMem &&
        (DWORD) src->lPitch == pDriverData->vpp.dwOverlayLastVisiblePitch &&
        src->ddpfSurface.dwFourCC == pDriverData->vpp.dwOverlayLastVisiblePixelFormat &&
        lpUOData->dwFlags == pDriverData->vpp.dwOverlayCachedFlags &&
        lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue == pDriverData->vpp.dwOverlayColorKey &&
        (DWORD)dwSrcWidth == pDriverData->vpp.dwOverlaySrcWidth &&
        (DWORD)dwSrcHeight == pDriverData->vpp.dwOverlaySrcHeight &&
        (DWORD)dwDstWidth == pDriverData->vpp.dwOverlayDstWidth &&

        (DWORD)dwDstHeight == pDriverData->vpp.dwOverlayDstHeight &&
#ifndef NVPE
        !(srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)) {
#else
        !(bMediaPortControl)) {
#endif // !NVPE
        pDriverData->dwRingZeroMutex = TRUE;
        pDriverData->vpp.dwOverlayDstX = (short)lpUOData->rDest.left;
        pDriverData->vpp.dwOverlayDstY = (short)lpUOData->rDest.top;

        // Direct Access to a VPP object - should not! -@mjl@
        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, NV_VPP_OVERLAY_IID);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_SET_OVERLAY_POINT_OUT_A | 0x40000);
        nvPushData(3, asmMergeCoords(lpUOData->rDest.left, lpUOData->rDest.top));  // SetOverlayPointOutA
        nvPusherAdjust(4);
        nvPusherStart(TRUE);

        pDriverData->dwRingZeroMutex = FALSE;
        pDriverData->dDrawSpareSubchannelObject = 0;
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

        NvReleaseSemaphore(pDriverData);
        lpUOData->ddRVal = DD_OK;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    } else {
        // reset the overlay flip count
        // both overlay buffers must be set with correct paramters before using the setoverlyposition optimization
        pDriverData->vpp.dwOverlayFlipCount = 0;
    }

    pDriverData->vpp.dwOverlayCachedFlags = lpUOData->dwFlags;

    if ((lpUOData->dwFlags & DDOVER_HIDE ) || dwDstWidth <= 1 || dwDstHeight <= 1) {
        LPPROCESSINFO lpProcInfo = pmGetProcess(pDriverData, GetCurrentProcessId());

        DPF("UPDATE OVERLAY - DDOVER_HIDE");

#ifdef WINNT
        // WIN2K HACK: DX7 under win2k presents incorrect flags, so manually fix them
        if (pLastVisibleSurfLcl) {
            pLastVisibleSurfLcl->ddsCaps.dwCaps &= ~(DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER);
            pLastVisibleSurfLcl->ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER;
        }
#endif  // WINNT
        pDriverData->vpp.dwOverlayLastVisibleSurfaceLCL = 0;

        pDriverData->vpp.dwOverlaySrcWidth = 0;
        pDriverData->vpp.dwOverlayDstWidth = 0;
        pDriverData->vpp.dwOverlaySrcHeight = 0;
        pDriverData->vpp.dwOverlayDstHeight = 0;

#ifdef NVPE

        // H.AZAR: (10/20/99) tell VPE kernel to stop autoflip !
        if (bMediaPortControl)
            NVPUpdateOverlay(pDriverData, lpUOData->lpDD);
        else
        {
            Sleep(VPP_TIMEOUT_TIME * 3);

            // Direct Access to VPP object! -@mjl@
            nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
            nvPushData(1, NV_VPP_OVERLAY_IID);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_STOP_OVERLAY(0) | 0x80000);
            nvPushData(3, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[0]
            nvPushData(4, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[1]
            nvPusherAdjust(5);
            nvPusherStart(TRUE);

            pPioFlipOverlayNotifier[1].status = 0;      /* clear notifiers */
            pPioFlipOverlayNotifier[2].status = 0;
        }
#else
        Sleep(VPP_TIMEOUT_TIME * 3);

        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, NV_DD_DMA_OVERLAY);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_STOP_OVERLAY(0) | 0x80000);
        nvPushData(3, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[0]
        nvPushData(4, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[1]
        nvPusherAdjust(5);
        nvPusherStart(TRUE);

        pPioFlipOverlayNotifier[1].status = 0;      /* clear notifiers */
        pPioFlipOverlayNotifier[2].status = 0;

#endif  // #ifdef NVPE

          //pDriverData->dwRingZeroMutex = FALSE;

        pDriverData->dDrawSpareSubchannelObject = 0;
        pDriverData->vpp.dwOverlayFormat = 0;     // This should hide things, since we'll stop flipping

        /* Let DDraw know how many visible overlays are active */
        pHalInfo->ddCaps.dwCurrVisibleOverlays = 0;

    } else {

        DPF("UPDATE OVERLAY - DDOVER_SHOW OR MOVE/RESIZE ");
        DPF( "Source Rect = (%d,%d), (%d,%d)",
             lpUOData->rSrc.left, lpUOData->rSrc.top,
             lpUOData->rSrc.right, lpUOData->rSrc.bottom );

        DPF( "Dest Rect = (%d,%d), (%d,%d)",
             lpUOData->rDest.left, lpUOData->rDest.top,
             lpUOData->rDest.right, lpUOData->rDest.bottom );

#ifdef WINNT
        // WIN2K HACK: DX7 under win2k presents incorrect flags, so manually fix them
        if (pLastVisibleSurfLcl) {
            pLastVisibleSurfLcl->ddsCaps.dwCaps &= ~(DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER);
            pLastVisibleSurfLcl->ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER;
        }
        srcx->ddsCaps.dwCaps |= DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER;
        srcx->ddsCaps.dwCaps &= ~DDSCAPS_BACKBUFFER;
#endif  // WINNT
        pDriverData->vpp.dwOverlayLastVisibleSurfaceLCL = (DWORD) srcx;
        pDriverData->vpp.dwOverlayLastVisibleFpVidMem = src->fpVidMem;
        pDriverData->vpp.dwOverlayLastVisiblePitch = src->lPitch;
        pDriverData->vpp.dwOverlayLastVisiblePixelFormat = src->ddpfSurface.dwFourCC;

        if ((lpUOData->rDest.left < 0) || (lpUOData->rDest.top < 0)) {
            pDriverData->dwRingZeroMutex = FALSE;

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
            NvReleaseSemaphore(pDriverData);
            lpUOData->ddRVal = DDERR_INVALIDPOSITION;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }

        // For bus mastering TV tuners that never flip
        TVTunerTimerHack();

        /* Remember sizes so Flip32 knows what to do */
        pDriverData->vpp.dwOverlaySrcWidth = dwSrcWidth;
        pDriverData->vpp.dwOverlayDstWidth = dwDstWidth;
        pDriverData->vpp.dwOverlaySrcHeight = dwSrcHeight;
        pDriverData->vpp.dwOverlayDstHeight = dwDstHeight;


#ifndef NVPE
        // cheehoi. Need driver to flip (bMediaPortControl=TRUE) for nv4/5 when
        // media port is used. Only applies to nv4/5????
        if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10){
            if (srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) {
                //if (lpUOData->dwFlags & DDOVER_AUTOFLIP  ||
                //  pDriverData->vpp.dwOverlaySurfaces==1){ //autoflip or 1 ovl.surf.
                    bMediaPortControl = TRUE;
                //}
            }
        }else{
            //nv10. This was leftover from Chris. May not be needed.
            if (srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) {
                if (lpUOData->dwFlags & DDOVER_AUTOFLIP){
                    bMediaPortControl = TRUE;
                }
            }
        }

        if(pDriverData != 0) {
          if (pDriverData->bRing0FlippingFlag  )  {
            bMediaPortControl = TRUE;
          }
        }

#endif // !NVPE

        if (dwDstWidth <= 1) {
            srcDeltaX = 0x100000;
        } else {
            srcDeltaX = ((dwSrcWidth - 1) << 20) / (dwDstWidth - 1);
        }

        // Align to 2 pixel boundary + 1 for video scaler
//        dwDstWidth = (dwDstWidth + 2) & ~1L;
//        dwDstHeight = (dwDstHeight + 1) & ~1L;

        if (dwDstHeight <= 1) {
            srcDeltaY = 0x100000;
        } else {
            srcDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight - 1);
        }

        // Let the world know what the overlay src info is
        pDriverData->vpp.dwOverlaySrcX = (short)(lpUOData->rSrc.left & ~1);
        pDriverData->vpp.dwOverlaySrcY = (short)lpUOData->rSrc.top;

        // Do 1080i single field fixups if necessary
        if ((pDriverData->bMCOverlaySrcIsSingleField1080i) && (src->ddpfSurface.dwFourCC == FOURCC_NV12)) {
            if (pDriverData->bMCHorizontallyDownscale1080i) {
                dwSrcWidth = (dwSrcWidth * 10) / 18;
                srcDeltaX = ((dwSrcWidth - 1) << 20) / (dwDstWidth - 1);
                pDriverData->vpp.dwOverlaySrcX = (pDriverData->vpp.dwOverlaySrcX * 10) / 18;
            }
            dwSrcHeight >>= 1;
            srcDeltaY = ((dwSrcHeight - 1) << 20) / (dwDstHeight - 1);
            pDriverData->vpp.dwOverlaySrcHeight = dwSrcHeight;
        }

        pDriverData->vpp.dwOverlaySrcSize = asmMergeCoords(dwSrcWidth, dwSrcHeight);

        // Get the video mirror source dimensions
        if (pDriverData->vpp.dwOverlayFSSrcWidth < (DWORD) dwSrcWidth) {
            pDriverData->vpp.dwOverlayFSSrcWidth = dwSrcWidth;
        }
        if (pDriverData->vpp.dwOverlayFSSrcHeight < (DWORD) dwSrcHeight) {
            pDriverData->vpp.dwOverlayFSSrcHeight = dwSrcHeight;
        }
        if (pDriverData->vpp.dwOverlayFSSrcMinX > (DWORD) pDriverData->vpp.dwOverlaySrcX) {
            pDriverData->vpp.dwOverlayFSSrcMinX = pDriverData->vpp.dwOverlaySrcX;
        }
        if (pDriverData->vpp.dwOverlayFSSrcMinY > (DWORD) pDriverData->vpp.dwOverlaySrcY) {
            pDriverData->vpp.dwOverlayFSSrcMinY = pDriverData->vpp.dwOverlaySrcY;
        }

        /* Let the world know what the current overlay destination offset is */
        pDriverData->vpp.dwOverlayDstX = (short)lpUOData->rDest.left;
        pDriverData->vpp.dwOverlayDstY = (short)lpUOData->rDest.top;

        if (pDriverData->vpp.extraNumSurfaces == 0) {
            // VPP pipe is disable, only overlay scaling will be used
            if (dwSrcWidth > (dwDstWidth * (long)pDriverData->vpp.dwOverlayMaxDownScale)) {
                srcDeltaX = 0x100000 * pDriverData->vpp.dwOverlayMaxDownScale;
            }

            if (dwSrcHeight > (dwDstHeight * (long)pDriverData->vpp.dwOverlayMaxDownScale)) {
                srcDeltaY = 0x100000 * pDriverData->vpp.dwOverlayMaxDownScale;
            }
        } else {
            if (!bMediaPortControl)
            {
                if (pDriverData->vpp.dwOverlayMaxDownScaleX <= 1) {
                    // prescaler will be doing all the downscaling
                    if (dwSrcWidth > dwDstWidth) {
                        srcDeltaX = 0x100000;
                    }
                } else {
                    // predict what the cascading downscaler will produce for the overlay scale factor
                    long dwNewSrcWidth = dwSrcWidth;
                    while (dwNewSrcWidth > (dwDstWidth * (long)pDriverData->vpp.dwOverlayMaxDownScaleX)) {
                        dwNewSrcWidth /= 2;
                        srcDeltaX = ((dwNewSrcWidth - 1) << 20) / dwDstWidth;
                    }
                }
                if (pDriverData->vpp.dwOverlayMaxDownScaleY <= 1) {
                    // prescaler will be doing all the downscaling
                    if (dwSrcHeight > dwDstHeight) {
                        srcDeltaY = 0x100000;
                    }
                } else {
                    // predict what the cascading downscaler will produce for the overlay scale factor
                    long dwNewSrcHeight = dwSrcHeight;
                    while (dwNewSrcHeight > (dwDstHeight * (long)pDriverData->vpp.dwOverlayMaxDownScaleY)) {
                        dwNewSrcHeight /= 2;
                        srcDeltaY = ((dwNewSrcHeight - 1) << 20) / dwDstHeight;
                    }
                }
            }
            else
            {
                if (dwSrcWidth > (dwDstWidth * (long)pDriverData->vpp.dwOverlayMaxDownScale))
                {
                    srcDeltaX = 0x100000;
                }

                if (dwSrcHeight > (dwDstHeight * (long)pDriverData->vpp.dwOverlayMaxDownScale))
                {
                    srcDeltaY = 0x100000;
                }
            }

            if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
                // if HQVUp is enabled, the prescaler will do vertical upscale too
                if ((pDriverData->vpp.regOverlayMode & NV4_REG_OVL_MODE_VUPSCALE) ||
                    (lpUOData->dwFlags & (DDOVER_INTERLEAVED | DDOVER_BOB))) {
                    srcDeltaY = 0x100000;
                }
            }
        }



#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_0 (0x04000000)
#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1 (0x02000000)
#define NV_VFM_FORMAT_BOBFROMINTERLEAVE           (0x08000000)

        // color key?
        if (lpUOData->dwFlags & DDOVER_KEYDESTOVERRIDE) {
            DPF( "Dest ColorKey = %08x,%08x",
                lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue,
                lpUOData->overlayFX.dckDestColorkey.dwColorSpaceHighValue );

            // Save this for later reference
            pDriverData->vpp.dwOverlayColorKey =
                lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue;

            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_IMAGE << 20);

        } else if (lpUOData->dwFlags & DDOVER_KEYDEST) {
            DPF( "Dest ColorKey = %08x,%08x",
                lpUOData->lpDDDestSurface->ddckCKDestOverlay.dwColorSpaceLowValue,
                lpUOData->lpDDDestSurface->ddckCKDestOverlay.dwColorSpaceHighValue );

            // Save this for later reference
            pDriverData->vpp.dwOverlayColorKey =
                lpUOData->lpDDDestSurface->ddckCKDestOverlay.dwColorSpaceLowValue;

            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_IMAGE << 20);

        } else {    // disable color key
            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_OVERLAY << 20);
        }

/*
        if(!bMediaPortControl) {
          nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
          nvPushData(1, NV_DD_DMA_OVERLAY);
          nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_SET_OVERLAY_COLORKEY(index) | 0x40000);
          nvPushData(3, pDriverData->vpp.dwOverlayColorKey);                                // SetOverlayColorKey
          nvPushData(4, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_SET_OVERLAY_DU_DX(index) | 0x100000);
          nvPushData(5, srcDeltaX);                                                   // SetOverlayDuDx
          nvPushData(6, srcDeltaY);                                                   // SetOverlayDvDy
          nvPushData(7, asmMergeCoords(lpUOData->rDest.left, lpUOData->rDest.top));   // SetOverlayPointOut
          nvPushData(8, asmMergeCoords((dwDstWidth & ~1), dwDstHeight));              // SetOverlaySizeOut
          nvPusherAdjust(9);
          nvPusherStart(TRUE);
        }
*/
        if (lpUOData->dwFlags & DDOVER_AUTOFLIP)
            index = 0; // overide index for mediaport/overlay autoflip

        if ((src->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
            (src->ddpfSurface.dwFourCC == FOURCC_UYNV)) {

            // we need to know if we're bobbing
          if((srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)
              && (lpUOData->dwFlags & DDOVER_BOB)
              && (lpUOData->dwFlags & DDOVER_INTERLEAVED) ){
              pDriverData->vpp.dwOverlayMode =  NV_VFM_FORMAT_BOBFROMINTERLEAVE | NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1;
              DPF( "Video Port Overlay control - Interleaved - BOB");
          } else if((srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)
              && (lpUOData->dwFlags & DDOVER_BOB)
              ){
              pDriverData->vpp.dwOverlayMode =  NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1;
              DPF( "Video Port Overlay control - Separate - BOB");
          } else if((srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)
              ){
              pDriverData->vpp.dwOverlayMode =  NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1;
              DPF( "Video Port Overlay control - weave or odd or even");
          } else {
              pDriverData->vpp.dwOverlayMode = 0;

          }

          dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_COLOR_LE_YB8CR8YA8CB8 << 16);

        } else {
            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16);
        }

        dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_MATRIX_ITURBT601 << 24);
        // don't know the source pitch yet, let vppFlip update it
//        dwOverlayFormat |= dwSrcPitch;
        dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_NOTIFY_WRITE_ONLY << 31);

        // Let the world know what the overlay format info is
        pDriverData->vpp.dwOverlayFormat = dwOverlayFormat;

        pDriverData->vpp.dwOverlayDeltaX = (DWORD)srcDeltaX;
        pDriverData->vpp.dwOverlayDeltaY = (DWORD)srcDeltaY;

        // all the overlay source information is set in vppFlip
        // no way to convey odd or even field, assume odd if bobbed
        dwVPPFlags = VPP_STATIC | VPP_WAIT;
        if (lpUOData->dwFlags & DDOVER_BOB) {
            dwVPPFlags |= VPP_BOB | VPP_ODD;
        }
        if (lpUOData->dwFlags & DDOVER_INTERLEAVED) {
            dwVPPFlags |= VPP_INTERLEAVED | VPP_ODD;
        }
        if (bMediaPortControl) {
            dwVPPFlags |= VPP_VIDEOPORT;
            pDriverData->vpp.dwOverlaySrcPitch = src->lPitch;

        }  else {
#if (NVARCH >= 0x10)
            if (src->ddpfSurface.dwFourCC == FOURCC_NV12) {
                LPNVMCSURFACEFLAGS lpSurfaceFlags =
                    (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

                while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
                       (lpSurfaceFlags->dwMCSurfaceBase != src->fpVidMem))
                        lpSurfaceFlags++;

                // Make sure the surface has been format converted
                if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) &&
                    (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
                    nvMoCompConvertSurfaceFormat(src->fpVidMem, TRUE, 15);
                else if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD) &&
                    ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3))
                    nvMoCompConvertSurfaceFormat(src->fpVidMem, TRUE, 3);
                else if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) &&
                    (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 12))
                    nvMoCompConvertSurfaceFormat(src->fpVidMem, TRUE, 12);

                if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted == 15) &&
                    (lpSurfaceFlags->bMCFrameIsFiltered == FALSE) &&
                    (pDriverData->bMCTemporalFilterDisabled == FALSE) &&
                    IS_FIELD(dwVPPFlags))
                    nvMoCompTemporalFilter(src->fpVidMem, pDriverData->bMCPercentCurrentField);

                // Check if filtering is disabled on this surface for this display call only
                if (lpSurfaceFlags->bMCFrameType & TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE) {
                    lpSurfaceFlags->bMCFrameIsFiltered = FALSE;
                    lpSurfaceFlags->bMCFrameType &= ~TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE;
                }

                // Always display non-filtered portion of non-interleaved frames
                if (lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME)
                    lpSurfaceFlags->bMCFrameIsFiltered = FALSE;

            }
#endif  // (NVARCH >= 0x10)

#ifdef  DEBUG
#if (NVARCH >= 0x010)
            if ((moCompDebugLevel > 0) && (src->ddpfSurface.dwFourCC == FOURCC_NV12)) {
                if (overlaySurfaceBase == 0)
                    overlaySurfaceBase = src->fpVidMem;

                dwVPPFlags &= ~VPP_BOB;
                dwVPPFlags &= ~VPP_ODD;

                VppDoFlip(
                    &(pDriverData->vpp),
                    overlaySurfaceBase - pDriverData->BaseAddress,
                    src->lPitch,
                    src->wWidth,
                    src->wHeight,
                    src->ddpfSurface.dwFourCC,
                    dwVPPFlags);
            }
            else
#endif  // NVARCH >= 0x010
#endif  // DEBUG
            {
                VppDoFlip(&(pDriverData->vpp),
                              src->fpVidMem - pDriverData->BaseAddress,
                              src->lPitch,
                              src->wWidth,
                              src->wHeight,
                              src->ddpfSurface.dwFourCC,
                              dwVPPFlags);
            }
        }

        /* Let DDraw know how many visible overlays are active */
        if (pHalInfo->ddCaps.dwCurrVisibleOverlays == 0) {
            pHalInfo->ddCaps.dwCurrVisibleOverlays = 1;
        }
    }

    pDriverData->dwRingZeroMutex = FALSE;

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

#ifdef NVPE

    ///// H.AZAR: (10/20/1999): notify VPE kernel of overlay changes !
    if (bMediaPortControl)
        NVPUpdateOverlay(pDriverData, lpUOData->lpDD);
#endif  // #ifdef NVPE

    // We make this call here simply to ensure that the color control
    // values are reset to the values we're holding in pDriverData.
    //
    // There is an outstanding issue which causes zeros to be written
    // sometimes on a modeswitch.  By forcing this here, we update the
    // value very near the 'end' of the overlay rebuild process (just before
    // show) so we're likely to overwrite the zeros.
    //
    // The proper fix of course is to determine what part of the system is
    // writing the zeros.  Refer to BUG #20000807-184148
    //
    if (!bMediaPortControl) {
    VppSetOverlayColourControl(&(pDriverData->vpp));
    }

    NvReleaseSemaphore(pDriverData);
    lpUOData->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;

} /* UpdateOverlay32 */


/*
 * SetOverlayPosition32
 */
DWORD __stdcall
SetOverlayPosition32( LPDDHAL_SETOVERLAYPOSITIONDATA lpSOPData )
{
    NvNotification              *pPioFlipOverlayNotifier;
    DWORD                       index = 0;
    DWORD                       match = 0;
    LPDDRAWI_DDRAWSURFACE_LCL   srcx = lpSOPData->lpDDSrcSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   src = srcx->lpGbl;
    BOOL                        bMediaPortControl = FALSE;

    dbgTracePush ("SetOverlayPosition32");

    nvSetDriverDataPtrFromDDGbl (lpSOPData->lpDD);

    pPioFlipOverlayNotifier = pDriverData->pPioFlipOverlayNotifierFlat;

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_TWINVIEW_STATE_CHANGE) {
        VppDestroyFSMirror(&pDriverData->vpp);
        VppDestroyOverlay(&pDriverData->vpp);
        ResetTwinViewState(pDriverData);
        VppCreateOverlay(&pDriverData->vpp);
        VppCreateFSMirror(&pDriverData->vpp, pDriverData->vpp.dwOverlayFSSrcWidth, pDriverData->vpp.dwOverlayFSSrcHeight);
    }

    if (!(srcx->ddsCaps.dwCaps & DDSCAPS_OVERLAY)) {
            NvReleaseSemaphore(pDriverData);
            lpSOPData->ddRVal = DDERR_NOTAOVERLAYSURFACE;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
    }

    if ((lpSOPData->lXPos < 0) || (lpSOPData->lYPos < 0)) {
            NvReleaseSemaphore(pDriverData);
            lpSOPData->ddRVal = DDERR_INVALIDPOSITION;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
    }

    while ((index < NV_VPP_MAX_OVERLAY_SURFACES) && (!match))
        if ((DWORD)srcx == pDriverData->vpp.dwOverlaySurfaceLCL[index++])
            match = 1;

    if (!match) {
        // Do some DirectShow compatibility stuff
        if ((src->ddpfSurface.dwFourCC == FOURCC_NVMC) || (src->ddpfSurface.dwFourCC == FOURCC_NVDS)) {
#if (NVARCH >= 0x10)
            LPNVMCFRAMEDATA lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(src->fpVidMem);
            DWORD dwIndex = lpNvMCFrameData->bMCDestinationSurfaceIndex;
            nvAssert (dwIndex < 8);
            srcx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNV12Surface[dwIndex];
            nvAssert (srcx);
            src = srcx->lpGbl;
#endif  // NVARCH >= 0x10
        } else {
            NvReleaseSemaphore(pDriverData);
            lpSOPData->ddRVal = DDERR_OUTOFCAPS;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

#if 0
    // This code will eventually be used to specially opt out of overlay stuff when video port is in use
    if( srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)  {

        lpUOData->ddRVal = HandleVideoPortOverlayPosition( lpSOPData);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;

    }
#endif //

    if (srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)  {
        bMediaPortControl = TRUE;
    }

    if (pDriverData != 0) {
        if (pDriverData->bRing0FlippingFlag)  {
            bMediaPortControl = TRUE;
        }
    }

    getDC()->nvPusher.waitForOtherChannels();

    if ((getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) ||     /* Mode switches might occur without warning */
        pDriverData->vpp.dwOverlayFSOvlLost) {

        NvReleaseSemaphore(pDriverData);
        lpSOPData->ddRVal = DDERR_SURFACELOST;
        dbgTracePop();
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * NOTES:
     * This callback is invoked to set an overlay position
     */
    DPF( "SET OVERLAY POSITION" );
    DPF( "Overlay surface = %08x", lpSOPData->lpDDSrcSurface );
    DPF( "(%d,%d)", lpSOPData->lXPos, lpSOPData->lYPos );

    pDriverData->dwRingZeroMutex = TRUE;

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvDDrawRecover();
        } else {
            lpSOPData->ddRVal = DDERR_SURFACEBUSY;
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /* Let the world know what the current overlay destination offset is */
    pDriverData->vpp.dwOverlayDstX = (short)lpSOPData->lXPos;
    pDriverData->vpp.dwOverlayDstY = (short)lpSOPData->lYPos;

    // Direct Access to VPP object! -@mjl@
    nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
    nvPushData(1, NV_VPP_OVERLAY_IID);
    nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_SET_OVERLAY_POINT_OUT_A | 0x40000);
    nvPushData(3, asmMergeCoords(lpSOPData->lXPos, lpSOPData->lYPos));  // SetOverlayPointOutA
    nvPusherAdjust(4);
    nvPusherStart(TRUE);

    pDriverData->dwRingZeroMutex = FALSE;

    pDriverData->dDrawSpareSubchannelObject = 0;

    NvReleaseSemaphore(pDriverData);
    lpSOPData->ddRVal = DD_OK;
    dbgTracePop();
    return DDHAL_DRIVER_HANDLED;

} /* SetOverlayPosition32 */


//---------------------------------------------------------------------------
#endif  // NVARCH >= 0x04
