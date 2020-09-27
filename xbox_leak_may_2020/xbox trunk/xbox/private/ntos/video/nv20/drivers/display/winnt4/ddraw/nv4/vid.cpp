//******************************************************************************
//
// Copyright (c) 1995-1996 Microsoft Corporation
//
// Module Name:
//
//     DVIDEO.C
//
// Abstract:
//
//     Implements all the low level NV4 specific DirectVideo components for the driver.
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//
//******************************************************************************

#include "nvprecomp.h"
#include "nvalpha.h"
#include "ddminint.h"
#include "nv32.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "nvsubch.h"

#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free


#define MAGIC_DELAY         1000000

#define WAIT_OVERLAY_NOTIFIER(buf)                                                                  \
{   NvNotification *pFlipOverlayNotifier =    (NvNotification *)ppdev->Notifiers->FlipOverlay;      \
    ULONG i = 0;                                                                                    \
    while (pFlipOverlayNotifier[NV07A_NOTIFIERS_SET_OVERLAY(buf)].status == NV_IN_PROGRESS)         \
        if(i++ > MAGIC_DELAY) break;                                                                \
}                                                                                                   \
    

/******************************Public*Routine**********************************\
* DWORD Nv4UpdateOverlay
*
\******************************************************************************/

DWORD __stdcall Nv4UpdateOverlay( PDD_UPDATEOVERLAYDATA lpUOData )
    {
    long srcDeltaX;
    long srcDeltaY;                                                        
    long dwSrcOffset;
    long dwSrcWidth;
    long dwSrcHeight;
    long dwDstWidth;
    long dwDstHeight;
    long dwSrcPitch;
    ULONG   dwVideoColorKey;
    DWORD index = 0;
    DWORD dwBackBufferCount = 0;
    DWORD match = 0;
    BOOL    releaseOpenglSemaphore = FALSE;
    DWORD dwOverlayFormat;

    PDEV *ppdev = (PDEV *) lpUOData->lpDD->dhpdev;
    FAST Nv3ChannelPio  *npDev = (Nv3ChannelPio *)ppdev->ddChannelPtr;
    FAST USHORT     freeCount = ppdev->NVFreeCount;
    NvNotification *pFlipOverlayNotifier =
    (NvNotification *)ppdev->Notifiers->FlipOverlay;

    PDD_SURFACE_LOCAL  srcx = lpUOData->lpDDSrcSurface;
    PDD_SURFACE_GLOBAL  src = srcx->lpGbl;
    PDD_SURFACE_LOCAL  pOriginalSrcx = lpUOData->lpDDSrcSurface;
    PDD_SURFACE_GLOBAL  pOriginalSrc  = lpUOData->lpDDSrcSurface->lpGbl;

    if ((src->ddpfSurface.dwFourCC != FOURCC_UYVY) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YUY2) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YV12) &&
        (src->ddpfSurface.dwFourCC != FOURCC_420i) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IF09) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YVU9) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IV32) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IV31))
        {
        lpUOData->ddRVal = DDERR_INVALIDSURFACETYPE;
        return DDHAL_DRIVER_HANDLED;
        }

    if (!(srcx->ddsCaps.dwCaps & DDSCAPS_OVERLAY))
        {
        lpUOData->ddRVal = DDERR_NOTAOVERLAYSURFACE;
        return DDHAL_DRIVER_HANDLED;
        }

    while ((index < NV_MAX_OVERLAY_SURFACES) && (!match))
        if ((DWORD)srcx == ppdev->dwOverlaySurfaceLCL[index++])
            match = 1;

    if (!match)
        {
        lpUOData->ddRVal = DDERR_OUTOFCAPS;
        return DDHAL_DRIVER_HANDLED;
        }

    if (npDev == NULL)
        {  /* Mode switches might occur without warning */
        lpUOData->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
        }

    /*
     * NOTES:
     *
     * This callback is invoked to update an overlay surface.
     * This is where the src/destination is specified, any effects, and
     * it is shown or hidden
     */

    /* Alternate buffer indexes to keep the hardware happy */
	ppdev->OverlayBufferIndex ^= 1;
    index = ppdev->OverlayBufferIndex;

    ENTER_DMA_PUSH_SYNC();
    NV_DD_DMA_PUSHER_SYNC();
    freeCount = ppdev->NVFreeCount;

    // if Overlay is enabled, disabled it before updating new size and position.
    if ((lpUOData->dwFlags & DDOVER_HIDE ) || ppdev->dwOverlayEnabled)
        {
        DPF("UPDATE OVERLAY - DDOVER_HIDE");

        WAIT_OVERLAY_NOTIFIER(0);
        WAIT_OVERLAY_NOTIFIER(1);

        while (freeCount < 3*4)
            freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
        freeCount -= 3*4;

        npDev->dDrawSpare.SetObject = NV_DD_VIDEO_OVERLAY;
        npDev->dDrawSpare.Nv04VideoOverlay.StopOverlay[0] = NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE;
        npDev->dDrawSpare.Nv04VideoOverlay.StopOverlay[1] = NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE;

        ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_OVERLAY;

        /* Let DDraw know how many visible overlays are active */
        ppdev->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;
        ppdev->dwOverlaySrcHeight = 0;
        ppdev->dwOverlayDstHeight = 0;

        WAIT_OVERLAY_NOTIFIER(0);
        WAIT_OVERLAY_NOTIFIER(1);

        ppdev->dwOverlayEnabled = FALSE;
        }

    if (lpUOData->dwFlags & DDOVER_HIDE )
        {
            // already disabled
        }
    else
        {

        ppdev->dwOverlayEnabled = TRUE;
        DPF("UPDATE OVERLAY - DDOVER_SHOW OR MOVE/RESIZE ");
        DPF( "Source Rect = (%ld,%ld), (%ld,%ld)",
             lpUOData->rSrc.left, lpUOData->rSrc.top,
             lpUOData->rSrc.right, lpUOData->rSrc.bottom );

        DPF( "Dest Rect = (%ld,%ld), (%ld,%ld)",
             lpUOData->rDest.left, lpUOData->rDest.top,
             lpUOData->rDest.right, lpUOData->rDest.bottom );

        if ( lpUOData->dwFlags & (DDOVER_KEYSRCOVERRIDE | DDOVER_KEYSRC) )
            {
            ppdev->NVFreeCount = freeCount;
            lpUOData->ddRVal = DDERR_NOCOLORKEYHW;
            EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);
            }
        if ((lpUOData->rDest.left < 0) || (lpUOData->rDest.top < 0))
            {
            lpUOData->ddRVal = DDERR_INVALIDPOSITION;
            ppdev->NVFreeCount = freeCount;
            EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);
            }

        dwSrcWidth = lpUOData->rSrc.right - lpUOData->rSrc.left;
        dwSrcHeight = lpUOData->rSrc.bottom - lpUOData->rSrc.top;
        dwDstWidth = lpUOData->rDest.right - lpUOData->rDest.left;
        dwDstHeight = lpUOData->rDest.bottom - lpUOData->rDest.top;

        /* Remember heights so Flip32 knows what to do */
        ppdev->dwOverlaySrcHeight = dwSrcHeight;
        ppdev->dwOverlayDstHeight = dwDstHeight;

        if (dwSrcHeight > dwDstHeight)
            {
            dwSrcHeight >>= 1; /* We only support vertical shrinking and it can't be by more than .5 */
            if (dwSrcHeight > dwDstHeight)
                {
                lpUOData->ddRVal = DDERR_UNSUPPORTED;
                ppdev->NVFreeCount = freeCount;
                EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);
                }
            }

        /* Align to 2 pixel boundary + 1 for video scaler */
        dwDstWidth = (dwDstWidth + 2) & ~1L;
        dwDstHeight = (dwDstHeight + 2) & ~1L;

        srcDeltaX = (dwSrcWidth - 1) << 16;
        srcDeltaX /= dwDstWidth - 1;
        srcDeltaX <<= 4;
        srcDeltaY = (dwSrcHeight - 1) << 16;
        srcDeltaY /= dwDstHeight - 1;
        srcDeltaY <<= 4;

            // color key?
        if (lpUOData->dwFlags & DDOVER_KEYDESTOVERRIDE) {
            dwVideoColorKey = lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue;
            dwOverlayFormat = (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_IMAGE << 20);
        } else if (lpUOData->dwFlags & DDOVER_KEYDEST) {
            dwVideoColorKey = lpUOData->lpDDDestSurface->ddckCKDestOverlay.dwColorSpaceLowValue;
            dwOverlayFormat = (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_IMAGE << 20);
        } else {    // disable color key
            dwOverlayFormat = (NV07A_SET_OVERLAY_FORMAT_COLOR_KEY_MISMATCH_SHOW_OVERLAY << 20);
        }
        if (src->ddpfSurface.dwFourCC == FOURCC_UYVY) {
            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_COLOR_LE_YB8CR8YA8CB8 << 16);
        } else {
            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_COLOR_LE_CR8YB8CB8YA8 << 16);
        }

        while (srcx != NULL)
            {
            dwSrcOffset = src->fpVidMem;

            if ((src->ddpfSurface.dwFourCC == FOURCC_IF09) ||
                (src->ddpfSurface.dwFourCC == FOURCC_YVU9) ||
                (src->ddpfSurface.dwFourCC == FOURCC_IV32) ||
                (src->ddpfSurface.dwFourCC == FOURCC_IV31))
                {
                /* Skip over Indeo portion of surface */
                dwSrcPitch = (DWORD)src->wWidth;
                dwSrcOffset += ((dwSrcPitch * ((DWORD)src->wHeight * 10L)) >> 3);
                /* Force block to be properly aligned */
                dwSrcOffset = (dwSrcOffset + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
                dwSrcPitch = ((src->wWidth + 1) & 0xFFFFFFFE) << 1;
                dwSrcPitch = (dwSrcPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
                }
            else if ((src->ddpfSurface.dwFourCC == FOURCC_YV12) ||
                     (src->ddpfSurface.dwFourCC == FOURCC_420i))
                {
                /* Assumes a separate overlay YUY2 surface in video memory */
                if (src->fpVidMem == ppdev->NvFloatingMem2MemFormatBaseAddress)
                    dwSrcOffset = ppdev->NvYUY2Surface0Mem;

                else if (src->fpVidMem == ppdev->NvFloatingMem2MemFormatNextAddress)
                    dwSrcOffset = ppdev->NvYUY2Surface1Mem;
                else
                    dwSrcOffset = ppdev->NvYUY2Surface2Mem;

                dwSrcPitch = ((src->wWidth + 1) & 0xFFFFFFFE) << 1;
                dwSrcPitch = (dwSrcPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
                }
            else
                dwSrcPitch = (DWORD)src->lPitch;

            dwSrcOffset += (lpUOData->rSrc.top * dwSrcPitch);
            dwSrcOffset += (lpUOData->rSrc.left << 1);

            /* Let the world know what the current overlay src offset is */
            ppdev->OverlaySrcX = (short)lpUOData->rSrc.left;
            ppdev->OverlaySrcY = (short)lpUOData->rSrc.top;

            /* We shrink vertically by skipping scanlines then stretching up */
            if (ppdev->dwOverlaySrcHeight > (DWORD)dwDstHeight)
                dwSrcPitch <<= 1;

// temp defines b4 becoming a class standard
#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_0 (0x04000000)
#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1 (0x02000000)

            /* Let the world know what the overlay src info is */
            ppdev->OverlaySrcOffset = dwSrcOffset;
            ppdev->OverlaySrcPitch = dwSrcPitch;
            ppdev->OverlaySrcSize = asmMergeCoords(dwSrcWidth, dwSrcHeight);

            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_MATRIX_ITURBT601 << 24);
            dwOverlayFormat |= (NV07A_SET_OVERLAY_FORMAT_NOTIFY_WRITE_ONLY << 31);

            ppdev->dwVideoColorKey = dwVideoColorKey;
            ppdev->OverlaySrcOffset = dwSrcOffset;
            ppdev->OverlaySrcSize = asmMergeCoords((dwSrcWidth & ~1), (dwSrcHeight & ~1));
            ppdev->dwOverlayDeltaX = srcDeltaX;
            ppdev->dwOverlayDeltaY = srcDeltaY;
            ppdev->dwOverlayDstX = lpUOData->rDest.left;
            ppdev->dwOverlayDstY = lpUOData->rDest.top;
            ppdev->dwOverlayDstWidth = dwDstWidth;
            ppdev->dwOverlayDstHeight = dwDstHeight;
            ppdev->dwOverlayFormat = dwOverlayFormat;
            ppdev->OverlaySrcPitch = dwSrcPitch;

            dwOverlayFormat |= dwSrcPitch;

            WAIT_OVERLAY_NOTIFIER(index);

            while (freeCount < 10*4)
                freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
            freeCount -= 10*4;

            // notifiers do not seem to be working...
            pFlipOverlayNotifier[NV07A_NOTIFIERS_SET_OVERLAY(index)].status = NV_IN_PROGRESS;

            npDev->dDrawSpare.SetObject = NV_DD_VIDEO_OVERLAY;
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].colorKey = dwVideoColorKey;
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].offset = dwSrcOffset;
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].sizeIn = asmMergeCoords((dwSrcWidth & ~1), (dwSrcHeight & ~1));
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].pointIn = 0;
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].dsDx = srcDeltaX;
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].dtDy = srcDeltaY;
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].pointOut = asmMergeCoords(lpUOData->rDest.left, lpUOData->rDest.top);
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].sizeOut = asmMergeCoords(dwDstWidth, dwDstHeight);
            npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].format = dwOverlayFormat;

            ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_OVERLAY;
            

            srcx = NULL;
            src  = NULL;
            }

        ppdev->dDrawSpareSubchannelObject = 0;

        /* Let DDraw know how many visible overlays are active */
        if (ppdev->HALInfo.ddCaps.dwCurrVisibleOverlays == 0)
            {
            ppdev->HALInfo.ddCaps.dwCurrVisibleOverlays = 1;
            }
        }
    ppdev->NVFreeCount = (short)freeCount;
    lpUOData->ddRVal = DD_OK;
    EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);

    } /* Nv4UpdateOverlay */


/******************************Public*Routine**********************************\
* DWORD DdSetOverlayPosition
*
\******************************************************************************/
DWORD __stdcall Nv4SetOverlayPosition( PDD_SETOVERLAYPOSITIONDATA lpSOPData )
    {
    DWORD           index = 0;
    DWORD           match = 0;
    PDEV *ppdev = (PDEV *) lpSOPData->lpDD->dhpdev;
    FAST Nv3ChannelPio  *npDev = (Nv3ChannelPio *)ppdev->ddChannelPtr;
    FAST USHORT     freeCount = ppdev->NVFreeCount;
    BOOL    releaseOpenglSemaphore = FALSE;

    PDD_SURFACE_LOCAL  srcx = lpSOPData->lpDDSrcSurface;

    if (!(srcx->ddsCaps.dwCaps & DDSCAPS_OVERLAY))
        {
        lpSOPData->ddRVal = DDERR_NOTAOVERLAYSURFACE;
        return DDHAL_DRIVER_HANDLED;
        }

    if ((lpSOPData->lXPos < 0) || (lpSOPData->lYPos < 0))
        {
        lpSOPData->ddRVal = DDERR_INVALIDPOSITION;
        return DDHAL_DRIVER_HANDLED;
        }

    while ((index < NV_MAX_OVERLAY_SURFACES) && (!match))
        if ((DWORD)srcx == ppdev->dwOverlaySurfaceLCL[index++])
            match = 1;

    if (!match)
        {
        lpSOPData->ddRVal = DDERR_OUTOFCAPS;
        return DDHAL_DRIVER_HANDLED;
        }

    if (npDev == NULL)
        {  /* Mode switches might occur without warning */
        lpSOPData->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
        }
        
    /* Alternate buffer indexes to keep the hardware happy */
	ppdev->OverlayBufferIndex ^= 1;
    index = ppdev->OverlayBufferIndex;

    ENTER_DMA_PUSH_SYNC();
    NV_DD_DMA_PUSHER_SYNC();
    freeCount = ppdev->NVFreeCount;

    /*
     * NOTES:
     *
     * This callback is invoked to set an overlay position
     */
    DPF( "SET OVERLAY POSITION" );
    DPF( "Overlay surface = %08lx", lpSOPData->lpDDSrcSurface );
    DPF( "(%ld,%ld)", lpSOPData->lXPos, lpSOPData->lYPos );

    while (freeCount < 2*4)
        freeCount = NvGetFreeCount(npDev, BLIT_SUBCHANNEL);
    freeCount -= 2*4;

    npDev->dDrawSpare.SetObject = NV_DD_VIDEO_OVERLAY;
    npDev->dDrawSpare.Nv04VideoOverlay.SetOverlayPointOutA = asmMergeCoords(lpSOPData->lXPos, lpSOPData->lYPos);
    ppdev->dwOverlayDstX = lpSOPData->lXPos;
    ppdev->dwOverlayDstY = lpSOPData->lYPos;

    ppdev->dDrawSpareSubchannelObject = 0;

    ppdev->NVFreeCount = (short)freeCount;

    lpSOPData->ddRVal = DD_OK;
    EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);

    } /* DdSetOverlayPosition */

/*
 * Nv4UnlockOverlay
 */
DWORD __stdcall Nv4UnlockOverlay( PDD_UNLOCKDATA lpUnlockData )
    {
    PDEV*   ppdev;
    FAST Nv3ChannelPio          *npDev;
    FAST USHORT                 freeCount;
    PDD_SURFACE_GLOBAL        pSurf_gbl;
    DWORD                       caps;
    DWORD                       dwSrcYOffset;
    DWORD                       dwSrcVOffset;
    DWORD                       dwSrcUOffset;
    long                        dwSrcYPitch;
    long                        dwSrcUVPitch;
    long                        dwSrcUVHeight;
    DWORD                       dwDstYOffset;
    long                        dwDstYPitch;
    unsigned char               *fpSrcYMem;
    unsigned char               *fpSrcUMem;
    unsigned char               *fpSrcVMem;
    long                        dwSrcPitch;
    long                        dwSrcWidth;
    long                        dwSrcHeight;
    BOOL    releaseOpenglSemaphore = FALSE;

    pSurf_gbl = lpUnlockData->lpDDSurface->lpGbl;
    ppdev = (PDEV *) lpUnlockData->lpDD->dhpdev;
    npDev = (Nv3ChannelPio *)ppdev->ddChannelPtr;

    caps = lpUnlockData->lpDDSurface->ddsCaps.dwCaps;


    //*************************************************************
    // Ensure GDI DMA Push channel processing has completed prior
    // to copying a large block of bitmap data into the fifo.
    //*************************************************************
    ENTER_DMA_PUSH_SYNC();
    NV_DD_DMA_PUSHER_SYNC();

    ppdev->pfnWaitEngineBusy(ppdev);

    /* If surface is IF09 format then must reformat data before
       video scaler can be used to display it */
    if ((pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IF09) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YVU9) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IV32) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IV31))
        {

        dwSrcYOffset = pSurf_gbl->fpVidMem;
        dwSrcYPitch = (DWORD)pSurf_gbl->wWidth;
        dwSrcYPitch = (dwSrcYPitch + 3) & ~3L;
        dwDstYOffset = dwSrcYOffset + ((dwSrcYPitch * ((DWORD)pSurf_gbl->wHeight * 10L)) >> 3);
        dwDstYOffset = (dwDstYOffset + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
        dwDstYPitch = ((pSurf_gbl->wWidth + 1) & 0xFFFE) << 1;
        dwDstYPitch = (dwDstYPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;

        fpSrcYMem = (unsigned char *)(ULONG)ppdev->pjFrameBufbase + pSurf_gbl->fpVidMem;
        dwSrcHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcHeight = (dwSrcHeight + 3) & ~3L;
        fpSrcVMem = fpSrcYMem + (dwSrcYPitch * dwSrcHeight);
        dwSrcHeight >>= 2;
        fpSrcUMem = fpSrcVMem + ((dwSrcYPitch >> 2) * dwSrcHeight);

        dwSrcUVHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcUVHeight = (dwSrcUVHeight + 3) & ~3L;
        dwSrcUVHeight >>= 2;

        dwSrcUVPitch = (DWORD)pSurf_gbl->wWidth;
        dwSrcUVPitch = (dwSrcUVPitch + 3) & ~3L;
        dwSrcUVPitch >>= 2;


        freeCount = ppdev->NVFreeCount;

        while (freeCount < 3*4)
            freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
        freeCount -= 3*4;

        npDev->dDrawRop.SetRop5 = 0xC0; /* MERGECOPY PSa */

        /* Temporarily change destination surface color format */
        npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetColorFormat =
            NV042_SET_COLOR_FORMAT_LE_Y8;

        DDRAW_SET_PRIMARY(ppdev, dwDstYOffset, dwDstYPitch);
        DDRAW_SET_SOURCE(ppdev, dwSrcYOffset, dwDstYPitch);
        while (freeCount < 16*4)
            freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
        freeCount -= 16*4;

//        npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetDestin = dwDstYOffset;
//        npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetPitch =
//            (dwDstYPitch << 16) | dwDstYPitch;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternSelect =
        NV044_SET_PATTERN_SELECT_MONOCHROME;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeShape =
        NV044_SET_MONOCHROME_SHAPE_64X_1Y;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeColor1 = 0xFFFFFFFF;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeColor0 = 0;

        /* First enable only V values to be written */
        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromePattern0 =
        0x11111111;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromePattern1 =
        0x11111111;

        npDev->dDrawStretch.SetObject =
            NV_DD_STRETCHED_UV_IMAGE_FROM_CPU;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.SizeIn =
            asmMergeCoords(dwSrcUVPitch, dwSrcUVHeight);

        npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipPoint = 0;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipSize =
            asmMergeCoords((dwSrcYPitch << 1), (DWORD)pSurf_gbl->wHeight);

        npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDxDu =
            8 << 20;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDyDv =
            4 << 20;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.Point12d4 = 0;

        dwSrcHeight = dwSrcUVHeight;

        while (--dwSrcHeight >= 0)
            {
            FAST DWORD vPxls0;
            FAST DWORD vPxls1;

            dwSrcPitch = dwSrcUVPitch;
            
            dwSrcPitch = (dwSrcPitch + 3) & ~3;

            while (dwSrcPitch >= 16)
                {
                dwSrcPitch -= 16;

                while (freeCount < 16)
                    freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                freeCount -= 16;

                vPxls0 = *(unsigned long *)fpSrcVMem;
                vPxls1 = *(unsigned long *)&fpSrcVMem[4];
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[0] =
                vPxls0;
                vPxls0 = *(unsigned long *)&fpSrcVMem[8];
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[1] =
                vPxls1;
                vPxls1 = *(unsigned long *)&fpSrcVMem[12];
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[2] =
                vPxls0;
                fpSrcVMem += 16;
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[3] =
                vPxls1;
                }

            while (freeCount < 16)
                freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);

            while (dwSrcPitch >= 4)
                {
                dwSrcPitch -= 4;
                freeCount -= 4;
                vPxls0 = *(unsigned long *)fpSrcVMem;
                fpSrcVMem += 4;
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[0] =
                vPxls0;
                }
            }

        while (freeCount < 56)
            freeCount = NvGetFreeCount(npDev, NV_DD_PATTERN);
        freeCount -= 56;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetPatternSelect =
        NV044_SET_PATTERN_SELECT_MONOCHROME;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeShape =
        NV044_SET_MONOCHROME_SHAPE_64X_1Y;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeColor1 = 0xFFFFFFFF;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeColor0 = 0;

        /* Now enable only U values to be written */
        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromePattern0 =
        0x44444444;

        npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromePattern1 =
        0x44444444;

        npDev->dDrawStretch.SetObject = NV_DD_STRETCHED_UV_IMAGE_FROM_CPU;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.SizeIn =
        asmMergeCoords(dwSrcUVPitch, dwSrcUVHeight);

        npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipPoint = 0;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipSize =
        asmMergeCoords((dwSrcYPitch << 1), (DWORD)pSurf_gbl->wHeight);

        npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDxDu = 8 << 20;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDyDv = 4 << 20;

        npDev->dDrawStretch.nv4StretchedImageFromCpu.Point12d4 = 0;
        dwSrcHeight = dwSrcUVHeight;

        while (--dwSrcHeight >= 0)
            {
            FAST DWORD uPxls0;
            FAST DWORD uPxls1;

            dwSrcPitch = dwSrcUVPitch;

            dwSrcPitch = (dwSrcPitch + 3) & ~3;
            
            while (dwSrcPitch >= 16)
                {
                dwSrcPitch -= 16;

                while (freeCount < 16)
                    freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                freeCount -= 16;

                uPxls0 = *(unsigned long *)fpSrcUMem;
                uPxls1 = *(unsigned long *)&fpSrcUMem[4];
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[0] =
                uPxls0;
                uPxls0 = *(unsigned long *)&fpSrcUMem[8];
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[1] =
                uPxls1;
                uPxls1 = *(unsigned long *)&fpSrcUMem[12];
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[2] =
                uPxls0;
                fpSrcUMem += 16;
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[3] =
                uPxls1;
                }

            while (freeCount < 16)
                freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);

            while (dwSrcPitch >= 4)
                {
                dwSrcPitch -= 4;
                freeCount -= 4;
                uPxls0 = *(unsigned long *)fpSrcUMem;
                fpSrcUMem += 4;
                npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[0] =
                uPxls0;
                }
            }

        while (freeCount < 54)
            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
        freeCount -= 54;


        /* reenable pattern object alpha */
       npDev->subchannel[NV_DD_PATTERN].nv4ImagePattern.SetMonochromeColor0 =
            0 | ppdev->AlphaEnableValue;

        /* restore destination surface color format */
        if (ppdev->cBitsPerPel == 8)
            npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetColorFormat =
                NV042_SET_COLOR_FORMAT_LE_Y8;
        else if (ppdev->cBitsPerPel == 16)
            npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetColorFormat =
                NV042_SET_COLOR_FORMAT_LE_R5G6B5;
        else
            npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetColorFormat =
                NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_O8R8G8B8;

        /* Update all Y values */
        npDev->subchannel[NV_DD_SPARE].SetObject =
        NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
            dwSrcYOffset;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
            dwDstYOffset;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
            dwSrcYPitch;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
            dwDstYPitch;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
            dwSrcYPitch;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
            (DWORD)pSurf_gbl->wHeight;
            //NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_2;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
            1 | (2 << 8);

        //((NvNotification *)(&(ppdev->Notifiers->DmaToMem)))[1].status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;
        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify = NV039_NOTIFY_WRITE_ONLY;

        //while (((NvNotification *)(&ppdev->Notifiers->DmaToMem))[1].status == NV039_NOTIFICATION_STATUS_IN_PROGRESS);

        ppdev->dDrawSpareSubchannelObject = 0;
        ppdev->NVFreeCount = (short)freeCount;
        }
    else if ((pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YV12)||
             (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_420i))
        {
        /* YV12/420i surfaces also need reformated before the video scaler can display them */
        DWORD srcMemType = 0;

        freeCount = ppdev->NVFreeCount;

        /* First determine which source buffer is being used */
        dwSrcYOffset = pSurf_gbl->fpVidMem; // - ppdev->NvFloatingMem2MemFormatBaseAddress;

        dwSrcYPitch = (DWORD)pSurf_gbl->wWidth;
        dwSrcYPitch = (dwSrcYPitch + 3) & ~3L;

        dwSrcWidth = dwSrcYPitch;

        if (dwSrcYOffset == ppdev->NvFloatingMem2MemFormatBaseAddress)
            dwDstYOffset = ppdev->NvYUY2Surface0Mem;
        else if (pSurf_gbl->fpVidMem == ppdev->NvFloatingMem2MemFormatNextAddress)
            dwDstYOffset = ppdev->NvYUY2Surface1Mem;
        else
            dwDstYOffset = ppdev->NvYUY2Surface2Mem;
#ifdef SYSMEM_FALLBACK
        /* Now correct source offset if it is really in video memory */
        if ((ppdev->NvFloatingMem2MemFormatBaseAddress >= pDriverData->BaseAddress) && /* If video memory */
            (pDriverData->NvFloatingMem2MemFormatBaseAddress < pDriverData->BaseAddress + 0x1000000))
            {
            dwSrcYOffset = pSurf_gbl->fpVidMem - pDriverData->BaseAddress;
            srcMemType = DDSCAPS_LOCALVIDMEM;
            }
#endif
        srcMemType = DDSCAPS_LOCALVIDMEM;
        dwDstYPitch = ((pSurf_gbl->wWidth + 1) & 0xFFFE) << 1;
        dwDstYPitch = (dwDstYPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;

        dwSrcHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcHeight = (dwSrcHeight + 1) & ~1L;

        if (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YV12)
            {
            dwSrcVOffset = dwSrcYOffset + (dwSrcYPitch * dwSrcHeight);
            dwSrcUOffset = dwSrcVOffset + ((dwSrcYPitch * dwSrcHeight) >> 2);

            dwSrcUVPitch = (DWORD)pSurf_gbl->wWidth;
            dwSrcUVPitch = (dwSrcUVPitch + 3) & ~3L;
            dwSrcUVPitch >>= 1;

            }
        else
            { /* FOURCC_420i */
            dwSrcVOffset = dwSrcYOffset + dwSrcYPitch;
            dwSrcYPitch += (dwSrcYPitch >> 1);
            dwSrcUOffset = dwSrcVOffset + dwSrcYPitch;

            dwSrcUVPitch = dwSrcYPitch << 1;
            }

        dwSrcUVHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcUVHeight = (dwSrcUVHeight + 1) & ~1L;
        dwSrcUVHeight >>= 1;

        while (freeCount < 72)
            freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
        freeCount -= 72;

        if (srcMemType == 0) /* If system memory */
            npDev->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
        else
            npDev->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
        dwSrcVOffset;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
        (dwDstYOffset + 3);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
        dwSrcUVPitch;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
        (dwDstYPitch << 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
        (dwSrcWidth >> 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
        dwSrcUVHeight;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
        1 | (4 << 8);
        //NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_4;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify = 0;

        if (srcMemType == 0) /* If system memory */
            npDev->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
        else
            npDev->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
        dwSrcUOffset;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
        (dwDstYOffset + 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
        dwSrcUVPitch;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
        (dwDstYPitch << 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
        (dwSrcWidth >> 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
        dwSrcUVHeight;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
        1 | (4 << 8);
        //NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_4;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify = 0;

        while (freeCount < 72)
            freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
        freeCount -= 72;

        if (srcMemType == 0) /* If system memory */
            npDev->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
        else
            npDev->subchannel[NV_DD_SPARE].SetObject =
            NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
        dwSrcYOffset;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
        dwDstYOffset;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
        dwSrcYPitch;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
        dwDstYPitch;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
        dwSrcWidth;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
        (DWORD)pSurf_gbl->wHeight;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
        1 | (2 << 8);
        //NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_2;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify = 0;

        npDev->subchannel[NV_DD_SPARE].SetObject =
        NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
        (dwDstYOffset + 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
        (dwDstYOffset + 1 + dwDstYPitch);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
        (dwDstYPitch << 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
        (dwDstYPitch << 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
        (dwDstYPitch >> 1);

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
        (DWORD)pSurf_gbl->wHeight >> 1;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
        (2 << 8) | 2;
        //NV_MTMF_FORMAT_INPUT_INC_2 | NV_MTMF_FORMAT_OUTPUT_INC_2;

        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify = 0;

        ppdev->dDrawSpareSubchannelObject = 0;
        ppdev->NVFreeCount = (short)freeCount;
        }

    lpUnlockData->ddRVal = DD_OK;

    ppdev->pfnWaitEngineBusy(ppdev);
    EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);

    } /* Nv4UnlockOverlay */

extern "C" VOID __cdecl NvTurnOffVideoOverlay(PDEV *ppdev)
{
    FAST Nv3ChannelPio  *npDev = (Nv3ChannelPio *)ppdev->ddChannelPtr;
    FAST USHORT     freeCount = ppdev->NVFreeCount;

    WAIT_OVERLAY_NOTIFIER(0);
    WAIT_OVERLAY_NOTIFIER(1);

    while (freeCount < 3*4)
        freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
    freeCount -= 3*4;

    npDev->dDrawSpare.SetObject = NV_DD_VIDEO_OVERLAY;
    npDev->dDrawSpare.Nv04VideoOverlay.StopOverlay[0] = NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE;
    npDev->dDrawSpare.Nv04VideoOverlay.StopOverlay[1] = NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE;

    ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_OVERLAY;

    WAIT_OVERLAY_NOTIFIER(0);
    WAIT_OVERLAY_NOTIFIER(1);

    ppdev->NVFreeCount = (short)freeCount;
}
