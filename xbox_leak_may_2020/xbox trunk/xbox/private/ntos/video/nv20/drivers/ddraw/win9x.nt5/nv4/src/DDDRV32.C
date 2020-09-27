/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1995, 1998 NVidia Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1997 SGS-THOMSON Microelectronics  All Rights Reserved.
 *
 *  File:       dddrv32.c
 *  Content:    sample Windows95 display driver
 *              32 bit DirectDraw part.
 *
 ***************************************************************************/
#ifndef WINNT
#define INITGUID
#endif // #ifndef WINNT
#include "nvd3ddrv.h"
#include "nvcm.h"
#include "nvtypes.h"
#include "nv32.h"
#include "nvddobj.h"
#include "wingdi.h"
#include "dmemmgr.h"

#include "ddrvmem.h"
#include "d3dinc.h"
#include "global.h"
#include "nv4dreg.h"
#include "nv3ddbg.h"
#include "nvheap.h"

#ifdef  MULTI_MON
#include "nvrmapi.h"
#include "nvrmarch.inc"
#endif  /* MULTI_MON */

#define ddHALInfo            pDriverData->HALInfo
//#define OVLCCTL

#define  ST_MAX_VIDEOPORTS   1

GLOBALDATA              *pDriverData;
static HINSTANCE        hInstance;
#ifdef WINNT
SURFACEALIGNMENT    AgpHeapAlignment;
LONG                AgpDummyPitch;
#endif // #ifdef WINNT

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

#ifndef WINNT
FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void EXTERN_DDAPI DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);
#endif

extern Nv10ControlDma * __stdcall NvSysOpenDma(char *, int, int, int);
extern DWORD __stdcall NvSysOpen(DWORD);
extern void  __stdcall NvSysClose(DWORD);
extern DWORD __stdcall Blt8bpp32( LPDDHAL_BLTDATA pbd );
extern DWORD __stdcall Blt16bpp32( LPDDHAL_BLTDATA pbd );
extern DWORD __stdcall Blt32bpp32( LPDDHAL_BLTDATA pbd );
extern DWORD NvDDEnable32();
extern DWORD NvDDDisable32();


extern DWORD __stdcall ConvertOverlay32 ( LPDDRAWI_DDRAWSURFACE_LCL, BOOL waitForCompletion);
extern DWORD __stdcall PreScaleOverlay32 ( LPDDRAWI_DDRAWSURFACE_LCL );
extern DWORD __stdcall DestroyFloating422Context32 ( LPDDRAWI_DDRAWSURFACE_LCL );
extern DWORD __stdcall CreateFloating422Context32 ( LPDDRAWI_DDRAWSURFACE_LCL, DWORD dwBlockSize );
extern DWORD __stdcall UpdateOverlay32( LPDDHAL_UPDATEOVERLAYDATA );
extern DWORD __stdcall SetOverlayPosition32( LPDDHAL_SETOVERLAYPOSITIONDATA );

#ifndef WINNT
extern BOOL  __stdcall GetVideoScalerBandwidthStatus32( DWORD );
#endif

extern DWORD __stdcall CanCreateVideoPort32 ( LPDDHAL_CANCREATEVPORTDATA );
extern DWORD __stdcall CreateVideoPort32 ( LPDDHAL_CREATEVPORTDATA );
extern DWORD __stdcall FlipVideoPort32 ( LPDDHAL_FLIPVPORTDATA );
extern DWORD __stdcall GetVideoPortBandwidth32 ( LPDDHAL_GETVPORTBANDWIDTHDATA );
extern DWORD __stdcall GetVideoPortInputFormat32 ( LPDDHAL_GETVPORTINPUTFORMATDATA );
extern DWORD __stdcall GetVideoPortOutputFormat32 ( LPDDHAL_GETVPORTOUTPUTFORMATDATA );
extern DWORD __stdcall GetVideoPortField32 ( LPDDHAL_GETVPORTFIELDDATA );
extern DWORD __stdcall GetVideoPortLine32 ( LPDDHAL_GETVPORTLINEDATA );
extern DWORD __stdcall GetVideoPortConnectInfo ( LPDDHAL_GETVPORTCONNECTDATA );
extern DWORD __stdcall DestroyVideoPort32 ( LPDDHAL_DESTROYVPORTDATA );
extern DWORD __stdcall GetVideoPortFlipStatus32 ( LPDDHAL_GETVPORTFLIPSTATUSDATA );
extern DWORD __stdcall UpdateVideoPort32 ( LPDDHAL_UPDATEVPORTDATA );
extern DWORD __stdcall WaitForVideoPortSync32 ( LPDDHAL_WAITFORVPORTSYNCDATA );
extern DWORD __stdcall GetVideoSignalStatus32 ( LPDDHAL_GETVPORTSIGNALDATA );
extern DWORD HookUpVxdStorage();


#ifdef NVD3D
/*
 * D3D Hal Fcts
 */
extern BOOL __stdcall D3DHALCreateDriver(LPD3DHAL_GLOBALDRIVERDATA *lplpGlobal,
                                  LPD3DHAL_CALLBACKS* lplpHALCallbacks);
extern BOOL __stdcall D3DGetDriverInfo(LPDDHAL_GETDRIVERINFODATA lpData);

extern BOOL DestroyItemArrays(void);
#endif


#ifndef DEBUG
    #define DPF         1 ? (void)0 : (void)
#else
/*
 * Msg
 * Defined in ddsamp.c
 */
extern void __cdecl DPF( LPSTR szFormat, ... );


void __stdcall NvCachedFreeCountError(long freeCount, long correctFreeCount, DWORD *currentBufferAddress)
{
    DPF( "NvCachedFreeCountError: currentFreeCount=%08lx, correctFreeCount=%08lx, currentPutAddress=%08lx", freeCount, correctFreeCount, currentBufferAddress );
}

#endif


/*
 * ResetNV
 *
 * Reset NV after a mode change
 */
DWORD ResetNV()
{

    /* If currently in full screen DOS, don't allow channel to be enabled */
    if (pDriverData->fFullScreenDosOccurred == 2)
        return;

    if (pDriverData->fDDrawFullScreenDosOccurred == 2)
        return;

    if (pDriverData->NvAGPDmaPusherBufferBase != 0) {
        pDriverData->NvDmaPusherBufferBase = pDriverData->NvAGPDmaPusherBufferBase;
    } else if (pDriverData->NvDmaPusherBufferHandle == 0) {
        pDriverData->NvDmaPusherBufferHandle = (DWORD)
            HEAPCREATE(pDriverData->NvDmaPusherBufferSize);
        pDriverData->NvDmaPusherBufferBase = (DWORD)
            HEAPALLOC(pDriverData->NvDmaPusherBufferHandle, pDriverData->NvDmaPusherBufferSize);
    }

    /*
     * This obviously doesn't handle the case where the AGP push buffer gets moved.
     * That case is handled in UpdateNonlocalVidMem32().
     */

    if (pDriverData->dwDmaPusherCtxDmaSize == 0) {
        if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                           NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY,
                           NV01_CONTEXT_DMA,
                           (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                           ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                           (PVOID)pDriverData->NvDmaPusherBufferBase,
                           (pDriverData->NvDmaPusherBufferSize - 1)) != NVOS07_STATUS_SUCCESS)
                                return(FALSE);

        pDriverData->dwDmaPusherCtxDmaSize = pDriverData->NvDmaPusherBufferSize;
    }

    if (pDriverData->NvDevFlatDma == 0) {
        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            if (NvRmAllocChannelDma(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                                NV_DD_DEV_DMA, NV10_CHANNEL_DMA, 0,
                                NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY, 0,
                                (PVOID)&pDriverData->NvDevFlatDma) != NVOS07_STATUS_SUCCESS)
                                    return(FALSE);
        } else {
            if (NvRmAllocChannelDma(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                                NV_DD_DEV_DMA, NV4_CHANNEL_DMA, 0,
                                NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY, 0,
                                (PVOID)&pDriverData->NvDevFlatDma) != NVOS07_STATUS_SUCCESS)
                                    return(FALSE);
        }

        pDriverData->NvDmaPusherPutAddress = pDriverData->NvDmaPusherBufferBase;
        pDriverData->NvDmaPusherBufferEnd = (long *)
            pDriverData->NvDmaPusherBufferBase + ((pDriverData->NvDmaPusherBufferSize - 4) >> 2);
    }

    if (pDriverData->NvDevVideoFlatPio == 0) {
        if (NvRmAllocChannelPio(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                            NV_DD_DEV_VIDEO, NV3_CHANNEL_PIO, 0,
                            (PVOID)&pDriverData->NvDevVideoFlatPio,
                            ALLOC_CHL_PIO_FLAGS_FIFO_IGNORE_RUNOUT) != NVOS04_STATUS_SUCCESS)
                                return(FALSE);
    }

    pDriverData->wDDrawActive = (WORD)TRUE; // let everyone know DDraw is active

    if (!NvDDEnable32())
        return(FALSE);

    DPF( "    Successfully enabled NV" );

    /* Let D3D code know that we have touched NV */
    pDriverData->TwoDRenderingOccurred = 1;

    pDriverData->DDrawVideoSurfaceCount = 0;

    pDriverData->ddClipUpdate = TRUE;

    // All active floating DMA contexts have been reset
    pDriverData->fNvActiveFloatingContexts = 0;

    // reset overlay source offset
    pDriverData->OverlaySrcX = 0;
    pDriverData->OverlaySrcY = 0;
    pDriverData->OverlayFormat = 0;
    pDriverData->OverlaySrcPitch = 0;
    pDriverData->OverlaySrcSize = 0;

    pDriverData->OverlayBufferIndex = 1;

    /* Make sure overlay owner checking is enabled */
    pDriverData->OverlayRelaxOwnerCheck = 0;

    pDriverData->NvPrevDepth = (BYTE)pDriverData->bi.biBitCount;
    pDriverData->NvFirstCall = 0;

    // Initialize primary surface pointer
    pDriverData->CurrentVisibleSurfaceAddress = pDriverData->BaseAddress;

    // No DMA push blit synchronizations currently in progress
    pDriverData->syncDmaRecord.bSyncFlag = FALSE;

    return(TRUE);
}

#ifndef WINNT
/*
 * SetMode32
 */
DWORD __stdcall SetMode32(
        LPDDHAL_SETMODEDATA lpSetModeData )
{
    HDC hdc = GetDC(0);

    NvSetDriverDataPtr(lpSetModeData->lpDD);

    pDriverData->dwModeIndex = lpSetModeData->dwModeIndex;
    pDriverData->dwUseRefreshRate = lpSetModeData->useRefreshRate;

    // Call back to 16 bit direct draw code to set mode
    ExtEscape(hdc, NVSETDDRAWMODE, 0, 0, 0, 0);

    lpSetModeData->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;
}
#endif


/*
 * updateFlipStatus
 *
 * checks and sees if the most recent flip has occurred
 */
static __inline HRESULT updateFlipStatus( FLATPTR fpVidMem )
{
    int index;

    /*
     * see if a flip has happened recently
     */
    if (pDriverData->flipPrimaryRecord.bFlipFlag &&
        ((fpVidMem == 0) || (fpVidMem == pDriverData->flipPrimaryRecord.fpFlipFrom)) ) {
        NvNotification *pDmaFlipPrimaryNotifier =
            (NvNotification *)pDriverData->NvDmaFlipPrimaryNotifierFlat;

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            if ((pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) ||
                (pDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS))
                return DDERR_WASSTILLDRAWING;
            else {
                pDriverData->CurrentVisibleSurfaceAddress = pDriverData->flipPrimaryRecord.fpFlipTo;
                pDriverData->flipPrimaryRecord.bFlipFlag = FALSE;
                return DD_OK;
            }
        } else {
            NvNotification *pDmaSyncNotifier =
                (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
            /* pDmaSyncNotifier is used for NOVSYNC flips for devices < NV10 */
            if ((pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) ||
                 ((pDmaSyncNotifier->status == NV_IN_PROGRESS) &&
                  (pDriverData->fIsBltStyleFlip)))
                return DDERR_WASSTILLDRAWING;
            else {
                pDriverData->CurrentVisibleSurfaceAddress = pDriverData->flipPrimaryRecord.fpFlipTo;
                pDriverData->flipPrimaryRecord.bFlipFlag = FALSE;
                return DD_OK;
            }
        }
    }

    if (pDriverData->flipOverlayRecord.bFlipFlag &&
        ((fpVidMem == 0) || (fpVidMem == pDriverData->flipOverlayRecord.fpFlipFrom)) ) {
        NvNotification *pPioFlipOverlayNotifier =
            (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;

        index = (pDriverData->OverlayBufferIndex ^ 1) + 1;

        if (pPioFlipOverlayNotifier[index].status == NV_IN_PROGRESS)
            return DDERR_WASSTILLDRAWING;
        else {
            pDriverData->flipOverlayRecord.bFlipFlag = FALSE;
            return DD_OK;
        }
    }

    return DD_OK;

} /* updateFlipStatus */


#ifndef WINNT
/*
 * getDisplayDuration
 *
 * get the length (in ticks) of a refresh cycle
 */
static void getDisplayDuration( void )
{
    int         i;
    __int64     start, end;
    DWORD       oldclass;
    HANDLE      hprocess;

    memset( &pDriverData->flipPrimaryRecord, 0, sizeof( pDriverData->flipPrimaryRecord ) );
    memset( &pDriverData->flipOverlayRecord, 0, sizeof( pDriverData->flipOverlayRecord ) );

    hprocess = GetCurrentProcess();
    oldclass = GetPriorityClass( hprocess );
//    SetPriorityClass( hprocess, REALTIME_PRIORITY_CLASS ); Can't do this due to Electronic Arts bug in Triple Play 97
    SetPriorityClass( hprocess, HIGH_PRIORITY_CLASS );

    NvConfigSet(NV_CFG_VBLANK_TOGGLE, 1, pDriverData->dwDeviceIDNum);
    while (NvConfigGet(NV_CFG_VBLANK_TOGGLE, pDriverData->dwDeviceIDNum)); // wait for next vertical blank to happen
    QueryPerformanceCounter((LARGE_INTEGER*)&start);
    #define LOOP_CNT    20
    for( i=0;i<LOOP_CNT;i++ ) {
        NvConfigSet(NV_CFG_VBLANK_TOGGLE, 1, pDriverData->dwDeviceIDNum);
        while (NvConfigGet(NV_CFG_VBLANK_TOGGLE, pDriverData->dwDeviceIDNum)); // wait for next vertical blank to happen
    }
    QueryPerformanceCounter((LARGE_INTEGER*)&end);

    SetPriorityClass( hprocess, oldclass );
    pDriverData->flipPrimaryRecord.dwFlipDuration = ((DWORD)(end-start) + LOOP_CNT/2)/LOOP_CNT;
    pDriverData->HALInfo.dwMonitorFrequency = (DWORD)start / pDriverData->flipPrimaryRecord.dwFlipDuration;

#ifdef DEBUG
    QueryPerformanceFrequency((LARGE_INTEGER*)&start);

    DPF( "Duration = %d ticks, %dms, %dHz",
        pDriverData->flipPrimaryRecord.dwFlipDuration,
        pDriverData->flipPrimaryRecord.dwFlipDuration * 1000 / (DWORD)start,
        (DWORD)start / pDriverData->flipPrimaryRecord.dwFlipDuration);
#endif
} /* getDisplayDuration */
#endif  // #ifndef WINNT



/*
 * GetBltStatus32
 */
DWORD __stdcall GetBltStatus32(LPDDHAL_GETBLTSTATUSDATA lpGetBltStatus )
{
    FAST Nv10ControlDma *npDev;
    NvNotification      *pDmaSyncNotifier;
    unsigned long       *dmaPusherPutAddress;
    unsigned long       *dmaPusherBufferBase;
    long                freeCount = 0;

    NvSetDriverDataPtr(lpGetBltStatus->lpDD);

    pDmaSyncNotifier = (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
    dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;
    dmaPusherBufferBase = (unsigned long *)pDriverData->NvDmaPusherBufferBase;

    /*
     * CANBLT: can we add a blt?
     */
    if( lpGetBltStatus->dwFlags == DDGBS_CANBLT ) {
        /*
         * make sure that we've flipped away from the destination surface
         */
        lpGetBltStatus->ddRVal = updateFlipStatus(
                        lpGetBltStatus->lpDDSurface->lpGbl->fpVidMem );

        if( lpGetBltStatus->ddRVal == DD_OK ) {

            npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

            if (npDev == NULL) {
                if (!ResetNV()) {
                    lpGetBltStatus->ddRVal = DDERR_GENERIC;
                    return DDHAL_DRIVER_HANDLED;
                }

                dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;
                        pDriverData->fReset = FALSE;
                npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

                if (npDev == NULL) {
                    lpGetBltStatus->ddRVal = DDERR_SURFACELOST;
                    return DDHAL_DRIVER_HANDLED;
                }
            }

            /*
             * so there was no flip going on, is there room in the fifo
             * to add a blt?
             */

            NV_DD_DMA_PUSHER_SYNC();

            NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);

            /* Must save pointer in case it was changed by above macro */
            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            pDriverData->dwDmaPusherFreeCount = freeCount;

            if(freeCount < 10) {
                lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
                return DDHAL_DRIVER_HANDLED;
            } else {
                lpGetBltStatus->ddRVal = DD_OK;
            }
        }
    } else {

    /*
     * DONEBLT: is a blt in progress?
     */

        /* Some apps call GetBltStatus many times after calling Blit even after they were told that it was done */
        if (pDriverData->blitCalled == FALSE) {
            lpGetBltStatus->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;
        }

        npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

        if (npDev == NULL) {
            if (!ResetNV()) {
                lpGetBltStatus->ddRVal = DDERR_GENERIC;
                return DDHAL_DRIVER_HANDLED;
            }

            dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;
                pDriverData->fReset = FALSE;
            npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

            if (npDev == NULL) {
                lpGetBltStatus->ddRVal = DDERR_SURFACELOST;
                return DDHAL_DRIVER_HANDLED;
            }
        }

        /* If a texture surface then just wait for notifier */
        if (lpGetBltStatus->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {

            NV_DD_DMA_PUSHER_SYNC();

            /* Let D3D code know that we have touched NV */
            pDriverData->TwoDRenderingOccurred = 1;

#ifdef  CACHE_FREECOUNT
            freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
            freeCount = 0;
#endif  /* CACHE_FREECOUNT */

            while (freeCount < 4)
                NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
            freeCount -= 4;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = 0;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = 0;

            dmaPusherPutAddress += 4;

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            pDriverData->dwDmaPusherFreeCount = freeCount;

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            lpGetBltStatus->ddRVal = DD_OK;

            return DDHAL_DRIVER_HANDLED;
        }


        /* Is there a DMA push blit synchronization in progress ? */
        if (pDriverData->syncDmaRecord.bSyncFlag) { /* if so then check status */
            if (pDmaSyncNotifier->status == NV_IN_PROGRESS)
                lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
            else {
                lpGetBltStatus->ddRVal = DD_OK;
                pDriverData->syncDmaRecord.bSyncFlag = FALSE;
                pDriverData->blitCalled = FALSE;
            }
        } else { /* start a blit synchronization sequence */
            pDriverData->syncDmaRecord.bSyncFlag = TRUE;

            NV_DD_DMA_PUSHER_SYNC();

            /* Let D3D code know that we have touched NV */
            pDriverData->TwoDRenderingOccurred = 1;

#ifdef  CACHE_FREECOUNT
            freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
            freeCount = 0;
#endif  /* CACHE_FREECOUNT */

            while (freeCount < 4)
                NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
            freeCount -= 4;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = 0;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = 0;

            dmaPusherPutAddress += 4;

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            pDriverData->dwDmaPusherFreeCount = freeCount;

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            if (pDmaSyncNotifier->status == NV_IN_PROGRESS)
                lpGetBltStatus->ddRVal = DDERR_WASSTILLDRAWING;
            else {
                lpGetBltStatus->ddRVal = DD_OK;
                pDriverData->syncDmaRecord.bSyncFlag = FALSE;
                pDriverData->blitCalled = FALSE;
            }
        }
    }

    return DDHAL_DRIVER_HANDLED;

} /* GetBltStatus32 */



/*
 * Blit32
 *
 * 32-bit Blt function for all surfaces.
 *
 * Stupid callback table is built before mode is changed so we never know
 * which function to call until after mode is changed.
 */
DWORD __stdcall Blit32( LPDDHAL_BLTDATA pbd )
{
    FAST Nv10ControlDma *npDev;
    HRESULT     ddrval;

    NvSetDriverDataPtr(pbd->lpDD);

    /*
     * This needs to be here for cases where the channel has closed due to
     * a mode switch or some other reason when there is a pending notification
     * which gets lost due to the channel closure.   ResetNV will deal
     * with clearing the pending notifier.
     */

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

    if (npDev == NULL) {
        if (!ResetNV()) {
            pbd->ddRVal = DDERR_GENERIC;
            return DDHAL_DRIVER_HANDLED;
        }

        pDriverData->fReset = FALSE;
        npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

        if (npDev == NULL) {
            pbd->ddRVal = DDERR_SURFACELOST;
            return DDHAL_DRIVER_HANDLED;
        }
    }

#ifdef NV_TEX2
    /*
     * texture manager wants full control before all the sync points below
     * it will check for these in time to not break
     */
    if (pbd->lpDDDestSurface->ddsCaps.dwCaps & DDSCAPS_TEXTURE)
    {
        return nvTextureBlt(pbd);
    }
#endif // NV_TEX2

    /*
     * is a flip in progress?
     */
#ifndef DX7
    // DX7 requires that Blit() NEVER return DDERR_WASSTILLDRAWING
    ddrval = updateFlipStatus(pbd->lpDDDestSurface->lpGbl->fpVidMem);
    if( ddrval != DD_OK ) {
        if (pbd->dwFlags & DDBLT_WAIT) {
           while (ddrval != DD_OK) {
               NV_SLEEPFLIP;
               ddrval = updateFlipStatus(pbd->lpDDDestSurface->lpGbl->fpVidMem);
           }
        } else {
           pbd->ddRVal = ddrval;
           return DDHAL_DRIVER_HANDLED;
        }
    }
#endif  // DX7

    pDriverData->blitCalled = TRUE;

    /*
     * Make sure push buffer idle.
     */
    NV_DD_DMA_PUSHER_SYNC();


    /*
     * yet another d3d aa semntics hack
     */
    {
        //pCurrentContext = (PNVD3DCONTEXT)pbd->dwhContext;
        // we depend on pCurrentContext to point to the last used context
        // this may fail if pCurrentContext is not valid
        if ((pCurrentContext)
         && (pCurrentContext->dwAntiAliasFlags & AA_ENABLED_MASK)
         && (pCurrentContext->dwAntiAliasFlags & AA_IN_SCENE))
        {
            NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
            nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   // CACHE_FREECOUNT
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetRenderTargetContexts + sizeSetRenderTarget));
#endif  // CACHE_FREECOUNT
            nvAARenderPrimitive();
            pCurrentContext->dwAntiAliasFlags |= AA_COPY_SCREEN;
            NV_D3D_GLOBAL_SAVE();
        }
    }

    if (pDriverData->bi.biBitCount == 8) {
        return(Blt8bpp32(pbd));
    } else if (pDriverData->bi.biBitCount == 16) {
        return(Blt16bpp32(pbd));
    } else {
        return(Blt32bpp32(pbd));
    }

} /* Blit32 */


/*
 * GetFlipStatus32
 *
 */
DWORD __stdcall GetFlipStatus32(LPDDHAL_GETFLIPSTATUSDATA lpGetFlipStatus )
{
    Nv10ControlDma *npDev;
    DWORD fpVidMem;

    NvSetDriverDataPtr(lpGetFlipStatus->lpDD);

    fpVidMem = lpGetFlipStatus->lpDDSurface->lpGbl->fpVidMem;

    lpGetFlipStatus->ddRVal = updateFlipStatus(fpVidMem);

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

    if (npDev == NULL) {
        lpGetFlipStatus->ddRVal = DDERR_SURFACELOST;
    } else {
        if (lpGetFlipStatus->dwFlags & (DDGFS_CANFLIP | DDGFS_ISFLIPDONE)) {
            NvNotification *pDmaFlipPrimaryNotifier =
                (NvNotification *)pDriverData->NvDmaFlipPrimaryNotifierFlat;

            if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
                if (((pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) ||
                     (pDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS)) &&
                     ((fpVidMem == pDriverData->flipPrimaryRecord.fpFlipFrom) ||
                      (fpVidMem == pDriverData->flipPrimaryRecord.fpFlipTo)))
                        lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
            } else {
                NvNotification *pDmaSyncNotifier =
                    (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
                /* pDmaSyncNotifier is used for NOVSYNC flips for devices < NV10 */
                if (((fpVidMem == pDriverData->flipPrimaryRecord.fpFlipFrom) ||
                     (fpVidMem == pDriverData->flipPrimaryRecord.fpFlipTo)) &&
                     ((pDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) ||
                     ((pDriverData->fIsBltStyleFlip) &&
                      (pDmaSyncNotifier->status == NV_IN_PROGRESS))))
                        lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
            }
        }

        /*
         * Check if the push buffer is busy if someone wants to know if they can flip
         */

        if (lpGetFlipStatus->dwFlags & DDGFS_CANFLIP) {

            if (lpGetFlipStatus->ddRVal == DD_OK) {
                unsigned long *dmaPusherPutAddress =
                    (unsigned long *)pDriverData->NvDmaPusherPutAddress;
                unsigned long *dmaPusherBufferBase =
                    (unsigned long *)pDriverData->NvDmaPusherBufferBase;

                NV_DD_DMA_PUSHER_SYNC();

                /* Give the application a chance to do something else if push buffer isn't empty yet */
                if (npDev->Get != (unsigned long)(dmaPusherPutAddress - dmaPusherBufferBase) << 2) {
                    /* Force write combine buffer to flush */
                    pDriverData->NvDmaPusherBufferEnd[0] = 0;
                    /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                    _outp (0x3d0,0);
                    npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
                    lpGetFlipStatus->ddRVal = DDERR_WASSTILLDRAWING;
                }
            }
        }
    }

    return DDHAL_DRIVER_HANDLED;

} /* GetFlipStatus32 */


// ------------------------------------------------------------------------------------------
// YUV422ColourControl
//      Simulates colour controls on YUV422 surfaces.  So far only supports contrast.
//      Returns TRUE if success
#ifndef OVLCCTL
#define YUV422ColourControl(a,b)
#else
BOOL __stdcall YUV422ColourControl(LPDDRAWI_DDRAWSURFACE_GBL lpGbl, LPDDCOLORCONTROL lpDDCC)
{
//FAST Nv10ControlDma *npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
//unsigned long *dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;
//unsigned long *dmaPusherBufferBase = (unsigned long *)pDriverData->NvDmaPusherBufferBase;
//long freeCount = 0;
DDPIXELFORMAT *lpPixelFormat = &lpGbl->ddpfSurface;
DWORD overlayFourCC = 0;
DWORD dwContrast;
DWORD  dwOffset = (DWORD)lpGbl->fpVidMem - (DWORD)pDriverData->BaseAddress;
FAST Nv3ChannelPio *npDevVideo = (Nv3ChannelPio *)pDriverData->NvDevVideoFlatPio;
FAST long videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);

    if (lpPixelFormat->dwSize == sizeof(DDPIXELFORMAT) && (lpPixelFormat->dwFlags & DDPF_FOURCC))
        overlayFourCC = lpPixelFormat->dwFourCC;
    if (overlayFourCC == MAKEFOURCC('Y','U','Y','2'))
        dwContrast = lpDDCC->lContrast | (lpDDCC->lContrast << 16) | 0xFF00FF00;
    else if (overlayFourCC == MAKEFOURCC('U','Y','V','Y'))
        dwContrast = (lpDDCC->lContrast << 8) | (lpDDCC->lContrast << 24) | 0x00FF00FF;
    else dwContrast = 0xFFFFFFFF;

    if (dwContrast == 0xFFFFFFFF) return FALSE;

    while (videoFreeCount < 48)
        videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
    videoFreeCount -= 48;

    /* Trash spare subchannel */
    npDevVideo->dDrawSpare.SetObject = NV_DD_CONTEXT_BETA4;
    npDevVideo->dDrawSpare.nv4ContextBeta.SetBetaFactor = dwContrast;
    npDevVideo->dDrawSpare.SetObject = NV_DD_SURFACES_2D_A8R8G8B8;
    npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetPitch = (lpGbl->lPitch << 16) | lpGbl->lPitch;
    npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetSource = dwOffset;
    npDevVideo->dDrawSpare.nv4ContextSurfaces2D.SetOffsetDestin = dwOffset;
    npDevVideo->dDrawSpare.SetObject = NV_DD_SRCCOPY_PREMULT_A8R8G8B8_IMAGE_BLIT;
    npDevVideo->dDrawSpare.nv4ImageBlit.ControlPointIn = 0;
    npDevVideo->dDrawSpare.nv4ImageBlit.ControlPointOut = 0;
    npDevVideo->dDrawSpare.nv4ImageBlit.Size = (lpGbl->wHeight << 16) | (lpGbl->wWidth >> 1);

    pDriverData->dDrawSpareSubchannelObject = 0;

    /* Force fifo to stall until blit operation finishes */
    npDevVideo->dDrawSpare.nv4ImageBlit.Notify = 0;
    npDevVideo->dDrawSpare.nv4ImageBlit.NoOperation = 0;

    return TRUE;
}
#endif

/*
 * Flip32
 *
 * Flip RGB or overlay surfaces.
 *
 * NOTES:
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
 *
 */

DWORD __stdcall Flip32(  LPDDHAL_FLIPDATA pfd )
{
    FAST Nv10ControlDma         *npDev;
    NvNotification              *pDmaFlipPrimaryNotifier;
    NvNotification              *pPioFlipOverlayNotifier;
    NvNotification              *pDmaSyncNotifier;
    NvNotification              *pDmaDmaFromMemNotifier;
    HRESULT                     ddrval;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurfTarg_gbl;
    BOOL                        bIsD3D;
    BOOL                        waitForVsync = TRUE;

    NvSetDriverDataPtr(pfd->lpDD);

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
    pDmaFlipPrimaryNotifier = (NvNotification *)pDriverData->NvDmaFlipPrimaryNotifierFlat;
    pPioFlipOverlayNotifier = (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;
    pDmaSyncNotifier = (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
    pDmaDmaFromMemNotifier = (NvNotification *)pDriverData->NvDmaDmaFromMemNotifierFlat;

    /*
     * NOTES:
     *
     * This callback is invoked whenever we are about to flip to from
     * one surface to another.   pfd->lpSurfCurr is the surface we were at,
     * pfd->lpSurfTarg is the one we are flipping to.
     *
     * You should point the hardware registers at the new surface, and
     * also keep track of the surface that was flipped away from, so
     * that if the user tries to lock it, you can be sure that it is done
     * being displayed
     */

    DPF_LEVEL(NVDBG_LEVEL_FLIP,"Flip32: curr=%08lx, targ=%08lx", pfd->lpSurfCurr, pfd->lpSurfTarg );
    DPF_LEVEL(NVDBG_LEVEL_FLIP,"        vidmem ptrs change: %08lx->%08lx",
                                        pfd->lpSurfCurr->lpGbl->fpVidMem,
                                        pfd->lpSurfTarg->lpGbl->fpVidMem );

    /*
     * D3D: no VSync will insert a Blt and exit
     */
    bIsD3D = (pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_3DDEVICE)
          && (pfd->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE)
          && (pfd->lpSurfCurr->ddsCaps.dwCaps & DDSCAPS_3DDEVICE);

    if ((((pDriverData->regD3DEnableBits1 & REG_BIT1_NO_WAIT_4_VSYNC) && bIsD3D)) ||
          (pfd->dwFlags & DDFLIP_NOVSYNC)) {

        /* DDFLIP_NOVSYNC flips must be restricted to normal frame depth count */
        if ((pfd->dwFlags & DDFLIP_NOVSYNC) &&
            (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10)) {

            ddrval = updateFlipStatus(0);

            if (ddrval != DD_OK) {
                if (pfd->dwFlags & DDFLIP_WAIT) {
                    while (ddrval != DD_OK) {
                                        NV_SLEEPFLIP;
                        ddrval = updateFlipStatus(0);
                    }
                } else {
                    pfd->ddRVal = ddrval;
                    return DDHAL_DRIVER_HANDLED;
                }
            }
        }

        /*
         * Check how many frames we are ahead
         *  we want to limit this since we have serious lag effect on fast CPUs
         */
        // free count for d3d stuff must be accurate
#ifndef NV_NULL_HW_DRIVER
        {
            NV_D3D_GLOBAL_SETUP();
#ifdef  CACHE_FREECOUNT
            nvFreeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, 1);
#endif  /* CACHE_FREECOUNT */

            if ((int)(pDriverData->dwCurrentFrame - pDriverData->dwRenderedFrame) > pDriverData->regPreRenderLimit)
            {
                nvStartDmaBuffer (FALSE);
                /*
                 * if we do not have to wait here - don't
                 */
                /* it does not seem to work - some apps just fail to call Flip again
                if (!(pfd->dwFlags & DDFLIP_WAIT))
                {
                    pfd->ddRVal = DDERR_WASSTILLDRAWING;
                    return DDHAL_DRIVER_HANDLED;
                }
                */
                /*
                 * wait for HW to catch up
                 */
#ifdef NV_PROFILE
                NVP_START(NVP_T_FLIP);
#endif

                do
                {
                    NV_DELAY;
#ifndef WINNT
                  pDriverData->dwRenderedFrame = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_SPOOFED_FLIP_COUNT + pDriverData->BaseAddress);
#else
                  pDriverData->dwRenderedFrame = *(DWORD*)(pDriverData->dwTMPseudoNotifierOffset + NV_PN_SPOOFED_FLIP_COUNT + (ULONG) pDriverData->ppdev->pjScreen);
#endif // #ifdef WINNT

                } while ((int)(pDriverData->dwCurrentFrame - pDriverData->dwRenderedFrame) > pDriverData->regPreRenderLimit);

#ifdef NV_PROFILE
                NVP_STOP(NVP_T_FLIP);
                nvpLogTime(NVP_T_FLIP,nvpTime[NVP_T_FLIP]);
#endif
            }
            /*
             * Update frame counters
             */
            pDriverData->dwCurrentFrame++;
            if (!pDriverData->dwCurrentFrame) { pDriverData->dwCurrentFrame = pDriverData->dwRenderedFrame = 0; } // wrap around case
            nvPlacePsuedoNotifier (NV_PN_SPOOFED_FLIP_COUNT,pDriverData->dwCurrentFrame);
            NV_D3D_GLOBAL_SAVE();
#ifdef  CACHE_FREECOUNT
            pDriverData->dwDmaPusherFreeCount = nvFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(nvFreeCount, pDriverData->NvDmaPusherPutAddress);
#endif  /* DEBUG */
#endif  /* CACHE_FREECOUNT */
        }
#endif  //!NV_NULL_HW_DRIVER


        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            waitForVsync = FALSE;
        } else {
            /*
             * Do the Blt
             */
            DWORD  dwSrcOffset =  (DWORD)VIDMEM_OFFSET(pfd->lpSurfTarg->lpGbl->fpVidMem);
            DWORD  dwDstOffset =  (DWORD)VIDMEM_OFFSET(pfd->lpSurfCurr->lpGbl->fpVidMem);
            DWORD  dwPitch     = (pfd->lpSurfTarg->lpGbl->lPitch  << 16) | pfd->lpSurfCurr->lpGbl->lPitch;
            DWORD  dwDims      = (pfd->lpSurfCurr->lpGbl->wHeight << 16) | pfd->lpSurfCurr->lpGbl->wWidth;
            unsigned long *dmaPusherPutAddress =
                (unsigned long *)pDriverData->NvDmaPusherPutAddress;
            unsigned long *dmaPusherBufferBase =
                (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
            long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
            long freeCount = 0;
#endif  /* CACHE_FREECOUNT */

            if (pDriverData->dwSharedClipChangeCount != pDriverData->dwDDMostRecentClipChangeCount) {

                while (freeCount < 7)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 7, dmaPusherPutAddress);
                freeCount -= 7;

                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
                dmaPusherPutAddress[1] = NV_DD_IMAGE_BLACK_RECTANGLE;
                dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                    IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000;
                dmaPusherPutAddress[3] = 0;
                dmaPusherPutAddress[4] = asmMergeCoords(NV_MAX_X_CLIP, NV_MAX_Y_CLIP);
                dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
                dmaPusherPutAddress[6] = NV_DD_SURFACES_2D;

                dmaPusherPutAddress += 7;

                pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                /* Force write combine buffer to flush */
                pDriverData->NvDmaPusherBufferEnd[0] = 0;
                /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                _outp (0x3d0,0);

                pDriverData->dwDmaPusherFreeCount = freeCount;

                npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                pDriverData->dwSharedClipChangeCount++;

                pDriverData->dwDDMostRecentClipChangeCount = pDriverData->dwSharedClipChangeCount;
            }

            while (freeCount < 10)
                NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
            freeCount -= 10;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                SET_ROP_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = SRCCOPYINDEX;

            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                SURFACES_2D_PITCH_OFFSET | 0xC0000;
            dmaPusherPutAddress[3] = dwPitch;
            dmaPusherPutAddress[4] = dwSrcOffset;
            dmaPusherPutAddress[5] = dwDstOffset;

            dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_BLIT) +
                BLIT_POINT_IN_OFFSET | 0xC0000;
            dmaPusherPutAddress[7] = 0;
            dmaPusherPutAddress[8] = 0;
            dmaPusherPutAddress[9] = dwDims;

            dmaPusherPutAddress += 10;

            if (pfd->dwFlags & DDFLIP_NOVSYNC) {

                pDriverData->flipPrimaryRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
                pDriverData->flipPrimaryRecord.fpFlipTo = pfd->lpSurfTarg->lpGbl->fpVidMem;

                pDriverData->flipPrimaryRecord.bFlipFlag = TRUE;

                pDriverData->fIsBltStyleFlip = TRUE;

                pDmaSyncNotifier->status = NV_IN_PROGRESS;

                while (freeCount < 4)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                freeCount -= 4;

                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
                dmaPusherPutAddress[1] = 0;
                dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
                dmaPusherPutAddress[3] = 0;

                dmaPusherPutAddress += 4;
            }

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            pDriverData->dwDmaPusherFreeCount = freeCount;

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            /*
             * flip the vidmem pointers (so ddraw can flip them back)
             */
            pfd->lpSurfCurr->lpGbl->fpVidMem = dwSrcOffset + (DWORD)pDriverData->BaseAddress;
            pfd->lpSurfTarg->lpGbl->fpVidMem = dwDstOffset + (DWORD)pDriverData->BaseAddress;

            /*
             * done
             */
            pfd->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;
        }
    }

    /*
     * everything is OK, do the flip here
     */

    pDriverData->fIsBltStyleFlip = FALSE;

    pSurfTarg_gbl = pfd->lpSurfTarg->lpGbl;

    pDriverData->flipPrimaryRecord.bIsD3DSurface = FALSE;

    /* Is this an overlay surface ? */
    if (!(pfd->lpSurfTarg->ddsCaps.dwCaps & (DDSCAPS_OVERLAY | DDSCAPS_VIDEOPORT))) {
        /* No - assume it is the primary surface */
        unsigned long *dmaPusherPutAddress =
            (unsigned long *)pDriverData->NvDmaPusherPutAddress;
        unsigned long *dmaPusherBufferBase =
            (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
        long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
        long freeCount = 0;
#endif  /* CACHE_FREECOUNT */

#ifdef NV_PROFILE
        NVP_START(NVP_T_FLIP);
#endif

#ifdef FORCED_TRIPLE_BUFFER
        if (pDriverData->bNeedToAttachBuffer) {
            HRESULT result;
            IDirectDrawSurface *lpSurfPrimary_int = (IDirectDrawSurface *) pfd->lpDD->lpExclusiveOwner->lpPrimary;
            if (lpSurfPrimary_int) {
                result = IDirectDrawSurface_AddAttachedSurface(lpSurfPrimary_int, pDriverData->lpBuffer3);
                if (result != DD_OK) {
                    IDirectDrawSurface_Release(pDriverData->lpBuffer3);
                    pDriverData->lpBuffer3 = NULL;
                }
            }
            pDriverData->bNeedToAttachBuffer = FALSE;
        }
#endif // FORCED_TRIPLE_BUFFER

        ddrval = updateFlipStatus(0);

        if (ddrval != DD_OK) {
            /* Flush DMA push buffer on first Flip call regardless of current flip status */
            if (!(pDriverData->fDmaPushBufferHasBeenFlushed)) {

                /*
                 * is NV busy right now?
                 */
                NV_DD_DMA_PUSHER_SYNC();

                /* Optimize flip waits for D3D rendered surfaces. */
                pDriverData->flipPrimaryRecord.bIsD3DSurface = bIsD3D;

                if (npDev == NULL) {
                    pfd->ddRVal = DDERR_SURFACELOST;
                    return DDHAL_DRIVER_HANDLED;
                }

                while (freeCount < 4)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                freeCount -= 4;

                /* Let D3D code know that we have touched NV */
                pDriverData->TwoDRenderingOccurred = 1;

                pDmaSyncNotifier->status = NV_IN_PROGRESS;

                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
                dmaPusherPutAddress[1] = 0;
                dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
                dmaPusherPutAddress[3] = 0;

                dmaPusherPutAddress += 4;

                pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                /* Force write combine buffer to flush */
                pDriverData->NvDmaPusherBufferEnd[0] = 0;
                /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                _outp (0x3d0,0);

                pDriverData->dwDmaPusherFreeCount = freeCount;

                npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                pDriverData->fDmaPushBufferHasBeenFlushed = TRUE;
            }

            if (pfd->dwFlags & DDFLIP_WAIT) {
                while (ddrval != DD_OK) {
                    NV_SLEEPFLIP;
                    ddrval = updateFlipStatus(0);
                }
            } else {
                pfd->ddRVal = ddrval;
                return DDHAL_DRIVER_HANDLED;
            }
        }

        /*
         * is NV busy right now?
         */
        NV_DD_DMA_PUSHER_SYNC();

#ifdef NV_PROFILE
        NVP_STOP(NVP_T_FLIP);
        nvpLogTime(NVP_T_FLIP,nvpTime[NVP_T_FLIP]);
#endif

        /* Optimize flip waits for D3D rendered surfaces. */
        /*
        if (pSurfTarg_gbl->fpVidMem == pDriverData->lpLast3DSurfaceRendered)
            pDriverData->flipPrimaryRecord.bIsD3DSurface = TRUE;
        */
        pDriverData->flipPrimaryRecord.bIsD3DSurface = bIsD3D;

        if (npDev == NULL) {
            pfd->ddRVal = DDERR_SURFACELOST;
            return DDHAL_DRIVER_HANDLED;
        }

        while (freeCount < 4)
            NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
        freeCount -= 4;

        /* Let D3D code know that we have touched NV */
        pDriverData->TwoDRenderingOccurred = 1;

        pDmaSyncNotifier->status = NV_IN_PROGRESS;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
            RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
        dmaPusherPutAddress[1] = 0;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
            RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
        dmaPusherPutAddress[3] = 0;

        dmaPusherPutAddress += 4;

        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

        pDriverData->flipPrimaryRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
        pDriverData->flipPrimaryRecord.fpFlipTo = pSurfTarg_gbl->fpVidMem;

        pDriverData->flipPrimaryRecord.bFlipFlag = TRUE;

        pDriverData->primaryBufferIndex ^= 1;

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

            pDmaFlipPrimaryNotifier[pDriverData->primaryBufferIndex + 1].status = NV_IN_PROGRESS;

            /* Temporarily load VIDEO_LUT_CURSOR_DAC object */
            while (freeCount < 7)
                NvGetDmaBufferFreeCount(npDev, freeCount, 7, dmaPusherPutAddress);
            freeCount -= 7;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_VIDEO_LUT_CURSOR_DAC;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                NV067_SET_IMAGE_OFFSET(pDriverData->primaryBufferIndex) | 0x80000;
            dmaPusherPutAddress[3] = pSurfTarg_gbl->fpVidMem - pDriverData->BaseAddress;
            if (waitForVsync)
                dmaPusherPutAddress[4] = (NV067_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31) ||
                                         (NV067_SET_IMAGE_FORMAT_WHEN_NOT_WITHIN_BUFFER << 20) ||
                                         pSurfTarg_gbl->lPitch;
            else
                dmaPusherPutAddress[4] = (NV067_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31) ||
                                         (NV067_SET_IMAGE_FORMAT_WHEN_IMMEDIATELY << 20) ||
                                         pSurfTarg_gbl->lPitch;
            dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
            dmaPusherPutAddress[6] = NV_DD_SURFACES_2D;

            dmaPusherPutAddress += 7;

        } else {

            pDmaFlipPrimaryNotifier[1].status = NV_IN_PROGRESS;

            /* Temporarily load VIDEO_FROM_MEMORY object */
            while (freeCount < 10)
                NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
            freeCount -= 10;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_PRIMARY_VIDEO_FROM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                VIDEO_FROM_MEM_OFFSET_OFFSET | 0x140000;
            dmaPusherPutAddress[3] = pSurfTarg_gbl->fpVidMem - pDriverData->BaseAddress;
            dmaPusherPutAddress[4] = pSurfTarg_gbl->lPitch;
            dmaPusherPutAddress[5] = asmMergeCoords(pSurfTarg_gbl->wWidth, pSurfTarg_gbl->wHeight);
            if (pDriverData->bi.biBitCount == 8)
                dmaPusherPutAddress[6] = NV_VFM_FORMAT_COLOR_LE_Y8_P4;
            else if (pDriverData->bi.biBitCount == 16)
                dmaPusherPutAddress[6] = NV_VFM_FORMAT_COLOR_LE_X1R5G5B5_P2;
            else
                dmaPusherPutAddress[6] = NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;
            dmaPusherPutAddress[7] = 0;
            dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
            dmaPusherPutAddress[9] = NV_DD_SURFACES_2D;

            dmaPusherPutAddress += 10;
        }

        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        pDriverData->dwDmaPusherFreeCount = freeCount;

        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

        pDriverData->fDmaPushBufferHasBeenFlushed = FALSE;

        DPF( "Flip32: RGB Flip request completed successfully");

    } else if (pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_OVERLAY) { /* flip overlay */
        FAST Nv3ChannelPio *npDevVideo = (Nv3ChannelPio *)pDriverData->NvDevVideoFlatPio;
        FAST long videoFreeCount;
        DWORD dwOffset;
        DWORD dwPitch;
        DWORD dwHeight;
        DWORD index;
        DWORD doBob = 0;
        DWORD dwFieldFlags = 0;
        DWORD dwOverlayDeltaY = pDriverData->dwOverlayDeltaY;
        DWORD preScaleOverlay = 0;
        DWORD srcPreShrinkDeltaX;
        DWORD srcPreShrinkDeltaY;
        BOOL  surfaceIs422 = FALSE;

        if (npDevVideo == NULL) {
            pfd->ddRVal = DDERR_SURFACELOST;
            return DDHAL_DRIVER_HANDLED;
        }

        if (pDriverData->fDDrawFullScreenDosOccurred) {
            pDriverData->OverlayRelaxOwnerCheck = 1;        /* relax owner check when we come back from DOS mode */
            if (pDriverData->fDDrawFullScreenDosOccurred == 2) {  /* still in DOS mode ? */
                pPioFlipOverlayNotifier[1].status = 0;      /* clear notifiers */
                pPioFlipOverlayNotifier[2].status = 0;
                pfd->ddRVal = DDERR_SURFACEBUSY;
                return DDHAL_DRIVER_HANDLED;
            } else {                                        /* back to hires mode */
                pPioFlipOverlayNotifier[1].status = 0;      /* clear notifiers */
                pPioFlipOverlayNotifier[2].status = 0;
                pDriverData->fDDrawFullScreenDosOccurred = 0;    /* reset flag */
            }
        }

        /* Limit flips to 1 at a time to work around NV4 RM limitation */
        ddrval = updateFlipStatus(pSurfTarg_gbl->fpVidMem);

        /* Only reject flip requests when flipping frames (but allow fields to proceed) */
        if (pfd->dwFlags & (DDFLIP_ODD | DDFLIP_EVEN) == 0) {
            if (ddrval != DD_OK) {
                if (pfd->dwFlags & DDFLIP_WAIT) {
                    // do contrast blt (only supports YUV422 style formats)
                    YUV422ColourControl(pfd->lpSurfTarg->lpGbl, &pDriverData->OverlayColourControl);

                    while (ddrval != DD_OK) {
                        NV_SLEEPFLIP;
                        ddrval = updateFlipStatus(pSurfTarg_gbl->fpVidMem);
                    }
                } else {
                    pfd->ddRVal = ddrval;
                    return DDHAL_DRIVER_HANDLED;
                }
            } else {
                // do contrast blt (only supports YUV422 style formats)
                YUV422ColourControl(pfd->lpSurfTarg->lpGbl, &pDriverData->OverlayColourControl);
            }
        } else {
            // do contrast blt (only supports YUV422 style formats)
            YUV422ColourControl(pfd->lpSurfTarg->lpGbl, &pDriverData->OverlayColourControl);
        }

        /*
         * is NV busy right now?
         */
        NV_DD_DMA_PUSHER_SYNC();

        if (pDriverData->dwOverlaySurfaces > 2)
            ConvertOverlay32(pfd->lpSurfTarg, TRUE);

        videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);

        pDriverData->flipOverlayRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
        pDriverData->flipOverlayRecord.fpFlipTo = pSurfTarg_gbl->fpVidMem;

        pDriverData->flipOverlayRecord.bFlipFlag = TRUE;

        /* Make sure we flip to the correct starting offset in new overlay surface */
        dwOffset = pSurfTarg_gbl->fpVidMem - pDriverData->BaseAddress;

        if ((pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_IF09) ||
            (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_YVU9) ||
            (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_IV32) ||
            (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_IV31)) {
            /* Skip over Indeo portion of surface */
            dwPitch = (DWORD)pSurfTarg_gbl->wWidth;
            dwOffset += ((dwPitch * ((DWORD)pSurfTarg_gbl->wHeight * 10L)) >> 3);
            /* Force block to be properly aligned */
            dwOffset = (dwOffset + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
            dwPitch = ((pSurfTarg_gbl->wWidth + 1) & 0xFFFE) << 1;
            dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        } else if ((pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_YV12) ||
                   (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_420i)) {
            /* Assumes a separate overlay YUY2 surface in video memory */
            if (pSurfTarg_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0])
                dwOffset = pDriverData->NvYUY2Surface0Mem -
                               pDriverData->BaseAddress;
            else if (pSurfTarg_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1])
                dwOffset = pDriverData->NvYUY2Surface1Mem -
                               pDriverData->BaseAddress;
            else if (pSurfTarg_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2])
                dwOffset = pDriverData->NvYUY2Surface2Mem -
                               pDriverData->BaseAddress;
            else
                dwOffset = pDriverData->NvYUY2Surface3Mem -
                               pDriverData->BaseAddress;

            dwPitch = ((pSurfTarg_gbl->wWidth + 1) & 0xFFFE) << 1;
            dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        } else {
            dwPitch = (DWORD)pSurfTarg_gbl->lPitch;
            surfaceIs422 = TRUE;
        }

        /* Assume full surface height */
        dwHeight = (DWORD)pSurfTarg_gbl->wHeight;

        if ( !(pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)  ) {
            if (pfd->dwFlags & (DDFLIP_ODD | DDFLIP_EVEN)) {
                if (pfd->lpSurfTarg->lpSurfMore) {
                    if (pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_INTERLEAVED) {
                        dwPitch <<= 1;
                        dwFieldFlags = pfd->dwFlags;
                        if ((pfd->dwFlags & DDFLIP_ODD) &&
                            (pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_BOB))
                            doBob = 2; /* Tell RM to BOB this field */
                    } else {
                        if ((pfd->dwFlags & DDFLIP_ODD) &&
                            (pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_BOB))
                            doBob = 2; /* Tell RM to BOB this field */
                    }
                }
                /* If flipping interleaved fields then halve the surface height */
                if (dwFieldFlags & (DDFLIP_ODD | DDFLIP_EVEN)) {
                    dwHeight >>= 1;
#ifdef  NOPRESTRETCH
                    dwOverlayDeltaY >>= 1;
#endif  // NOPRESTRETCH
                }
            } else {
                /* Sometimes we get calls with DDOVER_INTERLEAVED set but with neither DDFLIP_ODD nor DDFLIP_EVEN flags set. */
                /* This happens mostly during transitions when dynamic bob and weave are occurring. */
                if (pfd->lpSurfTarg->lpSurfMore) {
                    if (pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_INTERLEAVED) {
                        if (pfd->lpSurfTarg->lpSurfMore->dwOverlayFlags & DDOVER_BOB) {
#ifdef  NOPRESTRETCH
                            dwOverlayDeltaY >>= 1;
#endif  // NOPRESTRETCH
                            dwHeight >>= 1;
                            dwPitch <<= 1;
                            doBob = 2;
                            dwFieldFlags = DDFLIP_ODD;
                        }
                    }
                }
            }
        }

        srcPreShrinkDeltaX = 0x100000;
        srcPreShrinkDeltaY = 0x100000;

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

            if ((((pDriverData->dwOverlaySrcWidth + 7) >> 3) >
                   pDriverData->dwOverlayDstWidth) && (pDriverData->dwOverlayDstWidth > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
                srcPreShrinkDeltaX =
                    (pDriverData->dwOverlaySrcWidth << 20) / pDriverData->dwOverlayDstWidth;
            }

            if ((((pDriverData->dwOverlaySrcHeight + 7) >> 3) >
                   pDriverData->dwOverlayDstHeight) && (pDriverData->dwOverlayDstHeight > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
                srcPreShrinkDeltaY =
                    (pDriverData->dwOverlaySrcHeight << 20) / pDriverData->dwOverlayDstHeight;
                dwOverlayDeltaY = pDriverData->dwOverlayDeltaY;
            }
        } else {

            if ((pDriverData->dwOverlaySrcWidth >
                   pDriverData->dwOverlayDstWidth) && (pDriverData->dwOverlayDstWidth > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
                srcPreShrinkDeltaX =
                    (pDriverData->dwOverlaySrcWidth << 20) / pDriverData->dwOverlayDstWidth;
            }

            if ((pDriverData->dwOverlaySrcHeight >
                   pDriverData->dwOverlayDstHeight) && (pDriverData->dwOverlayDstHeight > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
                srcPreShrinkDeltaY =
                    (pDriverData->dwOverlaySrcHeight << 20) / pDriverData->dwOverlayDstHeight;
                dwOverlayDeltaY = pDriverData->dwOverlayDeltaY;
            }
        }

#ifndef NOPRESTRETCH
        if (dwFieldFlags & (DDFLIP_ODD | DDFLIP_EVEN)) {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_FIELD;
            srcPreShrinkDeltaY >>= 1;
        }
#endif  // NOPRESTRETCH

        /* Alternate buffer indexes to keep the hardware happy */
        pDriverData->OverlayBufferIndex ^= 1;

        index = pDriverData->OverlayBufferIndex;

        /* Preshrink overlay surface if necessary */
        if ((pDriverData->TotalVRAM >> 20 > 4) && (preScaleOverlay) &&
            (!(pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT))) {

            while (videoFreeCount < 76)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 76;

            /* Trash spare subchannel */
            npDevVideo->dDrawSpare.SetObject = NV_DD_DVD_SUBPICTURE;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutPoint = 0;

            /* Widths below are increased by 2 instead of 1 to remain hardware compatible */
            if (preScaleOverlay == NV_PRESCALE_OVERLAY_X) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                   pSurfTarg_gbl->wHeight);
            } else if (preScaleOverlay == NV_PRESCALE_OVERLAY_Y) {
                if (dwFieldFlags & (DDFLIP_ODD | DDFLIP_EVEN)) {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((pSurfTarg_gbl->wWidth + 2) & ~1),
                                       (pDriverData->dwOverlayDstHeight >> 1));
                } else {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((pSurfTarg_gbl->wWidth + 2) & ~1),
                                       pDriverData->dwOverlayDstHeight);
                }
            } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_X + NV_PRESCALE_OVERLAY_Y)) {
                if (dwFieldFlags & (DDFLIP_ODD | DDFLIP_EVEN)) {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                       (pDriverData->dwOverlayDstHeight >> 1));
                } else {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                       pDriverData->dwOverlayDstHeight);
                }
            } else if (preScaleOverlay == NV_PRESCALE_OVERLAY_FIELD) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 2) & ~1),
                                   pDriverData->dwOverlaySrcHeight);
            } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_X + NV_PRESCALE_OVERLAY_FIELD)) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                   pDriverData->dwOverlaySrcHeight);
            } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_Y + NV_PRESCALE_OVERLAY_FIELD)) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pSurfTarg_gbl->wWidth + 2) & ~1),
                                   pDriverData->dwOverlayDstHeight);
            } else { // NV_PRESCALE_OVERLAY_X + NV_PRESCALE_OVERLAY_Y + NV_PRESCALE_OVERLAY_FIELD
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                   pDriverData->dwOverlayDstHeight);
            }

            if (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD) {
                if (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY)
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | (dwPitch >> 1);
                else
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | (dwPitch >> 1);


            } else {
                if (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY)
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | dwPitch;
                else
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | dwPitch;
            }

            /* Output to extra overlay surface memory area */
            if ((surfaceIs422) && (pDriverData->extra422OverlayOffset[index] != 0)) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutOffset =
                    pDriverData->extra422OverlayOffset[index];
            } else {
                if (dwFieldFlags & DDFLIP_ODD)
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutOffset =
                        (dwOffset + (dwPitch >> 1) + pSurfTarg_gbl->dwReserved1);
                else
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutOffset =
                        (dwOffset + pSurfTarg_gbl->dwReserved1);
            }

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInDeltaDuDx = srcPreShrinkDeltaX;
            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInDeltaDvDy = srcPreShrinkDeltaY;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInSize =
                asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 1) & ~1), dwHeight);

            if (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY)
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInFormat =
                    (NV038_IMAGE_IN_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | dwPitch;
            else
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInFormat =
                    (NV038_IMAGE_IN_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | dwPitch;

            /* Temporarily offset to the beginning of the visible portion of source surface */
            dwOffset += (pDriverData->OverlaySrcY * dwPitch);
            dwOffset += ((pDriverData->OverlaySrcX & ~1) << 1);

            if (dwFieldFlags & DDFLIP_ODD)
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInOffset =
                    (dwOffset + (dwPitch >> 1));
            else
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInOffset =
                    dwOffset;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInPoint = 0;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayDeltaDuDx = srcPreShrinkDeltaX;
            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayDeltaDvDy = srcPreShrinkDeltaY;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlaySize =
                asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 1) & ~1), dwHeight);

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayFormat =
                (NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | dwPitch;

            if (dwFieldFlags & DDFLIP_ODD)
                npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayOffset =
                    (dwOffset + (dwPitch >> 1));
            else
                npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayOffset =
                    dwOffset;

            /* Restore offset */
            dwOffset -= (pDriverData->OverlaySrcY * dwPitch);
            dwOffset -= ((pDriverData->OverlaySrcX & ~1) << 1);

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayPoint = 0;

            pDriverData->dDrawSpareSubchannelObject = 0;

            /* Change offset to point to shrunken overlay surface memory */
            if ((surfaceIs422) && (pDriverData->extra422OverlayOffset[index] != 0))
                dwOffset = pDriverData->extra422OverlayOffset[index];
            else
                dwOffset += pSurfTarg_gbl->dwReserved1;

            /* Restore overlay source pitch and height to normal since we prescaled it */
            if (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD) {
                dwPitch >>= 1;
                dwHeight <<= 1;
            }

            /* Force fifo to stall until shrink operation finishes */
            npDevVideo->dDrawSpare.nv4DvdSubpicture.Notify = 0;
            npDevVideo->dDrawSpare.nv4DvdSubpicture.NoOperation = 0;
        } else {
            /* Offset to visible portion of source surface */
            if (pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) {
                dwOffset += (pDriverData->OverlaySrcY * dwPitch);
                dwOffset += ((pDriverData->OverlaySrcX & ~1) << 1);
            }
        }

        /* This actually refers to the video channel in this case */
        pDriverData->dDrawSpareSubchannelObject = 0;

        /* This should never wait unless there is a hardware glitch */
        while ((volatile)pPioFlipOverlayNotifier[index + 1].status == NV_IN_PROGRESS);

        pPioFlipOverlayNotifier[index + 1].status = NV_IN_PROGRESS;

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {
            DWORD dwPointIn;
            DWORD dwOverlayFormat;

            while (videoFreeCount < 40) {
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
                if (videoFreeCount < 40)
                    NV_SLEEP;
            }

            /* Trash spare subchannel */
            npDevVideo->subchannel[NV_DD_SPARE].SetObject = NV_VIDEO_OVERLAY;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].colorKey =
                pDriverData->OverlayColorKey;

            if ((!(surfaceIs422)) && (dwFieldFlags & DDFLIP_ODD))
                dwOffset += dwPitch >> 1;

            /* Remember this in case we get a SetOverlayPosition call */
            pDriverData->OverlaySrcOffset = dwOffset;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].offset =
                dwOffset;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].sizeIn =
                (pDriverData->OverlaySrcSize & ~0x00010001);

            if (preScaleOverlay == 0) {
                dwPointIn = asmMergeCoords((pDriverData->OverlaySrcX << 4), (pDriverData->OverlaySrcY << 4));
            } else
                dwPointIn = 0;

            /* Offset source starting Y by -0.5 when bobing */
            if (doBob)
                dwPointIn += 0xFFF80000;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].pointIn = dwPointIn;

            if ( !(pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) ) {
                npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].duDx =
                    pDriverData->dwOverlayDeltaX;
                npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].dvDy =
                    dwOverlayDeltaY;
            }

            npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].pointOut =
                asmMergeCoords(pDriverData->OverlayDstX, pDriverData->OverlayDstY);

            if (preScaleOverlay) {
                npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].sizeOut =
                    asmMergeCoords(pDriverData->dwOverlayDstWidth, pDriverData->dwOverlayDstHeight);
            } else {
                npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].sizeOut =
                    asmMergeCoords(pSurfTarg_gbl->wWidth, dwHeight);
            }

            dwOverlayFormat = pDriverData->OverlayFormat;

            /* replace old surface pitch value with current pitch value */
            dwOverlayFormat &= 0xFFFF0000;

            dwOverlayFormat |= dwPitch;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.SetOverlay[index].format = dwOverlayFormat;

        } else { // pre NV10

            while (videoFreeCount < 32) {
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
                if (videoFreeCount < 32)
                    NV_SLEEP;
            }

            /* Trash spare subchannel */
            npDevVideo->subchannel[NV_DD_SPARE].SetObject = NV_DD_VIDEO_SCALER;

            if ( !(pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) )
                    npDevVideo->subchannel[NV_DD_SPARE].videoScaler.SetDeltaDvDy = dwOverlayDeltaY;

            /* Temporarily load VIDEO_FROM_MEMORY object */
            npDevVideo->subchannel[NV_DD_SPARE].SetObject =
                NV_DD_YUV422_VIDEO_FROM_MEMORY;

            if ((!(surfaceIs422)) && (dwFieldFlags & DDFLIP_ODD))
                dwOffset += dwPitch >> 1;

            /* Remember this in case we get a SetOverlayPosition call */
            pDriverData->OverlaySrcOffset = dwOffset;

            npDevVideo->subchannel[NV_DD_SPARE].videoFromMemory.ImageScan[index].offset =
                dwOffset;

            npDevVideo->subchannel[NV_DD_SPARE].videoFromMemory.ImageScan[index].pitch =
                (dwPitch | doBob);

                if (preScaleOverlay) {
                npDevVideo->subchannel[NV_DD_SPARE].videoFromMemory.ImageScan[index].size =
                    asmMergeCoords(pDriverData->dwOverlayDstWidth,
                                   pDriverData->dwOverlayDstHeight);
                } else {
                npDevVideo->subchannel[NV_DD_SPARE].videoFromMemory.ImageScan[index].size =
                    asmMergeCoords(pSurfTarg_gbl->wWidth, dwHeight);
                }

            if (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY)
                npDevVideo->subchannel[NV_DD_SPARE].videoFromMemory.ImageScan[index].format =
                    NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;
            else
                npDevVideo->subchannel[NV_DD_SPARE].videoFromMemory.ImageScan[index].format =
                    NV_VFM_FORMAT_COLOR_LE_V8Y8U8Y8;

            npDevVideo->subchannel[NV_DD_SPARE].videoFromMemory.ImageScan[index].notify =
                0;
        }

        pDriverData->NvVideoFreeCount = (short)NvGetFreeCount(npDevVideo, NV_DD_SPARE);

    } else { /* Video port flips handled by FlipVideoPort32 */
        pfd->ddRVal = DDERR_INVALIDSURFACETYPE;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * D3D will not wait for flip to complete - so much for ddraw semantics
     */
    if (!bIsD3D && !(pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_OVERLAY)) {
        if (pfd->dwFlags & DDFLIP_WAIT) {
            ddrval = updateFlipStatus(0);

            while (ddrval != DD_OK) {
                NV_SLEEPFLIP;
                ddrval = updateFlipStatus(0);
            }
        }
    }

    pfd->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

} /* Flip32 */



/*
 * Lock32
 */
DWORD __stdcall Lock32( LPDDHAL_LOCKDATA lpLockData )
{
    HRESULT                                     ddrval;
    FAST Nv10ControlDma         *npDev;
    NvNotification                              *pDmaFlipPrimaryNotifier;
    NvNotification                              *pPioFlipOverlayNotifier;
    NvNotification                              *pDmaSyncNotifier;
    NvNotification                              *pDmaDmaFromMemNotifier;
    DWORD                                       dwOffset;
        DWORD                                           caps;
    LPDDRAWI_DDRAWSURFACE_GBL   psurf_gbl;
    unsigned long               *dmaPusherPutAddress;
    unsigned long               *dmaPusherBufferBase;
    long                        freeCount;

    NvSetDriverDataPtr(lpLockData->lpDD);
    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

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

    caps = lpLockData->lpDDSurface->ddsCaps.dwCaps;

    /*
     * Reset NV and get the monitor frequency after a mode reset
     */
    if( pDriverData->fReset ) {
        NV_DD_DMA_PUSHER_SYNC();
        getDisplayDuration();
        if (!ResetNV()) {
            lpLockData->ddRVal = DDERR_GENERIC;
            return(DDHAL_DRIVER_HANDLED);
        }
        pDriverData->fReset = FALSE;
    }

    pDmaFlipPrimaryNotifier = (NvNotification *)pDriverData->NvDmaFlipPrimaryNotifierFlat;
    pPioFlipOverlayNotifier = (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;
    pDmaSyncNotifier = (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
    pDmaDmaFromMemNotifier = (NvNotification *)pDriverData->NvDmaDmaFromMemNotifierFlat;
    dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;
    dmaPusherBufferBase = (unsigned long *)pDriverData->NvDmaPusherBufferBase;

    if ((npDev == NULL) && (caps & DDSCAPS_VIDEOMEMORY)) {  /* Mode switches might occur without warning */
        lpLockData->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
    }

    psurf_gbl = lpLockData->lpDDSurface->lpGbl;

    /*
     * check to see if any pending physical flip has occurred
     */


    if ((caps & DDSCAPS_OVERLAY) == 0) { /* Only wait for RGB surfaces */
        ddrval = updateFlipStatus(psurf_gbl->fpVidMem);
        if( ddrval != DD_OK ) {
            if (lpLockData->dwFlags & DDLOCK_WAIT) {
                while (ddrval != DD_OK) {
                    long countDown = 1000;  // sometimes we can loose a Flip notification
                    // do not sleep on non-overlay locks cuz OpenGL does alot of locks
                    // NV_SLEEPFLIP;
                    ddrval = updateFlipStatus(psurf_gbl->fpVidMem);
                    if (--countDown <= 0) {
                        pDmaFlipPrimaryNotifier[1].status = 0;
                        pDmaFlipPrimaryNotifier[2].status = 0;
                    }
                }
            } else {
                lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
                return DDHAL_DRIVER_HANDLED;
            }
        }
    }

    /*
     * don't allow a lock if a blt is in progress (only do this if your
     * hardware requires it)
     */

//    if(DRAW_ENGINE_BUSY)
//    {
//        lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
//        return DDHAL_DRIVER_HANDLED;
//    }

    if ((pDmaDmaFromMemNotifier->status == NV_IN_PROGRESS) &&
        (pDriverData->NvDmaFromMemSurfaceMem == psurf_gbl->fpVidMem)) {
        lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
        return DDHAL_DRIVER_HANDLED;
    }

    if ((caps & DDSCAPS_VIDEOMEMORY) && ((caps & DDSCAPS_TEXTURE) == 0)) {

        // No need to lock textures even though they may currently be in use because
        //     their surface memory is not used directly when rendering
        // Every other surface type with DDSCAPS_VIDMEMORY set does need locked however


        if ((lpLockData->dwFlags & DDLOCK_WAIT) || // workaround for Telurian Defence bug
            ((lpLockData->dwFlags & DDLOCK_READONLY) == 0)) {

            NV_DD_DMA_PUSHER_SYNC();

            // Give the application a chance to do something else if push buffer isn't empty yet
            if (npDev->Get != (unsigned long)(dmaPusherPutAddress - dmaPusherBufferBase) << 2) {
                /* Force write combine buffer to flush */
                pDriverData->NvDmaPusherBufferEnd[0] = 0;
                /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                _outp (0x3d0,0);
                npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
                lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
                return DDHAL_DRIVER_HANDLED;
            }

            freeCount = 0; /* Force a hardware read */

            /* Let D3D code know that we have touched NV */
            pDriverData->TwoDRenderingOccurred = 1;

            // MUST wait for any pending notification to prevent possible loss of notification serialization
            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS);

            while (freeCount < 4)
                NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
            freeCount -= 4;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = 0;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = 0;

            dmaPusherPutAddress += 4;

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            pDriverData->dwDmaPusherFreeCount = freeCount;

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS);

            if (caps & DDSCAPS_VISIBLE) {
                pDriverData->fVidMemLocked = TRUE;
            }
        }
    }

    /*
     * Signal that the texture surface has been locked.  This means that the
     * texture has possibly been modified which means it needs to be retranslated.
     * This needs to be done no matter where the texture was allocated.
     */
    if (caps & DDSCAPS_TEXTURE) {
        PNVD3DTEXTURE   pTexture;
#ifdef NV_TEX2
        /*
         * user wants to lock a texture - process elsewhere
         */
        if (pTexture = (PNVD3DTEXTURE)lpLockData->lpDDSurface->dwReserved1)
        {
#ifdef WINNT
            DWORD retval;
            pTexture->lpLcl = lpLockData->lpDDSurface;
            retval = nvTextureLock(pTexture,lpLockData);
            pTexture->lpLcl = NULL;
            return(retval);
#else
            return(nvTextureLock(pTexture,lpLockData));
#endif // #ifndef WINNT
        }
#else // NV_TEX2
       if (pTexture = (PNVD3DTEXTURE)lpLockData->lpDDSurface->dwReserved1)
        {
            /*
             * When the application is locking and touching the texture directly, it's
             * neccessary to allocate a seperate block of system memory for it to use
             * since the texture memory used by the hardware stores the texture in a
             * different format than what the application understands.
             */
#ifdef WINNT
            pTexture->lpLcl = lpLockData->lpDDSurface;
            nvAllocateUserTexture(pTexture);
            pTexture->dwTextureFlags |= (NV3_TEXTURE_MODIFIED | NV3_TEXTURE_IS_LOCKED);
            lpLockData->lpSurfData = pTexture->lpLcl->lpGbl->fpVidMem;
            pTexture->lpLcl = NULL;
            lpLockData->ddRVal = DD_OK;
            return(DDHAL_DRIVER_HANDLED);
#else
            nvAllocateUserTexture(pTexture);
#endif // #ifdef WINNT
        }
#endif // NV_TEX2
    }

    /* Currently works only for pDriverData->bi.biBitCount pixel depth surfaces */
    if (lpLockData->bHasRect) {
        dwOffset = lpLockData->rArea.top * psurf_gbl->lPitch;
        dwOffset += (lpLockData->rArea.left * (pDriverData->bi.biBitCount >> 3));
        lpLockData->lpSurfData = (VOID *)((DWORD)psurf_gbl->fpVidMem + dwOffset);
    }

    return DDHAL_DRIVER_NOTHANDLED;

} /* Lock32 */


/*
 * Unlock32
 */
DWORD __stdcall Unlock32( LPDDHAL_UNLOCKDATA lpUnlockData )
{
    Nv10ControlDma              *npDev;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    DWORD                       caps;

    NvSetDriverDataPtr(lpUnlockData->lpDD);

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

    caps = lpUnlockData->lpDDSurface->ddsCaps.dwCaps;

    if ((npDev == NULL) && (caps & DDSCAPS_VIDEOMEMORY)) {  /* Mode switches might occur without warning */
        lpUnlockData->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
    }

    pSurf_gbl = lpUnlockData->lpDDSurface->lpGbl;

    if ((caps & DDSCAPS_OVERLAY) && ((caps & DDSCAPS_VISIBLE) ||
        (pDriverData->dwOverlaySurfaces < 3))) {
        ConvertOverlay32(lpUnlockData->lpDDSurface, FALSE);
        if ((caps & DDSCAPS_VISIBLE) && ((caps & DDSCAPS_VIDEOPORT) == 0)) {
            if ((pDriverData->dwOverlaySurfaces == 1) && (pSurf_gbl->wHeight >= 480)) { // Quadrant MCI DVD decoder fix
                if (pDriverData->lockCount > 2)
                    PreScaleOverlay32(lpUnlockData->lpDDSurface);
            } else if (((pDriverData->dwOverlaySrcWidth > pDriverData->dwOverlayDstWidth) ||
                (pDriverData->dwOverlaySrcHeight > pDriverData->dwOverlayDstHeight))) {
                PreScaleOverlay32(lpUnlockData->lpDDSurface);
            }
        }
        pDriverData->lockCount++;
    }

    /*
     * Signal that the texture surface has been unlocked.  This means that the
     * texture has possibly been modified which means it needs to be retranslated.
     * This needs to be done no matter where the texture was allocated.
     */
    if (caps & DDSCAPS_TEXTURE) {
        PNVD3DTEXTURE   pTexture;
#ifdef NV_TEX2
        /*
         * user wants to unlock a texture - process elsewhere
         */
        if (pTexture = (PNVD3DTEXTURE)lpUnlockData->lpDDSurface->dwReserved1)
        {
#ifdef WINNT
            DWORD retval;
            pTexture->lpLcl = lpUnlockData->lpDDSurface;
            pDriverData->fVidMemLocked = FALSE;
            retval = nvTextureUnlock(pTexture,lpUnlockData);
            pTexture->lpLcl = NULL;
            return(retval);
#else
            pDriverData->fVidMemLocked = FALSE;
            return(nvTextureUnlock(pTexture,lpUnlockData));
#endif // #ifndef WINNT
        }
#else // NV_TEX2
        pTexture = (PNVD3DTEXTURE)lpUnlockData->lpDDSurface->dwReserved1;
        if (pTexture) {
            pTexture->dwTextureFlags |= NV3_TEXTURE_MODIFIED;
            pTexture->dwTextureFlags &= ~NV3_TEXTURE_IS_LOCKED;

            /*
             * Swizzle the texture now.
             */
#ifdef WINNT
            pTexture->lpLcl = lpUnlockData->lpDDSurface;
            nvLoadTexture(pTexture);
            pTexture->lpLcl = NULL;
#else
            nvLoadTexture(pTexture);
#endif // #ifndef WINNT

        }
#endif // NV_TEX2
    }

    pDriverData->fVidMemLocked = FALSE;

    lpUnlockData->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;

} /* Unlock32 */




/*
 * WaitForVerticalBlank32
 */
DWORD __stdcall WaitForVerticalBlank32(
        LPDDHAL_WAITFORVERTICALBLANKDATA lpWaitForVerticalBlank )
{
    NvSetDriverDataPtr(lpWaitForVerticalBlank->lpDD);

    switch( lpWaitForVerticalBlank->dwFlags ) {

    case DDWAITVB_I_TESTVB:
        /*
         * if testvb is just a request for the current vertical blank status
         */
        lpWaitForVerticalBlank->ddRVal = DD_OK;
#ifndef WINNT
        if (NvConfigGet(NV_CFG_VBLANK_STATUS, pDriverData->dwDeviceIDNum))
#else
        if (pDriverData->ppdev->pfnVBlankIsActive(pDriverData->ppdev))
#endif // #ifdef WINNT
           lpWaitForVerticalBlank->bIsInVB = TRUE;
        else
           lpWaitForVerticalBlank->bIsInVB = FALSE;

        return DDHAL_DRIVER_HANDLED;

    case DDWAITVB_BLOCKBEGIN:
        /*
         * if blockbegin is requested we wait until the vertical blank
         * is over, and then wait for the display period to end.
         */
#ifndef WINNT
        while(NvConfigGet(NV_CFG_VBLANK_STATUS, pDriverData->dwDeviceIDNum) != 0);
        while(NvConfigGet(NV_CFG_VBLANK_STATUS, pDriverData->dwDeviceIDNum) == 0);
#else
        pDriverData->ppdev->pfnWaitWhileVBlankActive(pDriverData->ppdev);
        pDriverData->ppdev->pfnWaitWhileDisplayActive(pDriverData->ppdev);
#endif // #ifdef WINNT
        lpWaitForVerticalBlank->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;

    case DDWAITVB_BLOCKEND:
        /*
         * if blockend is requested we wait for the vblank interval to end.
         */
#ifndef WINNT
        if (NvConfigGet(NV_CFG_VBLANK_STATUS, pDriverData->dwDeviceIDNum)) {
            while(NvConfigGet(NV_CFG_VBLANK_STATUS, pDriverData->dwDeviceIDNum) != 0);
        } else {
            while(NvConfigGet(NV_CFG_VBLANK_STATUS, pDriverData->dwDeviceIDNum) == 0);
            while(NvConfigGet(NV_CFG_VBLANK_STATUS, pDriverData->dwDeviceIDNum) != 0);
        }
#else
        if (pDriverData->ppdev->pfnVBlankIsActive(pDriverData->ppdev)) {
            pDriverData->ppdev->pfnWaitWhileVBlankActive(pDriverData->ppdev);
        } else {
            pDriverData->ppdev->pfnWaitWhileDisplayActive(pDriverData->ppdev);
            pDriverData->ppdev->pfnWaitWhileVBlankActive(pDriverData->ppdev);
        }
#endif // #ifdef WINNT
        lpWaitForVerticalBlank->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

    return DDHAL_DRIVER_NOTHANDLED;

} /* WaitForVerticalBlank32 */



/*
 * GetScanLine32
 */
DWORD __stdcall GetScanLine32( LPDDHAL_GETSCANLINEDATA lpGetScanLine )
{
    NvSetDriverDataPtr(lpGetScanLine->lpDD);

    /*
     * If a vertical blank is in progress the scan line is in
     * indeterminant. If the scan line is indeterminant we return
     * the error code DDERR_VERTICALBLANKINPROGRESS.
     * Otherwise we return the scan line and a success code
     */
#ifndef WINNT
    lpGetScanLine->dwScanLine = NvConfigGet(NV_CFG_CURRENT_SCANLINE, pDriverData->dwDeviceIDNum);
#else
    lpGetScanLine->dwScanLine = pDriverData->ppdev->pfnGetScanline(pDriverData->ppdev);
#endif // #ifdef WINNT

    if (lpGetScanLine->dwScanLine >= (DWORD)pDriverData->bi.biHeight)
        lpGetScanLine->ddRVal = DDERR_VERTICALBLANKINPROGRESS;
    else
        lpGetScanLine->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;

} /* GetScanLine32 */



/*
 * SetSurfaceColorKey32
 */
DWORD __stdcall SetSurfaceColorKey32(LPDDHAL_SETCOLORKEYDATA lpSetColorKey)
{
    LPDDRAWI_DIRECTDRAW_GBL     pdrv;
    LPDDRAWI_DDRAWSURFACE_LCL   psurf;

    NvSetDriverDataPtr(lpSetColorKey->lpDD);

    pdrv = lpSetColorKey->lpDD;
    psurf = lpSetColorKey->lpDDSurface;

    DPF( "in SetSurfaceColorKey, lpSetColorKey->lpDD=%08lx, lpSetColorKey->lpDDSurface = %08lx",
                pdrv, lpSetColorKey->lpDDSurface );

    if (lpSetColorKey->dwFlags == DDCKEY_SRCBLT) {
        psurf->ddckCKSrcBlt.dwColorSpaceLowValue =
            lpSetColorKey->ckNew.dwColorSpaceLowValue & pDriverData->physicalColorMask;
        /* Use no mask on this one on NV3 so that our D3D driver can get all the bits when surface is 565 */
        psurf->ddckCKSrcBlt.dwColorSpaceHighValue =
            lpSetColorKey->ckNew.dwColorSpaceHighValue;
    } else if (lpSetColorKey->dwFlags == DDCKEY_DESTOVERLAY) {
        psurf->ddckCKDestOverlay.dwColorSpaceLowValue =
            lpSetColorKey->ckNew.dwColorSpaceLowValue & pDriverData->physicalColorMask;
        psurf->ddckCKDestOverlay.dwColorSpaceHighValue =
            lpSetColorKey->ckNew.dwColorSpaceHighValue & pDriverData->physicalColorMask;
    } else {
        lpSetColorKey->ddRVal = DDERR_UNSUPPORTED;
        return DDHAL_DRIVER_HANDLED;
    }


    lpSetColorKey->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;
}


/*
 * DestroySurface32
 */
DWORD __stdcall DestroySurface32( LPDDHAL_DESTROYSURFACEDATA lpDestroySurface )
{
    FAST Nv10ControlDma          *npDev;
    LPDDRAWI_DIRECTDRAW_GBL     pdrv;
    LPDDRAWI_DDRAWSURFACE_LCL   psurf;
    LPDDRAWI_DDRAWSURFACE_GBL   psurf_gbl;
    HGLOBAL                     hMem;
    DWORD                       index = 0;

    NvSetDriverDataPtr(lpDestroySurface->lpDD);

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

    pdrv = lpDestroySurface->lpDD;
    DPF( "in DestroySurface, lpDestroySurface->lpDD=%08lx, lpDestroySurface->lpDDSurface = %08lx",
                pdrv, lpDestroySurface->lpDDSurface );

    psurf = lpDestroySurface->lpDDSurface;
    psurf_gbl = psurf->lpGbl;

    if (psurf_gbl->fpVidMem == pDriverData->BaseAddress) {
        lpDestroySurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
       Should check if YUV surface is an active VPE surface and make sure
       to stop VPE hardware before destroying VPE surface somewhere in YUV
       surface code in this function.
    */

    if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (psurf->ddsCaps.dwCaps & (DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER))) {
        FAST Nv3ChannelPio *npDevVideo = (Nv3ChannelPio *)pDriverData->NvDevVideoFlatPio;
        FAST long          videoFreeCount = pDriverData->NvVideoFreeCount;
        NvNotification     *pPioFlipOverlayNotifier =
                            (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;

        pDriverData->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;

        NV_DD_DMA_PUSHER_SYNC();

        if (npDevVideo == NULL) {  /* Mode switches might occur without warning */
            if (!ResetNV()) {
                lpDestroySurface->ddRVal = DDERR_GENERIC;
                return DDHAL_DRIVER_HANDLED;
            }
            pDriverData->fReset = FALSE;
            npDevVideo = (Nv3ChannelPio *)pDriverData->NvDevVideoFlatPio;
            videoFreeCount = pDriverData->NvVideoFreeCount;
            npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
            if ((npDevVideo == NULL) || (npDev == NULL))  {
                pDriverData->dwOverlaySurfaces = 0;
                pDriverData->dwOverlayOwner = 0;
                lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                return DDHAL_DRIVER_HANDLED;
            }
        }

        pDriverData->dwRingZeroMutex = TRUE;

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

            while (videoFreeCount < 12)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 12;

            npDevVideo->dDrawSpare.SetObject = NV_VIDEO_OVERLAY;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.StopOverlay[0] =
                NV07A_STOP_OVERLAY_BETWEEN_BUFFERS;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.StopOverlay[1] =
                NV07A_STOP_OVERLAY_BETWEEN_BUFFERS;

        } else {

            while (videoFreeCount < 20)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 20;

            /* Trash spare subchannel */
            npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_COLOR_KEY;

            npDevVideo->dDrawSpare.videoColorKey.SetSize = 0; // hide overlay

            npDevVideo->dDrawSpare.SetObject = NV_DD_YUV422_VIDEO_FROM_MEMORY;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[0].size = 0;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[0].notify = 0;
        }

        pDriverData->dwRingZeroMutex = FALSE;
        pDriverData->dDrawSpareSubchannelObject = 0;

        pDriverData->NvVideoFreeCount = (short)videoFreeCount;

        /* Make sure we don't get confused later on */
        pPioFlipOverlayNotifier[1].status = 0;      /* clear notifiers */
        pPioFlipOverlayNotifier[2].status = 0;
        pDriverData->fDDrawFullScreenDosOccurred = 0;    /* reset flag */
    }

    if( (psurf->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
         (psurf_gbl->ddpfSurface.dwFlags & DDPF_FOURCC) ) {
             DPF( "  FOURCC surface" );

        switch( psurf->lpGbl->ddpfSurface.dwFourCC ) {
            case FOURCC_RGB0:
            case FOURCC_RAW8:
               if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
                   if (psurf_gbl->fpVidMem > 0) {
                       NVHEAP_FREE (psurf_gbl->fpVidMem);
                       //DDHAL32_VidMemFree(pdrv, 0, psurf_gbl->fpVidMem);
                       psurf_gbl->fpVidMem = 0;
                       if (pDriverData->DDrawVideoSurfaceCount > 0)
                           pDriverData->DDrawVideoSurfaceCount--;
                   }
               } else {
                   hMem = (HGLOBAL)psurf_gbl->dwReserved1;
                   GlobalUnlock(hMem);
                   GlobalFree(hMem);
                   psurf_gbl->dwReserved1 = 0;
               }
               lpDestroySurface->ddRVal = DD_OK;
               return DDHAL_DRIVER_HANDLED;
               break;

            case FOURCC_IF09:
            case FOURCC_YVU9:
            case FOURCC_IV32:
            case FOURCC_IV31:
            case FOURCC_YUY2:
            case FOURCC_YUNV:
            case FOURCC_UYVY:
            case FOURCC_UYNV:

               if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                   while (index < NV_MAX_OVERLAY_SURFACES) {
                       if ((DWORD)psurf == pDriverData->dwOverlaySurfaceLCL[index]) {
                           pDriverData->dwOverlaySurfaceLCL[index] = 0;
                           if (pDriverData->dwOverlaySurfaces > 0)
                               pDriverData->dwOverlaySurfaces--;
                           if (pDriverData->dwOverlaySurfaces == 0) {
                               pDriverData->dwOverlayOwner = 0;
                               pDriverData->extra422OverlayOffset[0] = 0;
                               pDriverData->extra422OverlayOffset[1] = 0;
                           }
                       }
                       index++;
                   }
               }

               if ((psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) &&
                   ((psurf->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) == 0)) {
                   if (psurf_gbl->fpVidMem > 0) {
                       if (psurf_gbl->fpVidMem + psurf_gbl->dwReserved1 - pDriverData->BaseAddress ==
                           pDriverData->extra422OverlayOffset[0]) {
                           pDriverData->extra422OverlayOffset[0] = 0;
                           pDriverData->extra422OverlayOffset[1] = 0;
                       }
                       NVHEAP_FREE (psurf_gbl->fpVidMem);
                       //DDHAL32_VidMemFree(pdrv, 0, psurf_gbl->fpVidMem);
                       psurf_gbl->fpVidMem = 0;
                       if (pDriverData->DDrawVideoSurfaceCount > 0)
                           pDriverData->DDrawVideoSurfaceCount--;
                   }
               } else {
                   if ((pDriverData->fNvActiveFloatingContexts) &&
                       ((psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
                        (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYNV) ||
                        (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_YUY2) ||
                        (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_YUNV))) {

                       if (npDev == NULL) {  /* Mode switches might occur without warning */
                           lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                           return DDHAL_DRIVER_HANDLED;
                       }

                       DestroyFloating422Context32(psurf);
                   }

                   hMem = (HGLOBAL)psurf_gbl->dwReserved1;
                   GlobalUnlock(hMem);
                   GlobalFree(hMem);
               }
               psurf_gbl->dwReserved1 = 0;
               lpDestroySurface->ddRVal = DD_OK;
               return DDHAL_DRIVER_HANDLED;
               break;
            case FOURCC_YV12:
            case FOURCC_420i:
               if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                   while (index < NV_MAX_OVERLAY_SURFACES) {
                       if ((DWORD)psurf == pDriverData->dwOverlaySurfaceLCL[index]) {
                           pDriverData->dwOverlaySurfaceLCL[index] = 0;
                           if (pDriverData->dwOverlaySurfaces > 0)
                               pDriverData->dwOverlaySurfaces--;
                           if (pDriverData->dwOverlaySurfaces == 0) {
                               pDriverData->dwOverlayOwner = 0;
                           }
                       }
                       index++;
                   }
               }

               if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                   (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)) {
                   if (psurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0]) {

                       if (pDriverData->NvYUY2Surface0Mem > 0) {
                           NVHEAP_FREE (pDriverData->NvYUY2Surface0Mem);
                           //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface0Mem);
                           pDriverData->NvYUY2Surface0Mem = 0;
                           if (pDriverData->DDrawVideoSurfaceCount > 0)
                               pDriverData->DDrawVideoSurfaceCount--;
                       }

                       NV_DD_DMA_PUSHER_SYNC();

                       if (npDev == NULL) {  /* Mode switches might occur without warning */
                           lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                           return DDHAL_DRIVER_HANDLED;
                       }

                       /* If AGP memory then send a sync rectangle to make sure we are done with it */
                       if (psurf->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
                           NvNotification *pDmaSyncNotifier =
                               (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
                           unsigned long *dmaPusherPutAddress =
                               (unsigned long *)pDriverData->NvDmaPusherPutAddress;
                           unsigned long *dmaPusherBufferBase =
                               (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
                           long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
                           NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
                           long freeCount = 0;
#endif  /* CACHE_FREECOUNT */

                           /* Let D3D code know that we have touched NV */
                           pDriverData->TwoDRenderingOccurred = 1;

                           // MUST wait for any pending notification to prevent possible loss of notification serialization
                           while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

                           while (freeCount < 4)
                               NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                           freeCount -= 4;

                           pDmaSyncNotifier->status = NV_IN_PROGRESS;

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                               RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
                           dmaPusherPutAddress[1] = 0;
                           dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                               RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
                           dmaPusherPutAddress[3] = 0;

                           dmaPusherPutAddress += 4;

                           pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                           /* Force write combine buffer to flush */
                           pDriverData->NvDmaPusherBufferEnd[0] = 0;
                           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                           _outp (0x3d0,0);

                           pDriverData->dwDmaPusherFreeCount = freeCount;

                           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                           while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification
                       }

                       if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] <
                            pDriverData->BaseAddress) || /* If system memory */
                           (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >
                           (unsigned long)pDriverData->VideoHeapEnd)) {
                           NvNotification *pDmaSyncNotifier =
                               (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
                           unsigned long *dmaPusherPutAddress =
                               (unsigned long *)pDriverData->NvDmaPusherPutAddress;
                           unsigned long *dmaPusherBufferBase =
                               (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
                           long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
                           NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
                           long freeCount = 0;
#endif  /* CACHE_FREECOUNT */

                           /* Let D3D code know that we have touched NV */
                           pDriverData->TwoDRenderingOccurred = 1;

                           // MUST wait for any pending notification to prevent possible loss of notification serialization
                           while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

                           while (freeCount < 28)
                               NvGetDmaBufferFreeCount(npDev, freeCount, 28, dmaPusherPutAddress);
                           freeCount -= 28;

                           pDriverData->dDrawSpareSubchannelObject = 0;

                           pDmaSyncNotifier->status = NV_IN_PROGRESS;

                           /* Trash spare subchannel */
                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                           dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
                           dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                           dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                           dmaPusherPutAddress[5] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                           dmaPusherPutAddress[7] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
                           dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                           dmaPusherPutAddress[9] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                           dmaPusherPutAddress[11] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[12] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                           dmaPusherPutAddress[13] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
                           dmaPusherPutAddress[14] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                           dmaPusherPutAddress[15] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[16] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                           dmaPusherPutAddress[17] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[18] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                           dmaPusherPutAddress[19] = NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
                           dmaPusherPutAddress[20] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                           dmaPusherPutAddress[21] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[22] = dDrawSubchannelOffset(NV_DD_SPARE) +
                               MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                           dmaPusherPutAddress[23] = NV1_NULL_OBJECT;
                           dmaPusherPutAddress[24] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                               RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
                           dmaPusherPutAddress[25] = 0;
                           dmaPusherPutAddress[26] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                               RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
                           dmaPusherPutAddress[27] = 0;

                           dmaPusherPutAddress += 28;

                           pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                           /* Force write combine buffer to flush */
                           pDriverData->NvDmaPusherBufferEnd[0] = 0;
                           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                           _outp (0x3d0,0);

                           pDriverData->dwDmaPusherFreeCount = freeCount;

                           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                           while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

                           NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                                    NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
                       }

                        /* If in AGP memory then deallocate it from there */
                       if (psurf->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) {
                           DDHAL32_VidMemFree(pdrv, AGP_HEAP, psurf_gbl->fpVidMem);
                       } else {
                           if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] <
                                pDriverData->BaseAddress) || /* If system memory */
                               (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >
                               (unsigned long)pDriverData->VideoHeapEnd)) {
                               GlobalUnlock((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
                               GlobalFree((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
                           } else {
                               NVHEAP_FREE (psurf_gbl->fpVidMem);
                               //DDHAL32_VidMemFree(pdrv, 0, psurf_gbl->fpVidMem);
                               if (pDriverData->DDrawVideoSurfaceCount > 0)
                                   pDriverData->DDrawVideoSurfaceCount--;
                           }
                       }

                       pDriverData->NvFloatingMem2MemFormatMemoryHandle = 0;

                       pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] = 0;
                       pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
                       pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
                       pDriverData->NvFloatingMem2MemFormatSurfaceAddress[3] = 0;

                   } else {
                       if (psurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1]) {
                           pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
                           if (pDriverData->NvYUY2Surface1Mem > 0) {
                               NVHEAP_FREE (pDriverData->NvYUY2Surface1Mem);
                               //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface1Mem);
                               pDriverData->NvYUY2Surface1Mem = 0;
                               if (pDriverData->DDrawVideoSurfaceCount > 0)
                                   pDriverData->DDrawVideoSurfaceCount--;
                           }
                       } else if (psurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2]) {
                           pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
                           if (pDriverData->NvYUY2Surface2Mem > 0) {
                               NVHEAP_FREE (pDriverData->NvYUY2Surface2Mem);
                               //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface2Mem);
                               pDriverData->NvYUY2Surface2Mem = 0;
                               if (pDriverData->DDrawVideoSurfaceCount > 0)
                                   pDriverData->DDrawVideoSurfaceCount--;
                           }
                       } else {
                           if (pDriverData->NvYUY2Surface3Mem > 0) {
                               NVHEAP_FREE (pDriverData->NvYUY2Surface3Mem);
                               //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface3Mem);
                               pDriverData->NvYUY2Surface3Mem = 0;
                               if (pDriverData->DDrawVideoSurfaceCount > 0)
                                   pDriverData->DDrawVideoSurfaceCount--;
                           }
                       }
                   }
                   psurf_gbl->fpVidMem = 0;
               } else if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
                   if (psurf_gbl->fpVidMem > 0) {
                       NVHEAP_FREE (psurf_gbl->fpVidMem);
                       //DDHAL32_VidMemFree(pdrv, 0, psurf_gbl->fpVidMem);
                       psurf_gbl->fpVidMem = 0;
                       if (pDriverData->DDrawVideoSurfaceCount > 0)
                           pDriverData->DDrawVideoSurfaceCount--;
                   }
               } else {
                   hMem = (HGLOBAL)psurf_gbl->dwReserved1;
                   GlobalUnlock(hMem);
                   GlobalFree(hMem);
               }
               psurf_gbl->dwReserved1 = 0;
               lpDestroySurface->ddRVal = DD_OK;
               return DDHAL_DRIVER_HANDLED;
               break;
        }
    } else if ((psurf->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) &&
        (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)) {
#ifndef WINNT
        if (psurf_gbl->fpVidMem > 0) {
#endif
            if ((psurf->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) &&
               (pDriverData->GARTLinearBase > 0)) {

               DDHAL32_VidMemFree(pdrv, AGP_HEAP, psurf_gbl->fpVidMem); // AGP memory
                } else {
               NVHEAP_FREE (psurf_gbl->fpVidMem);
               //DDHAL32_VidMemFree(pdrv, 0, psurf_gbl->fpVidMem);
               if (pDriverData->DDrawVideoSurfaceCount > 0)
                   pDriverData->DDrawVideoSurfaceCount--;
            }
            psurf_gbl->fpVidMem = 0;
#ifdef WINNT
            psurf_gbl->lPitch = 0;
#else
        }
#endif
        DPF( "    Offscreen deallocated" );
        psurf_gbl->dwReserved1 = 0;
        lpDestroySurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    } else if (psurf->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {
        /*
         * Call the D3D driver to destroy the texture surface.
         */
        nvDestroyTextureSurface(lpDestroySurface, psurf, psurf_gbl);
        return DDHAL_DRIVER_HANDLED;
    } else if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
#ifndef WINNT
        if (psurf_gbl->fpVidMem > 0) {
#endif
            if ((psurf->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM) &&
               (pDriverData->GARTLinearBase > 0))
               {
               DDHAL32_VidMemFree(pdrv, AGP_HEAP, psurf_gbl->fpVidMem); // AGP memory
                } else {
               NVHEAP_FREE (psurf_gbl->fpVidMem);
               //DDHAL32_VidMemFree(pdrv, 0, psurf_gbl->fpVidMem);
               if (pDriverData->DDrawVideoSurfaceCount > 0)
                   pDriverData->DDrawVideoSurfaceCount--;
            }
            psurf_gbl->fpVidMem = 0;
#ifdef WINNT
            psurf_gbl->lPitch = 0;
#else
        } else {
            DPF( "    Unspecified Video Memory deallocation not handled");
            return DDHAL_DRIVER_NOTHANDLED;
        }
#endif
        DPF( "    Offscreen deallocated" );
        psurf_gbl->dwReserved1 = 0;
        lpDestroySurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    } else if (psurf->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {
        DPF( "    System memory deallocated" );
        hMem = (HGLOBAL)psurf_gbl->dwReserved1;
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        psurf_gbl->dwReserved1 = 0;
        lpDestroySurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

    return DDHAL_DRIVER_NOTHANDLED;
}



/*
 * CreateSurface32
 */
DWORD __stdcall CreateSurface32( LPDDHAL_CREATESURFACEDATA lpCreateSurface )
{
    LPDDRAWI_DIRECTDRAW_GBL     pdrv;
    BOOL                        handled;
    int                         i;
    LPDDRAWI_DDRAWSURFACE_LCL   psurf;
    LPDDRAWI_DDRAWSURFACE_LCL   *powner;
    LPDDRAWI_DDRAWSURFACE_GBL   psurf_gbl;
    LONG                        pitch;
    LONG                        adjustedHeight;
    HGLOBAL                     hMem;
    DWORD                       dwBlockSize;
    DWORD                       *fpVidMem;
    HRESULT                     ddrval;
    DWORD                       memType;
    DWORD                       index;
#ifdef DEBUG
    char                        fourCCStr[5];
#endif // DEBUG

    NvSetDriverDataPtr(lpCreateSurface->lpDD);

#if 0 //def WINNT
    /*
     * Under NT, if this is the "first" ddraw surface allocation, move all
     * offscreen DFBs into sys memory to make room for ddraw surfaces.
     */
     if (pDriverData->DDrawVideoSurfaceCount == 0) {
        bMoveAllDfbsFromOffscreenToDibs(pDriverData->ppdev);
        }
#endif

    /*
     * Reset NV and get the monitor frequency after a mode reset
     */
    if( pDriverData->fReset ) {
        getDisplayDuration();
        if (!ResetNV()) {
            lpCreateSurface->ddRVal = DDERR_GENERIC;
            return(DDHAL_DRIVER_HANDLED);
        }
        pDriverData->fReset = FALSE;
    }

    /*
     * is a flip in progress?
     */
    {
        HRESULT ddrval = updateFlipStatus(0);
        while (ddrval == DDERR_WASSTILLDRAWING)
        {
            NV_SLEEPFLIP;
            ddrval = updateFlipStatus(0);
        }
    }

    /*
     * NOTES:
     *
     * This callback is invoked once the surface objects have been created.
     * You can:
     *   - compute the size of the block, by returning
     *     DDHAL_PLEASEALLOC_BLOCKSIZE in fpVidMem, and putting the size
     *     in dwBlockSizeX and dwBlockSizeY
     *   - override some fields in the surface structure, like the pitch.
     *     (you must specifiy the pitch if you are computing the size)
     */

    pdrv = lpCreateSurface->lpDD;
    DPF( "in CreateSurface, lpCreateSurface->lpDD=%08lx, lpCreateSurface->lpDDSurfaceDesc = %08lx",
                pdrv, lpCreateSurface->lpDDSurfaceDesc );
    DPF( "lpCreateSurface->lplpSList=%08lx, lpCreateSurface->dwSCnt = %d", lpCreateSurface->lplpSList, lpCreateSurface->dwSCnt );

    /*
     * see if any of these surfaces are FOURCC codes...
     */
    handled = FALSE;

    powner = lpCreateSurface->lplpSList;

    for( i=0;i<(int)lpCreateSurface->dwSCnt;i++ ) {
        psurf = lpCreateSurface->lplpSList[i];
        psurf_gbl = psurf->lpGbl;

        DPF( "psurf=%08lx, psurf->lpGbl=%08lx", psurf, psurf_gbl );
        if (psurf->dwFlags & DDRAWISURF_HASPIXELFORMAT) {

            if (psurf_gbl->ddpfSurface.dwFlags & DDPF_ALPHAPREMULT) {
                DPF( "  ALPHA_PREMULT surface requested" );

                pitch = psurf_gbl->wWidth *
                        (pDriverData->bi.biBitCount >> 3);
                pitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                dwBlockSize = (pitch * ((DWORD)psurf_gbl->wHeight));
                psurf_gbl->ddpfSurface.dwRGBBitCount = pDriverData->bi.biBitCount;
                psurf_gbl->ddpfSurface.dwRGBAlphaBitMask = 0xFF000000;
                psurf_gbl->ddpfSurface.dwRBitMask = 0x00FF0000;
                psurf_gbl->ddpfSurface.dwGBitMask = 0x0000FF00;
                psurf_gbl->ddpfSurface.dwBBitMask = 0x000000FF;
                psurf_gbl->lPitch = pitch;
                psurf_gbl->fpVidMem = 0;
                psurf_gbl->dwReserved1 = 0;
                handled = TRUE;

                if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

                    /*
                     * Notify 16 bit display driver code that Direct Draw is beginning
                     * to use local video memory.
                     */
#ifndef WINNT
                    if (pDriverData->DDrawVideoSurfaceCount == 0) {
                        HDC hdc = GetDC(0);
                        ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                    }
#endif // #ifndef WINNT

                    NVHEAP_ALLOC(psurf_gbl->fpVidMem, dwBlockSize, TYPE_IMAGE);
                    //psurf_gbl->fpVidMem = DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                    if (psurf_gbl->fpVidMem != 0) {
                        pDriverData->DDrawVideoSurfaceCount++;
                        psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                        psurf->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
                        break;
                    }
                }

                hMem = GlobalAlloc(GHND | GMEM_SHARE, dwBlockSize);
                if (!hMem) {
                    DPF( "    System memory ALPHA_PREMULT surface allocation failed" );
                    lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                    return DDHAL_DRIVER_HANDLED;
                } else {
                    DPF( "    System memory ALPHA_PREMULT surface allocation succeeded" );
                    psurf_gbl->fpVidMem = (FLATPTR)GlobalLock(hMem);
                    psurf_gbl->dwReserved1 = (DWORD)hMem;
                    psurf->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
                }
            }

            if (psurf_gbl->ddpfSurface.dwFlags & DDPF_FOURCC) {
                 DPF( "  FOURCC surface requested" );
                /*
                 * we know the FOURCC code is OK, because we validated it
                 * during CanCreateSurface.   Compute a block size based
                 * on the code, and allocate system memory for it.
                 */
                switch( psurf->lpGbl->ddpfSurface.dwFourCC ) {
                    case FOURCC_RGB0:
                    case FOURCC_RAW8:
                       if (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_RAW8)
                           pitch = psurf_gbl->wWidth;
                       else
                           pitch = psurf_gbl->wWidth *
                               (pDriverData->bi.biBitCount >> 3);
                       pitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                       dwBlockSize = (pitch * ((DWORD)psurf_gbl->wHeight));
                       if (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_RAW8) {
                           psurf_gbl->ddpfSurface.dwRGBBitCount = 8;
                           psurf_gbl->ddpfSurface.dwRBitMask = 0;
                           psurf_gbl->ddpfSurface.dwGBitMask = 0;
                           psurf_gbl->ddpfSurface.dwBBitMask = 0;
                       } else {
                           psurf_gbl->ddpfSurface.dwRGBBitCount = pDriverData->bi.biBitCount;
                           if (pDriverData->bi.biBitCount == 8) {
                               psurf_gbl->ddpfSurface.dwRBitMask = 0;
                               psurf_gbl->ddpfSurface.dwGBitMask = 0;
                               psurf_gbl->ddpfSurface.dwBBitMask = 0;
                               psurf_gbl->ddpfSurface.dwFlags |= DDPF_PALETTEINDEXED8;
                           } else if (pDriverData->bi.biBitCount == 16) {

                               psurf_gbl->ddpfSurface.dwRBitMask = 0x0000F800;
                               psurf_gbl->ddpfSurface.dwGBitMask = 0x000007E0;
                               psurf_gbl->ddpfSurface.dwBBitMask = 0x0000001F;

                           } else if (pDriverData->bi.biBitCount == 32) {
                               psurf_gbl->ddpfSurface.dwRBitMask = 0x00FF0000;
                               psurf_gbl->ddpfSurface.dwGBitMask = 0x0000FF00;
                               psurf_gbl->ddpfSurface.dwBBitMask = 0x000000FF;
                           }
                       }
                       psurf_gbl->ddpfSurface.dwRGBAlphaBitMask = 0;
#ifdef DEBUG
                       fourCCStr[0] = (char)psurf_gbl->ddpfSurface.dwFourCC & 0xFF;
                       fourCCStr[1] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 8) & 0xFF;
                       fourCCStr[2] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 16) & 0xFF;
                       fourCCStr[3] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 24) & 0xFF;
                       fourCCStr[4] = 0;
                       DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG
                       psurf_gbl->lPitch = pitch;
                       psurf_gbl->fpVidMem = 0;
                       psurf_gbl->dwReserved1 = 0;
                       psurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;
                       handled = TRUE;

                       if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

                           /*
                            * Notify 16 bit display driver code that Direct Draw is beginning
                            * to use local video memory.
                            */
#ifndef WINNT
                           if (pDriverData->DDrawVideoSurfaceCount == 0) {
                               HDC hdc = GetDC(0);
                               ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                           }
#endif // #ifndef WINNT

                           NVHEAP_ALLOC(psurf_gbl->fpVidMem, dwBlockSize, TYPE_IMAGE);
                           //psurf_gbl->fpVidMem = DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                           if (psurf_gbl->fpVidMem != 0) {
                               pDriverData->DDrawVideoSurfaceCount++;
                               psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                               psurf->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
                               break;
                           }
                       }

                       hMem = GlobalAlloc(GHND | GMEM_SHARE, dwBlockSize);
                       if (!hMem) {
                           DPF( "    System memory FOURCC surface allocation failed" );
                           lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                           return DDHAL_DRIVER_HANDLED;
                       } else {
                           DPF( "    System memory FOURCC surface allocation succeeded" );
                           psurf_gbl->fpVidMem = (FLATPTR)GlobalLock(hMem);
                           psurf_gbl->dwReserved1 = (DWORD)hMem;
                           psurf->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
                       }
                    break;

                    case FOURCC_IF09:
                    case FOURCC_YVU9:
                    case FOURCC_IV32:
                    case FOURCC_IV31:

                       if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                           (pDriverData->dwOverlaySurfaces > 0)) {
                           if (pDriverData->OverlayRelaxOwnerCheck)
                                pDriverData->dwOverlayOwner = (DWORD)powner;
                           if ((pDriverData->dwOverlayOwner != (DWORD)powner) ||
                               (pDriverData->dwOverlaySurfaces + 1 >= NV_MAX_OVERLAY_SURFACES)) {
                               lpCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                               return DDHAL_DRIVER_HANDLED;
                           }
                       }

                       /* First allocate enough video memory for YUY2 overlay pixel area */
                       pitch = ((psurf_gbl->wWidth + 1) & 0xFFFE) << 1;
                       pitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                       /* pad with an extra scanline */
                       dwBlockSize = (pitch * (DWORD)psurf_gbl->wHeight);
                       /* now allocate enough video memory for the Indeo part of the surface */
                       pitch = psurf_gbl->wWidth;
                       /* allow extra room for block skip bits between Indeo portion of surface and YUY2 portion */
                       dwBlockSize += ((pitch * ((DWORD)psurf_gbl->wHeight * 10L)) >> 3);
                       /* Force block to be properly aligned */
                       dwBlockSize = (dwBlockSize + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

                       if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && (pDriverData->TotalVRAM >> 20 > 4)) {
                           /* Remember offset to 2nd half YUY2 portion of surface */
                           psurf_gbl->dwReserved1 = dwBlockSize;
                           /* Double actual amount of YUY2 video memory allocated for this surface */
                           pitch = ((psurf_gbl->wWidth + 1) & 0xFFFE) << 1;
                           pitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                           /* pad with an extra scanline */
                           dwBlockSize += (pitch * (DWORD)psurf_gbl->wHeight);
                           /* reset pitch to actual Indeo surface pitch */
                           pitch = psurf_gbl->wWidth;
                       } else
                           psurf_gbl->dwReserved1 = 0;

                       psurf_gbl->ddpfSurface.dwYUVBitCount = 9;
                       psurf_gbl->ddpfSurface.dwYBitMask = 0;
                       psurf_gbl->ddpfSurface.dwUBitMask = 0;
                       psurf_gbl->ddpfSurface.dwVBitMask = 0;
                       psurf_gbl->ddpfSurface.dwYUVAlphaBitMask = 0;
#ifdef DEBUG
                       fourCCStr[0] = (char)psurf_gbl->ddpfSurface.dwFourCC & 0xFF;
                       fourCCStr[1] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 8) & 0xFF;
                       fourCCStr[2] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 16) & 0xFF;
                       fourCCStr[3] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 24) & 0xFF;
                       fourCCStr[4] = 0;
                       DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG
                       psurf_gbl->lPitch = pitch;
                       psurf_gbl->fpVidMem = 0;
                       psurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;
                       handled = TRUE;

                       if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

                           /*
                            * Notify 16 bit display driver code that Direct Draw is beginning
                            * to use local video memory.
                            */
#ifndef WINNT
                           if (pDriverData->DDrawVideoSurfaceCount == 0) {
                               HDC hdc = GetDC(0);
                               ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                           }
#endif // #ifndef WINNT

                           NVHEAP_ALLOC(psurf_gbl->fpVidMem, dwBlockSize, TYPE_OVERLAY);
                           //psurf_gbl->fpVidMem = DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                           if (psurf_gbl->fpVidMem != 0) {
                               pDriverData->DDrawVideoSurfaceCount++;
                               psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                               psurf->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;

                               if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                                   index = 0;
                                   while ((pDriverData->dwOverlaySurfaceLCL[index] != 0) &&
                                          (index < NV_MAX_OVERLAY_SURFACES))
                                          index++;
                                   pDriverData->dwOverlaySurfaceLCL[index] = (DWORD) psurf;
                                   pDriverData->dwOverlayOwner = (DWORD)powner;
                                   pDriverData->dwOverlaySurfaces++;
                               }

                               break;
                           } else {
                               /* We can't support overlay surfaces in system memory */
                               if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                                   lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                                   return DDHAL_DRIVER_HANDLED;
                               }
                           }
                       }

                       hMem = GlobalAlloc(GHND | GMEM_SHARE, dwBlockSize);
                       if (!hMem) {
                           DPF( "    System memory FOURCC surface allocation failed" );
                           lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                           return DDHAL_DRIVER_HANDLED;
                       } else {
                           DPF( "    System memory FOURCC surface allocation succeeded" );
                           psurf_gbl->fpVidMem = (FLATPTR)GlobalLock(hMem);
                           psurf_gbl->dwReserved1 = (DWORD)hMem;
                           psurf->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
                       }

                       if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                           index = 0;
                           while ((pDriverData->dwOverlaySurfaceLCL[index] != 0) &&
                                  (index < NV_MAX_OVERLAY_SURFACES))
                                  index++;
                           pDriverData->dwOverlaySurfaceLCL[index] = (DWORD) psurf;
                           pDriverData->dwOverlayOwner = (DWORD)powner;
                           pDriverData->dwOverlaySurfaces++;
                       }

                    break;

                    case FOURCC_YUY2:
                    case FOURCC_YUNV:
                    case FOURCC_UYVY:
                    case FOURCC_UYNV:

                       if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                           (pDriverData->dwOverlaySurfaces > 0)) {
                           if (pDriverData->OverlayRelaxOwnerCheck)
                                pDriverData->dwOverlayOwner = (DWORD)powner;
                           if ((pDriverData->dwOverlayOwner != (DWORD)powner) ||
                               (pDriverData->dwOverlaySurfaces + 1 >= NV_MAX_OVERLAY_SURFACES)) {
                               lpCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                               return DDHAL_DRIVER_HANDLED;
                           }
                       }

                       /* First try video memory */
                       pitch = ((psurf_gbl->wWidth + 1) & 0xFFFE) << 1;
                       pitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                       /* pad with an extra scanline */
                       dwBlockSize = (pitch * (DWORD)psurf_gbl->wHeight);
                       psurf_gbl->ddpfSurface.dwYUVBitCount = 16;
                       if (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_YUY2) {
                           psurf_gbl->ddpfSurface.dwYBitMask = 0x00FF00FF;
                           psurf_gbl->ddpfSurface.dwUBitMask = 0x0000FF00;
                           psurf_gbl->ddpfSurface.dwVBitMask = 0xFF000000;
                       } else {
                           psurf_gbl->ddpfSurface.dwYBitMask = 0xFF00FF00;
                           psurf_gbl->ddpfSurface.dwUBitMask = 0x000000FF;
                           psurf_gbl->ddpfSurface.dwVBitMask = 0x00FF0000;
                       }
                       psurf_gbl->ddpfSurface.dwYUVAlphaBitMask = 0;
#ifdef DEBUG
                       fourCCStr[0] = (char)psurf_gbl->ddpfSurface.dwFourCC & 0xFF;
                       fourCCStr[1] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 8) & 0xFF;
                       fourCCStr[2] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 16) & 0xFF;
                       fourCCStr[3] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 24) & 0xFF;
                       fourCCStr[4] = 0;
                       DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG
                       psurf_gbl->lPitch = pitch;
                       psurf_gbl->dwReserved1 = 0;
                       psurf_gbl->fpVidMem = 0;
                       psurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;
                       handled = TRUE;

                       /*
                        * Notify 16 bit display driver code that Direct Draw is beginning
                        * to use local video memory.
                        */
#ifndef WINNT
                       if (pDriverData->DDrawVideoSurfaceCount == 0) {
                           HDC hdc = GetDC(0);
                           ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                       }
#endif // #ifndef WINNT

                       if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && (pDriverData->TotalVRAM >> 20 > 4)) {
                           /* Remember offset to 2nd half of surface */
                           psurf_gbl->dwReserved1 = dwBlockSize;
                           /* Tripple actual amount of video memory allocated for this surface only */
                           if (pDriverData->extra422OverlayOffset[0] == 0)
                               dwBlockSize *= 3;
                       }

                       NVHEAP_ALLOC(psurf_gbl->fpVidMem, dwBlockSize, TYPE_OVERLAY);
                       //psurf_gbl->fpVidMem = DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                       if (psurf_gbl->fpVidMem != 0) {
                           pDriverData->DDrawVideoSurfaceCount++;
                           psurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                           psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                           if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                               (pDriverData->TotalVRAM >> 20 > 4) &&
                               (pDriverData->extra422OverlayOffset[0] == 0)) {
                               pDriverData->extra422OverlayOffset[0] =
                                   psurf_gbl->fpVidMem - pDriverData->BaseAddress;
                               pDriverData->extra422OverlayOffset[0] += psurf_gbl->dwReserved1;
                               pDriverData->extra422OverlayOffset[1] =
                                   pDriverData->extra422OverlayOffset[0] + psurf_gbl->dwReserved1;
                           }
                       } else {
                           /* We can't support overlay surfaces in system memory */
                           if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                               lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                               return DDHAL_DRIVER_HANDLED;
                           }

                           hMem = GlobalAlloc(GHND | GMEM_SHARE, dwBlockSize);

                           if (!hMem) {
                               DPF( "    System memory FOURCC surface allocation failed" );
                               lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                               return DDHAL_DRIVER_HANDLED;
                           } else {

                               DPF( "    System memory FOURCC surface allocation succeeded" );
                               psurf_gbl->fpVidMem = (FLATPTR)GlobalLock(hMem);
                               psurf_gbl->dwReserved1 = (DWORD)hMem;
                               psurf->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

                               CreateFloating422Context32(psurf, dwBlockSize);
                           }
                       }

                       if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                           index = 0;
                           while ((pDriverData->dwOverlaySurfaceLCL[index] != 0) &&
                                  (index < NV_MAX_OVERLAY_SURFACES))
                                  index++;
                           pDriverData->dwOverlaySurfaceLCL[index] = (DWORD) psurf;
                           pDriverData->dwOverlayOwner = (DWORD)powner;
                           pDriverData->dwOverlaySurfaces++;
                       }

                    break;

                    case FOURCC_YV12:
                    case FOURCC_420i:

                       if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                           (pDriverData->dwOverlaySurfaces > 0)) {
                           if (pDriverData->OverlayRelaxOwnerCheck)
                                pDriverData->dwOverlayOwner = (DWORD)powner;
                           if ((pDriverData->dwOverlayOwner != (DWORD)powner) ||
                               (pDriverData->dwOverlaySurfaces + 1 >= NV_MAX_OVERLAY_SURFACES)) {
                               lpCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                               return DDHAL_DRIVER_HANDLED;
                           }
                       }

                       /*
                        * Always split allocation of YV12/420i overlay surfaces.
                        *   Allocate the YV12/420i portion in AGP memory on AGP systems, local video memory on PCI systems.
                        *   Fall back to system memory only if preferred allocation fails.
                        *   Allocate additional YUY2 surfaces in local video memory for the video scaler to use
                        */
                       psurf_gbl->ddpfSurface.dwYUVBitCount = 12;
                       psurf_gbl->ddpfSurface.dwYBitMask = 0;
                       psurf_gbl->ddpfSurface.dwUBitMask = 0;
                       psurf_gbl->ddpfSurface.dwVBitMask = 0;
                       psurf_gbl->ddpfSurface.dwYUVAlphaBitMask = 0;

                       psurf_gbl->fpVidMem = 0;
                       psurf_gbl->dwReserved1 = 0;
                       psurf->ddsCaps.dwCaps |= DDSCAPS_HWCODEC;
                       handled = TRUE;

                       pitch = ((psurf_gbl->wWidth + 1) & 0xFFFE) << 1;
                       pitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

                       adjustedHeight = (DWORD)((psurf_gbl->wHeight + 1) & ~1);

                       dwBlockSize = pitch * adjustedHeight;

                       if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) && (pDriverData->TotalVRAM >> 20 > 4)) {
                           /* Remember offset to 2nd half of surface */
                           psurf_gbl->dwReserved1 = dwBlockSize;
                           /* Double actual amount of video memory allocated for this surface */
                           dwBlockSize <<= 1;
                       }

                       if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {

                           /*
                            * Notify 16 bit display driver code that Direct Draw is beginning
                            * to use local video memory.
                            */
#ifndef WINNT
                           if (pDriverData->DDrawVideoSurfaceCount == 0) {
                               HDC hdc = GetDC(0);
                               ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                           }
#endif // #ifndef WINNT

                           NVHEAP_ALLOC((unsigned long)fpVidMem, dwBlockSize, TYPE_OVERLAY);
                           //fpVidMem = (DWORD *)DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                           if (fpVidMem != 0) {
                               pDriverData->DDrawVideoSurfaceCount++;
                               psurf->ddsCaps.dwCaps |= DDSCAPS_LOCALVIDMEM;
                               if (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] == 0)
                                   pDriverData->NvYUY2Surface0Mem = (DWORD)fpVidMem; /* save pointer to YUY2 overlay surface */
                               else if (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] == 0)
                                   pDriverData->NvYUY2Surface1Mem = (DWORD)fpVidMem;
                               else if (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] == 0)
                                   pDriverData->NvYUY2Surface2Mem = (DWORD)fpVidMem;
                               else if (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[3] == 0)
                                   pDriverData->NvYUY2Surface3Mem = (DWORD)fpVidMem;
                           } else {
                               /* one or more overlay surfaces could not be allocated, clean up already allocated memory */
                               if (pDriverData->NvYUY2Surface2Mem > 0) {
                                   NVHEAP_FREE (pDriverData->NvYUY2Surface2Mem);
                                   //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface2Mem);
                                   pDriverData->NvYUY2Surface2Mem = 0;
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
                                   if (pDriverData->DDrawVideoSurfaceCount > 0)
                                       pDriverData->DDrawVideoSurfaceCount--;
                               }

                               if (pDriverData->NvYUY2Surface1Mem > 0) {
                                   NVHEAP_FREE (pDriverData->NvYUY2Surface1Mem);
                                   //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface1Mem);
                                   pDriverData->NvYUY2Surface1Mem = 0;
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
                                   if (pDriverData->DDrawVideoSurfaceCount > 0)
                                       pDriverData->DDrawVideoSurfaceCount--;
                               }

                               if (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] != 0) {
                                   if (pDriverData->NvYUY2Surface0Mem > 0) {
                                       NVHEAP_FREE (pDriverData->NvYUY2Surface0Mem);
                                       //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface0Mem);
                                       pDriverData->NvYUY2Surface0Mem = 0;
                                       if (pDriverData->DDrawVideoSurfaceCount > 0)
                                           pDriverData->DDrawVideoSurfaceCount--;
                                   }

                                   if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] <
                                        pDriverData->BaseAddress) || /* If system memory */
                                       (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >
                                        (unsigned long)pDriverData->VideoHeapEnd)) {
                                       NvNotification *pDmaSyncNotifier =
                                           (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
                                       Nv10ControlDma *npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
                                       unsigned long *dmaPusherPutAddress =
                                           (unsigned long *)pDriverData->NvDmaPusherPutAddress;
                                       unsigned long *dmaPusherBufferBase =
                                           (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
                                       long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
                                       NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
                                       long freeCount = 0;
#endif  /* CACHE_FREECOUNT */

                                       NV_DD_DMA_PUSHER_SYNC();

                                       /* Let D3D code know that we have touched NV */
                                       pDriverData->TwoDRenderingOccurred = 1;

                                       // MUST wait for any pending notification to prevent possible loss of notification serialization
                                       while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

                                       while (freeCount < 28)
                                           NvGetDmaBufferFreeCount(npDev, freeCount, 28, dmaPusherPutAddress);
                                       freeCount -= 28;

                                       pDriverData->dDrawSpareSubchannelObject = 0;

                                       pDmaSyncNotifier->status = NV_IN_PROGRESS;

                                       /* Trash spare subchannel */
                                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                       dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
                                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                       dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                       dmaPusherPutAddress[5] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                       dmaPusherPutAddress[7] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
                                       dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                       dmaPusherPutAddress[9] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                       dmaPusherPutAddress[11] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[12] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                       dmaPusherPutAddress[13] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
                                       dmaPusherPutAddress[14] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                       dmaPusherPutAddress[15] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[16] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                       dmaPusherPutAddress[17] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[18] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                       dmaPusherPutAddress[19] = NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
                                       dmaPusherPutAddress[20] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                       dmaPusherPutAddress[21] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[22] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                           MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                       dmaPusherPutAddress[23] = NV1_NULL_OBJECT;
                                       dmaPusherPutAddress[24] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                                           RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
                                       dmaPusherPutAddress[25] = 0;
                                       dmaPusherPutAddress[26] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                                           RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
                                       dmaPusherPutAddress[27] = 0;

                                       dmaPusherPutAddress += 28;

                                       pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                                       /* Force write combine buffer to flush */
                                       pDriverData->NvDmaPusherBufferEnd[0] = 0;
                                       /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                                       _outp (0x3d0,0);

                                       pDriverData->dwDmaPusherFreeCount = freeCount;

                                       npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                                       while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

                                       NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                                                NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
                                   }

                                   /* If in AGP memory then deallocate it from there */
                                   if ((pDriverData->GARTLinearBase > 0) &&
                                       ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >= pDriverData->GARTLinearBase) &&
                                        (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] < (pDriverData->GARTLinearBase + NV_MAX_AGP_MEMORY_LIMIT)))) {
                                             DDHAL32_VidMemFree(pdrv, AGP_HEAP, pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0]);
                                   } else {
                                       if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] <
                                            pDriverData->BaseAddress) || /* If system memory */
                                           (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >
                                           (unsigned long)pDriverData->VideoHeapEnd)) {
                                           GlobalUnlock((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
                                           GlobalFree((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
                                       } else {
                                           NVHEAP_FREE (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0]);
                                           //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0]);
                                           if (pDriverData->DDrawVideoSurfaceCount > 0)
                                               pDriverData->DDrawVideoSurfaceCount--;
                                       }
                                   }

                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] = 0;
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
                               }

                               pDriverData->dwOverlayOwner = 0;
                               pDriverData->dwOverlaySurfaces = 0;

                               /* We can't support overlay surfaces in system memory */
                               if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                                   lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                                   return DDHAL_DRIVER_HANDLED;
                               }
                           }
                       }
#ifdef DEBUG
                       fourCCStr[0] = (char)psurf_gbl->ddpfSurface.dwFourCC & 0xFF;
                       fourCCStr[1] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 8) & 0xFF;
                       fourCCStr[2] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 16) & 0xFF;
                       fourCCStr[3] = (char)(psurf_gbl->ddpfSurface.dwFourCC >> 24) & 0xFF;
                       fourCCStr[4] = 0;
                       DPF( "FOURCC %s allocation: size=%lu bytes", &fourCCStr, dwBlockSize );
#endif // DEBUG
                       /* now allocate enough AGP memory or local video memory for the YV12 part of the surface */
                       pitch = psurf_gbl->wWidth;
                       psurf_gbl->lPitch = pitch;

                       dwBlockSize = ((pitch * (adjustedHeight * 12L)) >> 3);
                       /* Force block to be properly aligned */
                       dwBlockSize = (dwBlockSize + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

                       if (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] == 0) { /* If this is the first of a complex overlay surface */

                           /*
                            * Allocate enough AGP, video, or system memory for ALL of the surfaces in the
                            * complex surface in a single block
                            */

                           if (lpCreateSurface->dwSCnt > 2)
                               dwBlockSize *= (DWORD)lpCreateSurface->dwSCnt;
                           else /* Assume that 2 of these surfaces will be created */
                               dwBlockSize <<= 1;

                           fpVidMem = 0;

                           memType = 0;

                           if (pDriverData->GARTLinearBase > 0) { /* First attempt to allocate in AGP memory */
                               fpVidMem = (DWORD *)DDHAL32_VidMemAlloc(pdrv, AGP_HEAP, dwBlockSize, 1);
                               if (fpVidMem != 0) {
                                   memType = DDSCAPS_NONLOCALVIDMEM;
                                   psurf->ddsCaps.dwCaps |= DDSCAPS_NONLOCALVIDMEM;
                               }
                           } else { /* else try to allocate the surfaces in framebuffer video memory */
                               /*
                                * Notify 16 bit display driver code that Direct Draw is beginning
                                * to use local video memory.
                                */
#ifndef WINNT
                               if (pDriverData->DDrawVideoSurfaceCount == 0) {
                                   HDC hdc = GetDC(0);
                                   ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                               }
#endif // #ifndef WINNT

                               NVHEAP_ALLOC((unsigned long)fpVidMem, dwBlockSize, TYPE_OVERLAY);
                               //fpVidMem = (DWORD *)DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                               if (fpVidMem != 0) {
                                   memType = DDSCAPS_LOCALVIDMEM;
                                   pDriverData->DDrawVideoSurfaceCount++;
                               }
                           }

                           if (!fpVidMem) { /* Finally try system memory */
                               hMem = GlobalAlloc(GHND | GMEM_SHARE, dwBlockSize);
                               if (!hMem)
                                   memType = DDSCAPS_SYSTEMMEMORY;
                           }

                           if ((!hMem) && (!fpVidMem)) {
                               DPF( "    System memory FOURCC surface allocation failed" );
                               /* Free video memory just allocated above */
                               NVHEAP_FREE (psurf_gbl->dwReserved1);
                               //DDHAL32_VidMemFree(pdrv, 0, psurf_gbl->dwReserved1);
                               if (pDriverData->DDrawVideoSurfaceCount > 0)
                                   pDriverData->DDrawVideoSurfaceCount--;
                               psurf_gbl->dwReserved1 = 0;
                               lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                               return DDHAL_DRIVER_HANDLED;
                           } else {
                               Nv10ControlDma *npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
                               NvNotification *pDmaDmaToMemNotifier =
                                   (NvNotification *)pDriverData->NvDmaDmaToMemNotifierFlat;

                               DPF( "    System memory FOURCC surface allocation succeeded" );

                               if (fpVidMem) {
                                   pDriverData->NvFloatingMem2MemFormatMemoryHandle = 0;
                                   psurf_gbl->fpVidMem = (FLATPTR)fpVidMem;
                               } else {
                                   pDriverData->NvFloatingMem2MemFormatMemoryHandle = (DWORD)hMem;
                                   psurf_gbl->fpVidMem = (FLATPTR)GlobalLock(hMem);
                               }

                               NV_DD_DMA_PUSHER_SYNC();

                               /* Let D3D code know that we have touched NV */
                               pDriverData->TwoDRenderingOccurred = 1;

                               if ((memType == DDSCAPS_NONLOCALVIDMEM) || (memType == DDSCAPS_SYSTEMMEMORY)) { /* If system memory */
                                   NvNotification *pDmaSyncNotifier =
                                       (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
                                   unsigned long *dmaPusherPutAddress =
                                       (unsigned long *)pDriverData->NvDmaPusherPutAddress;
                                   unsigned long *dmaPusherBufferBase =
                                       (unsigned long *)pDriverData->NvDmaPusherBufferBase;
                                   unsigned long status;
#ifdef  CACHE_FREECOUNT
                                   long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
                                   NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
                                   long freeCount = 0;
#endif  /* CACHE_FREECOUNT */
                                   /* First disconnect everything */

                                   // MUST wait for any pending notification to prevent possible loss of notification serialization
                                   while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

                                   while (freeCount < 28)
                                       NvGetDmaBufferFreeCount(npDev, freeCount, 28, dmaPusherPutAddress);
                                   freeCount -= 28;

                                   pDriverData->dDrawSpareSubchannelObject = 0;

                                   pDmaSyncNotifier->status = NV_IN_PROGRESS;

                                   /* Trash spare subchannel */
                                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                   dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                   dmaPusherPutAddress[5] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[7] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                   dmaPusherPutAddress[9] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                   dmaPusherPutAddress[11] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[12] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[13] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[14] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                   dmaPusherPutAddress[15] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[16] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                   dmaPusherPutAddress[17] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[18] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[19] = NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[20] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                                   dmaPusherPutAddress[21] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[22] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                                   dmaPusherPutAddress[23] = NV1_NULL_OBJECT;
                                   dmaPusherPutAddress[24] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                                       RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
                                   dmaPusherPutAddress[25] = 0;
                                   dmaPusherPutAddress[26] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                                       RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
                                   dmaPusherPutAddress[27] = 0;

                                   dmaPusherPutAddress += 28;

                                   pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                                   /* Force write combine buffer to flush */
                                   pDriverData->NvDmaPusherBufferEnd[0] = 0;
                                   /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                                   _outp (0x3d0,0);

                                   pDriverData->dwDmaPusherFreeCount = freeCount;

                                   npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                                   while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

                                   NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                                            NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY);

                                   /* Now reconnect everything */

                                   pDmaDmaToMemNotifier->status = NV_IN_PROGRESS;

                                   status = NvRmAllocContextDma(pDriverData->dwRootHandle,
                                                       NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                                                       NV01_CONTEXT_DMA,
                                                       (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                                       ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                                                       (PVOID)psurf_gbl->fpVidMem,
                                                       (dwBlockSize - 1));

                                   if (status == ALLOC_CTX_DMA_STATUS_SUCCESS) {
                                       pDmaDmaToMemNotifier->status = NV_STATUS_DONE_OK;
                                   }

                                   while (freeCount < 20)
                                       NvGetDmaBufferFreeCount(npDev, freeCount, 20, dmaPusherPutAddress);
                                   freeCount -= 20;

                                   /* Both buffers must be reconnected due to an bug in NV4 */
                                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x80000;
                                   dmaPusherPutAddress[3] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
                                   dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;
                                   dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[6] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x80000;
                                   dmaPusherPutAddress[8] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
                                   dmaPusherPutAddress[9] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;
                                   dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[11] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[12] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x80000;
                                   dmaPusherPutAddress[13] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
                                   dmaPusherPutAddress[14] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;
                                   dmaPusherPutAddress[15] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                                   dmaPusherPutAddress[16] = NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
                                   dmaPusherPutAddress[17] = dDrawSubchannelOffset(NV_DD_SPARE) +
                                       MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x80000;
                                   dmaPusherPutAddress[18] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
                                   dmaPusherPutAddress[19] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

                                   dmaPusherPutAddress += 20;

                                   pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                                   /* Force write combine buffer to flush */
                                   pDriverData->NvDmaPusherBufferEnd[0] = 0;
                                   /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                                   _outp (0x3d0,0);

                                   pDriverData->dwDmaPusherFreeCount = freeCount;

                                   npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                                   while ((volatile)pDmaDmaToMemNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

                               } else { /* If video memory */
                                   pDmaDmaToMemNotifier->status = NV_STATUS_DONE_OK;
                               }

                               if (pDmaDmaToMemNotifier->status == NV_STATUS_DONE_OK) {
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] =
                                       (DWORD)psurf_gbl->fpVidMem;
                               } else {
                                   /* Can't lock down system memory, forget using this technique */
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] = 0;
                                   /* Free video memory just allocated above */
                                   NVHEAP_FREE (pDriverData->NvYUY2Surface0Mem);
                                   //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvYUY2Surface0Mem);
                                   if (pDriverData->DDrawVideoSurfaceCount > 0)
                                       pDriverData->DDrawVideoSurfaceCount--;

                                   if (pDriverData->NvFloatingMem2MemFormatMemoryHandle) {
                                       GlobalUnlock((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
                                       GlobalFree((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
                                   } else {
                                       /* Free AGP memory just allocated above */
                                       DDHAL32_VidMemFree(pdrv, AGP_HEAP, psurf_gbl->fpVidMem);

                                   }
                                   psurf_gbl->fpVidMem = 0;
                                   lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                                   return DDHAL_DRIVER_HANDLED;
                               }
                           }
                       } else {
                           /* Assign surface address within preallocated and locked video or AGP/system memory block */
                           if (i > 0) {
                               psurf_gbl->fpVidMem =
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] +
                                       (i * dwBlockSize);
                               if (i == 1)
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = psurf_gbl->fpVidMem;
                               else if (i == 2)
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = psurf_gbl->fpVidMem;
                               else if (i == 3)
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[3] = psurf_gbl->fpVidMem;
                           } else {
                               psurf_gbl->fpVidMem =
                                   pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] +
                                       dwBlockSize;
                               pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = psurf_gbl->fpVidMem;
                           }

                           /* If in AGP memory then set capability flag */
                           if (pDriverData->GARTLinearBase > 0) {
                               if ((psurf_gbl->fpVidMem >= pDriverData->GARTLinearBase) &&
                                   (psurf_gbl->fpVidMem < (pDriverData->GARTLinearBase + NV_MAX_AGP_MEMORY_LIMIT)))
                                   psurf->ddsCaps.dwCaps |= DDSCAPS_NONLOCALVIDMEM;
                           }
                       }

                       if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {
                           index = 0;
                           while ((pDriverData->dwOverlaySurfaceLCL[index] != 0) &&
                                  (index < NV_MAX_OVERLAY_SURFACES))
                                  index++;
                           pDriverData->dwOverlaySurfaceLCL[index] = (DWORD) psurf;
                           pDriverData->dwOverlayOwner = (DWORD)powner;
                           pDriverData->dwOverlaySurfaces++;
                       }

                    break;
                }
#ifdef NVD3D
            } else if (psurf->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {
                /*
                 * Notify 16 bit display driver code that Direct Draw is beginning
                 * to use local video memory.
                 */
#ifndef WINNT
                if (pDriverData->DDrawVideoSurfaceCount == 0) {
                    HDC hdc = GetDC(0);
                    ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                }
#endif // #ifndef WINNT
                /*
                 * Call the texture surface allocation code in the D3D driver. (D3dtex.c)
                 * If the routine returns FALSE, then the surface creation failed and the
                 * error should be returned immediately.
                 */
                if (!(handled = nvCreateTextureSurface(lpCreateSurface, psurf, psurf_gbl)))
                   return DDHAL_DRIVER_HANDLED;
            } else if (psurf->ddsCaps.dwCaps & DDSCAPS_ZBUFFER) {

                if (pDriverData->regValidateZMethod == REG_VZM_STRICT) {
                    /*
                     * Allocate z-buffer memory exactly as the surface specifies.
                     * Z-Buffer must match render depth or D3D context creation will fail.
                     */
                    if ( (psurf->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
                         (psurf_gbl->ddpfSurface.dwFlags & DDPF_ZBUFFER) ) {
                       /*
                        * I need to check this here and reject it because a lot of times
                        * CanCreateSurface will be called and the format will not have been
                        * set and then CreateSurface will be called and the format will be
                        * set to something other than the current primary surface bit depth,
                        * which we can't support.
                        */
                       if (psurf_gbl->ddpfSurface.dwZBufferBitDepth != pDriverData->bi.biBitCount) {
                           lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                           return DDHAL_DRIVER_HANDLED;
                       }
                       pitch = ((psurf_gbl->wWidth <<
                                (psurf_gbl->ddpfSurface.dwZBufferBitDepth >> 4)) +
                                 pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                    }
                    else
                       pitch = ((psurf_gbl->wWidth <<
                                (pDriverData->bi.biBitCount >> 4)) +
                                 pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                }
                else {
                    /*
                     * Pad the z-buffer if neccessary to match z-buffer depth to the D3D rendering depth.
                     */
                    if ( (psurf->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
                         (psurf_gbl->ddpfSurface.dwFlags & DDPF_ZBUFFER) ) {
                       /*
                        * When allocating the z-buffer, we have to make sure that it matches
                        * the rendering surface.  I'm saying that I support 16, 24 and 32 bits
                        * of z-buffer, because applications seem to only understand 16 and 24.
                        * So here's what I'm going to do (this is subject to change).  If the
                        * application is running in 16bpp mode and it requests a 24 or 32 bit
                        * z buffer, they will only get 16 bpp of precision (because that's what
                        * the hardware supports) but we'll go ahead and waste as much memory
                        * as they request (for now anyway).  If the application requests a 16 or 24
                        * bit z-buffer when we are rendering in 32 bpp mode, then they will get
                        * 24 bit z-precision, but we will have to waste the full 32bpp to give
                        * it to them.
                        */
                       if (pDriverData->bi.biBitCount == 16) {
                           if (psurf_gbl->ddpfSurface.dwZBufferBitDepth == 16) {
                               pitch = ((psurf_gbl->wWidth << 1) +
                                         pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                           }
                           else if ((psurf_gbl->ddpfSurface.dwZBufferBitDepth == 24)
                                 || (psurf_gbl->ddpfSurface.dwZBufferBitDepth == 32)) {
                               pitch = ((psurf_gbl->wWidth << 2) +
                                         pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                           }
                           else {
                               /* Anything else just won't do. */
                               lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                               return DDHAL_DRIVER_HANDLED;
                           }
                       }
                       else if (pDriverData->bi.biBitCount == 32) {
                           if ((psurf_gbl->ddpfSurface.dwZBufferBitDepth == 16)
                            || (psurf_gbl->ddpfSurface.dwZBufferBitDepth == 24)
                            || (psurf_gbl->ddpfSurface.dwZBufferBitDepth == 32)) {
                               pitch = ((psurf_gbl->wWidth << 2) +
                                         pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                           }
                           else {
                               /* Anything else just won't do. */
                               lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                               return DDHAL_DRIVER_HANDLED;
                           }
                       }
                       else {
                           /* Anything else just won't do. */
                           lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                           return DDHAL_DRIVER_HANDLED;
                       }
                    }
                    else
                       pitch = ((psurf_gbl->wWidth <<
                                (pDriverData->bi.biBitCount >> 4)) +
                                 pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                }

                psurf_gbl->lPitch = pitch;
                dwBlockSize = pitch * psurf_gbl->wHeight;
                psurf_gbl->dwReserved1 = 0;

                /*
                 * Notify 16 bit display driver code that Direct Draw is beginning
                 * to use local video memory.
                 */
#ifndef WINNT
                if (pDriverData->DDrawVideoSurfaceCount == 0) {
                    HDC hdc = GetDC(0);
                    ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                }
#endif // #ifndef WINNT
                NVHEAP_ALLOC_TILED(psurf_gbl->fpVidMem, psurf_gbl->lPitch, psurf_gbl->wHeight, TYPE_DEPTH);
                //psurf_gbl->fpVidMem = DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                if (psurf_gbl->fpVidMem != 0) {
                    pDriverData->DDrawVideoSurfaceCount++;
                    DPF( "    Video memory Z buffer surface allocation succeeded" );
                    psurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                    psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                    handled = TRUE;
                } else {
                   DPF( "    Video memory Z buffer surface allocation failed" );
                   lpCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
                   return DDHAL_DRIVER_HANDLED;
                }
#endif // NVD3D
            }
        } else {
            if (psurf->ddsCaps.dwCaps &
               (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE)) {

                /* Must allow primary surface access even if not correct pixel depth for 3D */
                /* Fixes Jedi Knight bug */
                if ((psurf->ddsCaps.dwCaps & DDSCAPS_3DDEVICE) &&
                    (!(psurf->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE))) {
                    if (pDriverData->bi.biBitCount == 8) {
                        lpCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                        return DDHAL_DRIVER_HANDLED;
                    }
                }

                DPF( "    Front buffer granted" );
                pitch = pDriverData->bi.biWidth *
                        (pDriverData->bi.biBitCount >> 3);
                /* Force primary surface alignment to be correct */
                psurf_gbl->lPitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

                /* Must wait for any pending flip to finish so that CurrentVisibleSurface will be valid */
                ddrval = updateFlipStatus(0);
                if (ddrval != DD_OK) {
                    while (ddrval != DD_OK) {
                        NV_SLEEPFLIP;
                        ddrval = updateFlipStatus(0);
                    }
                }

                psurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                psurf_gbl->fpVidMem = pDriverData->CurrentVisibleSurfaceAddress;
                psurf_gbl->dwReserved1 = 0;
                psurf->dwReserved1 = 0;
                handled = TRUE;
            } else if (psurf->ddsCaps.dwCaps & (DDSCAPS_BACKBUFFER | DDSCAPS_FLIP)) {
                DPF( "    Back buffer or flip surface granted" );
                pitch = pDriverData->bi.biWidth *
                        (pDriverData->bi.biBitCount >> 3);
                psurf_gbl->lPitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                dwBlockSize = psurf_gbl->lPitch * pDriverData->bi.biHeight;
                psurf_gbl->dwReserved1 = 0;
                psurf->dwReserved1 = 0;

                /*
                 * Notify 16 bit display driver code that Direct Draw is beginning
                 * to use local video memory.
                 */
#ifndef WINNT
                if (pDriverData->DDrawVideoSurfaceCount == 0) {
                    HDC hdc = GetDC(0);
                    ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                }
#endif // #ifndef WINNT
                NVHEAP_ALLOC_TILED(psurf_gbl->fpVidMem, psurf_gbl->lPitch, pDriverData->bi.biHeight, MEM_TYPE_IMAGE_TILED);
                //psurf_gbl->fpVidMem = DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                if (psurf_gbl->fpVidMem != 0) {
                    pDriverData->DDrawVideoSurfaceCount++;
                    DPF( "    Backbuffer or flipable video memory surface allocation succeeded" );
                    psurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                    psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                    handled = TRUE;

#ifdef FORCED_TRIPLE_BUFFER
                    // a flippable double buffered scenario has been granted,
                    // let's try and make it triple buffered
                    if (lpCreateSurface->dwSCnt == 2) {
                        IDirectDraw2 *lpDD_int = (IDirectDraw2 *) psurf->lpSurfMore->lpDD_int;
                        DDSURFACEDESC surfDesc;
                        HRESULT result;

                        if (lpDD_int) {
                            surfDesc = *(lpCreateSurface->lpDDSurfaceDesc);
                            surfDesc.dwFlags |= DDSD_HEIGHT | DDSD_WIDTH;
                            surfDesc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
                            surfDesc.dwWidth = psurf_gbl->wWidth;
                            surfDesc.dwHeight = psurf_gbl->wHeight;
                            surfDesc.dwBackBufferCount = 0;
                            surfDesc.ddsCaps.dwCaps &= ~(DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP);
                            result = IDirectDraw2_CreateSurface(lpDD_int, &surfDesc, &(pDriverData->lpBuffer3), NULL);
                            if (result == DD_OK) {
                                LPDDRAWI_DDRAWSURFACE_LCL lpSurfLcl = ((LPDDRAWI_DDRAWSURFACE_INT)pDriverData->lpBuffer3)->lpLcl;
                                pDriverData->bNeedToAttachBuffer = TRUE;
//                                lpSurfLcl->ddsCaps.dwCaps |= DDSCAPS_COMPLEX;
//                                lpSurfLcl->dwFlags |= 0x22; // private flags IMPLICITCREATE and PARTOFPRIMARYCHAIN
                                psurf->dwFlags &= ~0x2;
                            } else {
                                pDriverData->lpBuffer3 = NULL;
                                pDriverData->bNeedToAttachBuffer = FALSE;
                            }
                        }
                    }
#endif // FORCED_TRIPLE_BUFFER
                } else {
                    DPF( "    Backbuffer or flipable video memory surface allocation failed" );
                    lpCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
                    return DDHAL_DRIVER_HANDLED;
                }
            } else if (psurf->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) {
putInOffscreen:     DPF( "    Offscreen surface granted" );
                pitch = psurf_gbl->wWidth *
                        (pDriverData->bi.biBitCount >> 3);
                psurf_gbl->lPitch = (pitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
                dwBlockSize = psurf_gbl->lPitch * psurf_gbl->wHeight;
                psurf_gbl->dwReserved1 = 0;

                if ((psurf->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM)  &&
                    (pDriverData->GARTLinearBase > 0))  { /* First attempt to allocate in AGP memory */
#ifndef WINNT
                        psurf_gbl->fpVidMem =  (FLATPTR)DDHAL32_VidMemAlloc(pdrv, AGP_HEAP, dwBlockSize, 1);
#else
                        psurf_gbl->fpVidMem = DDHAL_PLEASEALLOC_BLOCKSIZE;
                        psurf_gbl->dwBlockSizeX = dwBlockSize;
                        psurf_gbl->dwBlockSizeY = 1;
                        lpCreateSurface->lpDDSurfaceDesc->lPitch = psurf_gbl->lPitch;
                        lpCreateSurface->lpDDSurfaceDesc->dwFlags |= DDSD_PITCH;
#endif // WINNT
                    if (psurf_gbl->fpVidMem != 0) {
                        DPF( "    Offscreen AGP memory surface allocation succeeded" );
                        psurf->ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
                        psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
#ifndef WINNT
                        handled = TRUE;
#else
                        handled = FALSE;
#endif // WINNT

                    } else {
                        DPF( "    Offscreen AGP memory surface allocation failed" );
                        psurf_gbl->dwReserved1 = 0; /* Make sure we don't try to deallocate it if DestroySurface is called */
                        lpCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
                        return DDHAL_DRIVER_HANDLED;
                    }
                } else {
                    /*
                     * Notify 16 bit display driver code that Direct Draw is beginning
                     * to use local video memory.
                     */
#ifndef WINNT
                    if (pDriverData->DDrawVideoSurfaceCount == 0) {
                        HDC hdc = GetDC(0);
                        ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                    }
#endif // #ifndef WINNT

                    NVHEAP_ALLOC(psurf_gbl->fpVidMem, dwBlockSize, TYPE_IMAGE);
                    //psurf_gbl->fpVidMem = DDHAL32_VidMemAlloc(pdrv, 0, dwBlockSize, 1);

                    if (psurf_gbl->fpVidMem != 0) {
                        pDriverData->DDrawVideoSurfaceCount++;
                        DPF( "    Offscreen video memory surface allocation succeeded" );
                        psurf->ddsCaps.dwCaps |= (DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM);
                        psurf->ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
                        handled = TRUE;
                    } else {
                        if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
                            DPF( "    Offscreen video memory surface allocation failed" );
                            psurf_gbl->dwReserved1 = 0; /* Make sure we don't try to deallocate it if DestroySurface is called */
                            lpCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
                            return DDHAL_DRIVER_HANDLED;
                        }

                        DPF( "    Offscreen surface allocation failed, trying system memory" );
                        pitch = (pitch * 3) & ~3L;
                        dwBlockSize = pitch * psurf_gbl->wHeight;
                        hMem = GlobalAlloc(GHND | GMEM_SHARE, dwBlockSize);
                        if (!hMem) {
                            DPF( "    System memory surface allocation failed" );
                            psurf_gbl->dwReserved1 = 0; /* Make sure we don't try to deallocate it if DestroySurface is called */
                            lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                            return DDHAL_DRIVER_HANDLED;
                        } else {
                            DPF( "    System memory surface allocation succeeded" );
                        }
                        psurf_gbl->lPitch = pitch;
                        psurf_gbl->fpVidMem = (FLATPTR)GlobalLock(hMem);
                        psurf_gbl->dwReserved1 = (DWORD)hMem;
                        psurf->ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
                        psurf->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
                        handled = TRUE;
                    }
                }
            } else if (psurf->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) {
                DPF( "    System memory surface requested" );
                pitch = psurf_gbl->wWidth *
                       (pDriverData->bi.biBitCount >> 3);
                pitch = (pitch + 3) & ~3L;  // pad to DWORD alignment
                hMem = GlobalAlloc(GHND | GMEM_SHARE, (pitch * psurf_gbl->wHeight));
                if (!hMem) {
                    DPF( "    System memory surface allocation failed" );
                    lpCreateSurface->ddRVal = DDERR_OUTOFMEMORY;
                    return DDHAL_DRIVER_HANDLED;
                } else {
                    DPF( "    System memory surface allocation succeeded" );
                }
                psurf_gbl->lPitch = pitch;
                psurf_gbl->fpVidMem = (FLATPTR)GlobalLock(hMem);
                psurf_gbl->dwReserved1 = (DWORD)hMem;
                handled = TRUE;
#ifdef NVD3D
            } else if (psurf->ddsCaps.dwCaps & DDSCAPS_TEXTURE) {
                /*
                 * Notify 16 bit display driver code that Direct Draw is beginning
                 * to use local video memory.
                 */
#ifndef WINNT
                if (pDriverData->DDrawVideoSurfaceCount == 0) {
                    HDC hdc = GetDC(0);
                    ExtEscape(hdc, RECONFIGNVOFFSCREEN, 0, 0, 0, 0);
                }
#endif // #ifndef WINNT
                /*
                 * Call the texture surface allocation code in the D3D driver. (D3dtex.c)
                 * If the routine returns FALSE, then the surface creation failed and the
                 * error should be returned immediately.
                 */
                if (!(handled = nvCreateTextureSurface(lpCreateSurface, psurf, psurf_gbl)))
                   return DDHAL_DRIVER_HANDLED;
#endif  // NVD3D
            } else if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) {
                goto putInOffscreen;
            }
        }
    }

    if( handled ) {
        lpCreateSurface->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * if we return handled, then it is assumed that we did SOMETHING
     * with the surface structures to indicate either what size of block
     * or a new pitch or some modification; or we are returning an error.
     */
    return DDHAL_DRIVER_NOTHANDLED;

} /* CreateSurface32 */


/*
 * CanCreateSurface32
 */
DWORD __stdcall CanCreateSurface32( LPDDHAL_CANCREATESURFACEDATA lpCanCreateSurface ) {
    DWORD               caps;
    LPDDSURFACEDESC     lpDDSurface;
    int                 i;

    NvSetDriverDataPtr(lpCanCreateSurface->lpDD);

    /*
     * Reset NV and get the monitor frequency after a mode reset
     */
    if( pDriverData->fReset ) {
        getDisplayDuration();
        if (!ResetNV()) {
            lpCanCreateSurface->ddRVal = DDERR_GENERIC;
            return(DDHAL_DRIVER_HANDLED);
        }
        pDriverData->fReset = FALSE;
    }

    /*
     * NOTES:
     *
     * This entry point is called after parameter validation but before
     * any object creation.   You can decide here if it is possible for
     * you to create this surface.  For example, if the person is trying
     * to create an overlay, and you already have the maximum number of
     * overlays created, this is the place to fail the call.
     *
     * You also need to check if the pixel format specified can be supported.
     *
     * lpCanCreateSurface->bIsDifferentPixelFormat tells us if the pixel format of the
     * surface being created matches that of the primary surface.  It can be
     * true for Z buffer and alpha buffers, so don't just reject it out of
     * hand...
     */
    DPF( "CanCreateSurface, lpCanCreateSurface->lpDD=%08lx", lpCanCreateSurface->lpDD );
    DPF( "    lpCanCreateSurface->lpDDSurfaceDesc=%08lx", lpCanCreateSurface->lpDDSurfaceDesc );

    lpDDSurface = lpCanCreateSurface->lpDDSurfaceDesc;

    caps = lpDDSurface->ddsCaps.dwCaps;

    if (caps & (DDSCAPS_MODEX | DDSCAPS_PALETTE))
        return DDHAL_DRIVER_NOTHANDLED;

    if (pDriverData->bi.biBitCount == 8) {
        if (caps & DDSCAPS_ALPHA)
            return DDHAL_DRIVER_NOTHANDLED;
    }

    /*
     * check pixel format.   Don't allow pixel formats that aren't
     * the same, unless we have a valid fourcc code, an overlay,
     * an alpha surface, or z buffer.
     */
    if ( lpCanCreateSurface->bIsDifferentPixelFormat ) {
        DPF( "    different pixel format!" );

        if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_ALPHAPREMULT ) {
            if ((pDriverData->bi.biBitCount == 8) || (pDriverData->bi.biBitCount == 16)) {
                /* We don't support alpha surfaces in 8bpp mode */
                DPF("NVDD32: CanCreateSurface32 - no alpha surf in 8bpp");
                lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                return DDHAL_DRIVER_HANDLED;
            }

            if (pDriverData->bi.biBitCount == 32) {
                if ((lpDDSurface->ddpfPixelFormat.dwRGBAlphaBitMask != 0xFF000000) ||
                    (lpDDSurface->ddpfPixelFormat.dwRBitMask != 0x00FF0000) ||
                    (lpDDSurface->ddpfPixelFormat.dwGBitMask != 0x0000FF00) ||
                    (lpDDSurface->ddpfPixelFormat.dwBBitMask != 0x000000FF)) {
                    DPF("NVDD32: CanCreateSurface32 - unsupported format 10");
                    lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                    return DDHAL_DRIVER_HANDLED;
                } else {
                    lpCanCreateSurface->ddRVal = DD_OK;
                    return DDHAL_DRIVER_HANDLED;
                }
            }
        }

        if ( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_FOURCC ) {
            if (pDriverData->bi.biBitCount == 8) {
                /* We don't support RGB FOURCCs in 8bpp mode */
                if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_RGB0) {
                    DPF("NVDD32: CanCreateSurface32 - invalid pixel format 10");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_HANDLED;
                }
            }

            DPF( "    FourCC requested (%4.4hs, 0x%08lx)",
                (LPSTR) &lpDDSurface->ddpfPixelFormat.dwFourCC,
                lpDDSurface->ddpfPixelFormat.dwFourCC );

            for( i=0; i < NV_MAX_FOURCC; i++ ) {
                if( lpDDSurface->ddpfPixelFormat.dwFourCC == pDriverData->fourCC[i] ) {
                    DPF( "    FOURCC=%4.4hs", (LPSTR) &pDriverData->fourCC[i] );
                    if (caps & DDSCAPS_OVERLAY) {
                        if ((lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYVY) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYNV) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUY2) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUNV) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YV12) &&
//                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_420i) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IF09) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YVU9) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IV32) &&
                            (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_IV31)) {
                            DPF("NVDD32: CanCreateSurface32 - unsupported format 20");
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                        }

                        if ((caps & (DDSCAPS_VIDEOPORT | DDSCAPS_FLIP)) == (DDSCAPS_VIDEOPORT | DDSCAPS_FLIP)) {
                            // Surface is for overlay with flip associated with video port
                            // check number of buffers, can't have more than 5 surfaces
                            // This will probably change when we have kernel mode flipping support
                            if ((lpDDSurface->dwFlags & DDSD_BACKBUFFERCOUNT) &&
                                (lpDDSurface->dwBackBufferCount > 4)) {
                                DPF("NVDD32: CanCreateSurface32 - invalid parameters 10");
                                lpCanCreateSurface->ddRVal = DDERR_INVALIDPARAMS;
                                return DDHAL_DRIVER_HANDLED;
                            }
                        } else {
                            // Workaround for Microsoft DirectVideo Cinepak decoding bug
                            // The reason for the existence of FOURCC_UYNV and FOURCC_YUNV
//                          if ((lpDDSurface->dwFlags & DDSD_BACKBUFFERCOUNT) &&
//                                (lpDDSurface->dwBackBufferCount > 1) &&
//                                ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_UYVY) ||
//                                 (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YUY2))) {
//                              lpCanCreateSurface->ddRVal = DDERR_INVALIDPARAMS;
//                                return DDHAL_DRIVER_HANDLED;
//                          }

                            // We must limit number of YV12 overlay surfaces to the maximum number we can manage reformatting
                            if ((lpDDSurface->dwFlags & DDSD_BACKBUFFERCOUNT) &&
                                (lpDDSurface->dwBackBufferCount > 3) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YV12)) {
                                DPF("NVDD32: CanCreateSurface32 - out of caps 10");
                                lpCanCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                                return DDHAL_DRIVER_HANDLED;
                            }
                        }

                        if (pDriverData->dwOverlaySurfaces > 0) {
                            DPF("NVDD32: CanCreateSurface32 - out of caps 20");
                            lpCanCreateSurface->ddRVal = DDERR_OUTOFCAPS;
                            return DDHAL_DRIVER_HANDLED;
                        }

                        if (GetVideoScalerBandwidthStatus32(caps) == FALSE) {
                            DPF("NVDD32: CanCreateSurface32 - no overlay hardware 10");
                            lpCanCreateSurface->ddRVal = DDERR_NOOVERLAYHW;
                            return DDHAL_DRIVER_HANDLED;
                        }
                    } else if (caps & (DDSCAPS_VIDEOMEMORY | DDSCAPS_OFFSCREENPLAIN)) {
                        /* Front end YUV support cannot reformat data so Indeo is not supported */
                        /* Currently also disallowing front end YUV420 since YUYV or UYVY
                           should do ok when not enough backend bandwidth is available */
                        if ((lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_IF09) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YVU9) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_IV32) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_IV31) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_YV12) ||
                            (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_420i)) {
                            DPF("NVDD32: CanCreateSurface32 - unsupported format 30");
                            lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                        }

                        if (caps & DDSCAPS_VIDEOPORT) {
                            /* Nonoverlay FOURCC video surfaces are only supported for YUV422
                               formats and only when associated with the video port */
                            if ((lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUY2) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_YUNV) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYVY) &&
                                (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_UYNV) &&
                                          (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_RAW8)
                                          ) {
                                DPF("NVDD32: CanCreateSurface32 - unsupported format 40");
                                lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                                return DDHAL_DRIVER_HANDLED;
                            }
                        /* Front end YUV support not available in 8bit indexed mode */
                        } else if (pDriverData->bi.biBitCount == 8) {
                            if (lpDDSurface->ddpfPixelFormat.dwFourCC != FOURCC_RAW8) {
                                DPF("NVDD32: CanCreateSurface32 - unsupported format 50");
                                lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                                return DDHAL_DRIVER_HANDLED;
                            }
                        }
                    }

                    if (lpDDSurface->ddpfPixelFormat.dwFourCC == FOURCC_RGB0) {
                        if (((pDriverData->bi.biBitCount == 16) &&
                            (lpDDSurface->ddpfPixelFormat.dwRGBBitCount != DDBD_16)) ||
                            ((pDriverData->bi.biBitCount == 32) &&
                            (lpDDSurface->ddpfPixelFormat.dwRGBBitCount != DDBD_32))) {
                            DPF("NVDD32: CanCreateSurface32 - invalid pixel format 20");
                            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                            return DDHAL_DRIVER_HANDLED;
                        }
                    }

                    lpCanCreateSurface->ddRVal = DD_OK;
                    return DDHAL_DRIVER_HANDLED;
                }
            }

        } else if( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_RGB ) {

            /* Don't allow 3D devices in 8bpp */
            if (caps & DDSCAPS_3DDEVICE) {
                if (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 8) {
                    DPF("NVDD32: CanCreateSurface32 - invalid pixel format 30");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_HANDLED;
                }
            }

            if (caps & DDSCAPS_TEXTURE) {
                /* Don't allow textures in 8bpp. */
                if (pDriverData->bi.biBitCount == 8)
                {
                    DPF("NVDD32: CanCreateSurface32 - invalid pixel format 40");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_NOTHANDLED;
                }
                /* accept 8 bpp texture formats if conversion is enabled */
                if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 8) &&
                    (pDriverData->regPal8TextureConvert != REG_P8TC_NONE))
                {
                    lpCanCreateSurface->ddRVal = DD_OK;
                    return DDHAL_DRIVER_HANDLED;
                }
                /* accept all 16 and 32 bpp texture formats */
                if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16) ||
                    (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 32))
                {
                    lpCanCreateSurface->ddRVal = DD_OK;
                    return DDHAL_DRIVER_HANDLED;
                }
            }

            if (caps & DDSCAPS_ZBUFFER) {
                if (pDriverData->regValidateZMethod == REG_VZM_STRICT) {
                    /* For now, support z buffers in video memory only when our primary rendering depth is 16 bpp */

                    if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16)
                     && (pDriverData->bi.biBitCount == 16))
                    {
                        /* accept all 16 bpp z buffer formats when in 16 bpp disply mode */
                        lpCanCreateSurface->ddRVal = DD_OK;
                        return DDHAL_DRIVER_HANDLED;
                    }

                    if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 32)
                     && (pDriverData->bi.biBitCount == 32))
                    {
                        /* accept all 32 bpp z buffer formats when in 32 bpp disply mode */
                        lpCanCreateSurface->ddRVal = DD_OK;
                        return DDHAL_DRIVER_HANDLED;
                    }

                    /*
                     * Any other z buffer depth or combination of z/display surface depths
                     * are invalid.
                     */
                    return DDHAL_DRIVER_NOTHANDLED;
                }
                else {
                    /* Allocate the z-buffer with the proper padding based on the bit depth of the primary surface */
                    /* accept all 16, 24, 32 bpp z buffer formats when in 16 bpp disply mode */
                    if ((lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 16)
                     || (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 24)
                     || (lpDDSurface->ddpfPixelFormat.dwRGBBitCount == 32)) {
                        lpCanCreateSurface->ddRVal = DD_OK;
                        return DDHAL_DRIVER_HANDLED;
                    }

                    /*
                     * Any other z buffer depth or combination of z/display surface depths
                     * are invalid.
                     */
                    return DDHAL_DRIVER_NOTHANDLED;
               }
            }

            /* We don't support any other RGB surfaces not in our native format */
            DPF("NVDD32: CanCreateSurface32 - invalid pixel format 50");
            lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
            return DDHAL_DRIVER_HANDLED;

        } else if( lpDDSurface->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER ) {

            /* Don't allow z-buffer surfaces in 8bpp */
            if (caps & DDSCAPS_ZBUFFER) {
                if (pDriverData->bi.biBitCount == 8) {
                    DPF("NVDD32: CanCreateSurface32 - invalid pixel format 60");
                    lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                    return DDHAL_DRIVER_NOTHANDLED;
                }

                if (pDriverData->regValidateZMethod == REG_VZM_STRICT) {
                    if ((lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != pDriverData->bi.biBitCount)) {
                        DPF("NVDD32: CanCreateSurface32 - invalid pixel format 70");
                        lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                        return DDHAL_DRIVER_NOTHANDLED;
                    }
                }
                else {
                    if ((lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != 16)
                     && (lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != 24)
                     && (lpDDSurface->ddpfPixelFormat.dwZBufferBitDepth != 32)) {
                        DPF("NVDD32: CanCreateSurface32 - invalid pixel format 80");
                        lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
                        return DDHAL_DRIVER_NOTHANDLED;
                    }
                }
            }

            DPF( "Z BUFFER OK!" );
            lpCanCreateSurface->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;
        }

        /*
         * can't handle any other kinds of different fourcc or RGB overlays
         */
        DPF("NVDD32: CanCreateSurface32 - invalid pixel format 90");
        lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
        return DDHAL_DRIVER_HANDLED;
   }

   if (caps & DDSCAPS_MODEX) {
       DPF( "    Mode X requested" );
       lpCanCreateSurface->ddRVal = DDERR_UNSUPPORTEDMODE;
       return DDHAL_DRIVER_HANDLED;
   }

   /* Don't allow 3D devices in 8bpp (except for primary surface access, fixes Jedi Knight) */
   if ((caps & DDSCAPS_3DDEVICE) && (!(caps & DDSCAPS_PRIMARYSURFACE))) {
       if (pDriverData->bi.biBitCount == 8) {
           DPF("NVDD32: CanCreateSurface32 - invalid pixel format 100");
           lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
           return DDHAL_DRIVER_HANDLED;
       }
   }

   if (caps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_VISIBLE))  {
       DPF( "    Frontbuffer requested" );

       if (caps & DDSCAPS_OVERLAY) {
           DPF("NVDD32: CanCreateSurface32 - invalid pixel format 110");
           lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
           return DDHAL_DRIVER_HANDLED;
       }

       if (caps & DDSCAPS_FLIP) {
           if (lpDDSurface->dwHeight > (unsigned long)pDriverData->bi.biHeight) {
               if (lpDDSurface->dwFlags & DDSD_HEIGHT) {
                   DPF("NVDD32: CanCreateSurface32 - height too big 10");
                   lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                   return DDHAL_DRIVER_HANDLED;
               } else {
                   lpDDSurface->dwHeight = (unsigned long)pDriverData->bi.biHeight;
               }

           }
           if (lpDDSurface->dwWidth > (unsigned long)pDriverData->bi.biWidth) {
               if (lpDDSurface->dwFlags & DDSD_WIDTH) {
                   DPF("NVDD32: CanCreateSurface32 - width too big 10");
                   lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                   return DDHAL_DRIVER_HANDLED;
               } else {
                   lpDDSurface->dwWidth = (unsigned long)pDriverData->bi.biWidth;
               }
           }
       }
       lpCanCreateSurface->ddRVal = DD_OK;
       return DDHAL_DRIVER_HANDLED;

   } else if (caps & DDSCAPS_BACKBUFFER) {
       DPF( "    Backbuffer requested" );
#ifndef NVHEAPMGR
       if (pDriverData->HALInfo.vmiData.dwNumHeaps == 0) {
           DPF("NVDD32: CanCreateSurface32 - out of video memory 10");
           lpCanCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
           return DDHAL_DRIVER_HANDLED;
       }
#endif
       if (caps & DDSCAPS_OVERLAY) {
           DPF("NVDD32: CanCreateSurface32 - invalid pixel format 120");
           lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
           return DDHAL_DRIVER_HANDLED;
       }

       if (caps & DDSCAPS_FLIP) {
           if (lpDDSurface->dwHeight > (unsigned long)pDriverData->bi.biWidth) {
               if (lpDDSurface->dwFlags & DDSD_HEIGHT) {
                   DPF("NVDD32: CanCreateSurface32 - height too big 20");
                   lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                   return DDHAL_DRIVER_HANDLED;
               } else {
                   lpDDSurface->dwHeight = (unsigned long)pDriverData->bi.biHeight;
               }

           }
           if (lpDDSurface->dwWidth > (unsigned long)pDriverData->bi.biWidth) {
               if (lpDDSurface->dwFlags & DDSD_WIDTH) {
                   DPF("NVDD32: CanCreateSurface32 - width too big 20");
                   lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                   return DDHAL_DRIVER_HANDLED;
               } else {
                   lpDDSurface->dwWidth = (unsigned long)pDriverData->bi.biWidth;
               }
           }
       }
       lpCanCreateSurface->ddRVal = DD_OK;
       return DDHAL_DRIVER_HANDLED;

   } else if (caps & DDSCAPS_TEXTURE) {

       /* Don't allow textures in 8bpp */
       if (pDriverData->bi.biBitCount == 8) {
           DPF("NVDD32: CanCreateSurface32 - invalid pixel format 130");
           lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
           return DDHAL_DRIVER_HANDLED;
       }

       lpDDSurface->ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
       lpDDSurface->ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
       lpCanCreateSurface->ddRVal = DD_OK;
       return DDHAL_DRIVER_HANDLED;

   } else if (caps & DDSCAPS_OFFSCREENPLAIN) {
       DPF( "    Can create offscreen requested" );

       if (caps & DDSCAPS_VIDEOMEMORY) {
#ifndef NVHEAPMGR
           if (pDriverData->HALInfo.vmiData.dwNumHeaps == 0) {
               DPF("NVDD32: CanCreateSurface32 - out of video memory 20");
               lpCanCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
               return DDHAL_DRIVER_HANDLED;
           }
#endif
           if (caps & DDSCAPS_OVERLAY) {
               DPF("NVDD32: CanCreateSurface32 - invalid pixel format 140");
               lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
               return DDHAL_DRIVER_HANDLED;
           }

           if (caps & DDSCAPS_FLIP) {
               if (lpDDSurface->dwHeight > (unsigned long)pDriverData->bi.biHeight) {
                   if (lpDDSurface->dwFlags & DDSD_HEIGHT) {
                       DPF("NVDD32: CanCreateSurface32 - height too big 30");
                       lpCanCreateSurface->ddRVal = DDERR_TOOBIGHEIGHT;
                       return DDHAL_DRIVER_HANDLED;
                   } else {
                       lpDDSurface->dwHeight = (unsigned long)pDriverData->bi.biHeight;
                   }
               }
               if (lpDDSurface->dwWidth > (unsigned long)pDriverData->bi.biWidth) {
                   if (lpDDSurface->dwFlags & DDSD_WIDTH) {
                       DPF("NVDD32: CanCreateSurface32 - width too big 30");
                       lpCanCreateSurface->ddRVal = DDERR_TOOBIGWIDTH;
                       return DDHAL_DRIVER_HANDLED;
                   } else {
                       lpDDSurface->dwWidth = (unsigned long)pDriverData->bi.biWidth;
                   }

               }
           }
       }
       lpCanCreateSurface->ddRVal = DD_OK;
       DPF( "    Can create offscreen granted" );
       return DDHAL_DRIVER_HANDLED;

   } else if (caps & (DDSCAPS_FLIP | DDSCAPS_VIDEOMEMORY | DDSCAPS_ZBUFFER)) {
       DPF( "    Can create flipping surface requested" );

       if (caps & DDSCAPS_OVERLAY) {
           DPF("NVDD32: CanCreateSurface32 - invalid pixel format 150");
           lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
           return DDHAL_DRIVER_HANDLED;
       }

       if (caps & DDSCAPS_ZBUFFER) {
           if ((pDriverData->bi.biBitCount != 16) &&
               (pDriverData->bi.biBitCount != 32)) {
               DPF("NVDD32: CanCreateSurface32 - invalid pixel format 160");
               lpCanCreateSurface->ddRVal = DDERR_INVALIDPIXELFORMAT;
               return DDHAL_DRIVER_HANDLED;
           }
       }

#ifndef NVHEAPMGR
       if (pDriverData->HALInfo.vmiData.dwNumHeaps == 0) {
           DPF("NVDD32: CanCreateSurface32 - out of video memory 30");
           lpCanCreateSurface->ddRVal = DDERR_OUTOFVIDEOMEMORY;
           return DDHAL_DRIVER_HANDLED;
       }
       else
#endif
       {
           DPF( "    Can create flipping surface available" );
           lpCanCreateSurface->ddRVal = DD_OK;
           return DDHAL_DRIVER_HANDLED;
       }

   } else if (caps & DDSCAPS_SYSTEMMEMORY) {
       lpCanCreateSurface->ddRVal = DD_OK;
       DPF( "    Can create system memory requested" );
       return DDHAL_DRIVER_HANDLED;
   }


#ifdef NVD3D
   if (caps & DDSCAPS_TEXTURE) {
       lpCanCreateSurface->ddRVal = DD_OK;
       return DDHAL_DRIVER_HANDLED;
   }
#endif

   return DDHAL_DRIVER_NOTHANDLED;

} /* CanCreateSurface32 */

/***************************************************************************
 * DestroyDriver32
 ***************************************************************************/

#ifndef WINNT
HRESULT __stdcall DestroyDriver32(LPDDHAL_DESTROYDRIVERDATA pDestroyDriverData)
#else
HRESULT __stdcall DestroyDriver32(GLOBALDATA *pGlobalData)
#endif // #ifdef WINNT
{
    FAST Nv10ControlDma         *npDev;
    FAST Nv3ChannelPio          *npDevVideo;
    LPDDRAWI_DIRECTDRAW_GBL     pdrv;
    DWORD                       index = 0;

    DPF("DestroyDriver32");
#ifndef WINNT
    pdrv = pDestroyDriverData->lpDD;
#else
    /*
     * Load global ptr variable to "global" data
     */
    pDriverData = pGlobalData;

#endif

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
    npDevVideo = (Nv3ChannelPio *)pDriverData->NvDevVideoFlatPio;

    /* If an overlay is currently active then shut it down */
    if ((npDevVideo != NULL) && (pDriverData->dwOverlaySurfaces > 0)) {
        FAST long videoFreeCount = pDriverData->NvVideoFreeCount;

        NV_DD_DMA_PUSHER_SYNC();

        /* Trash spare subchannel */
        pDriverData->dwRingZeroMutex = TRUE;

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

            while (videoFreeCount < 12)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 12;

            npDevVideo->dDrawSpare.SetObject = NV_VIDEO_OVERLAY;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.StopOverlay[0] =
                NV07A_STOP_OVERLAY_BETWEEN_BUFFERS;

            npDevVideo->dDrawSpare.Nv10VideoOverlay.StopOverlay[1] =
                NV07A_STOP_OVERLAY_BETWEEN_BUFFERS;

        } else { // pre NV10

            while (videoFreeCount < 20)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 20;

            npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_COLOR_KEY;

            npDevVideo->dDrawSpare.videoColorKey.SetSize = 0; // hide overlay

            npDevVideo->dDrawSpare.SetObject = NV_DD_YUV422_VIDEO_FROM_MEMORY;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[0].size = 0;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[0].notify = 0;
        }

        pDriverData->dwRingZeroMutex = FALSE;
        pDriverData->dDrawSpareSubchannelObject = 0;

        pDriverData->dwOverlayOwner = 0;

        pDriverData->dwOverlaySurfaces = 0;

        while (index < NV_MAX_OVERLAY_SURFACES)
            pDriverData->dwOverlaySurfaceLCL[index++] = 0;

        pDriverData->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;

        pDriverData->NvVideoFreeCount = (short)videoFreeCount;
    }

    /* Unlock and free any remaining locked down system memory */
    if (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] > 0) {
        if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] <
                 pDriverData->BaseAddress) || /* If system memory */
            (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >
                 (unsigned long)pDriverData->VideoHeapEnd)) {
            NvNotification *pDmaSyncNotifier =
                (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
            unsigned long *dmaPusherPutAddress =
                (unsigned long *)pDriverData->NvDmaPusherPutAddress;
            unsigned long *dmaPusherBufferBase =
                (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
            long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
            NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
            long freeCount = 0;
#endif  /* CACHE_FREECOUNT */

            if (npDev != NULL) {

                NV_DD_DMA_PUSHER_SYNC();

                // MUST wait for any pending notification to prevent possible loss of notification serialization
                while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

                while (freeCount < 28)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 28, dmaPusherPutAddress);
                freeCount -= 28;

                pDriverData->dDrawSpareSubchannelObject = 0;

                pDmaSyncNotifier->status = NV_IN_PROGRESS;

                /* Trash spare subchannel */
                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
                dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                dmaPusherPutAddress[5] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                dmaPusherPutAddress[7] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
                dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                dmaPusherPutAddress[9] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                dmaPusherPutAddress[11] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[12] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                dmaPusherPutAddress[13] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
                dmaPusherPutAddress[14] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                dmaPusherPutAddress[15] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[16] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                dmaPusherPutAddress[17] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[18] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                dmaPusherPutAddress[19] = NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
                dmaPusherPutAddress[20] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_OUT_OFFSET | 0x40000;
                dmaPusherPutAddress[21] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[22] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_BUFFER_IN_OFFSET | 0x40000;
                dmaPusherPutAddress[23] = NV1_NULL_OBJECT;
                dmaPusherPutAddress[24] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
                dmaPusherPutAddress[25] = 0;
                dmaPusherPutAddress[26] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
                dmaPusherPutAddress[27] = 0;

                dmaPusherPutAddress += 28;

                pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                /* Force write combine buffer to flush */
                pDriverData->NvDmaPusherBufferEnd[0] = 0;
                /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                _outp (0x3d0,0);

                pDriverData->dwDmaPusherFreeCount = freeCount;

                npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

                NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                         NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
            }

            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] = 0;
            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[3] = 0;

            GlobalUnlock((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);
            GlobalFree((HGLOBAL)pDriverData->NvFloatingMem2MemFormatMemoryHandle);

            pDriverData->NvFloatingMem2MemFormatMemoryHandle = 0;

        } else { /* If video memory */
            NVHEAP_FREE (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0]);
            //DDHAL32_VidMemFree(pdrv, 0, pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0]);

            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] = 0;
            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1] = 0;
            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2] = 0;
            pDriverData->NvFloatingMem2MemFormatSurfaceAddress[3] = 0;
        }
    }

    /* Unlock and free any remaining locked down system memory */

#ifndef WINNT
    pDriverData->HALCallbacks.lpSetInfo = 0;
#endif

    if (pDriverData->NvDevFlatDma != 0) {
        NvNotification *npDmaSyncNotifier =
            (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
        long freeCount = 0;
        unsigned long *dmaPusherPutAddress =
            (unsigned long *)pDriverData->NvDmaPusherPutAddress;
        unsigned long *dmaPusherBufferBase =
            (unsigned long *)pDriverData->NvDmaPusherBufferBase;
        /*
         * Ensure the push buffer has completed prior to deleting objects
         * which it might reference.
         */
        while ((volatile)npDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

        while (freeCount < 4)
            NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
        freeCount -= 4;

        npDmaSyncNotifier->status = NV_IN_PROGRESS;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
            RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
        dmaPusherPutAddress[1] = 0;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
            RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
        dmaPusherPutAddress[3] = 0;

        dmaPusherPutAddress += 4;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        NV_DD_DMA_PUSHER_SYNC();
        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
        pDriverData->NvDmaPusherPutAddress = dmaPusherPutAddress;
        while ((volatile)npDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

        /* Unlock and free any remaining locked down system memory */

        /*
        * This routine will insure that any allocated texture heap is destroyed.
        */
        D3DDestroyTextureContexts();
        if (pDriverData->DDrawVideoSurfaceCount > 0)
            if (!NvDDDisable32())
                return(FALSE);
        if (NvRmFree(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                 NV_DD_DEV_DMA) != NVOS00_STATUS_SUCCESS)
                    return(FALSE);
        pDriverData->NvDevFlatDma = 0;
    }

    /*
     * This routine will insure that any allocated texture heap is destroyed.
     */

    if (pDriverData->dwDmaPusherCtxDmaSize) {

        if (NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                 NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(FALSE);

        pDriverData->dwDmaPusherCtxDmaSize = 0;
    }

    if (pDriverData->NvAGPDmaPusherBufferBase != 0) {
        /* Make sure we destroy the AGP push buffer context DMA */
#ifndef WINNT
        DDHAL32_VidMemFree(pdrv, AGP_HEAP, pDriverData->NvAGPDmaPusherBufferBase);
#endif // WINNT
        pDriverData->NvAGPDmaPusherBufferBase = 0;
    }

    if ((pDriverData->NvDeviceVersion < NV_DEVICE_VERSION_10) &&
        (pDriverData->NvDevVideoFlatPio != 0)) {
        if (NvRmFree(pDriverData->dwRootHandle, NV_WIN_DEVICE,
                 NV_DD_DEV_VIDEO) != NVOS00_STATUS_SUCCESS)
                    return(FALSE);
        pDriverData->NvDevVideoFlatPio = 0;
    }

    pDriverData->extra422OverlayOffset[0] = 0;
    pDriverData->extra422OverlayOffset[1] = 0;

    // Let 16 bit Display Driver code know that Direct Draw is no longer active
    pDriverData->DDrawVideoSurfaceCount = 0;

    pDriverData->fNvActiveFloatingContexts = 0;

    // Reset AGP GART addresses
    pDriverData->GARTLinearBase = 0;
    pDriverData->GARTPhysicalBase = 0;

    pDriverData->wDDrawActive = (WORD)FALSE; // let everyone know DDraw is no longer active
    pDriverData->dwRingZeroMutex = FALSE;
#ifdef WINNT
    if (NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
             NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(FALSE);
    if (NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
             NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(FALSE);
    if (NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
             NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(FALSE);
#endif // #ifdef WINNT

#ifndef WINNT
    pDestroyDriverData->ddRVal = DD_OK;
#endif // #ifndef WINNT
    return DDHAL_DRIVER_HANDLED;

} /* DestroyDriver32 */

#ifndef WINNT
/*
 * GetHeapAlignment32
 *
 * Returns more specific heap alignment requirements to DDRAW than
 * those described in the heap structure.
 */

DWORD __stdcall GetHeapAlignment32(LPDDHAL_GETHEAPALIGNMENTDATA lpGhaData) {

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

        lpGhaData->Alignment.Overlay.Linear.dwStartAlignment = NV_BYTE_ALIGNMENT;
        lpGhaData->Alignment.Overlay.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT;

        lpGhaData->Alignment.Texture.Linear.dwStartAlignment = NV_BYTE_ALIGNMENT; /* Texture surfaces, not our optimized texture alignment */
        lpGhaData->Alignment.Texture.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT;

        lpGhaData->Alignment.ZBuffer.Linear.dwStartAlignment = NV_BYTE_ALIGNMENT;
        lpGhaData->Alignment.ZBuffer.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT;

        lpGhaData->Alignment.AlphaBuffer.Linear.dwStartAlignment = NV_BYTE_ALIGNMENT;
        lpGhaData->Alignment.AlphaBuffer.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT;

        lpGhaData->Alignment.Offscreen.Linear.dwStartAlignment = NV_BYTE_ALIGNMENT;
        lpGhaData->Alignment.Offscreen.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT;

        lpGhaData->Alignment.FlipTarget.Linear.dwStartAlignment = NV_SCAN_OUT_BYTE_ALIGNMENT;
        lpGhaData->Alignment.FlipTarget.Linear.dwPitchAlignment = NV_BYTE_ALIGNMENT;

    } else {
        DPF("D3D: GetHeapAlignment32 - invalid parameters 10");
        lpGhaData->ddRVal = DDERR_INVALIDPARAMS;
        return DDHAL_DRIVER_NOTHANDLED;
    }

    return DDHAL_DRIVER_HANDLED;

} /* GetHeapAlignment32 */


/*
 * UpdateNonLocalHeap32
 *
 * Records actual AGP memory linear and physical addresses.
 */

DWORD __stdcall UpdateNonLocalHeap32( LPDDHAL_UPDATENONLOCALHEAPDATA lpd )
{
    unsigned long oldDmaPusherBufferBase;

    NvSetDriverDataPtr(lpd->lpDD);

    oldDmaPusherBufferBase = pDriverData->NvAGPDmaPusherBufferBase;

    if( lpd->dwHeap == AGP_HEAP ) { /* AGP heap */
        /* This it the linear non-local heap we use for DMA push buffers, textures and YV12 surfaces... */
        pDriverData->GARTLinearBase = lpd->fpGARTLin;
        pDriverData->GARTPhysicalBase = lpd->fpGARTDev;

        /* Let the resource manager know this information */
        NvConfigSet(NV_CFG_AGP_PHYS_BASE, pDriverData->GARTPhysicalBase, pDriverData->dwDeviceIDNum);
        NvConfigSet(NV_CFG_AGP_LINEAR_BASE, pDriverData->GARTLinearBase, pDriverData->dwDeviceIDNum);
        /* currently limit AGP memory to 32 MB */
        NvConfigSet(NV_CFG_AGP_LIMIT, NV_MAX_AGP_MEMORY_LIMIT, pDriverData->dwDeviceIDNum);

        pDriverData->NvAGPDmaPusherBufferBase =
            (DWORD) DDHAL32_VidMemAlloc(lpd->lpDD, AGP_HEAP, pDriverData->NvDmaPusherBufferSize, 1);

        /* Handle case where AGP DMA push buffer gets moved */
        if ((oldDmaPusherBufferBase) &&
            (oldDmaPusherBufferBase != pDriverData->NvAGPDmaPusherBufferBase) &&
            (pDriverData->dwDmaPusherCtxDmaSize)) {

            if (NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                     NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY) != NVOS00_STATUS_SUCCESS) {
                lpd->ddRVal = DDERR_GENERIC;
                return DDHAL_DRIVER_HANDLED;
            }
            pDriverData->dwDmaPusherCtxDmaSize = 0;
        }
    }

    lpd->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

} /* UpdateNonLocalHeap32 */
#endif // #ifndef WINNT

/*
 * GetAvailDriverMemory32
 *
 * Reports amount of memory that the driver is managing itself.
 */
DWORD __stdcall GetAvailDriverMemory32( LPDDHAL_GETAVAILDRIVERMEMORYDATA lpd )
{
    NvSetDriverDataPtr(lpd->lpDD);

#ifdef NVHEAPMGR
    /*
     * The driver is using the RM to manage video memory.
     */
    lpd->dwTotal = pDriverData->VideoHeapTotal;
    lpd->dwFree  = pDriverData->VideoHeapFree;
#else
    /*
     * The driver is only managing private memory on PCI systems for textures.
     */
    lpd->dwTotal = 0;
    lpd->dwFree  = 0;
#endif
    /*
     * The driver is only managing private memory on PCI systems for textures.
     */
    if ((lpd->DDSCaps.dwCaps & DDSCAPS_TEXTURE) && (pDriverData->GARTLinearBase == 0))
    {
        /* Check to see if D3D has been fired up yet. */
        if (pDriverData->dwTextureHeapSizeMax == 0)
        {
            /*
             * If the current texture heap size is not initialized yet, then this means
             * D3D has not be fired up yet to actually allocate the texture heap.  In this
             * case return the amount that is being requested.  Once D3D is up and running,
             * the memory sizes will be based on what the heap size really is.
             */
            lpd->dwTotal += pDriverData->regTexHeap;
            lpd->dwFree  += pDriverData->regTexHeap;
        }
        else
        {
            /*
             * Return the maximum and available size of the internal PCI
             * system meory texture heap
             */
            lpd->dwTotal += pDriverData->dwTextureHeapSizeMax;
            lpd->dwFree  += pDriverData->dwFreeSize;
        }
    }
    else
    {
        /*
         * For non-texture surface requests or AGP systems, there is no internal memory.
         */
        lpd->dwTotal += 0;
        lpd->dwFree  += 0;
    }
    lpd->ddRVal = DD_OK;
    return (DDHAL_DRIVER_HANDLED);
}


/* Safe procedures, basically if they are called nothing happens */
DWORD FAR PASCAL SafeSyncSurfaceData(LPDDHAL_SYNCSURFACEDATA lpInput)
{
    lpInput->ddRVal= DD_OK;

    return DDHAL_DRIVER_HANDLED;
}

DWORD FAR PASCAL SafeSyncVideoPortData(LPDDHAL_SYNCVIDEOPORTDATA lpInput)
{
    lpInput->ddRVal= DD_OK;

    return DDHAL_DRIVER_HANDLED;
}

// -----------------------------------------------------------------------------
// nvSetOverlayColorControl
//      Sets the colour control for the hardware overlay.  This is a private
//      interface for codec vendors.  The colour controls will be applied to
//      any flippable overlay to the target surface.  Only YUY2 and UYVY surfaces
//      are supported.  If the lpDDCC is NULL, then capabilities are returned,
//      otherwise returns TRUE if succeeded.
//      NOTE to codec vendors: if you are applying this to an anchor frame that
//      needs to be used again, blit it somewhere else first because the frame
//      data is modified in place to the flip target.
DWORD __stdcall nvSetOverlayColorControl(LPDDCOLORCONTROL lpDDCC)
{
    if (lpDDCC) {
#ifdef OVLCCTL
        if (lpDDCC->dwSize != sizeof(DDCOLORCONTROL)) return FALSE;
        pDriverData->OverlayColourControl.lContrast = lpDDCC->lContrast;
        if ((lpDDCC->dwFlags & ~DDCOLOR_CONTRAST) != 0) return FALSE;
        return TRUE;
#else
        return FALSE;
#endif
    } else {
#ifdef OVLCCTL
        return DDCOLOR_CONTRAST;
#else
        return 0;
#endif
    }
}

#ifdef OVLCCTL
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

    if (lBrightness>10000)
        lBrightness = 10,000;

    if (lBrightness<=750)
        {
        dwOffset = 1+ ( 0x3D * (lBrightness/749) );
        }
    else
        {
        dwOffset = (0x69 * (lBrightness/(10,000-750)));
        }

    return dwOffset;
}

#define NV_PVIDEO_RED_CSC 0x680280
#define NV_PVIDEO_GREEN_CSC 0x680284
#define NV_PVIDEO_BLUE_CSC 0x680288

DWORD GetOffsetTranslated()
{
    U032 * nvBase;
    U032 dwRedCsc;

    // all of this is based of the red csc value
    nvBase = (U032 *) (pDriverData->NvBaseFlat);

    dwRedCsc = nvBase[NV_PVIDEO_RED_CSC/4];

    if (dwRedCsc>=0x69)
        {// 750 to 0 range
        return (750 * ((dwRedCsc-0x69)/0x3E));
        }
    else
        {// 750-10,000 range
        return (750 + ((10000-750)* ((0x69-dwRedCsc)/0x69)) );
        }
}


/*
 * Colour Control
 * Extended functionality of DirectDraw/VPE
 */
DWORD FAR PASCAL ColourControl( LPDDHAL_COLORCONTROLDATA pvpcd )
{
    U032 *nvBase;

    if (pvpcd->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_OVERLAY) {

        NvSetDriverDataPtr(pvpcd->lpDD);

        if (pvpcd->dwFlags & DDRAWI_GETCOLOR ) {

            pvpcd->lpColorData->dwSize = sizeof(DDCOLORCONTROL);

            // time being can only do brightness and contrast
            pvpcd->lpColorData->dwFlags = DDCOLOR_BRIGHTNESS; // DDCOLOR_HUE
            pvpcd->lpColorData->dwFlags = DDCOLOR_CONTRAST;

            // is in units of 1, maximum is 10,000
            pvpcd->lpColorData->lBrightness = GetOffsetTranslated();

            // contrast ranges from 0 to 20,000 (intended to be 0 to 200%)
            // since we can't do overcontrasting, make 20,000 equal to 100% (technically not illegal)
            pvpcd->lpColorData->lContrast = min(pDriverData->OverlayColourControl.lContrast * 20000 / 255, 20000);

            pvpcd->ddRVal = DD_OK;
            return DDHAL_DRIVER_HANDLED;
        }

        if (pvpcd->dwFlags & DDRAWI_SETCOLOR ) {
            // can only do brightness and contrast, any other request is denied
            if (pvpcd->lpColorData->dwFlags & DDCOLOR_BRIGHTNESS) {
                DWORD dwCscOffset;
                DWORD dwCscRed, dwCscGreen, dwCscBlue;

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
                pDriverData->OverlayColourControl.lContrast = min(pvpcd->lpColorData->lContrast * 256 / 20000, 255);
            }
            if (pvpcd->lpColorData->dwFlags & (DDCOLOR_BRIGHTNESS | DDCOLOR_CONTRAST)) {
                pvpcd->ddRVal = DD_OK;
                return DDHAL_DRIVER_HANDLED;
            }
        }
    }

    return DDHAL_DRIVER_NOTHANDLED;
}
#endif

#ifndef WINNT
/*
 * GetDriverInfo32
 *
 * Initialize Extended Functionality Classes
 */

DWORD __stdcall GetDriverInfo32(LPDDHAL_GETDRIVERINFODATA lpData)
{
    pDriverData = (GLOBALDATA *)lpData->dwContext;

    lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;

    if (IsEqualIID(&(lpData->guidInfo), &GUID_MiscellaneousCallbacks) ) {
        DDHAL_DDMISCELLANEOUSCALLBACKS miscCB;
        DWORD dwSize = lpData->dwExpectedSize;

        if ( sizeof(miscCB) < dwSize )
            dwSize = sizeof(miscCB);
        lpData->dwActualSize = sizeof(miscCB);
        memset(&miscCB, 0, dwSize);
        miscCB.dwSize = dwSize;

        miscCB.dwFlags =
#ifndef WINNT
                         DDHAL_MISCCB32_GETHEAPALIGNMENT |
                         DDHAL_MISCCB32_UPDATENONLOCALHEAP |
#endif
                         DDHAL_MISCCB32_GETAVAILDRIVERMEMORY;

#ifndef WINNT
        miscCB.GetHeapAlignment = GetHeapAlignment32;
        miscCB.UpdateNonLocalHeap = UpdateNonLocalHeap32;
#endif
        miscCB.GetAvailDriverMemory = GetAvailDriverMemory32;

        memcpy(lpData->lpvData, &miscCB, dwSize );
        lpData->ddRVal = DD_OK;
    }

#ifdef  DX7
    if (IsEqualIID(&(lpData->guidInfo), &GUID_Miscellaneous2Callbacks) ) {
        DDHAL_DDMISCELLANEOUS2CALLBACKS misc2CB;
        DWORD dwSize = lpData->dwExpectedSize;

        if ( sizeof(misc2CB) < dwSize )
            dwSize = sizeof(misc2CB);
        lpData->dwActualSize = sizeof(misc2CB);
        memset(&misc2CB, 0, dwSize);
        misc2CB.dwSize = dwSize;

        misc2CB.dwFlags = DDHAL_MISC2CB32_ALPHABLT;

        misc2CB.AlphaBlt = Blit32;  /* Use common callback with Blit32 calls */

        memcpy(lpData->lpvData, &misc2CB, dwSize );
        lpData->ddRVal = DD_OK;
    }
#endif  /* DX7 */

    if( IsEqualGUID( &lpData->guidInfo, &GUID_NonLocalVidMemCaps ) ) {
        DWORD dwSize;
        LPDDNONLOCALVIDMEMCAPS lpCaps;
        int i;

        /* Size validation code omitted for clarity */
        dwSize = min(lpData->dwExpectedSize, sizeof(DDNONLOCALVIDMEMCAPS));
        lpCaps = (LPDDNONLOCALVIDMEMCAPS)(lpData->lpvData);
        lpCaps->dwSize = dwSize;

        lpCaps->dwNLVBCaps = ddHALInfo.ddCaps.dwCaps;
        lpCaps->dwNLVBCaps2 = ddHALInfo.ddCaps.dwCaps2;
        lpCaps->dwNLVBCKeyCaps = ddHALInfo.ddCaps.dwCKeyCaps;
        lpCaps->dwNLVBFXCaps = ddHALInfo.ddCaps.dwFXCaps;

        for( i=0; i < DD_ROP_SPACE; i++ )
            lpCaps->dwNLVBRops[i] = ropList[i];

        lpData->dwActualSize = dwSize;
        lpData->ddRVal = DD_OK;
    }


    if (IsEqualIID(&(lpData->guidInfo), &GUID_VideoPortCallbacks) ) {
        DDHAL_DDVIDEOPORTCALLBACKS vpCB;
        DWORD dwSize = lpData->dwExpectedSize;

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

#ifdef OVLCCTL
    if (IsEqualIID(&(lpData->guidInfo),&GUID_ColorControlCallbacks) ) {
        DDHAL_DDCOLORCONTROLCALLBACKS ccCB;
        DWORD dwSize = lpData->dwExpectedSize;

        lpData->dwActualSize = sizeof( DDHAL_DDCOLORCONTROLCALLBACKS );

        ccCB.dwSize = (dwSize<lpData->dwActualSize)?
                        dwSize:lpData->dwActualSize;


        ccCB.dwFlags = DDHAL_COLOR_COLORCONTROL;
        ccCB.ColorControl = ColourControl;

        memcpy(lpData->lpvData, &ccCB, ccCB.dwSize );

        lpData->ddRVal = DD_OK;
    }
#endif

    if (IsEqualIID(&(lpData->guidInfo), &GUID_VideoPortCaps) ) {
        DDVIDEOPORTCAPS VideoPortCaps;
        DDVIDEOPORTCAPS *pVideoPortCapsDest = lpData->lpvData;
        DWORD dwIndex;

        // expecting query for 1 video port
        if (lpData->dwExpectedSize != (sizeof(VideoPortCaps)) )
            return DDHAL_DRIVER_HANDLED;

        lpData->dwActualSize = (sizeof(VideoPortCaps) );

    // index changed to allow only 1 report resolution, for time being only NTSC
    // until class structure is changed to test PAL etc
        for (dwIndex = 0; dwIndex < 1; dwIndex++) {

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
//        VideoPortCaps.dwMaxWidth  = ST_VPE_MAX_INPUT_X;
//       VideoPortCaps.dwMaxVBIWidth = ST_VPE_MAX_INPUT_X;
//      VideoPortCaps.dwMaxHeight   = ST_VPE_MAX_INPUT_Y;
        VideoPortCaps.dwMaxWidth    = 4096;     // must talk to joe and get the details on this..
        VideoPortCaps.dwMaxVBIWidth = 4096;
        VideoPortCaps.dwMaxHeight   = 640;      // just arbitarily larger than PAL field *2 for progressive

// could have share even/odd, but for now keep things simple

        VideoPortCaps.dwCaps =
                                DDVPCAPS_AUTOFLIP       |
                                DDVPCAPS_INTERLACED     |
                                DDVPCAPS_NONINTERLACED  |
//                              DDVPCAPS_SHAREABLE      |
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


        VideoPortCaps.dwAlignVideoPortBoundary      = NV_BYTE_ALIGNMENT;
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
        }
        lpData->ddRVal = DD_OK;
    }

    if (IsEqualIID(&(lpData->guidInfo), &GUID_KernelCallbacks) ) {
        DDHAL_DDKERNELCALLBACKS kCB;
        DWORD dwSize = lpData->dwExpectedSize;

        if ( sizeof(kCB) < dwSize )
            dwSize = sizeof(kCB);
        lpData->dwActualSize = sizeof(kCB);

        kCB.dwSize = sizeof(DDHAL_DDKERNELCALLBACKS);
        kCB.dwFlags = DDHAL_KERNEL_SYNCSURFACEDATA | DDHAL_KERNEL_SYNCVIDEOPORTDATA;
        kCB.SyncSurfaceData = SafeSyncSurfaceData;          // we're not doing anything on EITHER of these yet
        kCB.SyncVideoPortData = SafeSyncVideoPortData;

        memcpy(lpData->lpvData, &kCB, dwSize );
        //lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        lpData->ddRVal = DD_OK;
    }

    if (IsEqualIID(&(lpData->guidInfo), &GUID_KernelCaps) ) {
        DDKERNELCAPS KernelCaps;

        if (lpData->dwExpectedSize != sizeof(DDKERNELCAPS) )
            return DDHAL_DRIVER_HANDLED;

        lpData->dwActualSize = sizeof(DDKERNELCAPS);

        KernelCaps.dwSize = sizeof(DDKERNELCAPS);
        KernelCaps.dwCaps =
            DDKERNELCAPS_SKIPFIELDS         |
            DDKERNELCAPS_AUTOFLIP           |
            DDKERNELCAPS_SETSTATE           |
            DDKERNELCAPS_LOCK                   |
//          DDKERNELCAPS_FLIPVIDEOPORT      |
            DDKERNELCAPS_FLIPOVERLAY        |
//          DDKERNELCAPS_CAPTURE_SYSMEM |
//          DDKERNELCAPS_CAPTURE_NONLOCALVIDMEM |
            DDKERNELCAPS_FIELDPOLARITY      ;
//          DDKERNELCAPS_CAPTURE_INVERTED   ;

        KernelCaps.dwIRQCaps = DDIRQ_VPORT0_VSYNC |
//                                      DDIRQ_BUSMASTER;
//                                      #define DDIRQ_BUSMASTER             0x00000002l
                                        0x00000002;

//          DDIRQ_DISPLAY_VSYNC |
//          DDIRQ_RESERVED1     |
//          DDIRQ_VPORT0_VSYNC          // this is the only IRQ we'll be supporting
//          DDIRQ_VPORT0_LINE       |
//          DDIRQ_VPORT1_VSYNC  |
//          DDIRQ_VPORT1_LINE   |

        memcpy(lpData->lpvData, &KernelCaps, sizeof(DDKERNELCAPS) );
        //lpData->ddRVal = DDERR_CURRENTLYNOTAVAIL;
        lpData->ddRVal = DD_OK;
    }

#ifdef  DX7
    if (IsEqualIID(&(lpData->guidInfo), &GUID_DDMoreCaps) ) {
        DDMORECAPS ddMoreCaps;

        if (lpData->dwExpectedSize != sizeof(DDMORECAPS) )
            return DDHAL_DRIVER_HANDLED;

        lpData->dwActualSize = sizeof(DDMORECAPS);

        ddMoreCaps.dwSize = sizeof(DDMORECAPS);

        ddMoreCaps.dwAlphaCaps = DDALPHACAPS_BLTALPHAPIXELS |
                                 DDALPHACAPS_BLTSATURATE |
                                 DDALPHACAPS_BLTPREMULT |
//                                 DDALPHACAPS_BLTNONPREMULT |
                                 DDALPHACAPS_BLTRGBASCALE1F |
                                 DDALPHACAPS_BLTRGBASCALE4F |
                                 DDALPHACAPS_BLTALPHAFILL;

        ddMoreCaps.dwSVBAlphaCaps = DDALPHACAPS_BLTALPHAPIXELS |
                                    DDALPHACAPS_BLTSATURATE |
                                    DDALPHACAPS_BLTPREMULT |
//                                    DDALPHACAPS_BLTNONPREMULT |
                                    DDALPHACAPS_BLTRGBASCALE1F |
                                    DDALPHACAPS_BLTRGBASCALE4F |
                                    DDALPHACAPS_BLTALPHAFILL;

        ddMoreCaps.dwFilterCaps = DDFILTCAPS_BLTBILINEARFILTER |
                                  DDFILTCAPS_BLTFLATFILTER |
                                  DDFILTCAPS_OVERLAYBILINEARFILTER;

        ddMoreCaps.dwSVBFilterCaps = DDFILTCAPS_BLTBILINEARFILTER |
                                     DDFILTCAPS_BLTFLATFILTER;

        memcpy(lpData->lpvData, &ddMoreCaps,
            sizeof(DDMORECAPS) );
        lpData->ddRVal = DD_OK;
    }
#endif  /* DX7 */


#ifdef NVD3D
    /*
     * Call D3D HAL to see if there's any GUIDs it wants to handle.
     */
    D3DGetDriverInfo(lpData);
#endif

    return DDHAL_DRIVER_HANDLED;

} /* GetDriverInfo32 */
#endif // #ifndef WINNT

/*
 * buildDDHALInfo32
 *
 * build DDHALInfo structure
 */
BOOL buildDDHALInfo32()
{
   int i;

    /*
     * modify the structures inour shared window with the 16bit driver
     */
    #define cbDDCallbacks        pDriverData->DDCallbacks
    #define cbDDSurfaceCallbacks pDriverData->DDSurfaceCallbacks
    #define cbDDPaletteCallbacks pDriverData->DDPaletteCallbacks

    /*
     * fill out the main driver callbacks
     */
    memset(&cbDDCallbacks, 0, sizeof(DDHAL_DDCALLBACKS));
    cbDDCallbacks.dwSize                = sizeof(DDHAL_DDCALLBACKS);
//    cbDDCallbacks.SetMode               = SetMode32;   Never called by DDRAW
    cbDDCallbacks.WaitForVerticalBlank  = WaitForVerticalBlank32;
    cbDDCallbacks.GetScanLine           = GetScanLine32;
    cbDDCallbacks.CreateSurface         = CreateSurface32;
    cbDDCallbacks.CanCreateSurface      = CanCreateSurface32;
#ifndef WINNT       // this is called from DrvDisableDirectDraw
    cbDDCallbacks.DestroyDriver         = DestroyDriver32;
#endif              // WINNT

    cbDDCallbacks.dwFlags               = DDHAL_CB32_WAITFORVERTICALBLANK |
//                                          DDHAL_CB32_SETMODE |
                                          DDHAL_CB32_GETSCANLINE |
                                          DDHAL_CB32_CANCREATESURFACE |
                                          DDHAL_CB32_CREATESURFACE |
                                          DDHAL_CB32_DESTROYDRIVER;

    /*
     * fill out the palette callbacks
     */
    memset(&cbDDPaletteCallbacks, 0, sizeof(DDHAL_DDPALETTECALLBACKS));
    cbDDPaletteCallbacks.dwSize  = sizeof(DDHAL_DDPALETTECALLBACKS);
    cbDDPaletteCallbacks.dwFlags = 0;

    /*
     * fill out the HALINFO
     */
    memset(&ddHALInfo, 0, sizeof(DDHALINFO));
    ddHALInfo.dwSize = sizeof(DDHALINFO);

    /*
     * callback functions
     */
#ifndef WINNT
    ddHALInfo.lpDDCallbacks        = &cbDDCallbacks;
    ddHALInfo.lpDDSurfaceCallbacks = &cbDDSurfaceCallbacks;
    ddHALInfo.lpDDPaletteCallbacks = &cbDDPaletteCallbacks;

    /*
     * return our HINSTANCE
     */
    ddHALInfo.hInstance = (DWORD)hInstance;

    /*
     * pdevice
     */
    ddHALInfo.lpPDevice = (LPVOID)pDriverData->lpPDevice;
#else
    ddHALInfo.lpD3DHALCallbacks        = &cbDDCallbacks;
#endif // #ifdef WINNT

    /*
     * ROPS supported
     */

    for( i=0; i < DD_ROP_SPACE; i++ )
        ddHALInfo.ddCaps.dwRops[i] = ropList[i];

    for( i=0; i < DD_ROP_SPACE; i++ )
        ddHALInfo.ddCaps.dwSVBRops[i] = ropList[i];


   /*
    * fill out the surface callbacks
    */
    memset(&cbDDSurfaceCallbacks, 0, sizeof(DDHAL_DDSURFACECALLBACKS));
    cbDDSurfaceCallbacks.dwSize             = sizeof(DDHAL_DDSURFACECALLBACKS);
    cbDDSurfaceCallbacks.DestroySurface     = DestroySurface32;
    cbDDSurfaceCallbacks.Blt                = Blit32;
    cbDDSurfaceCallbacks.Flip               = Flip32;
    cbDDSurfaceCallbacks.Lock               = Lock32;
    cbDDSurfaceCallbacks.Unlock             = Unlock32;
    cbDDSurfaceCallbacks.GetBltStatus       = GetBltStatus32;
    cbDDSurfaceCallbacks.GetFlipStatus      = GetFlipStatus32;
    cbDDSurfaceCallbacks.UpdateOverlay      = UpdateOverlay32;
    cbDDSurfaceCallbacks.SetOverlayPosition = SetOverlayPosition32;
    cbDDSurfaceCallbacks.SetColorKey        = SetSurfaceColorKey32;
    cbDDSurfaceCallbacks.dwFlags            = DDHAL_SURFCB32_BLT |
                                              DDHAL_SURFCB32_FLIP |
                                              DDHAL_SURFCB32_LOCK |
                                              DDHAL_SURFCB32_UNLOCK |
                                              DDHAL_SURFCB32_SETCOLORKEY |
                                              DDHAL_SURFCB32_GETBLTSTATUS |
                                              DDHAL_SURFCB32_GETFLIPSTATUS |
                                              DDHAL_SURFCB32_UPDATEOVERLAY |
                                              DDHAL_SURFCB32_SETOVERLAYPOSITION |
                                              DDHAL_SURFCB32_DESTROYSURFACE;

    /*
     * capabilities supported
     */

    ddHALInfo.ddCaps.dwCaps = DDCAPS_ALIGNSTRIDE |
                              DDCAPS_ALIGNBOUNDARYDEST |
                              DDCAPS_GDI |
                              DDCAPS_BLT |
                              DDCAPS_BLTFOURCC |
                              DDCAPS_BLTSTRETCH |
                              DDCAPS_BLTQUEUE |
                              DDCAPS_BLTCOLORFILL |
                              DDCAPS_CANBLTSYSMEM |
#ifdef  DX7
                              DDCAPS_ALPHA |
#endif  /* DX7 */
                              DDCAPS_COLORKEY |
                              DDCAPS_OVERLAY |
                              DDCAPS_OVERLAYCANTCLIP  |
                              DDCAPS_OVERLAYFOURCC |
                              DDCAPS_OVERLAYSTRETCH |
                              DDCAPS_READSCANLINE |
                              DDCAPS_BLTDEPTHFILL |
                              DDCAPS_3D;

    ddHALInfo.ddCaps.dwCaps2 |= DDCAPS2_NOPAGELOCKREQUIRED |
                                DDCAPS2_WIDESURFACES |
                                DDCAPS2_FLIPNOVSYNC |
#ifndef WINNT
                                DDCAPS2_PRIMARYGAMMA |
#endif // #ifndef WINNT
                                DDCAPS2_CANRENDERWINDOWED |
                                DDCAPS2_CANBOBHARDWARE |
                                DDCAPS2_CANBOBINTERLEAVED |
                                DDCAPS2_CANFLIPODDEVEN |
#ifdef OVLCCTL
                                DDCAPS2_COLORCONTROLOVERLAY |
#endif
                                DDCAPS2_NONLOCALVIDMEMCAPS;

    ddHALInfo.ddCaps.dwSVBCaps = ddHALInfo.ddCaps.dwCaps;

    ddHALInfo.ddCaps.dwSVBCaps2 = ddHALInfo.ddCaps.dwCaps2;


    ddHALInfo.ddCaps.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY |
                                      DDSCAPS_SYSTEMMEMORY |
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
#ifdef  DX7
                                      DDSCAPS_ALPHA |
#endif  /* DX7 */
                                      DDSCAPS_TEXTURE |
                                      DDSCAPS_MIPMAP;

    ddHALInfo.ddCaps.dwFXAlphaCaps  = 0;

    ddHALInfo.ddCaps.dwCKeyCaps = DDCKEYCAPS_SRCBLT |
                                  DDCKEYCAPS_DESTOVERLAY |
                                  DDCKEYCAPS_DESTOVERLAYONEACTIVE;

    ddHALInfo.ddCaps.dwSVBCKeyCaps = ddHALInfo.ddCaps.dwCKeyCaps;

    ddHALInfo.ddCaps.dwFXCaps = DDFXCAPS_BLTARITHSTRETCHY |
//                              DDFXCAPS_BLTMIRRORLEFTRIGHT |
                              DDFXCAPS_BLTMIRRORUPDOWN |
                              DDFXCAPS_BLTSHRINKX |
                              DDFXCAPS_BLTSHRINKY |
                              DDFXCAPS_BLTSTRETCHX |
                              DDFXCAPS_BLTSTRETCHY |
#ifdef  DX7
                              DDFXCAPS_BLTALPHA |
                              DDFXCAPS_BLTFILTER |
                              DDFXCAPS_OVERLAYFILTER |
                              DDFXCAPS_OVERLAYMIRRORLEFTRIGHT |
#endif  /* DX7 */
                              DDFXCAPS_OVERLAYMIRRORUPDOWN |
                              DDFXCAPS_OVERLAYSHRINKX |
                              DDFXCAPS_OVERLAYSHRINKY |
                              DDFXCAPS_OVERLAYSTRETCHX |
                              DDFXCAPS_OVERLAYARITHSTRETCHY |
                              DDFXCAPS_OVERLAYSTRETCHY;

    ddHALInfo.ddCaps.dwSVBFXCaps = ddHALInfo.ddCaps.dwFXCaps;



    /*
     * Z buffer bit depths supported
     */

    ddHALInfo.ddCaps.dwZBufferBitDepths = DDBD_16 | DDBD_24;

    /*
     * NV3 Required alignments
     */

    ddHALInfo.ddCaps.dwAlignBoundaryDest = 1;
    /* All video memory surfaces must be aligned with aligned strides */
    ddHALInfo.ddCaps.dwAlignStrideAlign = NV_BYTE_ALIGNMENT;

    /*
     * NV3 stretching capabilities
     */

    ddHALInfo.ddCaps.dwMinOverlayStretch = 100;
    ddHALInfo.ddCaps.dwMaxOverlayStretch = 20000;

    ddHALInfo.ddCaps.dwMinLiveVideoStretch = 100;
    ddHALInfo.ddCaps.dwMaxLiveVideoStretch = 20000;

    ddHALInfo.ddCaps.dwMinHwCodecStretch = 1;
    ddHALInfo.ddCaps.dwMaxHwCodecStretch = 4095000;

    /*
     *  FOURCCs supported
     *
     *  Table address MUST be set in 16 bit portion of Direct Draw Driver but
     *  can add more FOURCC codes to table here if needed.
     */

    ddHALInfo.ddCaps.dwNumFourCCCodes = NV_MAX_FOURCC;

    /*
     *  Maximum number of overlays supported
     */

    ddHALInfo.ddCaps.dwMaxVisibleOverlays = 1;

#ifndef WINNT
    /*
     * current video mode, the 16bit driver will fill this out for real.
     */

    ddHALInfo.dwModeIndex = DDUNSUPPORTEDMODE;
#endif // #ifndef WINNT

    /*
     * required byte alignments of the scan lines for each kind of memory
     * (default is 4, NV3 needs 16, NV4 needs 32, NV10 wants 64
     */
    ddHALInfo.vmiData.dwOffscreenAlign = NV_BYTE_ALIGNMENT;
    ddHALInfo.vmiData.dwOverlayAlign   = NV_BYTE_ALIGNMENT;
    ddHALInfo.vmiData.dwTextureAlign   = NV_BYTE_ALIGNMENT; /* Texture surface, not our optimized texture */
    ddHALInfo.vmiData.dwAlphaAlign     = NV_BYTE_ALIGNMENT;
    ddHALInfo.vmiData.dwZBufferAlign   = NV_BYTE_ALIGNMENT;

#ifdef NVD3D
    pDriverData->lpLast3DSurfaceRendered = 0;
    pDriverData->ThreeDRenderingOccurred = FALSE;

    /*
     * fill out the D3D callbacks
     */
    D3DHALCreateDriver((LPD3DHAL_GLOBALDRIVERDATA*)&ddHALInfo.lpD3DGlobalDriverData,
                       (LPD3DHAL_CALLBACKS*)&ddHALInfo.lpD3DHALCallbacks);
#endif


    /*
     * Video Port Capabilities
     */


   ddHALInfo.ddCaps.dwCaps2 |=  DDCAPS2_VIDEOPORT               |
                                DDCAPS2_CANBOBINTERLEAVED       |
                                DDCAPS2_CANBOBNONINTERLEAVED    |
                                DDCAPS2_AUTOFLIPOVERLAY;

    ddHALInfo.ddCaps.dwMaxVideoPorts = ST_MAX_VIDEOPORTS;
    ddHALInfo.ddCaps.dwCurrVideoPorts = 0;
#ifndef WINNT
    ddHALInfo.GetDriverInfo = GetDriverInfo32;
    ddHALInfo.dwFlags |= DDHALINFO_GETDRIVERINFOSET | DDHALINFO_MODEXILLEGAL;
    DPF("Display: Filling in ddHALInfo.GetDriverInfo: %lx", GetDriverInfo32);
#endif // #ifndef WINNT

    return TRUE;

} /* buildDDHALInfo */

/*
 * DriverInit
 *
 * this is the entry point called by DirectDraw to
 * initialize the 32-bit driver.
 *
 */
DWORD __stdcall DriverInit( DWORD dwDriverData)
{
    DWORD index = 0;

    pDriverData = (GLOBALDATA *)dwDriverData;

    /* Make sure we match the GLOBALDATA structure size of the current display driver */
    if (pDriverData->dwGlobalStructSize < sizeof(GLOBALDATA))
        return(0);

    pDriverData->dwCurrentFrame = 0;
    pDriverData->dwRenderedFrame = 0;
    pDriverData->dwRingZeroMutex = FALSE;

    pDriverData->blitCalled = FALSE;

    pDriverData->NvFirstCall = 1;    // let ddEnable32 know this is the first call

#ifdef WINNT
    pDriverData->dwSurfaceAlignPad = pDriverData->ppdev->ulSurfaceAlign;
    pDriverData->dwSurfaceAlign = pDriverData->ppdev->ulSurfaceAlign + 1;
    pDriverData->dwSurfaceBitAlign = pDriverData->ppdev->ulSurfaceBitAlign;
#else
    pDriverData->dwSurfaceAlignPad = NV_BYTE_ALIGNMENT_PAD;
    pDriverData->dwSurfaceAlign =    NV_BYTE_ALIGNMENT;
    pDriverData->dwSurfaceBitAlign = NV_BIT_ALIGNMENT;
#endif    

    /* Some apps call DriverInit during mode switches */
    if (pDriverData->dwDmaPusherCtxDmaSize) {

        if (NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                 NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(FALSE);

        pDriverData->dwDmaPusherCtxDmaSize = 0;
    }

    pDriverData->NvAGPDmaPusherBufferBase = 0;
#ifndef WINNT
    // dynamically assign pusher space
    //
    //  >= 60Mb memory gets a 4 meg push buffer
    //          else a 2 meg opush buffer
    //      note - the GlobalMemoryStatus function does not return exact
    //             amounts thus we give it some head room
    {
        MEMORYSTATUS ms;
        ms.dwLength = sizeof(ms);
        GlobalMemoryStatus (&ms);
        if (ms.dwTotalPhys >= 60*1024*1024)
            pDriverData->NvDmaPusherBufferSize = NV_DD_DMA_PUSH_BUFFER_SIZE_LRG;
        else
            pDriverData->NvDmaPusherBufferSize = NV_DD_DMA_PUSH_BUFFER_SIZE;
    }
    #else
        pDriverData->NvDmaPusherBufferSize = NV_DD_DMA_PUSH_BUFFER_SIZE;
    #endif        // #ifdef WINNT
    // Reset in case of a mode switch which unloaded the driver
    pDriverData->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;

    pDriverData->extra422OverlayOffset[0] = 0;
    pDriverData->extra422OverlayOffset[1] = 0;

    pDriverData->dwOverlayOwner = 0;

    pDriverData->dwOverlaySurfaces = 0;

    while (index < NV_MAX_OVERLAY_SURFACES)
        pDriverData->dwOverlaySurfaceLCL[index++] = 0;

    pDriverData->dwVideoPortsAvailable = 1;

    pDriverData->OverlayColourControl.dwSize = sizeof(DDCOLORCONTROL);
    pDriverData->OverlayColourControl.dwFlags = DDCOLOR_CONTRAST;
    pDriverData->OverlayColourControl.lBrightness = 0;
    pDriverData->OverlayColourControl.lContrast = 0xFF;
    pDriverData->OverlayColourControl.lHue = 0;
    pDriverData->OverlayColourControl.lSaturation = 0xFF;
    pDriverData->OverlayColourControl.lGamma = 0xFF;
    pDriverData->OverlayColourControl.lColorEnable = 1;

#ifdef FORCED_TRIPLE_BUFFER
    pDriverData->bNeedToAttachBuffer = FALSE;
    pDriverData->lpBuffer3 = NULL;
#endif

    buildDDHALInfo32();

    DPF( "    Successfully built 32 bit HAL Info" );

    pDriverData->DDrawVideoSurfaceCount = 0;

    // Set the reset flag here for a future call to ResetNV
    // Don't do the call here because we don't neccessarily now about AGP memory yet.
    pDriverData->fReset = TRUE;
//    if (pDriverData->fReset == FALSE) {
//        ResetNV();
//    }

    // Can't reinitialize NV here because mode change may not have happened yet.

#ifndef WINNT // BUGBUG -- no vpe for nt right now.
    // this call is for VPE/KMVT stuff, to hook up with and initialize their storage
        HookUpVxdStorage();
#else
#if _WIN32_WINNT >= 0x0500
        if (pDriverData->ppdev->AgpHeapBase)
            {
            LPDDRAWI_DIRECTDRAW_GBL     pdrv = NULL;
            //*******************************************************
            // Initialize GLOBALDATA's copy of AGP base address.
            //*******************************************************
            pDriverData->GARTLinearBase = pDriverData->ppdev->AgpHeapBase;

            //********************************************************
            // Alloc AGP DMA Push Buffer
            //********************************************************
            pDriverData->NvAGPDmaPusherBufferBase =
                (ULONG) pDriverData->GARTLinearBase +
                (ULONG) pDriverData->ppdev->DmaPushBufTotalSize;
                    //pDriverData->NvDmaPusherBufferSize;
            }
#endif // #if _WIN32_WINNT >= 0x0500
#endif  // !WINNT


    return dwDriverData;

} /* DriverInit */


#ifndef WINNT
/*
 * DllMain
 */
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
    hInstance = hModule;

    switch( dwReason ) {

    case DLL_PROCESS_ATTACH:
#ifndef WINNT
        DisableThreadLibraryCalls( hModule );
#endif // #ifndef WINNT
#ifdef  NVD3D
        init_globals();
#endif  // NVD3D
        break;

    case DLL_PROCESS_DETACH:
#ifdef  NVD3D
        // Destroy Item Arrays
        DestroyItemArrays ();
        DestroyIPM();
#endif  // NVD3D
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    default:
        break;
    }

    return TRUE;

} /* DllMain */

/*
 * D3D DX5 Draw Primitive entry points
 */
DWORD __stdcall DrawOnePrimitive32(LPD3DHAL_DRAWONEPRIMITIVEDATA pdopd)
{
    /*
     * Get the pointer to the D3D context data.
     */
    pCurrentContext = (PNVD3DCONTEXT)pdopd->dwhContext;
    if (!pCurrentContext)
    {
        DPF("D3D:DrawOnePrimitive32 - Bad Context");
        pdopd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    NvSetDriverDataPtrFromContext(pCurrentContext);
#if 0 // D3D will take care of waiting for flip by itself
    /*
     * Check if this surface is waiting to be flipped.  If it is, then
     * return to D3D and let it do the right thing.
     */
    if ((pdopd->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK)
        return (DDHAL_DRIVER_HANDLED);
#endif

    return (nvDrawOnePrimitive(pdopd));
}

DWORD __stdcall DrawOneIndexedPrimitive32(LPD3DHAL_DRAWONEINDEXEDPRIMITIVEDATA pdoipd)
{
    /*
     * Get the pointer to the D3D context data.
     */
    pCurrentContext = (PNVD3DCONTEXT)pdoipd->dwhContext;
    if (!pCurrentContext)
    {
        DPF("D3D:DrawOneIndexedPrimitive32 - Bad Context");
        pdoipd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    NvSetDriverDataPtrFromContext(pCurrentContext);

#if 0 // D3D will take care of waiting for flip by itself
    /*
     * Check if this surface is waiting to be flipped.  If it is, then
     * return to D3D and let it do the right thing.
     */
    if ((pdoipd->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK)
        return (DDHAL_DRIVER_HANDLED);
#endif
    return (nvDrawOneIndexedPrimitive(pdoipd));
}

DWORD __stdcall DrawPrimitives32(LPD3DHAL_DRAWPRIMITIVESDATA pdpd)
{
    /*
     * Get the pointer to the D3D context data.
     */
    pCurrentContext = (PNVD3DCONTEXT)pdpd->dwhContext;
    if (!pCurrentContext)
    {
        DPF("D3D:DrawPrimitives32 - Bad Context");
        pdpd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    NvSetDriverDataPtrFromContext(pCurrentContext);

#if 0 // D3D will take care of waiting for flip by itself
    /*
     * Check if this surface is waiting to be flipped.  If it is, then
     * return to D3D and let it do the right thing.
     */
    if ((pdpd->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK)
        return (DDHAL_DRIVER_HANDLED);
#endif

    return (nvDrawPrimitives(pdpd));
}
DWORD __stdcall Clear32(LPD3DHAL_CLEARDATA pcd)
{
    /*
     * Get the pointer to the D3D context data.
     */
    pCurrentContext = (PNVD3DCONTEXT)pcd->dwhContext;
    if (!pCurrentContext)
    {
        DPF("D3D:Clear32 - Bad Context");
        pcd->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    NvSetDriverDataPtrFromContext(pCurrentContext);

#if 0 // D3D will take care of waiting for flip by itself
    /*
     * Check if this surface is waiting to be flipped.  If it is, then
     * return to D3D and let it do the right thing.
     */

//    if ((pcd->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK)
//        return (DDHAL_DRIVER_HANDLED);
    /*
     * Wait until the flip has occurred, otherwise we will start clearing the buffer while
     * it is still visible.
     */
    while ((pcd->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK);
#endif

    return (nvClear(pcd));
}
#endif
#ifdef  NVD3D_DX6
DWORD FAR PASCAL DrawPrimitives2
(
    LPD3DHAL_DRAWPRIMITIVES2DATA pdp2d
)
{
    /*
     * Get the pointer to the D3D context data.
     */
    pCurrentContext = (PNVD3DCONTEXT)pdp2d->dwhContext;
    if (!pCurrentContext)
    {
        DPF("D3D:nvDrawPrimitives2 - Bad Context");
        pdp2d->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (!pCurrentContext->lpLcl)
    {
        pdp2d->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (!pDriverData->NvDevFlatDma)
    {
        pdp2d->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }
    NvSetDriverDataPtrFromContext(pCurrentContext);

#ifdef  WINNT // D3D will take care of waiting for flip by itself
    /*
     * Check if this surface is waiting to be flipped.  If it is, then
     * return to D3D and let it do the right thing.
     */
    if ((pdp2d->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK)
        return (DDHAL_DRIVER_HANDLED);
#endif

    return(nvDrawPrimitives2(pdp2d));
}
DWORD FAR PASCAL Clear2
(
    LPD3DHAL_CLEAR2DATA pc2d
)
{
    /*
     * Get the pointer to the D3D context data.
     */
    pCurrentContext = (PNVD3DCONTEXT)pc2d->dwhContext;
    if (!pCurrentContext)
    {
        DPF("D3D:Clear2 - Bad Context");
        pc2d->ddrval = D3DHAL_CONTEXT_BAD;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (!pCurrentContext->lpLcl)
    {
        pc2d->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }
    if (!pDriverData->NvDevFlatDma)
    {
        pc2d->ddrval = DD_OK;
        return (DDHAL_DRIVER_HANDLED);
    }
    NvSetDriverDataPtrFromContext(pCurrentContext);

#ifdef  WINNT // D3D will take care of waiting for flip by itself
    /*
     * Check if this surface is waiting to be flipped.  If it is, then
     * return to D3D and let it do the right thing.
     */

//    if ((pc2d->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK)
//        return (DDHAL_DRIVER_HANDLED);

    /*
     * Wait until the flip has occurred, otherwise we will start clearing the buffer while
     * it is still visible.
     */
    while ((pc2d->ddrval = updateFlipStatus(pCurrentContext->lpLcl->lpGbl->fpVidMem)) != DD_OK);
#endif
    return(nvClear2(pc2d));
}

/*
 * WaitForVSync
 */
void WaitForVSync (void)
{
    HRESULT ddrval;

    /*
     * wait until flip has occurred
     */
    for (;;)
    {
        ddrval = updateFlipStatus(0);
        if (ddrval == DD_OK) break;
    }
}

#endif

/*
 * DPF debug stuff
 */
#ifdef DEBUG

#define START_STR       "NVDD32: "
#define END_STR         "\r\n"

/*
 * Msg
 */
#ifndef WINNT
void __cdecl DPF(LPSTR szFormat, ...)
{
    char str[256];

    lstrcpy(str, START_STR);
    wvsprintf(str+lstrlen(str), szFormat, (LPVOID)(&szFormat+1));
    lstrcat(str, END_STR);
    OutputDebugString(str);
}
#else
void __cdecl DPF(LPSTR szFormat, ...)
{
    va_list ap;

    va_start(ap, szFormat);
    EngDebugPrint(START_STR, szFormat, ap);
    EngDebugPrint("", "\n", ap);

    va_end(ap);
}
#endif // #ifdef WINNT

#endif // DEBUG
