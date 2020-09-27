// **************************************************************************
//
//       Copyright 1993-2001 NVIDIA, Corporation.  All rights reserved.
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
//
//  Module: dd.cpp
//      nVidia routines to support the DirectDraw interface on NT4
//
// **************************************************************************
//
// Environment: kernel mode only
//
//******************************************************************************
#include "nvprecomp.h"
#include "nvalpha.h"
#include "ddminint.h"
#include "nv32.h"
#include "nvcm.h"
#include "nvUtil.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "nvsubch.h"

#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free

extern ULONG NV4_ChannelIsGdi(PDEV *ppdev);
extern VOID NV4_DdPioSync(PDEV *ppdev);
    
extern GLOBALDATA           DriverData;     // in ddmini16.c

static OLD_FLIPRECORD       flipPrimaryRecord;
static OLD_FLIPRECORD       flipOverlayRecord;

/*
 * IN_VBLANK should be replaced by a test to see if the hardware is currently
 * in the vertical blank
 */
#define IN_VBLANK               FALSE
#define IN_DISPLAY              TRUE

/*
 * CURRENT_VLINE should be replaced by a macro to retrieve the current
 * scanline
 */
#define CURRENT_VLINE   (ppdev->pfnGetScanline(ppdev))

// Defines we'll use in the surface's 'dwReserved1' field:

#define DD_RESERVED_DIFFERENTPIXELFORMAT    0x0001


// Worst-case possible number of FIFO entries we'll have to wait for in
// DdBlt for any operation:

#define DDBLT_FIFO_COUNT    9

// NT is kind enough to pre-calculate the 2-d surface offset as a 'hint' so
// that we don't have to do the following, which would be 6 DIVs per blt:
//
//    y += (offset / pitch)
//    x += (offset % pitch) / bytes_per_pixel

#define convertToGlobalCord(x, y, surf) \
{                                       \
    y += surf->yHint;                   \
    x += surf->xHint;                   \
}


#define MAGIC_DELAY         1000000

#define WAIT_OVERLAY_NOTIFIER(buf)                                                                  \
{   NvNotification *pFlipOverlayNotifier =    (NvNotification *)ppdev->Notifiers->FlipOverlay;      \
    ULONG i = 0;                                                                                    \
    while (pFlipOverlayNotifier[NV07A_NOTIFIERS_SET_OVERLAY(buf)].status == NV_IN_PROGRESS)         \
        if(i++ > MAGIC_DELAY) break;                                                                \
}                                                                                                   \


/*
 * Nv4UpdateFlipStatus
 *
 * checks and sees if the most recent flip has occurred
 */
unsigned long Nv4UpdateFlipStatus( PDEV *ppdev, FLATPTR fpVidMem )
    {

	ULONG ulIndex;

    /*
     * see if a flip has happened recently
     */
    if (flipPrimaryRecord.bFlipFlag &&
        ((fpVidMem == 0) || (fpVidMem == flipPrimaryRecord.fpFlipFrom)) )
        {
        if(((NvNotification *) (&(ppdev->Notifiers->FlipPrimary[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS)
        {
            return DDERR_WASSTILLDRAWING;
        }
	    else
        {
    	    ppdev->CurrentVisibleSurfaceAddress = flipPrimaryRecord.fpFlipTo;
            flipPrimaryRecord.bFlipFlag = FALSE;
        	return DD_OK;
        }

        /*
         * if we aren't in the vertical blank, we can use the scanline
         * to help decide on what to do
         */
//        if(!(IN_VBLANK)) {
//            if( (CURRENT_VLINE >= flipPrimaryRecord.wFlipScanLine) ) {
//                QueryPerformanceCounter((LARGE_INTEGER*)&ttime);
//                if( (DWORD)(ttime-flipPrimaryRecord.liFlipTime) <= flipPrimaryRecord.dwFlipDuration ) {
//                    return DDERR_WASSTILLDRAWING;
//                }
//            }
//        }
        /*
         * in the vertical blank, scanline is useless
         */
//        else {
//            QueryPerformanceCounter((LARGE_INTEGER*)&ttime);
//            if( ((DWORD)(ttime-flipPrimaryRecord.liFlipTime) <= flipPrimaryRecord.dwFlipDuration) ) {
//                return DDERR_WASSTILLDRAWING;
//            }
//        }
//        flipPrimaryRecord.bFlipFlag = FALSE;
        }

    if (flipOverlayRecord.bFlipFlag &&
        ((fpVidMem == 0) ||
         (fpVidMem == flipOverlayRecord.fpFlipFrom) ||
         (fpVidMem == flipOverlayRecord.fpPreviousFlipFrom)) ) {
        NvNotification *pPioFlipOverlayNotifier =
            (NvNotification *)ppdev->Notifiers->FlipOverlay;

        if (ppdev->CurrentClass.VideoOverlay == NV04_VIDEO_OVERLAY) {

            if (fpVidMem == flipOverlayRecord.fpPreviousFlipFrom) {
                // only need for one to flush out
                if (pPioFlipOverlayNotifier[ppdev->OverlayBufferIndex + 1].status == NV_IN_PROGRESS)
                    return (DDERR_WASSTILLDRAWING);
            } else {
                // need to wait for both flips to flush out
                if ((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) ||
                    (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                    return (DDERR_WASSTILLDRAWING);
                } else {
                    flipOverlayRecord.bFlipFlag = FALSE;
                }
            }
        } else {
            if (fpVidMem == flipOverlayRecord.fpPreviousFlipFrom) {
/* The WHQL flip status test doesn't like this conservatism.  Comment out for now.
                LONGLONG timeNow;
                // only need for one to flush out, no way to really determine this on NV10,
                // but if we wait at least one CRTC refresh since it was issued, we can
                // guarantee it has been flushed through
                if ((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) &&
                    (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                    EngQueryPerformanceCounter((LONGLONG *) &timeNow);
                    if ((timeNow - flipOverlayRecord.llPreviousFlipTime) < flipPrimaryRecord.dwFlipDuration) {
                        return (DDERR_WASSTILLDRAWING);
                    }
                }
*/
            } else {
                if (fpVidMem == flipOverlayRecord.fpFlipFrom) {
                    // need to wait for both flips to flush out
                    if ((pPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) &&
                        (pPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS)) {
                        return (DDERR_WASSTILLDRAWING);
                    } else {
                        flipOverlayRecord.bFlipFlag = FALSE;
                    }
                }
            }
        }
        return (DD_OK);
    }

    return DD_OK;
}

//******************************Public*Routine**********************************
//
// Function: Nv4Blt()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
DWORD Nv4Blt(PDD_BLTDATA pbd)
    {
    HRESULT     ddrval;
    PDEV        *ppdev;
    Nv3ChannelPio   *npDev;
    USHORT  freeCount;
    ULONG   DdReturncode;
    BOOL    releaseOpenglSemaphore = FALSE;
    NvNotification *pSyncNotifier;

    ppdev = (PDEV *)pbd->lpDD->dhpdev;
    npDev = (Nv3ChannelPio *) ppdev->ddChannelPtr;

    if (npDev == NULL)
        {
        pbd->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
        }

    /*
     * is a flip in progress?
     */
    ddrval = Nv4UpdateFlipStatus(ppdev, pbd->lpDDDestSurface->lpGbl->fpVidMem);
    if ( ddrval != DD_OK )
        {
        if (pbd->dwFlags & DDBLT_WAIT)
            {
            while (ddrval != DD_OK)
                ddrval = Nv4UpdateFlipStatus(ppdev, pbd->lpDDDestSurface->lpGbl->fpVidMem);
            }
        else
            {
            pbd->ddRVal = ddrval;
            return DDHAL_DRIVER_HANDLED;
            }
        }

        ENTER_DMA_PUSH_SYNC();
        NV_DD_DMA_PUSHER_SYNC();

        if (ppdev->DdClipResetFlag)
            {
            freeCount = ppdev->NVFreeCount;
            while (freeCount < 2*4)
                freeCount = NvGetFreeCount(npDev, NV_DD_CLIP);
            freeCount -= 2*4;
            npDev->subchannel[NV_DD_CLIP].Nv01ContextClipRectangle.SetPoint = 0;
            npDev->subchannel[NV_DD_CLIP].Nv01ContextClipRectangle.SetSize =
            asmMergeCoords(ppdev->cxMemory, ppdev->cyMemory);
            ppdev->DdClipResetFlag = 0;       // Clipping rectangle has been reset
            ppdev->NVFreeCount = freeCount;
            }


    if (ppdev->cBitsPerPel == 8)
        {
        DdReturncode = Nv4Blt8(pbd);
        }
    else if (ppdev->cBitsPerPel == 16)
        {
        DdReturncode = Nv4Blt16(pbd);
        }
    else if (ppdev->cBitsPerPel == 32)
        {
        DdReturncode = Nv4Blt32(pbd);
        }
    else
        {
        pbd->ddRVal = DDERR_INVALIDPIXELFORMAT;
        DdReturncode = DDHAL_DRIVER_NOTHANDLED;
        }
    ppdev->NVFreeCount = 0;
    EXIT_DMA_PUSH_SYNC(DdReturncode);
    }

//******************************Public*Routine**********************************
//
// Function: Nv4Flip()
//
//  Note that lpSurfCurr may not necessarily be valid.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD __stdcall Nv4Flip(PDD_FLIPDATA pfd)
    {
    FAST short freeCount;
    HRESULT     ddrval;
    PDD_SURFACE_GLOBAL   pSurfTarg_gbl;
    BOOL    releaseOpenglSemaphore = FALSE;

    PDEV *ppdev = (PDEV *) pfd->lpDD->dhpdev;
    FAST Nv3ChannelPio *npDev = (Nv3ChannelPio *)ppdev->ddChannelPtr;

    NvNotification *pFlipOverlayNotifier =
    (NvNotification *)ppdev->Notifiers->FlipOverlay;
    NvNotification *pDmaToMemNotifier =
    (NvNotification *)ppdev->Notifiers->DmaToMem;

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

    DPF( "Flip32: curr=%08lx, targ=%08lx", pfd->lpSurfCurr, pfd->lpSurfTarg );
    DPF( "        vidmem ptrs change: %08lx->%08lx",
         pfd->lpSurfCurr->lpGbl->fpVidMem,
         pfd->lpSurfTarg->lpGbl->fpVidMem );


    ddrval = Nv4UpdateFlipStatus(ppdev, 0);

    if (ddrval != DD_OK)
        {
        if (pfd->dwFlags & DDFLIP_WAIT)
            {
            while (ddrval != DD_OK)
                {
                ddrval = Nv4UpdateFlipStatus(ppdev, 0);
                }
            }
        else
            {
            pfd->ddRVal = ddrval;
            return DDHAL_DRIVER_HANDLED;
            }
        }

    if (npDev == NULL)
        {
        pfd->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
        }

    if ((pfd->dwFlags & DDFLIP_WAIT) &&
        ppdev->pfnGraphicsEngineBusy(ppdev))
        {
        ppdev->NVFreeCount = 0;
        pfd->ddRVal = DDERR_WASSTILLDRAWING;
        EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);
        }

    ENTER_DMA_PUSH_SYNC();
    NV_DD_DMA_PUSHER_SYNC();
    NV4_DdPioSync(ppdev);

    freeCount = NvGetFreeCount(npDev, 7);

    /*
     * everything is OK, do the flip here
     */

    pSurfTarg_gbl = pfd->lpSurfTarg->lpGbl;

    /* Is this an overlay surface ? */
    if (!(pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_OVERLAY))
        {
        /* No - assume it is the primary surface */

        ULONG i, ulHead;

        flipPrimaryRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
        flipPrimaryRecord.bHaveEverCrossedVBlank = FALSE;
        flipPrimaryRecord.fpFlipTo = pSurfTarg_gbl->fpVidMem;

        flipPrimaryRecord.bFlipFlag = TRUE;

        ((NvNotification *) (&(ppdev->Notifiers->FlipPrimary[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;
    	for(i = 0; i < ppdev->ulNumberDacsActive; i++)
	    {
            NV_CREATE_OBJECT_SETUP();

            ulHead = ppdev->ulDeviceDisplay[i];

            while (freeCount < 4 * 6)
                freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
            freeCount -= 4 * 6;


            npDev->subchannel[NV_DD_SPARE].SetObject = NV_DD_VIDEO_LUT_CURSOR_DAC+ulHead;
            ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_LUT_CURSOR_DAC+ulHead;
            
            npDev->dDrawSpare.SetObject = NV_DD_VIDEO_LUT_CURSOR_DAC+ulHead;
            npDev->dDrawSpare.Nv04VideoLutCursorDac.SetImage[0].offset = 
                pSurfTarg_gbl->fpVidMem + i * ppdev->ulHeadDisplayOffset[ulHead];
            npDev->dDrawSpare.Nv04VideoLutCursorDac.SetImage[0].format = ppdev->lDelta | 
                NV046_SET_IMAGE_FORMAT_NOTIFY_WRITE_THEN_AWAKEN << 31;

            ppdev->ulFlipBase = pSurfTarg_gbl->fpVidMem;

        }
        // Syn. with both heads
        if(ppdev->ulNumberDacsActive > 1)
            while(((NvNotification *) (&(ppdev->Notifiers->FlipPrimary[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);

        if (IN_VBLANK)
            {
            flipPrimaryRecord.dwFlipScanLine = 0;
            flipPrimaryRecord.bWasEverInDisplay = FALSE;
            }
        else
            {
            flipPrimaryRecord.dwFlipScanLine = CURRENT_VLINE;
            flipPrimaryRecord.bWasEverInDisplay = TRUE;
            }
        }
    else if (pfd->lpSurfTarg->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
        { /* overlay */
        DWORD dwOffset;
        DWORD dwPitch;
        DWORD index;

        flipOverlayRecord.liPreviousFlipTime = flipOverlayRecord.liFlipTime;
        EngQueryPerformanceCounter((LONGLONG *) &flipOverlayRecord.liFlipTime);
        flipOverlayRecord.fpPreviousFlipFrom = flipOverlayRecord.fpFlipFrom;
        flipOverlayRecord.fpFlipFrom = pfd->lpSurfCurr->lpGbl->fpVidMem;
        flipOverlayRecord.fpFlipTo = pSurfTarg_gbl->fpVidMem;
        flipOverlayRecord.bHaveEverCrossedVBlank = FALSE;

        flipOverlayRecord.bFlipFlag = TRUE;

        /* Alternate buffer indexes to keep the hardware happy */

   	    ppdev->OverlayBufferIndex ^= 1;
    	index = ppdev->OverlayBufferIndex;

        /* Make sure we flip to the correct starting offset in new overlay surface */
        dwOffset = pSurfTarg_gbl->fpVidMem;

        if ((pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_IF09) ||
            (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_YVU9) ||
            (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_IV32) ||
            (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_IV31))
            {
            /* Skip over Indeo portion of surface */
            dwPitch = (DWORD)pSurfTarg_gbl->wWidth;
            dwOffset += ((dwPitch * ((DWORD)pSurfTarg_gbl->wHeight * 10L)) >> 3);
            /* Force block to be properly aligned */
            dwOffset = (dwOffset + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
            dwPitch = ((pSurfTarg_gbl->wWidth + 1) & 0xFFFFFFFE) << 1;
            dwPitch = (dwPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
            }
        else if ((pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_YV12) ||
                 (pSurfTarg_gbl->ddpfSurface.dwFourCC == FOURCC_420i))
            {
            /* Assumes a separate overlay YUY2 surface in video memory */
            if (pSurfTarg_gbl->fpVidMem == ppdev->NvFloatingMem2MemFormatBaseAddress)
                dwOffset = ppdev->NvYUY2Surface0Mem;
            else if (pSurfTarg_gbl->fpVidMem == ppdev->NvFloatingMem2MemFormatNextAddress)
                dwOffset = ppdev->NvYUY2Surface1Mem;
            else
                dwOffset = ppdev->NvYUY2Surface2Mem;

            dwPitch = ((pSurfTarg_gbl->wWidth + 1) & 0xFFFFFFFE) << 1;
            dwPitch = (dwPitch + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
            }
        else
            dwPitch = (DWORD)pSurfTarg_gbl->lPitch;

        dwOffset += (ppdev->OverlaySrcY * dwPitch);
        dwOffset += (ppdev->OverlaySrcX << 1);



        while (freeCount < 10*4)
            freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
        freeCount -= 10*4;

        if (ppdev->dDrawSpareSubchannelObject != NV_DD_VIDEO_OVERLAY)
        {
            npDev->subchannel[NV_DD_SPARE].SetObject = NV_DD_VIDEO_OVERLAY;
            ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_OVERLAY;
        }

        npDev->dDrawSpare.SetObject = NV_DD_VIDEO_OVERLAY;

        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].colorKey = ppdev->dwVideoColorKey;
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].offset =  dwOffset;
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].sizeIn = ppdev->OverlaySrcSize;
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].pointIn = 0;
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].dsDx = ppdev->dwOverlayDeltaX;
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].dtDy = ppdev->dwOverlayDeltaY;
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].pointOut = asmMergeCoords(ppdev->dwOverlayDstX, ppdev->dwOverlayDstY);
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].sizeOut = asmMergeCoords(ppdev->dwOverlayDstWidth, ppdev->dwOverlayDstHeight);
        npDev->dDrawSpare.Nv04VideoOverlay.SetOverlay[index].format = ppdev->dwOverlayFormat | dwPitch;
        ppdev->OverlaySrcOffset = dwOffset;
        }
    else
        { /* video port flips handled by FlipVideoPort32 */
        ppdev->NVFreeCount = freeCount;
        pfd->ddRVal = DDERR_INVALIDSURFACETYPE;
        EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);
        }

    if (pfd->dwFlags & DDFLIP_WAIT)
        {
        ddrval = Nv4UpdateFlipStatus(ppdev, 0);

        while (ddrval != DD_OK)
            ddrval = Nv4UpdateFlipStatus(ppdev, 0);
        }

    ppdev->NVFreeCount = freeCount;

    pfd->ddRVal = DD_OK;
    EXIT_DMA_PUSH_SYNC(DDHAL_DRIVER_HANDLED);

    } /* Flip32 */

//******************************Public*Routine**********************************
//
// Function: Nv4Lock()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************
DWORD Nv4Lock(
            PDD_LOCKDATA lpLockData)
    {
    HRESULT                     ddrval;
    DWORD                       dwOffset;
    DWORD                       caps;
    PDD_SURFACE_GLOBAL          psurf_gbl;
    BOOL    releaseOpenglSemaphore = FALSE;

    PDEV    *ppdev = (PDEV *) lpLockData->lpDD->dhpdev;
    FAST Nv3ChannelPio              *npDev = (Nv3ChannelPio *)ppdev->ddChannelPtr;
    NvNotification *pPioFlipOverlayNotifier = (NvNotification *)ppdev->Notifiers->FlipOverlay;

    NvNotification              *pDmaToMemNotifier =
    (NvNotification *)ppdev->Notifiers->DmaToMem;
    NvNotification              *pDmaFromMemNotifier =
    (NvNotification *)ppdev->Notifiers->DmaFromMem;
    NvNotification              *pSyncNotifier =
      (NvNotification *)ppdev->Notifiers->Sync;


    psurf_gbl = lpLockData->lpDDSurface->lpGbl;

    if (lpLockData->lpDDSurface->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
        {
        //*****************************************************************
        // For surfaces in system memory, just return the user-mode address
        //*****************************************************************
        lpLockData->lpSurfData = (VOID*) psurf_gbl->fpVidMem;
        lpLockData->ddRVal = DD_OK;

        //*****************************************************************
        // When a driver returns DD_OK and DDHAL_DRIVER_HANDLED from DdLock,
        // DirectDraw expects it to have adjusted the resulting pointer
        // to point to the upper left corner of the specified rectangle, if
        // any:
        //*****************************************************************
        if (lpLockData->bHasRect)
            {
            lpLockData->lpSurfData = (VOID*) ((BYTE*) lpLockData->lpSurfData
                                              + lpLockData->rArea.top * psurf_gbl->lPitch
                                              + lpLockData->rArea.left
                                              * (psurf_gbl->ddpfSurface.dwYUVBitCount >> 3));
            }

        return (DDHAL_DRIVER_HANDLED);
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

    caps = lpLockData->lpDDSurface->ddsCaps.dwCaps;

    if ((npDev == NULL) && (caps & DDSCAPS_VIDEOMEMORY))
        {  /* Mode switches might occur without warning */
        lpLockData->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
        }

    psurf_gbl = lpLockData->lpDDSurface->lpGbl;

    /*
     * check to see if any pending physical flip has occurred
     */

    ddrval = Nv4UpdateFlipStatus(ppdev, psurf_gbl->fpVidMem);
    if ( ddrval != DD_OK )
        {
        if (lpLockData->dwFlags & DDLOCK_WAIT)
            {
            while (ddrval != DD_OK)
                {
                long countDown = 500000;  // sometimes we can loose a Flip notification
                ddrval = Nv4UpdateFlipStatus(ppdev, psurf_gbl->fpVidMem);
                if (--countDown <= 0)
                    {
                    ((NvNotification *) (&(ppdev->Notifiers->FlipPrimary[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status = 0;
                    pPioFlipOverlayNotifier[1].status = 0;
                    pPioFlipOverlayNotifier[2].status = 0;
                    }
                }
            }
        else
            {
            lpLockData->ddRVal = DDERR_WASSTILLDRAWING;
            return DDHAL_DRIVER_HANDLED;
            }
        }

    if (caps & (DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE))
        {
        if (((caps & DDSCAPS_TEXTURE) == 0) && ((lpLockData->dwFlags & DDLOCK_READONLY) == 0))
            {

            if (!OglIsEnabled(ppdev))
                {
                //***********************************************************************
                // Ensure graphics engine has completed all processing in Ddraw
                // channel. We know the GDI channel is idle at this point.
                //***********************************************************************
                NV_DD_DMA_PUSHER_SYNC();
                NV4_DdPioSync(ppdev);
                }
            else
                {
                //************************************************************************
                // If OpenGL ICD is running, the fifo sync routine will ensure the fifo is
                // empty and the graphics engine has completed its processing.
                //*************************************************************************
                START_DMA_PUSH_SYNC();
                NV_DD_DMA_PUSHER_SYNC();
                ppdev->NVFreeCount = 0;
                END_DMA_PUSH_SYNC();
                }
            }
        else
            {
            return(DDHAL_DRIVER_NOTHANDLED);
            }
        }
    return(DDHAL_DRIVER_NOTHANDLED);
    }

//******************************Public*Routine**********************************
//
// Function: Nv4Unlock()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD __stdcall Nv4Unlock( PDD_UNLOCKDATA lpUnlockData )
    {
    FAST Nv3ChannelPio          *npDev;
    PDD_SURFACE_GLOBAL   pSurf_gbl;
    DWORD                       caps;
    PDEV*   ppdev;

    caps = lpUnlockData->lpDDSurface->ddsCaps.dwCaps;
    ppdev = (PDEV *) lpUnlockData->lpDD->dhpdev;
    npDev = (Nv3ChannelPio *) ppdev->ddChannelPtr;

    if ((npDev == NULL) && (caps & DDSCAPS_VIDEOMEMORY))
        {  /* Mode switches might occur without warning */
        lpUnlockData->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
        }

    pSurf_gbl = lpUnlockData->lpDDSurface->lpGbl;

    if (caps & DDSCAPS_OVERLAY)
        Nv4UnlockOverlay(lpUnlockData);

    lpUnlockData->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;

    } /* Unlock32 */


//******************************Public*Routine**********************************
//
// Function: Nv4GetBltStatus()
//
//      Doesn't currently really care what surface is specified, just checks
//      and goes.
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD Nv4GetBltStatus(
                    PDD_GETBLTSTATUSDATA lpGetBltStatus)

    {
    PDEV*   ppdev;
    BYTE*   pjMmBase;
    HRESULT ddRVal;
    FNGRAPHICSENGINEBUSY*  pfnGraphicsEngineBusy;
    FNFIFOISBUSY*         pfnFifoIsBusy;
    Nv3ChannelPio *npDev;

    ppdev    = (PDEV*) lpGetBltStatus->lpDD->dhpdev;

    //**************************************************************************
    // Init chip specific function pointers
    //**************************************************************************

    pfnGraphicsEngineBusy = ppdev->pfnGraphicsEngineBusy;
    pfnFifoIsBusy         = ppdev->pfnFifoIsBusy;

    ddRVal = DD_OK;
    if (lpGetBltStatus->dwFlags == DDGBS_CANBLT)
        {
        //**********************************************************************
        // DDGBS_CANBLT case: can we add a blt?
        //**********************************************************************

        ddRVal = Nv4UpdateFlipStatus(ppdev,
                                        lpGetBltStatus->lpDDSurface->lpGbl->fpVidMem);

         }

    else

        {
        //**********************************************************************
        // DDGBS_ISBLTDONE case: is a blt in progress?
        //**********************************************************************
        npDev = (Nv3ChannelPio *) ppdev->ddChannelPtr;
        NV_DD_DMA_PUSHER_SYNC();
        NV4_DdPioSync(ppdev);
        ddRVal = DD_OK;
        }

    lpGetBltStatus->ddRVal = ddRVal;
    return (DDHAL_DRIVER_HANDLED);
    }

//******************************Public*Routine**********************************
//
// Function: Nv4DestroySurface
//
// Routine Description:
//
//      Note that if DirectDraw did the allocation, DDHAL_DRIVER_NOTHANDLED
//      should be returned.
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

DWORD __stdcall Nv4DestroySurface( PDD_DESTROYSURFACEDATA lpDestroySurface )
    {
    PDD_DIRECTDRAW_GLOBAL pdrv;
    PDD_SURFACE_LOCAL   psurf;
    PDD_SURFACE_GLOBAL  psurf_gbl;
    BOOL    releaseOpenglSemaphore = FALSE;

    DWORD                       index = 0;
    PDEV *ppdev = (PDEV *) lpDestroySurface->lpDD->dhpdev;
    Nv3ChannelPio  *npDev = (Nv3ChannelPio *)ppdev->ddChannelPtr;
    USHORT     freeCount = ppdev->NVFreeCount;
    NvNotification *pFlipOverlayNotifier = (NvNotification *)ppdev->Notifiers->FlipOverlay;

    pdrv = lpDestroySurface->lpDD;
    DPF( "in DestroySurface, lpDestroySurface->lpDD=%08lx, lpDestroySurface->lpDDSurface = %08lx",
         pdrv, lpDestroySurface->lpDDSurface );

    psurf = lpDestroySurface->lpDDSurface;
    psurf_gbl = psurf->lpGbl;

    if (psurf_gbl->fpVidMem == 0)
        {
        lpDestroySurface->ddRVal = DD_OK;
        goto DESTROY_HANDLED;
        }

    /*
       Should check if YUV surface is an active VPE surface and make sure
       to stop VPE hardware before destroying VPE surface somewhere YUV
       surface code in this function.
    */

    if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
        (psurf->ddsCaps.dwCaps & (DDSCAPS_VISIBLE | DDSCAPS_FRONTBUFFER)))
        {
        //**************************************************************
        // Ensure GDI channel has completed processing.
        //**************************************************************
        ENTER_DMA_PUSH_SYNC();
        NV_DD_DMA_PUSHER_SYNC();


        freeCount = ppdev->NVFreeCount;
        ppdev->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;

        WAIT_OVERLAY_NOTIFIER(0);
        WAIT_OVERLAY_NOTIFIER(1);

        while (freeCount < 3*4)
            freeCount = NvGetFreeCount(npDev, BLIT_SUBCHANNEL);
        freeCount -= 3*4;

        npDev->dDrawSpare.SetObject = NV_DD_VIDEO_OVERLAY;
        npDev->dDrawSpare.Nv04VideoOverlay.StopOverlay[0] = NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE;
        npDev->dDrawSpare.Nv04VideoOverlay.StopOverlay[1] = NV07A_STOP_OVERLAY_AS_SOON_AS_POSSIBLE;

        ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_OVERLAY;

        ppdev->NVFreeCount = (short)freeCount;
        END_DMA_PUSH_SYNC();
        }

    if (
       // (psurf->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
       (psurf_gbl->ddpfSurface.dwFlags & DDPF_FOURCC) )
        {
        DPF( "  FOURCC surface" );

        switch ( psurf->lpGbl->ddpfSurface.dwFourCC )
            {
            case FOURCC_RGB0:
                if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
                    {
                    if (psurf_gbl->fpVidMem > 0)
                        {
                        NVHEAP_FREE( psurf_gbl->fpVidMem);
                        psurf_gbl->fpVidMem = 0;
                        if (ppdev->DDrawVideoSurfaceCount > 0)
                            ppdev->DDrawVideoSurfaceCount--;
                        }
                    }
                else
                    {
                    lpDestroySurface->ddRVal = DD_OK;
                    return DDHAL_DRIVER_NOTHANDLED;
                    }
                lpDestroySurface->ddRVal = DD_OK;
                goto DESTROY_HANDLED;
                break;
            case FOURCC_IF09:
            case FOURCC_YVU9:
            case FOURCC_IV32:
            case FOURCC_IV31:
            case FOURCC_YUY2:
            case FOURCC_UYVY:
                if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                    {
                    while (index < NV_MAX_OVERLAY_SURFACES)
                        {
                        if ((DWORD)psurf == ppdev->dwOverlaySurfaceLCL[index])
                            {
                            ppdev->dwOverlaySurfaceLCL[index] = 0;
                            if (ppdev->dwOverlaySurfaces > 0)
                                ppdev->dwOverlaySurfaces--;
                            if (ppdev->dwOverlaySurfaces == 0)
                                {
                                ppdev->dwOverlayOwner = 0;
                                }
                            }
                        index++;
                        }
                    }

                if ((psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) &&
                    ((psurf->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY) == 0))
                    {
                    if (psurf_gbl->fpVidMem > 0)
                        {
                        NVHEAP_FREE(psurf_gbl->fpVidMem);
                        psurf_gbl->fpVidMem = 0;
                        if (ppdev->DDrawVideoSurfaceCount > 0)
                            ppdev->DDrawVideoSurfaceCount--;
                        }
                    }
                else
                    {
                    if ((ppdev->fNvActiveFloatingContexts) &&
                        ((psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
                         (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_YUY2)))
                        {
                        if (psurf->lpGbl->ddpfSurface.dwFourCC == FOURCC_UYVY)
                            {
                            if ((ppdev->fNvActiveFloatingContexts &
                                 NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                (ppdev->NvFloating0UYVYSurfaceMem == psurf_gbl->fpVidMem))
                                {

                                if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                                    {
                                    /* Mode switches might occur without warning */
                                    lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                                    goto DESTROY_HANDLED;
                                    }

                                ppdev->fNvActiveFloatingContexts &=
                                ~NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

                                }
                            else if ((ppdev->fNvActiveFloatingContexts &
                                      NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                     (ppdev->NvFloating1UYVYSurfaceMem == psurf_gbl->fpVidMem))
                                {
                                /* Trash spare subchannel */
                                if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                                    {
                                    /* Mode switches might occur without warning */
                                    lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                                    goto DESTROY_HANDLED;
                                    }

                                ppdev->fNvActiveFloatingContexts &=
                                ~NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
                                }
                            }
                        else
                            {
                            if ((ppdev->fNvActiveFloatingContexts &
                                 NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                (ppdev->NvFloating0YUYVSurfaceMem == psurf_gbl->fpVidMem))
                                {
                                if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                                    {
                                    /* Mode switches might occur without warning */
                                    lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                                    goto DESTROY_HANDLED;
                                    }

                                ppdev->fNvActiveFloatingContexts &=
                                ~NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

                                }
                            else if ((ppdev->fNvActiveFloatingContexts &
                                      NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                     (ppdev->NvFloating1YUYVSurfaceMem == psurf_gbl->fpVidMem))
                                {
                                if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                                    {
                                    /* Mode switches might occur without warning */
                                    lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                                    goto DESTROY_HANDLED;
                                    }

                                ppdev->fNvActiveFloatingContexts &=
                                ~NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

                                }
                            }
                        }

                    return(DDHAL_DRIVER_NOTHANDLED);
                    }
                lpDestroySurface->ddRVal = DD_OK;
                goto DESTROY_HANDLED;
                break;

            case FOURCC_YV12:
            case FOURCC_420i:
                if (psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY)
                    {
                    while (index < NV_MAX_OVERLAY_SURFACES)
                        {
                        if ((DWORD)psurf == ppdev->dwOverlaySurfaceLCL[index])
                            {
                            ppdev->dwOverlaySurfaceLCL[index] = 0;
                            if (ppdev->dwOverlaySurfaces > 0)
                                ppdev->dwOverlaySurfaces--;
                            if (ppdev->dwOverlaySurfaces == 0)
                                {
                                ppdev->dwOverlayOwner = 0;
                                }
                            }
                        index++;
                        }
                    }

                if ((psurf->ddsCaps.dwCaps & DDSCAPS_OVERLAY) &&
                    (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY))
                    {
                    if (psurf_gbl->fpVidMem == ppdev->NvFloatingMem2MemFormatBaseAddress)
                        {

                        if (ppdev->NvYUY2Surface0Mem > 0)
                            {
                            NVHEAP_FREE(ppdev->NvYUY2Surface0Mem);
                            ppdev->NvYUY2Surface0Mem = 0;
                            if (ppdev->DDrawVideoSurfaceCount > 0)
                                ppdev->DDrawVideoSurfaceCount--;
                            }

                        if (npDev == NULL)
                            {  /* Mode switches might occur without warning */
                            lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                            goto DESTROY_HANDLED;
                            }
#ifdef SYSMEM_FALLBACK
                        if ((ppdev->NvFloatingMem2MemFormatBaseAddress < (ULONG) ppdev->pjFrameBufbase) || /* If system memory */
                            (ppdev->NvFloatingMem2MemFormatBaseAddress > (ULONG) ppdev->pjFrameBufbase + 0x1000000))
                            {
                            if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                                {
                                /* Mode switches might occur without warning */
                                lpDestroySurface->ddRVal = DDERR_SURFACELOST;
                                goto DESTROY_HANDLED;
                                }
                            }
                        else
#endif // #ifdef SYSMEM_FALLBACK
                            {
                            NVHEAP_FREE(psurf_gbl->fpVidMem);
                            if (ppdev->DDrawVideoSurfaceCount > 0)
                                ppdev->DDrawVideoSurfaceCount--;
                            }

                        ppdev->NvFloatingMem2MemFormatBaseAddress = 0;
                        ppdev->NvFloatingMem2MemFormatNextAddress = 0;

                        }
                    else
                        {
                        if (psurf_gbl->fpVidMem == ppdev->NvFloatingMem2MemFormatNextAddress)
                            {
                            ppdev->NvFloatingMem2MemFormatNextAddress = 0;
                            if (ppdev->NvYUY2Surface1Mem > 0)
                                {
                                NVHEAP_FREE(ppdev->NvYUY2Surface1Mem);
                                ppdev->NvYUY2Surface1Mem = 0;
                                if (ppdev->DDrawVideoSurfaceCount > 0)
                                    ppdev->DDrawVideoSurfaceCount--;
                                }
                            }
                        else
                            {
                            if (ppdev->NvYUY2Surface2Mem > 0)
                                {
                                NVHEAP_FREE(ppdev->NvYUY2Surface2Mem);
                                ppdev->NvYUY2Surface2Mem = 0;
                                if (ppdev->DDrawVideoSurfaceCount > 0)
                                    ppdev->DDrawVideoSurfaceCount--;
                                }
                            }
                        }
                    psurf_gbl->fpVidMem = 0;
                    }
                else if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
                    {
                    if (psurf_gbl->fpVidMem > 0)
                        {
                        NVHEAP_FREE(psurf_gbl->fpVidMem);
                        psurf_gbl->fpVidMem = 0;
                        if (ppdev->DDrawVideoSurfaceCount > 0)
                            ppdev->DDrawVideoSurfaceCount--;
                        }
                    }
                else
                    {
                    return(DDHAL_DRIVER_NOTHANDLED);
                    }
                lpDestroySurface->ddRVal = DD_OK;
                goto DESTROY_HANDLED;
                break;
            }
        }
    else if ((psurf->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN) &&
             (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY))
        {
        if (psurf_gbl->fpVidMem > 0)
            {
            NVHEAP_FREE(psurf_gbl->fpVidMem);
            if (ppdev->DDrawVideoSurfaceCount > 0)
                ppdev->DDrawVideoSurfaceCount--;
            psurf_gbl->fpVidMem = 0;
            }
        DPF( "    Offscreen deallocated" );
        lpDestroySurface->ddRVal = DD_OK;
        goto DESTROY_HANDLED;
        }
    else if (psurf->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
        {
        if (psurf_gbl->fpVidMem > 0)
            {
            NVHEAP_FREE(psurf_gbl->fpVidMem);
            if (ppdev->DDrawVideoSurfaceCount > 0)
                ppdev->DDrawVideoSurfaceCount--;
            psurf_gbl->fpVidMem = 0;
            }
        else
            {
            DPF( "    Unspecified Video Memory deallocation not handled");
            return DDHAL_DRIVER_NOTHANDLED;
            }
        DPF( "    Offscreen deallocated" );
        lpDestroySurface->ddRVal = DD_OK;
        goto DESTROY_HANDLED;
        }
    else if (psurf->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
        {
        DPF( "    System memory deallocated" );
        return DDHAL_DRIVER_NOTHANDLED;
        }

    return DDHAL_DRIVER_NOTHANDLED;

DESTROY_HANDLED:
    return DDHAL_DRIVER_HANDLED;

    }

//
// From nv_ref.h - RM guys may some day give a better way to read this
//
#define NV_PBUS_PCI_NV_18                                0x00001848 /* RW-4R */
#define NV_PBUS_PCI_NV_19                                0x0000184C /* RW-4R */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE                8:8 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE                 4:4 /* RWIVF */
#define NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE                9:9 /* RWIVF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_FW                         4:4 /* R--VF */
#define NV_PBUS_PCI_NV_18_AGP_STATUS_SBA                        9:9 /* R--VF */

// search for a particular class in a list of classes
// return TRUE if found, FALSE if not
// (right now this is just a dumb linear search. it could be better...)

static __inline BOOL nvClassListCheck
(
    NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS params,
    DWORD dwClass
)
{
    NvU32 i=0;
    while (i < params.numClasses) {
        if (params.classBuffer[i] == dwClass) {
            return (TRUE);
        }
        i++;
    }
    return (FALSE);
}

//
// nvDeterminePerformanceStrategy
//
// detects HW and use this info together with the CPU info to figure out the best
//  performance strategy. pDriverData->dwDeviceIDNum must be initialized
//
// Note: This was copied and modified from a routine of similar name in nvProf.
//       That routine is highly non-portable.
EXTERN_C BOOL __stdcall nvDeterminePerformanceStrategy_NT4(GLOBALDATA *pDriverData, NV_SystemInfo_t *pSysInfo)
{
    DWORD dwRoot, dwDevID;
    BOOL  bOverride = FALSE;
    DWORD dwWidth;
    DWORD dwNVCLK;
    DWORD dwMCLK;
    DWORD dwGPUMemory;
    DWORD dwCpuClock;
    DWORD dwCpuType;
    DWORD dwFullspeedFW;
    DWORD dwFastWrite;
    DWORD dwBusCaps;
    DWORD dwStrapCaps;
    DWORD dwBase;
    DWORD dwBusType;
    DWORD dwPCIID;
    HANDLE hDriver;
    NV_CFGEX_GET_SUPPORTED_CLASSES_PARAMS params;
    DWORD dwSuccess;
    DWORD dwPSOr;
    DWORD dwPSAnd;
    DWORD dwPBSize;
    DWORD dwSBSize;
    DWORD dwCaps;
    DWORD res;
    union
    {
            ULONG osName;
            char devName[NV_DEVICE_NAME_LENGTH_MAX+1];
    } nameBuffer;

    pSysInfo->dwNVClasses      = 0;
    pSysInfo->dwCPUFeatureSet  = 0;
    pSysInfo->dwProcessorSpeed = 0;

    hDriver = (HANDLE) pDriverData->ppdev->hDriver;

    //
    // determine amount of memory present
    //
    // roughly the number of MB of system memory on the machine

//    pSysInfo->dwSystemMemory = (DWORD)((pDriverData->ppdev->SysMemInfo.SystemMemorySize) >> 20);
    // No way to query true amount of system memory on NT4 from disp driver (yet anyway) @mjl@
    pSysInfo->dwSystemMemory = 16;

    // round to the ceiling 8 MB
    pSysInfo->dwSystemMemory = (pSysInfo->dwSystemMemory + 7) & ~7;

    /*
     * determine installed HW
     *
     * at this point ddraw has not been initialized yet so this code is
     *  opening a RM channel for itself to do the detection and then
     *  close it.
     */

    /*
    * Under WINNT, must id the correct device in a  multimon
    * system.
    */

    nameBuffer.osName = pDriverData->ppdev->ulDeviceReference;

    dwDevID = 0xDEAD0001;

    NvOpen(hDriver);

    res = NvAllocRoot(hDriver,NV01_ROOT,&dwRoot);
    if (res != NVOS01_STATUS_SUCCESS) return FALSE;

    res = NvAllocDevice(hDriver, dwRoot, dwDevID, NV01_DEVICE_0, (BYTE*)nameBuffer.devName);
    if (res != NVOS06_STATUS_SUCCESS) return FALSE;

    // get general HW info
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_DAC_INPUT_WIDTH,   &dwWidth);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_DAC_GRAPHICS_CLOCK,&dwNVCLK);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_DAC_MEMORY_CLOCK,  &dwMCLK);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_RAM_SIZE_MB,       &dwGPUMemory);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_PROCESSOR_SPEED,   &dwCpuClock);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_PROCESSOR_TYPE,    &dwCpuType);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_ADDRESS_NVADDR,    &dwBase);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_AGP_FULLSPEED_FW,  &dwFullspeedFW);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_AGP_FW_ENABLE,     &dwFastWrite);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_BUS_TYPE,          &dwBusType);
    NvConfigGet(hDriver, dwRoot, dwDevID, NV_CFG_PCI_ID,            &dwPCIID);

    // get the list of classes this HW supports

    params.numClasses  = 0;
    params.classBuffer = NULL;

    // call to get the number of supported classes
    dwSuccess = NvConfigGetEx (hDriver, dwRoot, dwDevID, NV_CFGEX_GET_SUPPORTED_CLASSES, &params, sizeof(params));
    nvAssert (dwSuccess == NVOS_CGE_STATUS_SUCCESS);

    // allocate and call again to actually get the class list itself
    params.classBuffer   = (NvU32 *)  EngAllocMem( HEAP_ZERO_MEMORY, params.numClasses * sizeof(NvU32), ALLOC_TAG);

    dwSuccess = NvConfigGetEx (hDriver, dwRoot, dwDevID, NV_CFGEX_GET_SUPPORTED_CLASSES, &params, sizeof(params));
    nvAssert (dwSuccess == NVOS_CGE_STATUS_SUCCESS);

    // parse the class list
    if (nvClassListCheck (params, 0x0055))  pSysInfo->dwNVClasses |= NVCLASS_0055_DX6TRI;
    if (nvClassListCheck (params, 0x0095))  pSysInfo->dwNVClasses |= NVCLASS_0095_DX6TRI;

    if (nvClassListCheck (params, 0x0054))  pSysInfo->dwNVClasses |= NVCLASS_0054_DX5TRI;
    if (nvClassListCheck (params, 0x0094))  pSysInfo->dwNVClasses |= NVCLASS_0094_DX5TRI;

    if (nvClassListCheck (params, 0x0056))  pSysInfo->dwNVClasses |= NVCLASS_0056_CELSIUS;
    if (nvClassListCheck (params, 0x0096))  pSysInfo->dwNVClasses |= NVCLASS_0096_CELSIUS;
    if (nvClassListCheck (params, 0x1196))  pSysInfo->dwNVClasses |= NVCLASS_1196_CELSIUS;

    if (nvClassListCheck (params, 0x0097))  pSysInfo->dwNVClasses |= NVCLASS_0097_KELVIN;

    if (nvClassListCheck (params, 0x0046))  pSysInfo->dwNVClasses |= NVCLASS_0046_DAC;
    if (nvClassListCheck (params, 0x0049))  pSysInfo->dwNVClasses |= NVCLASS_0049_DAC;
    if (nvClassListCheck (params, 0x0067))  pSysInfo->dwNVClasses |= NVCLASS_0067_DAC;
    if (nvClassListCheck (params, 0x007C))  pSysInfo->dwNVClasses |= NVCLASS_007C_DAC;

    if (nvClassListCheck (params, 0x0042))  pSysInfo->dwNVClasses |= NVCLASS_0042_CTXSURF2D;
    if (nvClassListCheck (params, 0x0062))  pSysInfo->dwNVClasses |= NVCLASS_0062_CTXSURF2D;

    if (nvClassListCheck (params, 0x0060))  pSysInfo->dwNVClasses |= NVCLASS_0060_IDXIMAGE;
    if (nvClassListCheck (params, 0x0064))  pSysInfo->dwNVClasses |= NVCLASS_0064_IDXIMAGE;

    if (nvClassListCheck (params, 0x0077))  pSysInfo->dwNVClasses |= NVCLASS_0077_SCALEDIMG;
    if (nvClassListCheck (params, 0x0063))  pSysInfo->dwNVClasses |= NVCLASS_0063_SCALEDIMG;
    if (nvClassListCheck (params, 0x0089))  pSysInfo->dwNVClasses |= NVCLASS_0089_SCALEDIMG;

    if (nvClassListCheck (params, 0x0038))  pSysInfo->dwNVClasses |= NVCLASS_0038_DVDPICT;
    if (nvClassListCheck (params, 0x0088))  pSysInfo->dwNVClasses |= NVCLASS_0088_DVDPICT;

    if (nvClassListCheck (params, 0x007A))  pSysInfo->dwNVClasses |= NVCLASS_007A_OVERLAY;

    if (nvClassListCheck (params, 0x0053))  pSysInfo->dwNVClasses |= NVCLASS_0053_CTXSURF3D;
    if (nvClassListCheck (params, 0x0093))  pSysInfo->dwNVClasses |= NVCLASS_0093_CTXSURF3D;

    if (nvClassListCheck (params, 0x004A))  pSysInfo->dwNVClasses |= NVCLASS_004A_GDIRECT;
    if (nvClassListCheck (params, 0x005E))  pSysInfo->dwNVClasses |= NVCLASS_005E_SOLIDRECT;

    if (nvClassListCheck (params, 0x0052))  pSysInfo->dwNVClasses |= NVCLASS_0052_CTXSURFSWZ;
    if (nvClassListCheck (params, 0x009E))  pSysInfo->dwNVClasses |= NVCLASS_009E_CTXSURFSWZ;

    if (nvClassListCheck (params, 0x0004))  pSysInfo->dwNVClasses |= NVCLASS_0004_TIMER;

    if (nvClassListCheck (params, 0x006C))  pSysInfo->dwNVClasses1 |= NVCLASS1_006C_CHANNELDMA;
    if (nvClassListCheck (params, 0x006E))  pSysInfo->dwNVClasses1 |= NVCLASS1_006E_CHANNELDMA;
    if (nvClassListCheck (params, 0x206E))  pSysInfo->dwNVClasses1 |= NVCLASS1_206E_CHANNELDMA;

    // free the dynamically allocated list
    EngFreeMem(params.classBuffer);

    dwNVCLK = (dwNVCLK + 500000) / 1000000;
    dwMCLK  = (dwMCLK  + 500000) / 1000000;

    // disconnect
    {
        DWORD res;
        res = NvFree (hDriver, dwRoot, NV01_NULL_OBJECT, dwRoot);
        if (res != NVOS00_STATUS_SUCCESS) return FALSE;
    }
    NvClose(hDriver);

    dwStrapCaps = *(DWORD*)(dwBase + NV_PBUS_PCI_NV_18);
    dwBusCaps   = *(DWORD*)(dwBase + NV_PBUS_PCI_NV_19);

    //
    // override
    //
#ifdef FORCE_NV10_AS_NV5
    pSysInfo->dwNVClasses &= ~(NVCLASS_0056_CELSIUS |
                                                NVCLASS_0096_CELSIUS |
                                                NVCLASS_1196_CELSIUS);
#endif

#ifdef SPOOF_KELVIN
    pSysInfo->dwNVClasses |= NVCLASS_0097_KELVIN;
#endif

    //
    // map cpu onto feature flags
    //
    pSysInfo->dwProcessorSpeed = dwCpuClock;
    if (dwCpuType & NV_CPU_FUNC_MMX)     pSysInfo->dwCPUFeatureSet |= FS_MMX;
    if (dwCpuType & NV_CPU_FUNC_SSE)     pSysInfo->dwCPUFeatureSet |= FS_KATMAI;
    if (dwCpuType & NV_CPU_FUNC_SSE2)    pSysInfo->dwCPUFeatureSet |= FS_WILLAMETTE;
    if (dwCpuType & NV_CPU_FUNC_3DNOW)   pSysInfo->dwCPUFeatureSet |= FS_3DNOW;
    if (dwCpuType & NV_CPU_FUNC_SFENCE)  pSysInfo->dwCPUFeatureSet |= FS_SFENCE;
    if ((dwCpuType & 0xff) == NV_CPU_K7) pSysInfo->dwCPUFeatureSet |= FS_ATHLON;

    if ((pSysInfo->dwCPUFeatureSet & FS_KATMAI) ||
        (pSysInfo->dwCPUFeatureSet & FS_WILLAMETTE) ||
        (pSysInfo->dwCPUFeatureSet & FS_ATHLON))
    {
        pSysInfo->dwCPUFeatureSet |= FS_PREFETCH;
    }

    // See if this is a PCI bus interface
    if (dwBusType == 1)
    {
        pSysInfo->dwCPUFeatureSet |= FS_PCIBUS;
    }

    //
    // map misc flags
    //
    // fast writes
    if (dwFastWrite && dwFullspeedFW)   pSysInfo->dwCPUFeatureSet |= FS_FULLFASTWRITE;
    if (dwFastWrite && !dwFullspeedFW)  pSysInfo->dwCPUFeatureSet |= FS_HALFFASTWRITE;
    // detect low back end bus
    if (dwWidth == 64)                  pSysInfo->dwCPUFeatureSet |= FS_64BIT;

    //
    // determine strategy...
    //
    dwPSOr  =  0;
    dwPSAnd = ~0;
    {
        BOOL bFastMachine      = pSysInfo->dwProcessorSpeed >= 390;
        BOOL bRealFastMachine  = pSysInfo->dwProcessorSpeed >= 650; //650
        BOOL bSuperFastBackEnd = dwMCLK >= 300;
        // bFastBackEnd should only be FALSE on VANTA
        BOOL bFastBackEnd      = !(pSysInfo->dwCPUFeatureSet & FS_64BIT)
                               || (dwMCLK > 200);
        BOOL bHalfFastWrites   = pSysInfo->dwCPUFeatureSet & FS_HALFFASTWRITE;
        BOOL bFullFastWrites   = pSysInfo->dwCPUFeatureSet & FS_FULLFASTWRITE;
        BOOL bHasAGP           = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE));

        //
        // slow back end
        //
        if (!bFastBackEnd)
        {
            if (bFastMachine) pSysInfo->dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_RENAME     |
                                                                                 PS_TEXTURE_PREFER_AGP;
                         else pSysInfo->dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_USEHW;
        }
#if 0 // (NVARCH >= 0x020)
        else if (pSysInfo->dwNVClasses & NVCLASS_FAMILY_KELVIN)
        {
            // put stuff here
        }
#endif
        else if (pSysInfo->dwNVClasses & NVCLASS_FAMILY_CELSIUS)
        {
            // decide who does what
            pSysInfo->dwPerformanceStrategy = PS_TEXTURE_RENAME | PS_TEXTURE_USEHW;
            // decide where to put stuff
            switch ((bFullFastWrites   ? 4 : 0)
                  | (bHalfFastWrites   ? 2 : 0)
                  | (bSuperFastBackEnd ? 1 : 0))
            {
                case 0: // !FW, !SF   - bx, SDR
                        pSysInfo->dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pSysInfo->dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 1: // !FW,  SF   - bx, DDR
                        pSysInfo->dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pSysInfo->dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 2: //  1/2 FW, !SF   - camino, SDR
                        pSysInfo->dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pSysInfo->dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 3: //  1/2 FW,  SF   - camino, DDR
                        pSysInfo->dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pSysInfo->dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 6: //  FW, !SF   - camino, SDR
                        pSysInfo->dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pSysInfo->dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
                case 7: //  FW,  SF   - camino, DDR (we have not experimentd with this one yet - be conservative)
                        pSysInfo->dwPerformanceStrategy &= ~PS_PUSH_VID;
                        pSysInfo->dwPerformanceStrategy |=  PS_VB_PREFER_AGP;
                        break;
            }

            if (pSysInfo->dwCPUFeatureSet & FS_PCIBUS)
            {
                // PCI bus interface -- don't prefer AGP!
                pSysInfo->dwPerformanceStrategy &=  ~PS_VB_PREFER_AGP;
            }

            // should we use auto-palettes?
            if (pSysInfo->dwNVClasses & NVCLASS_FAMILY_CELSIUS)
            {
                pSysInfo->dwPerformanceStrategy |= PS_AUTOPALETTE;
            }

            //camino (i.e. FW's) do 32 bit perf strategy.
            if ((pSysInfo->dwNVClasses & NVCLASS_1196_CELSIUS)
             || (pSysInfo->dwNVClasses & NVCLASS_0096_CELSIUS)) {
                pSysInfo->dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_32;
                pSysInfo->dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_16;
            }

            if (pSysInfo->dwNVClasses & NVCLASS_FAMILY_KELVIN)
            {
                // No CT for Kelvin
                pSysInfo->dwPerformanceStrategy &= ~(PS_CONTROL_TRAFFIC_16 | PS_CONTROL_TRAFFIC_32);
            }

            // should we do super triangles?
#ifdef SUPERTRI
           if (bRealFastMachine
              && ((pSysInfo->dwCPUFeatureSet & FS_KATMAI)
                    || (pSysInfo->dwCPUFeatureSet & FS_WILLAMETTE)
                    || (pSysInfo->dwCPUFeatureSet & FS_ATHLON)))
            {
                pSysInfo->dwPerformanceStrategy |= (PS_SUPERTRI | PS_ST_XFORMCLIP | PS_ST_MODEL);

                // TODO: Add transformation and clip code for Athlon
                if (pSysInfo->dwCPUFeatureSet & FS_ATHLON)
                    pSysInfo->dwPerformanceStrategy &= ~PS_ST_XFORMCLIP;
            }
#endif
        }
        // ben BUGBUG
        // this is ugly. we infer nv4 vs. nv5 based on the DAC object. philosophically, this is illegal.
        else if (pSysInfo->dwNVClasses & NVCLASS_0049_DAC)
        {
            if (bFastMachine) pSysInfo->dwPerformanceStrategy = PS_CONTROL_TRAFFIC_32 | PS_TEXTURE_RENAME;
                         else pSysInfo->dwPerformanceStrategy = PS_CONTROL_TRAFFIC_32 | PS_TEXTURE_USEHW;
        }
        else
        {
            if (bFastMachine) pSysInfo->dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_RENAME     |
                                                                                 PS_TEXTURE_PREFER_AGP;
                         else pSysInfo->dwPerformanceStrategy = PS_CONTROL_TRAFFIC_16 |
                                                                                 PS_CONTROL_TRAFFIC_32 |
                                                                                 PS_TEXTURE_USEHW;
        }

#ifdef FORCE_CT
#if FORCE_CT
    pSysInfo->dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_32;
    pSysInfo->dwPerformanceStrategy |= PS_CONTROL_TRAFFIC_16;
#else
    pSysInfo->dwPerformanceStrategy &= ~PS_CONTROL_TRAFFIC_32;
    pSysInfo->dwPerformanceStrategy &= ~PS_CONTROL_TRAFFIC_16;
#endif
#endif 

    }

/*
    pSysInfo->dwPerformanceStrategy = (1 ? PS_TEXTURE_RENAME     : 0)
                                 | (0 ? PS_TEXTURE_USEHW      : 0)
                                 | (1 ? PS_TEXTURE_PREFER_AGP : 0)
                                 | (1 ? PS_CONTROL_TRAFFIC_16 : 0)
                                 | (1 ? PS_CONTROL_TRAFFIC_32 : 0);
//*/

#define SIZE_VIDMEM_PUSH_BUFFER         (1.0f /  32.0f)     // of total vid mem
#define SIZE_VIDMEM_STAGE_BUFFER        (2.0f /  32.0f)     // of total vid mem

#define SIZE_SYSMEM_PUSH_BUFFER         (2.0f / 128.0f)     // of total sys mem (buffer lives in PCI or AGP)
#define SIZE_SYSMEM_STAGE_BUFFER        (4.0f / 128.0f)     // of total sys mem (buffer lives in PCI or AGP)


    if (pSysInfo->dwPerformanceStrategy & PS_PUSH_VID) // buffers in video memory
    {
        dwPBSize = (DWORD)(1024.0f * 1024.0f * dwGPUMemory * SIZE_VIDMEM_PUSH_BUFFER);
        dwSBSize = (DWORD)(1024.0f * 1024.0f * dwGPUMemory * SIZE_VIDMEM_STAGE_BUFFER);
    }
    else // buffers in AGP or PCI memory
    {
        // base AGP buffer sizes on aperture or memory size, depending on their relative sizes
        DWORD dwMem = pSysInfo->dwSystemMemory;
        dwPBSize = (DWORD)(1024.0f * 1024.0f * dwMem * SIZE_SYSMEM_PUSH_BUFFER);
    }

    dwPBSize = max(dwPBSize, 128 * 1024); // must be at least 128K

    dwPBSize = (dwPBSize + 4095) & ~4095; // 4K align

    // limit maximum size of push buffer
    dwPBSize = min(dwPBSize, 8 * 1024 * 1024); // 8MB max

    pSysInfo->dwRecommendedPushBufferSize  = dwPBSize;

#define PRINT
#if defined(DEBUG) || defined(PRINT)
    {
        static BOOL bPrintedThisBefore = FALSE;

        if (!bPrintedThisBefore)
        {
            char szString[128];

            PF (" NV: classes=%08x %d/%d RAM=%dMB bus=%d", pSysInfo->dwNVClasses,dwNVCLK,dwMCLK,dwGPUMemory,dwWidth);
            PF ("CPU: FS=%08x %dMHz", pSysInfo->dwCPUFeatureSet, pSysInfo->dwProcessorSpeed);

            {
                BOOL  bAGP   = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_AGP_ENABLE));
                DWORD dwRate = dwBusCaps ? (dwBusCaps & 7) : 0;
                BOOL  bSBA   = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_SBA_ENABLE));
                BOOL  bFW    = dwBusCaps & (1 << (0 ? NV_PBUS_PCI_NV_19_AGP_COMMAND_FW_ENABLE));

                PF (" BC: AGP=%dx, SBA=%d, HFW=%d, SSFW=%d, SFFW=%d",
                     bAGP ? dwRate : 0,
                     bSBA ? 1 : 0,
                     bFW ? 1 : 0,
                     (pSysInfo->dwCPUFeatureSet & FS_HALFFASTWRITE) ? 1 : 0,
                     (pSysInfo->dwCPUFeatureSet & FS_FULLFASTWRITE) ? 1 : 0);
            }
            {
                DWORD dwRate = dwStrapCaps ? (dwStrapCaps & 7) : 0;
                BOOL  bSBA   = dwStrapCaps & (1 << (0 ? NV_PBUS_PCI_NV_18_AGP_STATUS_SBA));
                BOOL  bFW    = dwStrapCaps & (1 << (0 ? NV_PBUS_PCI_NV_18_AGP_STATUS_FW));

                PF (" SC: AGP=%s%s%s%s, SBA=%d, FW=%d",
                     (dwRate & 4) ? "4" : "",
                     (dwRate & 2) ? "2" : "",
                     (dwRate & 1) ? "1" : "",
                     (dwRate & 7) ? "x" : "n/a",
                     bSBA ? 1 : 0,
                     bFW ? 1 : 0);
            }

            PF (" PS: %08x", pSysInfo->dwPerformanceStrategy);

            nvStrCpy (szString, "     ");
            if (pSysInfo->dwPerformanceStrategy & PS_TEXTURE_RENAME)     nvStrCat (szString, "texRn ");
            if (pSysInfo->dwPerformanceStrategy & PS_TEXTURE_USEHW)      nvStrCat (szString, "texHW ");
            if (pSysInfo->dwPerformanceStrategy & PS_TEXTURE_PREFER_AGP) nvStrCat (szString, "texAGP ");
            if (pSysInfo->dwPerformanceStrategy & PS_CONTROL_TRAFFIC_16) nvStrCat (szString, "ct16 ");
            if (pSysInfo->dwPerformanceStrategy & PS_CONTROL_TRAFFIC_32) nvStrCat (szString, "ct32 ");
            if (pSysInfo->dwPerformanceStrategy & PS_VB_RENAME)          nvStrCat (szString, "vbRn ");
            if (pSysInfo->dwPerformanceStrategy & PS_VB_PREFER_AGP)      nvStrCat (szString, "vbAGP ");
            if (pSysInfo->dwPerformanceStrategy & PS_PUSH_VID)           nvStrCat (szString, "pushVID ");
            if (pSysInfo->dwPerformanceStrategy & PS_SUPERTRI)           nvStrCat (szString, "supTri ");
            PF (szString);

            if (bOverride) {
                PF ("     * PS set from registry");
            }

            bPrintedThisBefore = TRUE;
        }
    }
#endif

    // calibrate delay loop
   //    assume cpuid capable
    __asm
    {
        push    eax
        push    ebx
        push    ecx
        push    edx
        mov eax,1

        _emit 0x0f //    __asm cpuid - not known to compiler used for NT4 build
        _emit 0xa2 

        mov [dwCaps],edx
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    if (dwCaps & 0x10) // rdtsc capable
    {
        __int64 qwStart;
        __int64 qwStop;

        nvSpin(10000);
        nvSpin(10000);

        __asm
        {
            push    eax
            push    edx

            _emit 0x0f //    __asm rdtsc - not known to compiler used for NT4 build
            _emit 0x31 

            mov DWORD PTR [qwStart + 0],eax
            mov DWORD PTR [qwStart + 4],edx

            pop     edx
            pop     eax
        }
        nvSpin(10000);
        __asm
        {
            push    eax
            push    edx

            _emit 0x0f //    __asm rdtsc - not known to compiler used for NT4 build
            _emit 0x31 

            mov DWORD PTR [qwStop + 0],eax
            mov DWORD PTR [qwStop + 4],edx

            pop     edx
            pop     eax

            // 3DMark 2000 leaves the floating point stack full of garbage
            // which causes the 3rd FILD below to fail resulting in a global.dwDelayCount of 0 and
            // ultimately causing the driver to hang
            // (perhaps they use MMX instructions)
            // clearing the MMX state here solves the problem
        }

        // this used to cause a fault on non-MMX machines.  Have to check for MMX before using
        // EMMS
        if (pSysInfo->dwCPUFeatureSet & FS_MMX)
        {
            __asm emms
        }

        // set delay to 10us
        double time = (double)(qwStop - qwStart) / 10000.0;
        time /= (double) pSysInfo->dwProcessorSpeed;
        pSysInfo->dwSpinLoopCount = (unsigned long) (10.0 / time);
    }
    else
    {
        // for non tsc machines we just assign a hard value for the delay loop
        pSysInfo->dwSpinLoopCount = pSysInfo->dwProcessorSpeed * 4;
    }

    return TRUE;
}



