/******************************Module*Header*******************************\
* Module Name: oglswap.c
*
* This module contains the functions to support the OpenGL swap buffers.
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
#include "oglOverlay.h"
#include "oglsync.h"
#include "oglflip.h"
#include "Nvcm.h"   // Needed for NV_CFG_VBLANK_COUNTER,

#if (NVARCH >= 0x4)
#include "nvcom.h"          // drivers/common/inc
#include "nvtracecom.h"     // drivers/common/inc
#endif // (NVARCH >= 0x4)


#define DBG_SWAP_MESSAGES 1
#undef DBG_SWAP_MESSAGES


#define __glNV4SetupSwap(a, b, c, d, e, f)

//******************************************************************************
//
//  Function: OglUseDisplayDriverBlt
//
//  Routine Description:
//
//        Utility routine to use display driver channel for a BLIT.
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************
void
OglUseDisplayDriverBlt(
    PDEV *ppdev, 
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo,
    LONG srcOffset,
    LONG srcPitch,
    LONG srcLeft, 
    LONG srcTop, 
    LONG dstOffset,
    LONG dstPitch,
    LONG dstLeft, 
    LONG dstTop, 
    LONG width, 
    LONG height
)
    {
    if (srcLeft != dstLeft || srcTop != dstTop)
        {
        LONG dstRight, dstBottom;
        RECTL  rclDst;
        POINTL ptlSrc;

        ppdev->NVFreeCount = 0;

        //**************************************************************
        // Clip to display... (pixels outside of display size are undefined)
        //**************************************************************
        if ((long)dstLeft < (long)0)
            {
            srcLeft -= dstLeft;
            width += dstLeft;
            dstLeft = 0;
            }
        if ((long)dstTop < (long)0)
            {
            srcTop -= dstTop;
            height += dstTop;
            dstTop = 0;
            }

        if ((long)width > 0 && (long)height > 0)
            {
            LONG lDeltaX, lDeltaY;

            dstRight = dstLeft + width;
            dstBottom = dstTop + height;

            if ((long)srcLeft < (long)0)
                {
                ptlSrc.x = 0;
                rclDst.left = -srcLeft + dstLeft;
                }
            else
                {
                ptlSrc.x = srcLeft;
                rclDst.left = dstLeft;
                }
            if ((long)srcTop < (long)0)
                {
                ptlSrc.y = 0;
                rclDst.top = -srcTop + dstTop;
                }
            else
                {
                ptlSrc.y = srcTop;
                rclDst.top = dstTop;
                }

            // right and bottom of source must be inside screen too,
            // because we cannot blit from outside of screen!
            lDeltaX = srcLeft + width - clientDrawableInfo->cxScreen;
            if (lDeltaX > 0)
                {
                width    -= lDeltaX;
                dstRight -= lDeltaX;
                }

            lDeltaY = srcTop + height - clientDrawableInfo->cyScreen;
            if (lDeltaY > 0)
                {
                height    -= lDeltaY;
                dstBottom -= lDeltaY;
                }

            if ((long)dstRight > (long)clientDrawableInfo->cxScreen)
                {
                rclDst.right = clientDrawableInfo->cxScreen;
                }
            else
                {
                rclDst.right = dstRight;
                }

            if ((long)dstBottom > (long)clientDrawableInfo->cyScreen)
                {
                rclDst.bottom = clientDrawableInfo->cyScreen;
                }
            else
                {
                rclDst.bottom = dstBottom;
                }

            if ((long)rclDst.right >= (long)rclDst.left &&
                (long)rclDst.bottom >= (long)rclDst.top)
                {

                ppdev->pfnAcquireOglMutex(ppdev);

                // do complete sync on all ICD channels
                OglSyncEngineUsingPDEV(ppdev);

                (ppdev->pfnSetSourceBase)(ppdev, srcOffset, srcPitch);
                (ppdev->pfnSetDestBase)(ppdev, dstOffset, dstPitch);
                (ppdev->pfnCopyBlt)(ppdev, 1, &rclDst, 0xcccc, &ptlSrc, &rclDst, NULL);

                ppdev->pfnWaitEngineBusy(ppdev);

                ppdev->pfnReleaseOglMutex(ppdev);
                }
            }

        }
    }

// This function builds the intersection of a cliplist with a SwapHintRect list.
// INPUT:   pSwapHintRects points to the list that was passed in by the escape
//          numSwapHintRects is the number of rects in pSwapHintRects
//          windowRectList is the translated cliplist from the clientDrawableInfo
//          numWindowRects is the number of cliprects in windowRectList
//          pSwapRects points to the resulting list of swap rectangles
// OUTPUT:  pSwapRects contains NULL if there are no SwapHintRects in the escape data,
//              otherwise it contains a pointer to the newly allocated list of swap rectangles
//          the return value is the number of resulting swap rectangles

ULONG OglApplySwapHintRects(
    __GLregionRect *pSwapHintRects, 
    ULONG numSwapHintRects, 
    __GLregionRect *windowRectList, 
    ULONG numWindowRects, 
    __GLregionRect *pSwapRects
)
    {
    ULONG ulNumResultRects = 0;

    if ((numSwapHintRects > 0) && (NULL != pSwapHintRects)) 
        {
        __GLregionRect *pSwapRect;
        __GLregionRect *pClipRect;
        __GLregionRect *pResultRect;
        int numSwapRects, numClipRects;

        numSwapRects = numSwapHintRects;

        if (NULL != pSwapRects) 
            {
            // clip each clip rect against each swaphint rect to get the resulting list of swap rectangles 
            pSwapRect = pSwapHintRects;
            ASSERT(NULL != pSwapRect); // if the counter is > 0, there must be a rectangle
            while (numSwapRects > 0) 
                {
                pClipRect       = windowRectList;
                numClipRects    = numWindowRects;
                while (numClipRects > 0) {
                    // Test on intersection                    
                    if (   (pSwapRect->x0 < pClipRect->x1) 
                        && (pSwapRect->x1 > pClipRect->x0)  
                        && (pSwapRect->y0 < pClipRect->y1) 
                        && (pSwapRect->y1 > pClipRect->y0) )
                        {
                        // copy intersection rectangle into resulting swaprect list
                        pResultRect = &pSwapRects[ulNumResultRects];
                        pResultRect->x0 = max(pSwapRect->x0, pClipRect->x0);
                        pResultRect->x1 = min(pSwapRect->x1, pClipRect->x1);
                        pResultRect->y0 = max(pSwapRect->y0, pClipRect->y0);
                        pResultRect->y1 = min(pSwapRect->y1, pClipRect->y1);
                        ulNumResultRects++;
                        }
                    pClipRect++;
                    numClipRects--;
                    }
                pSwapRect++;
                numSwapRects--;
                }    
            }
        }

    return ulNumResultRects;
    }


//******************************************************************************
//
//  Function: NV4_OglSwapBuffers
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************
ULONG
NV4_OglSwapBuffers(
    PDEV *ppdev,
    HWND hWnd,
    HDC hDC,
    ULONG hClient,
    __GLNVswapInfo *swapInfo
)
{
#if (NVARCH >= 0x4)
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo = NULL;
    __GLNVcmdInfo              *cmdInfo;
    LONG                       status;
    RECTL                      *windowRectList;
    ULONG                      numWindowRects;
    ULONG                      swapHintUsed;

    if (!swapInfo ||
        ppdev->dwGlobalModeSwitchCount != swapInfo->localModeSwitchCount)
        {
        OglDebugPrint("NV4_OglSwapBuffers: bailing OGL_ERROR_MODE_SWITCH\n");
        swapInfo->retStatus = OGL_ERROR_MODE_SWITCH;
        return((ULONG)FALSE);
        }

    clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }
    else
        {
        OglDebugPrint("NV4_OglSwapBuffers: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        swapInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
        }

    if (clientInfo)
        {
        clientDrawableInfo = clientInfo->clientDrawableInfo;
        }
    else
        {
        OglDebugPrint("NV4_OglSwapBuffers: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        swapInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
        }

    try
    {
        cmdInfo = swapInfo->cmdInfo;
    }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        swapInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        DISPDBG((1, "NV4_OglSwapBuffers: cmdInfo: exception = 0x%lx", status));
        return((ULONG)FALSE);
    }    
    
    // If the client side clipChangedCount is out of date the window may
    // have moved or resized.  Kick back to the client so buffer info may
    // be reloaded.
    if ((LONG)swapInfo->clipChangedCount != (LONG)clientDrawableInfo->clipChangedCount)
        {
        swapInfo->retStatus = OGL_ERROR_WINDOW_CHANGED_COUNT;
        return((ULONG)FALSE); 
        }

#ifdef DBG
    nvDebugLevel   = cmdInfo->nvDebugLevel;
    nvDebugOptions = cmdInfo->nvDebugOptions;
    nvDebugMask    = cmdInfo->nvDebugMask;
    nvControlOptions = cmdInfo->nvControlOptions;
#endif
    
    // If the window is not visible, stop
    if (clientDrawableInfo->clipFlags == NV_WINDOW_CLIP_OCCLUDED ||
        clientDrawableInfo->numClipRects == 0)
        {
        swapInfo->retStatus = OGL_ERROR_NO_ERROR;
        return TRUE;
        }

    if (FALSE == NV_OglUpdateClipLists(ppdev, clientDrawableInfo)) {
        swapInfo->retStatus = OGL_ERROR_REALLOC_FAILURE;
        return FALSE;
    }

    //
    // If retry counter is set, try to enable pageflipping again
    //
    bOglPageFlipModeRetryEnable(ppdev);

    windowRectList = clientDrawableInfo->cachedClipRectList;
    numWindowRects = clientDrawableInfo->cachedNumClipRects;

    cmdInfo->ntOther      = (void *)ppdev;
    cmdInfo->ntDrawable   = (void *)clientDrawableInfo;
    cmdInfo->ntClientInfo = clientInfo;
    ASSERT(bOglHasDrawableInfo(cmdInfo->ntClientInfo));

    // RBiermann: apply SwapHintRects on the windowRectList if there is a list in the __GLNVswapInfo
    swapHintUsed = FALSE;
    if (swapInfo->numSwapHintRects > 0)
        {
        __GLregionRect *pSwapRectList = NULL; // ptr to the swaprect list after applying the SwapHintRects extension
        ULONG          numSwapRects = 0;      // number of swaprect after applying the SwapHintRects extension

        // I have to allocate the size of the maximal resulting list because there is no EngReallocMem.
        // But this is not too bad because numSwapHintRects is always 1 in 3DSMax.
        pSwapRectList = EngAllocMem(  FL_ZERO_MEMORY, 
                                      swapInfo->numSwapHintRects * numWindowRects * sizeof(__GLregionRect),
                                      ALLOC_TAG);

        ASSERT(pSwapRectList);
        if (pSwapRectList != NULL)
            {
            swapHintUsed = TRUE;

            numSwapRects = OglApplySwapHintRects(   swapInfo->pSwapHintRects,
                                                    swapInfo->numSwapHintRects, 
                                                    (__GLregionRect *)windowRectList, 
                                                    numWindowRects, 
                                                    pSwapRectList);
            if (numSwapRects > 0)
                {
                // Call into the shared library to do the swap.
                __glNV4SetupSwap(cmdInfo, (void *) swapInfo->drawable, 
                                 numSwapRects,
                                 pSwapRectList,
                                 (int) clientDrawableInfo->cxScreen,
                                 (int) clientDrawableInfo->cyScreen);

                }

            EngFreeMem(pSwapRectList);
            }
        }


    if (FALSE == swapHintUsed)
        {
        // Call into the shared library to do the swap.
        __glNV4SetupSwap(cmdInfo, (void *) swapInfo->drawable, 
                         numWindowRects,
                         (__GLregionRect *)windowRectList,
                         (int) clientDrawableInfo->cxScreen,
                         (int) clientDrawableInfo->cyScreen);
        }

    swapInfo->retStatus = OGL_ERROR_NO_ERROR;
#endif // (NVARCH >= 0x4)
    return((ULONG)TRUE);
}

#if (NVARCH >= 0x4)
//******************************************************************************
//
//  Function: NV_OglSwapBuffers
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//            NONE
//
//******************************************************************************
ULONG
NV_OglSwapBuffers(
    PDEV *ppdev,
    HWND hWnd,
    HDC hDC,
    ULONG hClient,
    __GLNVswapInfo *swapInfo
)
{
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo = NULL;
    __GLNVcmdInfo              *cmdInfo;
    LONG                       status;
    RECTL                      *windowRectList;
    ULONG                      numWindowRects;
    ULONG                      swapHintUsed;

    if (!swapInfo ||
        ppdev->dwGlobalModeSwitchCount != swapInfo->localModeSwitchCount)
        {
        OglDebugPrint("NV_OglSwapBuffers: bailing OGL_ERROR_MODE_SWITCH\n");
        swapInfo->retStatus = OGL_ERROR_MODE_SWITCH;
        return((ULONG)FALSE);
        }
    clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }
    else
        {
        OglDebugPrint("NV_OglSwapBuffers: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        swapInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
        }

    if (clientInfo)
        {
        clientDrawableInfo = clientInfo->clientDrawableInfo;
        }
    else
        {
        OglDebugPrint("NV_OglSwapBuffers: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        swapInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
        }

    try
    {
        cmdInfo = swapInfo->cmdInfo;
    }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        swapInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        DISPDBG((1, "NV_OglSwapBuffers: cmdInfo: exception = 0x%lx", status));
        return((ULONG)FALSE);
    }    

    // If the client side clipChangedCount is out of date the window may
    // have moved or resized.  Kick back to the client so buffer info may
    // be reloaded.
    if ((LONG)swapInfo->clipChangedCount != (LONG)clientDrawableInfo->clipChangedCount)
        {
        swapInfo->retStatus = OGL_ERROR_WINDOW_CHANGED_COUNT;
        return((ULONG)FALSE); 
        }

#ifdef DBG
    nvDebugLevel   = cmdInfo->nvDebugLevel;
    nvDebugOptions = cmdInfo->nvDebugOptions;
    nvDebugMask    = cmdInfo->nvDebugMask;
    nvControlOptions = cmdInfo->nvControlOptions;
#endif
    
    // If the window is not visible, stop
    if (clientDrawableInfo->clipFlags == NV_WINDOW_CLIP_OCCLUDED ||
        clientDrawableInfo->numClipRects == 0)
        {
        swapInfo->retStatus = OGL_ERROR_NO_ERROR;
        return TRUE;
        }

    if (FALSE == NV_OglUpdateClipLists(ppdev, clientDrawableInfo)) {
        swapInfo->retStatus = OGL_ERROR_REALLOC_FAILURE;
        return FALSE;
    }

    //
    // If retry counter is set, try to enable pageflipping again
    //
    bOglPageFlipModeRetryEnable(ppdev);

    windowRectList = clientDrawableInfo->cachedClipRectList;
    numWindowRects = clientDrawableInfo->cachedNumClipRects;

    cmdInfo->ntOther      = (void *)ppdev;
    cmdInfo->ntDrawable   = (void *)clientDrawableInfo;
    cmdInfo->ntClientInfo = clientInfo;
    ASSERT(bOglHasDrawableInfo(cmdInfo->ntClientInfo));

    // In a stereo environment enable broadcasting to
    // right buffer if stereo is active. 
    // NOTE: this is also done for monoscopic apps, but we need the right 
    //       buffers to be initialized
    if (ppdev->bOglStereoDesired)
    {
        __glNVPrepareStereoSwap((void *)swapInfo->drawable, ppdev->bOglStereoActive);
    }

    // RBiermann: apply SwapHintRects on the windowRectList if there is a list in the __GLNVswapInfo
    swapHintUsed = FALSE;
    if (swapInfo->numSwapHintRects > 0)
        {
        __GLregionRect *pSwapRectList = NULL; // ptr to the swaprect list after applying the SwapHintRects extension
        ULONG          numSwapRects = 0;      // number of swaprect after applying the SwapHintRects extension

        // I have to allocate the size of the maximal resulting list because there is no EngReallocMem.
        // But this is not too bad because numSwapHintRects is always 1 in 3DSMax.
        pSwapRectList = EngAllocMem(  FL_ZERO_MEMORY, 
                                      swapInfo->numSwapHintRects * numWindowRects * sizeof(__GLregionRect),
                                      ALLOC_TAG);

        ASSERT(pSwapRectList);
        if (pSwapRectList != NULL)
            {
            swapHintUsed = TRUE;

            numSwapRects = OglApplySwapHintRects(   swapInfo->pSwapHintRects,
                                                    swapInfo->numSwapHintRects, 
                                                    (__GLregionRect *)windowRectList, 
                                                    numWindowRects, 
                                                    pSwapRectList);
            if (numSwapRects > 0)
                {
                // Call into the shared library to do the swap.
                __glNVSetupSwap(cmdInfo, (void *) swapInfo->drawable, 
                                numSwapRects,
                                pSwapRectList,
                                (int) clientDrawableInfo->cxScreen,
                                (int) clientDrawableInfo->cyScreen);

                }

            EngFreeMem(pSwapRectList);
            }
        }


    if (FALSE == swapHintUsed)
        {
        // Call into the shared library to do the swap.
        __glNVSetupSwap(cmdInfo, (void *) swapInfo->drawable, 
                        numWindowRects,
                        (__GLregionRect *)windowRectList,
                        (int) clientDrawableInfo->cxScreen,
                        (int) clientDrawableInfo->cyScreen);
        }

    swapInfo->retStatus = OGL_ERROR_NO_ERROR;
    return (TRUE);
}


/*
** NV_OglOverlayMergeBlit
** 
** Derived from NV_OglSwapBuffers.
**
** Display driver version of overlay merge blit implementation. 
** Needs to feed the surface offsets to the blit.
** Needs to feed correct clip information.
** 
** return:
**    TRUE  - succeeded
**    FALSE - failed to do the merge blit
** 
** FNicklisch 26.09.2000: New
*/ 
ULONG NV_OglOverlayMergeBlit(
    PDEV                       *ppdev,
    HWND                        hWnd,
    HDC                         hDC,
    __GLNVoverlayMergeBlitInfo *pMergeInfo
)
{
    NV_OPENGL_CLIENT_INFO      *clientInfo         = NULL;  // Client data...
    NV_OPENGL_CLIENT_INFO_LIST *clientList         = NULL;
    NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo = NULL;
    __GLNVcmdInfo              *cmdInfo;                    // Common commandinfo passed to merge blit
    RECTL                      *prclWindowClipList;         // local clip list
    ULONG                       numWindowRects;             // number of valid rects inside prclWindowClipList
    __GLMergeBlitData           MergeBlitData;              // Data structure passed to merge blit
    NV_LAYER_PALETTE           *pLayerPalette;              // overlay palette used for this window
    ULONG                       ulRet              = FALSE; // default error Exit
    unsigned int                dwAction           = 0;     // Local mergeblit configuration bits
    ULONG                       ulTemp;

    ASSERT(NULL!=ppdev);
    ASSERT(NULL!=pMergeInfo);

    //
    // check if overlay is possible
    //
    if (!ppdev->bOglOverlaySurfacesCreated)
    {
        // This happens during startup of an overlayed ogl app!
        pMergeInfo->retStatus = OGL_ERROR_SURFACE_INVALID;
        goto Exit;
    }

    try
    {
        cmdInfo = pMergeInfo->cmdInfo;
    }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        ASSERTDD(0,"cannot access pMergeInfo, please fix that!");
        DISPDBG((1, "NV_OglOverlayMergeBlit: pMergeInfo: exception = 0x%lx", GetExceptionCode()));
        pMergeInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        goto Exit;
    }    
    
    clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
    ASSERT(NULL!=clientList);
    if (clientList)
    {
        clientInfo = &clientList->clientInfo;
        ASSERT(NULL!=clientInfo);
    }
    else
    {
        pMergeInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        goto Exit;
    }

    if (clientInfo)
    {
        clientDrawableInfo = clientInfo->clientDrawableInfo;
        ASSERT(NULL!=clientDrawableInfo);
    }
    else
    {
        pMergeInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        goto Exit;
    }

    // If the window is not visible, stop
    if (clientDrawableInfo->clipFlags == NV_WINDOW_CLIP_OCCLUDED ||
        clientDrawableInfo->numClipRects == 0)
    {
        pMergeInfo->retStatus = OGL_ERROR_NO_ERROR;
        ulRet = TRUE;
        goto Exit;
    }

    prclWindowClipList =
        EngAllocMem(FL_ZERO_MEMORY, 
                    (sizeof(RECTL) * clientDrawableInfo->numClipRects), ALLOC_TAG);

    if (NULL==prclWindowClipList)
    {
        OglDebugPrint("NV_OglOverlayMergeBlit: EngAllocMem failed for prclWindowClipList, bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        pMergeInfo->retStatus = OGL_ERROR_CLIP_LIST_BAD;
        goto Exit;
    }


    //
    // FNicklisch: ToDo later...
    // Can spend some time on rewritting the clip list code below.
    // It isn't neccessary to convert to client relative and then back to 
    // desktop relative.
    // 
    
    //
    // Client relative part
    //

    OglTranslateCachedClipRectList(ppdev,
                                   clientDrawableInfo->copyWindowRect,
                                   clientDrawableInfo->numClipRects,
                                   prclWindowClipList,
                                   &numWindowRects,
                                   (RECTL *) &(clientDrawableInfo->rect));
    ASSERT(numWindowRects==clientDrawableInfo->numClipRects);

    // RBiermann: apply SwapHintRects on the windowRectList if there is a list
    if (pMergeInfo->numSwapHintRects > 0)
        {
        __GLregionRect *pSwapRectList = NULL; // ptr to the swaprect list after applying the SwapHintRects extension
        ULONG           numSwapRects  = 0;    // number of swaprect after applying the SwapHintRects extension

        // I have to allocate the size of the maximal resulting list because there is no EngReallocMem.
        // But this is not too bad because numSwapHintRects is always 1 in 3DSMax.
        pSwapRectList = EngAllocMem(  FL_ZERO_MEMORY, 
                                      pMergeInfo->numSwapHintRects * numWindowRects * sizeof(__GLregionRect),
                                      ALLOC_TAG);

        ASSERT(pSwapRectList);
        if (NULL != pSwapRectList)
            {
            ASSERT(NULL != prclWindowClipList);
            ASSERT(numWindowRects>0);

            numSwapRects = OglApplySwapHintRects(pMergeInfo->pSwapHintRects,
                                                 pMergeInfo->numSwapHintRects, 
                                                 (__GLregionRect *)prclWindowClipList, 
                                                 numWindowRects, 
                                                 pSwapRectList);

            // use the swaphint list as current clip list and release the old one
            EngFreeMem(prclWindowClipList);

            prclWindowClipList = (RECTL *) pSwapRectList;
            numWindowRects     = numSwapRects;
            }
        }


    //
    // Use desktop relative coordinates here. In place translation!
    //
    OglTranslateClientRelativeRectListToDevice(ppdev,
                                               prclWindowClipList, numWindowRects,
                                               prclWindowClipList, &ulTemp,
                                               (RECTL *) &(clientDrawableInfo->rect));
    ASSERT(numWindowRects==ulTemp);

    MergeBlitData.prclClip           = (__GLregionRect *)prclWindowClipList;
    MergeBlitData.cClip              = numWindowRects;

    //
    // Setup offset data 
    //
    MergeBlitData.PrimaryFrontOffset = ppdev->ulPrimarySurfaceOffset;
    MergeBlitData.PrimaryFrontPitch  = ppdev->lDelta;
    MergeBlitData.ScratchOffset      = ppdev->singleBack1Offset[OGL_STEREO_BUFFER_LEFT];
    MergeBlitData.ScratchPitch       = ppdev->singleBack1Pitch[OGL_STEREO_BUFFER_LEFT];

    MergeBlitData.MainFrontOffset    = ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT];
    MergeBlitData.MainFrontPitch     = ppdev->singleFrontMainPitch[OGL_STEREO_BUFFER_LEFT];
    MergeBlitData.MainBackOffset     = ppdev->singleBack1MainOffset[OGL_STEREO_BUFFER_LEFT];
    MergeBlitData.MainBackPitch      = ppdev->singleBack1MainPitch[OGL_STEREO_BUFFER_LEFT];

    MergeBlitData.OverlayFrontOffset = ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT];
    MergeBlitData.OverlayFrontPitch  = ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_LEFT];
    MergeBlitData.OverlayBackOffset  = ppdev->singleBack1OverlayOffset[OGL_STEREO_BUFFER_LEFT]; 
    MergeBlitData.OverlayBackPitch   = ppdev->singleBack1OverlayPitch[OGL_STEREO_BUFFER_LEFT]; 


    if (!bOglFindGlobalLayerPaletteHWND(hWnd, &pLayerPalette))
    {
        // 
        // No palette does mean no layer plane!
        // FNicklisch: This is kind of a hack! Should find a better way to check if a window is an overlay window.
        //
        //isn't so critical...OglDebugPrint("NV_OglOverlayMergeBlit: couldn't find layer palette!\n");

        dwAction               = NV_OVERLAY_NO_OVERLAY_PLANE;
        MergeBlitData.colorref = ICD_PALETTE_TRANSPARENT_COLOR;
    }
    else
    {
        ASSERT(NULL!=pLayerPalette);
        MergeBlitData.colorref = pLayerPalette->entries[pLayerPalette->crTransparent];
    }

    ASSERT(2==ppdev->cjPelSize || 4==ppdev->cjPelSize);

    MergeBlitData.colordepth         = ppdev->cjPelSize * 8; // 16 or 32!
    MergeBlitData.dwAction           = pMergeInfo->dwAction | dwAction;

    // Only know these bits
    ASSERT(0==(MergeBlitData.dwAction & ~(  NV_OVERLAY_MERGE_BLIT
                                          | NV_OVERLAY_UPDATE_OVERLAY
                                          | NV_OVERLAY_UPDATE_MAIN
                                          | NV_OVERLAY_USE_CLIPRECT
                                          | NV_OVERLAY_DEFERRED
                                          | NV_OVERLAY_NO_OVERLAY_PLANE
                                          | NV_OVERLAY_RUN_IN_DD
                                          )));

    //
    // If we use the user clipping, transfer it to desktop relative space
    //
    if (MergeBlitData.dwAction & NV_OVERLAY_USE_CLIPRECT)
    {
        // NOTE: User cliping in pMergeInfo is client relative!
        MergeBlitData.rclUser.x0 = pMergeInfo->x;
        MergeBlitData.rclUser.y0 = pMergeInfo->y;
        MergeBlitData.rclUser.x1 = pMergeInfo->x + pMergeInfo->width;
        MergeBlitData.rclUser.y1 = pMergeInfo->y + pMergeInfo->height;

        OglTranslateClientRelativeRectListToDevice(ppdev,
                                                   (RECTL *)&MergeBlitData.rclUser, 1,
                                                   (RECTL *)&MergeBlitData.rclUser, &ulTemp,
                                                   (RECTL *)&(clientDrawableInfo->rect));
        ASSERT(1==ulTemp);
    }

    if (   (NULL==cmdInfo)                                   // this happens if're called from inside display driver
        || (MergeBlitData.dwAction & NV_OVERLAY_RUN_IN_DD) ) // this happens if we want to run in the display driver channel
    {
        // this is a merge blit which has to happen in the display driver!
        // DD channel

        bOglSwapMergeBlit(ppdev,
                          &MergeBlitData);
    }
    else
    {
        // Call into the shared library to do the merge blit.
        // ICD channel
        cmdInfo->ntClientInfo = clientInfo;
        ASSERT(bOglHasDrawableInfo(cmdInfo->ntClientInfo));

        __glNVSetupMergeBlit(cmdInfo,
                             &MergeBlitData);
    }

    //
    // This sync here fixes most of the Softimage problems (preview, texture selectio box...)
    //
    ppdev->pfnAcquireOglMutex(ppdev);
    ppdev->pfnWaitForChannelSwitch(ppdev);
    ppdev->pfnWaitEngineBusy(ppdev);
    ppdev->pfnReleaseOglMutex(ppdev);

    EngFreeMem(prclWindowClipList);
    pMergeInfo->retStatus = OGL_ERROR_NO_ERROR;

    ulRet = TRUE;
Exit:
    return (ulRet);
}

#endif // (NVARCH >= 0x4)
