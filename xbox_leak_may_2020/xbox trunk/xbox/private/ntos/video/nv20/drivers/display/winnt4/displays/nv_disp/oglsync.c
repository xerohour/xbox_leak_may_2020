//******************************Module*Header***********************************
// Module Name: oglsync.c
//
// syncing routines GDI against ogl, and ogl against ogl
//
// Copyright (c) 2001, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

#include "precomp.h"
#include "driver.h"
#include "RectUtils.h"
#include "nvReg.h"
#include "nvcm.h"
#include "oglDD.h"
#include "oglclip.h"
#include "oglflip.h"
#include "oglutils.h"

#include "oglsync.h"

/*
** Problem GDI sync:
** Only want to sync if two channels, here GDI and OpenGL render to the same 
** window (rectangle). As we have cliplists for all OpenGL clients, we are able 
** to track if GDI is going to render to an OpenGL client window. If so we sync
** on the GDI entry and set a flag to sync again on the next OpenGL entry.
**
** Affected functions (all GDI driver entries that render) and code used to 
** quickly determine rclBounds:
**
** + DrvBitBlit (1)
**      uses bOglSyncIfGdiInOglWindowBlt
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? *prcl : pco->rclBounds;
**
** + DrvCopyBits (1)
**      uses bOglSyncIfGdiInOglWindowBlt
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? *prcl : pco->rclBounds;
**
** + DrvStretchBlt (1)
**      uses bOglSyncIfGdiInOglWindowStretch
**      // can do inversions of x and y when the destination rectangle is not well ordered.
**      prclRectOrder(&rclDstOrdered, prcl);    // creates an ordered rect
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? rclDstOrdered : pco->rclBounds;
**
** + DrvTextOut            
**      uses bOglSyncIfGdiInOglWindowRect
**      if (prclOpaque) 
**        rclBounds = (DC_TRIVIAL==iDComplexity) ? *prclOpaque : pco->rclBounds;
**      else
**        rclBounds = (DC_TRIVIAL==iDComplexity) ? pstro->rclBkGround : pco->rclBounds;
**
** + DrvLineTo             
**      uses bOglSyncIfGdiInOglWindowRect
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? *prclBounds : pco->rclBounds;
**
** + DrvStrokePath         
**      uses bOglSyncIfGdiInOglWindowPath
**      PATHOBJ_vGetBounds(ppo, &rcfxBounds);
**      rclTrg.left   = rcfxBounds.xLeft>>4;        // round down
**      rclTrg.top    = rcfxBounds.yTop>>4;
**      rclTrg.right  = (rcfxBounds.xRight+15)>>4;  // round up
**      rclTrg.bottom = (rcfxBounds.yBottom+15)>>4;
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? rclTrg : pco->rclBounds;
** 
** + DrvFillPath           
**      uses bOglSyncIfGdiInOglWindowPath
**      PATHOBJ_vGetBounds(ppo, &rcfxBounds);
**      rclTrg.left   = rcfxBounds.xLeft>>4;        // round down
**      rclTrg.top    = rcfxBounds.yTop>>4;
**      rclTrg.right  = (rcfxBounds.xRight+15)>>4;  // round up
**      rclTrg.bottom = (rcfxBounds.yBottom+15)>>4;
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? rclTrg : pco->rclBounds;
** 
** + DrvPaint              
**      uses bOglSyncIfGdiInOglWindowRect
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? rclDstOrdered : pco->rclBounds;
** 
** + DrvAlphaBlend (1)
**      uses bOglSyncIfGdiInOglWindowStretch
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? *prcl : pco->rclBounds;
**
** + DrvTransparentBlt (1)
**      uses bOglSyncIfGdiInOglWindowStretch
**      rclBounds = (DC_TRIVIAL==iDComplexity) ? *prcl : pco->rclBounds;
**
** + DrvSaveScreenBits
**      uses bOglSyncIfGdiInOglWindowRect
**      rclBounds = *prcl;
**
** - DrvMovePointer        
** - DrvSetPointerShape    
**      left it out for now, if through DAC then I don't care, 
**      otherwise it should come in through copybits again
**
** + DrvEscape
**      calls bOglSyncOglIfGdiRenderedToWindow
**      no render rect (OPENGL_CMD escape needs to track old GDI rendering and sync)
**             
** + DrvSwapBuffers        
**      calls bOglSyncOglIfGdiRenderedToWindow
**      needs to track old GDI rendering and sync
**
** (1) prcl is prclSrc or prclTrg, depending on src/dst pso
**
*/


#if DBG
// global variable to disable smart syncing
BOOL bOglSyncGdiVsOglDisabled = FALSE;
BOOL bOglFillSyncedGdi        = FALSE;
BOOL bOglFillSyncedOgl        = FALSE;

#define OGLSYNC_FILL_GDI(ppdev,prcl)                                                     \
    if (bOglFillSyncedGdi && bRet)                                                       \
    {                                                                                    \
        RBRUSH_COLOR rbc = {0};                                                          \
        rbc.iSolidColor = 0xff;                                                          \
        ppdev->pfnSetDestBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);      \
        ppdev->DoubleBuffer.SavedProcs.pfnFillSolid(ppdev, 1, prcl, 0xF0F0, rbc, NULL);  \
        ppdev->pfnWaitEngineBusy(ppdev);                                                 \
    }
#define OGLSYNC_FILL_OGL(ppdev,prcl)                                                     \
    if (bOglFillSyncedOgl && bRet)                                                       \
    {                                                                                    \
        RBRUSH_COLOR rbc = {0};                                                          \
        rbc.iSolidColor = 0xff00;                                                        \
        ppdev->pfnSetDestBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);      \
        ppdev->DoubleBuffer.SavedProcs.pfnFillSolid(ppdev, 1, prcl, 0xF0F0, rbc, NULL);  \
        ppdev->pfnWaitEngineBusy(ppdev);                                                 \
    }
#else
    #define OGLSYNC_FILL_GDI(ppdev,prcl)
    #define OGLSYNC_FILL_OGL(ppdev,prcl)
#endif


/*
 * FUNCTION: OglSyncEngineUsingPDEV
 *
 * DESC: Using the given PDEV, sync/engine-stall GDI and OGL Rendering Channels.
 */
void OglSyncEngineUsingPDEV(PDEV *ppdev)
{
    ASSERT(ppdev->bEnabled);

    ppdev->pfnAcquireOglMutex(ppdev);

    DISPDBG((4, "OglSyncEngineUsingPDEV"));

    // wait for GDI to finish
//    ppdev->pfnWaitEngineBusy(ppdev);

    // force a sync/engine-stall with GDI channel
    ppdev->oglLastChannel = -1;  // FORCE IT TO SYNC...
    ppdev->pfnWaitEngineBusy(ppdev);

    // force a sync/engine-stall with all OGL rendering channels
    ppdev->oglLastChannel = -1;  // FORCE IT TO SYNC...
    ppdev->NVFreeCount = 0;
    ppdev->pfnWaitForChannelSwitch(ppdev);

    ppdev->pfnReleaseOglMutex(ppdev);
}

#ifdef DISABLE_OGLSYNC
#pragma message("  FNicklisch 21.05.2001: WARNING: OglSync against GDI is disabled!")
#else

//*************************************************************************
// bOglSyncOglIfGdiRenderedToWindow
// 
// Syncs all channels if we recognized a gdi command that rendered into the
// OpenGL window. Now we need to sync again, as we need to be sure GDI is 
// finished before we continue with OpenGL.
//*************************************************************************
BOOL bOglSyncOglIfGdiRenderedToWindow(
    IN PPDEV ppdev)
{
    BOOL bRet; 

    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppdev->pOglServer);
    bRet = ppdev->pOglServer->bGdiInOglWindowNeedsSync;

    if (bRet && ppdev->bEnabled)
    {
        OglSyncEngineUsingPDEV(ppdev);

        ppdev->pOglServer->bGdiInOglWindowNeedsSync = FALSE;
    }

    return (bRet);
}


//*************************************************************************
// bOglSyncIfGdiInOglWindowRect
// 
// Syncs all channels if rcl and pco describe a drawing region that is 
// within any visible OpenGL client. Either prclTrg or pco can be NULL but
// not both at the same time! pso can be NULL!
// 
// Return: TRUE  - sync happend, intersection with OpenGL
//         FALSE - nothing donde, no intersection with OpenGL
//*************************************************************************
BOOL bOglSyncIfGdiInOglWindowRect(
    IN PPDEV    ppdev,
    IN SURFOBJ *pso,        // surface belonging to prclBounds
    IN RECTL   *prclTrg,    // destination rectangle of drawing surface
    IN CLIPOBJ *pco)        // clip obj for destination
{
    BOOL     bRet = FALSE;  // default: no sync necessary, GDI outside all OpenGL clients

    NV_OPENGL_CLIENT_INFO   *clientInfo         = NULL;
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo = NULL;

    ASSERT(NULL != ppdev);

#if DBG
    // debug code can disable fix
    if (bOglSyncGdiVsOglDisabled)
        goto Exit;
#endif

    //
    // Need a enabled hardware and only work on Quadro!
    //
    if (!ppdev->bEnabled)
    {
        // need valid ppdev
        goto Exit;
    }

    //
    // KEY: disable fix code for GeForce!
    //      Only QUADRO will handle sync OGL vs GDI!
    //
    if (!HWGFXCAPS_QUADRO_GENERIC(ppdev))
    {
        // need supported board
        goto Exit;
    }

    //
    // Need a valid surface
    //
    if (   (NULL == pso)
        || !IS_SCREEN(pso) )
    {
        // not rendering to front, no need to sync
        // NOTE: GDI can only render to front (whereever it is)!
        goto Exit;
    }

    //
    // Calculate final rclBounds for rendering on primary surface
    //    
    if (NULL != pco)
    {
        if (   (DC_TRIVIAL != pco->iDComplexity)
            || (NULL == prclTrg) )
        {
            prclTrg = &pco->rclBounds;
        }
    }

    ASSERTDD(NULL != prclTrg, "bOglSyncIfGdiInOglWindowRect either needs pco or prclTrg, use it correctly!");

    // fast out if no intersection with all clients bounding rect
    if (!bRclIntersect(NULL, &ppdev->pOglServer->rclAllClientsBounds, prclTrg))
    {
        goto Exit;
    }


    // find first visible ogl client 
    // and initialize clientInfo with pointer to it
    if (!bOglGetNextVisibleClient(&clientInfo))
    {
        // no visible OpenGL client, no need to sync
        goto Exit;
    }

    //
    // Enumerate clients and clip against rclBounds
    //
    do
    {
        ASSERT(NULL != clientInfo); // we enter this loop with a valid client!

        clientDrawableInfo = clientInfo->clientDrawableInfo;
        ASSERT(NULL != clientDrawableInfo);

        //
        // check if bounding rects of client and gdi are intersecting
        //
        if (bRclIntersect(NULL, (RECTL *)&clientDrawableInfo->rect, prclTrg))
        {
            ULONG ulRect;

            //
            // This potentially is a critical GDI call. Verify that it is inside 
            // one of the ogl clipping rects.
            //

            // Loop over clients clip list 
            for (ulRect=0; !bRet && (ulRect < clientDrawableInfo->numClipRects); ulRect++)
            {
                // if we have a proper pco 
                // clip each client rect against all clipobj rects

                if (   (NULL != pco)
                    && (DC_TRIVIAL != pco->iDComplexity) )
                {
                    bRet = bOglClipRectAgainstClipobj(pco, (RECTL *)&clientDrawableInfo->copyWindowRect[ulRect]);
                }
                else
                {
                    bRet = bRclIntersect(NULL, (RECTL *)&clientDrawableInfo->copyWindowRect[ulRect], prclTrg);
                }
                OGLSYNC_FILL_OGL(ppdev,(RECTL *)&clientDrawableInfo->copyWindowRect[ulRect]);
                OGLSYNC_FILL_GDI(ppdev,prclTrg);
            }
        }
    } 
    while (!bRet && bOglGetNextVisibleClient(&clientInfo));

    //
    // Handle the sync if needed
    //
    if (bRet)
    {
        ASSERT(NULL != ppdev->pOglServer);
        DISPDBG((3, "bOglSyncIfGdiInOglWindowRect: need sync"));
        // we now know that GDI is rendering from/to an OpenGL window
        // Sync everything, to get a stable base for GDI
        OglSyncEngineUsingPDEV(ppdev);

        //
        // As double pumping to within a Ogl's backbuffer potentially 
        // destroyes its content, we cannot allow this. Unfortunately
        // our double pumping cannot handle this. Therefore we need to
        // disable pageflipping (and with that double pumping) now.
        //

        if (   ppdev->pOglServer->bWindowInMovement
            || (   !bRclIsEqual(prclTrg, (RECTL *)&clientDrawableInfo->rect) // not client size
                && bRclIsIn (prclTrg, (RECTL *)&clientDrawableInfo->rect) )) // bounds is bigger than client
        {
            // this is a GDI that starts outside the window, although we sync
            // we don't disable page flipping as it possibly is a copy of the entire window!
            DISPDBG((3, "bOglSyncIfGdiInOglWindowRect: GDI bounding ogl: copy on window move?"));
        }
        else
        {
            DISPDBG((3, "bOglSyncIfGdiInOglWindowRect: GDI inside ogl: disable pf"));
            // Because of our double pumping code, we cannot pageflip GDI to OpenGL back!
            if (0 != cOglFlippingClients(ppdev))
            {
                bOglPageFlipModeDisableAll(ppdev);

                // try again later: Take any random number, I use refresh rate for now, but
                // could calculate a number depending on window size vs desktop size
                ppdev->pOglServer->ulPageFlipRetryCounter = PAGEFLIP_RETRY_COUNTER_START;
            }
        }

        ppdev->pOglServer->bGdiInOglWindowNeedsSync = TRUE;
    }

Exit:
    return (bRet);
}


//*************************************************************************
// bOglSyncIfGdiInOglWindowPath
// 
// Syncs all channels if rcl and pco describe a drawing region that is 
// within any visible OpenGL client. pco can be NULL.
// 
// Return: TRUE  - sync happend, intersection with OpenGL
//         FALSE - nothing donde, no intersection with OpenGL
//*************************************************************************
BOOL bOglSyncIfGdiInOglWindowPath(
    IN PPDEV    ppdev,
    IN SURFOBJ *pso,        // surface belonging to object
    IN PATHOBJ *ppo,        // pathobject to be rendered on pso
    IN CLIPOBJ *pco)        // clip obj for destination
{
    BOOL     bRet = FALSE;  // default: no sync necessary, GDI outside all OpenGL clients
    RECTFX   rcfxBounds;
    RECTL    rclTrg;

    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppo);
    ASSERT(NULL != ppdev->pOglServer);

    PATHOBJ_vGetBounds(ppo, &rcfxBounds);
    rclTrg.left   = rcfxBounds.xLeft >> 4;        // round down
    rclTrg.top    = rcfxBounds.yTop  >> 4;
    rclTrg.right  = (rcfxBounds.xRight+15)  >> 4;  // round up
    rclTrg.bottom = (rcfxBounds.yBottom+15) >> 4;

    // fast out if no intersection with all clients bounding rect
    if (bRclIntersect(NULL, &ppdev->pOglServer->rclAllClientsBounds, &rclTrg))
    {
        bRet = bOglSyncIfGdiInOglWindowRect(
            ppdev,
            pso,        // surface belonging to prclBounds
            &rclTrg,    // destination rectangle of drawing surface
            pco);       // clip obj for destination
    }

    return (bRet);
}


//*************************************************************************
// bOglSyncIfGdiInOglWindowBlt
// 
// Simple wrawpper for bOglSyncIfGdiInOglWindowRect to be called from a 
// blt function that uses source point and target rect as blt functions.
//*************************************************************************
BOOL bOglSyncIfGdiInOglWindowBlt(
    IN PPDEV    ppdev,
    IN SURFOBJ *psoDst,     // surface belonging to dst object     
    IN SURFOBJ *psoSrc,     // surface belonging to src object     
    IN CLIPOBJ *pco,        // clip obj for destination        
    IN RECTL   *prclDst,    // destination rectangle
    IN POINTL  *pptlSrc)    // source point
{
    BOOL     bRet = FALSE;  // default: no sync necessary, GDI outside all OpenGL clients

    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppdev->pOglServer);

    // fast out if no intersection with all clients bounding rect
    if (bRclIntersect(NULL, &ppdev->pOglServer->rclAllClientsBounds, prclDst))
    {
        // try destination first
        bRet = bOglSyncIfGdiInOglWindowRect(ppdev, psoDst, prclDst, pco);
    }

    if (   !bRet
        && (NULL != psoSrc)
        && (NULL != pptlSrc) )
    {
        RECTL rclSrc;

        rclSrc.left   = pptlSrc->x;
        rclSrc.right  = pptlSrc->x + lRclWidth(prclDst);
        rclSrc.top    = pptlSrc->y;
        rclSrc.bottom = pptlSrc->y + lRclHeight(prclDst);

        if (bRclIntersect(NULL, &ppdev->pOglServer->rclAllClientsBounds, &rclSrc))
        {
            // Now source, but cannot use destination pco
            bRet = bOglSyncIfGdiInOglWindowRect(ppdev, psoSrc, &rclSrc, NULL);
        }
    }
    ppdev->pOglServer->bWindowInMovement = FALSE;

    return (bRet);
}



//*************************************************************************
// bOglSyncIfGdiInOglWindowStretch
// 
// Simple wrawpper for bOglSyncIfGdiInOglWindowRect to be called from a 
// blt function that uses source and target rects as stretch functions.
//*************************************************************************
BOOL bOglSyncIfGdiInOglWindowStretch(
    IN PPDEV    ppdev,
    IN SURFOBJ *psoDst,     // surface belonging to dst object     
    IN SURFOBJ *psoSrc,     // surface belonging to src object     
    IN CLIPOBJ *pco,        // clip obj for destination        
    IN RECTL   *prclDst,    // destination rectangle
    IN RECTL   *prclSrc)    // source point
{
    BOOL     bRet = FALSE;  // default: no sync necessary, GDI outside all OpenGL clients
    RECTL    rclOrdered;

    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppdev->pOglServer);

    // try destination first
    prclRclOrder(&rclOrdered, prclDst);
    if (bRclIntersect(NULL, &ppdev->pOglServer->rclAllClientsBounds, &rclOrdered))
    {
        bRet = bOglSyncIfGdiInOglWindowRect(ppdev, psoDst, &rclOrdered, pco);
    }

    if (   !bRet
        && (NULL != psoSrc)
        && (NULL != prclSrc) )
    {
        // Now source, but cannot use destination pco
        prclRclOrder(&rclOrdered, prclSrc);
        if (bRclIntersect(NULL, &ppdev->pOglServer->rclAllClientsBounds, &rclOrdered))
        {
            bRet = bOglSyncIfGdiInOglWindowRect(ppdev, psoSrc, &rclOrdered, NULL);
        }
    }

    return (bRet);
}
#endif // DISABLE_OGLSYNC
// End of oglsync.c

