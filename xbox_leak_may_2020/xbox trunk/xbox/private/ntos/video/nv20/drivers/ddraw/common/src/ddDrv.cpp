/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: ddDrv.cpp                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           7/6/99                                           *
*                                                                           *
\***************************************************************************/
// NB: This file is 'opted out' of using precompiled headers
#define INITGUID
#include "nvprecomp.h"

#include "escape.h"
#include "dxshare.h"
#include "nvcm.h"
#include "nvreg.h"
#include "nvILHash.h"
#include "..\..\..\modeset.mhd\include\modeext.h"
#include "wincommon.h"


#ifdef WINNT
    #ifdef _NO_COM
        #include <ddkernel.h>
    #else
        #define _NO_COM
        #include <ddkernel.h>
        #undef _NO_COM
    #endif // _NO_COM
#endif // WINNT

#ifdef WINNT
    PDEV    *ppdev;
#else
    DISPDRVDIRECTXCOMMON *pDXShare;

    U032  __stdcall SetSharedFSDOSBoxPtr (U032 *pData);
    DWORD __stdcall buildDDHALInfo16 (void);
    DWORD __stdcall UpdateNonLocalHeap32 (LPDDHAL_UPDATENONLOCALHEAPDATA lpd);
    DWORD __stdcall nvModeSetPrepare16 (void);
    DWORD __stdcall nvModeSetCleanup16 (void);
    void  __stdcall BuildDefaultDDHalModeTable (void);
    DWORD __stdcall GetSelectorsFromDisplayDriver (void);
    DWORD __stdcall FreeSelectorsFromDisplayDriver (void);
    void  __stdcall BuildDDHalModeTable (void);
#endif // !WINNT

#define NV_BUS_TYPE_AGP 4

CDriverContext         *pDriverData;

#ifdef CNT_TEX
DWORD g_dwVidTexDL[MAX_FRAME_CNT];
DWORD g_dwAgpTexDL[MAX_FRAME_CNT];
DWORD g_dwFrames;
#endif

/*
 * list of ROPs we support
 *
 * SRCCOPY is the key one to support
 */
static DWORD ropList[] = {
    BLACKNESSBIT | NOTSRCERASEBIT,               // 0x00 - 0x1F
    NOTSRCCOPYBIT,                               // 0x20 - 0x3F
    SRCERASEBIT,                                 // 0x40 - 0x5F
    SRCINVERTBIT,                                // 0x60 - 0x7F
    SRCANDBIT,                                   // 0x80 - 0x9F
    MERGEPAINTBIT,                               // 0xA0 - 0xBF
    SRCCOPYBIT,                                  // 0xC0 - 0xDF
    SRCPAINTBIT | WHITENESSBIT                   // 0xE0 - 0xFF
};

#ifdef WINNT
    // caps are not dynamic on WINNT, so always declare overlay and just fail creation if we need to
    #define NO_OVERLAY (FALSE)
#else
    #define NO_OVERLAY (pDriverData->dwHeads > 1 &&                                         \
                        pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE &&            \
                      !(pDriverData->vpp.regOverlayMode2 & NV4_REG_OVL_MODE2_FSDEVICEMASK) &&   \
                      !(pDriverData->vpp.regOverlayMode3 & NV4_REG_OVL_MODE3_ALLOWOVL))
#endif

#if WINNT
extern "C" void __cdecl nvDDrawSynWithNvGdiDrv(PDEV*   ppdev);
#endif

/*
 * IN_VBLANK should be replaced by a test to see if the hardware is currently
 * in the vertical blank
 */
static BOOL     bInVBlank;
static BOOL     bInDisplay;
#define IN_VBLANK               (bInVBlank= !bInVBlank)
#define IN_DISPLAY              (bInDisplay= !bInDisplay)

/*
 * DRAW_ENGINE_BUSY should be replaced by a test to see the bltter is still
 * busy drawing
 */
#define DRAW_ENGINE_BUSY        FALSE

/*
 * ENOUGH_FIFO_FOR_BLT should be replaced with a test to see if there is enough
 * room in the hardware fifo for a blt
 */
#define ENOUGH_FIFO_FOR_BLT     TRUE

/*
 * CURRENT_VLINE should be replaced by a macro to retrieve the current
 * scanline
 */
#define CURRENT_VLINE   0

extern Nv10ControlDma * __stdcall NvSysOpenDma(char *, int, int, int);
extern DWORD __stdcall NvSysOpen(DWORD);
extern void  __stdcall NvSysClose(DWORD);

#ifndef WINNT   // Win9x Hackery
#ifndef NVPE
extern DWORD HookUpVxdStorage();
#endif
extern vpSurfaces* pMySurfaces;
#endif // !WINNT

#ifdef NVD3D
/*
 * D3D Hal Fcts
 */
extern BOOL DestroyItemArrays(void);
#endif // NVD3D

#ifdef NVSTATDRIVER
/*
 * Statitistics driver functions
 */
void __stdcall AttachNVStat();
void __stdcall DetachNVStat();

#endif // NVSTATDRIVER

#ifdef NV_TRACKAGP
void __Reset (void);
#endif //NV_TRACKAGP

/*
 * Define the Z-Buffer/Stencil Buffer Formats supported.
 * 16bit Formats should appear first in the data structure and then 24/32 bit formats.
 */
#define NV_NUM_16BIT_ZBUFFER_FORMATS    1
#define NV_NUM_ZBUFFER_FORMATS          3

typedef struct _def_zpixel_formats
{
    DWORD           dwNumZFormats;
    DDPIXELFORMAT   NvZBufferFormats[NV_NUM_ZBUFFER_FORMATS];
} ZPIXELFORMATS;

static  ZPIXELFORMATS  NvZPixelFormats =
{
    NV_NUM_ZBUFFER_FORMATS,
    {
        /*
         * 16 bit z buffer
         */
        {
            sizeof (DDPIXELFORMAT),                 // dwSize
            DDPF_ZBUFFER,                           // dwFlags
            0,                                      // dwFourCC (Not used)
            16,                                     // dwZBufferBitDepth
            0,                                      // dwStencilBitDepth
            0x0000FFFF,                             // dwZBitMask
            0x00000000,                             // dwStencilBitMask
            0x00000000                              // dwRGBZBitMask (Not used)
        },
        /*
         * 24 bit z-buffer, no stencil
         */
        {
            sizeof (DDPIXELFORMAT),                 // dwSize
            DDPF_ZBUFFER,                           // dwFlags
            0,                                      // dwFourCC (Not used)
            24,                                     // dwZBufferBitDepth
            0,                                      // dwStencilBitDepth
            0xFFFFFF00,                             // dwZBitMask
            0x00000000,                             // dwStencilBitMask
            0x00000000                              // dwRGBZBitMask (Not used)
        },
        /*
         * 24 bit z-buffer, 8 bit stencil
         */
        {
            sizeof (DDPIXELFORMAT),                 // dwSize
            DDPF_ZBUFFER | DDPF_STENCILBUFFER,      // dwFlags
            0,                                      // dwFourCC (Not used)
            32,                                     // dwZBufferBitDepth - This is so brain dead.  MS needs to learn consistency.
            8,                                      // dwStencilBitDepth
            0xFFFFFF00,                             // dwZBitMask
            0x000000FF,                             // dwStencilBitMask
            0x00000000                              // dwRGBZBitMask (Not used)
        }
    }
};

#ifndef TEXFORMAT_CRD

// DX8 Texture/Z/Render formats
static DDPIXELFORMAT celsiusPixelFormatsDX8[] =
{
    CELSIUS_TEXTURE_FORMATS_DX8         // Texture formats defined in NVTEXFMTS.H
};

static DDPIXELFORMAT celsiusPixelFormatsDX8NoDXT[] =
{
    CELSIUS_TEXTURE_FORMATS_NO_DXT_DX8  // Texture formats defined in NVTEXFMTS.H
};
#if (NVARCH >= 0x020)
static DDPIXELFORMAT kelvinPixelFormatsDX8[] =
{
    KELVIN_TEXTURE_FORMATS_DX8         // Texture formats defined in NVTEXFMTS.H
};

static DDPIXELFORMAT kelvinPixelFormatsDX8NoDXT[] =
{
    KELVIN_TEXTURE_FORMATS_NO_DXT_DX8  // Texture formats defined in NVTEXFMTS.H
};
#endif // (NVARCH >= 0x020)

#endif  // !TEXFORMAT_CRD

//---------------------------------------------------------------------------

#ifndef WINNT
BOOL MyExtEscape (DWORD dwHead, int nEscapeID, int nInBytes, LPCSTR lpIn, int nOutBytes, LPSTR lpOut)
{
    DISPDRVDIRECTXCOMMON *pDXS;
    BOOL bRet;

    dbgTracePush ("MyExtEscape");

    nvAssert (pDXShare);

    // fetch the appropriate DXShare
    pDXS = getDXShare (pDXShare->dwRootHandle, dwHead);

    if (pDXS) {
        // get the DC handle for this pDXShare
        HDC hDC = nvCreateDCHandle (pDXS);
        nvAssert (GetObjectType(hDC));
        // make the escape call
        bRet = ExtEscape (hDC, nEscapeID, nInBytes, lpIn, nOutBytes, lpOut);
        // release the handle
        nvDestroyDCHandle (hDC);
    }
    else {
        // no heads are enabled
        bRet = FALSE;
    }

    dbgTracePop();
    return (bRet);
}
#endif // !WINNT

//---------------------------------------------------------------------------

DWORD WaitForIdle(BOOL bWait, BOOL bUseEvent)
{
    NvNotification *pDmaSyncNotifier = pDriverData->pDmaSyncNotifierFlat;
    LPPROCESSINFO   lpProcInfo;

    dbgTracePush ("WaitForIdle");

    if (bUseEvent) {
        lpProcInfo = pmGetProcess(pDriverData, GetCurrentProcessId());
        if ((lpProcInfo == NULL) || (lpProcInfo->hIdleSyncEvent3 == 0)) {
            DPF("WaitForIdle: Unable to get process ID, using NV_SLEEP");
            bUseEvent = FALSE;
        }
        else {
            NvResetEvent(lpProcInfo->hIdleSyncEvent3);
        }
    }

    if ((pDmaSyncNotifier == NULL) || (!getDC()->nvPusher.isValid())) {
        DPF("WaitForIdle: Invalid parameters");
        dbgTracePop();
        return (DD_OK);
    }

    // Waiting for a notifier with the FIFO in this state guarantees a hang
    if (pDriverData->dwFullScreenDOSStatus & (FSDOSSTATUS_RECOVERYNEEDED_D3D | FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {
        DPF("WaitForIdle: Early exit due to DosOccurred flag");
        dbgTracePop();
        return (DD_OK);
    }

    pDriverData->TwoDRenderingOccurred = 1;

    // wait for other dma channels to go idle
    getDC()->nvPusher.waitForOtherChannels();

    // MUST wait for any pending notification to prevent possible loss of notification serialization
    if (!bUseEvent && bWait) {
        while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;
#ifdef CAPTURE
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            CAPTURE_SYNC3 sync;
            sync.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            sync.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            sync.dwExtensionID        = CAPTURE_XID_SYNC3;
            sync.dwType               = CAPTURE_SYNC3_TYPE_NOTIFIER;
            sync.dwCtxDMAHandle       = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
            sync.dwOffset             = 0;
            sync.dwValue              = 0;
            captureLog (&sync,sizeof(sync));
        }
#endif // CAPTURE
    }

    pDmaSyncNotifier->status = NV_IN_PROGRESS;

    nvPushData (0, dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000);
    nvPushData (1, ((bWait && bUseEvent) ? NV04A_NOTIFY_WRITE_THEN_AWAKEN : 0));
    nvPushData (2, dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) + RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000);
    nvPushData (3, 0);

    nvPusherAdjust (4);
    nvPusherStart  (TRUE);

    if (bWait) {
        if (bUseEvent) {
            if (IS_EVENT_ERROR(NvWaitEvent(lpProcInfo->hIdleSyncEvent3, 500))) {
                // timeout, our process list must be messed up
                EVENT_ERROR;
            }
        } else {
            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;
#ifdef CAPTURE
            if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
                CAPTURE_SYNC3 sync;
                sync.dwJmpCommand         = CAPTURE_JMP_COMMAND;
                sync.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
                sync.dwExtensionID        = CAPTURE_XID_SYNC3;
                sync.dwType               = CAPTURE_SYNC3_TYPE_NOTIFIER;
                sync.dwCtxDMAHandle       = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
                sync.dwOffset             = 0;
                sync.dwValue              = 0;
                captureLog (&sync,sizeof(sync));
            }
#endif // CAPTURE
        }
    }
    else {
        NV_SLEEP;
        if (pDmaSyncNotifier->status == NV_IN_PROGRESS) {
            dbgTracePop();
            return (DDERR_WASSTILLDRAWING);
        }
    }

    dbgTracePop();
    return (DD_OK);
}

//---------------------------------------------------------------------------
#ifndef WINNT // HAL function not present under NT
/*
 * SetMode32

    This API call is only made on boards which do not have an
    associated display driver -- i.e. the 3D only boards. We
    should never see this on an NVidia board.
 */
DWORD __stdcall SetMode32( LPDDHAL_SETMODEDATA lpSetModeData )
{
    dbgTracePush ("SetMode32");

    nvSetDriverDataPtrFromDDGbl (lpSetModeData->lpDD);

    pDriverData->dwUseRefreshRate = lpSetModeData->useRefreshRate;

#if 0
    // Call back to 16 bit direct draw code to set mode
    MyExtEscape(NVSETDDRAWMODE, 0, 0, 0, 0);
#endif

    lpSetModeData->ddRVal = DD_OK;

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}
#endif // !WINNT

//---------------------------------------------------------------------------

/*
 * Lock32
 */
DWORD __stdcall Lock32 (LPDDHAL_LOCKDATA lpLockData)
{
    HRESULT                     ddrval = DDHAL_DRIVER_NOTHANDLED;
    DWORD                       dwCaps;
    LPDDRAWI_DDRAWSURFACE_GBL   psurf_gbl;

    dbgTracePush ("Lock32: fpvidmem = %08x", lpLockData->lpDDSurface->lpGbl->fpVidMem);

    dwCaps = lpLockData->lpDDSurface->ddsCaps.dwCaps;

    if (dwCaps & DDSCAPS_EXECUTEBUFFER)
    {
        // silly hack for screwed up kernel calls
        DWORD dwRetVal;
        dwRetVal = D3DLockExecuteBuffer32(lpLockData);
        dbgTracePop();
        return dwRetVal;
    }

    DDSTARTTICK(SURF4_LOCK);
    nvSetDriverDataPtrFromDDGbl (lpLockData->lpDD);

    lpLockData->ddRVal = DD_OK;

    // DO THIS FIRST!! We shouldn't assign pointers below first,
    // because if we end up resetting nv via nvEnable32, the pointers
    // may have changed in the pDriverData structure.

    // Reset NV and get the monitor frequency after a mode reset
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED) {
        getDC()->nvPusher.waitForOtherChannels();
        if (!nvEnable32 (lpLockData->lpDD)) {
            NvReleaseSemaphore(pDriverData);
            DDENDTICK(SURF4_CREATESURFACE);
            lpLockData->ddRVal = DDERR_OUTOFMEMORY;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
     * NOTES:
     *
     * This callback is invoked whenever a surface is about to be directly
     * accessed by the user.   This is where you need to make sure that
     * a surface can be safely accessed by the user.
     *
     * If your memory cannot be accessed while in accelerator mode, you
     * should either take the card out of accelerator mode or else
     * return DDERR_SURFACEBUSY
     *
     * If someone is accessing a surface that was just flipped away from,
     * make sure that the old surface (what was the primary) has finished
     * being displayed.
     */

    dwCaps = lpLockData->lpDDSurface->ddsCaps.dwCaps;

    psurf_gbl = lpLockData->lpDDSurface->lpGbl;

    if (pDriverData->dwFullScreenDOSStatus & (FSDOSSTATUS_RECOVERYNEEDED_D3D | FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
                nvD3DRecover();
            }
            if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_DDRAW) {
                nvDDrawRecover();
            }
        }
        else {
            lpLockData->ddRVal = DDERR_SURFACEBUSY;
            DDENDTICK(SURF4_LOCK);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

    if (((!getDC()->nvPusher.isValid()) && (dwCaps & DDSCAPS_VIDEOMEMORY)) || // Mode switches might occur without warning
        (IS_OVERLAY(dwCaps,psurf_gbl->ddpfSurface.dwFourCC) && pDriverData->vpp.dwOverlayFSOvlLost)) {    // overlay capability has vanished
        lpLockData->ddRVal = DDERR_SURFACELOST;
        DDENDTICK(SURF4_LOCK);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

#ifdef CAPTURE
    CNvObject *pNvObj_cap = GET_PNVOBJ(lpLockData->lpDDSurface);
    DWORD      dwHeap_cap = 0;
    DWORD      dwOffset_cap, dwSize_cap;
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        if (pNvObj_cap) {
            if (pNvObj_cap->getTexture()) {
                dwHeap_cap   = pNvObj_cap->getTexture()->getSwizzled()->getHeapLocation();
                dwOffset_cap = pNvObj_cap->getTexture()->getSwizzled()->getOffset();
                dwSize_cap   = pNvObj_cap->getTexture()->getSize();
            }
            else if (pNvObj_cap->getVertexBuffer()) {
                dwHeap_cap   = pNvObj_cap->getVertexBuffer()->getHeapLocation();
                dwOffset_cap = pNvObj_cap->getVertexBuffer()->getOffset();
                dwSize_cap   = pNvObj_cap->getVertexBuffer()->getSize();
            }
            else if (pNvObj_cap->getSimpleSurface()) {
                dwHeap_cap   = pNvObj_cap->getSimpleSurface()->getHeapLocation();
                dwOffset_cap = pNvObj_cap->getSimpleSurface()->getOffset();
                dwSize_cap   = pNvObj_cap->getSimpleSurface()->getSize();
            }
            else if (pNvObj_cap->getPaletteSurface()) {
                dwHeap_cap   = pNvObj_cap->getPaletteSurface()->getHeapLocation();
                dwOffset_cap = pNvObj_cap->getPaletteSurface()->getOffset();
                dwSize_cap   = pNvObj_cap->getPaletteSurface()->getSize();
            }
        }
        if (dwHeap_cap) {
            CAPTURE_LOCK lock;
            lock.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            lock.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            lock.dwExtensionID        = CAPTURE_XID_LOCK;
            lock.dwType               = CAPTURE_LOCK_TYPE_LOCK;
            lock.dwCtxDMAHandle       = (dwHeap_cap == CSimpleSurface::HEAP_VID)
                                      ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                      : D3D_CONTEXT_DMA_HOST_MEMORY;
            lock.dwOffset             = dwOffset_cap;
            captureLog (&lock, sizeof(lock));
        }
    }
#endif

    /*
     * Check for texture lock BEFORE syncing hard
     */
    if (dwCaps & DDSCAPS_TEXTURE) {
        /*
         * get texture
         */
        CTexture *pTexture = GET_PNVOBJ(lpLockData->lpDDSurface)->getTexture();
        /*
         * user wants to lock a texture - process elsewhere
         */
        if (IS_VALID_PNVOBJ(pTexture))
        {
            ddrval = nvTextureLock (pTexture,lpLockData);
            DDENDTICK(SURF4_LOCK);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return ddrval;
        }
    }

#if (NVARCH >= 0x10)

    if (!(pDriverData->vpp.dwDecoderCaps & NVOCID2_OS_SCHEDULED)) {
        if (psurf_gbl->ddpfSurface.dwFlags & DDPF_FOURCC) {
            if ((psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVMC) ||
                (psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVDS)) { // Is it our motion comp. control surface ?
                LPNVMCFRAMEDATA lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(psurf_gbl->fpVidMem);
#ifdef WINNT
                lpNvMCFrameData->dwMCNvExecute = NULL;
                lpNvMCFrameData->dwMCMultiMonID = 0;
#else // !WINNT
                lpNvMCFrameData->dwMCNvExecute = (PFUNC)nvMoCompExecute;
                lpNvMCFrameData->dwMCMultiMonID = (DWORD)pDXShare;
#endif // !WINNT
                if (psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVMC) // Don't trash NVDS surfaces
                    lpNvMCFrameData->bMCDestinationSurfaceIndex = 255;
                lpNvMCFrameData->bMCForwardSurfaceIndex = 255;
                lpNvMCFrameData->bMCBackwardSurfaceIndex = 255;
                lpNvMCFrameData->bMCEndOfPicture = FALSE;
                pDriverData->bMCNewFrame = TRUE;
                DDENDTICK(SURF4_LOCK);
                NvReleaseSemaphore(pDriverData);
                dbgTracePop();
                return (DDHAL_DRIVER_NOTHANDLED);
            }

            else if (psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVID) { // Is it a motion comp. IDCT surface ?

                if (lpLockData->lpDDSurface != (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCIDCTSurface) {

                    for (DWORD dwIndex=0; dwIndex<4; dwIndex++) {
                        if (lpLockData->lpDDSurface == (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCNVIDSurface[dwIndex]) {
                            break;
                        }
                    }

                    if (dwIndex < 4) {

                        if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                                       pDriverData->dwMCNVIDSurfaceReferenceCount[dwIndex],
                                                       1)) {
                             NvReleaseSemaphore(pDriverData);
                             lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
                             DDENDTICK(SURF4_LOCK);
                             dbgTracePop();
                             return (DDHAL_DRIVER_HANDLED);
                        }
                    }
                }

                DDENDTICK(SURF4_LOCK);
                NvReleaseSemaphore(pDriverData);
                dbgTracePop();
                return (DDHAL_DRIVER_NOTHANDLED);
            }

            else if (psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NV12) { // Is it a motion comp. image surface ?

                LPNVMCSURFACEFLAGS lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

                while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
                       (lpSurfaceFlags->dwMCSurfaceBase != psurf_gbl->fpVidMem)) {
                    lpSurfaceFlags++;
                }

                if (lpLockData->lpDDSurface == (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface) {
                    if (lpLockData->dwFlags & DDLOCK_READONLY) {
                        if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                                       lpSurfaceFlags->dwMCMotionCompReferenceCount,
                                                       0)) {
                            lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
                        }
                    }
                    else {

                        if ((!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                                        lpSurfaceFlags->dwMCMotionCompReferenceCount,
                                                        0))
                            ||
                            (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                                        lpSurfaceFlags->dwMCFormatCnvReferenceCount,
                                                        0))) {
                            lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
                        }
                    }
                }
                else if ((lpLockData->lpDDSurface == (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCForwardSurface) ||
                         (lpLockData->lpDDSurface == (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface)) {
                    if (!(lpLockData->dwFlags & DDLOCK_READONLY)) {
                        if (!getDC()->pRefCount->wait (CReferenceCount::WAIT_COOPERATIVE,
                                                       lpSurfaceFlags->dwMCMotionCompReferenceCount,
                                                       0)) {
                            lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
                        }
                    }
                }

                // If DXVA locks this NV12 surface it means the decoder is probably going to
                // change the contents, so force a reconversion from NV12 to YUY2 at Flip32 time.
                if (pDriverData->dwDXVAFlags & DXVA_MO_COMP_IN_PROGRESS)
                    lpSurfaceFlags->bMCFrameAlreadyFormatConverted = 0;

                DDENDTICK(SURF4_LOCK);
                NvReleaseSemaphore(pDriverData);
                dbgTracePop();
                if (lpLockData->ddRVal != DD_OK) {
                    return DDHAL_DRIVER_HANDLED;
                } else {
                    return DDHAL_DRIVER_NOTHANDLED;
                }
            }
        }
    }

#endif  // NVARCH >= 0x10

#if (NVARCH >= 0x010)
    // Handle AA: When locking a render target or Z buffer, minify first.
    if (getDC()->dwAAContextCount >= 1)
    {
#if (NVARCH >= 0x020)
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
            CNvObject *pObj = GET_PNVOBJ(lpLockData->lpDDSurface);
            if (pObj) {
                PNVD3DCONTEXT pContext = nvCelsiusAAFindContext(pObj->getSimpleSurface());
                if (pContext)
                {
                    CKelvinAAState::AccessType access = (lpLockData->dwFlags & DDLOCK_READONLY) ? CKelvinAAState::ACCESS_READ : CKelvinAAState::ACCESS_WRITE;
                    access = (lpLockData->dwFlags & DDLOCK_DISCARDCONTENTS) ? CKelvinAAState::ACCESS_WRITE_DISCARD : access;

                    if ((pObj->getSimpleSurface() == pContext->pRenderTarget)) {
                        pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_RT, access, false, true);
                    }
                    if ((pContext->pZetaBuffer) && (pObj->getSimpleSurface() == pContext->pZetaBuffer)) {
                        pContext->kelvinAA.GrantAccess(CKelvinAAState::BUFFER_ZB, access, false, true);
                    }
                }
            }
        } else
#endif
        if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) {
            CNvObject *pObj = GET_PNVOBJ(lpLockData->lpDDSurface);
            if (pObj) {
                PNVD3DCONTEXT pContext = nvCelsiusAAFindContext(pObj->getSimpleSurface());
                if (pContext && pContext->aa.isInit())
                {
                    if ((pObj->getSimpleSurface() == pContext->pRenderTarget)) {
                        pContext->aa.Lock(pContext);
                    }
                    if ((pContext->pZetaBuffer) && (pObj->getSimpleSurface() == pContext->pZetaBuffer)) {
                        pContext->aa.LockZ(pContext);
                    }
                }
            }
        }
    }
#endif  // NVARCH >= 0x010

    // determine conditions under which a wait is required
    BOOL bWrite = !(lpLockData->dwFlags & DDLOCK_READONLY);

    if (lpLockData->dwFlags & DDLOCK_WAIT) {
        if (bWrite) { // wait is only required if we intend to write to this surface
            // wait for flip if scanning out from this surface
            ddrval = getFlipStatus (psurf_gbl->fpVidMem, dwCaps);
            while (ddrval != DD_OK) {
                NV_SLEEP;
                ddrval = getFlipStatus (psurf_gbl->fpVidMem, dwCaps);
            }
        }
        // flush out all blit and clear operations
        if (IS_OVERLAY(dwCaps,psurf_gbl->ddpfSurface.dwFourCC) ||
            psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NV12 ||
            psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVID ||
            psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVMC ||
            psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVDS ||
            psurf_gbl->ddpfSurface.dwFourCC == FOURCC_NVSP ||
           (lpLockData->lpDDSurface->ddsCaps.dwCaps & (DDSCAPS_VIDEOPORT | DDSCAPS_HWCODEC | DDSCAPS_PRIMARYSURFACE))) {
            // yielding stall for video
            WaitForIdle(TRUE, TRUE);
        } else {
            // polling stall for D3D
            getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
        }

        if (lpLockData->lpDDSurface->ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE)) {
#ifdef WINNT
            nvDDrawSynWithNvGdiDrv(ppdev);
#else
            // flush out all GDI blits (MS does this for us in all cases except the primary)
            void *lpPDevice = GET_HALINFO()->lpPDevice;
            DrvCallbackFunc(NV_DRVSERVICE_BEGINACCESS, lpPDevice, NULL);
#endif
        }
    } else {
        // return error code if required
        if ((bWrite && (getFlipStatus (psurf_gbl->fpVidMem, dwCaps) != DD_OK)) || !getDC()->nvPusher.isIdle(TRUE)) {
            lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
            DDENDTICK(SURF4_LOCK);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
#ifdef WINNT
        if (lpLockData->lpDDSurface->ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE)) 
        {
            nvDDrawSynWithNvGdiDrv(ppdev);
        }
#endif
    }

    if ((pDriverData->pGenericScaledImageNotifier->status == NV_IN_PROGRESS) &&
        (pDriverData->NvDmaFromMemSurfaceMem == psurf_gbl->fpVidMem)) {
        lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
        DDENDTICK(SURF4_LOCK);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    //
    // check if at least one 3d context exists
    //
    if ((getDC()->dwD3DContextCount > 0)
     && (pDriverData->nvD3DPerfData.dwPerformanceStrategy & (PS_CONTROL_TRAFFIC_16 | PS_CONTROL_TRAFFIC_32)))
    {
        CNvObject *pNvObj = GET_PNVOBJ(lpLockData->lpDDSurface);

        // test for traffic control
        if (dwCaps & DDSCAPS_ZBUFFER)
        {
            if (IS_VALID_PNVOBJ(pNvObj) && pNvObj->hasCTEnabled())
            {
                pNvObj->tagZLock();
                pNvObj->disableCT();
                nvCTDecodeBuffer (pNvObj,
                                  VIDMEM_ADDR(psurf_gbl->fpVidMem),
                                  psurf_gbl->ddpfSurface.dwRGBBitCount >> 3,
                                  psurf_gbl->lPitch,
                                  psurf_gbl->wWidth,
                                  psurf_gbl->wHeight);
            }
        }
        else
        {
            PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)getDC()->dwContextListHead;
            while (pContext)
            {
                if ((pContext->pRenderTarget) &&
                    (pContext->pRenderTarget->getWrapper() == pNvObj))
                {
                    if (pContext->pZetaBuffer)
                    {
                        nvAssert (pContext->pZetaBuffer->getWrapper());
                        pContext->pZetaBuffer->getWrapper()->tagFLock();
                    }
                    break;
                }
                pContext = pContext->pContextNext;
            }
        }
    }

    // Note that this may return NULL
    CNvObject *pNvObj;
    pNvObj = GET_PNVOBJ(lpLockData->lpDDSurface);

    if ((dwCaps & DDSCAPS_VIDEOMEMORY) && ((dwCaps & DDSCAPS_TEXTURE) == 0)) {
        // No need to lock textures even though they may currently be in use because
        //     their surface memory is not used directly when rendering
        // Every other surface type with DDSCAPS_VIDMEMORY set does need locked however
        if(pNvObj && pNvObj->getSimpleSurface())
        {
            DDLOCKINDEX(NVSTAT_LOCK_VID_TEX);
            pNvObj->getSimpleSurface()->cpuLock(CSimpleSurface::LOCK_NORMAL);
            if (dwCaps & DDSCAPS_BACKBUFFER)
            {
                // avoid clearing the render target in 3D Mark 2000
                pNvObj->setRTLocked();
            }
        }
    }

#ifdef  STEREO_SUPPORT
    BOOL bLockSubstituted = StereoLockSubstitute(lpLockData);
#endif  //STEREO_SUPPORT


    if (pNvObj
#ifdef  STEREO_SUPPORT
        && bLockSubstituted == FALSE
#endif  //STEREO_SUPPORT
        )
    {
        // update MS cached pointer, which might be different if we renamed or otherwise
        // recreated the surface
        CSimpleSurface *pSurface = pNvObj->getSimpleSurface();

        if (pSurface) {
            switch(pSurface->getHeapLocation()) {
            case CSimpleSurface::HEAP_VID:
                psurf_gbl->fpVidMem = pSurface->getfpVidMem();
                break;
#ifdef NV_AGP
            case CSimpleSurface::HEAP_AGP:
                psurf_gbl->fpVidMem = nvAGPGetUserAddr(pSurface->getAddress());
                break;
#endif
            }
        }
    }

#ifndef WINNT
    // Currently works only for pDriverData->bi.biBitCount pixel depth surfaces
    DWORD dwOffset;
    if (lpLockData->bHasRect) {
        dwOffset = lpLockData->rArea.top * psurf_gbl->lPitch;
        dwOffset += (lpLockData->rArea.left * (GET_MODE_BPP() >> 3));
    }
    else {
        dwOffset = 0;
    }

    lpLockData->lpSurfData = (VOID *)((DWORD)psurf_gbl->fpVidMem + dwOffset);
#endif // !WINNT

#ifdef  STEREO_SUPPORT
    if (bLockSubstituted && lpLockData->ddRVal == DD_OK)
    {
        //We can't return DDHAL_DRIVER_NOTHANDLED for stereo if we really want to fool DX and substitute the
        //stereo buffer address. Otherwise DX retrieves the buffer address from its own internal structures
        //depending on the surface capabilities sometimes ignoring what we specified in the lpLockData->lpSurfData
        //or psurf_gbl->fpVidMem. For instance for the front buffer it retrieves the memory address from
        //LPDDRAWI_DIRECTDRAW_GBL->vmiData.fpPrimary. It is a pain in the ... for us to handle all the cases.
        //If we return DDHAL_DRIVER_HANDLED though it uses lpLockData->lpSurfData for sure (Andrei Osnovich).
        ddrval = DDHAL_DRIVER_HANDLED;
    }
#endif  //STEREO_SUPPORT

    DDENDTICK(SURF4_LOCK);
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return ddrval;

}  // Lock32

//---------------------------------------------------------------------------

/*
 * Unlock32
 */
DWORD __stdcall Unlock32( LPDDHAL_UNLOCKDATA lpUnlockData )
{
    LPDDRAWI_DDRAWSURFACE_GBL   pDDSGbl;
    DWORD                       caps;
    DWORD   ddrval;

    dbgTracePush ("Unlock32: fpvidmem = %08x", lpUnlockData->lpDDSurface->lpGbl->fpVidMem);

    if (lpUnlockData->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_EXECUTEBUFFER )
    {
        //silly hack for screwed up kernel calls
        DWORD dwRetVal;
        dwRetVal = D3DUnlockExecuteBuffer32(lpUnlockData);
        dbgTracePop();
        return dwRetVal;
    }

    DDSTARTTICK(SURF4_UNLOCK);
    nvSetDriverDataPtrFromDDGbl (lpUnlockData->lpDD);

    caps = lpUnlockData->lpDDSurface->ddsCaps.dwCaps;

    if ((!getDC()->nvPusher.isValid()) && (caps & DDSCAPS_VIDEOMEMORY)) {  /* Mode switches might occur without warning */
        lpUnlockData->ddRVal = DDERR_SURFACELOST;
        DDENDTICK(SURF4_UNLOCK);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

#ifdef CAPTURE
    CNvObject *pNvObj_cap = GET_PNVOBJ(lpUnlockData->lpDDSurface);
    DWORD      dwHeap_cap = 0;
    DWORD      dwAddr_cap, dwOffset_cap, dwSize_cap;
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        if (pNvObj_cap) {
            if (pNvObj_cap->getTexture()) {
                dwHeap_cap   = pNvObj_cap->getTexture()->getSwizzled()->getHeapLocation();
                dwAddr_cap   = pNvObj_cap->getTexture()->getSwizzled()->getAddress();
                dwOffset_cap = pNvObj_cap->getTexture()->getSwizzled()->getOffset();
                dwSize_cap   = pNvObj_cap->getTexture()->getSize();
            }
            else if (pNvObj_cap->getVertexBuffer()) {
                dwHeap_cap   = pNvObj_cap->getVertexBuffer()->getHeapLocation();
                dwAddr_cap   = pNvObj_cap->getVertexBuffer()->getAddress();
                dwOffset_cap = pNvObj_cap->getVertexBuffer()->getOffset();
                dwSize_cap   = pNvObj_cap->getVertexBuffer()->getSize();
            }
            else if (pNvObj_cap->getSimpleSurface()) {
                dwHeap_cap   = pNvObj_cap->getSimpleSurface()->getHeapLocation();
                dwAddr_cap   = pNvObj_cap->getSimpleSurface()->getAddress();
                dwOffset_cap = pNvObj_cap->getSimpleSurface()->getOffset();
                dwSize_cap   = pNvObj_cap->getSimpleSurface()->getSize();
            }
            else if (pNvObj_cap->getPaletteSurface()) {
                dwHeap_cap   = pNvObj_cap->getPaletteSurface()->getHeapLocation();
                dwAddr_cap   = pNvObj_cap->getPaletteSurface()->getAddress();
                dwOffset_cap = pNvObj_cap->getPaletteSurface()->getOffset();
                dwSize_cap   = pNvObj_cap->getPaletteSurface()->getSize();
            }
        }
        if (dwHeap_cap) {
            CAPTURE_LOCK lock;
            lock.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            lock.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            lock.dwExtensionID        = CAPTURE_XID_LOCK;
            lock.dwType               = CAPTURE_LOCK_TYPE_UNLOCK;
            lock.dwCtxDMAHandle       = (dwHeap_cap == CSimpleSurface::HEAP_VID)
                                      ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                      : D3D_CONTEXT_DMA_HOST_MEMORY;
            lock.dwOffset             = dwOffset_cap;
            captureLog (&lock, sizeof(lock));
        }
    }
#endif

    /*
     * Signal that the texture surface has been unlocked.
     */
    if (caps & DDSCAPS_TEXTURE) {
        /*
         * get texture
         */
        CTexture *pTexture = GET_PNVOBJ(lpUnlockData->lpDDSurface)->getTexture();

        /*
         * user wants to unlock a texture - process elsewhere
         */
        if (IS_VALID_PNVOBJ(pTexture))
        {
            ddrval = nvTextureUnlock(pTexture,lpUnlockData);
            DDENDTICK(SURF4_UNLOCK);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return ddrval;
        }
    }

    if ((caps & DDSCAPS_VIDEOMEMORY) && ((caps & DDSCAPS_TEXTURE) == 0)) {
        //unlock the video surface
        CNvObject *pNvObj;
        pNvObj = GET_PNVOBJ(lpUnlockData->lpDDSurface);
        if(pNvObj && pNvObj->getSimpleSurface()) pNvObj->getSimpleSurface()->cpuUnlock();
    }

#ifdef  STEREO_SUPPORT
    StereoLockSubstituteRestore(lpUnlockData);
#endif  //STEREO_SUPPORT

    pDDSGbl = lpUnlockData->lpDDSurface->lpGbl;

#ifndef WINNT
        if (lpUnlockData->lpDDSurface->ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE)) {
            // signal "end access" to the display driver
            void *lpPDevice = GET_HALINFO()->lpPDevice;
            DrvCallbackFunc(NV_DRVSERVICE_ENDACCESS, lpPDevice, NULL);
        }
#endif

    if (IS_OVERLAY(caps,pDDSGbl->ddpfSurface.dwFourCC) &&
        (pDDSGbl->ddpfSurface.dwFourCC != FOURCC_NV12) &&
        ((caps & (DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER)) ||
         (pDriverData->vpp.dwOverlaySurfaces < 3))) {

        // (H.AZAR: 08/17/99): surface must be visible to call VPP flip !
        if (!(caps & DDSCAPS_VIDEOPORT) && (caps & (DDSCAPS_VISIBLE
#ifndef WINNT
        | DDSCAPS_FRONTBUFFER
#endif
        )) && (pDriverData->bRing0FlippingFlag == FALSE)) {
            VppDoFlip(&(pDriverData->vpp),
                          pDDSGbl->fpVidMem - pDriverData->BaseAddress,
                          pDDSGbl->lPitch,
                          pDDSGbl->wWidth,
                          pDDSGbl->wHeight,
                          pDDSGbl->ddpfSurface.dwFourCC,
                          VPP_STATIC | VPP_WAIT);
        }
    }
#if (NVARCH >= 0x10)
    else if (pDDSGbl->ddpfSurface.dwFlags & DDPF_FOURCC) {
        if ((pDDSGbl->ddpfSurface.dwFourCC == FOURCC_NVMC) ||
            (pDDSGbl->ddpfSurface.dwFourCC == FOURCC_NVDS)) { // Is it our motion comp. control surface ?
            LPNVMCFRAMEDATA lpNvMCFrameData = (LPNVMCFRAMEDATA) VIDMEM_ADDR(pDDSGbl->fpVidMem);

            if (lpNvMCFrameData->bMCDestinationSurfaceIndex == 255) {
                lpUnlockData->ddRVal = DDERR_INVALIDPARAMS;
                DDENDTICK(SURF4_UNLOCK);
                NvReleaseSemaphore(pDriverData);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            } else if ((pDDSGbl->ddpfSurface.dwFourCC != FOURCC_NVDS) &&
                       (lpNvMCFrameData->bMCEndOfPicture))  {
                LPNVMCSURFACEFLAGS lpSurfaceFlags =
                    (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

                while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
                       (lpSurfaceFlags->dwMCSurfaceBase != pDriverData->dwMCMostRecentlyDecodedSurfaceBase)) {
                    lpSurfaceFlags++;
                }

                // Make sure surface has been format converted
                if (lpSurfaceFlags->dwMCSurfaceBase != 0) {
                    if ((lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) &&
                        (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 15))
                        nvMoCompConvertSurfaceFormat(lpSurfaceFlags->dwMCSurfaceBase, TRUE, 15);
                    else if (lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD) {
                        if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted & 3) < 3)
                            nvMoCompConvertSurfaceFormat(lpSurfaceFlags->dwMCSurfaceBase, TRUE, 3);
                    } else if (lpSurfaceFlags->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) {
                        if (lpSurfaceFlags->bMCFrameAlreadyFormatConverted < 12)
                            nvMoCompConvertSurfaceFormat(lpSurfaceFlags->dwMCSurfaceBase, TRUE, 12);
                    }
                    if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted == 15) &&
                        (lpSurfaceFlags->bMCFrameIsFiltered == FALSE) &&
                        (pDriverData->bMCTemporalFilterDisabled == FALSE))
                        nvMoCompTemporalFilter(lpSurfaceFlags->dwMCSurfaceBase, pDriverData->bMCPercentCurrentField);
                }
            }
        }
    }
#endif  // NVARCH >= 0x10

#ifdef CAPTURE
    if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
        if (dwHeap_cap) {
            CAPTURE_MEMORY_WRITE memwr;
            memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
            memwr.dwCtxDMAHandle       = (dwHeap_cap == CSimpleSurface::HEAP_VID)
                                       ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                       : D3D_CONTEXT_DMA_HOST_MEMORY;
            memwr.dwOffset             = dwOffset_cap;
            memwr.dwSize               = dwSize_cap;
            captureLog (&memwr, sizeof(memwr));
            captureLog ((void*)dwAddr_cap, memwr.dwSize);
        }
    }
#endif

    // Handle AA: When unlocking a render target or Z buffer, magnify the
    // modified buffer back into the super buffer.
    // Scott C: Removed AA stuff, we'll leave the buffers as they are
    // (we just have to make sure to supersize the buffers if we start 3D
    // rendering again)

    lpUnlockData->ddRVal = DD_OK;

    DDENDTICK(SURF4_UNLOCK);
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} /* Unlock32 */

//---------------------------------------------------------------------------

/*
 * WaitForVerticalBlank32
 */
// Old macro used when accessing raster register through the RM
#if 0
#define IS_IN_VBLANK(a) ((a)==0 || (a)>=(DWORD)GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head))
#endif

// Macros used when accessing raster register directly
#define MASK_IN_VBLANK(a) (a & 0x00010000)
#define MASK_SCANLINE(a) (a & 0x000007FF)
#define VBLANK_REGISTER_OFFSET  0x600808    // Physical head 0, add 0x2000 for head 1

DWORD __stdcall WaitForVerticalBlank32 (LPDDHAL_WAITFORVERTICALBLANKDATA lpWaitForVerticalBlank)
{
    LONGLONG timeStart;
    LONGLONG timeFrequency;
    NV_CFGEX_CURRENT_SCANLINE_MULTI_PARAMS cfgParams;
    BOOL timeOut;
    #ifdef WINNT
    int dwTimeoutCount;
    #endif

    dbgTracePush ("WaitForVerticalBlank32");

    DDSTARTTICK(DEVICE_WAITVBL);
    nvSetDriverDataPtrFromDDGbl (lpWaitForVerticalBlank->lpDD);

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            lpWaitForVerticalBlank->ddRVal = DD_OK;
            DDENDTICK(DEVICE_WAITVBL);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

#ifdef WINNT
    // under WINNT, we have a virtual desktop which spans both monitors, and it is not really possible
    // to make this work as a head specific thing, so just pick a head.
    cfgParams.Head = GET_CURRENT_HEAD();
    dwTimeoutCount = 1000000;
#else
    cfgParams.Head = pDXShare->dwPhysicalHeadNumber;
#endif

    if (GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head) == 0) {
        // can have this degenerate case in win2k, a call into this routine without nvEnable being called
        // at a minimum, we need to init some of these display timing structures
        getDisplayDuration();
    }

    timeOut = FALSE;

    // **** IMPORTANT ****
    // The following piece of code could break with future versions of the chip
    // if the raster register is moved.  Unfortunately, accessing this register directly
    // is the only way to limit the time spent in this function and allow the
    // WHQL Surface Flipping test to succeed.
    // 02/02/2001 PG/Video group

    volatile DWORD* npNvRasterRegister;
    volatile DWORD dwRaster = 0;

    npNvRasterRegister = (DWORD*) pDriverData->NvBaseFlat;
    npNvRasterRegister += ((VBLANK_REGISTER_OFFSET + (cfgParams.Head == 1 ? 0x2000 : 0)) >> 2);

    switch ( lpWaitForVerticalBlank->dwFlags ) {

        case DDWAITVB_I_TESTVB:
            /*
             * if testvb is just a request for the current vertical blank status
             */
            lpWaitForVerticalBlank->ddRVal = DD_OK;

            /*
             * Monitor is on
             */

            // Read the raster register directly
            dwRaster = *npNvRasterRegister;

            lpWaitForVerticalBlank->bIsInVB = (MASK_IN_VBLANK(dwRaster) ? TRUE : FALSE);  // Bit 16 is one when in the VBLANK

            DDENDTICK(DEVICE_WAITVBL);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);

        case DDWAITVB_BLOCKBEGIN:

            // The following code is for debug of 20001211-143119.
            // I am having trouble accessing this fields using softice.
            //   ... and I don't know why. -mlavoie
            {
                DWORD physicalModeHeight = (DWORD)GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head);
                DWORD physicalModeHeight2 = pDriverData->flipPrimaryRecord.nvTimingInfo[cfgParams.Head].VDisplayEnd;

            }

            /*
             * Monitor is ON
             */
            /*
             * if blockbegin is requested we wait until the vertical blank
             * is over, and then wait for the display period to end.
             */
            // get start time for timeouts (display may be disabled)
            QueryPerformanceFrequency((LARGE_INTEGER *)&timeFrequency);
            QueryPerformanceCounter((LARGE_INTEGER*)&timeStart);

            dwRaster = *npNvRasterRegister;
            cfgParams.RasterPosition = MASK_SCANLINE(dwRaster);  // Keep the raster position (bits 10:0)

            while (MASK_IN_VBLANK(dwRaster) && !timeOut) {
                dwRaster = *npNvRasterRegister;
                cfgParams.RasterPosition = MASK_SCANLINE(dwRaster);  // Keep the raster position (bits 10:0)
#ifdef WINNT
                if (dwTimeoutCount-- <= 0)
                    timeOut = TRUE;
#else
                LONGLONG timeNow;
                QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
                if ((timeNow - timeStart) >= (timeFrequency / 20)) timeOut = TRUE;  // timout of 1/20th of a second
#endif
            }
            while (!MASK_IN_VBLANK(dwRaster) && !timeOut) {
                dwRaster = *npNvRasterRegister;
                cfgParams.RasterPosition = MASK_SCANLINE(dwRaster);  // Keep the raster position (bits 10:0)
#ifdef WINNT
                if (dwTimeoutCount-- <= 0)
                    timeOut = TRUE;
#else
                LONGLONG timeNow;
                QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
                if ((timeNow - timeStart) >= (timeFrequency / 20)) timeOut = TRUE;  // timout of 1/20th of a second
#endif
            }

            lpWaitForVerticalBlank->ddRVal = DD_OK;
            DDENDTICK(DEVICE_WAITVBL);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);

        case DDWAITVB_BLOCKEND:

            /*
             * Monitor is ON
             */
            /*
             * if blockend is requested we wait for the vblank interval to end.
             */
            // get start time for timeouts (display may be disabled)
            QueryPerformanceFrequency((LARGE_INTEGER *)&timeFrequency);
            QueryPerformanceCounter((LARGE_INTEGER*)&timeStart);

            dwRaster = *npNvRasterRegister;
            cfgParams.RasterPosition = MASK_SCANLINE(dwRaster);  // Keep the raster position (bits 10:0)

            while (!MASK_IN_VBLANK(dwRaster) && !timeOut) {
                dwRaster = *npNvRasterRegister;
                cfgParams.RasterPosition = MASK_SCANLINE(dwRaster);  // Keep the raster position (bits 10:0)
#ifdef WINNT
                if (dwTimeoutCount-- <= 0)
                    timeOut = TRUE;
#else
                LONGLONG timeNow;
                QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
                if ((timeNow - timeStart) >= (timeFrequency / 20)) timeOut = TRUE;  // timout of 1/20th of a second
#endif
            }
            while (MASK_IN_VBLANK(dwRaster) && !timeOut) {
                dwRaster = *npNvRasterRegister;
                cfgParams.RasterPosition = MASK_SCANLINE(dwRaster);  // Keep the raster position (bits 10:0)
#ifdef WINNT
                if (dwTimeoutCount-- <= 0)
                    timeOut = TRUE;
#else
                LONGLONG timeNow;
                QueryPerformanceCounter((LARGE_INTEGER*)&timeNow);
                if ((timeNow - timeStart) >= (timeFrequency / 20)) timeOut = TRUE;  // timout of 1/20th of a second
#endif
            }

            lpWaitForVerticalBlank->ddRVal = DD_OK;
            DDENDTICK(DEVICE_WAITVBL);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
    }

    DDENDTICK(DEVICE_WAITVBL);
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_NOTHANDLED);

} /* WaitForVerticalBlank32 */

//---------------------------------------------------------------------------

/*
 * GetScanLine32
 */
DWORD __stdcall GetScanLine32( LPDDHAL_GETSCANLINEDATA lpGetScanLine )
{
    volatile DWORD* npNvRasterRegister;
    volatile DWORD dwRaster;
    NV_CFGEX_CURRENT_SCANLINE_MULTI_PARAMS cfgParams;
#if 0
    DWORD dwModeHeight;
#endif

    dbgTracePush ("GetScanLine32");

    DDSTARTTICK(DEVICE_GETSCANLINE);
    nvSetDriverDataPtrFromDDGbl (lpGetScanLine->lpDD);

    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE) {
            nvD3DRecover();
        }
        else {
            lpGetScanLine->ddRVal = DDERR_VERTICALBLANKINPROGRESS;
            DDENDTICK(DEVICE_GETSCANLINE);
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return(DDHAL_DRIVER_HANDLED);
        }
    }

    /*
     * If a vertical blank is in progress the scan line is in
     * indeterminant. If the scan line is indeterminant we return
     * the error code DDERR_VERTICALBLANKINPROGRESS.
     * Otherwise we return the scan line and a success code
     */
#ifdef WINNT
    // under WINNT, we have a virtual desktop which spans both monitors, and it is not really possible
    // to make this work as a head specific thing, so just pick the primary head.
    cfgParams.Head = GET_CURRENT_HEAD();
    if (GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head) == 0) {
         // can have this degenerate case in win2k, a call into this routine without nvEnable being called
        // at a minimum, we need to init some of these display timing structures
        getDisplayDuration();

    }
#if 0
    dwModeHeight = (DWORD)GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head);

    if ((ppdev->ulDesktopMode & NV_SPANNING_MODE) &&
         ppdev->TwinView_Orientation == VERTICAL_ORIENTATION) {
        dwModeHeight >>= 1;
    }
#endif

#else
    // RM needs physical head, map logical to physical
    NVTWINVIEWDATA twinData;
    twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
    twinData.dwAction = NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD;
    twinData.dwCrtc   = pDXShare->dwHeadNumber;
    MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);

    cfgParams.Head    = twinData.dwCrtc;
    if (GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head) == 0) {
        // can have this degenerate case in win2k, a call into this routine without nvEnable being called
        // at a minimum, we need to init some of these display timing structures
        getDisplayDuration();
    }
#if 0
    dwModeHeight = (DWORD)GET_MODE_PHYSICAL_HEIGHT(cfgParams.Head);

    // not really possible to make this work correctly for both heads in clone mode, one will always be wrong
    NvRmConfigGetEx (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
        NV_CFGEX_CURRENT_SCANLINE_MULTI, &cfgParams, sizeof(cfgParams));

    lpGetScanLine->dwScanLine = cfgParams.RasterPosition;
#endif

#endif
    // **** IMPORTANT ****
    // The following piece of code could break with future versions of the chip
    // if the raster register is moved.  Unfortunately, accessing this register directly
    // is the only way to limit the time spent in this function and allow the
    // WHQL Read Scanline test to succeed.
    // Kept the old way of doing for future reference or in case we can ever go back to it.
    // 02/01/2001 PG/Video Group

    // Read the raster register directly
    npNvRasterRegister = (DWORD*) pDriverData->NvBaseFlat;
    npNvRasterRegister += ((VBLANK_REGISTER_OFFSET + (cfgParams.Head == 1 ? 0x2000 : 0)) >> 2);
    dwRaster = *npNvRasterRegister;
    lpGetScanLine->dwScanLine = MASK_SCANLINE(dwRaster);  // Keep the raster position (bits 10:0)

    if (MASK_IN_VBLANK(dwRaster))  // Bit 16 is one when in the VBLANK
        lpGetScanLine->ddRVal = DDERR_VERTICALBLANKINPROGRESS;
    else
        lpGetScanLine->ddRVal = DD_OK;

    DDENDTICK(DEVICE_GETSCANLINE);
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} /* GetScanLine32 */

//---------------------------------------------------------------------------

/*
 * SetSurfaceColorKey32
 */

DWORD __stdcall SetSurfaceColorKey32(LPDDHAL_SETCOLORKEYDATA lpSetColorKey)
{
    LPDDRAWI_DIRECTDRAW_GBL   pdrv;
    LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl;

    dbgTracePush ("SetSurfaceColorKey32: lpSetCK->lpDD=%08x, lpSetCK->lpDDSurface = %08x",
                  lpSetColorKey->lpDD, lpSetColorKey->lpDDSurface);

    DDSTARTTICK(SURF4_SETCOLORKEY);
    nvSetDriverDataPtrFromDDGbl (lpSetColorKey->lpDD);

    pdrv = lpSetColorKey->lpDD;
    pDDSLcl = lpSetColorKey->lpDDSurface;

    if (lpSetColorKey->dwFlags == DDCKEY_SRCBLT) {
        pDDSLcl->ddckCKSrcBlt.dwColorSpaceLowValue  = lpSetColorKey->ckNew.dwColorSpaceLowValue & pDriverData->physicalColorMask;
        // Use no mask on this one on NV3 so that our D3D driver can get all the bits when surface is 565
        pDDSLcl->ddckCKSrcBlt.dwColorSpaceHighValue = lpSetColorKey->ckNew.dwColorSpaceHighValue;
        CNvObject *pNvObj = GET_PNVOBJ (pDDSLcl);
        if (IS_VALID_PNVOBJ(pNvObj) && (pNvObj->getClass() == CNvObject::NVOBJ_TEXTURE)) {
            CTexture *pTex = pNvObj->getTexture();
            DWORD     dwColorKey;
            BOOL      bRV;
            bRV = nvCalculateColorKey (&dwColorKey, pDDSLcl, pTex->getFormat());
            nvAssert (bRV);
            pTex->tagColorKey();
            pTex->setColorKey (dwColorKey);
        }
    }

    else if (lpSetColorKey->dwFlags == DDCKEY_DESTOVERLAY) {
        pDDSLcl->ddckCKDestOverlay.dwColorSpaceLowValue  = lpSetColorKey->ckNew.dwColorSpaceLowValue & pDriverData->physicalColorMask;
        pDDSLcl->ddckCKDestOverlay.dwColorSpaceHighValue = lpSetColorKey->ckNew.dwColorSpaceHighValue & pDriverData->physicalColorMask;
    }

    else {
        lpSetColorKey->ddRVal = DDERR_UNSUPPORTED;
        DDENDTICK(SURF4_SETCOLORKEY);
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DDHAL_DRIVER_HANDLED);
    }

    lpSetColorKey->ddRVal = DD_OK;
    DDENDTICK(SURF4_SETCOLORKEY);
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}


/********************************************************************************
 *
 * FUNCTION: GetK32ProcAddress
 *
 * DESC: A helper routine for getting the Win16 mutex.
 *
 *
 * Win32 code to import by ordinal from KERNEL32.DLL in Windows 95
 *
 * Andrew Schulman
 * andrew@ora.com
 * http://www.ora.com/windows/
 * ftp://ftp.ora.com/pub/examples/windows/win95.update/schulman.html
 * August 1995
 *
 ********************************************************************************/

#include "k32exp.h"

#define ENEWHDR     0x003CL         /* offset of new EXE header */
#define EMAGIC      0x5A4D          /* old EXE magic id:  'MZ'  */
#define PEMAGIC     0x4550          /* NT portable executable */

#define GET_DIR(x)  (hdr->OptionalHeader.DataDirectory[x].VirtualAddress)

static DWORD WINAPI GetK32ProcAddress(HMODULE hMod, int ord)
{
    IMAGE_NT_HEADERS       *hdr;
    IMAGE_EXPORT_DIRECTORY *exp;
    DWORD                  *AddrFunc;
    WORD                   enewhdr, *pw;
    BYTE                   *moddb;

    dbgTracePush ("GetK32ProcAddress");

    moddb = (BYTE *) hMod;
    pw = (WORD *) &moddb[0];
    if (*pw != EMAGIC) {
        dbgTracePop();
        return 0;
    }
    pw = (WORD *) &moddb[ENEWHDR];
    enewhdr = *pw;
    pw = (WORD *) &moddb[enewhdr];
    if (*pw != PEMAGIC) {
        dbgTracePop();
        return 0;
    }
    hdr = (IMAGE_NT_HEADERS *) pw;

    // Note: offset from moddb, *NOT* from hdr!
    exp = (IMAGE_EXPORT_DIRECTORY *) (((DWORD) moddb) +
                                      ((DWORD) GET_DIR(IMAGE_DIRECTORY_ENTRY_EXPORT)));
    AddrFunc = (DWORD *) (moddb + (DWORD) exp->AddressOfFunctions);

    // should verify that e.g.:
    // GetProcAddress(nvProcessGlobalData.win9x.hmod, "VirtualAlloc") == GetK32ProcAddress(710);

    ord--;  // table is 0-based, ordinals are 1-based
    if (ord < (int)exp->NumberOfFunctions) {
        dbgTracePop();
        return ((DWORD)(moddb + AddrFunc[ord]));
    }

    dbgTracePop();
    return (0);
}

#ifndef WINNT
/********************************************************************************
 *
 * FUNCTION: nvGetWin16MutexProcAddresses
 *
 * DESC: Get a pointer to the Win16 mutex routines in kernel32.dll.
 *
 ********************************************************************************/
typedef void (WINAPI *GETPWIN16MUTEX)(DWORD *);
typedef void (WINAPI *ENTERSYSLEVEL)(DWORD);
typedef void (WINAPI *LEAVESYSLEVEL)(DWORD);

BOOL nvGetWin16MutexProcAddresses(LPDWORD         win16Mutex,
                                  GETPWIN16MUTEX *lpfnGetpWin16Mutex,
                                  ENTERSYSLEVEL  *lpfnEnterSysLevel,
                                  ENTERSYSLEVEL  *lpfnLeaveSysLevel)
{
    HMODULE hMod = NULL;

    dbgTracePush ("nvGetWin16MutexProcAddresses");

    *win16Mutex         = 0;
    *lpfnGetpWin16Mutex = NULL;
    *lpfnEnterSysLevel  = NULL;
    *lpfnLeaveSysLevel  = NULL;

    hMod = GetModuleHandle("KERNEL32");

    if (hMod == NULL) { // still
        dbgTracePop();
        return FALSE;
    }

    *lpfnGetpWin16Mutex = (GETPWIN16MUTEX)GetProcAddress(hMod, "GetpWin16Lock");
    *lpfnEnterSysLevel  = (ENTERSYSLEVEL) GetProcAddress(hMod, "_EnterSysLevel");
    *lpfnLeaveSysLevel  = (LEAVESYSLEVEL) GetProcAddress(hMod, "_LeaveSysLevel");

    if (*lpfnGetpWin16Mutex == NULL) {
        *lpfnGetpWin16Mutex = (GETPWIN16MUTEX)GetK32ProcAddress(hMod, GETPWIN16MUTEX_ORD);
        *lpfnEnterSysLevel  = (ENTERSYSLEVEL) GetK32ProcAddress(hMod, ENTERSYSLEVEL_ORD);
        *lpfnLeaveSysLevel  = (LEAVESYSLEVEL) GetK32ProcAddress(hMod, LEAVESYSLEVEL_ORD);
    }

    if (*lpfnGetpWin16Mutex) {
        (*(*lpfnGetpWin16Mutex))(win16Mutex);
        dbgTracePop();
        return TRUE;
    }
    dbgTracePop();
    return (FALSE);
}

//---------------------------------------------------------------------------

// enumerate all the NVIDIA boards and store their pDXShare pointers in the g_adapterData table.
// returns TRUE if an nvidia display adapter is found and enabled, false otherwise.

BOOL nvFillAdapterTable (void)
{
    dbgTracePush ("nvFillAdapterTable");

    BOOL bEnabled = FALSE;

    if (global.pfEnumDisplayDevices) {

        DISPLAY_DEVICE DisplayDevice;
        DisplayDevice.cb = sizeof(DisplayDevice);

        for (DWORD dwDevNum=0; global.pfEnumDisplayDevices(NULL, dwDevNum, &DisplayDevice, 0); dwDevNum++) {

            if (DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {

                // create a DC for this display
                HDC hDC = CreateDC (NULL, (LPCSTR)DisplayDevice.DeviceName, NULL, NULL);

                if (hDC) {

                    // make sure this is an NVidia display driver
                    DISPDRVDIRECTXCOMMON *pThisDXShare = 0;

                    if (ExtEscape (hDC, NV_ESC_GET_FLAT_DIRECTX_SHARED_DATA, 0, NULL, 8, (LPSTR) &pThisDXShare) && pThisDXShare) {

                        // add this pDXShare to our g_adapterData table
                        g_adapterData[dwDevNum].pDXShare = pThisDXShare;

                        // only return TRUE if the current pDXShare belongs to an active driver
                        if (pDXShare == pThisDXShare) {
                            bEnabled = TRUE;
                        }

                        // get logical to physical head mapping
                        NVTWINVIEWDATA twinData;
                        twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
                        twinData.dwAction = NVTWINVIEW_ACTION_MAP_LOG_TO_PHYS_HEAD;
                        twinData.dwCrtc   = pThisDXShare->dwHeadNumber;
                        ExtEscape(hDC, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);
                        pThisDXShare->dwPhysicalHeadNumber = twinData.dwCrtc;
                    } else {

                        // not an NVIDIA driver
                        g_adapterData[dwDevNum].pDXShare    = NULL;
                        g_adapterData[dwDevNum].pDriverData = NULL;

                    }

                    DeleteDC (hDC);

                } // if (hDC)

            } else {

                // not attached to desktop (inactive)
                g_adapterData[dwDevNum].pDXShare    = NULL;
                g_adapterData[dwDevNum].pDriverData = NULL;

            }

        }  // for

    }

    else {

        // Windows 95 - no multi-mon
        bEnabled = TRUE;
        g_adapterData[0].pDXShare = pDXShare;

    }

    dbgTracePop();
    return (bEnabled);
}

//---------------------------------------------------------------------------

// return a DC handle for the specified device.

HDC nvCreateDCHandle (DISPDRVDIRECTXCOMMON *pDXS)
{
    dbgTracePush ("nvCreateDCHandle");

    if (global.pfEnumDisplayDevices) {

        DISPLAY_DEVICE DisplayDevice;
        DisplayDevice.cb = sizeof(DisplayDevice);

        for (DWORD dwDevNum=0; global.pfEnumDisplayDevices(NULL, dwDevNum, &DisplayDevice, 0); dwDevNum++) {

            if (DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {

                // create a DC for this display
                HDC hDC = CreateDC (NULL, (LPCSTR)DisplayDevice.DeviceName, NULL, NULL);

                if (hDC) {

                    DISPDRVDIRECTXCOMMON *pThisDXShare = 0;

                    if (ExtEscape (hDC, NV_ESC_GET_FLAT_DIRECTX_SHARED_DATA, 0, NULL, 8, (LPSTR)&pThisDXShare)) {
                        if (pDXS == pThisDXShare) {
                            // success! found the DC for this display.
                            dbgTracePop();
                            return (hDC);
                        }
                    }

                    DeleteDC (hDC);

                } // if (hDC)

            } // if (ATTACHED)

        } // for

    }

    else {

        // Windows 95 - no multi-mon
        dbgTracePop();
        return (GetDC (NULL));

    }

    DPF ("failed to find hDC!");

    dbgTracePop();
    return (NULL);
}

//--------------------------------------------------------------------------

// free the given DC handle if necessary

void nvDestroyDCHandle (HDC hDC)
{
    dbgTracePush ("nvDestroyDCHandle");

    if ((global.pfEnumDisplayDevices) && (hDC)) {
        DeleteDC (hDC);
    }
    // else win95, no need to delete...

    dbgTracePop();
}
#endif // !WINNT

/***************************************************************************
 *DestroyDriver32
 ***************************************************************************/
#ifdef VTUNE_SUPPORT
void nvVTuneFlushData (void);
#endif

/*
 * Under NT, DestroyDriver32 does not exist as a HAL function. However,
 * we still need to perform its cleanup tasks when DX is disabled.
 * So the NT display driver calls this routine when DX is turned off.
 */
#ifdef WINNT
HRESULT __stdcall DestroyDriver32(GLOBALDATA *pDrvData)
#else // !WINNT
HRESULT __stdcall DestroyDriver32(LPDDHAL_DESTROYDRIVERDATA pDestroyDriverData)
#endif  // !WINNT
{
    LPDDRAWI_DIRECTDRAW_GBL     pDDGbl;
    DWORD                       index = 0;
    DWORD                       i;
    DWORD                       retval = DDHAL_DRIVER_HANDLED;
    dbgTracePush ("DestroyDriver32");

#ifdef WINNT

    pDDGbl = NULL; // Under NT this is never used.
    NvAcquireSemaphore((CDriverContext *)pDrvData);
    pDriverData = (CDriverContext *)pDrvData;

#endif

    // must flush all before destroying anything
    if (getDC()->nvPusher.isValid()) {
        getDC()->nvPusher.flush (TRUE, CPushBuffer::FLUSH_WITH_DELAY);
        while (getFlipStatusPrimary(FLIP_ANYSURFACE) != DD_OK) {
            NV_SLEEPFLIP;
        }
    }

#ifdef KPFS
#if (NVARCH >= 0x20)
    pDriverData->nvKelvinPM.destroy();
#endif
#endif

#if IS_WIN9X

    DWORD                       win16Mutex;
    GETPWIN16MUTEX              lpfnGetpWin16Mutex;
    ENTERSYSLEVEL               lpfnEnterSysLevel;
    ENTERSYSLEVEL               lpfnLeaveSysLevel;
    DCICMD sMyDciCmd;

    // if full-screen DOS happened, destroy all the contexts as we don't get any calls from MS to do this, bad MS.
    if (pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_RECOVERYNEEDED_D3D) {
        D3DHAL_CONTEXTDESTROYALLDATA cdd;
        cdd.dwPID = ~0;
        nvContextDestroyAll(&cdd);
    }

    nvSetDriverDataPtrFromDDGbl (pDestroyDriverData->lpDD);
    pDDGbl = pDestroyDriverData->lpDD;

    // Assume call will succeed, will flag errors when they occur.
    pDestroyDriverData->ddRVal = DD_OK;

    pDriverData->dwDrvRefCount--;
    if (pDriverData->dwDrvRefCount != 0) {
        DWORD dwRoot;
        getAdapterIndex(pDXShare, &dwRoot);
        if (dwRoot == -1) {     // assume this is the AGP device
            // free the AGP push buffer here, a PCI push buffer should auto-create for the other device.
            nvDisable32();
        }

        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (retval);
    }

    // Call back to 16 bit direct draw code so it knows that
    // DirectX is going away.  Do this after the DC's are released.
    sMyDciCmd.dwVersion = DD_VERSION;
    sMyDciCmd.dwCommand = DDNVDESTROYDRIVEROBJECT;
    MyExtEscape(pDXShare->dwHeadNumber, DCICOMMAND, sizeof(DCICMD), (const char *)&sMyDciCmd, 0, 0);

    nvGetWin16MutexProcAddresses(&win16Mutex, &lpfnGetpWin16Mutex,
                                 &lpfnEnterSysLevel, &lpfnLeaveSysLevel);

    // grab win16 mutex
    if (lpfnEnterSysLevel) {
        (*(lpfnEnterSysLevel))(win16Mutex);
    }

#endif // !WINNT

#ifdef VTUNE_SUPPORT
    nvVTuneFlushData();
#endif

#ifdef ENABLE_VPP_DISPATCH_CODE
#ifndef WINNT
    vppDestroyDispatcher(&(pDriverData->vpp));
#endif // !WINNT
#endif

#ifdef WINNT
    //walk the pcontext list and deactivate ALL AA setup.
    //this is to fixe a strange AA config problem in MS Baseball
    PNVD3DCONTEXT pContext = (PNVD3DCONTEXT)pDriverData->dwContextListHead;
    while (pContext)
    {
        if(pContext->pDriverData == pDriverData){
            if(pContext->aa.isEnabled()) nvCelsiusAADestroy(pContext);
        }
        pContext = pContext->pContextNext; // next
    }
#endif

    // make sure all floating context DMA's are freed
    DWORD dwContextDma;
    for (i=0; i < pDriverData->bltData.dwSystemSurfaceContextDMAIndex; i+=2) {
        dwContextDma = NV_DD_FLOATING_CONTEXT_DMA_BASE + i;
        // HWFIX: deal with hw hash table bug, swap bits 1 and 2 with 11 and 22 (which are part of the hash function)
        dwContextDma = ((dwContextDma & 0x00000002) << 10) | ((dwContextDma & 0x00000800) >> 10) |
                       ((dwContextDma & 0x00000004) << 20) | ((dwContextDma & 0x00400000) >> 20) |
                       (dwContextDma & ~0x00400806);
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, dwContextDma);
    }
    pDriverData->bltData.dwSystemSurfaceContextDMAIndex = 0;

    // deallocate all blt workspaces
    if (pDriverData->bltVidMemInfo.dwLocale == BLTWS_VIDEO) {
        NVHEAP_FREE(pDriverData->bltVidMemInfo.fpVidMem);
        pDriverData->bltVidMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }
    if (pDriverData->bltSysMemInfo.dwLocale == BLTWS_SYSTEM) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, pDriverData->bltSysMemInfo.dwContextDma);
        FreeIPM((void*)pDriverData->bltSysMemInfo.fpVidMem);
        pDriverData->bltSysMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }
    if (pDriverData->bltAGPMemInfo.dwLocale == BLTWS_AGP) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, pDriverData->bltAGPMemInfo.dwContextDma);
        nvAGPFree((void*)pDriverData->bltAGPMemInfo.fpVidMem);
        pDriverData->bltAGPMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    // Clear motion comp context
    pDriverData->dwMCNVMCSurface = 0;
    pDriverData->dwMCNVDSSurfaceBase = 0;
    pDriverData->dwMCCurrentCelsiusIDCTCtxDma = 0;

    for (i=0; i<4; i++) {
        pDriverData->dwMCIDCTAGPSurfaceBase[i] = 0;
        if (pDriverData->dwMCIDCTAGPCtxDmaSize[i] != 0) {
            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                     NV_DD_DMA_FLOATING_MC_IDCT0_CONTEXT_DMA_FROM_MEMORY+i);
            pDriverData->dwMCIDCTAGPCtxDmaSize[i] = 0;
        }
    }

    // If an overlay is currently active then shut it down
    if ((getDC()->nvPusher.isValid()) && (pDriverData->vpp.dwOverlaySurfaces > 0)) {

        FAST long videoFreeCount = pDriverData->NvVideoFreeCount;
        LPPROCESSINFO lpProcInfo = pmGetProcess(pDriverData, GetCurrentProcessId());

        // make sure we sync with other channels before writing put
        getDC()->nvPusher.setSyncChannelFlag();

        // Trash spare subchannel
        pDriverData->dwRingZeroMutex = TRUE;

        Sleep(VPP_TIMEOUT_TIME * 3);

        // Should not be talking directly to VPP objects -@mjl@
        nvPushData(0, dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData(1, NV_VPP_OVERLAY_IID);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_SPARE) | NV07A_STOP_OVERLAY(0) | 0x80000);
        nvPushData(3, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[0]
        nvPushData(4, NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE);  // StopOverlay[1]
        nvPusherAdjust(5);
        nvPusherStart(TRUE);

        pDriverData->dwRingZeroMutex = FALSE;
        pDriverData->dDrawSpareSubchannelObject = 0;

        GET_HALINFO()->ddCaps.dwCurrVisibleOverlays = 0;
    }

    pmDeleteAllProcesses(pDriverData);


    /*
     * This routine will insure that any allocated preapproved context is destroyed.
     */
    nvDestroyPreapprovedContexts();

#ifndef WINNT
    pDriverData->HALCallbacks.lpSetInfo = 0;
#endif  // WINNT

    if (!nvDisable32()) {
#ifdef WINNT
        retval = DDHAL_DRIVER_NOTHANDLED;
#else // !WINNT
        pDestroyDriverData->ddRVal = DDERR_GENERIC;
#endif // !WINNT
    }

    // Free any existing dynamic context dma objects
    if (pDriverData->dwVidMemCtxDmaSize != 0) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);
        pDriverData->dwVidMemCtxDmaSize = 0;
    }

    VppDestructor(&pDriverData->vpp);

    // Make sure we reset the current Celsius user to none
    pDriverData->dwMostRecentHWUser = MODULE_ID_NONE;

    if (pDriverData->NvDevVideoFlatPio != 0) {
        NvRmFree(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_DD_DEV_VIDEO);
        pDriverData->NvDevVideoFlatPio = 0;
    }

    // Let 16 bit Display Driver code know that Direct Draw is no longer active
    pDriverData->DDrawVideoSurfaceCount = 0;

    pDriverData->fNvActiveFloatingContexts = 0;

#ifdef WINNT
    if (pDriverData->GARTLinearBase) {
        getDC()->defaultVB.destroy();
#ifndef NV_AGP
        NvWin2KUnmapAgpHeap(ppdev);
#endif // !NV_AGP
    }
#else // !WINNT
#ifndef NV_AGP
    // Reset AGP GART addresses
    pDriverData->GARTLinearBase = 0;
    pDriverData->GARTPhysicalBase = 0;
#endif // !NVAGP
#endif // !WINNT

    pDriverData->dwRingZeroMutex = FALSE;

#ifndef WINNT   // WINNT BUG
    pMySurfaces = NULL;

    // release win16 mutex
    if (lpfnLeaveSysLevel) {
        (*(lpfnLeaveSysLevel))(win16Mutex);
    }
#endif // !WINNT

    // delete the inner loop table entries
    for(i = 0; i < DRAW_PRIM_TABLE_ENTRIES; i++)
    {
        // release inner loops
        CILHashEntry *pEntry = (CILHashEntry*)dwDrawPrimitiveTable[i];
        while (pEntry)
        {
            CILHashEntry *pNext = pEntry->getNext();
            delete pEntry;
            pEntry = pNext;
        }
    }

    nvPatchDestroyFD();

    // clear the inner loop lookup table to zero
    memset ((void *)dwDrawPrimitiveTable, 0, DRAW_PRIM_TABLE_ENTRIES<<2);
    global.celsius.dwLoopCache = 0;
#if (NVARCH >= 0x20)
    global.kelvin.dwLoopCache = 0;
#endif
    // also clear the compiled memcopy routines
    memset (global.adwMemCopy,0,sizeof(global.adwMemCopy));

    // finally free the memory
    if (global.dwILCData) {
        // kill compiled state variables
        FreeIPM(global.dwILCData);
        global.dwILCData  = 0;
        global.dwILCCount = 0;
        global.dwILCMax   = 0;
    }


#ifdef CNT_TEX

    HANDLE hFile = CreateFile("\\TexDL.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);

    char cTempBuf[64];
    DWORD dwTotVid, dwTotAgp, dwTot;
    dwTotVid = 0;
    dwTotAgp = 0;
    SetFilePointer(hFile, 0,0,FILE_END);
    if (g_dwFrames > MAX_FRAME_CNT)
        g_dwFrames = MAX_FRAME_CNT;

    for (int iFrmCnt=0; iFrmCnt < (int)g_dwFrames; iFrmCnt++) {
        sprintf(cTempBuf, "\nFrame: %d", iFrmCnt);
        WriteFile(hFile, cTempBuf, strlen(cTempBuf), &dwTot, NULL);
        sprintf(cTempBuf, "\n\tNumber of Video Texture Downloads: %d", g_dwVidTexDL[iFrmCnt]);
        WriteFile(hFile, cTempBuf, strlen(cTempBuf), &dwTot, NULL);
        sprintf(cTempBuf, "\n\tNumber of AGP Texture Downloads: %d\n", g_dwAgpTexDL[iFrmCnt]);
        WriteFile(hFile, cTempBuf, strlen(cTempBuf), &dwTot, NULL);

        dwTotVid += g_dwVidTexDL[iFrmCnt];
        dwTotAgp += g_dwAgpTexDL[iFrmCnt];
        g_dwVidTexDL[iFrmCnt] = 0;
        g_dwAgpTexDL[iFrmCnt] = 0;
    }

    // print out totals
    sprintf(cTempBuf, "\nTotal Number of Frames: %d", g_dwFrames);
    WriteFile(hFile, cTempBuf, strlen(cTempBuf), &dwTot, NULL);
    sprintf(cTempBuf, "\nTotal Number of Video Texture Downloads: %d", dwTotVid);
    WriteFile(hFile, cTempBuf, strlen(cTempBuf), &dwTot, NULL);
    sprintf(cTempBuf, "\nTotal Number of AGP Texture Downloads: %d\n", dwTotAgp);
    WriteFile(hFile, cTempBuf, strlen(cTempBuf), &dwTot, NULL);

    CloseHandle(hFile);

    g_dwFrames = 0;
#endif

    // bAssertModeDirectDraw checks this to know if DriverInit() needs to be called again
    // in some modeset sequences.  The initialization sequence on Win9x requires that we
    // do not clear this however... *sigh*  it's a mess.
#if IS_WINNT5
    pDriverData->dwRootHandle = 0;
#endif

#if IS_WIN9X
    // kill timer proc
    if (pDriverData->dwTVTunerTimer) {
        KillTimer(NULL, pDriverData->dwTVTunerTimer);
    }

    // compact the heap and display usage
    g_nvIPHeap.checkHeapUsage();

    // done
    pDestroyDriverData->ddRVal = DD_OK;
#endif // IS_WIN9X

#ifdef NVSTATDRIVER
            DetachNVStat();
#endif // NVSTATDRIVER

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (retval);
} // DestroyDriver32

//---------------------------------------------------------------------------

/*
 * GetHeapAlignment32
 *
 * Returns more specific heap alignment requirements to DDRAW than
 * those described in the heap structure.
 */

DWORD __stdcall GetHeapAlignment32 (LPDDHAL_GETHEAPALIGNMENTDATA lpGhaData)
{
    dbgTracePush ("GetHeapAlignment32");

    lpGhaData->ddRVal= DD_OK;

    if (lpGhaData->dwHeap == 0) {
        lpGhaData->Alignment.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                              DDSCAPS_EXECUTEBUFFER |
                                              DDSCAPS_OVERLAY |
                                              DDSCAPS_TEXTURE |
                                              DDSCAPS_ZBUFFER |
                                              DDSCAPS_ALPHA |
                                              DDSCAPS_FLIP;

        /* Alignments in bytes */

        lpGhaData->Alignment.ExecuteBuffer.Linear.dwStartAlignment = 0;
        lpGhaData->Alignment.ExecuteBuffer.Linear.dwPitchAlignment = 0;

        lpGhaData->Alignment.Overlay.Linear.dwStartAlignment = NV_OVERLAY_BYTE_ALIGNMENT;
        lpGhaData->Alignment.Overlay.Linear.dwPitchAlignment = NV_OVERLAY_BYTE_ALIGNMENT;

        lpGhaData->Alignment.Texture.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign; /* Texture surfaces, not our optimized texture alignment */
        lpGhaData->Alignment.Texture.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

        lpGhaData->Alignment.ZBuffer.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign;
        lpGhaData->Alignment.ZBuffer.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

        lpGhaData->Alignment.AlphaBuffer.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign;
        lpGhaData->Alignment.AlphaBuffer.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

        lpGhaData->Alignment.Offscreen.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign;
        lpGhaData->Alignment.Offscreen.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

        lpGhaData->Alignment.FlipTarget.Linear.dwStartAlignment = pDriverData->dwSurfaceBitAlign;
        lpGhaData->Alignment.FlipTarget.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

    }
    else {
        DPF("D3D: GetHeapAlignment32 - invalid parameters 10");
        lpGhaData->ddRVal = DDERR_INVALIDPARAMS;
        dbgTracePop();
        return (DDHAL_DRIVER_NOTHANDLED);
    }

    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} /* GetHeapAlignment32 */

#ifndef WINNT
/*
 * This callback is Win9x specific - for its Winnt counterpart, see
 * GetDriverInfo32.GUID_UpdateNonLocalHeap.
 */

//---------------------------------------------------------------------------

/*
 * UpdateNonLocalHeap32
 *
 * Records actual AGP memory linear and physical addresses.
 */

DWORD __stdcall UpdateNonLocalHeap32( LPDDHAL_UPDATENONLOCALHEAPDATA lpd )
{
    dbgTracePush ("UpdateNonLocalHeap32");

    nvSetDriverDataPtrFromDDGbl (lpd->lpDD);

    // the heap has been ripped out from under us. mark it invalid so we don't try to use it
    nvAGPInvalidate();

    bltDestroyAllContextDMAs(lpd->lpDD);

    if (pDriverData->bltAGPMemInfo.dwLocale == BLTWS_SYSTEM) {
        NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle, pDriverData->bltAGPMemInfo.dwContextDma);
        nvAGPFree((void*)pDriverData->bltAGPMemInfo.fpVidMem);
        pDriverData->bltAGPMemInfo.dwLocale = BLTWS_UNINITIALIZED;
    }

    if (pDriverData->dwFullScreenDOSStatus & (FSDOSSTATUS_RECOVERYNEEDED_D3D | FSDOSSTATUS_RECOVERYNEEDED_DDRAW)) {
        if (!(pDriverData->dwFullScreenDOSStatus & FSDOSSTATUS_COMPLETE)) {
            lpd->ddRVal = DDERR_GENERIC;
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }
    }

#ifdef CAPTURE
    if ((getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) &&
        (getDC()->nvD3DRegistryData.regCaptureConfig & D3D_REG_CAPTURECONFIG_FORCEPCI)) {
        pDriverData->GARTPhysicalBase = 0;
        pDriverData->GARTLinearBase = 0;
    }
    else
#endif

    if ((lpd->dwHeap == AGP_HEAP) && (lpd->fpGARTLin != 0)) { // AGP heap

        // This is the linear non-local heap we use for DMA push buffers, textures and YV12 surfaces...
        pDriverData->GARTPhysicalBase = lpd->fpGARTDev;
        pDriverData->GARTLinearBase = lpd->fpGARTLin;

        // Let the resource manager know this information
        DWORD dwDummy;
        NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                       NV_CFG_AGP_PHYS_BASE, pDriverData->GARTPhysicalBase, &dwDummy);
        NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                       NV_CFG_AGP_LINEAR_BASE, pDriverData->GARTLinearBase, &dwDummy);

        if (!pDriverData->regMaxAGPLimit) {
            // MS is lazy about setting up MTRR's and sometimes this config get in DriverInit
            // will fail because they haven't done it yet.  Try to get it again here as a last
            // ditch attempt -- (this has the side affect of setting up some mapping variables
            // in pdev on the RM side).
            NvRmConfigGet (pDriverData->dwRootHandle, NV_WIN_DEVICE,
                           NV_CFG_AGP_LIMIT, &pDriverData->regMaxAGPLimit);
            if (!pDriverData->regMaxAGPLimit) {
                // we _still_ can't get the heap size. just revert to the default and pray...
                pDriverData->regMaxAGPLimit = NV_MAX_AGP_MEMORY_LIMIT;
                DPF ("falling back to default AGP heap size for lack of information from MS");
                dbgD3DError();
            }
        }

        // win9x fix for any system that sets an AGP aperature TOO large
        {
            DWORD dwMem = pDriverData->nvD3DPerfData.dwSystemMemory * 1024 * 1024 / 2 - 1;
            pDriverData->regMaxAGPLimit = min(pDriverData->regMaxAGPLimit, dwMem);
        }

        // Even though regMaxAGPLimit is accurate, we could be further limited by lpd->ulPolicyMaxBytes
        NvRmConfigSet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                       NV_CFG_AGP_LIMIT, pDriverData->regMaxAGPLimit, &dwDummy);

#ifdef NV_TRACKAGP
        __Reset();
#endif //NV_TRACKAGP

        // reset if we weren't yet enabled or if we had a valid
        // push buffer in AGP memory (which has been blown away)
        if ((getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED)
            ||
            (getDC()->nvPusher.isValid() && (getDC()->nvPusher.getHeap() == CSimpleSurface::HEAP_AGP))) {
            nvDisable32();
            if (!nvEnable32 (lpd->lpDD)) {
                NvReleaseSemaphore(pDriverData);
                lpd->ddRVal = DDERR_OUTOFMEMORY;
                dbgTracePop();
                return DDHAL_DRIVER_HANDLED;
            }
        }
    }

    NvReleaseSemaphore(pDriverData);
    lpd->ddRVal = DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} // UpdateNonLocalHeap32

#endif // !WINNT

//---------------------------------------------------------------------------

/*
 * GetAvailDriverMemory32
 *
 * Reports amount of memory that the driver is managing itself.
 */

DWORD __stdcall GetAvailDriverMemory32( LPDDHAL_GETAVAILDRIVERMEMORYDATA lpd )
{
    dbgTracePush ("GetAvailDriverMemory32");

    // We need to see if if there has been a mode set and re-init.
    // Although this routine does nothing with hardware, the offscreen
    // memory heap will not really be accurate if we don't. Fixes
    // WQHL bug where WHQL test allocates a bunch of surfaces and then
    // frees them and then checks to make sure that there is the
    // same amount of video memory before allocating as after freeing.
    // Because we allocate some extra video memory for a notifier in
    // NvDDEnable32 on the first createSurface call and don't free
    // the notifier when the last surface is destroyed (which is
    // perfectly fine!), WHQL thinks we are losing memory, but we aren't.
    // However, I fix it by checking if NV needs a reset here which
    // will force the notifier to allocate before the memory is sized.
    nvSetDriverDataPtrFromDDGbl (lpd->lpDD);

    // Make certain that we're enabled before reporting memory.
    // Our nvEnable code does do some minor allocations which we need
    // to account for before reporting to the app how much remains
    // available to it.
    if (getDC()->dwEnableStatus != CDriverContext::DC_ENABLESTATUS_ENABLED)
    {
        if (!nvEnable32 (lpd->lpDD)) {
            NvReleaseSemaphore(pDriverData);
            lpd->ddRVal = DDERR_OUTOFMEMORY;
            dbgTracePop();
            return DDHAL_DRIVER_HANDLED;
        }
    }

    // figure out what the caller requested
    BOOL bVidMem  = (lpd->DDSCaps.dwCaps & (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM))
                    == (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
    BOOL bAGPMem  = (lpd->DDSCaps.dwCaps & (DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM))
                    == (DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
    BOOL bTexture = (lpd->DDSCaps.dwCaps & DDSCAPS_TEXTURE) == DDSCAPS_TEXTURE;
    BOOL bZBufMem = (lpd->DDSCaps.dwCaps & DDSCAPS_ZBUFFER) == DDSCAPS_ZBUFFER;

    // Ths ZBuffer is always in video memory since the chip has to be able
    // to write to it. So, the request is really for vid mem.
    if (bZBufMem)
        bVidMem = TRUE;

    if (!bVidMem && !bAGPMem)
    {
        // if DDSCAPS_VIDEOMEMORY is set but the location is not specified we will return local vid mem only
        if (lpd->DDSCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
            bVidMem = TRUE;

        // when primary surface is specified, use local vid mem
        if (lpd->DDSCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
            bVidMem = TRUE;

        // off screen plain -> video only
        if (lpd->DDSCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN)
            bVidMem = TRUE;

        // only DDSCAPS_LOCALVIDMEM -> vid mem
        if (lpd->DDSCaps.dwCaps & DDSCAPS_LOCALVIDMEM)
            bVidMem = TRUE;

        // only DDSCAPS_NONLOCALVIDMEM -> agp mem
        if (lpd->DDSCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM)
            bAGPMem = TRUE;

        // only DDSCAPS_TEXTURE -> vid & agp (if it exists)
        if (bTexture)
        {
            bVidMem = TRUE;
            if (pDriverData->GARTLinearBase) {
                bAGPMem = TRUE;
            }
        }
    }

    nvCreateVidHeapVarsAndPseudoNotifier();

    lpd->dwTotal = 0;
    lpd->dwFree  = 0;

    //
    // local video memory
    //
    if (bVidMem)
    {
        // The driver is managing local video memory itself.
        // AGP memory is being managed by DirectX.  So just
        // return amount of local memory being managed.
        lpd->dwTotal += pDriverData->VideoHeapTotal - pDriverData->VideoHeapOverhead;
        lpd->dwFree  += pDriverData->VideoHeapFree;
    }

    //
    // PCI texture memory
    //
    if (bTexture
     && !bAGPMem
     && !pDriverData->GARTLinearBase)
    {
        if (getDC()->nvD3DTexHeapData.dwSizeMax == 0)
        {
             // If the current texture heap size is not initialized
             // yet, then this means D3D has not be fired up yet to
             // actually allocate the texture heap.  In this case
             // return the amount that is being requested.  Once D3D
             // is up and running, the memory sizes will be based on
             // what the heap size really is.
            DWORD dwHeapSize = getDC()->nvD3DRegistryData.regPCITexHeapSize ?
                               getDC()->nvD3DRegistryData.regPCITexHeapSize :
                               pDriverData->nvD3DPerfData.dwRecommendedPCITexHeapSize;
            lpd->dwTotal += dwHeapSize;
            lpd->dwFree  += dwHeapSize;
        }
        else
        {
            // Return the maximum and available size of the internal
            // PCI system memory texture heap
            lpd->dwTotal += getDC()->nvD3DTexHeapData.dwSizeMax;
            lpd->dwFree  += getDC()->nvD3DTexHeapData.dwFreeSize;
        }
    }

    //
    // AGP texture memory
    //
    if (bAGPMem)
    {
        // On AGP systems, the driver also manages textures, but in AGP memory.
        // Our driver allocates a chunk of AGP memory from the the DirectX
        // managed AGP heap. Then we sub-allocate from this chunk for our
        // textures. Hence, DirectX thinks there is less AGP memory available
        // than there actually is. We need to correct for that.
        DWORD total, free;

        nvAGPGetMemory (&total, &free);

#ifdef NV_AGP
        lpd->dwTotal += pDriverData->regMaxAGPLimit;
        lpd->dwFree  += pDriverData->regMaxAGPLimit - total + free;
#else // !NV_AGP
        lpd->dwTotal += 0;      // total is already correct
        lpd->dwFree  += free;   // we have this much free
#endif // !NV_AGP
    }

    lpd->ddRVal = DD_OK;
    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return(DDHAL_DRIVER_HANDLED);
}

//---------------------------------------------------------------------------

/* Safe procedures, basically if they are called nothing happens */
DWORD FAR PASCAL SafeSyncSurfaceData(LPDDHAL_SYNCSURFACEDATA lpInput)
{
    dbgTracePush ("SafeSyncSurfaceData");
    lpInput->ddRVal= DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

DWORD FAR PASCAL SafeSyncVideoPortData(LPDDHAL_SYNCVIDEOPORTDATA lpInput)
{
    dbgTracePush ("SafeSyncVideoPortData");
    lpInput->ddRVal= DD_OK;
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);
}

//---------------------------------------------------------------------------

/*
 * GetTranslatedOffset(DWORD lBrightness )
 *
 * Converts linear brightness into an offset to program into
 * CSC registers, this algorithm is non-linear and
 * includes two ranges.
 */
DWORD GetTranslatedOffset( unsigned long lBrightness)
{
    DWORD dwOffset;

    dbgTracePush ("GetTranslatedOffset");

    if (lBrightness>10000)
        lBrightness = 10,000;

    if (lBrightness<=750) {
        dwOffset = 1+ ( 0x3D * (lBrightness/749) );
    }
    else {
        dwOffset = (0x69 * (lBrightness/(10,000-750)));
    }

    dbgTracePop();
    return (dwOffset);
}

//---------------------------------------------------------------------------

#define NV_PVIDEO_RED_CSC 0x680280
#define NV_PVIDEO_GREEN_CSC 0x680284
#define NV_PVIDEO_BLUE_CSC 0x680288

DWORD GetOffsetTranslated()
{
    U032 * nvBase;
    U032 dwRedCsc;

    dbgTracePush ("GetOffsetTranslated");

    // all of this is based of the red csc value
    nvBase = (U032 *) (pDriverData->NvBaseFlat);

    dwRedCsc = nvBase[NV_PVIDEO_RED_CSC/4];

    if (dwRedCsc>=0x69) {// 750 to 0 range
        dbgTracePop();
        return(750 * ((dwRedCsc-0x69)/0x3E));
    }
    else {// 750-10,000 range
        dbgTracePop();
        return(750 + ((10000-750)* ((0x69-dwRedCsc)/0x69)) );
    }
}

//---------------------------------------------------------------------------

/*
 * Colour Control
 * Extended functionality of DirectDraw/VPE
 */
DWORD __stdcall ColourControl( LPDDHAL_COLORCONTROLDATA pvpcd )
{
    U032 *nvBase;

    dbgTracePush ("ColourControl");

    nvSetDriverDataPtrFromDDGbl (pvpcd->lpDD);

    if (IS_OVERLAY(pvpcd->lpDDSurface->ddsCaps.dwCaps, pvpcd->lpDDSurface->lpGbl->ddpfSurface.dwFourCC)) {

        if (pvpcd->dwFlags & DDRAWI_GETCOLOR ) {

            pvpcd->lpColorData->dwSize = sizeof(DDCOLORCONTROL);

            pvpcd->lpColorData->dwFlags = DDCOLOR_BRIGHTNESS;
            pvpcd->lpColorData->dwFlags |= DDCOLOR_CONTRAST;
            pvpcd->lpColorData->dwFlags |= DDCOLOR_SATURATION;
            pvpcd->lpColorData->dwFlags |= DDCOLOR_HUE;

            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                pvpcd->lpColorData->lBrightness = pDriverData->vpp.colorCtrl.lBrightness;
                pvpcd->lpColorData->lContrast = pDriverData->vpp.colorCtrl.lContrast;
                pvpcd->lpColorData->lHue = pDriverData->vpp.colorCtrl.lHue;
                pvpcd->lpColorData->lSaturation = pDriverData->vpp.colorCtrl.lSaturation;
            }
            else {
                // is in units of 1, maximum is 10,000
                pvpcd->lpColorData->lBrightness = GetOffsetTranslated();

                // contrast ranges from 0 to 20,000 (intended to be 0 to 200%)
                pvpcd->lpColorData->lContrast = min(pDriverData->vpp.colorCtrl.lContrast * 10000 / 255, 20000);

                // hue is in range of -180 to 180 degrees
                pvpcd->lpColorData->lHue = pDriverData->vpp.colorCtrl.lHue;
                while (pvpcd->lpColorData->lHue > 180)  pvpcd->lpColorData->lHue -= 360;
                while (pvpcd->lpColorData->lHue < -180) pvpcd->lpColorData->lHue += 360;

                // saturation ranges from 0 to 20,000
                pvpcd->lpColorData->lSaturation = min(pDriverData->vpp.colorCtrl.lSaturation * 10000 / 256, 20000);
            }

            pvpcd->ddRVal = DD_OK;
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        if (pvpcd->dwFlags & DDRAWI_SETCOLOR ) {
            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_CONTRAST) {
                    pDriverData->vpp.colorCtrl.lContrast = pvpcd->lpColorData->lContrast;
                }
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_BRIGHTNESS) {
                    pDriverData->vpp.colorCtrl.lBrightness = pvpcd->lpColorData->lBrightness;
                }
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_SATURATION) {
                    pDriverData->vpp.colorCtrl.lSaturation = pvpcd->lpColorData->lSaturation;
                }
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_HUE) {
                    pDriverData->vpp.colorCtrl.lHue = pvpcd->lpColorData->lHue;
                }
                VppSetOverlayColourControl(&(pDriverData->vpp));

                if (pvpcd->lpColorData->dwFlags & (DDCOLOR_BRIGHTNESS | DDCOLOR_CONTRAST | DDCOLOR_HUE | DDCOLOR_SATURATION)) {
                    pvpcd->ddRVal = DD_OK;
                    NvReleaseSemaphore(pDriverData);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }
            else {
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_BRIGHTNESS) {
                    DWORD dwCscOffset;
                    DWORD dwCscRed, dwCscGreen, dwCscBlue;

                    pDriverData->vpp.colorCtrl.lBrightness = min(pvpcd->lpColorData->lBrightness / 100, 255);
                    dwCscOffset = GetTranslatedOffset( pvpcd->lpColorData->lBrightness );

                    dwCscRed = 0x69     + dwCscOffset;
                    dwCscGreen = 0x3E   - dwCscOffset;
                    dwCscBlue  = 0x89   + dwCscOffset;

                    nvBase = (U032 *) (pDriverData->NvBaseFlat);

                    nvBase[NV_PVIDEO_RED_CSC/4] = dwCscRed;
                    nvBase[NV_PVIDEO_GREEN_CSC/4] = dwCscGreen;
                    nvBase[NV_PVIDEO_BLUE_CSC/4] = dwCscBlue;
                }
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_CONTRAST) {
                    pDriverData->vpp.colorCtrl.lContrast = min(pvpcd->lpColorData->lContrast * 255 / 10000, 511);
                }
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_HUE) {
                    pDriverData->vpp.colorCtrl.lHue = pvpcd->lpColorData->lHue;
                    while (pDriverData->vpp.colorCtrl.lHue < 0)    pDriverData->vpp.colorCtrl.lHue += 360;
                    while (pDriverData->vpp.colorCtrl.lHue >= 360) pDriverData->vpp.colorCtrl.lHue -= 360;
                }
                if (pvpcd->lpColorData->dwFlags & DDCOLOR_SATURATION) {
                    pDriverData->vpp.colorCtrl.lSaturation = min(pvpcd->lpColorData->lSaturation * 256 / 10000, 512);
                }
                if (pvpcd->lpColorData->dwFlags & (DDCOLOR_BRIGHTNESS | DDCOLOR_CONTRAST | DDCOLOR_HUE | DDCOLOR_SATURATION)) {
                    pvpcd->ddRVal = DD_OK;
                    NvReleaseSemaphore(pDriverData);
                    dbgTracePop();
                    return (DDHAL_DRIVER_HANDLED);
                }
            }
        }
    }

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_NOTHANDLED);
}

//---------------------------------------------------------------------------

/*
 * GetDriverInfo32
 *
 * Initialize Extended Functionality Classes
 */
DWORD __stdcall GetDriverInfo32(LPDDHAL_GETDRIVERINFODATA lpData)
{
    DWORD dwSize;

    dbgTracePush ("GetDriverInfo32");

#ifdef WINNT
    NvAcquireSemaphore((((PPDEV) (lpData->dhpdev))->pDriverData));
    pDriverData = (CDriverContext *) ((PPDEV)(lpData->dhpdev))->pDriverData;
#else // !WINNT
    pDXShare = (DISPDRVDIRECTXCOMMON *) lpData->dwContext;
    pDriverData = (CDriverContext *) pDXShare->pDirectXData;
#endif // !WINNT

    // set up pointer to DDHALINFO
    LPDDHALINFO pHalInfo = GET_HALINFO();

    lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;

    // please keep these GUIDs alphabetized. there are too
    // many of them to otherwise keep track of.

    if (IsEqualIID_C ((lpData->guidInfo), GUID_ColorControlCallbacks) &&
       (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10 || pDriverData->vpp.regOverlayColourControlEnable)) {
        DDHAL_DDCOLORCONTROLCALLBACKS ccCB;

        dwSize = lpData->dwExpectedSize;

        lpData->dwActualSize = sizeof( DDHAL_DDCOLORCONTROLCALLBACKS );

        ccCB.dwSize = (dwSize<lpData->dwActualSize)?
                      dwSize:lpData->dwActualSize;

        ccCB.dwFlags = DDHAL_COLOR_COLORCONTROL;
        ccCB.ColorControl = ColourControl;

        memcpy(lpData->lpvData, &ccCB, ccCB.dwSize );

        lpData->ddRVal = DD_OK;
    }

#ifndef WINNT
    if (IsEqualIID_C ((lpData->guidInfo), GUID_D3DCallbacks2))
    {
        D3DHAL_CALLBACKS2 D3DCallbacks2;

        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_D3DCallbacks2");

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0300);

        memset(&D3DCallbacks2, 0, sizeof(D3DHAL_CALLBACKS2));
        dwSize               = min(lpData->dwExpectedSize, sizeof(D3DHAL_CALLBACKS2));
        lpData->dwActualSize = sizeof(D3DHAL_CALLBACKS2);
        D3DCallbacks2.dwSize = dwSize;

        /*
         * Always export the SetRenderTarget callback.
         */
        D3DCallbacks2.dwFlags = D3DHAL2_CB32_SETRENDERTARGET;
        D3DCallbacks2.SetRenderTarget = nvSetRenderTarget;

        /*
         * Always export the D3D Clear callback.
         */
        D3DCallbacks2.dwFlags |= D3DHAL2_CB32_CLEAR;
        D3DCallbacks2.Clear    = nvClear;

        /*
         * Always export the DrawPrimitive callbacks.
         */
        D3DCallbacks2.dwFlags |= D3DHAL2_CB32_DRAWONEPRIMITIVE
                              |  D3DHAL2_CB32_DRAWONEINDEXEDPRIMITIVE
                              |  D3DHAL2_CB32_DRAWPRIMITIVES;
        D3DCallbacks2.DrawOnePrimitive        = DrawOnePrimitive32;
        D3DCallbacks2.DrawOneIndexedPrimitive = DrawOneIndexedPrimitive32;
        D3DCallbacks2.DrawPrimitives          = DrawPrimitives32;

        /*
         * Copy as much of the data as possible up to dwExpectedSize.
         */
        memcpy(lpData->lpvData, &D3DCallbacks2, dwSize);

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }
#endif  // !WINNT

    if (IsEqualIID_C((lpData->guidInfo), GUID_D3DCallbacks3))
    {
        D3DHAL_CALLBACKS3   D3DCallbacks3;

        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_D3DCallbacks3");

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0600);

        memset(&D3DCallbacks3, 0, sizeof(D3DHAL_CALLBACKS3));
        dwSize               = min(lpData->dwExpectedSize, sizeof(D3DHAL_CALLBACKS3));
        lpData->dwActualSize = sizeof(D3DHAL_CALLBACKS3);
        D3DCallbacks3.dwSize = dwSize;

        /*
         * Export DX6 DrawPrimitives2 DDI callback.
         */
        D3DCallbacks3.dwFlags |= D3DHAL3_CB32_DRAWPRIMITIVES2;
        D3DCallbacks3.DrawPrimitives2 = nvDrawPrimitives2;

        /*
         * Export DX6 Mult-Texture state validation callback.
         */
        D3DCallbacks3.dwFlags |= D3DHAL3_CB32_VALIDATETEXTURESTAGESTATE;
        D3DCallbacks3.ValidateTextureStageState = nvValidateTextureStageState;

        /*
         * Export DX6 callback for clear Render target, Z-Buffer and Stencil Buffer.
         */
        D3DCallbacks3.dwFlags |= D3DHAL3_CB32_CLEAR2;
        D3DCallbacks3.Clear2 = nvClear2;

        /*
         * Copy as much of the data as possible up to dwExpectedSize.
         */
        memcpy(lpData->lpvData, &D3DCallbacks3, dwSize);

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }

    if (IsEqualIID_C ((lpData->guidInfo), GUID_D3DExtendedCaps))
    {
        D3DHAL_D3DEXTENDEDCAPS  D3DExtendedCaps;

        D3DExtendedCaps        = getDC()->nvD3DDevCaps.d3dExtCaps;
        dwSize                 = min (lpData->dwExpectedSize, sizeof(D3DHAL_D3DEXTENDEDCAPS));
        lpData->dwActualSize   = dwSize;
        D3DExtendedCaps.dwSize = dwSize;

#ifdef  STEREO_SUPPORT
        if (STEREO_ENABLED && pStereoData->StereoSettings.dwFlags & STEREO_DISABLETnL)
        {
            D3DExtendedCaps.dwVertexProcessingCaps  = 0;
            D3DExtendedCaps.dwMaxActiveLights       = 0;
        }
#endif  //STEREO_SUPPORT

        // Copy as much of the data as possible up to dwExpectedSize.
        memcpy(lpData->lpvData, &D3DExtendedCaps, dwSize);

        // Set successful return code.
        lpData->ddRVal = DD_OK;
    }

    if (IsEqualIID_C((lpData->guidInfo), GUID_D3DParseUnknownCommandCallback))
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_D3DParseUnknownCommandCallback");

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0600);

        // Get address of callback function.
        fnD3DParseUnknownCommandCallback = (PFND3DPARSEUNKNOWNCOMMAND)lpData->lpvData;

        // Set successful return code.
        lpData->ddRVal = DD_OK;
    }

#if 0  // alpha blit sppears to have been removed from dx7 build 169   27may99   CRD
    if (IsEqualIID_C ((lpData->guidInfo), GUID_DDMoreCaps)) {

        DDMORECAPS ddMoreCaps;

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0700);

        if (lpData->dwExpectedSize != sizeof(DDMORECAPS) ) {
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return (DDHAL_DRIVER_HANDLED);
        }

        lpData->dwActualSize = sizeof(DDMORECAPS);

        ddMoreCaps.dwSize = sizeof(DDMORECAPS);

        ddMoreCaps.dwAlphaCaps = 0;
                                 DDALPHACAPS_BLTALPHAPIXELS |
                                 DDALPHACAPS_BLTSATURATE    |
                                 DDALPHACAPS_BLTPREMULT     |
                                 // DDALPHACAPS_BLTNONPREMULT |
                                 DDALPHACAPS_BLTARGBSCALE1F |
                                 DDALPHACAPS_BLTARGBSCALE4F |
                                 DDALPHACAPS_BLTALPHAFILL;

        ddMoreCaps.dwSVBAlphaCaps = 0;
                                    DDALPHACAPS_BLTALPHAPIXELS |
                                    DDALPHACAPS_BLTSATURATE |
                                    DDALPHACAPS_BLTPREMULT |
                                    // DDALPHACAPS_BLTNONPREMULT |
                                    DDALPHACAPS_BLTARGBSCALE1F |
                                    DDALPHACAPS_BLTARGBSCALE4F |
                                    DDALPHACAPS_BLTALPHAFILL;

        ddMoreCaps.dwFilterCaps = 0;
                                  DDFILTCAPS_BLTQUALITYFILTER |
                                  DDFILTCAPS_BLTCANDISABLEFILTER;

        ddMoreCaps.dwSVBFilterCaps = 0;
                                     DDFILTCAPS_BLTQUALITYFILTER |
                                     DDFILTCAPS_BLTCANDISABLEFILTER;

        memcpy(lpData->lpvData, &ddMoreCaps, sizeof(DDMORECAPS));
        lpData->ddRVal = DD_OK;
    }
#endif  // 0

    if (IsEqualIID_C ((lpData->guidInfo), GUID_DDMoreSurfaceCaps)) {

        DDMORESURFACECAPS ddMoreSurfaceCaps;
        DDSCAPSEX         ddsCapsEx, ddsCapsExAlt;

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0600);

        // fill in everything until expectedsize... (first heap's caps)
        memset(&ddMoreSurfaceCaps, 0, sizeof(ddMoreSurfaceCaps));

        // caps for heaps 2..n
        memset(&ddsCapsEx, 0, sizeof(ddsCapsEx));
        memset(&ddsCapsExAlt, 0, sizeof(ddsCapsEx));

        ddMoreSurfaceCaps.dwSize = lpData->dwExpectedSize;

        ddMoreSurfaceCaps.ddsCapsMore.dwCaps2 = DDSCAPS2_CUBEMAP;
        ddsCapsEx.dwCaps2 = DDSCAPS2_CUBEMAP;

        ddsCapsExAlt.dwCaps2 = DDSCAPS2_CUBEMAP;

        lpData->dwActualSize = lpData->dwExpectedSize;

        dwSize = min(sizeof(ddMoreSurfaceCaps),lpData->dwExpectedSize);
        memcpy(lpData->lpvData, &ddMoreSurfaceCaps, dwSize);

        // now fill in other heaps...
        while (dwSize < lpData->dwExpectedSize) {
            memcpy((PBYTE)lpData->lpvData+dwSize, &ddsCapsEx, sizeof(DDSCAPSEX));
            dwSize += sizeof(DDSCAPSEX);
            memcpy((PBYTE)lpData->lpvData+dwSize, &ddsCapsExAlt, sizeof(DDSCAPSEX));
            dwSize += sizeof(DDSCAPSEX);
        }

        lpData->ddRVal = DD_OK;
    }

#if (!defined(WINNT) || defined(NVPE))
    if (IsEqualIID_C ((lpData->guidInfo), GUID_KernelCallbacks) ) {
        DDHAL_DDKERNELCALLBACKS kCB;

        dwSize = lpData->dwExpectedSize;

        if ( sizeof(kCB) < dwSize )
            dwSize = sizeof(kCB);
        lpData->dwActualSize = sizeof(kCB);

        kCB.dwSize = sizeof(DDHAL_DDKERNELCALLBACKS);
        kCB.dwFlags = DDHAL_KERNEL_SYNCSURFACEDATA | DDHAL_KERNEL_SYNCVIDEOPORTDATA;
        kCB.SyncSurfaceData = SafeSyncSurfaceData;  // we're not doing anything
                                                    // on EITHER of these yet
        kCB.SyncVideoPortData = SafeSyncVideoPortData;

        memcpy(lpData->lpvData, &kCB, dwSize );
        //lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        lpData->ddRVal = DD_OK;
    }

    if (IsEqualIID_C ((lpData->guidInfo), GUID_KernelCaps) ) {
        DDKERNELCAPS KernelCaps;

        // these functions can be called to control the Overlay,
        // so even if VPE is not here we need to have these
        if (lpData->dwExpectedSize != sizeof(DDKERNELCAPS)) {
            dbgTracePop();
            NvReleaseSemaphore(pDriverData);
            return (DDHAL_DRIVER_HANDLED);
        }

        lpData->dwActualSize = sizeof(DDKERNELCAPS);

        KernelCaps.dwSize = sizeof(DDKERNELCAPS);
        KernelCaps.dwCaps = DDKERNELCAPS_SKIPFIELDS     |
                            DDKERNELCAPS_AUTOFLIP       |
                            DDKERNELCAPS_SETSTATE       |
                            DDKERNELCAPS_LOCK           |
                            DDKERNELCAPS_FLIPVIDEOPORT  |
                            DDKERNELCAPS_FLIPOVERLAY    |
                            DDKERNELCAPS_CAPTURE_SYSMEM |
                            // DDKERNELCAPS_CAPTURE_NONLOCALVIDMEM |
                            DDKERNELCAPS_FIELDPOLARITY  |
                            DDKERNELCAPS_CAPTURE_INVERTED;

        KernelCaps.dwIRQCaps = DDIRQ_VPORT0_VSYNC |
                               //  DDIRQ_BUSMASTER;
                               //  #define DDIRQ_BUSMASTER         0x00000002l
                               0x00000002;

        // DDIRQ_DISPLAY_VSYNC      |
        // DDIRQ_RESERVED1          |
        // DDIRQ_VPORT0_VSYNC       // this is the only IRQ we'll be supporting
        // DDIRQ_VPORT0_LINE        |
        // DDIRQ_VPORT1_VSYNC       |
        // DDIRQ_VPORT1_LINE        |


        memcpy(lpData->lpvData, &KernelCaps, sizeof(DDKERNELCAPS) );
        //lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        lpData->ddRVal = DD_OK;
    }
#endif // !WINNT || NVPE

    if (IsEqualIID_C ((lpData->guidInfo), GUID_MiscellaneousCallbacks) ) {
        DDHAL_DDMISCELLANEOUSCALLBACKS miscCB;

        dwSize = lpData->dwExpectedSize;

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0500);

        if ( sizeof(miscCB) < dwSize )
            dwSize = sizeof(miscCB);
        lpData->dwActualSize = sizeof(miscCB);
        memset(&miscCB, 0, dwSize);
        miscCB.dwSize = dwSize;

#ifdef WINNT
        miscCB.dwFlags = DDHAL_MISCCB32_GETAVAILDRIVERMEMORY;
#else // !WINNT
        miscCB.dwFlags = DDHAL_MISCCB32_GETHEAPALIGNMENT |
                         DDHAL_MISCCB32_UPDATENONLOCALHEAP |
                         DDHAL_MISCCB32_GETAVAILDRIVERMEMORY;
        miscCB.GetHeapAlignment = GetHeapAlignment32;
        miscCB.UpdateNonLocalHeap = UpdateNonLocalHeap32;
#endif // !WINNT

        // only GetAvailDriverMemory32 is valid for WINNT
        miscCB.GetAvailDriverMemory = GetAvailDriverMemory32;

        memcpy(lpData->lpvData, &miscCB, dwSize );
        lpData->ddRVal = DD_OK;
    }

    if (IsEqualIID_C ((lpData->guidInfo), GUID_Miscellaneous2Callbacks) ) {
        DDHAL_DDMISCELLANEOUS2CALLBACKS misc2CB;

        dwSize = lpData->dwExpectedSize;

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0700);

        if ( sizeof(misc2CB) < dwSize )
            dwSize = sizeof(misc2CB);
        lpData->dwActualSize = sizeof(misc2CB);
        memset(&misc2CB, 0, dwSize);
        misc2CB.dwSize = dwSize;
        misc2CB.dwFlags = 0;

#if 0 // alpha blit sppears to have been removed from dx7 build 169   27may99   CRD
        // alphablt
        misc2CB.dwFlags |= DDHAL_MISC2CB32_ALPHABLT;
        misc2CB.AlphaBlt = Blit32;  // Use common callback with Blit32 calls
#endif // 0

        // createsurfaceex
        misc2CB.dwFlags |= DDHAL_MISC2CB32_CREATESURFACEEX;
        misc2CB.CreateSurfaceEx = nvCreateSurfaceEx;

        // getdriverstate
        misc2CB.dwFlags |= DDHAL_MISC2CB32_GETDRIVERSTATE;
        misc2CB.GetDriverState = nvGetDriverState;

        // destroyddlocal
        misc2CB.dwFlags |= DDHAL_MISC2CB32_DESTROYDDLOCAL;
        misc2CB.DestroyDDLocal = nvDestroyDDLocal;

        memcpy(lpData->lpvData, &misc2CB, dwSize );
        lpData->ddRVal = DD_OK;
    }

#if (NVARCH >= 0x10)
    // Check for Motion Comp GUID
    if (IsEqualIID_C ((lpData->guidInfo), GUID_MotionCompCallbacks)) {
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
            nvGetMoCompDriverInfo(lpData);
    }
#endif // NVARCH >= 0x10

    if (IsEqualGUID_C((lpData->guidInfo), GUID_NonLocalVidMemCaps ) ) {
        LPDDNONLOCALVIDMEMCAPS lpCaps;
        int i;

        /* Size validation code omitted for clarity */
        dwSize = min(lpData->dwExpectedSize, sizeof(DDNONLOCALVIDMEMCAPS));
        lpCaps = (LPDDNONLOCALVIDMEMCAPS)(lpData->lpvData);
        lpCaps->dwSize = dwSize;

        lpCaps->dwNLVBCaps     = pHalInfo->ddCaps.dwCaps;
        lpCaps->dwNLVBCaps2    = pHalInfo->ddCaps.dwCaps2;
        lpCaps->dwNLVBCKeyCaps = pHalInfo->ddCaps.dwCKeyCaps;
        lpCaps->dwNLVBFXCaps   = pHalInfo->ddCaps.dwFXCaps;

        for ( i=0; i < DD_ROP_SPACE; i++ )
            lpCaps->dwNLVBRops[i] = ropList[i];

        lpData->dwActualSize = dwSize;
        lpData->ddRVal = DD_OK;
    }

#if (!defined(WINNT) || defined(NVPE))      // BUGBUG - we will add Win2K videoport support

    if (IsEqualIID_C ((lpData->guidInfo), GUID_VideoPortCallbacks) ) {

        DDHAL_DDVIDEOPORTCALLBACKS vpCB;

        dwSize = lpData->dwExpectedSize;

        // this test prevent reporting of VPE functions so that if necessary we can turn off VPE via registry.
        if (pDriverData->bEnableVPE == 1 ) {

            if ( sizeof(vpCB) < dwSize )
                dwSize = sizeof(vpCB);
            lpData->dwActualSize = sizeof(vpCB);
            memset(&vpCB, 0, dwSize);
            vpCB.dwSize = dwSize;

            vpCB.dwFlags = DDHAL_VPORT32_CANCREATEVIDEOPORT |
                           DDHAL_VPORT32_CREATEVIDEOPORT    |
                           DDHAL_VPORT32_DESTROY            |
                           DDHAL_VPORT32_FLIP               |
                           DDHAL_VPORT32_GETBANDWIDTH       |
                           DDHAL_VPORT32_GETINPUTFORMATS    |
                           DDHAL_VPORT32_GETOUTPUTFORMATS   |
                           DDHAL_VPORT32_GETFIELD           |
                           DDHAL_VPORT32_GETCONNECT         |
                           DDHAL_VPORT32_GETFLIPSTATUS      |
                           DDHAL_VPORT32_UPDATE             |
                           DDHAL_VPORT32_WAITFORSYNC        |
                           DDHAL_VPORT32_GETSIGNALSTATUS;


            vpCB.CanCreateVideoPort         = CanCreateVideoPort32;
            vpCB.CreateVideoPort            = CreateVideoPort32;
            vpCB.DestroyVideoPort           = DestroyVideoPort32;
            vpCB.FlipVideoPort              = FlipVideoPort32;

            vpCB.GetVideoPortBandwidth      = GetVideoPortBandwidth32;
            vpCB.GetVideoPortInputFormats   = GetVideoPortInputFormat32;
            vpCB.GetVideoPortOutputFormats  = GetVideoPortOutputFormat32;
            vpCB.GetVideoPortField          = GetVideoPortField32;
            vpCB.GetVideoPortConnectInfo    = GetVideoPortConnectInfo;

            vpCB.GetVideoPortFlipStatus     = GetVideoPortFlipStatus32;
            vpCB.UpdateVideoPort            = UpdateVideoPort32;
            vpCB.WaitForVideoPortSync       = WaitForVideoPortSync32;
            vpCB.GetVideoSignalStatus       = GetVideoSignalStatus32;

            memcpy(lpData->lpvData, &vpCB, dwSize );
            lpData->ddRVal = DD_OK;
        }

    }

    if (IsEqualIID_C ((lpData->guidInfo), GUID_VideoPortCaps) ) {
        DDVIDEOPORTCAPS VideoPortCaps;
        DDVIDEOPORTCAPS *pVideoPortCapsDest = (DDVIDEOPORTCAPS*)lpData->lpvData;
        DWORD dwIndex;

        // this test prevent reporting of VPE functions so that
        // if necessary we can turn off VPE via registry.
        if (pDriverData->bEnableVPE == 1 ) {
            // expecting query for 1 video port
            if (lpData->dwExpectedSize != (sizeof(VideoPortCaps))) {
                NvReleaseSemaphore(pDriverData);
                dbgTracePop();
                return (DDHAL_DRIVER_HANDLED);
            }

            lpData->dwActualSize = (sizeof(VideoPortCaps) );

            // index changed to allow only 1 report resolution, for time being only NTSC
            // until class structure is changed to test PAL etc
            for (dwIndex = 0; dwIndex < 1; dwIndex++) {
                if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

                  memset(&VideoPortCaps, 0, sizeof(DDVIDEOPORTCAPS));

                  VideoPortCaps.dwSize = sizeof(DDVIDEOPORTCAPS);

                  VideoPortCaps.dwFlags = DDVPD_WIDTH             |
                                          DDVPD_HEIGHT            |
                                          DDVPD_ID                |
                                          DDVPD_CAPS              |
                                          DDVPD_FX                |
                                          DDVPD_AUTOFLIP          |
                                          DDVPD_ALIGN             |
                                          DDVPD_PREFERREDAUTOFLIP |
                                          DDVPD_FILTERQUALITY;

                  VideoPortCaps.dwVideoPortID = dwIndex;
                  // for more supported video modes, place the resolutions here
                  //        VideoPortCaps.dwMaxWidth      = ST_VPE_MAX_INPUT_X;
                  //       VideoPortCaps.dwMaxVBIWidth = ST_VPE_MAX_INPUT_X;
                  //          VideoPortCaps.dwMaxHeight   = ST_VPE_MAX_INPUT_Y;
                  VideoPortCaps.dwMaxWidth    = 4096;   // must talk to joe and get
                                                        // the details on this..
                  VideoPortCaps.dwMaxVBIWidth = 4096;
                  VideoPortCaps.dwMaxHeight   = 4096;    //this value is calculated from
                                                         // 2^24 (bytes)  / 2  / 2048 pixels / line

                  // could have share even/odd, but for now keep things simple

                  VideoPortCaps.dwCaps =
                  DDVPCAPS_AUTOFLIP       |
                  DDVPCAPS_INTERLACED     |
                  DDVPCAPS_NONINTERLACED  |
                  // DDVPCAPS_SHAREABLE     |
                  DDVPCAPS_SKIPEVENFIELDS |
                  DDVPCAPS_SKIPODDFIELDS  |
                  DDVPCAPS_VBISURFACE     |
                  DDVPCAPS_OVERSAMPLEDVBI;

                  VideoPortCaps.dwFX =
                  DDVPFX_CROPTOPDATA      |
                  DDVPFX_INTERLEAVE       |
                  DDVPFX_PRESHRINKXB      |
                  DDVPFX_PRESHRINKY       | // y can accept arbitary (except 0)
                  DDVPFX_VBICONVERT       |
                  DDVPFX_VBINOSCALE       |
                  DDVPFX_VBINOINTERLEAVE  |
                  DDVPFX_IGNOREVBIXCROP;


                  VideoPortCaps.dwNumAutoFlipSurfaces = 5;


                  VideoPortCaps.dwAlignVideoPortBoundary      = pDriverData->vpp.dwOverlayByteAlignmentPad + 1;
                  VideoPortCaps.dwAlignVideoPortPrescaleWidth = 1;

                  // can't crop to an arbitary rectangle!!
                  VideoPortCaps.dwAlignVideoPortCropBoundary  = 1;
                  VideoPortCaps.dwAlignVideoPortCropWidth     = 1;

                  // not valid for PRESHRINKX/YB flag as set above
                  VideoPortCaps.dwPreshrinkXStep = 24;
                  VideoPortCaps.dwPreshrinkYStep = 0x400;


                  VideoPortCaps.dwNumVBIAutoFlipSurfaces = 5;

                  VideoPortCaps.dwNumPreferredAutoflip =3;
                  VideoPortCaps.wNumFilterTapsX = 2;
                  VideoPortCaps.wNumFilterTapsY = 1;
                  memcpy(&pVideoPortCapsDest[dwIndex], &VideoPortCaps, sizeof(VideoPortCaps) );

               } else {


                  memset(&VideoPortCaps, 0, sizeof(DDVIDEOPORTCAPS));

                  VideoPortCaps.dwSize = sizeof(DDVIDEOPORTCAPS);

                  VideoPortCaps.dwFlags = DDVPD_WIDTH             |
                                          DDVPD_HEIGHT            |
                                          DDVPD_ID                |
                                          DDVPD_CAPS              |
                                          DDVPD_FX                |
                                          DDVPD_AUTOFLIP          |
                                          DDVPD_ALIGN             |
                                          DDVPD_PREFERREDAUTOFLIP |
                                          DDVPD_FILTERQUALITY;

                  VideoPortCaps.dwVideoPortID = dwIndex;
                  // for more supported video modes, place the resolutions here
                  //        VideoPortCaps.dwMaxWidth      = ST_VPE_MAX_INPUT_X;
                  //       VideoPortCaps.dwMaxVBIWidth = ST_VPE_MAX_INPUT_X;
                  //          VideoPortCaps.dwMaxHeight   = ST_VPE_MAX_INPUT_Y;
                  VideoPortCaps.dwMaxWidth    = 4096;   // must talk to joe and get
                                                        // the details on this..
                  VideoPortCaps.dwMaxVBIWidth = 4096;
                  VideoPortCaps.dwMaxHeight   = 640;    // just arbitarily larger than
                                                        // PAL field *2 for progressive

                  // could have share even/odd, but for now keep things simple

                  VideoPortCaps.dwCaps =
                  DDVPCAPS_AUTOFLIP       |
                  DDVPCAPS_INTERLACED     |
                  DDVPCAPS_NONINTERLACED  |
                  // DDVPCAPS_SHAREABLE     |
                  DDVPCAPS_SKIPEVENFIELDS |
                  DDVPCAPS_SKIPODDFIELDS  |
                  DDVPCAPS_VBISURFACE     |
                  DDVPCAPS_OVERSAMPLEDVBI;

                  VideoPortCaps.dwFX =
                  DDVPFX_CROPTOPDATA      |
                  DDVPFX_INTERLEAVE       |
                  DDVPFX_PRESHRINKXB      |
                  DDVPFX_PRESHRINKY       | // y can accept arbitary (except 0)
                  DDVPFX_VBICONVERT       |
                  DDVPFX_VBINOSCALE       |
                  DDVPFX_VBINOINTERLEAVE  |
                  DDVPFX_IGNOREVBIXCROP;


                  VideoPortCaps.dwNumAutoFlipSurfaces = 5;


                  VideoPortCaps.dwAlignVideoPortBoundary      = pDriverData->vpp.dwOverlayByteAlignmentPad + 1;
                  VideoPortCaps.dwAlignVideoPortPrescaleWidth = 1;

                  // can't crop to an arbitary rectangle!!
                  VideoPortCaps.dwAlignVideoPortCropBoundary  = 1;
                  VideoPortCaps.dwAlignVideoPortCropWidth     = 1;

                  // not valid for PRESHRINKX/YB flag as set above
                  VideoPortCaps.dwPreshrinkXStep = 24;
                  VideoPortCaps.dwPreshrinkYStep = 0x400;


                  VideoPortCaps.dwNumVBIAutoFlipSurfaces = 5;

                  VideoPortCaps.dwNumPreferredAutoflip =3;
                  VideoPortCaps.wNumFilterTapsX = 5;
                  VideoPortCaps.wNumFilterTapsY = 1;

                  memcpy(&pVideoPortCapsDest[dwIndex], &VideoPortCaps, sizeof(VideoPortCaps) );
               }
            }
            lpData->ddRVal = DD_OK;
        }
    }
#endif // !WINNT || NVPE      // BUGBUG - we will add Win2K videoport support

    if (IsEqualIID_C((lpData->guidInfo), GUID_ZPixelFormats))
    {
        DPF_LEVEL(NVDBG_LEVEL_INFO, "D3DGetDriverInfo - Process GUID_ZPixelFormats");

        global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0600);

        /*
         * Return a pointer to the z-buffer format data.
         */
        dwSize = min(lpData->dwExpectedSize, sizeof(NvZPixelFormats));
        memcpy(lpData->lpvData, &NvZPixelFormats, dwSize);
        lpData->dwActualSize = dwSize;

        /*
         * Don't export 24-bit z-buffer formats if the registry has disabled them.
         */
        if (!(getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_Z24ENABLE_ENABLE)) {
            ((ZPIXELFORMATS *)lpData->lpvData)->dwNumZFormats = NV_NUM_16BIT_ZBUFFER_FORMATS;
        }

        /*
         * Set successful return code.
         */
        lpData->ddRVal = DD_OK;
    }

#ifdef WINNT
    // Under Win2K/DX7 there is a GUID_ for UpdateNonLocalHeap where under Win9x
    // this function has been a callback.

    if (IsEqualIID_C((lpData->guidInfo), GUID_UpdateNonLocalHeap) && ppdev->AgpHeap)
    {
#ifdef NV_AGP
        nvAssert(0); // we should never get GUID_UpdateNonLocalHeap with NV_AGP enabled
#else // !NV_AGP
        DD_UPDATENONLOCALHEAPDATA *pHeapData;
        ULONG pAgpBase;

        pHeapData = (DD_UPDATENONLOCALHEAPDATA *) lpData->lpvData;
        pHeapData->dwHeap = AGP_HEAP;
        pDriverData->GARTPhysicalBase = pHeapData->fpGARTDev;
        pDriverData->GARTLinearHeapOffset = pHeapData->fpGARTLin;

        // get the aperture size as set in the BIOS
        NvRmConfigGet(pDriverData->dwRootHandle,
                      NV_WIN_DEVICE,
                      NV_CFG_AGP_PHYS_BASE,
                      &pAgpBase);
        NvWin2KGetAgpLimit(pDriverData, pDriverData->dwRootHandle, NV_WIN_DEVICE);

        if (ppdev->AgpPushBuffer && (ppdev->DmaPushBufTotalSize <= pDriverData->regMaxAGPLimit))
            pDriverData->regMaxAGPLimit -= ppdev->DmaPushBufTotalSize;

        NvWin2KMapAgpHeap(pDriverData);
#endif // !NV_AGP
    }

    if (IsEqualIID_C((lpData->guidInfo), GUID_GetHeapAlignment))
    {
        DD_GETHEAPALIGNMENTDATA *lpGhaData;
        lpGhaData = (DD_GETHEAPALIGNMENTDATA *) lpData->lpvData;

        if (lpGhaData->dwHeap == 0) {
            lpGhaData->Alignment.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN |
                                                  DDSCAPS_EXECUTEBUFFER  |
                                                  DDSCAPS_OVERLAY        |
                                                  DDSCAPS_TEXTURE        |
                                                  DDSCAPS_ZBUFFER        |
                                                  DDSCAPS_ALPHA          |
                                                  DDSCAPS_FLIP;

            /* Alignments in bytes */

            lpGhaData->Alignment.ExecuteBuffer.Linear.dwStartAlignment = 0;
            lpGhaData->Alignment.ExecuteBuffer.Linear.dwPitchAlignment = 0;

            lpGhaData->Alignment.Overlay.Linear.dwStartAlignment = NV_OVERLAY_BYTE_ALIGNMENT;
            lpGhaData->Alignment.Overlay.Linear.dwPitchAlignment = NV_OVERLAY_BYTE_ALIGNMENT;

            lpGhaData->Alignment.Texture.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign; /* Texture surfaces, not our optimized texture alignment */
            lpGhaData->Alignment.Texture.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

            lpGhaData->Alignment.ZBuffer.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign;
            lpGhaData->Alignment.ZBuffer.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

            lpGhaData->Alignment.AlphaBuffer.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign;
            lpGhaData->Alignment.AlphaBuffer.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

            lpGhaData->Alignment.Offscreen.Linear.dwStartAlignment = pDriverData->dwSurfaceAlign;
            lpGhaData->Alignment.Offscreen.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

            lpGhaData->Alignment.FlipTarget.Linear.dwStartAlignment = pDriverData->dwSurfaceBitAlign;
            lpGhaData->Alignment.FlipTarget.Linear.dwPitchAlignment = pDriverData->dwSurfaceAlign;

        }
    }

    if (IsEqualIID_C((lpData->guidInfo), GUID_NTPrivateDriverCaps))
    {
#define DDHAL_PRIVATECAP_RESERVED1 0x00000004l        // WinXP - no 7 second stall on mode switch
        DD_NTPRIVATEDRIVERCAPS *pNTPrivateDriverCaps = (DD_NTPRIVATEDRIVERCAPS*)lpData->lpvData;
        pNTPrivateDriverCaps->dwPrivateCaps = DDHAL_PRIVATECAP_ATOMICSURFACECREATION
                                            | DDHAL_PRIVATECAP_NOTIFYPRIMARYCREATION
                                            | DDHAL_PRIVATECAP_RESERVED1;
        lpData->ddRVal = DD_OK;
    }
#endif // WINNT

    //watch out for stereo call GUID_DDStereoMode
    //only allow DX8 driver calls to be parsed by Celsius and Kelvin driver
    if (IsEqualIID_C((lpData->guidInfo), GUID_GetDriverInfo2) &&
        ((DD_STEREOMODE*)lpData->lpvData)->dwHeight == D3DGDI2_MAGIC &&
        (
#if (NVARCH >= 0x020)
        (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ||
#endif // (NVARCH >= 0x020)
        (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)
         ))
    {

        DD_GETDRIVERINFO2DATA* pGDI2 = (DD_GETDRIVERINFO2DATA*)lpData->lpvData;

        switch (pGDI2->dwType)
        {
            case D3DGDI2_TYPE_GETD3DCAPS8:
            {
                DWORD copySize = min(sizeof(getDC()->nvD3DDevCaps.dwD3DCap8), pGDI2->dwExpectedSize);

                global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, 0x0800);

                memcpy(lpData->lpvData, &(getDC()->nvD3DDevCaps.dwD3DCap8), copySize);
                lpData->dwActualSize = copySize;
                lpData->ddRVal       = DD_OK;
            }
            break;

            case D3DGDI2_TYPE_GETFORMATCOUNT:
            {
                DD_GETFORMATCOUNTDATA * pGFCD = (DD_GETFORMATCOUNTDATA *)lpData->lpvData;

#ifdef TEXFORMAT_CRD // --------------------------------

                pGFCD->dwFormatCount = getDC()->dwPixelFormatCount;

#else // !TEXFORMAT_CRD  --------------------------------

#if (NVARCH >= 0x020)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
                {
#ifdef DXT_SUPPORT
                    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTURECOMPRESSIONENABLE_MASK) == D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE)
                    {
                        pGFCD->dwFormatCount = sizeof(kelvinPixelFormatsDX8) / sizeof(DDPIXELFORMAT);
                    }
                    else
#endif
                    {
                        pGFCD->dwFormatCount = sizeof(kelvinPixelFormatsDX8NoDXT) / sizeof(DDPIXELFORMAT);
                    }
                }
                else
#endif
                {
#ifdef DXT_SUPPORT
                    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTURECOMPRESSIONENABLE_MASK) == D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE)
                    {
                        pGFCD->dwFormatCount =  sizeof(celsiusPixelFormatsDX8) / sizeof(DDPIXELFORMAT);
                    }
                    else
#endif
                    {
                        pGFCD->dwFormatCount = sizeof(celsiusPixelFormatsDX8NoDXT) / sizeof(DDPIXELFORMAT);
                    }
                }

                if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) {
                    if ((global.dwDXRuntimeVersion < 0x0800) ||
                        ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_PALETTEENABLE_MASK) == D3D_REG_PALETTEENABLE_DISABLE))
                    {
                        pGFCD->dwFormatCount -= NV_NUM_8BIT_TEXTURE_FORMATS;
                    }
                }

#endif // !TEXFORMAT_CRD  --------------------------------

                lpData->dwActualSize = sizeof(DD_GETFORMATCOUNTDATA);
                lpData->ddRVal       = DD_OK;
            }
            break;

            case D3DGDI2_TYPE_GETFORMAT:
            {
                DD_GETFORMATDATA* pGFD = (DD_GETFORMATDATA *)lpData->lpvData;

#ifdef TEXFORMAT_CRD // --------------------------------

                pGFD->format = (getDC()->pPixelFormats)[pGFD->dwFormatIndex];

#else // !TEXFORMAT_CRD  --------------------------------

#if (NVARCH >= 0x020)
                if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN)
                {
#ifdef DXT_SUPPORT
                    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTURECOMPRESSIONENABLE_MASK) == D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE)
                    {
                        pGFD->format = kelvinPixelFormatsDX8[pGFD->dwFormatIndex];
                    }
                    else
#endif // DXT_SUPPORT
                    {
                        pGFD->format = kelvinPixelFormatsDX8NoDXT[pGFD->dwFormatIndex];
                    }
                }
                else
#endif // (NVARCH >= 0x020)
                {
#ifdef DXT_SUPPORT
                    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTURECOMPRESSIONENABLE_MASK) == D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE)
                    {
                        pGFD->format = celsiusPixelFormatsDX8[pGFD->dwFormatIndex];
                    }
                    else
#endif // DXT_SUPPORT
                    {
                        pGFD->format = celsiusPixelFormatsDX8NoDXT[pGFD->dwFormatIndex];
                    }
                }
#endif // !TEXFORMAT_CRD  --------------------------------

                lpData->dwActualSize = sizeof(DD_GETFORMATDATA);
                lpData->ddRVal       = DD_OK;
            }
            break;

            case D3DGDI2_TYPE_DXVERSION:
            {
                DD_DXVERSION* pGFD = (DD_DXVERSION *)lpData->lpvData;
                global.dwDXRuntimeVersion = max (global.dwDXRuntimeVersion, pGFD->dwDXVersion);
                lpData->dwActualSize = sizeof(DD_DXVERSION);
                lpData->ddRVal       = DD_OK;
            }
            break;
        }
    }

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DDHAL_DRIVER_HANDLED);

} // GetDriverInfo32

//---------------------------------------------------------------------------
//
// SetExclusiveMode32 DDHAL callback
//
#ifndef WINNT
DWORD __stdcall SetExclusiveMode32 (LPDDHAL_SETEXCLUSIVEMODEDATA lpSetExclusiveMode)
{
HANDLE eventExclusive;

    lpSetExclusiveMode->ddRVal = DD_OK;

    // We need to signal quicktweak that we have entered fullscreen exclusive mode
    // for mobile hotkey disabling. This scenario is guaranteed to be in a single
    // physical adapter configuration.
    eventExclusive = OpenEvent(EVENT_MODIFY_STATE, FALSE, EVENTNAME_DXEXCLUSIVEMODE);
    if (eventExclusive) {
        if (lpSetExclusiveMode->dwEnterExcl) {
            SetEvent(eventExclusive);
        } else {
            ResetEvent(eventExclusive);
        }

        CloseHandle(eventExclusive);
    }

    return DDHAL_DRIVER_HANDLED;
}
#endif  // !WINNT

//---------------------------------------------------------------------------
//
// FlipToGDISurface32 DDHAL callback
//
// - enabling this callback causes flashing problems with MS Combat Flight Simulator 2
//
DWORD __stdcall FlipToGDISurface32 (LPDDHAL_FLIPTOGDISURFACEDATA lpFlipToGDISurface)
{
#ifndef WINNT
    lpFlipToGDISurface->ddRVal = DD_OK;
    nvSetDriverDataPtrFromDDGbl (lpFlipToGDISurface->lpDD);

    // In extended mode, the DX runtime could call us here after the push buffer has
    // been destroyed by the first call to DestroyDriver32.  Test for validity of the push buffer.
    if ((pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0097_KELVIN) ||
        (!getDC()->nvPusher.isValid()))
    {
        return DDHAL_DRIVER_NOTHANDLED;
    }
    else if ((pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_1196_CELSIUS | NVCLASS_0096_CELSIUS))
          && (lpFlipToGDISurface->dwToGDI))
    {
        // flush out all but one v-blank
        WaitForIdle(TRUE, FALSE);

        // get the current v-blank count
        NV_CFGEX_VBLANK_COUNTER_MULTI_PARAMS nvVBCounter;
        nvVBCounter.Head = GET_CURRENT_HEAD();
        NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_VBLANK_COUNTER_MULTI,
            &nvVBCounter, sizeof(nvVBCounter));

        // wait for one more v-blank (only really required in triple buffered case)
        DWORD dwFirstCount = nvVBCounter.VBlankCounter;
        do {
            NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE, NV_CFGEX_VBLANK_COUNTER_MULTI,
                &nvVBCounter, sizeof(nvVBCounter));
        } while (nvVBCounter.VBlankCounter == dwFirstCount);

        // tell the HW we want to start writing to the next buffer, this must stay in-sync
        nvglSetNv15CelsiusSyncIncWrite (NV_DD_CELSIUS);

        BOOL  bCloneMode = pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE;
        DWORD dwDAC      = NV_DD_VIDEO_LUT_CURSOR_DAC + (bCloneMode ? 0 : pDXShare->dwHeadNumber);

        DWORD dwNumHeads = bCloneMode ? pDriverData->dwHeads : 1;
        DWORD dwFormat   = (NV07C_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31) |
                           (NV07C_SET_IMAGE_FORMAT_FLAGS_COMPLETE_ON_OBJECT_CLEANUP << 25) |
                           (((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deDeltaScan);
        dwFormat |= (NV07C_SET_IMAGE_FORMAT_WHEN_IMMEDIATELY << 20);
        dwFormat |= bCloneMode ? (NV07C_SET_IMAGE_FORMAT_MULTIHEAD_SYNC_ENABLED << 24) :
                                 (NV07C_SET_IMAGE_FORMAT_MULTIHEAD_SYNC_DISABLED << 24);

        PRIMARY_BUFFER_INDEX ^= 0x1;

        for (DWORD dwHead = 0; dwHead < dwNumHeads; dwHead++) {

            // calculate offset, the RM will add the pan & scan adjustment from the display driver if needed
            DWORD dwOffset = GET_PRIMARY_ADDR() - pDriverData->BaseAddress;

            // program the flip
            nvglSetObject (NV_DD_SURFACES, dwDAC + dwHead); // load the DAC object
            nvglSetNv15VideoLUTCursorDACImageData (NV_DD_SURFACES, PRIMARY_BUFFER_INDEX, dwOffset, dwFormat);
        }

        // put back the NV_DD_SURFACES_2D object
        nvglSetObject (NV_DD_SURFACES, NV_DD_SURFACES_2D);

        nvPusherStart(TRUE);

        // update frame counter
        CURRENT_FLIP ++;

        return (DDHAL_DRIVER_HANDLED);
    }

#endif // !WINNT
    return DDHAL_DRIVER_NOTHANDLED;
}

//---------------------------------------------------------------------------

/*
 * buildDDHALInfo32
 *
 * build DDHALInfo structure
 */
BOOL __stdcall buildDDHALInfo32(GLOBALDATA *pDriverData)
{
    int i;
#ifndef NVPE
    char regStr[256] = {0,0,0,0,0,0};
    HKEY hKey;
#endif // NVPE

    dbgTracePush ("buildDDHALInfo32");

    /*
     * fill out the main driver callbacks
     */
    memset(&pDriverData->DDCallbacks, 0, sizeof(DDHAL_DDCALLBACKS));
    pDriverData->DDCallbacks.dwSize                = sizeof(DDHAL_DDCALLBACKS);
//    pDriverData->DDCallbacks.SetMode               = SetMode32;   Never called by DDRAW
    pDriverData->DDCallbacks.WaitForVerticalBlank  = WaitForVerticalBlank32;
    pDriverData->DDCallbacks.GetScanLine           = GetScanLine32;
    pDriverData->DDCallbacks.CreateSurface         = CreateSurface32;
    pDriverData->DDCallbacks.CanCreateSurface      = CanCreateSurface32;
#ifndef WINNT
    pDriverData->DDCallbacks.SetExclusiveMode      = SetExclusiveMode32;
#endif

    pDriverData->DDCallbacks.dwFlags               = DDHAL_CB32_WAITFORVERTICALBLANK
#ifndef WINNT
                                                   | DDHAL_CB32_SETEXCLUSIVEMODE
#endif
//                                                 | DDHAL_CB32_SETMODE
                                                   | DDHAL_CB32_GETSCANLINE
                                                   | DDHAL_CB32_CANCREATESURFACE
                                                   | DDHAL_CB32_CREATESURFACE;

#ifndef WINNT // not needed under WINNT
    pDriverData->DDCallbacks.DestroyDriver         = (LPDDHAL_DESTROYDRIVER)DestroyDriver32;

    pDriverData->DDCallbacks.dwFlags               |= DDHAL_CB32_DESTROYDRIVER;
#endif  // !WINNT


    // fill out the palette callbacks
    memset(&pDriverData->DDPaletteCallbacks, 0, sizeof(DDHAL_DDPALETTECALLBACKS));
    pDriverData->DDPaletteCallbacks.dwSize  = sizeof(DDHAL_DDPALETTECALLBACKS);
    pDriverData->DDPaletteCallbacks.dwFlags = 0;

    // set up pointer to DDHALINFO
    LPDDHALINFO pHalInfo = GET_HALINFO();

    // fill out the HALINFO
#ifndef WINNT   // These don't exist under NT, HAL callbacks returned via DrvGetDirectDrawInfo
    memset(pHalInfo, 0, sizeof(DDHALINFO));
#endif

    pHalInfo->dwSize = sizeof(DDHALINFO);
    pHalInfo->ddCaps.dwSize = sizeof(DDCORECAPS);

#ifndef WINNT
    // callback functions
    pHalInfo->lpDDCallbacks        = &pDriverData->DDCallbacks;
    pHalInfo->lpDDSurfaceCallbacks = &pDriverData->DDSurfaceCallbacks;
    pHalInfo->lpDDPaletteCallbacks = &pDriverData->DDPaletteCallbacks;
    pHalInfo->lpDDExeBufCallbacks  = &pDriverData->DDExecuteBufferCallbacks;

    // although MS calls this an instance handle, they actually just want a unique per head/card identifier
    pHalInfo->hInstance = (DWORD)pDXShare;

    // set the physical device ptr
    DIBENGINEHDR1 *pDibeng = (DIBENGINEHDR1 *)pDXShare->pFlatDibeng;
    pHalInfo->lpPDevice = (void*)pDibeng->delpPDevice;
#endif  // !WINNT

    // ROPS supported
    for ( i=0; i < DD_ROP_SPACE; i++ )
        pHalInfo->ddCaps.dwRops[i] = ropList[i];

    // fill out the surface callbacks
    memset(&pDriverData->DDSurfaceCallbacks, 0, sizeof(DDHAL_DDSURFACECALLBACKS));
    pDriverData->DDSurfaceCallbacks.dwSize             = sizeof(DDHAL_DDSURFACECALLBACKS);
    pDriverData->DDSurfaceCallbacks.DestroySurface     = DestroySurface32;
    pDriverData->DDSurfaceCallbacks.Blt                = Blit32;
    pDriverData->DDSurfaceCallbacks.Flip               = Flip32;
    pDriverData->DDSurfaceCallbacks.Lock               = Lock32;
    pDriverData->DDSurfaceCallbacks.Unlock             = Unlock32;
    pDriverData->DDSurfaceCallbacks.GetBltStatus       = GetBltStatus32;
    pDriverData->DDSurfaceCallbacks.GetFlipStatus      = GetFlipStatus32;
    pDriverData->DDSurfaceCallbacks.SetColorKey        = SetSurfaceColorKey32;
    if (NO_OVERLAY) {
        pDriverData->DDSurfaceCallbacks.UpdateOverlay      = NULL;
        pDriverData->DDSurfaceCallbacks.SetOverlayPosition = NULL;
        pDriverData->DDSurfaceCallbacks.dwFlags            = DDHAL_SURFCB32_BLT |
                                                             DDHAL_SURFCB32_FLIP |
                                                             DDHAL_SURFCB32_LOCK |
                                                             DDHAL_SURFCB32_UNLOCK |
                                                             DDHAL_SURFCB32_SETCOLORKEY |
                                                             DDHAL_SURFCB32_GETBLTSTATUS |
                                                             DDHAL_SURFCB32_GETFLIPSTATUS |
                                                             DDHAL_SURFCB32_DESTROYSURFACE;
    } else {
        pDriverData->DDSurfaceCallbacks.UpdateOverlay      = UpdateOverlay32;
        pDriverData->DDSurfaceCallbacks.SetOverlayPosition = SetOverlayPosition32;
        pDriverData->DDSurfaceCallbacks.dwFlags            = DDHAL_SURFCB32_BLT |
                                                             DDHAL_SURFCB32_FLIP |
                                                             DDHAL_SURFCB32_LOCK |
                                                             DDHAL_SURFCB32_UNLOCK |
                                                             DDHAL_SURFCB32_SETCOLORKEY |
                                                             DDHAL_SURFCB32_GETBLTSTATUS |
                                                             DDHAL_SURFCB32_GETFLIPSTATUS |
                                                             DDHAL_SURFCB32_UPDATEOVERLAY |
                                                             DDHAL_SURFCB32_SETOVERLAYPOSITION |
                                                             DDHAL_SURFCB32_DESTROYSURFACE;
    }

#ifndef DO_NOT_SUPPORT_VERTEX_BUFFERS // see nvprecomp.h
    if (
#if (NVARCH >= 0x020)
        (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ||
#endif // (NVARCH >= 0x020)
        (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)
       )
    {
        /*
         * populate execute buffer callbacks, so that we can allocate vertex buffers
         */
        pDriverData->DDExecuteBufferCallbacks.dwSize                 = sizeof(pDriverData->DDExecuteBufferCallbacks);
#ifdef WINNT
        pDriverData->DDExecuteBufferCallbacks.CanCreateD3DBuffer     = D3DCanCreateExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.CreateD3DBuffer        = D3DCreateExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.DestroyD3DBuffer       = D3DDestroyExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.LockD3DBuffer          = D3DLockExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.UnlockD3DBuffer        = D3DUnlockExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.dwFlags                = 0x1F; //hardcoded flags aren't defined
#else
        pDriverData->DDExecuteBufferCallbacks.CanCreateExecuteBuffer = D3DCanCreateExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.CreateExecuteBuffer    = D3DCreateExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.DestroyExecuteBuffer   = D3DDestroyExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.LockExecuteBuffer      = D3DLockExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.UnlockExecuteBuffer    = D3DUnlockExecuteBuffer32;
        pDriverData->DDExecuteBufferCallbacks.dwFlags                = DDHAL_EXEBUFCB32_CANCREATEEXEBUF
                                                                     | DDHAL_EXEBUFCB32_CREATEEXEBUF
                                                                     | DDHAL_EXEBUFCB32_DESTROYEXEBUF
                                                                     | DDHAL_EXEBUFCB32_LOCKEXEBUF
                                                                     | DDHAL_EXEBUFCB32_UNLOCKEXEBUF;
#endif
    }
    else
#endif //!DO_NOT_SUPPORT_VERTEX_BUFFERS
    {
        /*
         * we are not interested in any vertex buffer allocation
         */
        memset (&pDriverData->DDExecuteBufferCallbacks,0,sizeof(pDriverData->DDExecuteBufferCallbacks));
        pDriverData->DDExecuteBufferCallbacks.dwSize = sizeof(pDriverData->DDExecuteBufferCallbacks);
    }

    /*
     * capabilities supported
     */
    if (NO_OVERLAY) {
        pHalInfo->ddCaps.dwCaps = DDCAPS_ALIGNSTRIDE |
                                  DDCAPS_ALIGNBOUNDARYDEST |
                                  DDCAPS_BLT |
                                  DDCAPS_BLTFOURCC |
                                  DDCAPS_BLTSTRETCH |
                                  DDCAPS_BLTQUEUE |
                                  DDCAPS_BLTCOLORFILL |
                                  DDCAPS_CANBLTSYSMEM |
                                  DDCAPS_ALPHA |
                                  DDCAPS_COLORKEY |
                                  DDCAPS_READSCANLINE |
                                  DDCAPS_BLTDEPTHFILL |
                                  DDCAPS_3D;
    } else {
        pHalInfo->ddCaps.dwCaps = DDCAPS_ALIGNSTRIDE |
                                  DDCAPS_ALIGNBOUNDARYDEST |
                                  DDCAPS_BLT |
                                  DDCAPS_BLTFOURCC |
                                  DDCAPS_BLTSTRETCH |
                                  DDCAPS_BLTQUEUE |
                                  DDCAPS_BLTCOLORFILL |
                                  DDCAPS_CANBLTSYSMEM |
                                  DDCAPS_ALPHA |
                                  DDCAPS_COLORKEY |
                                  DDCAPS_OVERLAY |
                                  DDCAPS_OVERLAYCANTCLIP  |
                                  DDCAPS_OVERLAYFOURCC |
                                  DDCAPS_OVERLAYSTRETCH |
                                  DDCAPS_READSCANLINE |
                                  DDCAPS_BLTDEPTHFILL |
                                  DDCAPS_3D;
    }

#ifndef WINNT
    pHalInfo->ddCaps.dwCaps |= DDCAPS_GDI;
#endif

#ifndef WINNT
    if ((pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) &&
        (global.dwDXRuntimeVersion >= 0x0800) &&
#ifdef TEXFORMAT_CRD
        (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED))
#else   // !TEXFORMAT_CRD
        ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_PALETTEENABLE_MASK) == D3D_REG_PALETTEENABLE_ENABLE))
#endif  // !TEXFORMAT_CRD
    {
        pHalInfo->ddCaps.dwCaps |= DDCAPS_PALETTE;
    }
#endif

    pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_NOPAGELOCKREQUIRED
//                             | DDCAPS2_CERTIFIED           // **** Turning this on breaks HAL enumeration ****
                             |  DDCAPS2_WIDESURFACES
                             |  DDCAPS2_PRIMARYGAMMA
                             |  DDCAPS2_CANRENDERWINDOWED
                             |  DDCAPS2_NONLOCALVIDMEMCAPS
                             |  DDCAPS2_COPYFOURCC;

    if (pDriverData->dwBusType == NV_BUS_TYPE_AGP) {
        // intially set DDCAPS2_NONLOCALVIDMEM here, it may be cleared later
        // in buildDDHalInfo16 on Win9x, or in DrvGetDirectDrawInfo on Win2K
        pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_NONLOCALVIDMEM;
    }


    if (!NO_OVERLAY) {
        pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_CANBOBHARDWARE
                                 |  DDCAPS2_CANBOBNONINTERLEAVED
                                 |  DDCAPS2_CANBOBINTERLEAVED
                                 |  DDCAPS2_CANBOBHARDWARE
                                 |  DDCAPS2_CANFLIPODDEVEN;
    }

#if defined(TEX_MANAGE)
    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_TEXTUREMANAGEMENTENABLE_MASK) == D3D_REG_TEXTUREMANAGEMENTENABLE_ENABLE) {
        pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_CANMANAGETEXTURE;
    }
#endif

    // set flipnovsync flag only on hardware later than NV4
    if (pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_0049_DAC |
                                                  NVCLASS_0067_DAC |
                                                  NVCLASS_007C_DAC)) {
        pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_FLIPNOVSYNC;
    }


    if (NO_OVERLAY) {
        pHalInfo->ddCaps.dwCKeyCaps = DDCKEYCAPS_SRCBLT;

        pHalInfo->ddCaps.dwFXCaps = DDFXCAPS_BLTARITHSTRETCHY |
                                    DDFXCAPS_BLTMIRRORLEFTRIGHT |
                                    DDFXCAPS_BLTMIRRORUPDOWN |
                                    DDFXCAPS_BLTSHRINKX |
                                    DDFXCAPS_BLTSHRINKY |
                                    DDFXCAPS_BLTSTRETCHX |
                                    DDFXCAPS_BLTSTRETCHY |
                                    DDFXCAPS_BLTALPHA |
                                    DDFXCAPS_BLTFILTER;
    } else {
        if ((pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) || pDriverData->vpp.regOverlayColourControlEnable)
            pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_COLORCONTROLOVERLAY;

        pHalInfo->ddCaps.dwCKeyCaps = DDCKEYCAPS_SRCBLT |
                                      DDCKEYCAPS_DESTOVERLAY |
                                      DDCKEYCAPS_DESTOVERLAYONEACTIVE;

        pHalInfo->ddCaps.dwFXCaps = DDFXCAPS_BLTARITHSTRETCHY |
                                    DDFXCAPS_BLTMIRRORLEFTRIGHT |
                                    DDFXCAPS_BLTMIRRORUPDOWN |
                                    DDFXCAPS_BLTSHRINKX |
                                    DDFXCAPS_BLTSHRINKY |
                                    DDFXCAPS_BLTSTRETCHX |
                                    DDFXCAPS_BLTSTRETCHY |
                                    DDFXCAPS_BLTALPHA |
                                    DDFXCAPS_BLTFILTER |
                                    DDFXCAPS_OVERLAYFILTER |
                                    DDFXCAPS_OVERLAYMIRRORUPDOWN |
                                    DDFXCAPS_OVERLAYSHRINKX |
                                    DDFXCAPS_OVERLAYSHRINKY |
                                    DDFXCAPS_OVERLAYSTRETCHX |
                                    DDFXCAPS_OVERLAYARITHSTRETCHY |
                                    DDFXCAPS_OVERLAYSTRETCHY;
    }

#ifndef WINNT // sigh this causes us to die HMH
    if ((pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) &&
        (global.dwDXRuntimeVersion >= 0x0800) &&
#ifdef TEXFORMAT_CRD
        (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED))
#else   // !TEXFORMAT_CRD
        ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_PALETTEENABLE_MASK) == D3D_REG_PALETTEENABLE_ENABLE))
#endif  // !TEXFORMAT_CRD
    {
        pHalInfo->ddCaps.dwPalCaps = DDPCAPS_8BIT; // | DDPCAPS_ALLOW256;
    }
#endif

    pHalInfo->ddCaps.dwFXAlphaCaps  = 0;

    if (NO_OVERLAY) {
        pHalInfo->ddCaps.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY |
                                          DDSCAPS_PRIMARYSURFACE |
                                          DDSCAPS_HWCODEC |
                                          DDSCAPS_OFFSCREENPLAIN |
                                          DDSCAPS_VISIBLE |
                                          DDSCAPS_FLIP |
                                          DDSCAPS_FRONTBUFFER |
                                          DDSCAPS_BACKBUFFER |
                                          DDSCAPS_ZBUFFER |
                                          DDSCAPS_3DDEVICE |
                                          DDSCAPS_ALPHA |
                                          DDSCAPS_TEXTURE |
                                          DDSCAPS_MIPMAP |
                                          DDSCAPS_EXECUTEBUFFER;
    } else {
        pHalInfo->ddCaps.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY |
                                          DDSCAPS_PRIMARYSURFACE |
                                          DDSCAPS_HWCODEC |
                                          DDSCAPS_OFFSCREENPLAIN |
                                          DDSCAPS_VISIBLE |
                                          DDSCAPS_FLIP |
                                          DDSCAPS_FRONTBUFFER |
                                          DDSCAPS_BACKBUFFER |
                                          DDSCAPS_OVERLAY |
                                          DDSCAPS_ZBUFFER |
                                          DDSCAPS_3DDEVICE |
                                          DDSCAPS_ALPHA |
                                          DDSCAPS_TEXTURE |
                                          DDSCAPS_MIPMAP |
                                          DDSCAPS_EXECUTEBUFFER;
    }

#ifndef WINNT
    pHalInfo->ddCaps.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
#endif

#ifndef WINNT
    if ((pDriverData->nvD3DPerfData.dwNVClasses & (NVCLASS_FAMILY_CELSIUS | NVCLASS_FAMILY_KELVIN)) &&
        (global.dwDXRuntimeVersion >= 0x0800) &&
#ifdef TEXFORMAT_CRD
        (getDC()->nvD3DRegistryData.regSurfaceFormatsDX8 & D3D_REG_SURFACEFORMATS_PALETTIZED))
#else   // !TEXFORMAT_CRD
        ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_PALETTEENABLE_MASK) == D3D_REG_PALETTEENABLE_ENABLE))
#endif  // !TEXFORMAT_CRD
    {
        pHalInfo->ddCaps.ddsCaps.dwCaps |= DDSCAPS_PALETTE;
    }
#endif

#ifdef WINNT
    if (global.dwDXRuntimeVersion >= 0x800)
#else // !WINNT
    if (1)
#endif // !WINNT
    {
        for ( i=0; i < DD_ROP_SPACE; i++ )
            pHalInfo->ddCaps.dwSVBRops[i] = ropList[i];
        pHalInfo->ddCaps.dwSVBCaps = pHalInfo->ddCaps.dwCaps;
        pHalInfo->ddCaps.dwSVBCaps2 = pHalInfo->ddCaps.dwCaps2;
        pHalInfo->ddCaps.dwSVBCKeyCaps = pHalInfo->ddCaps.dwCKeyCaps;
        pHalInfo->ddCaps.dwSVBFXCaps = pHalInfo->ddCaps.dwFXCaps;

        pHalInfo->ddCaps.dwVSBCaps = DDCAPS_BLT |
            DDCAPS_BLTQUEUE;
        pHalInfo->ddCaps.dwVSBCKeyCaps = 0;
        pHalInfo->ddCaps.dwVSBFXCaps = 0;
        for ( i=0; i < DD_ROP_SPACE; i++ ) {
            pHalInfo->ddCaps.dwVSBRops[i] = 0;
        }
        pHalInfo->ddCaps.dwVSBRops[6] = SRCCOPYBIT;
    }
    else
    {
        // turn off system->video blits on Win2K DX7 runtime due to MS bug (TestDrive 6)
        for ( i=0; i < DD_ROP_SPACE; i++ )
            pHalInfo->ddCaps.dwSVBRops[i] = 0;

        pHalInfo->ddCaps.dwSVBCaps = 0;
        pHalInfo->ddCaps.dwSVBCaps2 = 0;
        pHalInfo->ddCaps.dwSVBCKeyCaps = 0;
        pHalInfo->ddCaps.dwSVBFXCaps = 0;
        pHalInfo->ddCaps.dwVSBCaps = 0;
        pHalInfo->ddCaps.dwVSBCKeyCaps = 0;
        pHalInfo->ddCaps.dwVSBFXCaps = 0;

        for ( i=0; i < DD_ROP_SPACE; i++ )
            pHalInfo->ddCaps.dwVSBRops[i] = 0;
    }

    /*
     * Z buffer bit depths supported
     */
    pHalInfo->ddCaps.dwZBufferBitDepths = DDBD_16 | DDBD_24;

    /*
     * NV3 Required alignments
     */
    pHalInfo->ddCaps.dwAlignBoundaryDest = 1;
    /* All video memory surfaces must be aligned with aligned strides */
    pHalInfo->ddCaps.dwAlignStrideAlign = pDriverData->dwSurfaceAlign;

    /*
     * NV3 stretching capabilities
     */
    pHalInfo->ddCaps.dwMinOverlayStretch = 1;
    pHalInfo->ddCaps.dwMaxOverlayStretch = 20000;

    pHalInfo->ddCaps.dwMinLiveVideoStretch = 1;
    pHalInfo->ddCaps.dwMaxLiveVideoStretch = 20000;

    pHalInfo->ddCaps.dwMinHwCodecStretch = 1;
    pHalInfo->ddCaps.dwMaxHwCodecStretch = 4095000;

    /*
     *  FOURCCs supported
     *
     *  Table address MUST be set in 16 bit portion of Direct Draw Driver but
     *  can add more FOURCC codes to table here if needed.
     */

    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        pHalInfo->ddCaps.dwNumFourCCCodes = NV_MAX_FOURCC;
    }
    else {
        pHalInfo->ddCaps.dwNumFourCCCodes = NV_MAX_FOURCC_REGULAR;
    }

    /*
     *  Maximum number of overlays supported
     */

    if (NO_OVERLAY) {
        pHalInfo->ddCaps.dwMaxVisibleOverlays = 0;
    } else {
        pHalInfo->ddCaps.dwMaxVisibleOverlays = 1;
    }

    /*
     * current video mode, the 16bit driver will fill this out for real.
     */
#ifndef WINNT   // Under WINNT, mode sets are handled by display driver
    pHalInfo->dwModeIndex = DDUNSUPPORTEDMODE;
#endif

    /*
     * required byte alignments of the scan lines for each kind of memory
     * (default is 4, NV3 needs 16, NV4 needs 32, NV10 wants 64
     */
    pHalInfo->vmiData.dwOffscreenAlign = pDriverData->dwSurfaceAlign;
    pHalInfo->vmiData.dwOverlayAlign   = NV_OVERLAY_BYTE_ALIGNMENT;
    pHalInfo->vmiData.dwTextureAlign   = pDriverData->dwSurfaceAlign; // Texture surface, not our optimized texture
    pHalInfo->vmiData.dwAlphaAlign     = pDriverData->dwSurfaceAlign;
    pHalInfo->vmiData.dwZBufferAlign   = pDriverData->dwSurfaceAlign;

#ifdef NVD3D
    pDriverData->bDirtyRenderTarget = TRUE;
    pDriverData->ThreeDRenderingOccurred = FALSE;

    // If 24-bit z-buffers are disabled, only show 16-bit formats.
    if ((getDC()->nvD3DRegistryData.regD3DEnableBits1 & D3D_REG_Z24ENABLE_MASK) == D3D_REG_Z24ENABLE_DISABLE) {
        pHalInfo->ddCaps.dwZBufferBitDepths = DDBD_16;
    }
#endif // NVD3D

#ifdef NVPE
    pDriverData->bEnableVPE = 0;
    if (NVPIsVPEEnabled(pDriverData, 0)) {
        pDriverData->bEnableVPE = 1;
    }
#else
    // Grab the local registry path and append the DirectDraw subkey to the string
    // without using the C runtime library.
#ifndef WINNT
    MyExtEscape(NV_ESC_GET_LOCAL_REGISTRY_PATH, 0, NULL, 256, regStr);
#endif // BUGBUG WINNT

    nvStrCat(&(regStr[4]), "\\" NV4_REG_DIRECT_DRAW_SUBKEY);

    //VPE enable stuff
    pDriverData->bEnableVPE = 1;

    if (RegOpenKeyEx((HKEY)(*((LPDWORD)&regStr[0])), &regStr[4], 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS) {
        hKey = 0;
    }
    if (hKey) {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        //check for VPE enable bit
        if (RegQueryValueEx(hKey, NV4_REG_VPE_ENABLE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS) {
            if(!lValue)
                pDriverData->bEnableVPE = 0;
        }
        RegCloseKey(hKey);
    }
#endif // NVPE

    //VPE enable stuff
    if(pDriverData->bEnableVPE == 1 && !NO_OVERLAY) {
      /*
       * Video Port Capabilities
       */
      pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_VIDEOPORT               |
                                             DDCAPS2_CANBOBINTERLEAVED       |
                                             DDCAPS2_CANBOBNONINTERLEAVED    |
                                             DDCAPS2_AUTOFLIPOVERLAY;

      pHalInfo->ddCaps.dwMaxVideoPorts = 1;
      pHalInfo->ddCaps.dwCurrVideoPorts = 0;

    }
    pHalInfo->GetDriverInfo = GetDriverInfo32;
    pHalInfo->dwFlags |= DDHALINFO_GETDRIVERINFOSET;
    if (
#if (NVARCH >= 0x020)
    (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) ||
#endif // (NVARCH >= 0x020)
    (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS)
     )
    {
    pHalInfo->dwFlags |= DDHALINFO_GETDRIVERINFO2;
    }
#ifndef WINNT
    pHalInfo->dwFlags |= DDHALINFO_MODEXILLEGAL;
#endif
#ifdef  STEREO_SUPPORT
    StereoModifyDDHALInfo();
#endif  //STEREO_SUPPORT

    DPF_LEVEL (NVDBG_LEVEL_INFO, "Display: Filling in ddHALInfo.GetDriverInfo: %x", GetDriverInfo32);

    dbgTracePop();
    return (TRUE);

}  // buildDDHALInfo32

//---------------------------------------------------------------------------

/*
    GetFlatCodeSel
*/
DWORD __stdcall GetFlatCodeSel (void)
{
    unsigned long dwSel;

    dbgTracePush ("GetFlatCodeSel");

    _asm
    {
        mov     ax,cs
        movzx   eax,ax
        mov     dwSel,eax
    }

    dbgTracePop();
    return (dwSel);
}

//---------------------------------------------------------------------------

/*
    GetFlatDataSel
*/
DWORD __stdcall GetFlatDataSel (void)
{
    unsigned long dwSel;

    dbgTracePush ("GetFlatDataSel");

    _asm
    {
        mov     ax,ds
        movzx   eax,ax
        mov     dwSel,eax
    }

    dbgTracePop();
    return (dwSel);
}

//---------------------------------------------------------------------------

#ifndef WINNT   // This is Win9x specific
/*
    buildDDHALInfo16 ()

    This procedure used to be in the display driver. It was there because
    the code in this routine needs to execute just after a modeset and
    before we call the DirectX internal funciton lpSetInfo and there is
    no mechanism for getting over to the DirectX driver and back to the
    display driver in a serialized fashion.  i.e. If we do a thunk to get
    over to the 32bit DirectX driver, then the Win16 mutex gets released
    and the sequence is not serialized properly.

    However, this code is properly DirectX code so it belongs in this
    driver, not over in the display driver. The bold way to execute this
    code in sequence with the code in the display driver is to CALL it
    directly without doing a thunk or releaseing the mutex!!! This imposes
    a few serious restrictions on what can occur in this routine.

    This routine can only have straight C code in it -- DO NOT call out
    to any system functions because you are not really running in flat
    land. When this routine is executing it is running as a part of the
    16 bit display driver!!  This should not be too much of a problem,
    though, because there is no necessity for doing more than setting
    capability bits and structure fields herein.

    If you call out to a system function (like memory allocation or
    any Kernel, USER32, GDI32, etc function, you will LOCK the system!!
*/

DWORD __stdcall buildDDHALInfo16 (void)
{
    LPDDHALMODEINFO     lpMode;

    // set this flag so that all functions called herein
    // know not to make any system calls
    global.b16BitCode = TRUE;

    DIBENGINEHDR1 *pDibeng = (DIBENGINEHDR1 *)pDXShare->pFlatDibeng;

    // set up pointer to DDHALINFO
    LPDDHALINFO pHalInfo = (LPDDHALINFO)pDXShare->pHalInfo;

    // Grab the mode list and add each mode.  This is largely just to fix
    // a bug in Motocross Madness where the mode list is gotten before
    // the 32bit DirectX driver initializes and builds a mode table. The
    // mode table returned here is a shotgun-return-all-the-modes in the
    // registry approach. Except for motocross madness (as far as we know),
    // all other DX apps won't grab the modes until after DriverInit
    // is called. DriverInit calls BuildDDHalModeTable which computes
    // the real mode list (should be very close to what we return here.)
#if 0
    LPMODEENTRYANDPITCH lpM = (LPMODEENTRYANDPITCH) pDXShare->lpMasterModeList;
    LPDDHALMODEINFO pMode = pDriverData->ModeList;
    for (i = 0; lpM[i].dwXRes != 0; i++) {
        AddModeToTable (pMode++, lpM[i].dwXRes, lpM[i].dwYRes, lpM[i].dwBpp); //, lpM[i].dwPitch);
    }

    SortDDHalModeList ();
#endif

    if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) { // Starting with NV10, we never close this channel
        if (pDriverData->NvDevVideoFlatPio != 0) {
            pDriverData->CloseNvDevVideoFlatPio = pDriverData->NvDevVideoFlatPio;
            pDriverData->NvDevVideoFlatPio = 0;
        }
    }

    DWORD dwRootIndex = 0;
    getAdapterIndex(pDXShare, &dwRootIndex);
    if ((dwRootIndex           == -1)                // if we are head 0
     || (pDXShare->deWidth     != GET_MODE_WIDTH())  // or if the mode has really changed
     || (pDXShare->deHeight    != GET_MODE_HEIGHT())
     || (pDXShare->deBitsPixel != GET_MODE_BPP()))
    {
        nvDisable32();
        pDXShare->deWidth     = GET_MODE_WIDTH();
        pDXShare->deHeight    = GET_MODE_HEIGHT();
        pDXShare->deBitsPixel = GET_MODE_BPP();
    }

    // Set a ptr to the table
    pHalInfo->lpModeInfo = (struct _DDHALMODEINFO *) (pDriverData->dwModeListSelector << 16);

    // Find the mode number
    pDXShare->dwModeNumber = -1;

    for (int i = 0; pDriverData->ModeList[i].dwWidth != 0; i++)
    {
        if ((pDriverData->ModeList[i].dwWidth  == pDibeng->deWidth)     &&
            (pDriverData->ModeList[i].dwHeight == pDibeng->deHeight)    &&
            (pDriverData->ModeList[i].dwBPP    == pDibeng->deBitsPixel) &&
            ((pDriverData->ModeList[i].lPitch * pDriverData->ModeList[i].dwHeight) <= pDriverData->TotalVRAM))
        {
#ifdef  STEREO_SUPPORT_
            if (STEREO_ENABLED)
            {
                __asm int 3
                DWORD dwWidth = pDriverData->ModeList[i].dwWidth;
                int RealIndex = i;
                for (i=i+1; pDriverData->ModeList[i].dwWidth == dwWidth; i++)
                {
                    if (pDriverData->ModeList[i].wRefreshRate == 0x1234)
                    {
                        //Our fake entry
                        pDriverData->ModeList[i] = pDriverData->ModeList[RealIndex];
                        break;
                    }
                }
            }
#endif  //STEREO_SUPPORT

            pDXShare->dwModeNumber = i;
            break;
        }
    }

    // Overlay information
    pHalInfo->ddCaps.dwMaxVisibleOverlays = 1;
    pHalInfo->ddCaps.dwCurrVisibleOverlays = 0;

    // FOURCC code information
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        pHalInfo->ddCaps.dwNumFourCCCodes = NV_MAX_FOURCC;
    }
    else {
        pHalInfo->ddCaps.dwNumFourCCCodes = NV_MAX_FOURCC_REGULAR;
    }

    // lpdwFourCC must be a 16:16 pointer. It is set at FirstTimeInit
    pHalInfo->lpdwFourCC = (unsigned long *) (pDriverData->dwFourCCSelector << 16);

    // current video mode
    pHalInfo->dwModeIndex = pDXShare->dwModeNumber;
    lpMode = &(pDriverData->ModeList[pHalInfo->dwModeIndex]);

    // current primary surface attributes
    pHalInfo->vmiData.fpPrimary = VIDMEM_ADDR(GET_PRIMARY_ADDR());

    // fill in the pixel format
    pHalInfo->vmiData.ddpfDisplay.dwSize  = sizeof (DDPIXELFORMAT);
    pHalInfo->vmiData.ddpfDisplay.dwFlags = DDPF_RGB;

    if  (pDXShare->dwModeNumber != -1)
    {
        pHalInfo->vmiData.ddpfDisplay.dwRGBBitCount     = lpMode->dwBPP;
        pHalInfo->vmiData.dwDisplayWidth                = lpMode->dwWidth;
        pHalInfo->vmiData.dwDisplayHeight               = lpMode->dwHeight;
        pHalInfo->vmiData.lDisplayPitch                 = lpMode->lPitch;
        pHalInfo->vmiData.ddpfDisplay.dwRGBAlphaBitMask = lpMode->dwAlphaBitMask;
        pHalInfo->vmiData.ddpfDisplay.dwRBitMask        = lpMode->dwRBitMask;
        pHalInfo->vmiData.ddpfDisplay.dwGBitMask        = lpMode->dwGBitMask;
        pHalInfo->vmiData.ddpfDisplay.dwBBitMask        = lpMode->dwBBitMask;

        if (lpMode->wFlags & DDMODEINFO_PALETTIZED) {
            pHalInfo->vmiData.ddpfDisplay.dwFlags |= DDPF_PALETTEINDEXED8;
        }
#ifdef SPOOF_ALPHA
        if (lpMode->dwAlphaBitMask) {
            pHalInfo->vmiData.ddpfDisplay.dwFlags |= DDPF_ALPHAPIXELS;
        }
#endif
    }

#ifdef NV_AGP
    // we are managing the heaps, don't ask MS for one
    pHalInfo->vmiData.dwNumHeaps = 0;
    if (pDriverData->dwBusType == NV_BUS_TYPE_AGP) {
        pHalInfo->ddCaps.dwCaps2 |=  DDCAPS2_NONLOCALVIDMEM;
    } else {
        pHalInfo->ddCaps.dwCaps2 &= ~DDCAPS2_NONLOCALVIDMEM;
    }
#else // !NV_AGP
    // Setup the AGP heap
    DWORD dwRoot;
    getAdapterIndex(pDXShare, &dwRoot);
    if ((pDriverData->dwBusType == NV_BUS_TYPE_AGP) &&
        (pDXShare->dwModeNumber != -1) &&
        (dwRoot == -1)) // this is the second or later device on a multi-head card (no AGP heap)
    {
        pHalInfo->vmiData.dwNumHeaps = 1;
        pHalInfo->ddCaps.dwCaps2 |= DDCAPS2_NONLOCALVIDMEM;
    }
    else
    {
        pHalInfo->vmiData.dwNumHeaps = 0;
        pHalInfo->ddCaps.dwCaps2 &= ~DDCAPS2_NONLOCALVIDMEM;
    }
#endif // !NV_AGP

    // pvmList must be a 16:16 pointer. It is set at FirstTimeInit.
    pHalInfo->vmiData.pvmList = (struct _VIDMEM *) (pDriverData->dwPvmListSelector << 16);

    if (pHalInfo->vmiData.dwNumHeaps == 1)
    {
        // Describe NONLOCALVIDMEM heap
        pDriverData->vidMem[0].dwFlags = VIDMEM_ISLINEAR   |
                                         VIDMEM_ISNONLOCAL |  // AGP memory
                                         VIDMEM_ISWC;         // do write combining

        // don't place in nonlocal video memory on first pass
        // prefer not to allocate anything but
        // optimized textures and YV12 overlays
        pDriverData->vidMem[0].ddsCaps.dwCaps = DDSCAPS_OVERLAY        |
                                                DDSCAPS_OFFSCREENPLAIN |
                                                DDSCAPS_FRONTBUFFER    |
                                                DDSCAPS_BACKBUFFER     |
                                                DDSCAPS_ZBUFFER;

        // don't place in nonlocal video memory on 2nd pass
        // Even if no other memory, don't allocate anything but
        // optimized textures and YV12 overlays
        pDriverData->vidMem[0].ddsCapsAlt.dwCaps = DDSCAPS_OVERLAY        |
                                                   DDSCAPS_OFFSCREENPLAIN |
                                                   DDSCAPS_FRONTBUFFER    |
                                                   DDSCAPS_BACKBUFFER     |
                                                   DDSCAPS_ZBUFFER;

        pDriverData->vidMem[0].fpStart = 0x00000000;
        if (pDriverData->regSuper7Compat == 2) {
            // Allocate 32-sizeof(pushbuffer)-sizeof(defaultVB) megabytes of address space
            pDriverData->vidMem[0].fpEnd = pDriverData->regMaxAGPLimit - getDC()->nvPusher.getSize();
        } else {
            pDriverData->vidMem[0].fpEnd = pDriverData->regMaxAGPLimit - pDriverData->regPersistAGPSize;
        }
    }

    // The color alphas
    switch (pDibeng->deBitsPixel) {
        case 8 : pDriverData->physicalColorMask = 0xFF;     break;
        case 16: pDriverData->physicalColorMask = 0xFFFF;   break;
        case 32: pDriverData->physicalColorMask = 0xFFFFFF; break;
        default: break;
    }

    // callback functions
    pHalInfo->lpDDCallbacks        = (DDHAL_DDCALLBACKS *)        (pDriverData->dwDDCallbacksSelector << 16);
    pHalInfo->lpDDSurfaceCallbacks = (DDHAL_DDSURFACECALLBACKS *) (pDriverData->dwDDSurfaceCallbacksSelector << 16);
    pHalInfo->lpDDPaletteCallbacks = (DDHAL_DDPALETTECALLBACKS *) (pDriverData->dwDDPaletteCallbacksSelector << 16);
    pHalInfo->lpDDExeBufCallbacks  = (DDHAL_DDEXEBUFCALLBACKS *)  (pDriverData->dwDDExecuteBufferCallbackSelector << 16);

    ///////////////////////////////////////////////////////////////////
    // The 16bit side (the caller) needs 4 pieces of info back:
    // 1) A flat ptr to the HALInfo structure - assigned in FirstTimeInit
    // 2) The length of the HALInfo structure
    // 3) The HINSTANCE of this driver (MS calls this an instance handle but all they want is a unique per head/card identifier)

    pDXShare->dwHALInfoSize    = (unsigned long) sizeof(DDHALINFO);
    pDXShare->hDirectXInstance = (unsigned long) pDXShare;
    ///////////////////////////////////////////////////////////////////

#ifdef  STEREO_SUPPORT
    StereoModeSetProcessing();
#endif  //STEREO_SUPPORT

    // clear the 16-bit flag
    global.b16BitCode = FALSE;

    return (TRUE);
}

#endif // !WINNT

//---------------------------------------------------------------------------

// allocate space for major driver structures
// returns TRUE on success, FALSE on failure
#ifndef WINNT
BOOL nvAllocDriverStructures (void)
{
    dbgTracePush ("nvAllocDriverStructures");

    // allocate memory for global (one per card) data structures
    // this memory is only released when the driver dll is unloaded

    // alloc pDriverData
    pDriverData = new CDriverContext;

    if (!pDriverData) {
        // out of memory
        DPF ("nvAllocDriverStructures: out of memory allocating CDriverContext");
        dbgTracePop();
        return (FALSE);
    }

    // initialise pDriverData
    getDC()->create();

    // alloc ModeList
    void *pModeList = AllocIPM(sizeof(DDHALMODEINFO) * MAX_MODEENTRYS);
    if (!pModeList)
    {
        // out of memory - prepare to die
        DPF ("nvAllocDriverStructures: out of memory allocating DDHALMODEINFO");
        // free pDriverData
        delete pDriverData;
        pDriverData = NULL;
        // free pModeList
        FreeIPM(pModeList);
        dbgTracePop();
        return (FALSE);
    }

    // store the pointers to our newly allocated memory
    pDXShare->pDirectXData = (DWORD)pDriverData;
    pDriverData->ModeList  = (DDHALMODEINFO*)pModeList;

    dbgTracePop();
    return (TRUE);
}
#endif // !WINNT
//---------------------------------------------------------------------------

// FirstTimeInit()
// perform one-time initialization required on first call to DriverInit()

DWORD __stdcall FirstTimeInit (void)
{
    dbgTracePush ("FirstTimeInit");

    // Initialize DX state based on info obtained from the display driver
#ifdef WINNT
    pDriverData->BaseAddress       = 0;   // offset of frame buffer
    pDriverData->dwDeviceIDNum     = ppdev->ulDeviceReference;
    pDriverData->dwRootHandle      = ppdev->hClient;
    pDriverData->dwDeviceHandle    = ppdev->hDevice;
    pDriverData->NvDeviceVersion   = (unsigned short) ppdev->dwDeviceVersion;
    pDriverData->dwSurfaceAlignPad = ppdev->ulSurfaceAlign;
    pDriverData->dwSurfaceAlign    = ppdev->ulSurfaceAlign + 1;
    pDriverData->dwSurfaceBitAlign = ppdev->ulSurfaceBitAlign;
    pDriverData->NvBaseFlat   = (DWORD) ppdev->NvBaseAddr;
    pDriverData->thisDeviceID = pDriverData->dwDeviceHandle;
#else // !WINNT
    pDriverData->BaseAddress       = pDXShare->dwScreenLinearBase;  // linear base address of frame buffer
    pDriverData->dwDeviceIDNum     = pDXShare->dwNVDeviceID;
    pDriverData->dwRootHandle      = pDXShare->dwRootHandle;
    pDriverData->dwDeviceHandle    = pDXShare->dwDeviceHandle;
    pDriverData->NvDeviceVersion   = (unsigned short) pDXShare->dwDeviceVersion;
    pDriverData->dwSurfaceAlignPad = NV_BYTE_ALIGNMENT_PAD;
    pDriverData->dwSurfaceAlign    = NV_BYTE_ALIGNMENT;
    pDriverData->dwSurfaceBitAlign = NV_BIT_ALIGNMENT;
    pDriverData->NvBaseFlat        = pDXShare->dwDeviceLinearBase;
    pDriverData->flatSelector      = (unsigned short) GetFlatCodeSel();
    pDriverData->thisDeviceID      = pDXShare->dwNVDeviceID;
    // Setup an initial default of 32MB - under Win2k we've already computed the limit
    pDriverData->regMaxAGPLimit = NV_MAX_AGP_MEMORY_LIMIT;

    DWORD dwRoot;
    if (getAdapterIndex (pDXShare, &dwRoot) == 0) {
        SetSharedFSDOSBoxPtr ((U032 *)(&(pDriverData->dwFullScreenDOSStatus)));
    }
#endif // !WINNT

    NvRmConfigGet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_CFG_RAM_SIZE_MB, &(pDriverData->TotalVRAM));
    pDriverData->TotalVRAM       <<= 20;
    pDriverData->dwDeviceClass     = pDriverData->dwDeviceIDNum + (NV01_DEVICE_0 - 1);

#if (NVARCH < 0x010)
    if (pDriverData->NvDeviceVersion > NV_DEVICE_VERSION_5) {
        pDriverData->NvDeviceVersion = NV_DEVICE_VERSION_5;
    }
#endif // !NV10

    pDriverData->dwVersion = DDMINI_VERSION;

    NvRmConfigGet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_CFG_BUS_TYPE, &(pDriverData->dwBusType));

#ifdef WINNT
    {
        // Ask miniport if AGP should be enabled.
        DWORD ulReturn = 0;

        if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SHOULD_AGP_BE_ENABLED,
                               NULL, 0, NULL, 0, &ulReturn))
        {
            pDriverData->dwBusType = 1; // Force it to be PCI if miniport thinks otherwise.
        }
    }
#endif

    pDriverData->pCurrentChannelID = 0;

    // get the number of heads (DACs) on this device
    NvRmConfigGet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle, NV_CFG_NUMBER_OF_HEADS, &pDriverData->dwHeads);

    if (pDriverData->thisDeviceID <= 1) {
        pDriverData->thisDeviceID = 0;
    }

    pDriverData->dwDrvRefCount = 0;

#ifndef TEXFORMAT_CRD
    DWORD dwNum = 0;

    // A few other random things that never change
    pDriverData->fourCC[dwNum++] = FOURCC_UYVY;
    pDriverData->fourCC[dwNum++] = FOURCC_UYNV;
    pDriverData->fourCC[dwNum++] = FOURCC_YUY2;
    pDriverData->fourCC[dwNum++] = FOURCC_YUNV;
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10)
    pDriverData->fourCC[dwNum++] = FOURCC_NVDS;
    pDriverData->fourCC[dwNum++] = FOURCC_YV12;
    pDriverData->fourCC[dwNum++] = FOURCC_YVU9;
    pDriverData->fourCC[dwNum++] = FOURCC_IF09;
    pDriverData->fourCC[dwNum++] = FOURCC_IV32;
    pDriverData->fourCC[dwNum++] = FOURCC_IV31;
    pDriverData->fourCC[dwNum++] = FOURCC_RAW8;
    pDriverData->fourCC[dwNum++] = FOURCC_NVT0;
    pDriverData->fourCC[dwNum++] = FOURCC_NVT1;
    pDriverData->fourCC[dwNum++] = FOURCC_NVT2;
    pDriverData->fourCC[dwNum++] = FOURCC_NVT3;
    pDriverData->fourCC[dwNum++] = FOURCC_NVT4;
    pDriverData->fourCC[dwNum++] = FOURCC_NVT5;
    pDriverData->fourCC[dwNum++] = FOURCC_NVS0;
    pDriverData->fourCC[dwNum++] = FOURCC_NVS1;
    pDriverData->fourCC[dwNum++] = FOURCC_NVS2;
    pDriverData->fourCC[dwNum++] = FOURCC_NVS3;
    pDriverData->fourCC[dwNum++] = FOURCC_NVS4;
    pDriverData->fourCC[dwNum++] = FOURCC_NVS5;

#ifdef DXT_SUPPORT
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        pDriverData->fourCC[dwNum++] = FOURCC_DXT1;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT2;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT3;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT4;
        pDriverData->fourCC[dwNum++] = FOURCC_DXT5;
    }
#endif
#if defined(HILO_SUPPORT_DX7) || defined(HILO_SUPPORT_DX8)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN) {
        pDriverData->fourCC[dwNum++] = FOURCC_NVHS;
        pDriverData->fourCC[dwNum++] = FOURCC_NVHU;
    }
#endif

#endif // !TEXFORMAT_CRD

    dbgTracePop();
    return  (TRUE);
}

//---------------------------------------------------------------------------
// ResetTwinViewState
//      Reset anything that has been destroyed by a change to the TwinView state
//

BOOL ResetTwinViewState (GLOBALDATA *pDriverData)
{
    dbgTracePush ("ResetTwinViewState");

    pDriverData->dwFullScreenDOSStatus &= ~FSDOSSTATUS_TWINVIEW_STATE_CHANGE;

#ifdef WINNT
    pDriverData->dwDesktopState = ppdev->TwinView_State;
#else

    // refresh the adapter table
    nvFillAdapterTable();

    // reinstantiate desktop state and reconnect DX in case TwinView state has changed
    NVTWINVIEWDATA twinViewData;
    DWORD dwOldDesktopState = pDriverData->dwDesktopState;
    memset(&twinViewData, 0, sizeof(twinViewData));
    twinViewData.dwFlag   = NVTWINVIEW_FLAG_GET;
    twinViewData.dwAction = NVTWINVIEW_ACTION_SETGET_STATE;
    MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, 0, NULL, sizeof(twinViewData), (char*)&twinViewData);
    nvAssert(twinViewData.dwSuccess);
    pDriverData->dwDesktopState = twinViewData.dwState;

    // reenable primary flip objects
    for (DWORD dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
        NvRmFree (pDriverData->dwRootHandle, NV_DD_DEV_DMA, NV_DD_VIDEO_LUT_CURSOR_DAC + dwHead);
    }

    if (!nvCreateDACObjects (NV_DD_DEV_DMA, NV_DD_VIDEO_LUT_CURSOR_DAC)) {
        dbgTracePop();
        return (FALSE);
    }

    for (dwHead = 0; dwHead < pDriverData->dwHeads; dwHead ++) {
        // initialize LUT-cursor-DAC object
        nvPushData (0,dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000);
        nvPushData (1,NV_DD_VIDEO_LUT_CURSOR_DAC + dwHead);
        nvPushData (2,dDrawSubchannelOffset(NV_DD_SPARE) + NV067_SET_CONTEXT_DMA_NOTIFIES | 0x1C0000);
        nvPushData (3,NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY + dwHead);
        nvPushData (4,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // image a
        nvPushData (5,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // image b
        nvPushData (6,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // LUT a
        nvPushData (7,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // LUT b
        nvPushData (8,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // cursor a
        nvPushData (9,NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);  // cursor b
        nvPushData (10,dDrawSubchannelOffset(NV_DD_SPARE) + NV067_SET_DAC(0) | 0x40000);
        nvPushData (11,asmMergeCoords(GET_MODE_WIDTH(), GET_MODE_HEIGHT()));
        nvPusherAdjust (12);
    }

#endif

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

#ifndef WINNT

// The Dll is about to be loaded.  Do whatever you want ...

DWORD __stdcall AboutToBeLoaded (void)
{
    dbgTracePush ("AboutToBeLoaded");
    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

// The Dll is about to be unloaded.  Do whatever you want ...
// Unfortunately, we do not have a context ptr. Assume whatever
// was the last one although this could be wrong.

DWORD __stdcall AboutToBeUnloaded (void)
{
    dbgTracePush ("AboutToBeUnloaded");

    // clear the display driver's pointers to our global data structures
    for (int i = 0; i < MAX_ADAPTERS; i++) {

        pDriverData = (CDriverContext *)g_adapterData[i].pDriverData;
        pDXShare    = g_adapterData[i].pDXShare;

        // this memory will be released when the IPM heap is destroyed
        if (pDXShare) {
            pDXShare->pHalInfo       = NULL;
            pDXShare->pNvPrimarySurf = NULL;
        }

        if (pDriverData) {

            VppDisable(&pDriverData->vpp);   // Free objects...
            VppDestructor(&pDriverData->vpp);

#ifdef STEREO_SUPPORT
            DestroyStereo();
#endif // STEREO_SUPPORT

            // Destroy Item Arrays
            DestroyItemArrays();

            // Set this to NULL, so we won't try to call from the display driver
            // and GPF.
            pDXShare->pfnInterfaceFunc = (unsigned long) NULL;

            // Free the 16bit selectors we got from the display driver.
            FreeSelectorsFromDisplayDriver ();

            // Purge the heap of all DirectX allocations
            NVHEAP_PURGE ();

            // clear this entry in the table
            memset(&g_adapterData[i], 0, sizeof(NV_ADAPTER_TYPE));
        }
    }

    // destroy the heap
    DestroyIPM();

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

/*
    if nGet is TRUE then, selectors are allocated (unless they are
    already allocated.) if nGet = FALSE, they are freed (unless they
    are already freed.)
*/
DWORD __stdcall GetSelectorsFromDisplayDriver (void)
{
    DWORD dwEsc[2];

    dbgTracePush ("GetSelectorsFromDisplayDriver");

    if (!pDriverData->dwFourCCSelector) {

        // Allocate some selectors from the display driver. We need
        // these to fill in the HALInfo table correctly later.
        dwEsc[0] = (DWORD) &(pDriverData->fourCC[0]);
        dwEsc[1] = NV_MAX_FOURCC*4;
        if (!MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_ALLOC_SELECTOR_AND_MAP, 8,
            (const char *) &(dwEsc[0]), 4, (char *) &pDriverData->dwFourCCSelector)) {
            dbgTracePop();
            return FALSE;
        }

        // Allocate some selectors from the display driver. We need
        // these to fill in the HALInfo table correctly later.
        dwEsc[0] = (DWORD) pDriverData->ModeList;
        dwEsc[1] = sizeof(DDHALMODEINFO) * MAX_MODEENTRYS;
        if (!MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_ALLOC_SELECTOR_AND_MAP, 8,
            (const char *) &(dwEsc[0]), 4, (char *) &pDriverData->dwModeListSelector)) {
            dbgTracePop();
            return FALSE;
        }

        dwEsc[0] = (DWORD) &(pDriverData->vidMem);
        dwEsc[1] = sizeof(VIDMEM) * 2;
        if (!MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_ALLOC_SELECTOR_AND_MAP, 8,
            (const char *) &(dwEsc[0]), 4, (char *) &pDriverData->dwPvmListSelector)) {
            dbgTracePop();
            return FALSE;
        }

        dwEsc[0] = (DWORD) &(pDriverData->DDCallbacks);
        dwEsc[1] = sizeof(pDriverData->DDCallbacks);
        if (!MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_ALLOC_SELECTOR_AND_MAP, 8,
            (const char *) &(dwEsc[0]), 4, (char *) &pDriverData->dwDDCallbacksSelector)) {
            dbgTracePop();
            return FALSE;
        }

        dwEsc[0] = (DWORD) &(pDriverData->DDSurfaceCallbacks);
        dwEsc[1] = sizeof(pDriverData->DDSurfaceCallbacks);
        if (!MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_ALLOC_SELECTOR_AND_MAP, 8,
            (const char *) &(dwEsc[0]), 4, (char *) &pDriverData->dwDDSurfaceCallbacksSelector)) {
            dbgTracePop();
            return FALSE;
        }

        dwEsc[0] = (DWORD) &(pDriverData->DDPaletteCallbacks);
        dwEsc[1] = sizeof(pDriverData->DDPaletteCallbacks);
        if (!MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_ALLOC_SELECTOR_AND_MAP, 8,
            (const char *) &(dwEsc[0]), 4, (char *) &pDriverData->dwDDPaletteCallbacksSelector)) {
            dbgTracePop();
            return FALSE;
        }

        dwEsc[0] = (DWORD) &(pDriverData->DDExecuteBufferCallbacks);
        dwEsc[1] = sizeof(pDriverData->DDExecuteBufferCallbacks);
        if (!MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_ALLOC_SELECTOR_AND_MAP, 8,
            (const char *) &(dwEsc[0]), 4, (char *) &pDriverData->dwDDExecuteBufferCallbackSelector)) {
            dbgTracePop();
            return FALSE;
        }
    }

    dbgTracePop();
    return(1);
}

DWORD __stdcall FreeSelectorsFromDisplayDriver (void)
{
    dbgTracePush ("FreeSelectorsFromDisplayDriver");

    if (pDriverData->dwFourCCSelector) {
        MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_UNMAP_AND_FREE_SELECTOR, 4,
            (const char *) &pDriverData->dwFourCCSelector, 0, 0);
        pDriverData->dwFourCCSelector = 0;
    }

    if (pDriverData->dwModeListSelector) {
        MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_UNMAP_AND_FREE_SELECTOR, 4,
            (const char *) &pDriverData->dwModeListSelector, 0, 0);
        pDriverData->dwModeListSelector = 0;
    }

    if (pDriverData->dwPvmListSelector) {
        MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_UNMAP_AND_FREE_SELECTOR, 4,
            (const char *) &pDriverData->dwPvmListSelector, 0, 0);
        pDriverData->dwPvmListSelector = 0;
    }

    if (pDriverData->dwDDCallbacksSelector) {
        MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_UNMAP_AND_FREE_SELECTOR, 4,
            (const char *) &pDriverData->dwDDCallbacksSelector, 0, 0);
        pDriverData->dwDDCallbacksSelector = 0;
    }

    if (pDriverData->dwDDSurfaceCallbacksSelector) {
        MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_UNMAP_AND_FREE_SELECTOR, 4,
            (const char *) &pDriverData->dwDDSurfaceCallbacksSelector, 0, 0);
        pDriverData->dwDDSurfaceCallbacksSelector = 0;
    }

    if (pDriverData->dwDDPaletteCallbacksSelector) {
        MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_UNMAP_AND_FREE_SELECTOR, 4,
            (const char *) &pDriverData->dwDDPaletteCallbacksSelector, 0, 0);
        pDriverData->dwDDPaletteCallbacksSelector = 0;
    }

    if (pDriverData->dwDDExecuteBufferCallbackSelector) {
        MyExtEscape(pDXShare->dwHeadNumber, GN_ESC_UNMAP_AND_FREE_SELECTOR, 4,
            (const char *) &pDriverData->dwDDExecuteBufferCallbackSelector, 0, 0);
        pDriverData->dwDDExecuteBufferCallbackSelector = 0;
    }

    dbgTracePop();
    return 1;
}

//---------------------------------------------------------------------------

/*
    DrvInterfaceFuncC (DWORD dwInterfaceID)

    This routine is called by the display driver directly from 16bit
    land. Do not call any system functions or you will hang. But you can
    set variables and just do normal C code.
*/

void __stdcall DrvInterfaceFuncC(DWORD dwInterfaceID, DWORD dwExtraData)
{
    global.b16BitCode = TRUE;

    // pDXShare has been set to the correct context data just before
    // DrvInterfaceFuncC is called, but pDriverData has not. Set it now.
    pDriverData = (CDriverContext *)pDXShare->pDirectXData;

    switch (dwInterfaceID)
    {
        case GDIDX_PRE_SETINFOINIT:
            buildDDHALInfo16 ();
            break;

        case GDIDX_PRE_MODESET_NOTIFY:
            nvModeSetPrepare16();
            break;

        case GDIDX_POST_MODESET_NOTIFY:
            nvModeSetCleanup16();
            break;

        case GDIDX_DIRECTX_OS_VERSION:
            // dwExtraData is the system DirectX runtime version.
            global.dwDXRuntimeVersion = dwExtraData;
            break;

        case GDIDX_DRIVER_UNLOADING:
        {
            DWORD dwRootIndex;
            int dwIndex = getAdapterIndex(pDXShare, &dwRootIndex);
            if (dwIndex != -1) {
                g_adapterData[dwIndex].pDXShare = NULL;
            }
            break;
        }

        case GDIDX_EARLY_DX_CONNECT:
            pDriverData->dwFullScreenDOSStatus |= FSDOSSTATUS_TWINVIEW_STATE_CHANGE;
            // if we were in video mirror mode, then context data is no longer valid
            pDriverData->vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pHWCrtcCX = 0;
            pDriverData->vpp.dwOverlayFSTwinData.nvtwPhysicalModeImmediate.pGNLogdevCX = 0;
            pDriverData->vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pHWCrtcCX = 0;
            pDriverData->vpp.dwOverlayFSTwinData.nvtwLockUnlockHwcrtc.pGNLogdevCX = 0;
            break;
    }

    global.b16BitCode = FALSE;
    return;
}

//---------------------------------------------------------------------------

/*
    This call is used as a direct thunk from the 2D display driver to
    the 3D DirectX driver. The Win16 Mutex is not released. You can't
    make system calls from within this call.  However, there are certain
    important events that need to be sent over here as notifications,
    that are particularly important, so this interface was created.

    Arguments:  eax         flat 32 pDXShare
                ebx         purpose of call.
                            One of the GDIDX_XXX equates in dxshare.h
                ecx         Other context data which depends upon ebx.
*/

void __declspec(naked) __stdcall DrvInterfaceFunc(void)
{
    __asm
    {
    mov     pDXShare,eax
    push    ecx
    push    ebx
    call    DrvInterfaceFuncC
    retf
    }
}

/*
    This call is used as an interface back to the display driver
    from DirectX. However!! It should only be used to call back to
    the display driver, if we are in the DirectX driver because the
    display driver called it though the DrvInterfaceFunc function
    above. Basically you can only use this if you are running
    under the 16bit display driver context.

    Arguments:
    I set this function up to have parameters similar to the
    Display Driver escape call. There is a function select
    parameter which tells what the specific callback is. Then
    there is an lpIn ptr giving input parameters to the callback
    and an lpOut ptr to return things in.

    Returns:
    This routine returns TRUE if the dwFunction was recognized. The
    return values for specific functions are given in the structure
    pointed to by lpOut.
    This routine returns FALSE if the dwFunction was not recognized.

    The callbacks supported are given in drivers\common\win9x\inc\dxshare.h
*/
int DrvCallbackFunc(DWORD dwFunction, void *pIn, void *pOut)
{
    int     iRetVal;
    DWORD   dwCallSelector, dwCallOffset, pDispDriverHWLogDevCX;

    dwCallOffset          = pDXShare->dwDrvCallbackOffset;
    dwCallSelector        = pDXShare->dwDrvCallbackSelector;
    pDispDriverHWLogDevCX = pDXShare->pDispDriverHWLogDevCX;

    __asm
    {
        push    esi
        push    edi
        push    ebx

        // Flip the order of the parameters around since the
        // display driver uses PASCAL calling conventions.
        push    pDispDriverHWLogDevCX
        push    dwFunction
        push    pIn
        push    pOut

        ; push the return address
        mov     ax,cs
        and     eax,0FFFFH
        push    eax
        lea     eax,ComeBack
        push    eax

        ; Push the call address
        push    dwCallSelector
        push    dwCallOffset

        ; "Call" the 16bit side by doing a retf
        retf

ComeBack:
        mov iRetVal,eax

        ; The caller already popped the parameters
        ; Just pop the saved registers
        pop ebx
        pop edi
        pop esi
    }

    return  (iRetVal);
}

//---------------------------------------------------------------------------

/*
    GetModePitch ()

    This routine figurs out the pitch of a given mode. Since there might
    be tiling, the RM may have to get involved.
*/

DWORD __stdcall GetModePitch (int nXRes, int nYRes, int nBitsPerPixel)
{
    DWORD   dwPrimaryDisplayPitchInBytes, dwRoundValue;
    NV_CFGEX_GET_SURFACE_PITCH_PARAMS params;

    dbgTracePush ("GetModePitch");

    params.Width = nXRes;
    params.Depth = nBitsPerPixel;

    // Get the pitch from the RM for a given width/bpp
    if (NvRmConfigGetEx(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                        NV_CFGEX_GET_SURFACE_PITCH,
                        &params, sizeof(params)) == NVOS_CGE_STATUS_SUCCESS) {
        dbgTracePop();
        return params.Pitch;
    }

    //
    // Couldn't get the value from the RM, so we'll make a best effort
    // attempt at determining what the pitch likely should be.
    //
    dwPrimaryDisplayPitchInBytes = (nXRes * nBitsPerPixel) >> 3;
    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
        dwRoundValue = 0x3F;
    }
    else {
        dwRoundValue = 0x1F;
    }

    dwPrimaryDisplayPitchInBytes += dwRoundValue;
    dwPrimaryDisplayPitchInBytes &= (~dwRoundValue);

    dbgTracePop();
    return (dwPrimaryDisplayPitchInBytes);
}

//---------------------------------------------------------------------------

/*
    SortDDHalModeList ()

    This routine sorts the DDHalMode list so that modes are sorted
    first by color depth and then by XResolution. It returns the
    number of modes in the table.
*/
int __stdcall SortDDHalModeList ()
{
    DDHALMODEINFO   *p1, *p2, sTempMode;
    int             nSwap;

    dbgTracePush ("SortDDHalModeList");

    // OK, let's bubble sort the mode list
    for (p1 = pDriverData->ModeList; p1[1].dwWidth != 0; p1++)
    {
        for (p2 = p1 + 1; p2->dwWidth != 0; p2++)
        {
            // Assume correct order
            nSwap = 0;

            // Primary sort is on pixel depth
            if  (p1->dwBPP > p2->dwBPP)
                nSwap = 1;
            else
            {
                // Check if pixel depths equal
                if  (p1->dwBPP == p2->dwBPP)
                {
                    // pixel depths are equal -- sort on width
                    if  (p1->dwWidth > p2->dwWidth)
                        nSwap = 1;
                    else
                    {
                        // check if widths are equal
                        if  (p1->dwWidth == p2->dwWidth)
                        {
                            // widths equal -- sort on height
                            if  (p1->dwHeight > p2->dwHeight)
                                nSwap = 1;
                        }
                    }

                }
            }

            if  (nSwap)
            {
                sTempMode = *p1;
                *p1 = *p2;
                *p2 = sTempMode;
            }
        }
    }

    dbgTracePop();
    return ((p1 - pDriverData->ModeList) + 1);
}

//---------------------------------------------------------------------------

/*
    AddModeToTable ()

    This routine adds the mode to the mode table.
*/
void __stdcall AddModeToTable (DDHALMODEINFO *pMode, int nXRes, int nYRes, int nBitsPerPixel, int refresh)
{
    dbgTracePush ("AddModeToTable");

    // Add an entry to the ModeList
    // Special case to add a termination mode.
    if  (nXRes == 0)
    {
        pMode->dwWidth          = 0;
        pMode->dwHeight         = 0;
        pMode->lPitch           = 0;
        pMode->dwBPP            = 0;
        pMode->wRefreshRate     = 0;
        pMode->wFlags           = 0;
        pMode->dwRBitMask       = 0;
        pMode->dwGBitMask       = 0;
        pMode->dwBBitMask       = 0;
        pMode->dwAlphaBitMask   = 0;
        dbgTracePop();
        return;
    }

    pMode->dwWidth  = (DWORD) nXRes;
    pMode->dwHeight = (DWORD) nYRes;
    pMode->lPitch   = (LONG) GetModePitch(nXRes, nYRes, nBitsPerPixel);
    pMode->dwBPP    = (DWORD) nBitsPerPixel;
    pMode->wRefreshRate = refresh;

    if  (nBitsPerPixel == 8)
    {
        pMode->wFlags           = DDMODEINFO_PALETTIZED;
        pMode->dwRBitMask       = 0;
        pMode->dwGBitMask       = 0;
        pMode->dwBBitMask       = 0;
        pMode->dwAlphaBitMask   = 0;
    }

    if  (nBitsPerPixel == 16)
    {
        pMode->wFlags           = 0;
        pMode->dwRBitMask       = 0x0000F800;
        pMode->dwGBitMask       = 0x000007E0;
        pMode->dwBBitMask       = 0x0000001F;
        pMode->dwAlphaBitMask   = 0x00000000;
    }

    if  (nBitsPerPixel == 32)
    {
        pMode->wFlags           = 0;
        pMode->dwRBitMask       = 0x00FF0000;
        pMode->dwGBitMask       = 0x0000FF00;
        pMode->dwBBitMask       = 0x000000FF;
#ifdef SPOOF_ALPHA
        pMode->dwAlphaBitMask   = 0xFF000000;
#else
        pMode->dwAlphaBitMask   = 0x00000000;
#endif
    }

    dbgTracePop();
    return;
}

//---------------------------------------------------------------------------

/*
    BuildDefaultDDHalModeTable ()

    This routine builds a default table.
*/
void __stdcall BuildDefaultDDHalModeTable ()
{
    DDHALMODEINFO *pMode;

    dbgTracePush ("BuildDefaultDDHalModeTable");

    // Start out with a mode ptr
    pMode = pDriverData->ModeList;

    // These modes MUST be valid modes!
    AddModeToTable (pMode++, 640, 480, 8, 0);
    AddModeToTable (pMode++, 640, 480, 16, 0);
    AddModeToTable (pMode++, 640, 480, 32, 0);
    AddModeToTable (pMode++, 800, 600, 8, 0);
    AddModeToTable (pMode++, 800, 600, 16, 0);
    AddModeToTable (pMode++, 800, 600, 32, 0);
    AddModeToTable (pMode++, 1024, 768, 8, 0);
    AddModeToTable (pMode++, 1024, 768, 16, 0);
    AddModeToTable (pMode++, 1024, 768, 32, 0);

    // Sort the list
    SortDDHalModeList();

    // Record the number of modes in the table
    GET_HALINFO()->dwNumModes = pMode - pDriverData->ModeList;

    dbgTracePop();
    return;
}
#endif  // !WINNT

//---------------------------------------------------------------------------

/*
    BuildDDHalModeTable ()

    This routine calls the display driver to get the list of destop
    modes and also adds in a bunch of DDraw specific modes. It filters
    each mode based upon:
    1) The amount of free contiguous video memory starting from offset 0
       and the pitch of a mode determining the maximum number of bytes
       that can be consumed by the primary surface.
    2) The DAC speed (determining maximum pixel rate)
    3) The display type (NTSC, PAL, FLATPANEL 800,1024,1280, VGA MONITOR)
*/
void __stdcall BuildDDHalModeTable ()
{
#ifndef WINNT   // WINNT BUG Is there a SetMode callback under NT?
    DDHALMODEINFO   *pMode;
    MODEENTRY       sModeDesc[MAX_MODEENTRYS];
    int             i;
    DWORD           dwModeEntryType;
    BOOL            doRestrictTVModes;

    dbgTracePush ("BuildDDHalModeTable");

    // Call back to display driver to get the mode list from the registry
    // - zero out the mode list so that we have at least one record at the end
    //   of the list with dwXRes == 0. this allows the for loop that adds the
    //   basic mode table to terminate properly.
    memset (sModeDesc,0,sizeof(sModeDesc)); // zero out
    dwModeEntryType = MODETYPE_MODEENTRY;
    if  (!MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_GET_REGISTRY_MODE_LIST, sizeof(DWORD), (LPCSTR)&dwModeEntryType,
                MAX_MODEENTRYS * sizeof(MODEENTRY), (char *) sModeDesc))
    {
        BuildDefaultDDHalModeTable ();
        dbgTracePop();
        return;
    }

    // Start off pointing to the ModeList
    pMode = pDriverData->ModeList;
    memset(pMode, 0, MAX_MODEENTRYS * sizeof(DDHALMODEINFO));

    // Make sure TwinView state is current
    ResetTwinViewState (pDriverData);

    // find out whether we need to restrict the modes to ones which will work on a TV
    doRestrictTVModes = FALSE;
    for (i = 0; i < (int)pDriverData->dwHeads; i++) {
        NVTWINVIEWDATA twinData = {0};
        DWORD          retVal;
        DWORD          dwCrtcNumber;
        DWORD          dwCompare;
        DWORD          dwHead;

        switch (pDriverData->dwDesktopState) {
        case NVTWINVIEW_STATE_DUALVIEW:
            dwCrtcNumber = 0;       // need to use logical head numbers in multimon mode
            dwHead = i;
            dwCompare = pDXShare->dwHeadNumber;
            break;
        case NVTWINVIEW_STATE_CLONE:
            dwCrtcNumber = i;
            dwHead = pDXShare->dwHeadNumber;
            dwCompare = i;
            break;
        default:
            // single mode
            dwCrtcNumber = 0;
            dwHead = pDXShare->dwHeadNumber;
            dwCompare = 0;
            break;
        }

        twinData.dwFlag   = NVTWINVIEW_FLAG_GET;
        twinData.dwAction = NVTWINVIEW_ACTION_SETGET_DEVICE_TYPE;
        twinData.dwCrtc   = dwCrtcNumber;

        retVal = MyExtEscape(dwHead, NV_ESC_SETGET_TWINVIEW_DATA, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPCSTR) &twinData, sizeof(NV_ESC_SETGET_TWINVIEW_DATA), (LPSTR) &twinData);
        if (twinData.dwSuccess && retVal &&
           (twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceType == MONITOR_TYPE_NTSC ||
            twinData.NVTWINVIEWUNION.nvtwdevdata[0].dwDeviceType == MONITOR_TYPE_PAL)) {
            if (i == (int)dwCompare) {
                doRestrictTVModes = TRUE;
            }
        }
    }

    // Now we need to take the basic mode list we got back from the
    // display driver, convert it, and stick those modes in the Ddraw
    // table.
    if (doRestrictTVModes) {
        DWORD dwEncoderType;
        BOOL allow720, allow800, allow1024, allow800orHigher;
        // determine on a case by case basis which modes are allowed
        NvRmConfigGet (pDriverData->dwRootHandle, pDriverData->dwDeviceHandle,
                       NV_CFG_VIDEO_ENCODER_TYPE, &dwEncoderType);

        switch (dwEncoderType) {
        case NV_ENCODER_BROOKTREE_868:
        case NV_ENCODER_BROOKTREE_869:
            allow720 = TRUE;
            allow800 = TRUE;
            allow1024 = FALSE;
            break;
            
        case NV_ENCODER_CHRONTEL_7007:
        case NV_ENCODER_CHRONTEL_7008:
            allow720 = TRUE;
            allow800 = TRUE;
            allow1024 = FALSE;
            break;

        case NV_ENCODER_CHRONTEL_7003:
        case NV_ENCODER_CHRONTEL_7004:
        case NV_ENCODER_CHRONTEL_7005:
        case NV_ENCODER_CHRONTEL_7006:
        case NV_ENCODER_PHILIPS_7102:
        case NV_ENCODER_PHILIPS_7103:
        case NV_ENCODER_PHILIPS_7108:
        case NV_ENCODER_PHILIPS_7109:
        case NV_ENCODER_PHILIPS_7108B:
            allow720 = FALSE;
            allow800 = TRUE;
            allow1024 = FALSE;
            break;

        case NV_ENCODER_BROOKTREE_871:
            allow720 = FALSE;
            allow800 = TRUE;
            allow1024 = TRUE;
            break;

        default:
            allow720 = FALSE;
            allow800 = TRUE;
            allow1024 = TRUE;
            break;
        }

        allow800orHigher = TRUE;    // Toshiba special request

        // no refresh rate enumeration for TV modes
        for (i = 0; sModeDesc[i].dwXRes != 0; i++) {
            if (sModeDesc[i].dwRefreshRate == 60 &&
               (sModeDesc[i].dwXRes == 320 ||
                sModeDesc[i].dwXRes == 640 ||
               (sModeDesc[i].dwXRes == 720 && allow720) ||
               (sModeDesc[i].dwXRes == 800 && allow800) ||
               (sModeDesc[i].dwXRes >= 800 && allow800orHigher) ||  // for Toshiba
               (sModeDesc[i].dwXRes == 1024 && allow1024) ||
               (sModeDesc[i].dwXRes >= 800 && pDriverData->dwDesktopState == NVTWINVIEW_STATE_CLONE))) {
                AddModeToTable (pMode++, sModeDesc[i].dwXRes, sModeDesc[i].dwYRes, sModeDesc[i].dwBpp, 0);
            }
        }
    } else {
        DWORD dwLastXRes = 0, dwLastYRes = 0;
        if (pDriverData->regEnumDDRefresh == NV4_REG_ENUM_REFRESH_DISABLE) {
            for (i = 0; sModeDesc[i].dwXRes != 0; i++) {
                if (sModeDesc[i].dwXRes != dwLastXRes || sModeDesc[i].dwYRes != dwLastYRes) {
                    AddModeToTable (pMode++, sModeDesc[i].dwXRes, sModeDesc[i].dwYRes, sModeDesc[i].dwBpp, 0);
                    dwLastXRes = sModeDesc[i].dwXRes;
                    dwLastYRes = sModeDesc[i].dwYRes;
                }
            }
        } else if (pDriverData->regEnumDDRefresh == NV4_REG_ENUM_REFRESH_ENABLEBIG) {
            for (i = 0; sModeDesc[i].dwXRes != 0; i++) {
                if (sModeDesc[i].dwXRes >= 640 && sModeDesc[i].dwYRes >= 480) {
                    AddModeToTable (pMode++, sModeDesc[i].dwXRes, sModeDesc[i].dwYRes, sModeDesc[i].dwBpp, sModeDesc[i].dwRefreshRate);
                } else {
                    if (sModeDesc[i].dwXRes != dwLastXRes || sModeDesc[i].dwYRes != dwLastYRes) {
                        AddModeToTable (pMode++, sModeDesc[i].dwXRes, sModeDesc[i].dwYRes, sModeDesc[i].dwBpp, 0);
                        dwLastXRes = sModeDesc[i].dwXRes;
                        dwLastYRes = sModeDesc[i].dwYRes;
                    }
                }
            }
        } else if (pDriverData->regEnumDDRefresh == NV4_REG_ENUM_REFRESH_ENABLEALL) {
            for (i = 0; sModeDesc[i].dwXRes != 0; i++) {
                AddModeToTable (pMode++, sModeDesc[i].dwXRes, sModeDesc[i].dwYRes, sModeDesc[i].dwBpp, sModeDesc[i].dwRefreshRate);
            }
        }
    }

#ifdef  STEREO_SUPPORT_
    // For ASUS type of stereo connection we want to fool the DX and force it to reset a video mode
    // even if it matches the current desktop one. It has to be done because ASUS stereo requires
    // all stereo surfaces pairs (left/right eyes) to be adjasent in memory. And we don't want to waste
    // video memory when we're in 2D (aka desktop or GDI). Here is how we handle this. First we let
    // resman know that we enter "adjasent stereo surface allocation mode". Then we fool DX by building
    // a dummy mode entry that matches the current desktop. Basically for the current desktop mode we
    // will have 2 identical entries but the dummy one will be located behind the real one so DX will
    // never see it. By returning the dummy index in the dwModeIndex we will force DX to reset the video
    // mode. Bingo!!! Then resman will take care of the rest and create an adjacent stereo surface.
    if (STEREO_ENABLED)
    {
        DIBENGINEHDR1 *pDibeng = (DIBENGINEHDR1 *)pDXShare->pFlatDibeng;
        AddModeToTable (pMode++, pDibeng->deWidth, pDibeng->deHeight, pDibeng->deBitsPixel, 0x1234);
    }
#endif  //STEREO_SUPPORT

    // set up pointer to DDHALINFO
    LPDDHALINFO pHalInfo = (LPDDHALINFO)pDXShare->pHalInfo;

    // Setup the number of modes counter
    pHalInfo->dwNumModes = pMode - pDriverData->ModeList;

    // Sanity check
    if  (pHalInfo->dwNumModes == 0)
    {
        BuildDefaultDDHalModeTable ();
        dbgTracePop();
        return;
    }

    // Sort the mode list appropriately
    SortDDHalModeList ();

    dbgTracePop();
#endif // !WINNT
    return;
}

//---------------------------------------------------------------------------

#ifndef WINNT
int getRootIndex(DWORD dwAdapterIndex)
{
    dbgTracePush ("getRootIndex");

    nvAssert(dwAdapterIndex < MAX_ADAPTERS);

    DISPDRVDIRECTXCOMMON *pDXShare = g_adapterData[dwAdapterIndex].pDXShare;
    if (!pDXShare) {
        // not a valid index
        dbgTracePop();
        return -1;
    }

    DWORD dwRootHandle = pDXShare->dwRootHandle;

    for (DWORD i = 0; i < dwAdapterIndex; i++) {

        if (NULL == g_adapterData[i].pDXShare)
            continue;
        if (g_adapterData[i].pDXShare->dwRootHandle == dwRootHandle) {
            // found the root device for this adapter
            dbgTracePop();
            return i;
        }

    }

    dbgTracePop();
    return -1;
}

//---------------------------------------------------------------------------

DISPDRVDIRECTXCOMMON* getDXShare(DWORD dwRootHandle, DWORD dwHeadNumber)
{
   dbgTracePush ("getDXShareForHead");

   for (DWORD i = 0; i < MAX_ADAPTERS; i++) {

        DISPDRVDIRECTXCOMMON* pDXShare = g_adapterData[i].pDXShare;
        if (pDXShare
         && (pDXShare->dwRootHandle == dwRootHandle)
         && (pDXShare->dwHeadNumber == dwHeadNumber)) {
            // found the root device for this adapter
            dbgTracePop();
            return pDXShare;
        }

    }

    dbgTracePop();
    return NULL;
}

//---------------------------------------------------------------------------

int getAdapterIndex(DISPDRVDIRECTXCOMMON *pNewDXShare, DWORD *pdwRootIndex)
{
    dbgTracePush ("getAdapterIndex");

    nvAssert(pNewDXShare);

    for(DWORD i = 0; i < MAX_ADAPTERS; i++) {

        DISPDRVDIRECTXCOMMON *pDXShare = g_adapterData[i].pDXShare;

        if (pDXShare == pNewDXShare) {
            // found an entry, get the root index
            *pdwRootIndex = getRootIndex(i);
            dbgTracePop();
            return i;
        }

    }

    // the adapter table is full
    nvAssert(0);

    *pdwRootIndex = -1;
    dbgTracePop();
    return -1;
}
#endif

//---------------------------------------------------------------------------

/*
 * DriverInit
 *
 * this is the entry point called by DirectDraw to
 * initialize the 32-bit driver.
 *
 */
DWORD __stdcall DriverInit(DWORD dwDriverData)
{
    char regStr[256];
    HKEY  hKey;

    dbgResetTraceLevel ();
    dbgTracePush ("DriverInit");

#ifdef WINNT

    NvAcquireSemaphore((CDriverContext *) ((PDEV*)dwDriverData)->pDriverData);
    ppdev = (PDEV*)dwDriverData;
    pDriverData = (CDriverContext *)ppdev->pDriverData;
    DWORD dwAdapterIndex = ppdev->ulDeviceReference;

    BOOL bFirstTime = TRUE;

#else // !WINNT

    // The context data is always the PDevice
    pDXShare = (DISPDRVDIRECTXCOMMON *) dwDriverData;

    // get an hDC if we don't already have one
    if (!nvFillAdapterTable()) {
        // return if device is disabled
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return 0;
    }

    // add a new entry, or get an existing entry from the adapter table
    DWORD dwAdapterIndex, dwRootIndex;
    dwAdapterIndex = getAdapterIndex(pDXShare, &dwRootIndex);

    if (dwRootIndex == -1) {
        pDriverData = (CDriverContext *)g_adapterData[dwAdapterIndex].pDriverData;
    } else {
        // this is a secondary (or later) unit on a mult-unit card
        pDriverData = (CDriverContext *)g_adapterData[dwRootIndex].pDriverData;
    }

    // Give the HW-specific portion of the PDevice the flat selectors.
    pDXShare->dwFlatCodeSel = (unsigned long) GetFlatCodeSel();
    pDXShare->dwFlatDataSel = (unsigned long) GetFlatDataSel();
    pDXShare->pDirectXData  = (DWORD)pDriverData;

    if (!pDXShare->pHalInfo) {
        // allocate a DDHALINFO structure for this unit
        pDXShare->pHalInfo = (unsigned long)AllocIPM(sizeof(DDHALINFO));
        if (!pDXShare->pHalInfo) {
            // out of memory
            DPF("DriverInit: out of memory allocating DDHALINFO");
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return 0;
        }
    }

    BOOL bFirstTime = FALSE;

    // Allocate a DriverData structure if there isn't already one
    if (!pDriverData) {
        bFirstTime = TRUE;
        if (!nvAllocDriverStructures()) {
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return 0;
        }
    }

#endif // !WINNT

#ifdef NVSTATDRIVER
    AttachNVStat();
#endif

    if (bFirstTime)
    {
        if (!FirstTimeInit()) {
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return 0;
        }
        // add the pDriverData entry for this device to the global adapter table
        // always associate the pDriverData with the the first entry for this card
#ifdef WINNT
        g_adapterData[dwAdapterIndex].pDriverData = pDriverData;
#else
        if (dwRootIndex == -1) {
            g_adapterData[dwAdapterIndex].pDriverData = pDriverData;
        } else {
            g_adapterData[dwRootIndex].pDriverData = pDriverData;
        }
#endif
    }

    // Get the current registry settings from the hardware specific routine.
    D3DReadRegistry (pDriverData);

#ifdef TEXFORMAT_CRD
#ifndef WINNT
    // Attempt to get the RM derived AGP limit from the RM
    NvRmConfigGet (pDriverData->dwRootHandle,
                   NV_WIN_DEVICE,
                   NV_CFG_AGP_LIMIT,
                   &pDriverData->regMaxAGPLimit);

    // detect installed HW and determine how we are going to operate
    // Under WINNT, we need this info much earlier in the DX initialization
    // sequence (at DrvGetDirectDrawInfo time). So we have already called
    // nvDeterminePerformanceStrategy from the GDI driver at DrvEnablePDEV time.
    nvDeterminePerformanceStrategy (pDriverData);
#endif // !WINNT
#endif // TEXFORMAT_CRD

    // start capture stuff
#ifdef CAPTURE
    if (bFirstTime) {
        captureStartup (getDC()->nvD3DRegistryData.regszCaptureRecordPath,
                        getDC()->nvD3DRegistryData.regszCaptureRecordFilename,
                        getDC()->nvD3DRegistryData.regCaptureRecordFilenum);
        capturePlayStartup (getDC()->nvD3DRegistryData.regszCapturePlayPath,
                            getDC()->nvD3DRegistryData.regszCapturePlayFilename,
                            getDC()->nvD3DRegistryData.regCapturePlayFilenum);
    }
    else {
        captureSetParams (getDC()->nvD3DRegistryData.regszCaptureRecordPath,
                          getDC()->nvD3DRegistryData.regszCaptureRecordFilename,
                          getDC()->nvD3DRegistryData.regCaptureRecordFilenum);
        capturePlaySetParams (getDC()->nvD3DRegistryData.regszCapturePlayPath,
                              getDC()->nvD3DRegistryData.regszCapturePlayFilename,
                              getDC()->nvD3DRegistryData.regCapturePlayFilenum);
    }
#endif  // CAPTURE

    if (bFirstTime)
    {
#ifdef TEXFORMAT_CRD
        if (!nvEnumerateFourCCs()) {
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return 0;
        }
        if (!nvEnumerateSurfaceFormats()) {
            NvReleaseSemaphore(pDriverData);
            dbgTracePop();
            return 0;
        }
#endif  // TEXFORMAT_CRD
    }

#ifdef WINNT
    //***************************************************************
    // Shared variable for active channel id is stored in NT display
    // driver state.
    //***************************************************************
    pDriverData->pCurrentChannelID = (ULONG) &(ppdev->oglLastChannel);
#else // !WINNT

    // Have to get the DC first before we can isue this call. Hence,
    // I can't do it in FirstTimeInit.
    MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_GETACTIVECHANNELPTR, 0, 0, 4, (char *) &(pDriverData->pCurrentChannelID));
    // Allocate 16 bit selectors we will need and assign them if it hasn't
    // already been done (the routine protects itself from doing twice.)
    if (!GetSelectorsFromDisplayDriver()) {
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return 0;
    }

    // Do this every time we come into DriverInit -- just in case
    // KillHelp is run. DllMain will clear out the ptr when the DirectX
    // driver is punted out of memory. Here we need to re-establish it.
    pDXShare->pfnInterfaceFunc = (unsigned long) DrvInterfaceFunc;

    {
        DCICMD sMyDciCmd;

        // Adding new escape to display driver to tell them that a DX
        // app is being started.
        sMyDciCmd.dwVersion = DD_VERSION;
        sMyDciCmd.dwCommand = DDNVCREATEDRIVEROBJECT;
        MyExtEscape(pDXShare->dwHeadNumber, DCICOMMAND, sizeof(DCICMD), (const char *)&sMyDciCmd, 0, 0);
    }
#endif // !WINNT

    // set up pointer to DDHALINFO
    LPDDHALINFO pHalInfo = GET_HALINFO();

    // if this is the second driverinit for the second head, kick out
    if (pDriverData->dwDrvRefCount++ > 0) {

        buildDDHALInfo32(pDriverData);
        nvSetHardwareCaps();

#ifdef WINNT
        pHalInfo->lpD3DGlobalDriverData = (LPVOID)&(getDC()->D3DGlobalDriverData);
        D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA) pHalInfo->lpD3DGlobalDriverData,
                           (LPD3DHAL_CALLBACKS*)&pHalInfo->lpD3DHALCallbacks,
                           (LPDDHAL_DDEXEBUFCALLBACKS *)&pHalInfo->lpD3DBufCallbacks,
                           pDriverData);
#else
        pHalInfo->lpD3DGlobalDriverData = (ULONG_PTR)&(getDC()->D3DGlobalDriverData);
        //they just HAD to rename the structure members didn't they....
        D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA) pHalInfo->lpD3DGlobalDriverData,
                           (LPD3DHAL_CALLBACKS*)&pHalInfo->lpD3DHALCallbacks,
                           (LPDDHAL_DDEXEBUFCALLBACKS *)&pHalInfo->lpDDExeBufCallbacks,
                           pDriverData);
#endif // !WINNT

        BuildDDHalModeTable ();
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return (DWORD)dwDriverData;

    }

#ifndef WINNT
    // grab north bridge ID
    DWORD nbID;
    _asm {
            mov edx, 0cf8h
            mov eax, 80000000h
            out dx,eax
            mov edx, 0cfch
            in  eax, dx
            mov nbID, eax
    }
    pDriverData->northBridgeID = nbID;
#else
    pDriverData->northBridgeID = 0;
#endif

    pDriverData->dwCurrentBlit   = 0;
    CURRENT_FLIP   = 0;
    COMPLETED_FLIP = 0;

    getDC()->dwAAContextCount = 0;
    pDriverData->dwRingZeroMutex = FALSE;

    pDriverData->blitCalled = FALSE;

#ifndef WINNT
    pDriverData->HALCallbacks.lpSetInfo = 0;
#endif // !WINNT

#ifndef TEXFORMAT_CRD  // moved up
#ifndef WINNT
    // Attempt to get the RM derived AGP limit from the RM
    NvRmConfigGet (pDriverData->dwRootHandle,
                   NV_WIN_DEVICE,
                   NV_CFG_AGP_LIMIT,
                   &pDriverData->regMaxAGPLimit);

    // detect installed HW and determine how we are going to operate
    // this effects the following test for pushbuffer size
    // Under WINNT, we need this info much earlier in the DX initialization
    // sequence (at DrvGetDirectDrawInfo time). So we have already
    // called nvDeterminePerformanceStrategy from the GDI driver
    // at DrvEnablePDEV time.
    nvDeterminePerformanceStrategy(pDriverData);
#endif // !WINNT
#endif // !TEXFORMAT_CRD

    // Construct the VPP object (must be after nvDeterminePerformanceStrategy)
    //   There must be NO references to vpp data prior to this point
    //
#ifdef WINNT
    if (!VppConstructor(ppdev, &(pDriverData->vpp), &pDriverData->nvD3DPerfData))
#else
    if (!VppConstructor(pDriverData, &(pDriverData->vpp), &pDriverData->nvD3DPerfData))
#endif
    {
        VppDestructor(&(pDriverData->vpp)); // Release anything we might have allocated thus far
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return 0;
    }
#if IS_WINNT5
    VppReadRegistry(&pDriverData->vpp, ppdev->hDriver, ppdev->ulDeviceDisplay[0]);
#else
    VppReadRegistry(&pDriverData->vpp, 0             , pDXShare->dwHeadNumber);
#endif

    // Reset in case of a mode switch which unloaded the driver
    pHalInfo->ddCaps.dwCurrVisibleOverlays = 0;

    // Reset motion comp control
    pDriverData->dwMCNVMCSurface = 0;
    pDriverData->dwMCNVDSSurfaceBase = 0;
    pDriverData->bMCOverlaySrcIsSingleField1080i = FALSE;
    pDriverData->bMCHorizontallyDownscale1080i = FALSE;

    pDriverData->dwVideoPortsAvailable = 1;

    bltInit();

    pDriverData->DDrawVideoSurfaceCount = 0;

    #ifdef NVPE
    // set VPE initial state
    NVPInitialState(pDriverData);
    #else
    pDriverData->nvpeState.bVPEInitialized = 0;
    pDriverData->nvpeState.bFsMirrorEnabled = 0;
    #endif

    // Can't reinitialize NV here because mode change may not have happened yet.

    // Append DirectDraw subkey to the global registry base path without using C runtime lib
    nvStrCpy(regStr, NV4_REG_GLOBAL_BASE_PATH "\\" NV4_REG_DIRECT_DRAW_SUBKEY);

    // Controls how much persistent AGP is reserved (i.e. not included in DDraw's heap)
    // We keep at least 8MB and no more than 1/8th the total amount of AGP memory
    pDriverData->regPersistAGPSize = max (((pDriverData->regMaxAGPLimit + 1) >> 3), 0x800000);

    // Let any registry entries override our values; lookup happens under the Global\DirectDraw key
    if (RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, regStr, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        if (RegQueryValueEx(hKey, PERSISTENT_AGP_SIZE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS) {
            pDriverData->regPersistAGPSize = lValue;
        }
        if (RegQueryValueEx(hKey, NV_MAX_AGP_SIZE, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS) {
            pDriverData->regMaxAGPLimit = lValue - 1;   // appears we can setup 128M and alloc at least 80M

        }
        RegCloseKey(hKey);
    }

    pDriverData->dwTVTunerFlipCount = 0;
    pDriverData->dwTVTunerTimer = 0;
    pDriverData->regEnumDDRefresh = 0;
    pDriverData->regTVTunerHack = NV4_REG_VBMM_NOHACK;

    //VPE enable stuff
#ifndef NVPE
    pDriverData->bEnableVPE = 1;
#endif // NVPE

    // Append System subkey to the global registry base path without using C runtime lib
    nvStrCpy(regStr, NV4_REG_GLOBAL_BASE_PATH "\\" NV4_REG_RESOURCE_MANAGER_SUBKEY);

#ifndef WINNT
    // controls whether dma push buffer should be forced into PCI memory
    // 1 = force DMA push buffer to be created in PCI memory
    // 2 = same as '1', but also reduce AGP heap size by an amount equal to the size of the push buffer
    if (IS_VIA_MVP3_NB_ID(pDriverData->northBridgeID)) {
        // default case for the VIA 598 is to put push buffer in system memory
        pDriverData->regSuper7Compat = 1;
    } else {
        pDriverData->regSuper7Compat = 0;
    }
#else
    pDriverData->regSuper7Compat = 0;
#endif

    if (RegOpenKeyEx(NV4_REG_GLOBAL_BASE_KEY, regStr, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        hKey = 0;
    }
    if (hKey) {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        if (RegQueryValueEx(hKey, NV_REG_SYS_SUPER7_COMPATIBILITY, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS) {
            pDriverData->regSuper7Compat = lValue;
        }
        RegCloseKey(hKey);
    }

#ifndef WINNT
    // Grab the local registry path and append the System subkey to the string
    // without using the C runtime library.
    MyExtEscape (pDXShare->dwHeadNumber, NV_ESC_GET_LOCAL_REGISTRY_PATH, 0, NULL, 256, regStr);
    nvStrCat (&(regStr[4]), "\\" NV4_REG_RESOURCE_MANAGER_SUBKEY);
#endif // !WINNT

    // Controls the push buffer location
    if (pDriverData->regSuper7Compat) {
        pDriverData->regPBLocale = NV_REG_SYS_PB_LOCATION_SYSTEM;
    } else {
        pDriverData->regPBLocale = NV_REG_SYS_PB_LOCATION_DEFAULT;
    }
    if (RegOpenKeyEx((HKEY)(*((LPDWORD)&regStr[0])), &regStr[4], 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD   dwSize = sizeof(long);
        DWORD   dwType = REG_DWORD;
        long    lValue;

        if (RegQueryValueEx(hKey, NV_REG_SYS_PB_LOCATION, NULL, &dwType, (LPBYTE)&lValue, &dwSize) == ERROR_SUCCESS) {
            pDriverData->regPBLocale = lValue;
        }
        RegCloseKey(hKey);
    }

// Used for texture profiling (counting the # of texture DL's)
#ifdef CNT_TEX
    for (int iFrmCnt = 0; iFrmCnt<MAX_FRAME_CNT; iFrmCnt++) {
        g_dwVidTexDL[iFrmCnt] = 0;
        g_dwAgpTexDL[iFrmCnt] = 0;
    }
    g_dwFrames = 0;
#endif

    // the desktopstate value needs to be processed before calling buildddhalinfo32
#ifdef WINNT
    pDriverData->dwDesktopState = ppdev->TwinView_State;
#else
    NVTWINVIEWDATA twinViewData;
    memset(&twinViewData, 0, sizeof(twinViewData));
    twinViewData.dwFlag   = NVTWINVIEW_FLAG_GET;
    twinViewData.dwAction = NVTWINVIEW_ACTION_SETGET_STATE;
    MyExtEscape(pDXShare->dwHeadNumber, NV_ESC_SETGET_TWINVIEW_DATA, 0, NULL, sizeof(twinViewData), (char*)&twinViewData);
    nvAssert(twinViewData.dwSuccess);
    pDriverData->dwDesktopState = twinViewData.dwState;
#endif

    buildDDHALInfo32(pDriverData);
    BuildDDHalModeTable();
    nvSetHardwareCaps();

#ifdef WINNT
    pHalInfo->lpD3DGlobalDriverData = (LPVOID)&(getDC()->D3DGlobalDriverData);
    D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA) pHalInfo->lpD3DGlobalDriverData,
                       (LPD3DHAL_CALLBACKS*)&pHalInfo->lpD3DHALCallbacks,
                       (LPDDHAL_DDEXEBUFCALLBACKS *)&pHalInfo->lpD3DBufCallbacks,
                       pDriverData);
#else
    pHalInfo->lpD3DGlobalDriverData = (ULONG_PTR)&(getDC()->D3DGlobalDriverData);
    //they just HAD to rename the structure members didn't they....
    D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA) pHalInfo->lpD3DGlobalDriverData,
                       (LPD3DHAL_CALLBACKS*)&pHalInfo->lpD3DHALCallbacks,
                       (LPDDHAL_DDEXEBUFCALLBACKS *)&pHalInfo->lpDDExeBufCallbacks,
                       pDriverData);
#endif // !WINNT

    // process manager initialization
    pDriverData->lpProcessInfoHead = 0;

#ifdef ENABLE_VPP_DISPATCH_CODE
#ifndef WINNT
    vppInitDispatcher(&(pDriverData->vpp));
#endif // !WINNT
#endif


#ifdef KPFS
#if (NVARCH >= 0x20)
    pDriverData->nvKelvinPM.create(128);
#endif
#endif


#ifndef WINNT   // Win9x Hackery
#ifndef NVPE
    // this call is for VPE/KMVT stuff, to hook up with and initialize their storage
    if ( !HookUpVxdStorage() ) {
        // couldn't get VXD... what do we do?
        // return zero indicating that we can't do it!
        NvReleaseSemaphore(pDriverData);
        dbgTracePop();
        return 0;
    }
#endif // !NVPE
#endif // !WINNT

    pDriverData->bRing0FlippingFlag = FALSE;

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return (DWORD)dwDriverData;
} // DriverInit


//---------------------------------------------------------------------------

/*
 * Here's where we redirect all DDHAL32_VidMemAlloc/DDHAL32_VidMemFree calls to the RM.
 * We also keep a AGP_TABLE that keeps a mapping of allocations to dynamically created
 * memory handles.
 *
 */
#ifndef WINNT
#undef DDHAL32_VidMemAlloc
#undef DDHAL32_VidMemFree
FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void    EXTERN_DDAPI DDHAL32_VidMemFree  (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);
#endif

typedef struct _AGP_TABLE
{
    FLATPTR             addr;
    FLATPTR             physaddr;
    DWORD               offset;
    DWORD               limit;
    DWORD               handle;
    struct _AGP_TABLE   *next;
} AGP_TABLE;

static AGP_TABLE *lpAGPTable = NULL;
static AGP_TABLE *lastOffset = NULL;

#ifndef WINNT

U032 __stdcall SetSharedFSDOSBoxPtr (U032 *pData)
{
    HANDLE  dwVXDHandle;
    U032    dwError;
    U032    outputSize = 0;

    dbgTracePush ("SetSharedFSDOSBoxPtr");

    if (pDriverData->thisDeviceID > 1) {
        // There is never a DOS box on the secondary device
        dbgTracePop();
        return (FALSE);
    }

    dwVXDHandle = NvCreateFile(NVX_PRIMARY_MINIVDD, 0,0,0, CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

    if ((dwVXDHandle == NULL) || (dwVXDHandle == INVALID_HANDLE_VALUE)) {
        DPF("\r\nCan't get Vxd handle   :-(");
        dwError = GetLastError();
        DPF("The error was %d (0x%x)", dwError, dwError);
        if (dwError == 50) {
            DPF("Which means most likely that you don't have a version of NVMINI.VXD which supports IOCTL");
        }
        dbgTracePop();
        return (FALSE);
    }

    DeviceIoControl(dwVXDHandle,
                    0x20000700,
                    pData,
                    4,
                    NULL,
                    0,
                    &outputSize,
                    NULL);

    NvCloseHandle(dwVXDHandle);

    dbgTracePop();
    return (TRUE);
}

//---------------------------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    static DWORD dwAttachCount = 0;

    dbgTracePush ("DllMain");

    switch ( dwReason ) {

        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls( hModule );
#ifdef  NVD3D
            if (dwAttachCount == 0) {
                init_globals();
            }
#endif  // NVD3D
            dwAttachCount++;
            InitIPM();
            AboutToBeLoaded();
            break;

        case DLL_PROCESS_DETACH:
            dwAttachCount--;
#ifdef  NVD3D
            if (dwAttachCount == 0) {
                SetSharedFSDOSBoxPtr (NULL);
                AboutToBeUnloaded();
            }
#endif  // NVD3D
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        default:
            break;
    }

    dbgTracePop();
    return (TRUE);

} // DllMain

//---------------------------------------------------------------------------

// these old entry points never were defined on NT and never will be

/*
 * D3D DX5 Draw Primitive entry points
 */
DWORD __stdcall DrawOnePrimitive32 (LPD3DHAL_DRAWONEPRIMITIVEDATA pdopd)
{
    dbgTracePush ("DrawOnePrimitive32");

    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pdopd);

    nvSetDriverDataPtrFromContext (pContext);

    nvSetD3DSurfaceState (pContext);

    if (global.dwDXRuntimeVersion < 0x700) {
        nvPusherSignalWaitForFlip (pContext->pRenderTarget->getfpVidMem(), DDSCAPS_PRIMARYSURFACE);
    }

    DWORD dwRV = nvDrawOnePrimitive(pdopd);

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return(dwRV);
}

//---------------------------------------------------------------------------

DWORD __stdcall DrawOneIndexedPrimitive32 (LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA pdoipd)
{
    dbgTracePush ("DrawOneIndexedPrimitive32");

    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pdoipd);

    nvSetDriverDataPtrFromContext (pContext);

    nvSetD3DSurfaceState (pContext);

    if (global.dwDXRuntimeVersion < 0x700) {
        nvPusherSignalWaitForFlip (pContext->pRenderTarget->getfpVidMem(), DDSCAPS_PRIMARYSURFACE);
    }

    DWORD dwRV = nvDrawOneIndexedPrimitive(pdoipd);

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return(dwRV);
}

//---------------------------------------------------------------------------

DWORD __stdcall DrawPrimitives32 (LPD3DHAL_DRAWPRIMITIVESDATA pdpd)
{
    dbgTracePush ("DrawPrimitives32");

    nvAssert (global.dwDXRuntimeVersion < 0x0700);

    // get the context
    PNVD3DCONTEXT pContext;
    NV_SET_CONTEXT (pContext, pdpd);

    nvSetDriverDataPtrFromContext (pContext);

    nvSetD3DSurfaceState (pContext);

    if (global.dwDXRuntimeVersion < 0x700) {
        nvPusherSignalWaitForFlip (pContext->pRenderTarget->getfpVidMem(), DDSCAPS_PRIMARYSURFACE);
    }

    DWORD dwRV = nvDrawPrimitives(pdpd);

    NvReleaseSemaphore(pDriverData);
    dbgTracePop();
    return(dwRV);
}

#endif // !WINNT

//---------------------------------------------------------------------------

// prepare for an impending mode switch under win9x.
// note this is 16-bit code. system calls and stuff are disallowed!

DWORD __stdcall nvModeSetPrepare16 (void)
{
#ifdef TEX_MANAGE
    nvTexManageEvictAll (NULL);
#endif
    return (DD_OK);
}

//---------------------------------------------------------------------------

// cleanup subsequent to a mode switch under win9x.
// note this is 16-bit code. system calls and stuff are disallowed!

DWORD __stdcall nvModeSetCleanup16 (void)
{
    // empty for now.
    return (DD_OK);
}

//---------------------------------------------------------------------------

#ifdef WINNT

//
// Win2K D3DHALCreateDriver entry point
//
BOOL __stdcall NvWin2KD3DHALCreateDriver(DD_HALINFO* pHalInfo, PDEV* ppdev)
{
    GLOBALDATA* pDriverData = ppdev->pDriverData;
    pHalInfo->lpD3DGlobalDriverData = (void*)&(getDC()->D3DGlobalDriverData);
    return D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA)pHalInfo->lpD3DGlobalDriverData,
                              (LPD3DHAL_CALLBACKS *)&pHalInfo->lpD3DHALCallbacks,
                              (PDD_D3DBUFCALLBACKS *)&pHalInfo->lpD3DBufCallbacks,
                              pDriverData);
}

//---------------------------------------------------------------------------

BOOL __stdcall NvWin2KAllocDriverData(PDEV* ppdev)
{
    nvAssert(ppdev);

    CDriverContext *pDriverContext = (CDriverContext*)g_adapterData[ppdev->ulDeviceReference].pDriverData;

    if (pDriverContext) {

        // return the pDriverData we already have
        ppdev->pDriverData = pDriverContext;

        return FALSE;
    }

    // set up local context
    //   this context has the same scope as pDriverData but it only defined for nvdd32.dll
    //   we also make certain it is aligned to a 32 byte boundary (rm gives us page aligned)
    //   rm requires size to be a page-size multiple

    // new operator is used to init it properly
    pDriverContext = new CDriverContext;

    if (!pDriverContext)
    {
        nvAssert(FALSE);
        return TRUE;
    }

    pDriverContext->create();

    //
    // Associate the DX GLOBALDATA with the GDI PDEV.
    //
    ppdev->pDriverData = pDriverContext;

    return FALSE;
}

//---------------------------------------------------------------------------

//
// Win2K routine to initialize all DX related global data structures.
//
void __stdcall NvWin2KInitDXGlobals()
{
    init_globals();
    InitIPM();
    return;
}

//---------------------------------------------------------------------------

DWORD __stdcall NvWin2KGetAgpLimit(PDEV *ppdev, DWORD dwRootHandle, DWORD dwDevID)
{
    DWORD dwAgpLimit, dwValue;

    if (!nvReadRegistryDWORD(ppdev, "MAXAGPHEAPSIZE", &dwValue))
    {
#ifdef NV_AGP
        dwValue = 128; // default heap size is 128MB
#else // !NV_AGP
        dwValue = 32; // default heap size is 32MB
#endif // !NV_AGP
    }

#ifndef NV_AGP
    // the heap size may not be more than 1/8 of the system memory size (win2k kernel issue)
    dwValue = min(ppdev->pDriverData->nvD3DPerfData.dwSystemMemory / 8, dwValue);
#else    
    // fix for any system that sets an AGP aperature TOO large
    {
        DWORD dwMem = ppdev->pDriverData->nvD3DPerfData.dwSystemMemory / 2;
        dwValue = min(dwValue, dwMem);
    }
#endif // !NV_AGP

    // convert to MB
    ppdev->pDriverData->regMaxAGPLimit = dwValue * 1024 * 1024 - 1;

#ifdef NV_AGP
    // the heap size may not be more than the aperture size
    NvRmConfigGet(dwRootHandle, dwDevID, NV_CFG_AGP_LIMIT, &dwAgpLimit);

    // GDI may be using an AGP push buffer so adjust for it
    if (ppdev->AgpPushBuffer)
        dwAgpLimit -= ppdev->DmaPushBufTotalSize;

    ppdev->pDriverData->regMaxAGPLimit = min(dwAgpLimit, ppdev->pDriverData->regMaxAGPLimit);

#else // !NV_AGP

    // the heap size may not be more than half the aperture size
    NvRmConfigGet(dwRootHandle, dwDevID, NV_CFG_AGP_LIMIT, &dwAgpLimit);

    // GDI AGP push buffer adjustment is done in NvWin2KpvmConfig for !NV_AGP case

    ppdev->pDriverData->regMaxAGPLimit = min(((dwAgpLimit + 1) / 2) - 1, ppdev->pDriverData->regMaxAGPLimit);

    if (ppdev->AgpHeap) {
        DWORD dwLimit = ppdev->AgpHeap->fpEnd - ppdev->AgpHeap->fpStart;
        if (ppdev->pDriverData->regMaxAGPLimit > dwLimit) {
            ppdev->pDriverData->regMaxAGPLimit = dwLimit;
            DPF("AGP Heap size adjusted to %d MB", (dwLimit + 1) / 1024 / 1024);
        }
    }
#endif // !NV_AGP

    return TRUE;
}

//---------------------------------------------------------------------------

#ifdef NV_AGP

//
// Win2K routine to fill in the pvmList data structure
//
void __stdcall NvWin2KpvmConfig(PDEV *ppdev, VIDEOMEMORY *pvmList)
{
    memset(pvmList, 0, sizeof(VIDEOMEMORY));
}

#else // !NV_AGP

//
// Win2K routine to fill in the pvmList data structure
//
void __stdcall NvWin2KpvmConfig(PDEV *ppdev, VIDEOMEMORY *pvmList)
{
    // Tell ddraw that this heap is not to be used for any
    // type of surface.
    pvmList->dwFlags = VIDMEM_ISLINEAR | VIDMEM_ISNONLOCAL | VIDMEM_ISWC;
    pvmList->fpStart = 0;
    pvmList->fpEnd   = pvmList->fpStart + ppdev->pDriverData->regMaxAGPLimit;

    // If GDI pushbuf is in AGP mem, need to adjust the AGP heap
    // size request.
    if (ppdev->AgpPushBuffer && (ppdev->DmaPushBufTotalSize <= pvmList->fpEnd))
        pvmList->fpEnd -= ppdev->DmaPushBufTotalSize;

    // Only use AGP mem for offscreenplain, or textures if first
    // pass allocation failed.
    pvmList->ddsCaps.dwCaps    = ~(DDSCAPS_OFFSCREENPLAIN | DDSCAPS_TEXTURE | DDSCAPS_NONLOCALVIDMEM);
    pvmList->ddsCapsAlt.dwCaps = ~(DDSCAPS_OFFSCREENPLAIN | DDSCAPS_TEXTURE | DDSCAPS_NONLOCALVIDMEM);
}

//
// Win2K routine to map the AGP heap into kernel mode address space.
//
DWORD __stdcall NvWin2KMapAgpHeap(PDEV *ppdev)
{
    NV_SYSMEM_SHARE_MEMORY      AgpHeapIn, AgpHeapOut;
    DWORD                       dwReturnedDataLength;

    AgpHeapIn.byteLength = ppdev->pDriverData->regMaxAGPLimit + 1;
    AgpHeapIn.physicalAddress = (ULONG) ppdev->pDriverData->GARTPhysicalBase;
    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_MAP_PHYS_ADDR,
                           &AgpHeapIn,  sizeof(NV_SYSMEM_SHARE_MEMORY),
                           &AgpHeapOut, sizeof(NV_SYSMEM_SHARE_MEMORY),
                           &dwReturnedDataLength))
    {
        // could not map AGP heap
        nvAssert(0);
        return FALSE;
    }

    ppdev->pDriverData->GARTLinearBase = (ULONG) AgpHeapOut.ddVirtualAddress;

    return TRUE;
}

//---------------------------------------------------------------------------

//
// Win2K routine to unmap the kernel mode address of the AGP heap.
//
DWORD __stdcall NvWin2KUnmapAgpHeap(PDEV *ppdev)
{
    NV_SYSMEM_SHARE_MEMORY      AgpHeapIn;
    DWORD                       dwReturnedDataLength;

    AgpHeapIn.byteLength = ppdev->pDriverData->regMaxAGPLimit + 1;
    AgpHeapIn.ddVirtualAddress = (PVOID) ppdev->pDriverData->GARTLinearBase;
    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_UNMAP_PHYS_ADDR,
                           &AgpHeapIn, sizeof(NV_SYSMEM_SHARE_MEMORY),
                           &AgpHeapIn, sizeof(NV_SYSMEM_SHARE_MEMORY),
                           &dwReturnedDataLength))
    {
        // could not unmap AGP heap
        nvAssert(0);
        return FALSE;
    }

    // Indicate that there is no longer an active AGP heap.
    ppdev->pDriverData->GARTLinearBase = 0;
    return TRUE;
}

//---------------------------------------------------------------------------

FLATPTR NvWin2kDxAllocMem(GLOBALDATA *pDriverData, ULONG ulHeapId, ULONG ulSize)
{
    FLATPTR fpOffset;

    if (ulHeapId == AGP_HEAP)
    {
        fpOffset = HeapVidMemAllocAligned(ppdev->AgpHeap, ulSize, 1,
            &(ppdev->AgpHeapAlignment), &(ppdev->AgpDummyPitch));
        if (fpOffset) {
            fpOffset -= pDriverData->GARTLinearHeapOffset;
        }
        else {
            // AGP memory allocation request failed
            fpOffset = ERR_DXALLOC_FAILED;
        }
    }
    else {
        nvAssert(0);
        fpOffset = ERR_DXALLOC_FAILED;
    }
    return(fpOffset);
}

//---------------------------------------------------------------------------

void NvWin2kDxFreeMem(GLOBALDATA *pDriverData, ULONG ulHeapId, FLATPTR fpOffset)
{
    if ((ulHeapId == AGP_HEAP) && pDriverData->GARTLinearBase)
    {
        fpOffset += pDriverData->GARTLinearHeapOffset;
        VidMemFree(ppdev->AgpHeap->lpHeap,
            fpOffset);
    }
}
#endif // !NV_AGP
#endif // WINNT
