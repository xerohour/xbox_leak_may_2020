// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// **************************************************************************
//  Content:    DirectDraw HAL surface management
//
//  History:
//        schaefer           7/6/99
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#include "nvOverlaySurf.h"  // For creation of FOURCC and overlay surfaces
#ifdef VPEFSMIRROR
#include "nvpkrnl.h" // for VPE FS Mirror
#endif

#ifdef WINNT
#include "nvMultiMon.h"
#endif

#ifdef WINNT
extern "C"
{
    extern void __cdecl NV_OglDDrawSurfaceCreated(PDEV *ppdev);
    extern void __cdecl NV_OglDDrawSurfaceDestroyed(PDEV *ppdev);
}
#define NV_OGLDDRAWSURFACECREATED(PDEV) NV_OglDDrawSurfaceCreated((PDEV))
#define NV_OGLDDRAWSURFACEDESTROYED(PDEV) NV_OglDDrawSurfaceDestroyed((PDEV))
#else
#define NV_OGLDDRAWSURFACECREATED(PDEV)
#define NV_OGLDDRAWSURFACEDESTROYED(PDEV)
#endif

#ifdef DEBUG_SURFACE_PLACEMENT
extern CSurfaceDebug csd;
#endif

// getDisplayDuration
//
// get the length (in ticks) of a refresh cycle
void getDisplayDuration( void )
{
#ifdef WINNT
    DWORD dwHead;
    NV_CFGEX_GET_REFRESH_INFO_PARAMS refreshInfo;
    __int64 timeFrequency;

    dbgTracePush ("getDisplayDuration");

    // for the most part inactive for win2k
    getDC()->flipPrimaryRecord.fpFlipFrom = 0xDEADBEEF;

    // we need this stuff for the back door CRTC synchronized blits

    // determine which head is the default
    dwHead = GET_CURRENT_HEAD();

    // get and save CRTC params
    getDC()->flipPrimaryRecord.nvTimingInfo[dwHead].Head = dwHead;
    getDC()->flipPrimaryRecord.nvTimingInfo[dwHead].Reg = NV_CFGEX_CRTC_TIMING_REGISTER;
    if (NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                        NV_CFGEX_CRTC_TIMING_MULTI,
                        &getDC()->flipPrimaryRecord.nvTimingInfo[dwHead],
                        sizeof(NV_CFGEX_CRTC_TIMING_PARAMS)) != NVOS_CGE_STATUS_SUCCESS) {
        // error, this structure is invalid
        getDC()->flipPrimaryRecord.nvTimingInfo[dwHead].Head = 0xFFFFFFFF;
    }

    // get refresh rate
    refreshInfo.Head = dwHead;
    refreshInfo.ulState = 0;
    if (NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                        NV_CFGEX_GET_REFRESH_INFO,
                        &refreshInfo,
                        sizeof(NV_CFGEX_GET_REFRESH_INFO_PARAMS)) != NVOS_CGE_STATUS_SUCCESS) {
        // error can find refresh rate
        getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead] = 0;
    } else {
        if (refreshInfo.ulRefreshRate) {
            getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead] = (LONGLONG)1000000000 / (LONGLONG)refreshInfo.ulRefreshRate;
        } else {
            getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead] = 0;
        }
    }

    // calculate flip duration in units of ticks
    EngQueryPerformanceFrequency(&timeFrequency);
    getDC()->flipPrimaryRecord.dwFlipDuration = (unsigned long)(timeFrequency * getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead] / 1000000000);

    // get and save CRTC params for the other head
    if ((ppdev->ulDesktopMode & NV_TWO_DACS) &&
       ((ppdev->ulDesktopMode & NV_CLONE_DESKTOP) ||
        (ppdev->ulDesktopMode & NV_SPANNING_MODE))) {
        // we are in a TwinView mode, get the CRTC params for the other head too
        dwHead ^= 1;
        getDC()->flipPrimaryRecord.nvTimingInfo[dwHead].Head = dwHead;
        getDC()->flipPrimaryRecord.nvTimingInfo[dwHead].Reg = NV_CFGEX_CRTC_TIMING_REGISTER;
        if (NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                            NV_CFGEX_CRTC_TIMING_MULTI,
                            &getDC()->flipPrimaryRecord.nvTimingInfo[dwHead],
                            sizeof(NV_CFGEX_CRTC_TIMING_PARAMS)) != NVOS_CGE_STATUS_SUCCESS) {
            // error, this structure is invalid
            getDC()->flipPrimaryRecord.nvTimingInfo[dwHead].Head = 0xFFFFFFFF;
        }

        // get refresh rate
        refreshInfo.Head = dwHead;
        refreshInfo.ulState = 0;
        if (NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                            NV_CFGEX_GET_REFRESH_INFO,
                            &refreshInfo,
                            sizeof(NV_CFGEX_GET_REFRESH_INFO_PARAMS)) != NVOS_CGE_STATUS_SUCCESS) {
            // error can find refresh rate
            getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead] = 0;
        } else {
            if (refreshInfo.ulRefreshRate) {
                getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead] = (LONGLONG)1000000000 / (LONGLONG)refreshInfo.ulRefreshRate;
            } else {
                getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead] = 0;
            }
        }

    } else {
        getDC()->flipPrimaryRecord.nvTimingInfo[dwHead ^ 1].Head = 0xFFFFFFFF;
        getDC()->flipPrimaryRecord.llFlipDurationNs[dwHead ^ 1] = 0;
    }
#else
#define IS_IN_VBLANK(a) ((a)==0 || (a)>=GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head))
    int         i;
    __int64     start, end;
    DWORD       oldclass;
    HANDLE      hprocess;
    NV_CFGEX_CURRENT_SCANLINE_MULTI_PARAMS cfgParams;
    BOOL timeOut;

    dbgTracePush ("getDisplayDuration");

    memset( &getDC()->flipPrimaryRecord, 0, sizeof( getDC()->flipPrimaryRecord ) );
    memset( &getDC()->flipOverlayRecord, 0, sizeof( getDC()->flipOverlayRecord ) );

    NVTWINVIEWDATA twinData;
    LONGLONG timeFrequency;
    // RM needs physical head, map logical to physical
    twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
    twinData.dwAction = NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD;
    twinData.dwCrtc   = pDXShare->dwHeadNumber;
    MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);
    cfgParams.Head    = twinData.dwCrtc;
    // get start time for timeouts (display may be disabled)
    QueryPerformanceFrequency((LARGE_INTEGER *)&timeFrequency);

    // get and save CRTC params
    getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head].Head = cfgParams.Head;
    getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head].Reg = NV_CFGEX_CRTC_TIMING_REGISTER;
    if (NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                        NV_CFGEX_CRTC_TIMING_MULTI,
                        &getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head],
                        sizeof(NV_CFGEX_CRTC_TIMING_PARAMS)) != NVOS_CGE_STATUS_SUCCESS) {
        // error, this structure is invalid
        getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head].Head = 0xFFFFFFFF;
    }

    hprocess = GetCurrentProcess();
    oldclass = GetPriorityClass( hprocess );
//    SetPriorityClass( hprocess, REALTIME_PRIORITY_CLASS ); Can't do this due to Electronic Arts bug in Triple Play 97
    SetPriorityClass( hprocess, HIGH_PRIORITY_CLASS );

    NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                     NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
    timeOut = FALSE;
    QueryPerformanceCounter((LARGE_INTEGER*)&start);
    while (!IS_IN_VBLANK(cfgParams.RasterPosition) && !timeOut) {
        NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                         NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
        LONGLONG timeNow;
        QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
        if ((timeNow - start) >= (timeFrequency / 20)) timeOut = TRUE;  // timout of 1/20th of a second
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&start);
    #define LOOP_CNT    20
    for ( i=0;i<LOOP_CNT;i++ ) {
        NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                         NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
        while (IS_IN_VBLANK(cfgParams.RasterPosition) && !timeOut) {
            NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                             NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
            LONGLONG timeNow;
            QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
            if ((timeNow - start) >= timeFrequency) timeOut = TRUE;  // timout of 1/20th of a second
        }
        while (!IS_IN_VBLANK(cfgParams.RasterPosition) && !timeOut) {
            NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                             NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
            LONGLONG timeNow;
            QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
            if ((timeNow - start) >= timeFrequency) timeOut = TRUE;  // timout of 1/20th of a second
        }
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&end);

    SetPriorityClass( hprocess, oldclass );
    getDC()->flipPrimaryRecord.dwFlipDuration = ((DWORD)(end-start) + LOOP_CNT/2)/LOOP_CNT;

    GET_HALINFO()->dwMonitorFrequency = (DWORD)start / getDC()->flipPrimaryRecord.dwFlipDuration;

    getDC()->flipPrimaryRecord.llFlipDurationNs[cfgParams.Head] = (LONGLONG)getDC()->flipPrimaryRecord.dwFlipDuration * 1000000000 / timeFrequency;

    // get and save CRTC params for the other head
    if (pDriverData->dwHeads > 1 &&
       (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE ||
        pDriverData->dwDesktopState == NVTWINVIEW_STATE_DUALVIEW)) {
        // we are in a TwinView mode, get the CRTC params for the other head too
        cfgParams.Head ^= 1;
        getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head].Head = cfgParams.Head;
        getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head].Reg = NV_CFGEX_CRTC_TIMING_REGISTER;
        if (NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                            NV_CFGEX_CRTC_TIMING_MULTI,
                            &getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head],
                            sizeof(NV_CFGEX_CRTC_TIMING_PARAMS)) != NVOS_CGE_STATUS_SUCCESS) {
            // error, this structure is invalid
            getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head].Head = 0xFFFFFFFF;
        }

        // need to do all the timing for the other head
        SetPriorityClass( hprocess, HIGH_PRIORITY_CLASS );

        NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                         NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
        timeOut = FALSE;
        QueryPerformanceCounter((LARGE_INTEGER*)&start);
        while (!IS_IN_VBLANK(cfgParams.RasterPosition) && !timeOut) {
            NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                             NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
            LONGLONG timeNow;
            QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
            if ((timeNow - start) >= (timeFrequency / 20)) timeOut = TRUE;  // timout of 1/20th of a second
        }

        QueryPerformanceCounter((LARGE_INTEGER*)&start);
        for ( i=0;i<LOOP_CNT;i++ ) {
            NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                             NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
            while (IS_IN_VBLANK(cfgParams.RasterPosition) && !timeOut) {
                NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                                 NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
                LONGLONG timeNow;
                QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
                if ((timeNow - start) >= timeFrequency) timeOut = TRUE;  // timout of 1/20th of a second
            }
            while (!IS_IN_VBLANK(cfgParams.RasterPosition) && !timeOut) {
                NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                                 NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));
                LONGLONG timeNow;
                QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
                if ((timeNow - start) >= timeFrequency) timeOut = TRUE;  // timout of 1/20th of a second
            }
        }
        QueryPerformanceCounter((LARGE_INTEGER*)&end);

        SetPriorityClass( hprocess, oldclass );
        getDC()->flipPrimaryRecord.llFlipDurationNs[cfgParams.Head] = (LONGLONG)(((DWORD)(end-start) + LOOP_CNT/2)/LOOP_CNT) * 1000000000 / timeFrequency;

    } else {
        getDC()->flipPrimaryRecord.nvTimingInfo[cfgParams.Head ^ 1].Head = 0xFFFFFFFF;
        getDC()->flipPrimaryRecord.llFlipDurationNs[cfgParams.Head ^ 1] = 0;
    }

#ifdef DEBUG
    QueryPerformanceFrequency((LARGE_INTEGER*)&start);

    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Duration = %d ticks, %dms, %dHz",
         getDC()->flipPrimaryRecord.dwFlipDuration,
         getDC()->flipPrimaryRecord.dwFlipDuration * 1000 / (DWORD)start,
         (DWORD)start / getDC()->flipPrimaryRecord.dwFlipDuration);
#endif // DEBUG

#endif // !WINNT

    dbgTracePop();

} // getDisplayDuration

//---------------------------------------------------------------------------
//
//  CreateSurface helper routines
//
//---------------------------------------------------------------------------

BOOL nvCalculateColorKey (DWORD *pColorKey, LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl, DWORD dwFormat)
{
    dbgTracePush ("nvCalculateColorKey");

    if (pDDSLcl->dwFlags & DDRAWISURF_HASCKEYSRCBLT)
    {
        switch (dwFormat)
        {
            case NV_SURFACE_FORMAT_Y8:
            case NV_SURFACE_FORMAT_A1R5G5B5:
            case NV_SURFACE_FORMAT_A4R4G4B4:
            case NV_SURFACE_FORMAT_R5G6B5:
                // Must assume the application does the right thing here and passes
                // in the exact color key value as is in the texture map. If the alpha
                // bits are masked here, WHQL will fail.
                *pColorKey = pDDSLcl->ddckCKSrcBlt.dwColorSpaceHighValue & 0x0000FFFF;
                break;
            case NV_SURFACE_FORMAT_X1R5G5B5:
            case NV_SURFACE_FORMAT_DXT1_A1R5G5B5:
                *pColorKey = pDDSLcl->ddckCKSrcBlt.dwColorSpaceHighValue & 0x00007FFF;
                break;
            case NV_SURFACE_FORMAT_A8R8G8B8:
            case NV_SURFACE_FORMAT_X8R8G8B8:
            case NV_SURFACE_FORMAT_DXT23_A8R8G8B8:
            case NV_SURFACE_FORMAT_DXT45_A8R8G8B8:
                *pColorKey = pDDSLcl->ddckCKSrcBlt.dwColorSpaceHighValue;
                break;
            case NV_SURFACE_FORMAT_I8_A8R8G8B8:
                if ((GET_PNVOBJ(pDDSLcl))
                 && (GET_PNVOBJ(pDDSLcl)->getTexture())
                 && (GET_PNVOBJ(pDDSLcl)->getTexture()->hasAutoPalette())
                 && (GET_PNVOBJ(pDDSLcl)->getTexture()->getOriginalFormat() == NV_SURFACE_FORMAT_X8R8G8B8))
                {
                    *pColorKey = pDDSLcl->ddckCKSrcBlt.dwColorSpaceHighValue | 0xFF000000; // fix colour-key alpha
                } else {
                    *pColorKey = pDDSLcl->ddckCKSrcBlt.dwColorSpaceHighValue;
                }
                break;
            default:
                DPF ("unhandled texture format %d in nvCalculateColorKey", dwFormat);
                nvAssert(0);
                dbgTracePop();
                return FALSE;
                break;
        }  // switch

        dbgTracePop();
        return TRUE;
    }

    dbgTracePop();
    return FALSE;
}

//---------------------------------------------------------------------------

DWORD nvCalculateZBufferParams (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl,
                                DWORD *pFormat, DWORD *pWidth, DWORD *pHeight, DWORD *pMultiSampleBits,
                                DWORD *pBPPRequested, DWORD *pBPPGranted, DWORD *pPitch)
{
    DWORD dwRenderDepth, dwZDepth;

    dbgTracePush ("nvCalculateZBufferParams");

#ifndef WINNT
    // on WIN2K this flag is undefined at this point
    nvAssert (pDDSLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT);
#endif

    // i contend that this will always be set. please let me know
    // if this assertion ever fails... CRD
    // (CRD note to self: if no failure, consolidate with other calc routine,
    // add param to (dis)allow BPP munging. FALSE for sys, true for vid)
    nvAssert (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_ZBUFFER);

    dwRenderDepth = GET_MODE_BPP();

    if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_ZBUFFER) {
        dwZDepth = pDDSLcl->lpGbl->ddpfSurface.dwZBufferBitDepth;
    }
    else {
        dwZDepth = dwRenderDepth;
    }

    // treat 24 and 32 bits as synonymous
    if (dwZDepth == 24) dwZDepth = 32;

    // we only allow 16- and 32-bit z-buffers
    if ((dwZDepth != 16) &&
        (dwZDepth != 32)) {
        DPF ("unsupported zeta depth in nvCalculateZBufferParams");
        dbgTracePop();
        return (DDERR_INVALIDPIXELFORMAT);
    }

    // ideally we want to always be strict if this is a DX7 app, but we have
    // no way of telling what kind of app this is in this context
    if ((getDC()->nvD3DRegistryData.regValidateZMethod == D3D_REG_VALIDATEZMETHOD_STRICT) &&
        (dwRenderDepth != dwZDepth)) {
        dbgTracePop();
        return (DDERR_INVALIDPIXELFORMAT);
    }

    *pBPPRequested = dwZDepth >> 3;

    if ((*pBPPRequested == 4)
     || (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
     || (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_1196_CELSIUS)) {
        // if they requested 32 bits or we have kelvin or an nv11, they get what they want
        *pBPPGranted = *pBPPRequested;
    }
    else {
        // on non-mixed buffer hardware, grant a z buffer compatible with the render target
        *pBPPGranted = (dwRenderDepth > 16) ? 4 : 2;
    }

    *pWidth  = pDDSLcl->lpGbl->wWidth;
    *pHeight = pDDSLcl->lpGbl->wHeight;

    *pPitch  = *pBPPGranted * *pWidth;
    *pPitch = (*pPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

    if (*pBPPGranted == 2) {
        *pFormat = NV_SURFACE_FORMAT_Z16;
    }
    else {  // *pBPPGranted = 4
        if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_STENCILBUFFER) {
            *pFormat = NV_SURFACE_FORMAT_Z24S8;
        }
        else {
            *pFormat = NV_SURFACE_FORMAT_Z24X8;
        }
    }

    // Multisampling surfaces
    *pMultiSampleBits = pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps3 & DDSCAPS3_MULTISAMPLE_MASK;

    dbgTracePop();
    return (DD_OK);
}

//---------------------------------------------------------------------------

DWORD nvCalculateSurfaceParams (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl,
                                DWORD *pFormat, BOOL *bFourCC,
                                DWORD *pWidth, DWORD *pHeight, DWORD *pDepth,
                                DWORD *pMultiSampleBits,
                                DWORD *pBPPRequested, DWORD *pBPPGranted,
                                DWORD *pPitch, DWORD *pSlicePitch, BOOL bComputePitch /* rather than extracting it */)
{
    DWORD dwBitCount, dwAlphaMask, dwRedMask, dwLumMask;
    BOOL  bIsDXT = FALSE;

    dbgTracePush ("nvCalculateSurfaceParams");

    // take care of execute buffers first
    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_EXECUTEBUFFER) {
        *pFormat       = 0;
        *bFourCC       = FALSE;
        *pWidth        = pDDSLcl->lpGbl->dwLinearSize;
        *pHeight       = 1;
        *pDepth        = 1;
        *pMultiSampleBits = 0;
        *pBPPRequested = 0;
        *pBPPGranted   = 0;
        *pPitch        = *pWidth;
        dbgTracePop();
        return (DD_OK);
    }

    // error message setup
#ifdef DEBUG
    const char *pErrMsg = NULL;
    #define ERRMSG(str) (pErrMsg = str)
#else
    #define ERRMSG(str) //no-op
#endif

    // determine surface format, BPP, and fourCC'ness according to surface type

#ifdef WINNT
    // under WIN2K the DDRAWISURF_HASPIXELFORMAT flag is undefined and implied to be set
    // use the display bit depth when getting a request for an 8 bit surface,
    // this allows the 8 bit DCT AGP Blit tests to run
    if (!((pDDSLcl->lpGbl->ddpfSurface.dwRGBBitCount == 8) && (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_RGB) && !(pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_PALETTEINDEXED8))) {
#else
    if (pDDSLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT) {
#endif

        // if we have a pixel format, we must be either RGB, fourCC, or a zbuffer

        if ((pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_RGB) ||
            (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_FOURCC) && (pDDSLcl->lpGbl->ddpfSurface.dwFourCC == FOURCC_RGB0)) {

            // find the bit count, the alpha mask, and the red mask.
            // these are sufficient to infer the surface format

            dwBitCount  = pDDSLcl->lpGbl->ddpfSurface.dwRGBBitCount;
            dwAlphaMask = (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_ALPHAPIXELS) ?
                           pDDSLcl->lpGbl->ddpfSurface.dwRGBAlphaBitMask : 0;
            dwRedMask   = pDDSLcl->lpGbl->ddpfSurface.dwRBitMask;

            // derive an nv format from the bit count and masks

            if ((dwBitCount == 32) || (dwBitCount == 24)) {
                switch (dwAlphaMask) {
                    case 0x00000000:
#ifdef SPOOF_ALPHA
                        if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_3DDEVICE) && !(pDDSLcl->ddsCaps.dwCaps & (DDSCAPS_TEXTURE | DDSCAPS_OFFSCREENPLAIN))) {
                            *pFormat = NV_SURFACE_FORMAT_A8R8G8B8;
                        } else {
                            *pFormat = NV_SURFACE_FORMAT_X8R8G8B8;
                        }

#else
                        *pFormat = NV_SURFACE_FORMAT_X8R8G8B8;
#endif
                        break;
                    case 0x7F000000:
                        *pFormat = NV_SURFACE_FORMAT_X1A7R8G8B8;
                        break;
                    case 0xFF000000:
                        *pFormat = NV_SURFACE_FORMAT_A8R8G8B8;
                        break;
                    default:
                        ERRMSG( "invalid %02ld bpp surface format!! [RGB]" );
                        goto bad_fmt_exit;
                }
                *pBPPRequested = *pBPPGranted = 4;
            }

            else if (dwBitCount == 16) {
                switch (dwAlphaMask) {
                    case 0x00000000:
                        if (dwRedMask == 0x7C00) {
                            *pFormat = NV_SURFACE_FORMAT_X1R5G5B5;
                        }
                        else {
                            // dwRedMask == 0xF800
                            *pFormat = NV_SURFACE_FORMAT_R5G6B5;
                        }
                        break;
                    case 0x00008000:
                        *pFormat = NV_SURFACE_FORMAT_A1R5G5B5;
                        break;
                    case 0x0000F000:
                        *pFormat = NV_SURFACE_FORMAT_A4R4G4B4;
                        break;
                    default:
                        ERRMSG( "invalid 16 bpp surface format!! [RGB]" );
                        goto bad_fmt_exit;
                }
                *pBPPRequested = *pBPPGranted = 2;
            }

            else if ((dwBitCount == 8) && (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_PALETTEINDEXED8)) {
                *pFormat = NV_SURFACE_FORMAT_I8_A8R8G8B8;  // Indexed format
                *pBPPRequested = *pBPPGranted = 1;
            }

            else {
                ERRMSG( "invalid %02ld bpp surface format!! [fallthru]" );
                goto bad_fmt_exit;
            }

            *bFourCC = FALSE;

        }

        else if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_BUMPDUDV) {

            dwBitCount = pDDSLcl->lpGbl->ddpfSurface.dwBumpBitCount;
            dwLumMask  = (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_BUMPLUMINANCE) ?
                         pDDSLcl->lpGbl->ddpfSurface.dwBumpLuminanceBitMask : 0;

            if (dwBitCount == 16) {
                *pFormat = (dwLumMask == 0) ? NV_SURFACE_FORMAT_DV8DU8 : NV_SURFACE_FORMAT_L6DV5DU5;
                *pBPPRequested = *pBPPGranted = 2;
            }
//            else if (dwBitCount == 24) {
//                *pFormat = NV_SURFACE_FORMAT_X8L8DV8DU8;
//                *pBPPRequested = *pBPPGranted = 3;
//            }
            else if (dwBitCount == 32) {
                *pFormat = NV_SURFACE_FORMAT_X8L8DV8DU8;
                *pBPPRequested = *pBPPGranted = 4;
            } else {
                // Invalid bitdepth
                ERRMSG( "nvCalculateSurfaceParams - invalid Bump map dUdV format!" );
                goto bad_fmt_exit;
            }

            *bFourCC = FALSE;

        }

        else if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_FOURCC) {

            // query the fourcc list
            switch (pDDSLcl->lpGbl->ddpfSurface.dwFourCC) {
                case FOURCC_RGB0:
                    // we should handle this case with regular rgb surfaces
                    nvAssert (0);
                    break;
                case FOURCC_NVT0:
                case FOURCC_NVS0:
                    *pFormat = NV_SURFACE_FORMAT_X1R5G5B5;
                    *pBPPRequested = 2;
                    SET_GBL_SURFACE_DATA (pDDSLcl,DDPF_RGB,16,0x7C00,0x03E0,0x001F,0x0000);
                    break;
                case FOURCC_NVT1:
                case FOURCC_NVS1:
                    *pFormat = NV_SURFACE_FORMAT_A1R5G5B5;
                    *pBPPRequested = 2;
                    SET_GBL_SURFACE_DATA (pDDSLcl,(DDPF_RGB|DDPF_ALPHAPIXELS),16,0x7C00,0x03E0,0x001F,0x8000);
                    break;
                case FOURCC_NVT2:
                case FOURCC_NVS2:
                    *pFormat = NV_SURFACE_FORMAT_A4R4G4B4;
                    *pBPPRequested = 2;
                    SET_GBL_SURFACE_DATA (pDDSLcl,(DDPF_RGB|DDPF_ALPHAPIXELS),16,0x0F00,0x00F0,0x000F,0xF000);
                    break;
                case FOURCC_NVT3:
                case FOURCC_NVS3:
                    *pFormat = NV_SURFACE_FORMAT_R5G6B5;
                    *pBPPRequested = 2;
                    SET_GBL_SURFACE_DATA (pDDSLcl,DDPF_RGB,16,0xF800,0x07E0,0x001F,0x0000);
                    break;
                case FOURCC_NVT4:
                case FOURCC_NVS4:
                    *pFormat = NV_SURFACE_FORMAT_X8R8G8B8;
                    *pBPPRequested = 4;
                    SET_GBL_SURFACE_DATA (pDDSLcl,DDPF_RGB,32,0x00FF0000,0x0000FF00,0x000000FF,0x00000000);
                    break;
                case FOURCC_NVT5:
                case FOURCC_NVS5:
                    *pFormat = NV_SURFACE_FORMAT_A8R8G8B8;
                    *pBPPRequested = 4;
                    SET_GBL_SURFACE_DATA (pDDSLcl,(DDPF_RGB|DDPF_ALPHAPIXELS),32,0x00FF0000,0x0000FF00,0x000000FF,0xFF000000);
                    break;
#ifdef TEXFORMAT_CRD // --------------------------------
                case FOURCC_NVHU:
                    *pFormat = NV_SURFACE_FORMAT_HILO_1_V16U16;
                    *pBPPRequested = 4;
                    break;
                case FOURCC_NVHS:
                    *pFormat = NV_SURFACE_FORMAT_HILO_HEMI_V16U16;
                    *pBPPRequested = 4;
                    break;
#else // !TEXFORMAT_CRD  --------------------------------
#if defined(HILO_SUPPORT_DX7) || defined(HILO_SUPPORT_DX8)
                case FOURCC_NVHU:
                    *pFormat = NV_SURFACE_FORMAT_HILO_1_V16U16;
                    *pBPPRequested = 4;
                    break;
                case FOURCC_NVHS:
                    *pFormat = NV_SURFACE_FORMAT_HILO_HEMI_V16U16;
                    *pBPPRequested = 4;
                    break;
#endif
#endif // !TEXFORMAT_CRD  --------------------------------
                case D3DFMT_Q8W8V8U8:
                    *pFormat = NV_SURFACE_FORMAT_A8B8G8R8;
                    *pBPPRequested = 4;
                    break;
                case FOURCC_DXT1:
                    *pFormat = NV_SURFACE_FORMAT_DXT1_A1R5G5B5;
                    *pBPPRequested = 8;
                    bIsDXT = TRUE;
                    break;
                case FOURCC_DXT2:
                case FOURCC_DXT3:
                    *pFormat = NV_SURFACE_FORMAT_DXT23_A8R8G8B8;
                    *pBPPRequested = 16;
                    bIsDXT = TRUE;
                    break;
                case FOURCC_DXT4:
                case FOURCC_DXT5:
                    *pFormat = NV_SURFACE_FORMAT_DXT45_A8R8G8B8;
                    *pBPPRequested = 16;
                    bIsDXT = TRUE;
                    break;
                default:
                    // Invalid pixel format. Return the error.
                    ERRMSG( "nvCalculateSurfaceParams - invalid 4cc format!" );
                    goto bad_fmt_exit;
            }  // switch

            *pBPPGranted = *pBPPRequested;
            *bFourCC = TRUE;

        }

        else if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_ZBUFFER) {

            DWORD dwRetVal = nvCalculateZBufferParams (pDDSLcl, pFormat, pWidth, pHeight,
                                                       pMultiSampleBits, pBPPRequested, pBPPGranted, pPitch);
            *pDepth  = 1;
            *bFourCC = FALSE;
            dbgTracePop();
            return (dwRetVal);

        }

        else {

            // we were told we had a pixel format but this is neither fourCC nor RGB nor a zbuffer
            ERRMSG( "formatted surface is neither fourCC nor RGB" );
            goto bad_fmt_exit;
        }

    }

    else {  // !(pDDSLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT)

        // we weren't given any pixel format
        dwBitCount  = GET_MODE_BPP();

        switch (dwBitCount)
        {
            case 8:
                *pFormat = NV_SURFACE_FORMAT_Y8;
                *pBPPRequested = 1;
                break;
            case 16:
                *pFormat = NV_SURFACE_FORMAT_R5G6B5;
                *pBPPRequested = 2;
                break;
            case 24:
            case 32:  // we treat these as synonymous
#ifdef SPOOF_ALPHA
                if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_3DDEVICE) && !(pDDSLcl->ddsCaps.dwCaps & (DDSCAPS_TEXTURE | DDSCAPS_OFFSCREENPLAIN))) {
                    *pFormat = NV_SURFACE_FORMAT_A8R8G8B8;
                } else {
                    *pFormat = NV_SURFACE_FORMAT_X8R8G8B8;
                }
#else
                *pFormat = NV_SURFACE_FORMAT_X8R8G8B8;
#endif
                *pBPPRequested = 4;
                break;
            default:
                ERRMSG( "illegal bit depth" );  // maybe do INT3 here?
                goto bad_fmt_exit;
        }

        *pBPPGranted = *pBPPRequested;
        *bFourCC = FALSE;

    }

    // get the width and height

    if (pDDSLcl->ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE)) {
        *pWidth  = GET_MODE_WIDTH();
        *pHeight = GET_MODE_HEIGHT();
        *pDepth  = 1;
    }
    else {
        *pWidth  = bIsDXT ? max (1, (pDDSLcl->lpGbl->wWidth  >> 2))
                          : pDDSLcl->lpGbl->wWidth;
        *pHeight = bIsDXT ? max (1, (pDDSLcl->lpGbl->wHeight >> 2))
                          : pDDSLcl->lpGbl->wHeight;
        // stored in the lower WORD of the dwCaps4
        *pDepth  = bIsDXT ? max (1, ((pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps4 & 0xffff) >> 2))
                          : max (1, pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps4 & 0xffff);
    }

    // calculate pitch
    if (bComputePitch)
    {
        *pPitch = *pBPPGranted * *pWidth;

        // align the pitch if this isn't a system memory surface or a texture
        if (!(pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
         && !(pDDSLcl->ddsCaps.dwCaps & DDSCAPS_TEXTURE))
        {
            *pPitch = (*pPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        }

        *pSlicePitch = (*pPitch) * (*pHeight);
    }
    else
    {
        if (bIsDXT) {
            // linear size is given - we recompute pitch here
            *pPitch = (*pWidth) * (*pBPPGranted);
            *pSlicePitch = (*pPitch) * (*pHeight);
        }
        else {
            *pPitch = pDDSLcl->lpGbl->lPitch;
            *pSlicePitch = pDDSLcl->lpGbl->lSlicePitch;
        }
    }

    // Multisampling surfaces
    *pMultiSampleBits = pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps3 & DDSCAPS3_MULTISAMPLE_MASK;

    dbgTracePop();
    return (DD_OK);

// jump to here for bad pixel format
bad_fmt_exit:
#ifdef DEBUG
    nvAssert( pErrMsg != NULL );
    DPF_LEVEL(NVDBG_LEVEL_ERROR, (char*)pErrMsg, dwBitCount);
    // dbgD3DError(); -- let the calling program decide if it wants to throw an INT3
    dbgTracePop();
#endif
    return DDERR_INVALIDPIXELFORMAT;
}

//---------------------------------------------------------------------------

DWORD nvCreateZBuffer (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl)
{
    DWORD dwFormat, dwWidth, dwHeight, dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwPitch;

    dbgTracePush ("nvCreateZBuffer");

#ifndef WINNT
    CNvObject         *pNvObj = new CNvObject (pDDSLcl->dwProcessId);
#else
    CNvObject         *pNvObj = new CNvObject (0);
#endif // WINNT
    CSimpleSurface    *pSurf  = new CSimpleSurface;

    if (!pNvObj || !pSurf) {
        if (pNvObj) pNvObj->release();
        if (pSurf ) delete pSurf;
        pDDSLcl->lpGbl->fpVidMem = NULL;
        SET_PNVOBJ (pDDSLcl, NULL);
        dbgTracePop();
        return (DDERR_OUTOFMEMORY);
    }

    pNvObj->setObject (CNvObject::NVOBJ_SIMPLESURFACE, pSurf);

    DWORD dwRV = nvCalculateZBufferParams (pDDSLcl, &dwFormat, &dwWidth, &dwHeight, &dwMultiSampleBits, &dwBPPRequested, &dwBPPGranted, &dwPitch);
    if (dwRV != DD_OK) {
        if (pNvObj) pNvObj->release();
        dbgTracePop();
        return (dwRV);
    }

#ifdef CAPTURE
    if ((getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) &&
        (getDC()->nvD3DRegistryData.regCaptureConfig & D3D_REG_CAPTURECONFIG_ALLOC4X)) {

        CAPTURE_ANNOTATION anno;
        anno.dwJmpCommand         = CAPTURE_JMP_COMMAND;
        anno.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
        anno.dwExtensionID        = CAPTURE_XID_ANNOTATION;
        anno.dwValue              = CAPTURE_ANNOTATION_ALLOC4X;
        captureLog (&anno, sizeof(anno));

        dwWidth  <<= 1;
        dwHeight <<= 1;
        dwPitch  <<= 1;
    }
#endif

    DWORD dwAllocFlags = (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN))        ?
                         (CSimpleSurface::ALLOCATE_TILED | CSimpleSurface::ALLOCATE_AS_ZBUFFER_UNC) :
                         (CSimpleSurface::ALLOCATE_TILED | CSimpleSurface::ALLOCATE_AS_ZBUFFER_CMP);
#ifdef  STEREO_SUPPORT
    if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_STEREOSURFACELEFT
        && STEREO_ENABLED)
    {
		dwAllocFlags |= CSimpleSurface::ALLOCATE_AS_STEREO;
	}
#endif  //STEREO_SUPPORT

    if (pSurf->create (pNvObj, dwFormat, dwWidth, dwHeight, 1,
                       dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwPitch,
                       CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID,
                       dwAllocFlags

#ifdef CAPTURE
                      ,CAPTURE_SURFACE_KIND_ZETA
#endif
                       ))
    {
        // successful allocation
        SET_PNVOBJ (pDDSLcl, pNvObj);  // BUGBUG does this really work for z buffers?
        pDDSLcl->lpGbl->fpVidMem                      = pSurf->getfpVidMem();
        pDDSLcl->lpGbl->lPitch                        = pSurf->getPitch();
        pDDSLcl->lpGbl->lSlicePitch                   = pSurf->getSlicePitch();
        pDDSLcl->lpGbl->ddpfSurface.dwZBufferBitDepth = dwBPPGranted << 3;
        pDDSLcl->lpGbl->ddpfSurface.dwZBitMask        = (dwBPPGranted==2) ? 0x0000ffff : 0xffffff00;
        pDDSLcl->lpGbl->dwReserved1                   = 0;
        pDDSLcl->ddsCaps.dwCaps                      |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
        pDDSLcl->ddsCaps.dwCaps                      &= ~DDSCAPS_SYSTEMMEMORY;
        pSurf->tagAsOwner();
    }

    else {
        DPF_LEVEL (NVDBG_LEVEL_SURFACE, "Video memory Z buffer surface allocation failed" );
        if (pNvObj) pNvObj->release();
        dbgTracePop();
        return (DDERR_OUTOFVIDEOMEMORY);
    }
#ifdef DEBUG_SURFACE_PLACEMENT
        csd.SetZBuffer(pSurf);
#endif

    dbgTracePop();
    return(DD_OK);

}  // nvCreateZBuffer()

//---------------------------------------------------------------------------

// returns an error code that CreateSurface should return via ->ddRVal

DWORD nvCreateSurfaceWithFormat (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl,
                                 LPDDRAWI_DIRECTDRAW_GBL pDDGbl, DWORD dwExtraSize, DWORD dwExtraNumSurfaces, DWORD dwBlockHeight)
{
    DWORD dwPitch, dwSlicePitch, dwMultiSampleBits;

    dbgTracePush ("nvCreateSurfaceWithFormat");

    DWORD dwModeBPP = GET_MODE_BPP() >> 3;

    if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_ALPHAPREMULT) {

        // BUGBUG needs to be G.U.D.ified someday
        DPF_LEVEL(NVDBG_LEVEL_SURFACE,  "  ALPHA_PREMULT surface requested" );
        dwPitch = pDDSLcl->lpGbl->wWidth * dwModeBPP;
        dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        dwSlicePitch = dwPitch * pDDSLcl->lpGbl->wHeight;

        DWORD dwBlockSize = (dwPitch * ((DWORD)pDDSLcl->lpGbl->wHeight));
        pDDSLcl->lpGbl->ddpfSurface.dwRGBBitCount = dwModeBPP << 3;
        pDDSLcl->lpGbl->ddpfSurface.dwRGBAlphaBitMask = 0xFF000000;
        pDDSLcl->lpGbl->ddpfSurface.dwRBitMask = 0x00FF0000;
        pDDSLcl->lpGbl->ddpfSurface.dwGBitMask = 0x0000FF00;
        pDDSLcl->lpGbl->ddpfSurface.dwBBitMask = 0x000000FF;
        pDDSLcl->lpGbl->lPitch = dwPitch;
        pDDSLcl->lpGbl->lSlicePitch = dwSlicePitch;
        pDDSLcl->lpGbl->fpVidMem = 0;
        pDDSLcl->lpGbl->dwReserved1 = 0;

        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

            DWORD dwStatus;
            NVHEAP_ALLOC (dwStatus, pDDSLcl->lpGbl->fpVidMem, dwBlockSize, TYPE_IMAGE);
            if (dwStatus == 0) {
                pDriverData->DDrawVideoSurfaceCount++;
                pDDSLcl->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                pDDSLcl->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
            }

        }

        else {
            dbgTracePop();
            return (DDERR_OUTOFMEMORY);
        }

        // if one of these fails, then this code was and is totally broken.
        // it used to just continue on from this if block. i've added the return.
        // as best i can tell. there was nothing preventing us from allocating this
        // surface twice. why would we want to do that?
        assert (!(pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_FOURCC));
        assert (!(pDDSLcl->ddsCaps.dwCaps & DDSCAPS_TEXTURE));
        assert (!(pDDSLcl->ddsCaps.dwCaps & DDSCAPS_ZBUFFER));
        dbgTracePop();
        return (DD_OK);

    }

    if (pDDSLcl->lpGbl->ddpfSurface.dwFlags & DDPF_FOURCC) {

        // BUGBUG needs to be G.U.D.ified someday
        DWORD dwRV = VppCreateFourCCSurface(&(pDriverData->vpp),pDDSLcl, pDDGbl,dwExtraSize, dwExtraNumSurfaces, dwBlockHeight);
        dbgTracePop();
        return (dwRV);

    }

    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) {

        DWORD dwFormat, dwWidth, dwHeight, dwDepth, dwBPPRequested, dwBPPGranted, dwPitch;
        BOOL bFourCC;

#ifndef WINNT
        CNvObject      *pNvObj = new CNvObject (pDDSLcl->dwProcessId);
#else
        CNvObject      *pNvObj = new CNvObject (0);
#endif // WINNT

        CSimpleSurface *pSurf  = new CSimpleSurface;

        if (!pNvObj || !pSurf) {
            if (pNvObj) pNvObj->release();
            if (pSurf ) delete pSurf;
            pDDSLcl->lpGbl->fpVidMem = NULL;
            SET_PNVOBJ (pDDSLcl, NULL);
            dbgTracePop();
            return (DDERR_OUTOFMEMORY);
        }

        pNvObj->setObject (CNvObject::NVOBJ_SIMPLESURFACE, pSurf);

        DWORD dwRV = nvCalculateSurfaceParams (pDDSLcl, &dwFormat, &bFourCC, &dwWidth, &dwHeight, &dwDepth, &dwMultiSampleBits,
                                               &dwBPPRequested, &dwBPPGranted, &dwPitch, &dwSlicePitch, TRUE);
        if (dwRV != DD_OK) {
            dbgD3DError();  // ?? can we get here for legit reasons?
            if (pNvObj) pNvObj->release();
            dbgTracePop();
            return (dwRV);
        }

        // figure out where to stick it
        DWORD dwPreferredHeap, dwAllowedHeaps, dwAllocFlags;

        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
            dwPreferredHeap = (pDriverData->GARTLinearBase > 0) ?
                              CSimpleSurface::HEAP_AGP :
                              CSimpleSurface::HEAP_PCI;
            dwAllowedHeaps  = dwPreferredHeap;
            dwAllocFlags    = CSimpleSurface::ALLOCATE_SIMPLE;
        }
        else {
            dwPreferredHeap = CSimpleSurface::HEAP_VID;
            dwAllowedHeaps  = dwPreferredHeap;

            if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_3DDEVICE) {
                dwAllocFlags = CSimpleSurface::ALLOCATE_TILED;
            }
            else {
                dwAllocFlags = CSimpleSurface::ALLOCATE_SIMPLE;
            }
            // if they haven't demanded video memory, allow fallback to system
            if (!(pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)) {
                dwAllowedHeaps |= CSimpleSurface::HEAP_SYS;
            }
        }

        if (pSurf->create (pNvObj, dwFormat, dwWidth, dwHeight, dwDepth,
                           dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwPitch,
                           dwAllowedHeaps, dwPreferredHeap, dwAllocFlags
#ifdef CAPTURE
                          ,CAPTURE_SURFACE_KIND_UNKNOWN
#endif
                           ))
        {
            // allocation succeeded. set stuff in MS-land
            if (pSurf->getHeapLocation() == CSimpleSurface::HEAP_SYS) {
                pDDSLcl->lpGbl->fpVidMem    = (FLATPTR)pSurf->getfpVidMem();
                pDDSLcl->lpGbl->dwReserved1 = (DWORD)pSurf->getfpVidMem();
            }
#ifdef WINNT
            // MS-suggested Hack to make AGP surfaces whose mem was
            // allocated through HeapVidMemAllocAligned work properly
            else if (pSurf->getHeapLocation() == CSimpleSurface::HEAP_AGP) {
#ifdef NV_AGP
                pDDSLcl->lpGbl->fpVidMem = nvAGPGetUserAddr(pSurf->getAddress());
#else
                pDDSLcl->lpGbl->lpVidMemHeap = ppdev->AgpHeap;
                pDDSLcl->lpGbl->fpHeapOffset = pSurf->getfpVidMem() + pDriverData->GARTLinearHeapOffset;
#endif
                pDDSLcl->lpGbl->dwReserved1  = 0;
            }
#endif // WINNT
            else {
                pDDSLcl->lpGbl->fpVidMem    = pSurf->getfpVidMem();
                pDDSLcl->lpGbl->dwReserved1 = 0;
            }
            SET_PNVOBJ (pDDSLcl, pNvObj);
            pDDSLcl->lpGbl->lPitch      = pSurf->getPitch();  // may be different than dwPitch!
            pDDSLcl->lpGbl->lSlicePitch = pSurf->getSlicePitch();
            pDDSLcl->ddsCaps.dwCaps     = pSurf->modifyDDCaps (pDDSLcl->ddsCaps.dwCaps);
            pSurf->tagAsOwner();
        }

        else {
            // surface creation failed
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Offscreen surface allocation failed" );
            if (pNvObj) pNvObj->release();
            pDDSLcl->lpGbl->dwReserved1 = 0;
            dbgTracePop();
            return (DDERR_OUTOFVIDEOMEMORY);
        }

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Offscreen surface granted" );
        return (DD_OK);
    }

    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_ZBUFFER) {

        DWORD dwRV = nvCreateZBuffer (pDDSLcl);
        dbgTracePop();
        return (dwRV);

    }

    DPF ("unknown surface type in nvCreateSurfaceWithFormat");
    dbgD3DError();

    dbgTracePop();
    return (DDERR_UNSUPPORTEDFORMAT);

}  // nvCreateSurfaceWithFormat()

//---------------------------------------------------------------------------
// nvApplicationCheck
//      Does application specific things
//      ***** DO NOT ADD ANY ENTRIES WITHOUT A COMPLETE DESCRIPTION OF THE BUG *****
//      ***** SYMPTOMS AND THE SOLUTION.  THIS MEANS YOU                       *****
void nvApplicationCheck(void)
{
#if 0
#ifndef WINNT
LPTSTR pCmdLine, pExe, pStr;

    // get command line
    pCmdLine = GetCommandLine();

    // strip path and leave the raw executable name
    for (pExe = pCmdLine; *pCmdLine != '\0'; pCmdLine++) {
        if (*pCmdLine == '\\' || *pCmdLine == ':') {
            pExe = pCmdLine + 1;
        }
    }

    // early string termination if quote, space, or control character is detected
    for (pStr = pExe; *pStr != '\0'; pStr++) {
        if (*pStr == '\"' || *pStr < ' ') {
            *pStr = '\0';
        }
    }

    // ------------------------
    // Rainbow Six: Rogue Spear
    //      This app thrashes blit context DMAs, so blit code wants to fall back to old
    //      blit code for performance reasons.  However, old blit code has a bug in it
    //      (if you want to fix it, you obviously have not seen the old fallback code)
    //      and text is corrupted.  The end of each scan line is not correctly fed into
    //      the image_from_cpu class.  The fix is to detect this app, and let it thrash.
    //      It will be slower, but at least it will work.  See code in nvBlit.cpp.
    //      --- slum
    if (nvStrCmp(pExe, "RogueSpear.exe")==0) {
        pDriverData->bltData.dwLetItThrash = TRUE;
    }
#endif
#endif
}

//---------------------------------------------------------------------------
//
// returns an error code that CreateSurface should return via ->ddRVal
//
// called only from one place in CreateSurface32()

DWORD nvCreateSurfaceWithoutFormat (LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl,
                                    LPDDRAWI_DIRECTDRAW_GBL pDDGbl)
{
    dbgTracePush ("nvCreateSurfaceWithoutFormat");

    DWORD dwModeBPP = GET_MODE_BPP() >> 3;

    // create the basic objects
#ifdef WINNT
    CNvObject      *pNvObj = new CNvObject (0);
#else // !WINNT
    CNvObject      *pNvObj = new CNvObject (pDDSLcl->dwProcessId);
#endif // !WINNT

    CSimpleSurface *pSurf  = new CSimpleSurface;

    if (!pNvObj || !pSurf) {
        if (pNvObj) pNvObj->release();
        if (pSurf ) delete pSurf;
        pDDSLcl->lpGbl->fpVidMem = NULL;
        SET_PNVOBJ (pDDSLcl, NULL);
        dbgTracePop();
        return (DDERR_OUTOFMEMORY);
    }

    pNvObj->setObject (CNvObject::NVOBJ_SIMPLESURFACE, pSurf);

    // determine common surface parameters
    DWORD dwFormat, dwWidth, dwHeight, dwDepth, dwBPPRequested, dwBPPGranted, dwPitch, dwSlicePitch, dwMultiSampleBits;
    BOOL  bFourCC;

    DWORD dwRV = nvCalculateSurfaceParams (pDDSLcl, &dwFormat, &bFourCC, &dwWidth, &dwHeight, &dwDepth, &dwMultiSampleBits,
                                           &dwBPPRequested, &dwBPPGranted, &dwPitch, &dwSlicePitch, TRUE);

    if (dwRV != DD_OK) {
        dbgD3DError();  // ?? can we get here for legit reasons?
        if (pNvObj) pNvObj->release();
        dbgTracePop();
        return (dwRV);
    }

    // handle surface specifics according to type
    if (pDDSLcl->ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE)) {

        nvApplicationCheck();

        // Must allow primary surface access even if not correct pixel depth for 3D
        // Fixes Jedi Knight bug
        if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_3DDEVICE) &&
            (!(pDDSLcl->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE))) {
            if (dwModeBPP == 1) {
                pNvObj->release();
                dbgTracePop();
                return (DDERR_INVALIDPIXELFORMAT);
            }
        }

#ifdef WINNT
        ppdev->bD3DappIsRunning = TRUE;
        dwPitch = ppdev->lDelta;
#else // !WINNT
        // override pitch determined above with the pitch that's
        // been rounded up to account for tiling
        dwPitch = ((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deDeltaScan;
#endif // !WINNT

#ifdef DEBUG
        dbgPrimaryPitch  = dwPitch;
        dbgPrimaryHeight = dwHeight;
        dbgPrimaryBPP    = dwBPPGranted;
#endif

#ifndef WINNT
        if (pDXShare->pNvPrimarySurf) {
            // some other context has already created an NvObject for this primary
            // delete the new objects we just created
            pNvObj->release();

            // assign pointers to the old objects
            pNvObj = (CNvObject*)pDXShare->pNvPrimarySurf;
            pSurf = pNvObj->getSimpleSurface();
        } else {
            // use the new pNvObject
            pDXShare->pNvPrimarySurf = (DWORD)pNvObj;
        }
#endif // !WINNT

#ifdef  STEREO_SUPPORT
        pSurf->destroyStereo();
#endif  //STEREO_SUPPORT

        pSurf->own (VIDMEM_ADDR(GET_PRIMARY_ADDR()), dwPitch, dwHeight, CSimpleSurface::HEAP_VID, FALSE);
        pSurf->setWrapper (pNvObj);
        pSurf->setFormat (dwFormat);
        pSurf->setWidth (dwWidth);
        pSurf->setBPP (dwBPPGranted);
        pSurf->setBPPRequested (dwBPPRequested);
        pSurf->setDepth(dwDepth);

#ifdef DEBUG_SURFACE_PLACEMENT
        csd.SetFrontBuffer(pSurf);
#endif
        // set stuff back in MS-land as well
        SET_PNVOBJ (pDDSLcl, pNvObj);
        pDDSLcl->lpGbl->fpVidMem    = GET_PRIMARY_ADDR();
        pDDSLcl->lpGbl->lPitch      = dwPitch;
        pDDSLcl->lpGbl->lSlicePitch = dwSlicePitch;
        pDDSLcl->lpGbl->dwReserved1 = 0;
        pDDSLcl->ddsCaps.dwCaps    |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);

#ifdef  STEREO_SUPPORT
		if (STEREO_ENABLED)
		{				
			CreateLockSubstitute(pDDSLcl);
			if (!pSurf->createStereo ())
			{
				LOG("Can't create a stereo rendering surface");
			}
		}
#endif  //STEREO_SUPPORT

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "front buffer granted" );

        // we need to tell displaydriver that a PRIMARY surface was acquired
        NV_OGLDDRAWSURFACECREATED( ppdev );

    }

    else if (pDDSLcl->ddsCaps.dwCaps & (DDSCAPS_BACKBUFFER | DDSCAPS_FLIP)) {

        // make sure we come up with the same value we used to...
        nvAssert (dwWidth  == (DWORD)GET_MODE_WIDTH());
        nvAssert (dwHeight == (DWORD)GET_MODE_HEIGHT());
        nvAssert (dwBPPRequested == dwModeBPP);

#ifdef CAPTURE
        if ((getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) &&
            (getDC()->nvD3DRegistryData.regCaptureConfig & D3D_REG_CAPTURECONFIG_ALLOC4X)) {

            CAPTURE_ANNOTATION anno;
            anno.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            anno.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            anno.dwExtensionID        = CAPTURE_XID_ANNOTATION;
            anno.dwValue              = CAPTURE_ANNOTATION_ALLOC4X;
            captureLog (&anno, sizeof(anno));

            dwWidth  <<= 1;
            dwHeight <<= 1;
            dwPitch  <<= 1;
        }
#endif

		DWORD	dwAllocFlags = CSimpleSurface::ALLOCATE_TILED;
#ifdef  STEREO_SUPPORT
		if (pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_STEREOSURFACELEFT
			&& STEREO_ENABLED)
		{
			dwAllocFlags |= CSimpleSurface::ALLOCATE_AS_STEREO;
		}
#endif  //STEREO_SUPPORT
        if (pSurf->create (pNvObj, dwFormat, dwWidth, dwHeight, dwDepth,
                           dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwPitch,
                           CSimpleSurface::HEAP_VID, CSimpleSurface::HEAP_VID,
                           dwAllocFlags
#ifdef CAPTURE
                          ,CAPTURE_SURFACE_KIND_TARGET
#endif
                           ))
        {
            // set stuff in MS-land
            SET_PNVOBJ (pDDSLcl, pNvObj);
            pDDSLcl->lpGbl->fpVidMem = pSurf->getfpVidMem();
            pDDSLcl->lpGbl->lPitch   = pSurf->getPitch();
            pDDSLcl->lpGbl->lSlicePitch   = pSurf->getSlicePitch();
            pDDSLcl->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
            pDDSLcl->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
            pSurf->tagAsOwner();
        }

        else {
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Backbuffer or flippable video memory surface allocation failed" );
            if(pNvObj) pNvObj->release();
            dbgTracePop();
            return (DDERR_OUTOFVIDEOMEMORY);
        }

#ifdef DEBUG_SURFACE_PLACEMENT
        csd.SetBackBuffer(pSurf);
#endif
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Back buffer or flip surface granted" );

    }

    else if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) {

        putInOffscreen:

        // make sure we come up with the same value we used to...
        nvAssert (dwWidth == (DWORD)pDDSLcl->lpGbl->wWidth);
        nvAssert (dwHeight == (DWORD)pDDSLcl->lpGbl->wHeight);
        nvAssert (dwBPPRequested == dwModeBPP);

        // figure out where to stick it

        DWORD dwPreferredHeap, dwAllowedHeaps, dwAllocFlags;

        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
            dwPreferredHeap = (pDriverData->GARTLinearBase > 0) ?
                              CSimpleSurface::HEAP_AGP :
                              CSimpleSurface::HEAP_PCI;
            dwAllowedHeaps  = dwPreferredHeap;
            dwAllocFlags    = CSimpleSurface::ALLOCATE_SIMPLE;
        }

        else {
            dwPreferredHeap = CSimpleSurface::HEAP_VID;
            dwAllowedHeaps  = dwPreferredHeap;

            if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_3DDEVICE) {
                dwAllocFlags = CSimpleSurface::ALLOCATE_TILED;
            }
            else {
                dwAllocFlags = CSimpleSurface::ALLOCATE_SIMPLE;
            }
            // if they haven't demanded video memory, allow fallback to system
            if (!(pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)) {
                dwAllowedHeaps |= CSimpleSurface::HEAP_SYS;
            }
        }

        if (pSurf->create (pNvObj, dwFormat, dwWidth, dwHeight, dwDepth,
                           dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwPitch,
                           dwAllowedHeaps, dwPreferredHeap, dwAllocFlags
#ifdef CAPTURE
                          ,CAPTURE_SURFACE_KIND_UNKNOWN
#endif
                           ))
        {
            // allocation succeeded. set stuff in MS-land
            if (pSurf->getHeapLocation() == CSimpleSurface::HEAP_SYS) {
                pDDSLcl->lpGbl->fpVidMem    = (FLATPTR)pSurf->getfpVidMem();
                pDDSLcl->lpGbl->dwReserved1 = (DWORD)pSurf->getfpVidMem();
            }
#ifdef WINNT
            // MS-suggested Hack to make AGP surfaces whose mem was
            // allocated through HeapVidMemAllocAligned work properly
            else if (pSurf->getHeapLocation() == CSimpleSurface::HEAP_AGP) {
#ifdef NV_AGP
                pDDSLcl->lpGbl->fpVidMem     = nvAGPGetUserAddr(pSurf->getAddress());
#else
                pDDSLcl->lpGbl->lpVidMemHeap = ppdev->AgpHeap;
                pDDSLcl->lpGbl->fpHeapOffset = pSurf->getfpVidMem() + pDriverData->GARTLinearHeapOffset;
#endif
                pDDSLcl->lpGbl->dwReserved1 = 0;
            }
#endif // WINNT
            else {
                pDDSLcl->lpGbl->fpVidMem    = pSurf->getfpVidMem();
                pDDSLcl->lpGbl->dwReserved1 = 0;
            }
            SET_PNVOBJ (pDDSLcl, pNvObj);
            pDDSLcl->lpGbl->lPitch  = pSurf->getPitch();  // may be different than dwPitch!
            pDDSLcl->lpGbl->lSlicePitch = pSurf->getSlicePitch();  // may be different than dwPitch!
            pDDSLcl->ddsCaps.dwCaps = pSurf->modifyDDCaps (pDDSLcl->ddsCaps.dwCaps);
            pSurf->tagAsOwner();
        }

        else {
            // surface creation failed
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Offscreen surface allocation failed" );
            pDDSLcl->lpGbl->dwReserved1 = 0;
            if (pNvObj) pNvObj->release();
            dbgTracePop();
            return (DDERR_OUTOFVIDEOMEMORY);
        }

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Offscreen surface granted" );

    }

    else if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {

        // make sure we come up with the same value we used to...
        nvAssert (dwWidth == (DWORD)pDDSLcl->lpGbl->wWidth);
        nvAssert (dwHeight == (DWORD)pDDSLcl->lpGbl->wHeight);
        nvAssert (dwBPPRequested == dwModeBPP);

        if (pSurf->create (pNvObj, dwFormat, dwWidth, dwHeight, dwDepth,
                           dwMultiSampleBits, dwBPPRequested, dwBPPGranted, dwPitch,
                           CSimpleSurface::HEAP_SYS, CSimpleSurface::HEAP_SYS,
                           CSimpleSurface::ALLOCATE_SIMPLE
#ifdef CAPTURE
                          ,CAPTURE_SURFACE_KIND_UNKNOWN
#endif
                           ))
        {
            // set stuff in MS-land
            SET_PNVOBJ (pDDSLcl, pNvObj);
            pDDSLcl->lpGbl->fpVidMem    = (FLATPTR)pSurf->getfpVidMem();
            pDDSLcl->lpGbl->dwReserved1 = (DWORD)pSurf->getfpVidMem();
            pDDSLcl->lpGbl->lPitch      = pSurf->getPitch();
            pDDSLcl->lpGbl->lSlicePitch      = pSurf->getSlicePitch();
            pSurf->tagAsOwner();
        }

        else {
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "System memory surface allocation failed" );
            if(pNvObj) pNvObj->release();
            dbgTracePop();
            return (DDERR_OUTOFMEMORY);
        }

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "System memory surface granted" );

    }

    else if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

        goto putInOffscreen;

    }

    dbgTracePop();
    return (DD_OK);

}  // nvCreateSurfaceWithoutFormat ()

#ifdef  NEW_FRONT_RENDER
//This mechanism doesn't work for triple buffering and is intended for debugging only.
FLATPTR BackVidMem;
CNvObject *pBackNvObj;
#endif  //NEW_FRONT_RENDER

//---------------------------------------------------------------------------

// CreateSurface32
// Create a Direct Draw surface.

DWORD __stdcall CreateSurface32 (LPDDHAL_CREATESURFACEDATA lpCSData)
{
    dbgTracePush ("CreateSurface32");
    if (lpCSData->lplpSList[0]->ddsCaps.dwCaps & DDSCAPS_EXECUTEBUFFER )
    {
        //silly hack for screwed up kernel calls
        DWORD dwRetVal;
        dwRetVal = D3DCreateExecuteBuffer32(lpCSData);
        dbgTracePop();
        return dwRetVal;
    }

    DDSTARTTICK(SURF4_CREATESURFACE);

    LPDDRAWI_DIRECTDRAW_GBL pDDGbl = lpCSData->lpDD;
    nvSetDriverDataPtrFromDDGbl (pDDGbl);

    if (pDriverData->dwFullScreenDOSStatus & (FSDOSSTATUS_RECOVERYNEEDED_D3D | FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
                nvD3DRecover();
            }
            if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
                nvDDrawRecover();
            }
        }
    }
    
    // Reset NV and get the monitor frequency after a mode reset
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        if (!nvEnable32 (lpCSData->lpDD)) {
            DDENDTICK(SURF4_CREATESURFACE);
            NvReleaseSemaphore(pDriverData);
            lpCSData->ddRVal = DDERR_OUTOFMEMORY;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    // NOTES:
    // This callback is invoked once the surface objects have been created.
    // You can:
    //   - compute the size of the block, by returning
    //     DDHAL_PLEASEALLOC_BLOCKSIZE in fpVidMem, and putting the size
    //     in dwBlockSizeX and dwBlockSizeY
    //   - override some fields in the surface structure, like the pitch.
    //     (you must specify the pitch if you are computing the size)

    DPF_LEVEL (NVDBG_LEVEL_SURFACE_ACTIVITY,
               "CreateSurface: lpCSData->lplpSList = %08x, lpCSData->dwSCnt = %d",
               lpCSData->lplpSList, lpCSData->dwSCnt);

    // see if any of these surfaces are FOURCC codes...
//    pmAddProcess(pDriverData, GetCurrentProcessId());

    // get first surface
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl = lpCSData->lplpSList[0];

    // can overlay be created in this mode?  Need to call this before nvCSCalcExtraSpace
    if (IS_OVERLAY(pDDSLcl->ddsCaps.dwCaps, pDDSLcl->lpGbl->ddpfSurface.dwFourCC)) {

        // Disable dual view for video overlay
#ifdef WINNT
        if ((ppdev->ulDesktopMode & NV_TWO_DACS) &&
           !(ppdev->TwinView_State == NVTWINVIEW_STATE_CLONE &&
            (pDriverData->vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK)) &&
           !(pDriverData->vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_ALLOWOVL))
#else
        if (pDriverData->dwHeads > 1 &&
            pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE &&
           (pDriverData->vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) == 0 &&
          !(pDriverData->vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_ALLOWOVL))
#endif
        {
            lpCSData->ddRVal = DDERR_NOOVERLAYHW;
            DDENDTICK(SURF4_CREATESURFACE);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
            if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
                nvDDrawRecover();
            }
            else {
                // deny overlay creation if we are in a full screen DOS box (because we need the draw engine)
                lpCSData->ddRVal = DDERR_NOOVERLAYHW;
                DDENDTICK(SURF4_CREATESURFACE);
                NvReleaseSemaphore(pDriverData);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }
        }

        if (VppGetVideoScalerBandwidthStatus(&pDriverData->vpp,pDDSLcl->lpGbl->wWidth) == FALSE) {
            // deny overlay creation if not enough video memory bandwidth
            lpCSData->ddRVal = DDERR_NOOVERLAYHW;
            DDENDTICK(SURF4_CREATESURFACE);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        // Reset flag indicating whether overlay has ever been in Bob mode.
        pDriverData->dwDXVAFlags &= ~DXVA_OVERLAY_WAS_BOBBED;

        // If this create is for an NV12 surface, limit the number of buffers
        // depending on frame buffer size and TwinView state, to leave room
        // for Video Mirror.
        if (pDDSLcl->lpGbl->ddpfSurface.dwFourCC == FOURCC_NV12) {
            DWORD   dwMaxAllowed;

            // For 16MB frame buffers...
            if (pDriverData->TotalVRAM < 0x2000000) {

                if((pDriverData->dwDesktopState == NVTWINVIEW_STATE_DUALVIEW) ||
                    (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE))
                    dwMaxAllowed = 4;
                else
                    dwMaxAllowed = 6;
            }

            // For 32MB frame buffers...
            else {

                if((pDriverData->dwDesktopState == NVTWINVIEW_STATE_DUALVIEW) ||
                    (pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE))
                    dwMaxAllowed = 7;
                else
                    dwMaxAllowed = 8;
            }

            // If number of buffers requested is more than this, return error.
            if (lpCSData->dwSCnt > dwMaxAllowed) {
                lpCSData->ddRVal = DDERR_OUTOFMEMORY;
                DDENDTICK(SURF4_CREATESURFACE);
                NvReleaseSemaphore(pDriverData);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }
        }
    }

    // check to see if the set of surfaces being created is the primary flip chain
    BOOL bPrimaryFlipChain = lpCSData->dwSCnt
                          && (lpCSData->lplpSList[0]->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
                          && (lpCSData->lplpSList[0]->ddsCaps.dwCaps & DDSCAPS_FLIP);

#if (NVARCH >= 0x04) && defined(HWFLIP)

    // set up HW support for flipping
    if (bPrimaryFlipChain) {
        if (lpCSData->dwSCnt > 2) {
            // in the triple buffered case, we may still have a v-blank outstanding, which causes a race
            // condition between the increment read (on processing the v-blank) and the SetRWM below.
            // wait for the last v-blank here to prevent this

            // flush out all but one v-blank
            WaitForIdle(TRUE, FALSE);

            // get the current v-blank count
            NV_CFGEX_VBLANK_COUNTER_MULTI_PARAMS nvVBCounter;
            nvVBCounter.Head = GET_CURRENT_HEAD();
            NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_VBLANK_COUNTER_MULTI,
                &nvVBCounter, sizeof(nvVBCounter));

            // wait for one more v-blank
            DWORD dwFirstCount = nvVBCounter.VBlankCounter;
            do {
                NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_VBLANK_COUNTER_MULTI,
                    &nvVBCounter, sizeof(nvVBCounter));
            } while (nvVBCounter.VBlankCounter == dwFirstCount);
        }
#if (NVARCH >= 0x020) && defined(KELVIN_SEMAPHORES)
        if ((pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_FAMILY_KELVIN)   &&
            (pDriverData->nvD3DPerfData.dwNVClasses  & NVCLASS_007C_DAC)        &&
            (pDriverData->nvD3DPerfData.dwNVClasses1 & NVCLASS1_206E_CHANNELDMA))
        {
            getDC()->nvFlipper.init (lpCSData->dwSCnt);
        }
        else if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS)) {
            DPF_LEVEL (NVDBG_LEVEL_SURFACE, "Setting number of surfaces in primary flip chain to %d", lpCSData->dwSCnt);
            nvglSetNv15CelsiusSyncSetRWM (NV_DD_CELSIUS, 0, 1, lpCSData->dwSCnt)
        }
#elif (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS | NVCLASS_FAMILY_KELVIN)) {
            DPF_LEVEL (NVDBG_LEVEL_SURFACE, "Setting number of surfaces in primary flip chain to %d", lpCSData->dwSCnt);
            nvglSetNv15CelsiusSyncSetRWM (NV_DD_CELSIUS, 0, 1, lpCSData->dwSCnt)
        }
#elif (NVARCH >= 0x010)
        if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS)) {
            DPF_LEVEL (NVDBG_LEVEL_SURFACE, "Setting number of surfaces in primary flip chain to %d", lpCSData->dwSCnt);
            nvglSetNv15CelsiusSyncSetRWM (NV_DD_CELSIUS, 0, 1, lpCSData->dwSCnt)
        }
#endif
    }

#endif  // HWFLIP

    // figure out how much extra space is required for video post processing
    // dwExtraSize is the size of one workspace surface, for NV4/5 we need 4,
    // for NV10 we need 3
    DWORD dwExtraSize, dwExtraNumSurfaces, dwBlockHeight;
    LPDDHALINFO pHalInfo = GET_HALINFO();
    VppCalcExtraSurfaceSpace(&(pDriverData->vpp), pDDSLcl, pHalInfo->vmiData.dwDisplayHeight, lpCSData->dwSCnt, &dwExtraSize, &dwExtraNumSurfaces, &dwBlockHeight);

    for (int i = 0; i < int(lpCSData->dwSCnt); i++) {

        BOOL bRealSurface = FALSE;

        // traversing SList in the reverse order ensures that we use one of the two faster
        // memory banks in triple buffer, render to front buffer only tests
        pDDSLcl = lpCSData->lplpSList[bPrimaryFlipChain ? (lpCSData->dwSCnt - i - 1) : i];

#ifdef  STEREO_SUPPORT
        nvAssert((pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 & DDSCAPS2_STEREOSURFACELEFT) == 0);
        pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 &= ~DDSCAPS2_STEREOSURFACELEFT;
        if (STEREO_ENABLED)
        {
            if (bPrimaryFlipChain || pDDSLcl->ddsCaps.dwCaps &
               (DDSCAPS_BACKBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_ZBUFFER))
            {
                //We will emulate the stereo bit for internal use. Will clear it on exit.
                pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 |= DDSCAPS2_STEREOSURFACELEFT;
            }
        }
#endif  //STEREO_SUPPORT

        if (global.dwDXRuntimeVersion < 0x0700) {
            DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "CreateSurface: pDDGbl = %08x, pDDSLcl = %08x", pDDGbl, pDDSLcl);
        } else {
            DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "CreateSurface: pDDGbl = %08x, pDDSLcl = %08x, handle = %02x",
                      pDDGbl, pDDSLcl, pDDSLcl->lpSurfMore->dwSurfaceHandle);
        }

        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Texture Create:");
            lpCSData->ddRVal = nvTextureCreateSurface (pDDSLcl, pDDGbl, lpCSData->lpDDSurfaceDesc);
        }

        else if (pDDSLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT) {
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Surface With Format:");
            lpCSData->ddRVal = nvCreateSurfaceWithFormat (pDDSLcl, pDDGbl, dwExtraSize, dwExtraNumSurfaces, dwBlockHeight);
            bRealSurface     = TRUE;
        }

        else {
            DPF_LEVEL(NVDBG_LEVEL_SURFACEALLOC,"Surface Without Format:");
            lpCSData->ddRVal = nvCreateSurfaceWithoutFormat (pDDSLcl, pDDGbl);
            bRealSurface     = TRUE;
        }

#if (NVARCH >= 0x010)
        if (bRealSurface && (global.dwDXRuntimeVersion >= 0x0700)) {
            // If we have forced aa on, then dont return error here.  Simply continue without aa
            if (!nvCelsiusAACanCreate(pDDSLcl) && !(pDriverData->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_ANTIALIASFORCEENABLE_MASK))
            {
                lpCSData->ddRVal = DDERR_OUTOFVIDEOMEMORY;
                DPF("CreateSurface32: cannot create AA buffers");
            }
        }
#endif  // NVARCH >= 0x010

        if (global.dwDXRuntimeVersion < 0x0700)
        {
            // For DX6 runtime we're going to have to keep a pointer to a Local structure.
            // We will reuse the Object->Handle which isn't used in DX6.
            CNvObject *pNvObj = GET_PNVOBJ (pDDSLcl);
            // if this a surface that hasn't yet been G.U.D.ified, we won't have an NvObject for it
            if (IS_VALID_PNVOBJ(pNvObj)) {
                pNvObj->setHandle ((DWORD)pDDSLcl);
            }
        }

#ifdef  STEREO_SUPPORT
        pDDSLcl->lpSurfMore->ddsCapsEx.dwCaps2 &= ~DDSCAPS2_STEREOSURFACELEFT;
#endif  //STEREO_SUPPORT

        if (IS_OVERLAY(pDDSLcl->ddsCaps.dwCaps, pDDSLcl->lpGbl->ddpfSurface.dwFourCC)) {
            // overlay has succeeded, enable VPP
            if (i == 0 && !VppCreateOverlay(&pDriverData->vpp)) {
                lpCSData->ddRVal = DDERR_GENERIC;
            }
        }

        if (lpCSData->ddRVal != DD_OK) {
            if (IS_OVERLAY(pDDSLcl->ddsCaps.dwCaps, pDDSLcl->lpGbl->ddpfSurface.dwFourCC)) {
                VppDestroyOverlay(&pDriverData->vpp);
            }
#ifdef DEBUG
            switch (lpCSData->ddRVal) {
                case DDERR_OUTOFMEMORY:
                    DPF("CreateSurface32: Out of memory");
                    break;
                case DDERR_OUTOFVIDEOMEMORY:
                    DPF("CreateSurface32: Out of video memory");
                    break;
                default:
                    DPF("CreateSurface32: Encountered error %x", lpCSData->ddRVal);
                    break;
            }
#endif

            // roll back the allocations that succeeded
            DDHAL_DESTROYSURFACEDATA ddDSData;
            ddDSData.lpDD = lpCSData->lpDD;
            for (i=i-1; i>=0; i--) {
                ddDSData.lpDDSurface = lpCSData->lplpSList[bPrimaryFlipChain ? (lpCSData->dwSCnt - i - 1) : i];
                DestroySurface32 (&ddDSData);
            }

            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

#ifdef CAPTURE
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        if (bPrimaryFlipChain) {
            CAPTURE_FLIP_CHAIN chain;
            chain.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            chain.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            chain.dwExtensionID        = CAPTURE_XID_FLIP_CHAIN;
            chain.dwCount              = lpCSData->dwSCnt;
            captureLog (&chain, sizeof(chain));
            for (DWORD ii=0; ii<lpCSData->dwSCnt; ii++) {
                pDDSLcl = lpCSData->lplpSList[lpCSData->dwSCnt - ii - 1];
                DWORD dwSurfOffset = (GET_PNVOBJ(pDDSLcl))->getSimpleSurface()->getOffset();
                captureLog (&dwSurfOffset, sizeof(DWORD));
            }
        }
    }
#endif

#ifdef  NEW_FRONT_RENDER
    if (bPrimaryFlipChain) {
        if (lpCSData->dwSCnt == 2) {
            BackVidMem = lpCSData->lplpSList[1]->lpGbl->fpVidMem;
            pBackNvObj = GET_PNVOBJ (lpCSData->lplpSList[1]);
            lpCSData->lplpSList[1]->lpGbl->fpVidMem = lpCSData->lplpSList[0]->lpGbl->fpVidMem;
            SET_PNVOBJ(lpCSData->lplpSList[1], GET_PNVOBJ (lpCSData->lplpSList[0]));
        }
    }
#endif  //NEW_FRONT_RENDER

    if (IS_OVERLAY(pDDSLcl->ddsCaps.dwCaps, pDDSLcl->lpGbl->ddpfSurface.dwFourCC)) {
        if (pDDSLcl->lpGbl->wWidth < 640 && pDDSLcl->lpGbl->wHeight < 480) {
            // media player, quicktime, or realplayer
            pDriverData->vpp.dwOverlayFSSrcWidth = pDDSLcl->lpGbl->wWidth;
            pDriverData->vpp.dwOverlayFSSrcHeight = pDDSLcl->lpGbl->wHeight;
            pDriverData->vpp.dwOverlayFSSrcMinX = 0;
            pDriverData->vpp.dwOverlayFSSrcMinY = 0;
        } else {
            // DVD player, figure out the source bounding box dimensions later
            pDriverData->vpp.dwOverlayFSSrcWidth = 0;
            pDriverData->vpp.dwOverlayFSSrcHeight = 0;
            pDriverData->vpp.dwOverlayFSSrcMinX = 0xFFFFFFFF;
            pDriverData->vpp.dwOverlayFSSrcMinY = 0xFFFFFFFF;
        }

#ifdef VPEFSMIRROR
        VppCreateFSMirror(&(pDriverData->vpp), pDDSLcl->lpGbl->wWidth, pDDSLcl->lpGbl->wHeight);
        if (pDriverData->nvpeState.bVPEInitialized && (pDriverData->vpp.dwOverlayFSNumSurfaces > 0)) {
            NVPFsMirrorEnable(pDriverData);
        }
#else
        if(!(pDriverData->nvpeState.bVPEInitialized)) {
            VppCreateFSMirror(&(pDriverData->vpp), pDDSLcl->lpGbl->wWidth, pDDSLcl->lpGbl->wHeight);
        }
#endif
    }

    NvReleaseSemaphore(pDriverData);
    DDENDTICK(SURF4_CREATESURFACE);

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} // CreateSurface32

//---------------------------------------------------------------------------

// CanCreateSurface32
//
// Determine whether we support creation of a particular surface type in our HAL driver
//
// NOTES:
// This entry point is called after parameter validation but before
// any object creation.   You can decide here if it is possible for
// you to create this surface.  For example, if the person is trying
// to create an overlay, and you already have the maximum number of
// overlays created, this is the place to fail the call.
//
// You also need to check if the pixel format specified can be supported.
//
// lpCanCreateSurface->bIsDifferentPixelFormat tells us if the pixel format of the
// surface being created matches that of the primary surface.  It can be
// true for Z buffer and alpha buffers, so don't just reject it out of
// hand...

DWORD __stdcall CanCreateSurface32 (LPDDHAL_CANCREATESURFACEDATA lpCanCreateSurface)
{
    dbgTracePush ("CanCreateSurface32");

    if (lpCanCreateSurface->lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_EXECUTEBUFFER) {
        // silly hack for screwed up kernel calls
        DWORD dwRetVal;
        dwRetVal = D3DCanCreateExecuteBuffer32(lpCanCreateSurface);
        dbgTracePop();
        return dwRetVal;
    }


    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface, lpCanCreateSurface->lpDD=%08x", lpCanCreateSurface->lpDD );
    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    lpCanCreateSurface->lpDDSurfaceDesc=%08x", lpCanCreateSurface->lpDDSurfaceDesc );

    DDSTARTTICK(SURF4_CANCREATESURFACE);
    nvSetDriverDataPtrFromDDGbl (lpCanCreateSurface->lpDD);

    LPDDSURFACEDESC lpDDSurface = lpCanCreateSurface->lpDDSurfaceDesc;
    DWORD caps         = lpDDSurface->ddsCaps.dwCaps;
    DWORD dwModeWidth  = GET_MODE_WIDTH();
    DWORD dwModeHeight = GET_MODE_HEIGHT();
    DWORD dwModeBPP    = GET_MODE_BPP() >> 3;

    // Reset NV and get the monitor frequency after a mode reset
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {

        if (!nvEnable32 (lpCanCreateSurface->lpDD)) {
            DDENDTICK(SURF4_CREATESURFACE);
            NvReleaseSemaphore(pDriverData);
            lpCanCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }

    }

    if (caps & (DDSCAPS_MODEX | DDSCAPS_PALETTE)) {

        NvReleaseSemaphore(pDriverData);
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_NOTHANDLED);

    }

    if ((dwModeBPP == 1) && (caps & DDSCAPS_ALPHA)) {

        NvReleaseSemaphore(pDriverData);
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_NOTHANDLED);

    }

    if (caps & DDSCAPS_3DDEVICE) {

        if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16) && (lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS)) {
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - no 16bit alpha 3d render target");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
          }
    }

    if (caps & DDSCAPS_TEXTURE) {

        // Don't allow textures in 8bpp
        if (dwModeBPP == 1) {
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 130");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        // ??BOGUS??  Do we really want to accept all other textures?  What about bogus
        // FOURCC formats?  What about contradictory flags in 'caps'?
        // Partial answer: CreateSurface() does these checks, which fills the functional
        // need and probably keeps code simpler for now.

        NvReleaseSemaphore(pDriverData);
        lpDDSurface->ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
        lpDDSurface->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        lpCanCreateSurface->ddRVal = DD_OK;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    // check pixel format.   Don't allow pixel formats that aren't
    // the same, unless we have a valid fourcc code, an overlay,
    // an alpha surface, or z buffer.

    if (lpCanCreateSurface->bIsDifferentPixelFormat) {

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    different pixel format!" );

        if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_ALPHAPREMULT ) {

            if ((dwModeBPP == 1) || (dwModeBPP == 2)) {
                // We don't support alpha surfaces in 8bpp mode
                NvReleaseSemaphore(pDriverData);
                DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - no alpha surf in 8bpp");
                lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                DDENDTICK(SURF4_CANCREATESURFACE);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }

            if (dwModeBPP == 4) {
                if ((lpDDSurface->ddpfPixelFormat.dwRGBAlphaBitMask != 0xFF000000) ||
                    (lpDDSurface->ddpfPixelFormat.dwRBitMask != 0x00FF0000) ||
                    (lpDDSurface->ddpfPixelFormat.dwGBitMask != 0x0000FF00) ||
                    (lpDDSurface->ddpfPixelFormat.dwBBitMask != 0x000000FF)) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - unsupported format 10");
                    lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
                else {
                    NvReleaseSemaphore(pDriverData);
                    lpCanCreateSurface->ddRVal = DD_OK;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }
        }

        if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_FOURCC ) {

#ifdef WINNT
            // Don't allow NVDS surfaces to be created under NT or Win2K, since the
            // proprietary motion comp interface doesn't work there.
            if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVDS) {
                NvReleaseSemaphore(pDriverData);
                lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                DDENDTICK(SURF4_CANCREATESURFACE);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }
#endif

            if (dwModeBPP == 1) {
                // We don't support RGB FOURCCs in 8bpp mode
                if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_RGB0) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 10");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }

            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    FourCC requested (%4.4s, 0x%08x)",
                 (LPSTR) &lpDDSurface->ddpfPixelFormat.dwFourCC,
                 lpDDSurface->ddpfPixelFormat.dwFourCC );

            for (DWORD i=0; i < NV_MAX_FOURCC; i++) {

                if ( lpDDSurface->ddpfPixelFormat.dwFourCC == pDriverData->fourCC[i] ) {

                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    FOURCC=%4.4s", (LPSTR) &pDriverData->fourCC[i] );

                    // ??BOGUS??  Won't this test always fail?  The same test higher up seems
                    // to catch all cases.
                    if (caps & DDSCAPS_TEXTURE) {

#ifdef TEXFORMAT_CRD // --------------------------------

                        lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;

                        if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
                            &&
                            ((getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_NVXN) ||
                             (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_NVXN))
                            &&
                            ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT0) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT1) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT2) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT3) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT4) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT5) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS0) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS1) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS2) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS3) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS4) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS5))
                           )
                        {
                            // accept
                            lpCanCreateSurface->ddRVal = DD_OK;
                        }

                        if (((getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_HILO) ||
                             (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_HILO))
                            &&
                            ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVHS) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVHU))
                           )
                        {
                            // accept
                            lpCanCreateSurface->ddRVal = DD_OK;
                        }

                        if (((getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_DXT) ||
                             (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_DXT))
                            &&
                            ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT1) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT2) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT3) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT4) ||
                             (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT5))
                           )
                        {
                            // accept
                            lpCanCreateSurface->ddRVal = DD_OK;
                        }

                        if (lpCanCreateSurface->ddRVal != DD_OK) {
                            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - unsupported texture format (via FOURCC)");
                        }

#else // !TEXFORMAT_CRD  --------------------------------

                        // allow only known texture fourcc formats
                        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                            if ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT0)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT1)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT2)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT3)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT4)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVT5)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS0)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS1)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS2)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS3)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS4)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVS5)
#if defined(HILO_SUPPORT_DX7) || defined(HILO_SUPPORT_DX8)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVHS)
                             || (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVHU)
#endif
                             )
                            {
                                // accept
                                lpCanCreateSurface->ddRVal = DD_OK;
                            }
                        }
#ifdef DXT_SUPPORT
                        if (((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTURECOMPRESSIONENABLE_MASK) ==
                               D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE)
                              &&
                              ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT1) ||
                               (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT2) ||
                               (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT3) ||
                               (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT4) ||
                               (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_DXT5))
                            )
                        {
                            // accept
                            lpCanCreateSurface->ddRVal = DD_OK;
                        } else
#endif
                        {
                            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - unsupported texture format (via FOURCC)");
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                        }

#endif // !TEXFORMAT_CRD  --------------------------------

                        NvReleaseSemaphore(pDriverData);
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_HANDLED);
                    }

                    else if (caps & DDSCAPS_OVERLAY) {

                        if ((lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYVY) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYNV) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUY2) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUNV) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YV12) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_NV12) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_NVDS) &&
//                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_420i) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IF09) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YVU9) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IV32) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IV31)) {
                            NvReleaseSemaphore(pDriverData);
                            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - unsupported format 20");
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            DDENDTICK(SURF4_CANCREATESURFACE);
                            dbgTracePop();
                            return (DDHAL_DRIVER_HANDLED);
                        }

                        if ((caps & (DDSCAPS_VIDEOPORT | DDSCAPS_FLIP)) == (DDSCAPS_VIDEOPORT | DDSCAPS_FLIP)) {
                            // Surface is for overlay with flip associated with video port
                            // check number of buffers, can't have more than 5 surfaces
                            // This will probably change when we have kernel mode flipping support
                            if ((lpDDSurface->dwFlags & DDSD_BACKBUFFERCOUNT) &&
                                (lpDDSurface->dwBackBufferCount > 4)) {
                                NvReleaseSemaphore(pDriverData);
                                DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid parameters 10");
                                lpCanCreateSurface->ddRVal = DDERR_INVALIDPARAMS;
                                DDENDTICK(SURF4_CANCREATESURFACE);
                                dbgTracePop();
                                return (DDHAL_DRIVER_HANDLED);
                            }
                        }
                        else {
                            // Workaround for Microsoft DirectVideo Cinepak decoding bug
                            // The reason for the existence of FOURCC_UYNV and FOURCC_YUNV
//                                              if ((lpDDSurface->dwFlags & DDSD_BACKBUFFERCOUNT) &&
//                                (lpDDSurface->dwBackBufferCount > 1) &&
//                                ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_UYVY) ||
//                                 (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YUY2))) {
//                                                      lpCanCreateSurface->ddRVal = DDERR_INVALIDPARAMS;
//                                dbgTracePop();
//                                return DDHAL_DRIVER_HANDLED;
//                                          }

                            // We must limit number of YV12 overlay surfaces to the maximum number we can manage reformatting
                            if ((lpDDSurface->dwFlags & DDSD_BACKBUFFERCOUNT) &&
                                (lpDDSurface->dwBackBufferCount > 3) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YV12)) {
                                NvReleaseSemaphore(pDriverData);
                                DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - out of caps 10");
                                lpCanCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                                DDENDTICK(SURF4_CANCREATESURFACE);
                                dbgTracePop();
                                return (DDHAL_DRIVER_HANDLED);
                            }
                        }

                        if (VppIsOverlayActive(&pDriverData->vpp))
                        {
                            NvReleaseSemaphore(pDriverData);
                            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - out of caps 20");
                            lpCanCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                            DDENDTICK(SURF4_CANCREATESURFACE);
                            dbgTracePop();
                            return (DDHAL_DRIVER_HANDLED);
                        }

                        // NB: Do not call VppGetVideoScalerBandwidthStatus() in CanCreateSurface() because
                        //     on some modesets (etc) Ddraw will re-create surfaces by ONLY calling CreateSurface().
                    }

                    else if (caps & (DDSCAPS_VIDEOMEMORY | DDSCAPS_OFFSCREENPLAIN)) {

                        // only support certain FOURCC formats
                        switch ( lpDDSurface->ddpfPixelFormat.dwFourCC )
                        {
                        // These are the formats we like.  They're supported in VppCreateFourCCSurface(),
                        // which seems to be the only routine that allocs a non-texture FOURCC surface.
                        //
                        // I've commented out a few for the following reasons (from previous check-ins):
                        //     Front end YUV support cannot reformat data so Indeo is not supported
                        //     Currently also disallowing front end YUV420 since YUYV or UYVY
                        //     should do ok when not enough backend bandwidth is available.
                        case FOURCC_RGB0:
                        case FOURCC_RAW8:
                      //case FOURCC_IF09:
                      //case FOURCC_YVU9:
                      //case FOURCC_IV32:
                      //case FOURCC_IV31:
#if IS_WINNT5 || IS_WIN9X
                        case FOURCC_NV12: // Our motion comp surface format
                        case FOURCC_NVMC: // Our motion comp control surface, NV10 or greater
                        case FOURCC_NVDS:
                        case FOURCC_NVSP: // A subpicture surface (overlay)
                        case FOURCC_NVID:
#endif
                        case FOURCC_YUY2:
                        case FOURCC_YUNV:
                        case FOURCC_UYVY:
                        case FOURCC_UYNV:
                      //case FOURCC_YV12:
                      //case FOURCC_420i:
                            // no-op -- we're happy if we get here
                            //--?? Why not return here?
                            break;

                        default:
                            // we can't/won't support anything but what's listed above
                            NvReleaseSemaphore(pDriverData);
                            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - unsupported format 30");
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            DDENDTICK(SURF4_CANCREATESURFACE);
                            dbgTracePop();
                            return (DDHAL_DRIVER_HANDLED);
                        }

                        if (caps & DDSCAPS_VIDEOPORT) {
                            // Nonoverlay FOURCC video surfaces are only supported for YUV422
                            // formats and only when associated with the video port
                            if ((lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUY2) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUNV) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYVY) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYNV) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_RAW8)
                               ) {
                                NvReleaseSemaphore(pDriverData);
                                DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - unsupported format 40");
                                lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                                DDENDTICK(SURF4_CANCREATESURFACE);
                                dbgTracePop();
                                return (DDHAL_DRIVER_HANDLED);
                            }
                            // Front end YUV support not available in 8bit indexed mode
                        }
                        else if (dwModeBPP == 1) {
                            if (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_RAW8) {
                                NvReleaseSemaphore(pDriverData);
                                DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - unsupported format 50");
                                lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                                DDENDTICK(SURF4_CANCREATESURFACE);
                                dbgTracePop();
                                return (DDHAL_DRIVER_HANDLED);
                            }
                        }
                    }

                    if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_RGB0) {
                        if (((dwModeBPP == 2) && (lpDDSurface->ddpfPixelFormat.dwRGBBitCount != DDBD_16)) ||
                            ((dwModeBPP == 4) && (lpDDSurface->ddpfPixelFormat.dwRGBBitCount != DDBD_32))) {
                            NvReleaseSemaphore(pDriverData);
                            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 20");
                            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                            DDENDTICK(SURF4_CANCREATESURFACE);
                            dbgTracePop();
                            return (DDHAL_DRIVER_HANDLED);
                        }
                    }

                    NvReleaseSemaphore(pDriverData);
                    lpCanCreateSurface->ddRVal = DD_OK;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }

                else { // Check for known "hidden" FOURCC formats
                    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                        if ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVMC) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVDS) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NV12) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVID) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVSP)) {

                            if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NVMC) {
                                if (pDriverData->vpp.dwOverlaySurfaces > 0) {
                                    NvReleaseSemaphore(pDriverData);
                                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - out of caps 21");
                                    lpCanCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                                    DDENDTICK(SURF4_CANCREATESURFACE);
                                    dbgTracePop();
                                    return (DDHAL_DRIVER_HANDLED);
                                }
                            }

                            if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_NV12) {
                                if (VppGetVideoScalerBandwidthStatus(&pDriverData->vpp, lpCanCreateSurface->lpDDSurfaceDesc->dwWidth) == FALSE) {
                                    NvReleaseSemaphore(pDriverData);
                                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - no overlay hardware 10");
                                    lpCanCreateSurface->ddRVal = DDERR_NOOVERLAYHW;
                                    DDENDTICK(SURF4_CANCREATESURFACE);
                                    dbgTracePop();
                                    return (DDHAL_DRIVER_HANDLED);
                                }
                            }

                            NvReleaseSemaphore(pDriverData);
                            lpCanCreateSurface->ddRVal = DD_OK;
                            DDENDTICK(SURF4_CANCREATESURFACE);
                            dbgTracePop();
                            return (DDHAL_DRIVER_HANDLED);
                        }
                    }
                }
            }
        }

        else if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_RGB ) {

            // Don't allow 3D devices in 8bpp
            if (caps & DDSCAPS_3DDEVICE) {
                if (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 8) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 30");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }

            // ??BOGUS??  Won't the texture bit always be zero?  It appears that all texture
            // cases are handled higher up.
            if (caps & DDSCAPS_TEXTURE || caps & DDSCAPS_OFFSCREENPLAIN) {

                // Don't allow textures in 8bpp.
                if (dwModeBPP == 1) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 40");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_NOTHANDLED);
                }

                // accept 8 bit palettized textures
                // do I need to be more specific like DDPF_RGB also???
                // supposedly palette creation should be allowed here
                // when I set the Palette to Surface I need to check
                // matching bit depths there.
                if ((lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
#ifdef TEXFORMAT_CRD
                    &&
                    ((getDC()->nvD3DRegistryData.regSurfaceFormatsDX7 & D3D_REG_SURFACEFORMATS_PALETTIZED) ||
                     (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED))
                   )
#else   // !TEXFORMAT_CRD
                 && ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_PALETTEENABLE_MASK) == D3D_REG_PALETTEENABLE_ENABLE))
#endif  // !TEXFORMAT_CRD
                {
                    lpCanCreateSurface->ddRVal = DD_OK;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    NvReleaseSemaphore(pDriverData);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }

                // accept all 16 and 32 bpp texture formats
                if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16) ||
                    (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 32)) {
                    NvReleaseSemaphore(pDriverData);
                    lpCanCreateSurface->ddRVal = DD_OK;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }

            if (caps & DDSCAPS_ZBUFFER) {
                if (getDC()->nvD3DRegistryData.regValidateZMethod == D3D_REG_VALIDATEZMETHOD_STRICT) {
                    // For now, support z buffers in video memory only when our primary rendering depth is 16 bpp
                    if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16) && (dwModeBPP == 2)) {
                        // accept all 16 bpp z buffer formats when in 16 bpp disply mode
                        NvReleaseSemaphore(pDriverData);
                        lpCanCreateSurface->ddRVal = DD_OK;
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_HANDLED);
                    }
                    if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 32) && (dwModeBPP == 4)) {
                        // accept all 32 bpp z buffer formats when in 32 bpp disply mode
                        NvReleaseSemaphore(pDriverData);
                        lpCanCreateSurface->ddRVal = DD_OK;
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_HANDLED);
                    }
                    // Any other z buffer depth or combination of z/display surface depths
                    // are invalid.
                    NvReleaseSemaphore(pDriverData);
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_NOTHANDLED);
                }
                else {
                    // Allocate the z-buffer with the proper padding based on the bit depth of the primary surface
                    // accept all 16, 24, 32 bpp z buffer formats when in 16 bpp disply mode
                    if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16)
                        || (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 24)
                        || (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 32)) {
                        NvReleaseSemaphore(pDriverData);
                        lpCanCreateSurface->ddRVal = DD_OK;
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_HANDLED);
                    }
                    // Any other z buffer depth or combination of z/display surface depths
                    // are invalid.
                    NvReleaseSemaphore(pDriverData);
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_NOTHANDLED);
                }
            }

            // We don't support any other RGB surfaces not in our native format
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 50");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);

        }

        else if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER ) {

            // Don't allow z-buffer surfaces in 8bpp
            if (caps & DDSCAPS_ZBUFFER) {
                if (dwModeBPP == 1) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 60");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_NOTHANDLED);
                }
                if (getDC()->nvD3DRegistryData.regValidateZMethod == D3D_REG_VALIDATEZMETHOD_STRICT) {
                    if ((lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != (dwModeBPP << 3))) {
                        NvReleaseSemaphore(pDriverData);
                        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 70");
                        lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_NOTHANDLED);
                    }
                }
                else {
                    if ((lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != 16)
                        && (lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != 24)
                        && (lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != 32)) {
                        NvReleaseSemaphore(pDriverData);
                        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 80");
                        lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_NOTHANDLED);
                    }
                }
            }

            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "Z BUFFER OK!" );
            lpCanCreateSurface->ddRVal = DD_OK;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);

        }

        // can't handle any other kinds of different fourcc or RGB overlays
        NvReleaseSemaphore(pDriverData);
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 90");
        lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

    if (caps & DDSCAPS_MODEX) {

        NvReleaseSemaphore(pDriverData);
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Mode X requested" );
        lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDMODE;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

    // Don't allow 3D devices in 8bpp (except for primary surface access, fixes Jedi Knight)
    if ((caps & DDSCAPS_3DDEVICE) && (!(caps & DDSCAPS_PRIMARYSURFACE))) {

        if (dwModeBPP == 1) {
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 100");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

    }

    // Although the DD API passes DWORD height and width, the internal structures
    // of DDRAW only hold 16 bits.  So we refuse all surfaces that exceed that limit.
    if ((lpDDSurface->dwFlags & DDSD_HEIGHT) && (lpDDSurface->dwHeight > 0xFFFF))
    {
        NvReleaseSemaphore(pDriverData);
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - height too big 105");
        lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    if ((lpDDSurface->dwFlags & DDSD_WIDTH) && (lpDDSurface->dwWidth > 0xFFFF))
    {
        NvReleaseSemaphore(pDriverData);
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - width too big 105");
        lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    if (caps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE)) {

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Frontbuffer requested" );

        if (caps & DDSCAPS_OVERLAY) {
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 110");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        if (caps & DDSCAPS_FLIP) {
            if (lpDDSurface->dwHeight > (unsigned long)dwModeHeight) {
                if (lpDDSurface->dwFlags & DDSD_HEIGHT) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - height too big 10");
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
                else {
                    lpDDSurface->dwHeight = (unsigned long)dwModeHeight;
                }

            }
            if (lpDDSurface->dwWidth > (unsigned long)dwModeWidth) {
                if (lpDDSurface->dwFlags & DDSD_WIDTH) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - width too big 10");
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
                else {
                    lpDDSurface->dwWidth = (unsigned long)dwModeWidth;
                }
            }
        }
        NvReleaseSemaphore(pDriverData);
        lpCanCreateSurface->ddRVal = DD_OK;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

    else if (caps & DDSCAPS_BACKBUFFER) {

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Backbuffer requested" );
        if (caps & DDSCAPS_OVERLAY) {
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 120");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        if (caps & DDSCAPS_FLIP) {
            if (lpDDSurface->dwHeight > (unsigned long)dwModeHeight) {
                if (lpDDSurface->dwFlags & DDSD_HEIGHT) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - height too big 20");
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
                else {
                    lpDDSurface->dwHeight = (unsigned long)dwModeHeight;
                }

            }
            if (lpDDSurface->dwWidth > (unsigned long)dwModeWidth) {
                if (lpDDSurface->dwFlags & DDSD_WIDTH) {
                    NvReleaseSemaphore(pDriverData);
                    DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - width too big 20");
                    lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                    DDENDTICK(SURF4_CANCREATESURFACE);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
                else {
                    lpDDSurface->dwWidth = (unsigned long)dwModeWidth;
                }
            }
        }
        NvReleaseSemaphore(pDriverData);
        lpCanCreateSurface->ddRVal = DD_OK;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

    else if (caps & DDSCAPS_OFFSCREENPLAIN) {

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Can create offscreen requested" );

        if (caps & DDSCAPS_VIDEOMEMORY) {
            if (caps & DDSCAPS_OVERLAY) {
                NvReleaseSemaphore(pDriverData);
                DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 140");
                lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                DDENDTICK(SURF4_CANCREATESURFACE);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }

            if (caps & DDSCAPS_FLIP) {
                if (lpDDSurface->dwHeight > (unsigned long)dwModeHeight) {
                    if (lpDDSurface->dwFlags & DDSD_HEIGHT) {
                        NvReleaseSemaphore(pDriverData);
                        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - height too big 30");
                        lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_HANDLED);
                    }
                    else {
                        lpDDSurface->dwHeight = (unsigned long)dwModeHeight;
                    }
                }
                if (lpDDSurface->dwWidth > (unsigned long)dwModeWidth) {
                    if (lpDDSurface->dwFlags & DDSD_WIDTH) {
                        NvReleaseSemaphore(pDriverData);
                        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - width too big 30");
                        lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                        DDENDTICK(SURF4_CANCREATESURFACE);
                        dbgTracePop();
                        return (DDHAL_DRIVER_HANDLED);
                    }
                    else {
                        lpDDSurface->dwWidth = (unsigned long)dwModeWidth;
                    }

                }
            }
        }
        NvReleaseSemaphore(pDriverData);
        lpCanCreateSurface->ddRVal = DD_OK;
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Can create offscreen granted" );
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

    else if (caps & (DDSCAPS_FLIP | DDSCAPS_VIDEOMEMORY | DDSCAPS_ZBUFFER)) {

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Can create flipping surface requested" );

        if (caps & DDSCAPS_OVERLAY) {
            NvReleaseSemaphore(pDriverData);
            DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 150");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            DDENDTICK(SURF4_CANCREATESURFACE);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
        if (caps & DDSCAPS_ZBUFFER) {
            if ((dwModeBPP != 2) && (dwModeBPP != 4)) {
                NvReleaseSemaphore(pDriverData);
                DPF_LEVEL(NVDBG_LEVEL_SURFACE, "CanCreateSurface32 - invalid pixel format 160");
                lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                DDENDTICK(SURF4_CANCREATESURFACE);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }
        }
        NvReleaseSemaphore(pDriverData);
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Can create flipping surface available" );
        lpCanCreateSurface->ddRVal = DD_OK;
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

    else if (caps & DDSCAPS_SYSTEMMEMORY) {

        NvReleaseSemaphore(pDriverData);
        lpCanCreateSurface->ddRVal = DD_OK;
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Can create system memory requested" );
        DDENDTICK(SURF4_CANCREATESURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);

    }

    NvReleaseSemaphore(pDriverData);
    DDENDTICK(SURF4_CANCREATESURFACE);
    dbgTracePop();
    return (DDHAL_DRIVER_NOTHANDLED);

} // CanCreateSurface32

//---------------------------------------------------------------------------

// DestroySurface32

DWORD __stdcall DestroySurface32 (LPDDHAL_DESTROYSURFACEDATA lpDestroySurface)
{
    dbgTracePush ("DestroySurface32");

    if (lpDestroySurface->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_EXECUTEBUFFER )
    {
        //silly hack for screwed up kernel calls
        DWORD dwRetVal;
        dwRetVal = D3DDestroyExecuteBuffer32(lpDestroySurface);
        dbgTracePop();
        return dwRetVal;
    }

    DDSTARTTICK(SURF4_DESTROYSURFACE);
    nvSetDriverDataPtrFromDDGbl (lpDestroySurface->lpDD);

    LPDDRAWI_DIRECTDRAW_GBL   pDDGbl  = lpDestroySurface->lpDD;
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl = lpDestroySurface->lpDDSurface;
    LPDDRAWI_DDRAWSURFACE_GBL pDDSGbl = pDDSLcl->lpGbl;

    DWORD dwIndex;


#ifdef  NEW_FRONT_RENDER
    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER)
    {
        pDDSLcl->lpGbl->fpVidMem = BackVidMem;
        SET_PNVOBJ(pDDSLcl, pBackNvObj);
    }
#endif  //NEW_FRONT_RENDER
    if (global.dwDXRuntimeVersion < 0x700) {
        DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "DestroySurface: pDDGbl = %08x, pDDSLcl = %08x", pDDGbl, pDDSLcl);
    } else {
        DPF_LEVEL(NVDBG_LEVEL_SURFACE_ACTIVITY, "DestroySurface: pDDGbl = %08x, pDDSLcl = %08x, handle = %02x",
            pDDGbl, pDDSLcl, pDDSLcl->lpSurfMore->dwSurfaceHandle);
    }

    // textures do not want to wait for idle. check for them here
    if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {
        // Call the D3D driver to destroy the texture surface.
        nvTextureDestroySurface (lpDestroySurface, pDDSLcl);
        NvReleaseSemaphore(pDriverData);
        DDENDTICK(SURF4_DESTROYSURFACE);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }


    if(!pDriverData->vpp.dwOverlaySurfaces &&
        (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) &&
        (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE)) {
        //clear FSDOSSSTATUS_RECOVERYNEEDED_DDRAW flag if not overlay surface exists
        pDriverData->dwFullScreenDOSStatus &=  ~FSDOSSTATUS_RECOVERYNEEDED_DDRAW;

    }


    if ((pDriverData->lpProcessInfoHead) && (getDC()->nvPusher.isValid())) {
        WaitForIdle(TRUE, TRUE);
    }

    // clear ring0 flipping flag to make sure new surfaces aren't tagged for ring0 flipping
    if (IS_OVERLAY(pDDSLcl->ddsCaps.dwCaps, pDDSLcl->lpGbl->ddpfSurface.dwFourCC))
    {

        if((pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) &&
           (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE)) {
            //clear FSDOSSSTATUS_RECOVERYNEEDED_D3D flag
            nvD3DRecover();
        }
        // Should check if YUV surface is an active VPE surface and make sure
        // to stop VPE hardware before destroying VPE surface somewhere in YUV
        // surface code in this function.

        if (VppIsOverlayActive(&pDriverData->vpp))
        {
            NvNotification *pPioFlipOverlayNotifier = pDriverData->pPioFlipOverlayNotifierFlat;
            LPPROCESSINFO lpProcInfo = pmGetProcess(pDriverData, GetCurrentProcessId());

            if (!(pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {
                // make sure we sync with other channels before writing put
                getDC()->nvPusher.setSyncChannelFlag();

                pDriverData->dwRingZeroMutex = TRUE;

                if (getDC()->nvPusher.isValid()) {
                    Sleep(VPP_TIMEOUT_TIME * 3);
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                    nvPushData(1, NV_VPP_OVERLAY_IID);
                    nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_STOP_OVERLAY(0) | 0x80000);
                    nvPushData(3, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[0]
                    nvPushData(4, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[1]
                    nvPusherAdjust(5);
                    nvPusherStart(TRUE);
                }
            }

            pDriverData->dwRingZeroMutex = FALSE;
            pDriverData->dDrawSpareSubchannelObject = 0;

            // Only do recover if a DOS full screen switch has completed.
            if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
                if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
                nvDDrawRecover();
                }
            }
        }

#ifdef NVPE
        NVPCleanUp(pDriverData, pDDGbl);
#else
        pDriverData->bRing0FlippingFlag = FALSE;
#endif // NVPE

        pDriverData->vpp.dwOverlayLastVisibleSurfaceLCL = NULL;

        if (pDriverData->vpp.dwOverlayFSNumSurfaces){
            VppDestroyFSMirror(&(pDriverData->vpp));
#ifdef VPEFSMIRROR
            if (pDriverData->nvpeState.bFsMirrorEnabled) {
                NVPFsMirrorDisable(pDriverData);
            }
#endif
        }

        //destroy overlay only when it is active and for the last overly
        //surface.
        if (VppIsOverlayActive(&pDriverData->vpp)
            && (pDriverData->vpp.dwOverlaySurfaces <= 1)){
            VppDestroyOverlay(&(pDriverData->vpp));
        }
    }

    // if there's an NvObject associated with this DDSLcl, remove its entry from
    // our list of handles. (this is because assinine apps will later reference
    // handles to surfaces they've previously destroyed)
    CNvObject *pObj = GET_PNVOBJ (pDDSLcl);

    if ((((DWORD)pObj) & MASK_FLOATING_CONTEXT_DMA_ID) == FLOATING_CONTEXT_DMA_ID) {
        // DX7 doesn't always call CreateSurfaceEx, in these cases we may be storing
        // a floating system memory DMA context at this location
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, ((DWORD)pObj));
        SET_PNVOBJ(pDDSLcl, NULL);
        pObj = NULL;
    }

    // Don't exit early for Primary surfaces on surfaces that aren't really Primary.
    // In Win2K fpVidMem and GET_PRIMARY_ADDR() may both be zero if an Overlay
    // or other FourCC surface create fails.
    if (!((pDDSLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
        (pDDSGbl->ddpfSurface.dwFlags & DDPF_FOURCC))) {

        if (pDDSGbl->fpVidMem == GET_PRIMARY_ADDR()) {
#ifdef WINNT
            ppdev->bD3DappIsRunning = FALSE;
#endif
            lpDestroySurface->ddRVal = DD_OK;
           // Need to tell NT displaydriver that we destroyed a PRIMARY surface
            NV_OGLDDRAWSURFACEDESTROYED(ppdev);
            goto Handled_Exit;
        }
    }

    // Mode switches might occur without warning
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED)
    {
        if (!nvEnable32 (lpDestroySurface->lpDD)) {
            lpDestroySurface->ddRVal = DDERR_OUTOFMEMORY;
            goto Handled_Exit;
        }
    }

    if ((pDDSLcl->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
        (pDDSGbl->ddpfSurface.dwFlags & DDPF_FOURCC)) {

        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "  FOURCC surface" );

        // destroy shared system workspace
        if (IS_OVERLAY(pDDSLcl->ddsCaps.dwCaps, pDDSLcl->lpGbl->ddpfSurface.dwFourCC) &&
            (pDriverData->vpp.dwOverlaySurfaces <= 1))
        {
            // tells the ring0 code to not run any more!
            pDriverData->vpp.regRing0ColourCtlInterlockFlags = pDriverData->vpp.regRing0ColourCtlInterlockFlags & 0xFFFFFFFE;
            {
                DWORD aCount = 16;
                while((pDriverData->vpp.regRing0ColourCtlInterlockFlags & 0x02) && aCount-- ) {
                    Sleep(1);  // waits 1 millisecond
                }
            }
            if (pDriverData->vpp.fpOverlayShadow)
            {
                NvRmFree(pDriverData->dwRootHandle, pDriverData->vpp.hChannel,
                         NV_DD_PIO_FLOATING_CONTEXT_DMA_IN_OVERLAY_SHADOW);
                FreeIPM((void*)pDriverData->vpp.fpOverlayShadow);
                pDriverData->vpp.fpOverlayShadow = 0;
            }

            pDriverData->dDrawSpareSubchannelObject = 0;
        }

        switch ( pDDSLcl->lpGbl->ddpfSurface.dwFourCC ) {
            case FOURCC_RGB0:
            case FOURCC_RAW8:
                if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
                    if (pDDSGbl->fpVidMem > 0) {
                        NVHEAP_FREE (pDDSGbl->fpVidMem);
                        pDDSGbl->fpVidMem = 0;
                        if (pDriverData->DDrawVideoSurfaceCount > 0)
                            pDriverData->DDrawVideoSurfaceCount--;
                    }
                }
                else {
                    HGLOBAL hMem = (HGLOBAL)pDDSGbl->dwReserved1;
                    GlobalFree(hMem);
                    pDDSGbl->dwReserved1 = 0;
                }
                lpDestroySurface->ddRVal = DD_OK;
                goto Handled_Exit;
                break;

            case FOURCC_NVMC: // Our Win9x motion comp control surface, only on NV10 or greater
                if (pDDSGbl->fpVidMem > 0) {
                    NVHEAP_FREE (pDDSGbl->fpVidMem);
                    pDDSGbl->fpVidMem = 0;
                    if (pDriverData->DDrawVideoSurfaceCount > 0)
                        pDriverData->DDrawVideoSurfaceCount--;
                }

                pDDSGbl->dwReserved1 = 0;

                pDriverData->dwMCNVMCSurface = 0;

                lpDestroySurface->ddRVal = DD_OK;
                goto Handled_Exit;
                break;

            case FOURCC_NVDS: // Our Win9x Direct Show dummy motion comp surface, only on NV10 or greater
                if ((pDriverData->dwMCNVDSSurfaceBase != 0) && (pDDSGbl->fpVidMem > 0)) {
                    pDriverData->dwMCNVDSSurfaceBase = 0;
                    NVHEAP_FREE (pDDSGbl->fpVidMem);
                }

                pDDSGbl->fpVidMem = 0;

                if (pDriverData->DDrawVideoSurfaceCount > 0)
                    pDriverData->DDrawVideoSurfaceCount--;

                pDDSGbl->dwReserved1 = 0;

                lpDestroySurface->ddRVal = DD_OK;
                goto Handled_Exit;
                break;

            case FOURCC_NVSP: // a subpicture surface (overlay)
                if (pDDSGbl->fpVidMem > 0) {
                    NVHEAP_FREE (pDDSGbl->fpVidMem);
                    pDDSGbl->fpVidMem = 0;
                    if (pDriverData->DDrawVideoSurfaceCount > 0)
                        pDriverData->DDrawVideoSurfaceCount--;
                }

                pDDSGbl->dwReserved1 = 0;

                lpDestroySurface->ddRVal = DD_OK;
                goto Handled_Exit;
                break;

            case FOURCC_NVID: // These surfaces can exist only on NV10 or greater
                // Send a sync to make sure we are done with it

                if (!(pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {
                    // Let D3D code know that we have touched NV
                    pDriverData->TwoDRenderingOccurred = 1;

                    getDC()->pRefCount->inc (CReferenceCount::INC_IMMEDIATE);

                    // Make sure celsius class isn't currently referencing an IDCT context Dma
                    if (getDC()->nvPusher.isValid()) {
                        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                          NV097_SET_CONTEXT_DMA_B | 0x40000);
                            nvPushData(1, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // Reset to what D3D expects to see
                            nvPusherAdjust(2);
                            nvPusherStart(TRUE);
                        } else {
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                          NV056_SET_CONTEXT_DMA_B | 0x40000);
                            nvPushData(1, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY); // Reset to what D3D expects to see
                            nvPusherAdjust(2);
                            nvPusherStart(TRUE);
                        }
                        pDriverData->dwMostRecentHWUser = MODULE_ID_DDRAW;
                    }
                    // wait for HW to catch up
                    getDC()->pRefCount->wait (CReferenceCount::WAIT_PREEMPTIVE);
                }

                // Clear current IDCT context Dma
                pDriverData->dwMCCurrentCelsiusIDCTCtxDma = 0;

                for (dwIndex=0; dwIndex<4; dwIndex++) {
                    if (pDDSGbl->fpVidMem == pDriverData->dwMCIDCTAGPSurfaceBase[dwIndex]) {
                        pDriverData->dwMCIDCTAGPSurfaceBase[dwIndex] = 0;
                        if (pDriverData->dwMCIDCTAGPCtxDmaSize[dwIndex] != 0) {
                            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                                     NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+dwIndex);
                            pDriverData->dwMCIDCTAGPCtxDmaSize[dwIndex] = 0;
                            pDriverData->dwMCCurrentCelsiusIDCTCtxDma = 0;
                        }
                    }
                }

                // First check if surface is in video memory
                if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM) {
                    if (pDDSGbl->fpVidMem > 0) {
                        // free memory using initial unadjusted pointer
                        NVHEAP_FREE (pDDSGbl->dwReserved1);
                        pDDSGbl->fpVidMem = 0;
                        if (pDriverData->DDrawVideoSurfaceCount > 0)
                            pDriverData->DDrawVideoSurfaceCount--;
                    }
                } else {
                    // this should never happen
                    nvAssert(0);
                }

                pDDSGbl->dwReserved1 = 0;

                lpDestroySurface->ddRVal = DD_OK;
                goto Handled_Exit;
                break;

            case FOURCC_IF09:
            case FOURCC_YVU9:
            case FOURCC_IV32:
            case FOURCC_IV31:
            case FOURCC_YUY2:
            case FOURCC_YUNV:
            case FOURCC_UYVY:
            case FOURCC_UYNV:
            case FOURCC_NV12:
            case FOURCC_YV12:
                if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                    DWORD dwIndex = 0;
                    if (pDriverData->vpp.dwOverlaySurfaces > 0)
                        pDriverData->vpp.dwOverlaySurfaces--;
                    if (pDriverData->vpp.dwOverlaySurfaces == 0) {
                        pDriverData->vpp.dwOverlayOwner = 0;
                        for (DWORD i = 0; i < NV_VPP_MAX_EXTRA_SURFACES; i++) {
                            pDriverData->vpp.extraOverlayOffset[i] = 0;
                        }
                        pDriverData->vpp.extraNumSurfaces = 0;
                        pDriverData->bMCOverlaySrcIsSingleField1080i = FALSE;
                        pDriverData->bMCHorizontallyDownscale1080i = FALSE;
                    }
                    while (dwIndex < NV_VPP_MAX_OVERLAY_SURFACES) {
                        if ((DWORD)pDDSLcl == pDriverData->vpp.dwOverlaySurfaceLCL[dwIndex]) {
                            pDriverData->vpp.dwOverlaySurfaceLCL[dwIndex] = 0;
                        }
                        dwIndex++;
                    }
                }

                if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) &&
                    ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) == 0)) {
                    if (pDDSGbl->fpVidMem > 0) {
                        if (pDDSGbl->fpVidMem + pDDSGbl->dwReserved1 - pDriverData->BaseAddress ==
                            pDriverData->vpp.extraOverlayOffset[0]) {
                            for (DWORD i = 0; i < NV_VPP_MAX_EXTRA_SURFACES; i++) {
                                pDriverData->vpp.extraOverlayOffset[i] = 0;
                            }
                            pDriverData->vpp.extraNumSurfaces = 0;
                        }
                        NVHEAP_FREE (pDDSGbl->fpVidMem);
                        pDDSGbl->fpVidMem = 0;
                        if (pDriverData->DDrawVideoSurfaceCount > 0)
                            pDriverData->DDrawVideoSurfaceCount--;
                    }
                }
                else {
                    HGLOBAL hMem = (HGLOBAL)pDDSGbl->dwReserved1;
                    GlobalFree(hMem);
                }
                pDDSGbl->dwReserved1 = 0;
                lpDestroySurface->ddRVal = DD_OK;
                goto Handled_Exit;
                break;
            case FOURCC_420i:
                if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                    DWORD dwIndex = 0;
                    while (dwIndex < NV_VPP_MAX_OVERLAY_SURFACES) {
                        if ((DWORD)pDDSLcl == pDriverData->vpp.dwOverlaySurfaceLCL[dwIndex]) {
                            pDriverData->vpp.dwOverlaySurfaceLCL[dwIndex] = 0;
                            if (pDriverData->vpp.dwOverlaySurfaces > 0)
                                pDriverData->vpp.dwOverlaySurfaces--;
                            if (pDriverData->vpp.dwOverlaySurfaces == 0) {
                                pDriverData->vpp.dwOverlayOwner = 0;
                            }
                        }
                        dwIndex++;
                    }
                }

                if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                    (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)) {
#ifdef OLD_BLIT_CODE
                    if (pDDSGbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0]) {

                        if (pDriverData->NvYUY2Surface0Mem > 0) {
                            NVHEAP_FREE (pDriverData->NvYUY2Surface0Mem);
                            pDriverData->NvYUY2Surface0Mem = 0;
                            if (pDriverData->DDrawVideoSurfaceCount > 0)
                                pDriverData->DDrawVideoSurfaceCount--;
                        }

                        if (!(pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) &&
                            (getDV()->nvPusher.isValid())) {
                            // make sure we sync with other channels before writing put
                            getDC()->nvPusher.setSyncChannelFlag();
                        }

                        if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {  // Mode switches might occur without warning
                            NvReleaseSemaphore(pDriverData);
                            lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                            DDENDTICK(SURF4_DESTROYSURFACE);
                            dbgTracePop();
                            return (DDHAL_DRIVER_HANDLED);
                        }

                        // If AGP memory then send a sync rectangle to make sure we are done with it
                        if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) &&
                           !(pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {

                            NvNotification *pDmaSyncNotifier = (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;

                            // Let D3D code know that we have touched NV
                            pDriverData->TwoDRenderingOccurred = 1;

                            // MUST wait for any pending notification to prevent possible loss of notification serialization
                            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;
#ifdef CAPTURE
                            if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                                CAPTURE_SYNC3 sync;
                                sync.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                                sync.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                                sync.dwExtensionID        = CAPTURE_XID_SYNC3;
                                sync.dwType               = CAPTURE_SYNC_TYPE_NOTIFIER;
                                sync.dwCtxDMAHandle       = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
                                sync.dwOffset             = 0;
                                sync.dwValue              = 0;
                                captureLog (&sync,sizeof(sync));
                            }
#endif // CAPTURE
                            pDmaSyncNotifier->status = NV_IN_PROGRESS;

                            nvPushData (0,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000);
                            nvPushData (1,0);
                            nvPushData (2,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000);
                            nvPushData (3,0);
                            nvPusherAdjust (4);
                            nvPusherStart  (TRUE);

                            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification
#ifdef CAPTURE
                            if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                                CAPTURE_SYNC3 sync;
                                sync.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                                sync.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                                sync.dwExtensionID        = CAPTURE_XID_SYNC3;
                                sync.dwType               = CAPTURE_SYNC_TYPE_NOTIFIER;
                                sync.dwCtxDMAHandle       = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
                                sync.dwOffset             = 0;
                                sync.dwValue              = 0;
                                captureLog (&sync,sizeof(sync));
                            }
#endif // CAPTURE
                        }

#ifdef OLD_BLIT_CODE
                        if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] <
                             pDriverData->BaseAddress) || // If system memory
                            (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >
                             (unsigned long)pDriverData->VideoHeapEnd)) {
                            NvNotification *pDmaSyncNotifier =
                            (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;

                            if (!(pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {
                                // Let D3D code know that we have touched NV
                                pDriverData->TwoDRenderingOccurred = 1;

                                // MUST wait for any pending notification to prevent possible loss of notification serialization
                                while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

                                pDriverData->dDrawSpareSubchannelObject = 0;

                                pDmaSyncNotifier->status = NV_IN_PROGRESS;

                                // Trash spare subchannel
                                nvPushData (0,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                                nvPushData (1,NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
                                nvPushData (2,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                         MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000);
                                nvPushData (3,NV1_NULL_OBJECT);
                                nvPushData (4,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                         MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000);
                                nvPushData (5,NV1_NULL_OBJECT);
                                nvPushData (6,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                                nvPushData (7,NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT);
                                nvPushData (8,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                         MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000);
                                nvPushData (9,NV1_NULL_OBJECT);
                                nvPushData (10,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                          MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000);
                                nvPushData (11,NV1_NULL_OBJECT);
                                nvPushData (12,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                                nvPushData (13,NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT);
                                nvPushData (14,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                          MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000);
                                nvPushData (15,NV1_NULL_OBJECT);
                                nvPushData (16,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                          MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000);
                                nvPushData (17,NV1_NULL_OBJECT);
                                nvPushData (18,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
                                nvPushData (19,NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT);
                                nvPushData (20,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                          MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000);
                                nvPushData (21,NV1_NULL_OBJECT);
                                nvPushData (22,dDrawSubchannelOffset(NV_DD_SPARE) +
                                                          MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000);
                                nvPushData (23,NV1_NULL_OBJECT);
                                nvPushData (24,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                                                          RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000);
                                nvPushData (25,0);
                                nvPushData (26,dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                                                          RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000);
                                nvPushData (27,0);

                                nvPusherAdjust (28);
                                nvPusherStart  (TRUE);

                                while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification
                            }

                            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                                     NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
                        }
#endif  // OLD_BLIT_CODE

                        // If in AGP memory then deallocate it from there
                        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
                            // this should never happen
                            nvAssert(0);
                        }
                        else {
#ifdef OLD_BLIT_CODE
                            if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] <
                                 pDriverData->BaseAddress) || // If system memory
                                (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >
                                 (unsigned long)pDriverData->VideoHeapEnd)) {
                                GlobalFree((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
                            }
                            else {
#endif
                                NVHEAP_FREE (pDDSGbl->fpVidMem);
                                if (pDriverData->DDrawVideoSurfaceCount > 0)
                                    pDriverData->DDrawVideoSurfaceCount--;
#ifdef OLD_BLIT_CODE
                            }
#endif
                        }

#ifdef OLD_BLIT_CODE
                        pDriverData->NvFloatingMem2MemFormatMemoryHandle = 0;

                        pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] = 0;
                        pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
                        pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
                        pDriverData->NvFloatingMem2MemFormatSurfaceAddress[3] = 0;
#endif  // OLD_BLIT_CODE
                    }
                    else {
#ifdef OLD_BLIT_CODE
                        if (pDDSGbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1]) {
                            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
                            if (pDriverData->NvYUY2Surface1Mem > 0) {
                                NVHEAP_FREE (pDriverData->NvYUY2Surface1Mem);
                                pDriverData->NvYUY2Surface1Mem = 0;
                                if (pDriverData->DDrawVideoSurfaceCount > 0)
                                    pDriverData->DDrawVideoSurfaceCount--;
                            }
                        }
                        else if (pDDSGbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2]) {
                            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
                            if (pDriverData->NvYUY2Surface2Mem > 0) {
                                NVHEAP_FREE (pDriverData->NvYUY2Surface2Mem);
                                pDriverData->NvYUY2Surface2Mem = 0;
                                if (pDriverData->DDrawVideoSurfaceCount > 0)
                                    pDriverData->DDrawVideoSurfaceCount--;
                            }
                        }
                        else {
                            if (pDriverData->NvYUY2Surface3Mem > 0) {
                                NVHEAP_FREE (pDriverData->NvYUY2Surface3Mem);
                                pDriverData->NvYUY2Surface3Mem = 0;
                                if (pDriverData->DDrawVideoSurfaceCount > 0)
                                    pDriverData->DDrawVideoSurfaceCount--;
                            }
                        }
#endif  // OLD_BLIT_CODE
                    }
#endif  // OLD_BLIT_CODE
                    pDDSGbl->fpVidMem = 0;
                }
                else if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
                    if (pDDSGbl->fpVidMem > 0) {
                        NVHEAP_FREE (pDDSGbl->fpVidMem);
                        pDDSGbl->fpVidMem = 0;
                        if (pDriverData->DDrawVideoSurfaceCount > 0)
                            pDriverData->DDrawVideoSurfaceCount--;
                    }
                }
                else {
                    HGLOBAL hMem = (HGLOBAL)pDDSGbl->dwReserved1;
                    GlobalFree(hMem);
                }
                NvReleaseSemaphore(pDriverData);
                pDDSGbl->dwReserved1 = 0;
                lpDestroySurface->ddRVal = DD_OK;
                DDENDTICK(SURF4_DESTROYSURFACE);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
                break;
        }
    }

    else if ((pDDSLcl->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)) {
        // surfaces in this branch must have an NVObject
        if (global.dwDXRuntimeVersion < 0x0700) {
            // make sure that DX6 render targets still point at the right memory
            CSimpleSurface* pSurf = pObj ? pObj->getSimpleSurface() : NULL;
            if (pSurf) {
                // undo damage caused by nvRefreshSurfaceInfo
                pSurf->setAddress(pDDSGbl->fpVidMem);
            }
        }

        if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_ZBUFFER){
            //walk the pcontext lists to remove any dangling references to this zbuffer
            //our nvCelsiusAACreate will touch this pointer later and cause problems.
            //ideally zbuffer/destroys are not frequent so this should be low impact.
            PNVD3DCONTEXT pContext;
            pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
            while(pContext){
                if(pObj && pContext->pZetaBuffer == pObj->getSimpleSurface()) pContext->pZetaBuffer = NULL;
                // update cached pointer
                pContext->kelvinAA.UpdateZBuffer();
                pContext = pContext->pContextNext;
            }
        }

        // the surface, surface object and NVObject will all be deallocated when the NVObject is released below
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    Offscreen deallocated" );
        pDDSGbl->fpVidMem = 0;
        pDDSGbl->dwReserved1 = 0;
        lpDestroySurface->ddRVal = DD_OK;
        goto Handled_Exit;
    }

    else if (pDDSLcl->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {
        DPF_LEVEL(NVDBG_LEVEL_SURFACE, "    System memory deallocated" );
        HGLOBAL hMem = (HGLOBAL)pDDSGbl->dwReserved1;
        GlobalFree(hMem);
        pDDSGbl->dwReserved1 = 0;
        lpDestroySurface->ddRVal = DD_OK;
        goto Handled_Exit;
    }

    NvReleaseSemaphore(pDriverData);
    DDENDTICK(SURF4_DESTROYSURFACE);
    dbgTracePop();
    return (DDHAL_DRIVER_NOTHANDLED);

Handled_Exit:
    if (pObj) {
        nvClearObjectListEntry (pObj, &global.pNvSurfaceLists);
#ifndef WINNT
        if (pObj == (CNvObject*)pDXShare->pNvPrimarySurf)
        {
            DPF("DestroySurface32 called for primary surface, freeing back buffers instead");
            // normally we don't get calls to free the primary surface on Win9X, but sometimes the runtime
            // gets mixed up and sends us a destroy call for the primary surface but not for the back buffer.
            // in this case we should release the backbuffer to prevent a memory leak
            pObj = pObj->getAttachedA();
            if (pObj) {
                pObj->release();
            }
        } else {
            pObj->release();
        }
#else
        pObj->release();
#endif
        // clear the dwReseved field (pointer to the NvObject). on NT we must check for this when
        // traversing the surface chain in CreateSurfaceEx and ignore surfaces with deleted NvObjects
        // this corresponds with the explicit attachment concept under win9x
        SET_PNVOBJ(pDDSLcl, NULL);
    }

    NvReleaseSemaphore(pDriverData);
    DDENDTICK(SURF4_DESTROYSURFACE);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
} // DestroySurface32

#endif  // NVARCH >= 0x04

