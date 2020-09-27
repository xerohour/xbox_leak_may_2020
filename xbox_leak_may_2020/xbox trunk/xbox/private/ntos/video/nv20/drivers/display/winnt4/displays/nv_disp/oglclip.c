/******************************Module*Header*******************************\
* Module Name: oglclip.c
*
* This module contains the functions to support the OpenGL client side
* window clip list processing.
*
* Copyright (c) 1998 NVidia Corporation
\**************************************************************************/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#include "nv32.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"

#include "oglDD.h"
#include "oglOverlay.h"
#include "oglstereo.h"
#include "oglflip.h"
#include "oglclip.h"
#include "nvdoublebuf.h"

#include "oglutils.h"
#include "ogldebug.h"
#include "cliplist.h"
#include "rectutils.h"
#ifdef NV_MAIN_MULTIMON
#include "linkedlist.h"
#endif

#if (NVARCH >= 0x4)
#include "nvRegionShared.h"
#include "nvcom.h"          // drivers/common/inc
#include "nvtracecom.h"     // drivers/common/inc
#endif // (NVARCH >= 0x4)

#include "Nvcm.h"
//
// export
//

#define DBG_CLIPPED_MESSAGES 1
#undef DBG_CLIPPED_MESSAGES

extern VOID MagicDelay(void);
static void CopyClipListToDrawableInfo(PDEV *ppdev, WNDOBJ *pwo, FLONG fl, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);
static void CopyClipListToOtherClients(PDEV *ppdev, WNDOBJ *pwo, FLONG fl, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo);

#if (NVARCH >= 0x4)
// Internal functions that are only called from outside of this file
// These should only be called from __glNv10Flush...()
// Moved to nvRegionShared.h
extern void nvConvertRegionGL2NV(RegionPtr nvRegion, unsigned char useExclusive, 
                          __GLregionRect *inclusiveRects, int numInclusiveRects,
                          __GLregionRect *exclusiveRects, int numExclusiveRects,
                          __GLregionRect *boundingRect);
extern void nvConvertRegionNV2GL(RegionPtr nvRegion, unsigned char useExclusive,
                          __GLregionRect **inclusiveRects, int *numInclusiveRects,
                          __GLregionRect **exclusiveRects, int *numExclusiveRects);

#endif // (NVARCH >= 0x4)

static void OglBltBackDepthBuffer(PDEV *ppdev,
                NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo,
                RECTL *srcRect,
                RECTL *dstRect);

static RECTL nullRect = { 0,0,0,0 };

//******************************************************************************
//
//  Function:   CheckUbbWindowClip
//
//  Routine Description:
//
//
//  Arguments:
//
//      ppdev - pointer to PDEV
//      clientDrawableInfo - window and clip information
//
//  Return Value:
//
//      aint none!
//
//******************************************************************************
static ULONG
AreRectanglesIntersecting(
    RECT *r1, 
    RECT *r2
)
    {
    // it is easier to check fail cases where they do not intersect...
    if ((int)r1->bottom <= (int)r2->top) return FALSE;
    if ((int)r1->top >= (int)r2->bottom) return FALSE;
    if ((int)r1->left >= (int)r2->right) return FALSE;
    if ((int)r1->right <= (int)r2->left) return FALSE;

    return TRUE;
    }

//******************************************************************************
//
//  Function:   IsNullRect
//
//  Routine Description:
//
//      Verify rectangle is valid by checking for a 0,0,0,0 rectangle.
//
//  Arguments:
//
//  Return Value:
//
//      non-zero means it is a NULL rectangle
//
//******************************************************************************
static INLINE int IsNullRect(RECTL *rect)
{
    return rect->left >= rect->right || rect->top >= rect->bottom;
}

static void
CheckUbbWindowClip(
    PDEV *ppdev,
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo
)
    {
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;
    ULONG                      fullyExposed;

    clientDrawableInfo->ubbWindowClipFlag = (ULONG)FALSE;
    if ((globalOpenGLData.oglClientListHead.clientList == NULL) ||
        (ppdev->bOglSingleBackDepthCreated == FALSE) || // Must be in UBB mode...
        (ppdev->DoubleBuffer.bEnabled && !ppdev->bOglOverlaySurfacesCreated) || // must disable ubbwinclip if pageflipping is enabled (front+back must look "same"), but when overlays are enabled we still do the ubbwinclip because mainplane+overlayplane live in  buffers, that are not visible (just copied from!)
        (IsNullRect((RECTL *)&clientDrawableInfo->rect) ||
        (clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_OCCLUDED)))
        {
        return;
        }

    OglAcquireGlobalMutex();

    fullyExposed = (ULONG)TRUE;
    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        clientInfo = &node->clientInfo;
        if (bOglHasDrawableInfo(clientInfo) &&
            clientInfo->clientDrawableInfo != clientDrawableInfo &&
            clientInfo->clientDrawableInfo->hWnd != clientDrawableInfo->hWnd)
            {
            if (AreRectanglesIntersecting(&clientInfo->clientDrawableInfo->rect, &clientDrawableInfo->rect) == TRUE)
                {
                if (!(clientInfo->clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_OCCLUDED))
                    {
                    // another OGL window intersects our window and it has some exposed pixels.
                    fullyExposed = (ULONG)FALSE;
                    }
                }
            }
        node = node->next;
        }

    clientDrawableInfo->ubbWindowClipFlag = fullyExposed;

    OglReleaseGlobalMutex();
    }

void OglTranslateCachedClipRectList(PDEV *ppdev,
                                    RECTL *srcRects, ULONG srcNumRects, 
                                    RECTL *destRects, ULONG *destNumRects,
                                    RECTL *boundingBox)
{
    ULONG ii;
    long windowLeft, windowTop;

    if (IsNullRect(&srcRects[0])) 
        {
        *destNumRects = 1;
        destRects[0] = nullRect;
        }
    else
        {
        windowLeft = boundingBox->left - ppdev->left;
        windowTop  = boundingBox->top - ppdev->top;
        for (ii = 0; ii < srcNumRects; ii++)
        {
            destRects[ii].left   = srcRects[ii].left   - windowLeft;
            destRects[ii].right  = srcRects[ii].right  - windowLeft;
            destRects[ii].top    = srcRects[ii].top    - windowTop;
            destRects[ii].bottom = srcRects[ii].bottom - windowTop;
        }
        *destNumRects = srcNumRects;
    }
}

/*
** OglTranslateClientRelativeRectListToDevice
** 
** Convert the client relative src rect list to 
** a desktop relative destination list.
** 
** FNicklisch 05.10.2000: New
*/ 
void OglTranslateClientRelativeRectListToDevice(
    PDEV *ppdev,
    RECTL *srcRects,     // src rects
    ULONG srcNumRects, 
    RECTL *destRects,    // dst rects
    ULONG *destNumRects,
    RECTL *boundingBox)  // bounding box for offset calculation
{
    ULONG ii;
    long windowLeft, windowTop;

    if (IsNullRect(&srcRects[0])) 
        {
        *destNumRects = 1;
        destRects[0] = nullRect;
        }
    else
        {
        windowLeft = boundingBox->left - ppdev->left;
        windowTop  = boundingBox->top - ppdev->top;
        for (ii = 0; ii < srcNumRects; ii++)
        {
            destRects[ii].left   = srcRects[ii].left   + windowLeft;
            destRects[ii].right  = srcRects[ii].right  + windowLeft;
            destRects[ii].top    = srcRects[ii].top    + windowTop;
            destRects[ii].bottom = srcRects[ii].bottom + windowTop;
        }
        *destNumRects = srcNumRects;
    }
}


//******************************************************************************
//
//  Function:   TranslateRectToScreen
//
//  Routine Description:
//
//      Use desktop left,top to translate rectangle to screen coordinates.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************
static void TranslateRectToScreen(
PDEV *ppdev,
RECTL *rect
)
{
    //
    // In multi-monitor system, ppdev->left and ppdev->top are the
    // left and top edge of the display relative to the desktop.
    // For example, if we have 2 monitors, primary on left and secondary
    // on right, then for primary left,top = 0,0 and for secondary
    // left,top = right_edge_of_primary, 0.  In a  single monitor system
    // left,top always equals 0,0.
    //
    rect[0].left -= ppdev->left;
    rect[0].top -= ppdev->top;
    rect[0].right -= ppdev->left;
    rect[0].bottom -= ppdev->top;
}

static void
CopyClipListToOtherClients(PDEV *ppdev, WNDOBJ *pwo, FLONG fl, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo)
{
    NV_OPENGL_CLIENT_INFO_LIST *node = NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_DRAWABLE_INFO    *nodeDrawableInfo = NULL;

    node = globalOpenGLData.oglClientListHead.clientList;
    while (node)
        {
        clientInfo = &node->clientInfo;
        nodeDrawableInfo = clientInfo->clientDrawableInfo; // drawable information for this client
        if (bOglHasDrawableInfo(clientInfo) &&
            (nodeDrawableInfo->hWnd == clientDrawableInfo->hWnd) &&
            (nodeDrawableInfo->hDC != clientDrawableInfo->hDC) &&
            (nodeDrawableInfo != clientDrawableInfo))
            {
            if (clientInfo->ppdev)
                {
                CopyClipListToDrawableInfo(clientInfo->ppdev, pwo, fl, nodeDrawableInfo);
                }
            else
                {
                CopyClipListToDrawableInfo(ppdev, pwo, fl, nodeDrawableInfo);
                }
            }
        node = node->next;
        }
}

//******************************************************************************
//
//  Function:   OglValidateDrawableInfo
//
//  Routine Description:
//
//      Walk the linked list of OpenGL nodes a verify that the drawable info
//      is in the list.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE if in list, FALSE otherwise
//
//******************************************************************************
BOOL OglValidateDrawableInfo(NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo)
{
    BOOL bRet;

    bRet = FALSE;
    if (clientDrawableInfo)
    {
        NV_OPENGL_CLIENT_INFO *clientInfo;

        OglAcquireGlobalMutex();

        // Start at top of list
        clientInfo = NULL;
        while (bOglGetNextClient(&clientInfo))
            {
            ASSERT(bOglHasDrawableInfo(clientInfo));
            if (clientDrawableInfo == clientInfo->clientDrawableInfo)
                {
                bRet = TRUE;
                break;
                }
            }
        OglReleaseGlobalMutex();
    }
    return(bRet);
}


/*
** bWindowHasMoved
**
** checks window's old and new position rectangles and determines if
** moved only ( without resize )
** 
** Return value: FALSE: if window did not move only ( but resize or did not move )
**               TRUE:  if window moved but did not resize
**
** MSchwarz 09/29/2000: new
*/
BOOL bWindowHasMoved( PRECTL prclSrc, PRECTL prclDest )
{
    BOOL bRet = FALSE;
    ASSERT(prclSrc);
    ASSERT(prclDest);

    if(    ( (prclSrc->right  - prclSrc->left)==(prclDest->right  - prclDest->left) )
        && ( (prclSrc->bottom - prclSrc->top )==(prclDest->bottom - prclDest->top)  )
        && (   (prclSrc->left != prclDest->left ) 
             ||(prclSrc->top  != prclDest->top  ) 
           )
      )            
    {
        DISPDBG((2, "  moved: l%4d t%4d r%4d b%4d (w%4d x h%4d) -> l%4d t%4d r%4d b%4d", 
            prclSrc->left, 
            prclSrc->top, 
            prclSrc->right, 
            prclSrc->bottom,
            prclSrc->right-prclSrc->left, 
            prclSrc->bottom-prclSrc->top,
            prclDest->left,
            prclDest->top,
            prclDest->right, 
            prclDest->bottom));
        bRet = TRUE;
    }
    return bRet;
}


/*
** bWindowHasBeenResized
**
** checks window's old and new position rectangles and determines if
** it has been resized
** 
** Return value: FALSE: if window hasn't been resized
**               TRUE:  if window has been resized
**
** MSchwarz 10/05/2000: new
*/
BOOL bWindowHasBeenResized( PRECTL prclSrc, PRECTL prclDest )
{
    BOOL bRet = FALSE;
    ASSERT(prclSrc);
    ASSERT(prclDest);

    if(    ( (prclSrc->right  - prclSrc->left)!=(prclDest->right  - prclDest->left) )
        || ( (prclSrc->bottom - prclSrc->top )!=(prclDest->bottom - prclDest->top)  )
      )
    {
        DISPDBG((2, "  resized: l%4d t%4d r%4d b%4d (w%4d x h%4d) -> l%4d t%4d r%4d b%4d (w%4d x h%4d)", 
            prclSrc->left, 
            prclSrc->top, 
            prclSrc->right, 
            prclSrc->bottom,
            prclSrc->right-prclSrc->left, 
            prclSrc->bottom-prclSrc->top,
            prclDest->left,
            prclDest->top,
            prclDest->right, 
            prclDest->bottom,
            prclDest->right-prclDest->left, 
            prclDest->bottom-prclDest->top
            ));
        bRet = TRUE;
    }
    return bRet;
}


// MSchwarzer 09/28/2000 need this to copy overlaymainplanefront to primary
static HWND g_hWnd = NULL;
// we need this to be able to handle occluded OGLClients
// in case of WOC_CHANGED
static PPDEV g_ppdev = NULL;
//******************************************************************************
//
//  Function:   DrvClipChanged
//
//  Routine Description:
//
//      Implements the routine which is notified when a clip list changes for
//      our client window.
//
//  Arguments:
//      fl:                     pwo:
//      WOC_CHANGED             NULL
//      WOC_RGN_CLIENT_DELTA    client delta clipping (region we will enter) + includes a pvConsumer
//      WOC_RGN_CLIENT          client clipping (new client area) + includes a pvConsumer
//      WOC_RGN_SURFACE_DELTA   desktop delta clipping (region we left)
//      WOC_RGN_SURFACE         desktop clipping (desktop - clients)
//      WOC_DELETE              includes a pvConsumer of the client that dies 
//                              NOTE: In our implementation pvConsumer already is invalidated!
//
//  Return Value:
//
//      None.
//
//******************************************************************************
VOID CALLBACK DrvClipChanged(
    WNDOBJ *pwo,
    FLONG  fl
)
{
    PDEV                    *ppdev              = NULL;
    NV_OPENGL_CLIENT_INFO   *clientInfo         = NULL;
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo = NULL;
    LONG status;
#ifdef NV_MAIN_MULTIMON
    ULONG_PTR ul;
    PVOID ppData;
    BOOL bRet; 
#endif

#if DBG
    if (NULL == pwo)
    {
        // don't have pwo
        DISPDBG((1, "DrvClipChanged: fl:0x%03x %-25s ", fl, szDbg_WOC_flGet(fl)));
    }
    else if (NULL == pwo->pvConsumer)
    {
        // don't have pvConsumer==clientInfo
        DISPDBG((1, "DrvClipChanged: fl:0x%03x %-25s , pwo:0x%p", fl, szDbg_WOC_flGet(fl), pwo));
    }
    else
    {
        if (fl & WOC_DELETE)
        {
            // should be the WOC_DELETE, but we will crash on invalid pvConsumer data if we access it
            DISPDBG((1, "DrvClipChanged: fl:0x%03x %-25s , pwo:0x%p, client:0x%p (INVALID)", fl, szDbg_WOC_flGet(fl), pwo, pwo->pvConsumer));
        }
        else if (   (NULL == OglFindClientListFromClientInfo(NULL, pwo->pvConsumer))
                 || !bOglHasDrawableInfo(pwo->pvConsumer))
        {
            // don't have clientDrawableInfo -> should not happen
            DISPDBG((1, "DrvClipChanged: fl:0x%03x %-25s , pwo:0x%p, client:0x%p", 
                fl, szDbg_WOC_flGet(fl), 
                pwo, pwo->pvConsumer));
        }
        else
        {
        
            DISPDBG((1, "DrvClipChanged: fl:0x%03x %-25s , pwo:0x%p, client:0x%p, drawable:0x%p (hWnd:0x%p, clipcount:0x%x)", 
                fl, szDbg_WOC_flGet(fl), 
                pwo, pwo->pvConsumer,
                ((NV_OPENGL_CLIENT_INFO *)pwo->pvConsumer)->clientDrawableInfo,
                ((NV_OPENGL_CLIENT_INFO *)pwo->pvConsumer)->clientDrawableInfo->hWnd,
                ((NV_OPENGL_CLIENT_INFO *)pwo->pvConsumer)->clientDrawableInfo->clipChangedCount
                ));
        }
    }
#endif

    // MSchwarzer 10/06/2000 now we do: we have to check all clientDrawableInfo->ubbWindowClipFlags
    // because after opening a new window underlying window has wrong Flag setting !!
    if (fl & WOC_CHANGED)
    {
        // It is possible for g_ppdev to be NULL here (why?)
        // Since this may cause a leak, print a debug error
        if (g_ppdev == NULL) {
            DBG_ERROR("DrvClipChanged: NULL g_ppdev w/ WOC_CHANGED");
        }
        if (   (NULL != g_ppdev)               // need a ppdev
            && g_ppdev->bEnabled               // running on enabled hardware
            && (NULL != g_ppdev->pOglServer) ) // with initialized oglserver structure
        {
            ppdev = g_ppdev;

            // calculate bounding clip rect around all clients
            bOglClipBoundsUpdate(ppdev->pOglServer);

            // loop over all clients with a NV_OPENGL_DRAWABLE_INFO
            clientInfo = NULL;
            while (bOglGetNextClient(&clientInfo))
            {
                CheckUbbWindowClip(ppdev,clientInfo->clientDrawableInfo); 
            }

            bOglStereoModeUpdate(ppdev);
            bOglOverlayModeUpdate(ppdev);

            // Defers it to next call to bOglPageFlipModeRetryEnable
            ppdev->pOglServer->ulPageFlipRetryCounter = PAGEFLIP_DEFER_UPDATE_TO_NEXT_SWAP; 

            // Clipping has changed, we should retry on next swap, but not here and now.
            // But we have to disable pageflipping if necessary!
            bOglPageFlipModeUpdate(ppdev);
        }

        // All done => reset globals to Zero
        g_hWnd                     = NULL;
        g_ppdev                    = NULL;
        DISPDBG((1, " "));
        return;
    }

    // from here on we need a pwo
    if (!pwo)
    {
        DISPDBG((1, "DrvClipChanged: pwo NULL"));
        return;
    }

    //
    // Try to get a valid ppdev.
    //
    // Win2K after a mode switch will give us outdated ppdev for all clients
    // that were started before the mode switch. So we have different 
    // mechanisms to retrieve a ppdev:
    //
    // 1. out of pso                            (default)
    // 2. out of client in pvConsumer           (fallback for all CLIENT calls)
    // 3. out of any client in global list      (fallback for all SURFACE calls)    
    // NOTE: Still a problem with WOC_DELETE because pvConsumer is already invalid.
    //       but we don't handle that call

    //
    // 1. try to get ppdev out of pso.
    //
    ppdev = (PDEV *)pwo->psoOwner->dhpdev;

    ASSERT(NULL!=ppdev);
    if (NULL==ppdev) // Be shure we don't crash!
    { 
        DISPDBG((0,"DrvClipChanged: ERROR: Invalid ppdev in Window!"));
        return;
    }

    // Win2K will give us outdated ppdevs after a modeswitch, 
    // try to find a valid ppdev instead
    if (!ppdev->bEnabled)
    {
        //
        // 2. Try this client
        //

        // NOTE:  clientInfo can be NULL!
        // NOTE2: clientInfo can be != NULL but invalid!
        clientInfo = (NV_OPENGL_CLIENT_INFO *)pwo->pvConsumer;

        if (   OglFindClientListFromClientInfo(NULL, clientInfo)
            && bOglHasDrawableInfo(clientInfo) )
        {
            DISPDBG((1, "DrvClipChanged: WARNING: GDI gave us a disabled ppdev, try to get a valid pdev out of given client!"));
            ASSERTDD(NULL!=clientInfo->ppdev, "clientInfo with a drawable should always have a ppdev!");

            ppdev = clientInfo->ppdev;
            ASSERT(NULL!=ppdev);
        }

        //
        // 3. Try any client
        //
        clientInfo = NULL; // start at top of list
        while (   !ppdev->bEnabled                 // still invalid?
               && bOglGetNextClient(&clientInfo) )
        {
            DISPDBG((1, "DrvClipChanged: WARNING: GDI gave us a disabled ppdev, try to get a valid pdev out of any client!"));
            ASSERTDD(NULL!=clientInfo->ppdev, "clientInfo with a drawable should always have a ppdev!");

            ppdev = clientInfo->ppdev;
            ASSERT(NULL!=ppdev);
        }
    }

    //memorize ppdev for WOC_CHANGED
    g_ppdev = ppdev;

#if 0 // still unused
    //
    // WOC_DELETE
    //
    // The WNDOBJ is being deleted as a result of the deletion of the window. 
    // Our problem is, that the pvConsumer (clientInfo) already was deleted 
    // through an escape in ogldrwbl.c in a OglEngDeleteDriverObj call, so 
    // there isn't much data we have left to do anything :-(
    //
    if (fl & WOC_DELETE)
    {
        ASSERT(NULL==pwo->pvConsumer);
        return;
    }
#endif

    // From now on we may access the hardware.
    // For that we need a valid and enabled ppdev!
    // FN moved it into branches...ASSERT(ppdev->bEnabled);

    //
    // WOC_RGN_SURFACE
    //
    // GDI should track changes in the surface region of the window object, and 
    // notify the driver when the surface region changes. The surface region is 
    // the display surface area excluding all visible client regions of the 
    // windows being tracked by the driver. 
    // The WNDOBJ refers to a surface region created by GDI. The pvConsumer 
    // member of the WNDOBJ structure is zero. 
    //
    // In other words: We get the entire desktop minus all ogl clients.
    //
    if (fl & WOC_RGN_SURFACE)
    {
        bOglNegativeClipListUpdate(ppdev, pwo);
    }

    // MSchwarz 09/26/2000 we need to clear all space on overlaybuffers we leave with our
    //                     window to no leave garbage there, which would affect other windows
    if (fl & WOC_RGN_CLIENT_DELTA )
    {
        ASSERT(ppdev->bEnabled);
        // this is the new delta region we will enter

        //  The window object contains the delta client region list. 
        //  The delta region is valid for this call only.

        // clear newly entered region in overlay buffers, if pwo belongs to an overlay client
        // but aware of popup-menues which save and restore frontbuffer contents but NOT 
        // overlaybuffer contents => don't destroy overlay ibn this case
        if( 0 == ppdev->lSavedScreenBits )
        {
            // clear overlay front- and backbuffer because ICD will receive a WM_PAINT message
            bClearOverlayForClient(ppdev, pwo, NULL, TRUE, TRUE);
        }
        else
        {
            // only clear overlay frontbuffer because DrvSaveScreenBits will restore it
            bClearOverlayForClient(ppdev, pwo, NULL, TRUE, FALSE);
        }
    }

    if (fl & WOC_RGN_SURFACE_DELTA)
    {
        ASSERT(ppdev->bEnabled);
        // this is the region we will leave

        //  The window object contains the delta client region list. 
        //  The delta region is valid for this call only.

        // this call belongs to a WOC_RGN_SURFACE_DELTA call into DrvClipChanged
        // since the call says the primary that it will grow, there is NO pwo->pvConsumer

        // clear overlay front of new desktop area
        bClearOverlayArea(ppdev, pwo, ICD_PALETTE_TRANSPARENT_COLOR | 0xFF000000, TRUE, FALSE );
    }


    if (fl & WOC_RGN_CLIENT)
    {
        try // just in case...
        {
            clientInfo = (NV_OPENGL_CLIENT_INFO *)pwo->pvConsumer;

            ASSERT(ppdev);
            if (OglFindClientListFromClientInfo(ppdev, clientInfo) != NULL)
            {

                ASSERT(ppdev->bEnabled);
                clientDrawableInfo = clientInfo->clientDrawableInfo;
                if (OglValidateDrawableInfo(clientDrawableInfo) == TRUE)
                {

                    // MSchwarzer 13/11/2000 we need this hWnd for a WOC_RGN_SURFACE_DELTA-call
                    // if it isn't only a windowmove
                    if( bWindowHasMoved( (PRECTL)&clientDrawableInfo->rect,&pwo->rclClient ) )
                    {
                        bGetOglClientInfoHWnd( clientInfo, &g_hWnd );
                    }

#ifdef NV_MAIN_MULTIMON                                      
                    /* Must synchronize all devices that we intersect */    
                    /* Get the first node in the OGLPDEV Linked list */
                    bRet = bLinkedListGetFirstElement(globalOpenGLData.oglpdevList, LL_CLASS_OGLPDEV, &ul, &ppData);
        
                    while ( bRet )
                    {
                        ASSERT(NULL != ppData);
    
                        /* Compare the node for intersection with the drawable */
                        if ( OglClientIntersectsDevice(ppData, clientDrawableInfo) )
                        {
                            OglSyncEngineUsingPDEV(ppData);
                        }

                        /* Get the next linked list entry */
                        bRet = bLinkedListGetNextElement(globalOpenGLData.oglpdevList, LL_CLASS_OGLPDEV, &ul, &ppData);            
                    }
#else
                    // force a sync/engine-stall with GDI channel
                    ppdev->pfnWaitEngineBusy(ppdev);
                    // force a sync/engine-stall with all OGL rendering channels
                    ppdev->oglLastChannel = -1;  // FORCE IT TO SYNC...
                    ppdev->NVFreeCount = 0;
                    ppdev->pfnWaitForChannelSwitch(ppdev);
#endif

                    CopyClipListToDrawableInfo(ppdev, pwo, fl, clientDrawableInfo);
                    CopyClipListToOtherClients(ppdev, pwo, fl, clientDrawableInfo);
                }
                else
                {
                    DISPDBG((1, "DrvClipChanged: BOGUS clientDrawableInfo"));
                }
            }
            else
            {
                DISPDBG((0, "DrvClipChanged: BOGUS clientInfo"));
            }
        }
        except(EXCEPTION_EXECUTE_HANDLER)
        {
            ASSERT(FALSE);
            status = GetExceptionCode();
            DISPDBG((0, "DrvClipChanged: WOC_RGN_CLIENT: exception = 0x%lx", status));
        }
    } 
}


//*************************************************************************
// bOglNegativeClipListCreate
// 
// allocate and initialize a negative clip list to entire desktop.
//
// NOTE: Negative cliplist only makes sense in a UBB and window flipping
//       environment.
//
// returns TRUE  if succeeded.
//         FALSE if failed
//*************************************************************************
BOOL bOglNegativeClipListCreate(PPDEV ppdev)
{
    BOOL      bRet = FALSE;
    CLIPLIST *pClip = NULL; // pointer to negative cliplist

    ASSERT(NULL != ppdev);

    if ((ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC))
    {
        pClip    = &ppdev->OglNegativeClipList;

        RtlZeroMemory(pClip, sizeof(CLIPLIST));

        if (bClipListAlloc(pClip, 1))
        {
            pClip->c = 1;
            prclRclSetXYWH(pClip->prcl, 0, 0, ppdev->cxScreen, ppdev->cyScreen);

            lClipListPixelCount(pClip);

            prclRclSetXYWH(&pClip->rclBounds, 0, 0, ppdev->cxScreen, ppdev->cyScreen);
            bRet = TRUE;
        }

        ASSERT(bRet);
    }
    return (bRet);
}


//*************************************************************************
// bOglNegativeClipListDestroy
// 
// free a negative cliplist
//
// returns TRUE  if succeeded.
//         FALSE if failed
//*************************************************************************
BOOL bOglNegativeClipListDestroy(PPDEV ppdev)
{
    BOOL bRet = FALSE;

    ASSERT(NULL != ppdev);
    if ((ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC))
    {
        bClipListFree(&ppdev->OglNegativeClipList);

        bRet = TRUE;
    }
    return (bRet);
}


//*************************************************************************
// bOglNegativeClipListUpdate
// 
// gets the rect list out of pwo and interpretes it as the negative client 
// list which is the desktop region minus all client views.
//
// returns TRUE  if succeeded.
//         FALSE if failed, negative clip list not valid!
//*************************************************************************
BOOL bOglNegativeClipListUpdate(PPDEV ppdev, WNDOBJ *pwo)
{
    LONG      c     = 0;
    CLIPENUM  ce;
    CLIPLIST *pClip = NULL; // pointer to negative cliplist
    BOOL      bMore = FALSE;
    BOOL      bRet  = TRUE; // default success exit!

    ASSERT(NULL != ppdev);

    if ((ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC))
    {
        RECTL     rclScreen;

        rclScreen.left   = ppdev->left;
        rclScreen.right  = ppdev->left+ppdev->cxScreen;
        rclScreen.top    = ppdev->top;
        rclScreen.bottom = ppdev->top+ppdev->cyScreen;

        ASSERT(NULL != pwo  );
        ASSERT(bClipListCheck(&ppdev->OglNegativeClipList));

        pClip    = &ppdev->OglNegativeClipList;
        pClip->c = 0; // invalidate all existing rects!

        // pwo and screen must be the same!
        if ( !bRclIsEqual(&rclScreen, &pwo->rclClient) )
        {
            LONG      lNeedRects;
            // Windows 2000 has a bug that after a mode switch 
            // it sends us pwo's with clip information of the mode in which the
            // OpenGL application started. 
            // This is bad if we switch to high res because we wouldn't cover outside areas.
            // Try to fix this by adding two rects to the negative cliplist
            lNeedRects = pClip->c + 4; // 4 is what lRclMinusRcl needs as minimum

            if (!bClipListReAlloc(pClip, lNeedRects))
            {
                DISPDBG((0, "bClipListReAlloc failed to realloc from %d to %d", pClip->cMax, lNeedRects));
                // NOTE: This error will override the success below!
                bRet = FALSE;
            }
            else
            {
                pClip->c = lRclMinusRcl(pClip->prcl, &rclScreen, &pwo->rclClient); 
                DISPDBG((0, "WARNING in bOglNegativeClipListUpdate: W2K modeswitch %dx%d -> %dx%d workaround active with %d rects", 
                    lRclWidth(&pwo->rclClient), lRclHeight(&pwo->rclClient), 
                    lRclWidth(&rclScreen)     , lRclHeight(&rclScreen)     , 
                    pClip->c));
            }
        }

        WNDOBJ_cEnumStart(pwo,CT_RECTANGLES,CD_ANY,0);
        do 
        {
            bMore = WNDOBJ_bEnum(pwo, sizeof(ce), &ce.c);

            if (!bClipListReAlloc(pClip, pClip->c + ce.c))
            {
                DISPDBG((0, "bClipListReAlloc failed to realloc from %d to %d", pClip->cMax, pClip->c + ce.c));
                bRet = FALSE;
                break;
            }

            for (c = 0; c < ce.c; c++)
            {
                ASSERT(pClip->c < pClip->cMax);

                // in the case of W2K switching to low res, clip against current screen
                if (bRclIntersect(&(pClip->prcl[pClip->c]), &(ce.arcl[c]), &rclScreen))
                {
                    pClip->c++;
                }
                else
                {
                    // Mode switch case??
                    DISPDBG((0, "bOglNegativeClipListUpdate: GDI did give us cliprect outside screen! %d,%d,%d,%d",
                        ce.arcl[c].left,ce.arcl[c].top,ce.arcl[c].right,ce.arcl[c].bottom));
                }
            }
        }
        while (bMore);

        //
        // Error handling
        //
        if (!bRet)
        {
            // This is an error case, where we try to do the best!
            if (pClip->cMax > 0)
            {
                // Add entire desktop
                pClip->c = 1;
                prclRclSetXYWH(pClip->prcl, 0, 0, ppdev->cxScreen, ppdev->cyScreen);
            }
            else
            {
                // can't use cliplist at all!
                pClip->c = 0;
            }
        }

        //
        // Finalize work on clip list
        //
        lClipListPixelCount(pClip);
        prclRclSetXYWH(&pClip->rclBounds, 0, 0, ppdev->cxScreen, ppdev->cyScreen);

        ASSERT(bClipListCheck(pClip));
    }
    return (bRet);
}

//*************************************************************************
// AllocateWindowRectList
//
// Given the clientDrawableInfo, allocate a memory for the list of rectangles.
//*************************************************************************
static void
ReallocCopyWindowRect(
    PDEV                    *ppdev, 
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo,
    ULONG                   numClipRects
)
{
#ifndef NV3
    // Do this for NV4 and higher only...
    // Is it a realloc?
    if ((clientDrawableInfo->sizeCopyWindowRectList > MAX_OGL_CLIP_RECTS) && 
        (clientDrawableInfo->copyWindowRect != NULL) &&
        (clientDrawableInfo->copyWindowRect != (&(clientDrawableInfo->windowRect[0])))) {
        EngFreeMem(clientDrawableInfo->copyWindowRect);
        clientDrawableInfo->sizeCopyWindowRectList = 0;
        clientDrawableInfo->copyWindowRect = NULL;
        if (clientDrawableInfo->origWindowRectList)
            {
            EngFreeMem(clientDrawableInfo->origWindowRectList);
            clientDrawableInfo->origWindowRectList = NULL;
            }
    }
                
    // To avoid thrashing allocate a bigger list.
    clientDrawableInfo->sizeCopyWindowRectList = numClipRects + MAX_OGL_CLIP_RECTS;
    clientDrawableInfo->copyWindowRect = (RECTL *)
        EngAllocMem(FL_ZERO_MEMORY, 
                    (sizeof(RECTL) * clientDrawableInfo->sizeCopyWindowRectList), 
                    ALLOC_TAG);
#else                
    // FOR NV3.....
    if (numClipRects > MAX_OGL_CLIP_RECTS) {
        numClipRects = MAX_OGL_CLIP_RECTS;
    }
#endif // NV3
    if (clientDrawableInfo->bOrigClipList == TRUE)
    {
        clientDrawableInfo->origWindowRectList = (RECTL *)
            EngAllocMem(FL_ZERO_MEMORY, 
                        (sizeof(RECTL) * clientDrawableInfo->sizeCopyWindowRectList), 
                        ALLOC_TAG);
    }
}

//*************************************************************************
// CharacterizeTheClip
//
// Determine what type of clip list is in the clientDrawableInfo.
//*************************************************************************
static void
CharacterizeTheClip(
    PDEV                    *ppdev, 
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo, 
    RECTL                   *windowRectList,
    RECTL                   *origRectList
)
{
    // do it here to avoid decision in loop above
    if (clientDrawableInfo->numClipRects == 0)
    {
        clientDrawableInfo->numClipRects = 1;
        windowRectList[0] = nullRect;
        if (clientDrawableInfo->bOrigClipList == TRUE)
            {
            COPY_RECT(origRectList[0], windowRectList[0]);
            }
    }

    // Characterize the clip
    if (clientDrawableInfo->numClipRects == 1)
    {
        if (IsNullRect(windowRectList))
        {
            clientDrawableInfo->clipFlags = NV_WINDOW_CLIP_OCCLUDED;
        }
        else if (windowRectList->left == 0 &&
                 windowRectList->top == 0 &&
                 (LONG)windowRectList->right == (LONG)clientDrawableInfo->cxScreen &&
                 (LONG)windowRectList->bottom == (LONG)clientDrawableInfo->cyScreen)
        {
            clientDrawableInfo->clipFlags = NV_WINDOW_CLIP_FULLSCREEN;
        }
        else
        {
            clientDrawableInfo->clipFlags = NV_WINDOW_CLIP_SINGLERECTANGLE;
        }
    }
    else
    {
        clientDrawableInfo->clipFlags = NV_WINDOW_CLIP_COMPLEX;
    }
}

//*************************************************************************
// CheckFullScreen
//
// Walk the list of OpenGL clients and evaluate whether there is an
// full screen OpenGL application.
//*************************************************************************
static void
CheckFullScreen(
    PDEV                    *ppdev, 
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo
)
{
    // Display driver should know whether the OpenGL is rendering in FULL screen mode
    // in order to avoid the channel switch between GDI and OpenGL
    if(clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_FULLSCREEN)
    {
        ppdev->ulOpenGLinFullScreen = TRUE;
    }
    else
    {   
        // Walk down the whole list to check whether any window is in FULL screen mode                                       
        NV_OPENGL_CLIENT_INFO_LIST *node = NULL;

        OglAcquireGlobalMutex();
        ppdev->ulOpenGLinFullScreen = FALSE;
        node = globalOpenGLData.oglClientListHead.clientList;
        while (node)
        {
            if (bOglHasDrawableInfo(&node->clientInfo))
            {
                if (node->clientInfo.clientDrawableInfo->clipFlags & NV_WINDOW_CLIP_FULLSCREEN)
                {
                    ppdev->ulOpenGLinFullScreen = TRUE;
                    break;
                }
            }
            node = node->next;
        }
        OglReleaseGlobalMutex();
    }
}

static void
ClipRectListToScreen(
    PDEV  *ppdev, 
    RECTL *windowRectList,
    ULONG numClipRects,
    LONG  cxScreen, 
    LONG  cyScreen
)
{
    int i;

    for (i = 0; i < (int)numClipRects; i++)
    {
        TranslateRectToScreen(ppdev, &windowRectList[i]);
        OglClipRectToScreen(&windowRectList[i], cxScreen, cyScreen);
    }
}

//*************************************************************************
// AllocateAndCopyWindowClipList
//
// Allocate memory for a new clip list and copy the rectangles from
// a given list.
//*************************************************************************
void
OglAllocateAndCopyWindowClipList(
    PDEV                    *ppdev,
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo,
    RECTL                   *srcRectList,
    ULONG                   numClipRects
)
{
    ASSERT(NULL!=ppdev);

    if (numClipRects > 0)
    {
        ASSERT(NULL!=clientDrawableInfo);
        ASSERT(NULL!=srcRectList);

        ReallocCopyWindowRect(ppdev, clientDrawableInfo, numClipRects);
        if (clientDrawableInfo->copyWindowRect)
        {
            RtlCopyMemory(clientDrawableInfo->copyWindowRect, // dst
                          srcRectList,                        // src
                          numClipRects * sizeof(RECTL));      // count
            ClipRectListToScreen(ppdev, 
                                 srcRectList, numClipRects,
                                 clientDrawableInfo->cxScreen, clientDrawableInfo->cyScreen);
            CharacterizeTheClip(ppdev, clientDrawableInfo, srcRectList, NULL);
        }
    }
}

//*************************************************************************
// CopyClipListToDrawableInfo
//
// This function receives the cliplist for the ICD client window
// from GDI. These rectangles are desktop related ( as all GDI stuff ).
//
// It is important to keep these rectangles in CD_RIGHTDOWN-order in
// ths cliplist because the routine which calculates an exclusive 
// cliplist needs the rectangle in this order
// 
//*************************************************************************
static void
CopyClipListToDrawableInfo(
    PDEV *ppdev,
    WNDOBJ *pwo,
    FLONG fl,
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo)
{
    int   isValidBounds = 0;
    int   isValidClientRect = 0;
    ULONG newNumClipRects, rectListLimit;
    RECT  clientRect;
    RECTL srcRect;
    POINTL ptlSrcMinusDest;
    ULONG i; // just for enumerations

    ASSERT(NULL != ppdev);
    ASSERT(NULL != ppdev->pOglServer);

    if (clientDrawableInfo)
    {
        RECTL *windowRectList = NULL;
        RECTL *origRectList = NULL;

        clientDrawableInfo->clipChangedCount++;
        clientDrawableInfo->cachedClipListDirty = TRUE;

        COPY_RECT(srcRect, clientDrawableInfo->rect);
        COPY_RECT(clientDrawableInfo->rect, pwo->rclClient);

        // remember distance between src and destination rectangle
        ptlSrcMinusDest.x = srcRect.left - pwo->rclClient.left;
        ptlSrcMinusDest.y = srcRect.top  - pwo->rclClient.top;

        switch (pwo->coClient.iDComplexity)
            {
            case DC_TRIVIAL:
            case DC_RECT:
                newNumClipRects = 1;
            break;
            case DC_COMPLEX:
            default: 
                {
                    BOOL        bMore;
                    ENUMRECTS   enumRect;
                    ULONG       i, tempRet;

                    rectListLimit = clientDrawableInfo->sizeCopyWindowRectList;
                    newNumClipRects = WNDOBJ_cEnumStart(pwo, CT_RECTANGLES, CD_RIGHTDOWN, rectListLimit);
                    if (newNumClipRects == 0xFFFFFFFF) {
                        i = 0;
                        bMore = WNDOBJ_bEnum(pwo, sizeof(ENUMRECTS), &enumRect.c);
                        while ((i < (ULONG)(ppdev->cxScreen*ppdev->cyScreen)) && bMore)
                        {
                            i++;
                            bMore = WNDOBJ_bEnum(pwo, sizeof(ENUMRECTS), &enumRect.c);
                        }
                        newNumClipRects = i;
                        // restart enumeration again
                        tempRet = WNDOBJ_cEnumStart(pwo, CT_RECTANGLES, CD_RIGHTDOWN, i);
                    }
                }

            break;
            }

        if (newNumClipRects > clientDrawableInfo->sizeCopyWindowRectList) {
            ReallocCopyWindowRect(ppdev, clientDrawableInfo, newNumClipRects);
        }

        windowRectList = clientDrawableInfo->copyWindowRect;
        clientDrawableInfo->numClipRects = newNumClipRects;
        if (clientDrawableInfo->bOrigClipList == TRUE)
            {
            origRectList = clientDrawableInfo->origWindowRectList;
            }

        isValidBounds = !IsNullRect(&pwo->coClient.rclBounds);
        COPY_RECT(clientRect, clientDrawableInfo->rect);
        OglClipRectToScreen(
            (RECTL *)&clientRect,
            clientDrawableInfo->cxScreen,
            clientDrawableInfo->cyScreen);
        isValidClientRect = !IsNullRect((RECTL *)&clientRect);

        // Move single-back-depth if necessary => moved behind clipenumeration

        switch (pwo->coClient.iDComplexity)
            {
            case DC_TRIVIAL:
                DISPDBG((2, "DrvClipChanged: DC_TRIVIAL"));
                clientDrawableInfo->numClipRects = 1;
            break;

            case DC_RECT:

                if (isValidBounds)
                {
                    DISPDBG((2, "DrvClipChanged: DC_RECT: SINGLERECTANGLE"));
                    clientDrawableInfo->numClipRects = 1;
                    COPY_RECT(windowRectList[0], pwo->coClient.rclBounds);
                    if (clientDrawableInfo->bOrigClipList == TRUE)
                        {
                        COPY_RECT(origRectList[0], windowRectList[0]);
                        }
                }
                else
                {
                    DISPDBG((2, "DrvClipChanged: DC_RECT: OCCLUDED"));
                    clientDrawableInfo->numClipRects = 0;
                }
            break;

            case DC_COMPLEX:
                {
                BOOL        bMore;
                ENUMRECTS   enumRect;
                ULONG       i;

                DISPDBG((2, "DrvClipChanged: DC_COMPLEX"));

                i = 0;
                bMore = WNDOBJ_bEnum(pwo, sizeof(ENUMRECTS), &enumRect.c);
                while (i < clientDrawableInfo->numClipRects && bMore)
                    {
                    COPY_RECT(windowRectList[i], enumRect.arcl[0]);
                    if (clientDrawableInfo->bOrigClipList == TRUE)
                        {
                        COPY_RECT(origRectList[i], windowRectList[i]);
                        }
                    i++;
                    bMore = WNDOBJ_bEnum(pwo, sizeof(ENUMRECTS), &enumRect.c);
                    }
                clientDrawableInfo->numClipRects = i;
                break;
                }
            default:
                break;
        }

        // MSchwarzer 10/06/2000  move CLIPPED client area to not overdraw other ogl-windows !!
        if (ppdev->bOglSingleBackDepthSupported &&
            ppdev->bOglSingleBackDepthCreated &&
            isValidBounds && isValidClientRect
            && bWindowHasMoved(&srcRect,&pwo->rclClient))
        {
            // notify GDI syncs that a window move is in progress and a DrvCopyBits will happen.
            ppdev->pOglServer->bWindowInMovement = TRUE;

            switch (pwo->coClient.iDComplexity)
            {
                case DC_TRIVIAL:
                case DC_RECT:
                {
                    srcRect.right  = pwo->coClient.rclBounds.right   + ptlSrcMinusDest.x;
                    srcRect.left   = pwo->coClient.rclBounds.left    + ptlSrcMinusDest.x;
                    srcRect.top    = pwo->coClient.rclBounds.top     + ptlSrcMinusDest.y;
                    srcRect.bottom = pwo->coClient.rclBounds.bottom  + ptlSrcMinusDest.y;

                    OglBltBackDepthBuffer(ppdev, 
                                          clientDrawableInfo,
                                          &srcRect,
                                          &pwo->coClient.rclBounds);
                    break;
                }
                case DC_COMPLEX:
                default: 
                {
                    ULONG       ulNumClips;
                    ULONG       ulDir; // blit direction
                    ULONG       i;
                    BOOL        bMore;
                    ENUMRECTS   enumRect;

                    // we need to know in which direction we have to blit to enumerate
                    // the clipping in the right order !
                    // but we do this only for copyonwindowmove !
                    if(ptlSrcMinusDest.x < 0)       // moving window to right ?
                    {
                        if(ptlSrcMinusDest.y < 0)   // moving window down ?
                            ulDir = CD_LEFTUP;
                        else
                            ulDir = CD_LEFTDOWN;
                    }
                    else
                    {
                        if(ptlSrcMinusDest.y < 0)   // moving window down ?
                            ulDir = CD_RIGHTUP;
                        else
                            ulDir = CD_RIGHTDOWN;
                    }

                    ulNumClips = WNDOBJ_cEnumStart(pwo, CT_RECTANGLES, ulDir, newNumClipRects);

                    do
                    {
                        bMore = WNDOBJ_bEnum(pwo, sizeof(ENUMRECTS), &enumRect.c);

                        // clipping against screen will be done in OglBltBackDepthBuffer
                        // translation is done in OglBltBackDepthBuffer, too
                        for(i=0;i<enumRect.c;i++)
                        {
                            srcRect.right  = enumRect.arcl[i].right   + ptlSrcMinusDest.x;
                            srcRect.left   = enumRect.arcl[i].left    + ptlSrcMinusDest.x;
                            srcRect.top    = enumRect.arcl[i].top     + ptlSrcMinusDest.y;
                            srcRect.bottom = enumRect.arcl[i].bottom  + ptlSrcMinusDest.y;

                            OglBltBackDepthBuffer(ppdev, 
                                                  clientDrawableInfo,
                                                  &srcRect,
                                                  &enumRect.arcl[i]);
                        }
                    }
                    while( bMore );

                    // reset enumeration with values for ICD exclusive cliplist generationg function
                    ulNumClips = WNDOBJ_cEnumStart(pwo, CT_RECTANGLES, CD_RIGHTDOWN, newNumClipRects);
                }
            }
        }

        ClipRectListToScreen(ppdev, 
                             windowRectList, clientDrawableInfo->numClipRects,
                             clientDrawableInfo->cxScreen, clientDrawableInfo->cyScreen);
        CharacterizeTheClip(ppdev, clientDrawableInfo, windowRectList, origRectList);
        CheckFullScreen(ppdev, clientDrawableInfo);
    }
}

//******************************************************************************
//
//  Function:   OglClipRectToScreen
//
//  Routine Description:
//
//      Implements a routine that clips a rectangle to the PDEV size.
//
//  Arguments:
//
//  Return Value:
//
//      None
//
//******************************************************************************
void OglClipRectToScreen(
    RECTL *rect,
    LONG cxScreen,
    LONG cyScreen
)
{
    if (rect->left < 0)
        {
        rect->left = 0;
        }
    if (rect->top < 0) 
        {
        rect->top = 0;
        }
    if (rect->right > cxScreen) 
        {
        rect->right = cxScreen;
        }
    if (rect->bottom > cyScreen) 
        {
        rect->bottom = cyScreen;
        }

    if (IsNullRect(rect))
        {
        *rect = nullRect;
        }
}

//******************************************************************************
//
//  Function:   NV4_OglFlushClipped
//
//  Routine Description:
//
//      Implements a routine that flushes a push buffer based on the clip list.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************
ULONG NV4_OglFlushClipped(
    PDEV *ppdev, 
    HWND hWnd, 
    HDC hDC, 
    ULONG hClient, 
    __GLNVflushInfo *flushInfo
)
{
#ifndef NV3 // Do this for NV4 and higher only...
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo = NULL;
    ULONG                      *pCmdBuf;
    ULONG                      *pClipStart;
    LONG                       dwCmdBufFreeCount;
    ULONG                      rectCount;
    RECTL                      *rectList;
    ULONG                      *pushBufferBase;
    ULONG                      *pushBufferEnd;
    LONG                       x;
    LONG                       y;
    LONG                       w;
    LONG                       h;
    ULONG                      ulClipHorizontal;
    ULONG                      ulClipVertical;
#ifndef USE_CLIENT_RECT_LIST
    LONG                       windowLeft;
    LONG                       windowTop;
#endif
    ULONG                      listSize;
    RECTL                      *windowRectList;
    LONG                       status;
    __GLNVcmdInfo              *cmdInfo;
    RECTL                      ubbRect;

    if (!flushInfo || 
        ppdev->dwGlobalModeSwitchCount != flushInfo->localModeSwitchCount) 
        {
        //
        // In this case, the client side should increment
        // the push buffer pointers but not render the buffer.
        //
        OglDebugPrint("NV4_OglFlushClipped: bailing OGL_ERROR_MODE_SWITCH\n");
        flushInfo->retStatus = OGL_ERROR_MODE_SWITCH;
        return((ULONG)TRUE);
        }

    clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }
    else
        {
        OglDebugPrint("NV4_OglFlushClipped: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        flushInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
        }

    if (clientInfo)
        {
        clientDrawableInfo = clientInfo->clientDrawableInfo;
        }
    else
        {
        OglDebugPrint("NV4_OglFlushClipped: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        flushInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
    }

        try
        {
        cmdInfo = flushInfo->cmdInfo;
        }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        flushInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        DISPDBG((1, "NV4_OglFlushClipped: cmdInfo: exception = 0x%lx", status));
                return((ULONG)FALSE);
    }    
    // assert(cmdInfo);
    
#ifdef DBG
    nvDebugLevel   = cmdInfo->nvDebugLevel;
    nvDebugOptions = cmdInfo->nvDebugOptions;
    nvDebugMask    = cmdInfo->nvDebugMask;
    nvControlOptions = cmdInfo->nvControlOptions;
#endif

    //
    // If the client side clipChangedCount is out of date the window may
    // have moved or resized.  Kick back to the client so buffer info may
    // be reloaded.
    //
    if (flushInfo->clipChangedCount != clientDrawableInfo->clipChangedCount)
        {
        flushInfo->retStatus = OGL_ERROR_WINDOW_CHANGED_COUNT;
        return((ULONG)TRUE); 
        }

    windowRectList = clientDrawableInfo->copyWindowRect;
    if (clientDrawableInfo->cachedClipListDirty ||
        flushInfo->wndClipRectChanged)
    {
        CombineRectLists((RECTL*) &(cmdInfo->coreClipRect), 1, 
            windowRectList,
            clientDrawableInfo->numClipRects,
            (LONG) (clientDrawableInfo->rect.left - ppdev->left),
            (LONG) (clientDrawableInfo->rect.top - ppdev->top),
            NULL, &listSize);
        
        if (listSize >= clientDrawableInfo->cachedClipListSize)
        {
            // Reallocate the cached clip list
            
            //
            // Free memory associated with cached clip list of client drawable info.
            //
            if (clientDrawableInfo->cachedClipRectList)
            {
                EngFreeMem(clientDrawableInfo->cachedClipRectList);
                clientDrawableInfo->cachedNumClipRects = 0;
                clientDrawableInfo->cachedClipListSize = 0;
            }
            
            // To avoid thrashing, add MAX_OGL_CLIP_RECTS
            listSize += MAX_OGL_CLIP_RECTS;

            // Allocate the cached clip list now.
            clientDrawableInfo->cachedClipRectList = 
                EngAllocMem(FL_ZERO_MEMORY, 
                (sizeof(RECTL) * listSize), ALLOC_TAG);
            
            if (clientDrawableInfo->cachedClipRectList == NULL)
            {
                DISPDBG((1, "NV4_OglFlushClipped  - Failed EngAllocMem  of CachedClipRectList"));
                OglDebugPrint("****************** FAILURE NV4_OglFlushClipped: EngAllocMem  of CachedClipRectList");
                flushInfo->retStatus = OGL_ERROR_REALLOC_FAILURE; 
                return((ULONG)FALSE);
            }
            
            clientDrawableInfo->cachedClipListSize = listSize;
        }
        
        
        CombineRectLists((RECTL*) &(cmdInfo->coreClipRect), 1, 
            windowRectList,
            clientDrawableInfo->numClipRects,
            (LONG) (clientDrawableInfo->rect.left - ppdev->left),
            (LONG) (clientDrawableInfo->rect.top - ppdev->top),
            &(clientDrawableInfo->cachedClipRectList[0]), 
            &(clientDrawableInfo->cachedNumClipRects));
        
        clientDrawableInfo->cachedClipListDirty = FALSE;
        
    }


    //**********************************************************************
    //**********************************************************************
    // Call into the shared library to do the kickoff
    //**********************************************************************
    //**********************************************************************
    cmdInfo->ntDrawable = (void *)clientDrawableInfo;
    cmdInfo->ntOther = ppdev;
   
    __glNV4FlushInternal(cmdInfo);

    flushInfo->retStatus = OGL_ERROR_NO_ERROR;
#endif  // #ifndef NV3  // Do this for NV4 and higher only...
    return (TRUE);


}

/*
 * Take two input rectangle regions and merge them into one.
 * If list1 and list2 are in window coordinates, leftList2 = topList2 = 0
 * If list1 and list2 are in screen coordinates, leftList2 = topList2 = 0
 * If list1 is window coordinates, list2 is in screen coordinates,
 *      leftList2 = windowLeft of list2, topList2 = windowTop oflist2.
 * Last case uninteresting as it should have been called by switching list1 and list2. 
 */
void CombineRectLists(RECTL *rectList1, ULONG count1, 
                 RECTL *rectList2, ULONG count2,
         LONG  leftList2, LONG topList2,
                 RECTL *resultList, ULONG *resultCount)
{
    ULONG i, j , k;
    RECTL *inpRect1, *inpRect2, *outRect;
    LONG l1, l2, r1, r2, t1, t2 ,b1, b2;
    RECTL tmpRect;

    // Error
    if (resultCount == NULL)
        return;

    *resultCount = 0;

    // Output list is null and out count is 0
    if ((rectList1 == NULL) || (rectList2 == NULL) || (count1 == 0) || (count2 == 0))
        return;

    // Assumes left <= right and top <= bottom
    k = 0;

    // If result List is NULL, the caller is just asking for size of output list.

    if (resultList == NULL)
    {
        outRect = &tmpRect;
    }
    else
    {
        outRect = &(resultList[0]);
    }

    for (j = 0, inpRect2 = &(rectList2[0]) ; j < count2; j++, inpRect2++) {

        l2 = inpRect2->left - leftList2;
        r2 = inpRect2->right - leftList2;
        b2 = inpRect2->bottom - topList2;
        t2 = inpRect2->top - topList2;

        for (i = 0, inpRect1 = &(rectList1[0]); i < count1; i++, inpRect1++) { 

            l1 = inpRect1->left; r1 = inpRect1->right; 
            t1 = inpRect1->top; b1 = inpRect1->bottom;


            outRect->left = ((l1 < l2) ? l2 : l1); 
            outRect->right = ((r1 > r2) ? r2 : r1); 
            outRect->top = ((t1 < t2) ? t2 : t1); 
            outRect->bottom = ((b1 > b2) ? b2 : b1); 

            if ((outRect->left < outRect->right) &&
                (outRect->top < outRect->bottom)) {
                k++; // intersection
                if (resultList != NULL)
                    outRect++;
            } else {
                outRect->left = 0;
                outRect->top = 0;
                outRect->right = 0;
                outRect->bottom = 0;
            }
        }
    }
    *resultCount = k;
}

#if (NVARCH >= 0x4)

ULONG NV_OglFlushClipped(
    PDEV *ppdev, 
    HWND hWnd, 
    HDC hDC, 
    ULONG hClient, 
    __GLNVflushInfo *flushInfo
)
{
    NV_OPENGL_CLIENT_INFO      *clientInfo = NULL;
    NV_OPENGL_CLIENT_INFO_LIST *clientList = NULL;
    NV_OPENGL_DRAWABLE_INFO    *clientDrawableInfo = NULL;
    __GLNVcmdInfo              *cmdInfo;
    LONG                       status;
    __GLregionRect             boundingRect;
    RECTL                      *windowRectList;
    RECTL                      ubbRect;
 
    if (!flushInfo || 
        ppdev->dwGlobalModeSwitchCount != flushInfo->localModeSwitchCount) 
        {
        //
        // In this case, the client side should increment
        // the push buffer pointers but not render the buffer.
        //
        OglDebugPrint("NV_OglFlushClipped: bailing OGL_ERROR_MODE_SWITCH\n");
        flushInfo->retStatus = OGL_ERROR_MODE_SWITCH;
        return((ULONG)TRUE);
        }

    clientList = OglFindClientInfoFromHWndHDC(ppdev, hWnd, hDC);
    if (clientList)
        {
        clientInfo = &clientList->clientInfo;
        }
    else
        {
        OglDebugPrint("NV_OglFlushClipped: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        flushInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
        }

    if (clientInfo)
        {
        clientDrawableInfo = clientInfo->clientDrawableInfo;
        }
    else
        {
        OglDebugPrint("NV_OglFlushClipped: bailing OGL_ERROR_CLIENT_NOT_FOUND\n");
        flushInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        return((ULONG)FALSE);
        }


    try
    {
        cmdInfo = flushInfo->cmdInfo;
    }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        flushInfo->retStatus = OGL_ERROR_CLIENT_NOT_FOUND;
        DISPDBG((1, "NV_OglFlushClipped: cmdInfo: exception = 0x%lx", status));
        return((ULONG)FALSE);
    }    
    // assert(cmdInfo);
    
#ifdef DBG
    nvDebugLevel   = cmdInfo->nvDebugLevel;
    nvDebugOptions = cmdInfo->nvDebugOptions;
    nvDebugMask    = cmdInfo->nvDebugMask;
    nvControlOptions = cmdInfo->nvControlOptions;
#endif


    //
    // If the client side clipChangedCount is out of date the window may
    // have moved or resized.  Kick back to the client so buffer info may
    // be reloaded.
    //
    if (!cmdInfo->flushedBetween2D &&
        ((LONG)cmdInfo->windowChangedCount != (LONG)clientDrawableInfo->clipChangedCount))
        {
            flushInfo->retStatus = OGL_ERROR_WINDOW_CHANGED_COUNT;
        return((ULONG)TRUE); 
        }

    cmdInfo->ntDrawable = (void *)clientDrawableInfo;
    cmdInfo->ntOther = (PDEV *)ppdev;


    if (FALSE == NV_OglUpdateClipLists(ppdev, clientDrawableInfo)) {
        flushInfo->retStatus = OGL_ERROR_REALLOC_FAILURE; 
        return((ULONG)FALSE);
    }

    __glNVFlushInternal(cmdInfo, FALSE);

    /***** We might need to send info. to client that there was a switch
    ********from complex to simple case....*****************************/

    flushInfo->retStatus = OGL_ERROR_NO_ERROR;
    return((ULONG)TRUE);

}

void *oglXAlloc(size_t size)
{
    void *ptr;

    if (size == 0) {
        return NULL;
    }
    ptr = (void *) EngAllocMem(FL_ZERO_MEMORY, size, ALLOC_TAG);

    if (ptr == NULL) {
        return NULL;    /* XXX out of memory error */
    }
    return ptr;
}

void oglXFree(void *ptr)
{
    if (ptr) {
        EngFreeMem(ptr);
    }
}


void *oglXRealloc(void *oldPtr, size_t oldSize, size_t newSize)
{
    void *newPtr = NULL;

    if (newSize != 0) {
        newPtr = (void *) EngAllocMem(FL_ZERO_MEMORY, newSize, ALLOC_TAG);
        if (oldPtr && newPtr) {
            memcpy(newPtr, oldPtr, (oldSize <= newSize ? oldSize : newSize));
            EngFreeMem(oldPtr);
        }
    } else if (oldPtr) {
        EngFreeMem(oldPtr);
    }
    if (newPtr == NULL) {
        return NULL;    /* XXX out of memory error */
    }
    return newPtr;
}

RegDataPtr oglXAllocData(int n) 
{
    RegDataPtr retPtr;

    retPtr = (RegDataPtr)oglXAlloc(REGION_SZOF(n));

    return retPtr;
}

void oglXFreeData(RegionPtr reg)
{
    if (reg)
    {
        if (reg->data && reg->data->size) 
            oglXFree((reg)->data);
    }
}

void nvConvertRegionGL2NV(RegionPtr nvRegion, unsigned char useExclusive, 
                          __GLregionRect *inclusiveRects, int numInclusiveRects,
                          __GLregionRect *exclusiveRects, int numExclusiveRects,
                          __GLregionRect *boundingRect){
    
    __GLregionRect *GLrects;
    BoxPtr nvRegionRects;
    int i;

    if (!nvRegion)
        return;

    oglXFreeData(nvRegion);

    if(useExclusive){
        nvRegion->data = oglXAllocData(numExclusiveRects);
        nvRegion->data->numRects = numExclusiveRects;
        GLrects = exclusiveRects;
    } else {
        nvRegion->data = oglXAllocData(numInclusiveRects);
        nvRegion->data->numRects = numInclusiveRects;
        GLrects = inclusiveRects;
    }
    nvRegion->data->size = nvRegion->data->numRects;
    nvRegion->extents.x1 = (short)boundingRect->x0;
    nvRegion->extents.x2 = (short)boundingRect->x1;
    nvRegion->extents.y1 = (short)boundingRect->y0;
    nvRegion->extents.y2 = (short)boundingRect->y1;

    nvRegionRects = REGION_RECTS(nvRegion);
    for (i=0; i<(int)numInclusiveRects; i++) {
        __GLregionRect *rect = &GLrects[i];

        nvRegionRects->x1 = (short)rect->x0;
        nvRegionRects->x2 = (short)rect->x1;
        nvRegionRects->y1 = (short)rect->y0;
        nvRegionRects->y2 = (short)rect->y1;
        nvRegionRects++;
    }
}

void nvConvertRegionNV2GL(RegionPtr nvRegion, unsigned char useExclusive,
                          __GLregionRect **inclusiveRects, int *numInclusiveRects,
                          __GLregionRect **exclusiveRects, int *numExclusiveRects){

    int i;
    BoxRec *nvRegionRects;
    GLint *numrects;
    __GLregionRect **rects;


    if (!nvRegion)
        return;

    nvOptimizeRegion(nvRegion);

    if(useExclusive){
        numrects = numExclusiveRects;
        rects    = exclusiveRects;
    } else {
        numrects = numInclusiveRects;
        rects    = inclusiveRects;
    }

    if(nvRegion->data){

        *numrects = nvRegion->data->numRects;
        if(*rects){
            oglXFree(*rects);
        }
        *rects = oglXAlloc((*numrects) * sizeof(__GLregionRect));

        nvRegionRects = REGION_RECTS(nvRegion);

        for (i=0; i<(int) *numrects; i++) {
            __GLregionRect *rect = &((*rects)[i]);

            // copy region to drawable private
            rect->x0 = nvRegionRects->x1;
            rect->x1 = nvRegionRects->x2;
            rect->y0 = nvRegionRects->y1;
            rect->y1 = nvRegionRects->y2;

            nvRegionRects++;
        }

    } else {
        // If there is no nvRegion->data, then there is just one
        // rect and it is in the 'extents' rect.
        *numrects = 1;
        *rects = oglXAlloc(sizeof(__GLregionRect));
        (*rects)[0].x0 = nvRegion->extents.x1;
        (*rects)[0].x1 = nvRegion->extents.x2;
        (*rects)[0].y0 = nvRegion->extents.y1;
        (*rects)[0].y1 = nvRegion->extents.y2;

    }
}

#endif // (NVARCH >= 0x4)


static void OglBltBackDepthBuffer(
    PDEV *ppdev, 
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo,
    RECTL *srcRect,
    RECTL *dstRect
)
{
    RECTL drawableRect;
    ULONG ulView;

    COPY_RECT(drawableRect, (*srcRect));
    // convert to screen relative coords
    drawableRect.left   -= ppdev->left;
    drawableRect.right  -= ppdev->left;
    drawableRect.top    -= ppdev->top;
    drawableRect.bottom -= ppdev->top;

    if ((dstRect->bottom - dstRect->top == 
         drawableRect.bottom - drawableRect.top) && // height of src and dest are equal

        (drawableRect.right - drawableRect.left > 0) && // src width > 0
        (drawableRect.bottom - drawableRect.top > 0) && // src height > 0

        (dstRect->right - dstRect->left > 0 && // dst width > 0
         dstRect->bottom - dstRect->top > 0))  // dst height > 0
    {
        LONG dstLeft, dstTop, dstRight, dstBottom;
        LONG srcLeft, srcTop;

        srcLeft   = drawableRect.left;
        srcTop    = drawableRect.top;

        dstLeft   = dstRect->left;
        dstTop    = dstRect->top;
        dstRight  = dstRect->right;
        dstBottom = dstRect->bottom;


        //****************************************************************
        // If double buffering is enabled, then the display driver is
        // already blitting some views when the window moves.
        // Only move buffers that are not double pumped by 2D.
        //****************************************************************

        // stereo loop 
        for( ulView=0; ulView < ppdev->ulOglActiveViews; ulView++ )
        {

            if (!bDoublePumped(ppdev, ppdev->singleBack1Offset[ulView])) 
            {
                OglUseDisplayDriverBlt(ppdev, clientDrawableInfo,
                                       ppdev->singleBack1Offset[ulView], ppdev->singleBack1Pitch[ulView],
                                       srcLeft, srcTop,
                                       ppdev->singleBack1Offset[ulView], ppdev->singleBack1Pitch[ulView],
                                       dstLeft, dstTop,
                                       dstRight - dstLeft,
                                       dstBottom - dstTop);
            }

            if (!bDoublePumped(ppdev, ppdev->singleDepthOffset[ulView])) 
            {
                OglUseDisplayDriverBlt(ppdev, clientDrawableInfo,
                                       ppdev->singleDepthOffset[ulView], ppdev->singleDepthPitch[ulView],
                                       srcLeft, srcTop,
                                       ppdev->singleDepthOffset[ulView], ppdev->singleDepthPitch[ulView],
                                       dstLeft, dstTop,
                                       dstRight - dstLeft,
                                       dstBottom - dstTop);
            }
            if (ppdev->bOglOverlaySurfacesCreated) 
            {
                if (!bDoublePumped(ppdev, ppdev->singleFrontMainOffset[ulView])) 
                {
                    OglUseDisplayDriverBlt(ppdev, clientDrawableInfo,
                                           ppdev->singleFrontMainOffset[ulView], ppdev->singleFrontMainPitch[ulView],
                                           srcLeft, srcTop,
                                           ppdev->singleFrontMainOffset[ulView], ppdev->singleFrontMainPitch[ulView],
                                           dstLeft, dstTop,
                                           dstRight - dstLeft,
                                           dstBottom - dstTop);
                }

                if (!bDoublePumped(ppdev, ppdev->singleBack1MainOffset[ulView])) 
                {
                    OglUseDisplayDriverBlt(ppdev, clientDrawableInfo,
                                           ppdev->singleBack1MainOffset[ulView], ppdev->singleBack1MainPitch[ulView],
                                           srcLeft, srcTop,
                                           ppdev->singleBack1MainOffset[ulView], ppdev->singleBack1MainPitch[ulView],
                                           dstLeft, dstTop,
                                           dstRight - dstLeft,
                                           dstBottom - dstTop);
                }

                //************************************************************
                // Only blit front and back overlay since the display driver
                // is already multi buffering into front and back main planes
                //************************************************************
                if (!bDoublePumped(ppdev, ppdev->singleFrontOverlayOffset[ulView])) 
                {
                    OglUseDisplayDriverBlt(ppdev, clientDrawableInfo,
                                           ppdev->singleFrontOverlayOffset[ulView], ppdev->singleFrontOverlayPitch[ulView],
                                           srcLeft, srcTop,
                                           ppdev->singleFrontOverlayOffset[ulView], ppdev->singleFrontOverlayPitch[ulView],
                                           dstLeft, dstTop,
                                           dstRight - dstLeft,
                                           dstBottom - dstTop);
                }

                if (!bDoublePumped(ppdev, ppdev->singleBack1OverlayOffset[ulView])) 
                {
                    OglUseDisplayDriverBlt(ppdev, clientDrawableInfo,
                                           ppdev->singleBack1OverlayOffset[ulView], ppdev->singleBack1OverlayPitch[ulView],
                                           srcLeft, srcTop,
                                           ppdev->singleBack1OverlayOffset[ulView], ppdev->singleBack1OverlayPitch[ulView],
                                           dstLeft, dstTop,
                                           dstRight - dstLeft,
                                           dstBottom - dstTop);
                }
            }
        }//stereo loop
    }


}

#define bRcIntersect(a,b,c) bRclIntersect((RECTL*)(a),(const RECTL*)(b),(const RECTL*)(c))

//*************************************************************************
// bOglClientIntersectingWithOtherClients
// 
// returns TRUE if the given client intersects with any visible OpenGL client.
//*************************************************************************
BOOL bOglClientIntersectingWithOtherClients(IN NV_OPENGL_CLIENT_INFO *clientInfo)
{
    NV_OPENGL_CLIENT_INFO *clientInfoCheck;
    BOOL                   bClipped = FALSE; // assume it isn't clipped

    ASSERTDD(bOglHasDrawableInfo(clientInfo), "give me a valid clientInfo!");

    clientInfoCheck = NULL; // loop through client list, starting at top
    while (bOglGetNextVisibleClient(&clientInfoCheck))
    {
        if (clientInfoCheck->clientDrawableInfo->hWnd != clientInfo->clientDrawableInfo->hWnd)
        {
            if ( bRcIntersect(NULL, 
                               &clientInfoCheck->clientDrawableInfo->rect, 
                               &clientInfo->clientDrawableInfo->rect) )
            {
                // another OGL window intersects our window and it has some exposed pixels.
                bClipped = TRUE;
                break;
            }
        }
    }

    return (bClipped);
}


//*************************************************************************
// Compute the translated inclusive and exclusive clip lists
// 
// returns TRUE if succeeds, FALSE if we fail (out of memory)
//*************************************************************************
BOOL NV_OglUpdateClipLists(PDEV *ppdev, NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo)
{
    if (clientDrawableInfo->cachedClipListDirty)
    {
        // Convert Client Drawable List to window coordinates
        // assert(clientDrawableInfo->numClipRects < clientDrawableInfo->cachedClipListSize);
        if (clientDrawableInfo->numClipRects >= clientDrawableInfo->cachedClipListSize)
        {
            // Reallocate the cached clip list
            
            //
            // Free memory associated with cached clip list of client drawable info.
            //
            if (clientDrawableInfo->cachedClipRectList)
            {
                EngFreeMem(clientDrawableInfo->cachedClipRectList);
            }

            // To avoid thrashing
            clientDrawableInfo->cachedClipListSize = clientDrawableInfo->numClipRects + MAX_OGL_CLIP_RECTS;
            
            // Allocate the cached clip list now.
            clientDrawableInfo->cachedClipRectList = 
                EngAllocMem(FL_ZERO_MEMORY, 
                (sizeof(RECTL) * (clientDrawableInfo->cachedClipListSize)), ALLOC_TAG);
            
            if (clientDrawableInfo->cachedClipRectList == NULL)
            {
                DISPDBG((1, "NV_OglUpdateClipLists  - Failed EngAllocMem  of CachedClipRectList"));
                OglDebugPrint("****************** FAILURE NV_OglUpdateClipLists: EngAllocMem of CachedClipRectList");
                clientDrawableInfo->cachedClipListSize = 0;
                clientDrawableInfo->cachedNumClipRects = 0;
                return FALSE;
            }
        }

        OglTranslateCachedClipRectList(ppdev,
                                       clientDrawableInfo->copyWindowRect,
                                       clientDrawableInfo->numClipRects,
                                       clientDrawableInfo->cachedClipRectList,
                                       &clientDrawableInfo->cachedNumClipRects,
                                       (RECTL *) &clientDrawableInfo->rect);

        /* We only need to update the exclusive clip if we are running on
         * CELSIUS or KELVIN, and if the number of inclusive clip rects
         * exceeds the number of clips supported in the hardware.  In the
         * interest of code simplicity, and because this only happens once
         * per clip change, we always update the exclusive clip.
         */
        if (1 /*clientDrawableInfo->cachedNumClipRects > numHWClipRects*/)
        {
            __GLregionRect boundingRect;

            RegionRec nvInclusiveRegion;
            RegionRec nvExclusiveRegion;

            nvInclusiveRegion.data = 0;
            nvExclusiveRegion.data = 0;

            boundingRect.x0 = 0;
            boundingRect.y0 = 0;
            boundingRect.x1 = clientDrawableInfo->rect.right - clientDrawableInfo->rect.left;
            boundingRect.y1 = clientDrawableInfo->rect.bottom - clientDrawableInfo->rect.top;

            // Convert Inclusive to nvRegion
            nvConvertRegionGL2NV(&nvInclusiveRegion, FALSE, 
                (__GLregionRect *)(clientDrawableInfo->cachedClipRectList), 
                clientDrawableInfo->cachedNumClipRects, 
                (__GLregionRect *)(clientDrawableInfo->cachedExclusiveRectList), 
                clientDrawableInfo->cachedNumExclusiveRects, 
                &boundingRect);

            nvInverse(&nvExclusiveRegion,&nvInclusiveRegion,&(nvInclusiveRegion.extents));

            // Convert nvRegion to Exclusive
            // This will free old exclusive rect list if exists...
            nvConvertRegionNV2GL(&nvExclusiveRegion, TRUE,
                             (__GLregionRect **)(&clientDrawableInfo->cachedClipRectList), 
                             &(clientDrawableInfo->cachedNumClipRects),
                             (__GLregionRect **)(&clientDrawableInfo->cachedExclusiveRectList), 
                             &(clientDrawableInfo->cachedNumExclusiveRects));

            oglXFreeData(&nvExclusiveRegion);
            oglXFreeData(&nvInclusiveRegion);
        }
        else
        {
            // Free associated exclusive rect memory
            // and set it so that it is not used.
            oglXFree(clientDrawableInfo->cachedExclusiveRectList);
            clientDrawableInfo->cachedExclusiveRectList = NULL;
            clientDrawableInfo->cachedNumExclusiveRects = 0;
        }

        clientDrawableInfo->cachedClipListDirty = FALSE;
    }

    return TRUE;
}


//*************************************************************************
// bOglClipRectAgainstClipobj
// 
// Clip the incoming pco against the single prcl. 
//
// return TRUE : there is an intersection between pco and prcl,
//        FALSE: no intersection found
//*************************************************************************
BOOL bOglClipRectAgainstClipobj(
    IN  CLIPOBJ *pco, 
    IN  RECTL   *prcl)
{
    BOOL      bRet = FALSE;

    ASSERT(NULL!=prcl);

    if (NULL == pco)
        goto Exit;

    switch (pco->iDComplexity)
    {
        case DC_TRIVIAL:
            // special case, we don't have a valid target rect here!
            // fall back to rclBounds, but better do not call it!
            ASSERT(pco->iDComplexity != DC_TRIVIAL);

        case DC_RECT:
            bRet = bRclIntersect(NULL, prcl, &pco->rclBounds);
            break;

        case DC_COMPLEX:
        {
            ENUMRECT4 ce;
            BOOL      bMore;

            // In complex case loop over clip list
        
            CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, CLIP_LIMIT);

            do
            {
                bMore = CLIPOBJ_bEnum(pco, sizeof(ce), (ULONG *)&ce);

                while (!bRet && ce.c--)
                {
                    bRet = bRclIntersect(NULL, prcl, &(ce.arcl[ce.c]));
                }
            }
            while (!bRet && bMore);
            break;
        }
    }

Exit:
    return (bRet);
}


//*************************************************************************
// bOglClipBoundsUpdate
// 
// Calculate bounding rect of all visible OpenGL clients
// returns TRUE if there is at least one visible client with a valid rect
// returns FALSE if no client is visible.
//*************************************************************************
BOOL bOglClipBoundsUpdate(struct _OGLSERVER *pOglServer )
{
    BOOL                     bRet               = FALSE; // no client visible, no bounding rect
    NV_OPENGL_CLIENT_INFO   *clientInfo         = NULL;
    RECTL                   *prclBounds;

    ASSERT(NULL!=pOglServer);

    prclBounds = &pOglServer->rclAllClientsBounds;

    if (bOglGetNextVisibleClient(&clientInfo))
    {
        ASSERT(NULL != clientInfo);
        ASSERT(NULL != clientInfo->clientDrawableInfo);

        // start with first clients rect.
        *prclBounds = *((RECTL *)&clientInfo->clientDrawableInfo->rect);

        // find first visible ogl client 
        // and initialize clientInfo with pointer to it
        while (bOglGetNextVisibleClient(&clientInfo))
        {
            ASSERT(NULL != clientInfo);
            ASSERT(NULL != clientInfo->clientDrawableInfo);

            vRclBounds(prclBounds, (RECTL*)&clientInfo->clientDrawableInfo->rect, prclBounds);
        }
        bRet = TRUE;
    }
    else
    {
        // initialize with rectangle completely outside screen
        *prclBounds = rclOutside;
        bRet = FALSE;
    }

    return (bRet);
}
