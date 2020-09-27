/******************************Module*Header*******************************\
* Module Name: oglsurf.c
*
* Single-back-depth surface API.
*
* Copyright (c) 1998 NVidia Corporation
\**************************************************************************/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#pragma hdrstop
#include "nv32.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "oglDD.h"
#include "oglutils.h"
#include "rectUtils.h"
#include "oglstereo.h"
#include "ogloverlay.h"
#include "oglflip.h"
#include "oglsync.h"

#if DBG
#define OglDebugPrint OglDebugPrintFunc
#else
#define OglDebugPrint
#endif

//******************************************************************************
//
//  Function: OglAllocOverlaySurfaces
//
//  Routine Description:
//            Allocate 4 auxiliary overlay surfaces for opengl pseudo
//            hardware implementation of overlay planes
//
//  Arguments:
//
//  Return Value:
//            TRUE if succeeds
//
//
//******************************************************************************
static int
OglAllocOverlaySurfaces(
    PDEV  *ppdev,
    ULONG  ulView
)
{
    RBRUSH_COLOR  rbc  = {0};    // solid colour brush
    RECTL         rclScreen;
    int   iRet   = FALSE;
    DWORD   dwStatus;

    //__asm int 3;
    // Alloc front main surface

    rbc.iSolidColor = ICD_PALETTE_TRANSPARENT_COLOR | 0xFF000000;


    // at this time we don't support real stereo-overlay
    ASSERT( OGL_STEREO_BUFFER_LEFT == ulView );
    ASSERT( NULL != ppdev );

    // desktop extend to clear overlaybuffers after creation
    rclScreen.left   = 0;
    rclScreen.top    = 0;
    rclScreen.right  = ppdev->cxScreen;
    rclScreen.bottom = ppdev->cyScreen;

    ppdev->singleFrontMainOffset[ulView] = 0;
    ppdev->singleFrontMainPitch[ulView]  = (ppdev->lDelta + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
    NVHEAP_ALLOC_TILED(dwStatus,ppdev->singleFrontMainOffset[ulView], (DWORD)ppdev->singleFrontMainPitch[ulView], (DWORD)ppdev->cyScreen, TYPE_IMAGE);

    if (dwStatus!=0)
    {
        ppdev->singleFrontMainPitch[ulView] = 0;
        goto Exit; // failure
    }

    // Alloc back main surface
    ppdev->singleBack1MainOffset[ulView] = 0;
    ppdev->singleBack1MainPitch[ulView]  = (ppdev->lDelta + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
    NVHEAP_ALLOC_TILED(dwStatus,ppdev->singleBack1MainOffset[ulView], (DWORD)ppdev->singleBack1MainPitch[ulView], (DWORD)ppdev->cyScreen, TYPE_IMAGE);

    if (dwStatus!=0)
    {
        ppdev->singleBack1MainPitch[ulView] = 0;
        goto Exit; // failure
    }

    // Alloc front overlay surface
    ppdev->singleFrontOverlayOffset[ulView] = 0;
    ppdev->singleFrontOverlayPitch[ulView]  = (ppdev->lDelta + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
    NVHEAP_ALLOC_TILED(dwStatus,ppdev->singleFrontOverlayOffset[ulView], (DWORD)ppdev->singleFrontOverlayPitch[ulView], (DWORD)ppdev->cyScreen, TYPE_IMAGE);

    if (dwStatus!=0)
    {
        ppdev->singleFrontOverlayPitch[ulView] = 0;
        goto Exit; // failure
    }

    // Alloc back overlay surface
    ppdev->singleBack1OverlayOffset[ulView] = 0;
    ppdev->singleBack1OverlayPitch[ulView]  = (ppdev->lDelta + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
    NVHEAP_ALLOC_TILED(dwStatus,ppdev->singleBack1OverlayOffset[ulView], (DWORD)ppdev->singleBack1OverlayPitch[ulView], (DWORD)ppdev->cyScreen, TYPE_IMAGE);

    if (dwStatus!=0)
    {
        ppdev->singleBack1OverlayPitch[ulView] = 0;
        goto Exit; // failure
    }


    ppdev->pfnAcquireOglMutex(ppdev);
    ppdev->NVFreeCount = 0;
    //**********************************************************************
    // We need to make absolutely sure that the Channel switch
    // from the DMA pusher to the PIO channel has been completed
    //**********************************************************************
    ppdev->pfnWaitForChannelSwitch(ppdev);

    // clear overlay front and back with default transparent color
    ppdev->pfnSetDestBase(ppdev, ppdev->singleFrontOverlayOffset[ulView], ppdev->singleFrontOverlayPitch[ulView] );
    NV4DmaPushFillSolid( ppdev, 1, &rclScreen, 0xF0F0, rbc, NULL);
    ppdev->pfnSetDestBase(ppdev, ppdev->singleBack1OverlayOffset[ulView], ppdev->singleBack1OverlayPitch[ulView] );
    NV4DmaPushFillSolid( ppdev, 1, &rclScreen, 0xF0F0, rbc, NULL);

    ppdev->pfnReleaseOglMutex(ppdev);

    iRet = TRUE;

Exit:

    return iRet;
}

//******************************************************************************
//
//  Function: OglAllocUnifiedSurfaces
//
//  Routine Description:
//            Allocate the single-back-depth buffer that is the size of the
//            display for OpenGL to use.
//            Also allocates the overlay front/back (main/overlay) surfaces
//             for OpenGL pseudo hardware overlay support.
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//
//******************************************************************************
int
OglAllocUnifiedSurfaces(
    PDEV *ppdev,
    __GLNVsbdFlags *sbdFlags
)
{
    int iRet = FALSE;
    DWORD dwStatus;


    if (!ppdev->bOglSingleBackDepthSupported)
    {
        goto Exit;
    }

    // our intension is to allocate all buffers which may be allocated according to the exported
    // pixelformats at the FIRST time an allocation is done, to not run out of offscreen memory e.g.
    // if 1st ICD consumes a lot of it for textures and 2nd ICD wants more UBB buffers than 1st one.
    if (ppdev->bOglSingleBackDepthCreated == FALSE)
    {
        ULONG ulDesiredViews = 0;
        ULONG ulView ;

        ASSERT(FALSE==ppdev->bOglOverlaySurfacesCreated);

        bMoveAllDfbsFromOffscreenToDibs(ppdev);

        // if we export stereo pixelformat, allocate the buffer for it
        if( ppdev->bOglStereoDesired )
            ulDesiredViews = 2;
        else
            ulDesiredViews = 1;


        for( ulView = 0; ulView<ulDesiredViews; ulView++ )
        {
            DWORD depthType;

            // primary is allocated by DD, only handle other front buffers.
            if( ulView > 0 )
            {
                // allocate right front buffer
                ppdev->singleFrontOffset[ulView] = 0;
                ppdev->singleFrontPitch[ulView]  = (ppdev->lDelta + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
                NVHEAP_ALLOC_TILED(dwStatus,ppdev->singleFrontOffset[ulView], (DWORD)ppdev->singleFrontPitch[ulView], (DWORD)ppdev->cyScreen, TYPE_IMAGE);
                if (dwStatus!=0)
                    goto Exit;  // failure
            }

            // allocate back buffer
            ppdev->singleBack1Offset[ulView] = 0;
            ppdev->singleBack1Pitch[ulView]  = (ppdev->lDelta + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;
            NVHEAP_ALLOC_TILED(dwStatus,ppdev->singleBack1Offset[ulView], (DWORD)ppdev->singleBack1Pitch[ulView], (DWORD)ppdev->cyScreen, TYPE_IMAGE);
            if (dwStatus!=0)
                goto Exit;  // failure

            // TODO: allocate back buffer 2 for triple buffering.

            // allocate depth buffer
            ppdev->singleDepthOffset[ulView] = 0;
            ppdev->singleDepthPitch[ulView] = ppdev->singleBack1Pitch[ulView];
            if (sbdFlags->depthCompressed) {
                switch (ppdev->cjPelSize) {
                case 2:
                    depthType = TYPE_DEPTH_COMPR16;
                    break;
                case 4:
                    depthType = TYPE_DEPTH_COMPR32;
                    break;
                default:
                    depthType = TYPE_DEPTH;
                    break;
                }
            } else {
                depthType = TYPE_DEPTH;
            }
            NVHEAP_ALLOC_TILED(dwStatus,ppdev->singleDepthOffset[ulView], (DWORD)ppdev->singleDepthPitch[ulView], (DWORD)ppdev->cyScreen, depthType);
            if (dwStatus!=0)
                goto Exit;

            ppdev->bOglSingleBackDepthCreated = TRUE;
    
            // allocate Overlay surfaces
            if (ppdev->bOglOverlayDesired)
            {
                if(FALSE == OglAllocOverlaySurfaces(ppdev,ulView)) 
                {
                    goto Exit;
                } 
                else 
                {
                    ppdev->bOglOverlaySurfacesCreated = TRUE;
                }
            }

        }//for( ulView = 0; ulView<ulDesiredViews; ulView++ )

        ppdev->ulOglActiveViews = ulDesiredViews;

        // Try to enable pageflipping for that client
        bOglStereoModeUpdate(ppdev);
        bOglOverlayModeUpdate(ppdev);
        bOglPageFlipModeUpdate(ppdev);
    }

    iRet = TRUE;// success

Exit:

    if( FALSE == iRet )
    {
        OglFreeUnifiedSurfaces(ppdev);
    }

    return iRet; 
}

//******************************************************************************
//
//  Function: OglFreeUnifiedSurfaces
//
//  Routine Description:
//            Free a previously allocated single-back-depth buffer that is the size of the
//            display for OpenGL to use.
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//  MSchwarzer  10/09/2000: now we're freeing all surfaces whose offset is not 0
//
//******************************************************************************
int
OglFreeUnifiedSurfaces(
    PDEV *ppdev
)
{
    ULONG ulView;

    // Try to enable pageflipping for that client
    bOglPageFlipModeDisableAll(ppdev);
    bOglOverlayModeDisable(ppdev);
    bOglStereoModeDisable(ppdev);

    NV_DisableDoubleBufferMode(ppdev);
    
    for( ulView=0; ulView < ppdev->ulOglActiveViews; ulView++ )
    {
        // primary is allocated by DD, only handle other front buffers.
        if( ulView > OGL_STEREO_BUFFER_LEFT )
        {   
            if(ppdev->singleFrontOffset[ulView]) 
            {
                NVHEAP_FREE(ppdev->singleFrontOffset[ulView]);
            }
            ppdev->singleFrontOffset[ulView] = 0;
            ppdev->singleFrontPitch[ulView]  = 0;
        }

        if(ppdev->singleBack1Offset[ulView]) 
            NVHEAP_FREE(ppdev->singleBack1Offset[ulView]);
        ppdev->singleBack1Offset[ulView] = 0;
        ppdev->singleBack1Pitch[ulView]  = 0;

        if(ppdev->singleDepthOffset[ulView]) 
            NVHEAP_FREE(ppdev->singleDepthOffset[ulView]);
        ppdev->singleDepthOffset[ulView] = 0;
        ppdev->singleDepthPitch[ulView]  = 0;

        // overlay buffers
        if(ppdev->singleFrontMainOffset[ulView]) 
            NVHEAP_FREE(ppdev->singleFrontMainOffset[ulView]);
        ppdev->singleFrontMainOffset[ulView] = 0;
        ppdev->singleFrontMainPitch[ulView]  = 0;

        if(ppdev->singleFrontOverlayOffset[ulView]) 
            NVHEAP_FREE(ppdev->singleFrontOverlayOffset[ulView]);
        ppdev->singleFrontOverlayOffset[ulView] = 0;
        ppdev->singleFrontOverlayPitch[ulView]  = 0;

        if(ppdev->singleBack1MainOffset[ulView]) 
            NVHEAP_FREE(ppdev->singleBack1MainOffset[ulView]);
        ppdev->singleBack1MainOffset[ulView] = 0;
        ppdev->singleBack1MainPitch[ulView]  = 0;

        if(ppdev->singleBack1OverlayOffset[ulView]) 
            NVHEAP_FREE(ppdev->singleBack1OverlayOffset[ulView]);
        ppdev->singleBack1OverlayOffset[ulView] = 0;
        ppdev->singleBack1OverlayPitch[ulView]  = 0;

    }

    ppdev->bOglSingleBackDepthCreated = FALSE;
    ppdev->bOglOverlaySurfacesCreated = FALSE;
    ppdev->ulOglActiveViews           = 0;

    return 1;
}

NvU32 
OglGetUnifiedSurfaceInfo(
    PDEV *ppdev, 
    __GLNVsbdFlags *sbdFlags
)
{
    if (ppdev->bOglSingleBackDepthSupported &&
        ppdev->bOglSingleBackDepthCreated) {
            switch (sbdFlags->surfaceInfoType) {
                case SBD_BACK_1_OFFSET:
                    return ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT];
                case SBD_BACK_1_PITCH:
                    return ppdev->singleBack1Pitch[OGL_STEREO_BUFFER_LEFT];
                case SBD_DEPTH_OFFSET:
                    return ppdev->singleDepthOffset[OGL_STEREO_BUFFER_LEFT];
                case SBD_DEPTH_PITCH:
                    return ppdev->singleDepthPitch[OGL_STEREO_BUFFER_LEFT];
                
                // Overlays
                case SBD_FRONT_MAIN_OFFSET:
                    return ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT];
                case SBD_FRONT_MAIN_PITCH:
                    return ppdev->singleFrontMainPitch[OGL_STEREO_BUFFER_LEFT];
                case SBD_BACK_1_MAIN_OFFSET:
                    return ppdev->singleBack1MainOffset[OGL_STEREO_BUFFER_LEFT];
                case SBD_BACK_1_MAIN_PITCH:
                    return ppdev->singleBack1MainPitch[OGL_STEREO_BUFFER_LEFT];
                case SBD_FRONT_OVERLAY_OFFSET:
                    return ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT];
                case SBD_FRONT_OVERLAY_PITCH:
                    return ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_LEFT];
                case SBD_BACK_1_OVERLAY_OFFSET:
                    return ppdev->singleBack1OverlayOffset[OGL_STEREO_BUFFER_LEFT];
                case SBD_BACK_1_OVERLAY_PITCH:
                    return ppdev->singleBack1OverlayPitch[OGL_STEREO_BUFFER_LEFT];

                // ogl api stereo buffers
                case SBD_FRONT_RIGHT_OFFSET:
                    return ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT];
                case SBD_FRONT_RIGHT_PITCH:
                    return ppdev->singleFrontPitch[OGL_STEREO_BUFFER_RIGHT];
                case SBD_BACK_1_RIGHT_OFFSET:
                    return ppdev->singleBack1Offset[OGL_STEREO_BUFFER_RIGHT];
                case SBD_BACK_1_RIGHT_PITCH:
                    return ppdev->singleBack1Pitch[OGL_STEREO_BUFFER_RIGHT];
                case SBD_DEPTH_RIGHT_OFFSET:
                    return ppdev->singleDepthOffset[OGL_STEREO_BUFFER_RIGHT];
                case SBD_DEPTH_RIGHT_PITCH:
                    return ppdev->singleDepthPitch[OGL_STEREO_BUFFER_RIGHT];
                
                // Overlays
                case SBD_FRONT_MAIN_RIGHT_OFFSET:
                    return ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_RIGHT];
                case SBD_FRONT_MAIN_RIGHT_PITCH:
                    return ppdev->singleFrontMainPitch[OGL_STEREO_BUFFER_RIGHT];
                case SBD_BACK_1_MAIN_RIGHT_OFFSET:
                    return ppdev->singleBack1MainOffset[OGL_STEREO_BUFFER_RIGHT];
                case SBD_BACK_1_MAIN_RIGHT_PITCH:
                    return ppdev->singleBack1MainPitch[OGL_STEREO_BUFFER_RIGHT];
                case SBD_FRONT_OVERLAY_RIGHT_OFFSET:
                    return ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_RIGHT];
                case SBD_FRONT_OVERLAY_RIGHT_PITCH:
                    return ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_RIGHT];
                case SBD_BACK_1_OVERLAY_RIGHT_OFFSET:
                    return ppdev->singleBack1OverlayOffset[OGL_STEREO_BUFFER_RIGHT];
                case SBD_BACK_1_OVERLAY_RIGHT_PITCH:
                    return ppdev->singleBack1OverlayPitch[OGL_STEREO_BUFFER_RIGHT];

                default:
                    ASSERTDD(0,"Invalid surface info type");
                    return FALSE;
            }
    }
    return FALSE;
}

/*
**  bExchangeClientsFrontAndBack
**
**  Swaps the contents of fullscreen front and backbuffer of
**  complete opengl client area (clipped against screen).
**  It uses a temporary buffer to do its work. 
**
**  return: TRUE  - buffers copied + flipped to primary
**          FALSE - if no buffers copied ( e.g. no UBB-mode ), not flipped to primary
**
**  10/16/2000 MSchwarzer: new
*/
BOOL bExchangeClientsFrontAndBack( PPDEV ppdev, NV_OPENGL_DRAWABLE_INFO *pClientDrawableInfo)
{
    BOOL bBufferExchanged = FALSE;

    ASSERT(ppdev);
    ASSERT(pClientDrawableInfo);


    if(  (ppdev->bOglSingleBackDepthSupported)
       &&( ppdev->bOglSingleBackDepthCreated)
      )
    {
        RECTL rclSrc;
        RECTL rclScreen;

        vRclCopy(&rclSrc,(RECTL*)(&pClientDrawableInfo->rect));

        // convert to screen relative coords
        vRclSubOffset(&rclSrc,&rclSrc,ppdev->left,ppdev->top);

        // set up primary ccordinates
        rclScreen.left   = 0;
        rclScreen.right  = ppdev->cxScreen;
        rclScreen.top    = 0;
        rclScreen.bottom = ppdev->cyScreen;

        // clip clientrect against screen and check if its a empty rect
        if(   (bRclIntersect(&rclSrc,&rclScreen,&rclSrc))
            &&(!bRclIsEmpty(&rclSrc))
           )
        {
            ULONG  ulTempBufferOffset         = 0;
            ULONG  ulTempBufferSize           = 0;
            LONG   lTempBufferStride          = 0;
            BOOL   bAllocatedTempBufferOnHeap = FALSE;
            BOOL   bCopyAtOnce                = FALSE;
            RECTL  rclClipScreen;
            RECTL  rclClipTmpBuffer;
            POINTL ptlSrcScreen;
            POINTL ptlSrcTmpBuffer;
            LONG   lRow;
            LONG   cy;
            DWORD  dwStatus;

            // 32 BYTE- align !
            lTempBufferStride = (( ppdev->cjPelSize * lRclWidth(&rclSrc) ) + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign;

            ulTempBufferSize = lTempBufferStride * lRclHeight(&rclSrc);

            if( ulTempBufferSize > ppdev->ulTempOffscreenBufferSize )
            {
                NVHEAP_ALLOC(dwStatus, ulTempBufferOffset,  ulTempBufferSize, TYPE_IMAGE)
                if( dwStatus == 0 )
                {
                    bAllocatedTempBufferOnHeap = TRUE;
                    bCopyAtOnce                = TRUE;
                }
            }

            if(!bAllocatedTempBufferOnHeap)
            {
                ASSERT(0 != ppdev->ulTempOffscreenBufferSize);

                if( ulTempBufferSize <= ppdev->ulTempOffscreenBufferSize )
                    bCopyAtOnce = TRUE;

                ulTempBufferSize   = ppdev->ulTempOffscreenBufferSize;
                ulTempBufferOffset = ppdev->ulTempOffscreenBufferOffset;
            }

            // constant values
            ptlSrcTmpBuffer.x = 0;
            ptlSrcTmpBuffer.y = 0;

            // we may have to copy the clientrect via several blockcopies
            cy    = ulTempBufferSize / lTempBufferStride;

            ptlSrcScreen.x = rclSrc.left;
            ptlSrcScreen.y = rclSrc.top;

            rclClipScreen.left   = rclSrc.left;
            rclClipScreen.right  = rclSrc.right;
            rclClipScreen.top    = rclSrc.top;
            rclClipScreen.bottom = rclSrc.top+cy;
           
            rclClipTmpBuffer.left   = 0;
            rclClipTmpBuffer.right  = lRclWidth(&rclSrc);
            rclClipTmpBuffer.top    = 0;
            rclClipTmpBuffer.bottom = cy;

            // loop once for each block ( height/(cy+1) ) but last block
            // may not occupy full tempbuffer 
            for ( lRow=(lRclHeight(&rclSrc)+cy-1)/cy; lRow>0; lRow-- )
            {
                // copy backbuffercontents to tempbuffer
                ppdev->pfnSetSourceBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);
                ppdev->pfnSetDestBase(ppdev, ulTempBufferOffset, lTempBufferStride);
                ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt
                    (ppdev, 1, &rclClipTmpBuffer, 0xcccc, &ptlSrcScreen, &rclClipTmpBuffer, NULL);

                // copy frontbuffercontents to primary
                ppdev->pfnSetSourceBase(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT], ppdev->lDelta);
                ppdev->pfnSetDestBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);
                ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt
                    (ppdev, 1, &rclClipScreen, 0xcccc, &ptlSrcScreen, &rclClipScreen, NULL);

                //flip to primary if offscreen bitmap was big enough for complete clientarea
                if( bCopyAtOnce )
                    NvRestoreDacs(ppdev);

                // copy tempbuffercontents to backbuffer
                ppdev->pfnSetSourceBase(ppdev, ulTempBufferOffset, lTempBufferStride);
                ppdev->pfnSetDestBase(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT], ppdev->lDelta);
                ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt
                    (ppdev, 1, &rclClipScreen, 0xcccc, &ptlSrcTmpBuffer, &rclClipScreen, NULL);

                ptlSrcScreen.y       += cy;
                rclClipScreen.top    += cy;
                rclClipScreen.bottom += cy;                

                // next row is last row which may be smaller than others
                if(  (2==lRow)
                   &&(rclClipScreen.bottom > rclSrc.bottom)
                  )
                {
                    rclClipTmpBuffer.bottom = cy - (rclClipScreen.bottom - rclSrc.bottom);
                    rclClipScreen.bottom    = rclSrc.bottom;
                }
            }

            ppdev->pfnWaitEngineBusy(ppdev);

            if(bAllocatedTempBufferOnHeap)
                NVHEAP_FREE(ulTempBufferOffset);

            bBufferExchanged = TRUE;

            // flip to primary here if we could not copy the clientarea at once
            if (!bCopyAtOnce)
                NvRestoreDacs(ppdev);
        }

    }

    return bBufferExchanged;
}


//*************************************************************************
// vOglFlipChangedUpdate
// 
// Run through client list and set notification that primary flipped. 
// This is done for all clients except the given one.
// The exclusion can be used to update all clients that this one has 
// flipped.
//*************************************************************************
VOID vOglFlipChangedUpdate(
    IN PPDEV                  ppdev, 
    IN NV_OPENGL_CLIENT_INFO *clientInfo) // exclude this client from update, can be NULL
{
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
    {
        if (   (NULL != node->clientInfo.clientDrawableInfo)
            && (&node->clientInfo != clientInfo) )
        {
            ASSERT(node->clientInfo.flags & NV_OGL_CLIENT_DRAWABLE_INFO);
            // flipping address has changed, notify ICD
            node->clientInfo.clientDrawableInfo->flipChangedCount++;
            // the client stopped with flipping now
            node->clientInfo.clientDrawableInfo->bFlipping = FALSE;
            break;
        }
        node = node->next;
    }
}


/*
**  vFlipToPrimary
**
**  Swaps the contents of fullscreen front and backbuffer
**  if there are any visible opengl windows and sets the 
**  dac to the primary
**
**  10/17/2000 MSchwarzer: new
**  01/16/2001           : function does a fullscreen swap now
*/
VOID vFlipToPrimary(PPDEV ppdev)
{
    BOOL bFlippedToPrimary = FALSE;

    ASSERT(ppdev);
    ASSERT(OGL_FLIPPED());

    DISPDBG((3, "vFlipToPrimary"));
    
    OglSyncEngineUsingPDEV(ppdev);

    if(  ( ppdev->bOglSingleBackDepthSupported )
       &&( ppdev->bOglSingleBackDepthCreated )
      )
    {
        ULONG  ulTempBufferOffset         = 0;
        ULONG  ulTempBufferSize           = 0;
        BOOL   bCopyAtOnce                = FALSE;
        BOOL   bAllocatedTempBufferOnHeap = FALSE;
        RECTL  rclClipScreen;
        RECTL  rclClipTmpBuffer;
        POINTL ptlSrcScreen;
        POINTL ptlSrcTmpBuffer;
        LONG   lRow;
        LONG   cy;
        DWORD  dwStatus;

        DISPDBG((3, "vFlipToPrimary: exchange buffers"));

        // try to get a full screensize temporary buffer
        ulTempBufferSize  = ppdev->lDelta * ppdev->cyScreen;

        if( ulTempBufferSize > ppdev->ulTempOffscreenBufferSize )
        {
            NVHEAP_ALLOC(dwStatus, ulTempBufferOffset,  ulTempBufferSize, TYPE_IMAGE)
            if( 0 == dwStatus ) // success
            {
                bAllocatedTempBufferOnHeap = TRUE;
                bCopyAtOnce                = TRUE;
            }
        }

        if(!bAllocatedTempBufferOnHeap)
        {
            // not enough room in offscreen to copy at once => copy line by line
            ASSERT(0 != ppdev->ulTempOffscreenBufferSize);
            if( 0 == ppdev->ulTempOffscreenBufferSize )
                goto Exit;

            if( ulTempBufferSize <= ppdev->ulTempOffscreenBufferSize )
                bCopyAtOnce = TRUE;

            ulTempBufferSize   = ppdev->ulTempOffscreenBufferSize;
            ulTempBufferOffset = ppdev->ulTempOffscreenBufferOffset;
        }

        // constant values
        ptlSrcTmpBuffer.x = 0;
        ptlSrcTmpBuffer.y = 0;

        // we may have to copy the clientrect via several blockcopies
        cy    = ulTempBufferSize / ppdev->lDelta;

        ptlSrcScreen.x = 0;
        ptlSrcScreen.y = 0;

        rclClipScreen.left   = 0;
        rclClipScreen.right  = ppdev->cxScreen;
        rclClipScreen.top    = 0;
        rclClipScreen.bottom = cy;
       
        rclClipTmpBuffer     = rclClipScreen;

        // loop once for each block ( height/(cy+1) ) but last block
        // may not occupy full tempbuffer 
        for ( lRow=(ppdev->cyScreen+cy-1)/cy; lRow>0; lRow-- )
        {
            // copy backbuffercontents to tempbuffer
            ppdev->pfnSetSourceBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);
            ppdev->pfnSetDestBase(ppdev, ulTempBufferOffset, ppdev->lDelta);
            ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt
                (ppdev, 1, &rclClipTmpBuffer, 0xcccc, &ptlSrcScreen, &rclClipTmpBuffer, NULL);

            // copy frontbuffercontents to primary
            ppdev->pfnSetSourceBase(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT], ppdev->lDelta);
            ppdev->pfnSetDestBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);
            ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt
                (ppdev, 1, &rclClipScreen, 0xcccc, &ptlSrcScreen, &rclClipScreen, NULL);

            //flip to primary if offscreen bitmap was big enough for complete clientarea
            if( bCopyAtOnce )
            {
                NvRestoreDacs(ppdev);
                bFlippedToPrimary = TRUE;
            }

            // copy tempbuffercontents to backbuffer
            ppdev->pfnSetSourceBase(ppdev, ulTempBufferOffset, ppdev->lDelta);
            ppdev->pfnSetDestBase(ppdev, ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT], ppdev->lDelta);
            ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt
                (ppdev, 1, &rclClipScreen, 0xcccc, &ptlSrcTmpBuffer, &rclClipScreen, NULL);

            ptlSrcScreen.y       += cy;
            rclClipScreen.top    += cy;
            rclClipScreen.bottom += cy;                

            // next row is last row which may be smaller than others
            if(  (2==lRow)
               &&(rclClipScreen.bottom > ppdev->cyScreen)
              )
            {
                rclClipTmpBuffer.bottom = cy - (rclClipScreen.bottom - ppdev->cyScreen);
                rclClipScreen.bottom    = ppdev->cyScreen;
            }
        }

        ppdev->pfnWaitEngineBusy(ppdev);

        if(bAllocatedTempBufferOnHeap)
            NVHEAP_FREE(ulTempBufferOffset);
    }

Exit:

    if( !bFlippedToPrimary )
    {
        // Restore if necessary!
        NvRestoreDacs(ppdev);
    }

    vOglFlipChangedUpdate(ppdev, NULL);

    ASSERT(!OGL_FLIPPED());
}

