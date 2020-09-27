/******************************Module*Header*******************************\
* Module Name: ogloverlay.c
*
* OpenGL Overlay plane implementation
*
* Copyright (c) 2000 NVidia Corporation
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
#include "nvReg.h"
#include "nvcm.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "oglDD.h"
#include "oglutils.h"
#include "nvdoublebuf.h"
#include "nvcom.h"

extern VOID NV4_DmaPushSend(PDEV *);

//
// Export 
//
#include "ogloverlay.h"


void OglInitLayerPalette(void)
{
    globalOpenGLData.oglLayerPaletteListHead.paletteList = NULL;
    globalOpenGLData.oglLayerPaletteClientCount = 0;
}

/*
** bOglFindGlobalLayerPaletteHWND
** 
** Find LayerPalette entry to given HWND in global linked list.
**
** return: TRUE  - entry found: *ppLayerPalette contains pointer to palette entry
**         FALSE - not found: *ppLayerPalette not valid!
** 
** FNicklisch 28.09.2000: New
*/ 
BOOL bOglFindGlobalLayerPaletteHWND(
    IN  HWND hWnd, 
    OUT NV_LAYER_PALETTE **ppLayerPalette)
{
    NV_LAYER_PALETTE *pLayerPalette = globalOpenGLData.oglLayerPaletteListHead.paletteList;

    while (pLayerPalette && pLayerPalette->windowHandle != hWnd)
    {
        pLayerPalette = pLayerPalette->next;
    }

    *ppLayerPalette = pLayerPalette;

    return (pLayerPalette != NULL);
}


//*************************************************************************
// OglSetLayerPalette
// 
// Create a display driver managed overlay palette for the given window and
// store the user palette to it. If the palette already exists, reuse it.
// Called by ICD through escape to set an overlay palette.
//*************************************************************************
BOOL OglSetLayerPalette(__GLNVpaletteData *pSourcePalette)
{
    NV_LAYER_PALETTE *pLayerPalette;
    ULONG ulIndex;

    ASSERTDD(NULL!=pSourcePalette, "pSourcePalette missing!");

    if (!bOglFindGlobalLayerPaletteHWND(pSourcePalette->windowHandle, &pLayerPalette))
    {
        // Need to create new entry
        pLayerPalette = EngAllocMem(FL_ZERO_MEMORY, sizeof(NV_LAYER_PALETTE), ALLOC_TAG);

        if (NULL != pLayerPalette)
        {
            pLayerPalette->next = globalOpenGLData.oglLayerPaletteListHead.paletteList;
            globalOpenGLData.oglLayerPaletteListHead.paletteList = pLayerPalette;
            globalOpenGLData.oglLayerPaletteClientCount++;
        }
        else
        {
            ASSERTDD(NULL!=pLayerPalette, "failed to allocate palette entry!");
            return FALSE;
        }
    }

    pLayerPalette->windowHandle  = pSourcePalette->windowHandle;
    pLayerPalette->crTransparent = pSourcePalette->crTransparent;
    for (ulIndex = 0; ulIndex < ICD_PALETTE_ENTRIES; ulIndex++)
    {
        pLayerPalette->entries[ulIndex] = pSourcePalette->entries[ulIndex];
    }

    return TRUE;
}   


//*************************************************************************
// OglGetLayerPalette
// 
// return the display driver managed overlay palette to a given window. 
// If the palette doesn't exist, create a default palette.
// Called by ICD through escape to retrieve an overlay palette.
//*************************************************************************
BOOL OglGetLayerPalette(__GLNVpaletteData *pDestPalette)
{
    NV_LAYER_PALETTE *pLayerPalette = globalOpenGLData.oglLayerPaletteListHead.paletteList;
    ULONG ulIndex;
    BOOL bRet=FALSE;

    ASSERTDD(NULL!=pDestPalette, "OglGetLayerPalette: pDestPalette missing!");

    if (!bOglFindGlobalLayerPaletteHWND(pDestPalette->windowHandle, &pLayerPalette))
    {
        // Palette data not found!
        // Initialize input data and call OglSetLayerPalette to create a new global entry.

        // set all to white aka ff
        pDestPalette->crTransparent = ICD_PALETTE_TRANSPARENT_INDEX;
        pDestPalette->entries[0]    = ICD_PALETTE_TRANSPARENT_COLOR;
        //pDestPalette->entries[pDestPalette->crTransparent] = RGB(0, 0, 0); // black
        for (ulIndex = 1; ulIndex<256; ulIndex++)
        {                     //  red   green blue
          pDestPalette->entries[ulIndex] = RGB(0xff, 0xff, 0xff);            // white
        }

        bRet = OglSetLayerPalette(pDestPalette);
    }
    else
    {
        // found existing entry, return that
        pDestPalette->windowHandle    = pLayerPalette->windowHandle;
        pDestPalette->crTransparent   = pLayerPalette->crTransparent;
        for (ulIndex = 0; ulIndex < ICD_PALETTE_ENTRIES; ulIndex++)
        {
            pDestPalette->entries[ulIndex] = pLayerPalette->entries[ulIndex];
        }
        bRet = TRUE;
    }

    ASSERTDD(bRet, "Don't assume OglGetLayerPalette to fail!");
    return bRet;
}


//*************************************************************************
// OglDestroyLayerPalette
// 
// Free the display driver managed overlay palette of the given window.
//*************************************************************************
BOOL OglDestroyLayerPalette(HWND hWnd)
{
    NV_LAYER_PALETTE *pLayerPalette = globalOpenGLData.oglLayerPaletteListHead.paletteList;

    if (bOglFindGlobalLayerPaletteHWND(hWnd, &pLayerPalette))
    {
        if (pLayerPalette == globalOpenGLData.oglLayerPaletteListHead.paletteList)
        {
            globalOpenGLData.oglLayerPaletteListHead.paletteList = pLayerPalette->next;
        }
        else
        {
            NV_LAYER_PALETTE *pPredecessor = globalOpenGLData.oglLayerPaletteListHead.paletteList;

            while (pPredecessor->next != pLayerPalette)
            {
                pPredecessor = pPredecessor->next;
            }
            ASSERTDD(NULL != pPredecessor,"OglDestroyLayerPalette: paletteList invalid !");

            pPredecessor->next = pLayerPalette->next;
        }

        EngFreeMem(pLayerPalette);

        return TRUE;
    }

    return FALSE;
}


/*
** bClearOverlayForClient
**
** Clears the delta area of clipped clientrect on overlay buffers ( newly entered areas )
** ( front for singlebuffered, front + back otherwise )
**
** Return value: FALSE: nothing done
**               TRUE:  if a clear happend
**
** MSchwarz  09/26/2000: derived from ELSA's old code
**           11/14/2000: redesigned interface: now an additional hWnd is possible for identifying client
*/
BOOL bClearOverlayForClient(PPDEV ppdev, WNDOBJ *pwo, HWND hWnd, BOOL bClearFrontBuffer, BOOL bClearBackBuffer)
{
    BOOL                     bRet               = FALSE;
    HWND                     hWndTemp           = NULL;
    NV_LAYER_PALETTE        *pLayerPalette      = NULL;
    NV_OPENGL_CLIENT_INFO   *pClientInfo        = NULL;
    RBRUSH_COLOR             rbc                = {0};    // solid colour brush

    ASSERT(NULL != pwo);
    ASSERT(NULL != ppdev);

    //
    // Whatever we do here it is only neccessary if 
    // overlays are enabled
    //
    if (ppdev->bOglOverlaySurfacesCreated)
    {
        BOOL bMore;

        if(pwo->pvConsumer)
        {
            pClientInfo = (NV_OPENGL_CLIENT_INFO*)pwo->pvConsumer;
            ASSERT(OglFindClientListFromClientInfo(ppdev, pClientInfo));
            bGetOglClientInfoHWnd( pClientInfo, &hWndTemp );
        }
        else
        {
            hWndTemp = hWnd;
        }

        if(hWndTemp)
        {
            //
            // For overlay apps take transparent color from palette 
            // for non overlay apps take default transparent color:
            //
            ULONG ulTransparentColor = ICD_PALETTE_TRANSPARENT_COLOR | 0xFF000000;

            if(bOglFindGlobalLayerPaletteHWND( hWndTemp, &pLayerPalette))
            {
                ASSERT( NULL!=pLayerPalette );
                ASSERT( pLayerPalette->crTransparent < ICD_PALETTE_ENTRIES);
                // if hWnd owns a Palette, it belongs to an overlay client.
                // get the transparent value as solid clearcolor
                ulTransparentColor = pLayerPalette->entries[pLayerPalette->crTransparent];
            }

            bRet = bClearOverlayArea(ppdev, pwo,ulTransparentColor, bClearFrontBuffer, bClearBackBuffer);
        }
    }

    return bRet;
}



/*
** bClearOverlayArea
**
** Clears the area described by a WNDOBJ in overlayfront
** and overlayback ubb-buffers with default transparent 
** color.
**
** Return value: FALSE: nothing done
**               TRUE:  if a clear happend
**
** MSchwarz  11/28/2000: new
*/
BOOL bClearOverlayArea(PPDEV ppdev, WNDOBJ *pwo, ULONG ulTransparentColor, BOOL bClearFrontBuffer, BOOL bClearBackBuffer)
{
    BOOL          bRet = FALSE;
    RBRUSH_COLOR  rbc  = {0};    // solid colour brush

    ASSERT(NULL != pwo);
    ASSERT(NULL != ppdev);

    //
    // Whatever we do here it is only neccessary if 
    // overlays are enabled
    //
    if(   (ppdev->bOglOverlaySurfacesCreated)
        &&(  (bClearFrontBuffer) 
           ||(bClearBackBuffer)
          )
      )
    {
        BOOL bMore;

        rbc.iSolidColor = ulTransparentColor;

        ppdev->pfnAcquireOglMutex(ppdev);
        ppdev->NVFreeCount = 0;
        //**********************************************************************
        // We need to make absolutely sure that the Channel switch
        // from the DMA pusher to the PIO channel has been completed
        //**********************************************************************
        ppdev->pfnWaitForChannelSwitch(ppdev);

        if(bClearBackBuffer)
        {
            // clear delta region on overlay front and back
            ppdev->pfnSetDestBase(ppdev, ppdev->singleBack1OverlayOffset[OGL_STEREO_BUFFER_LEFT] , ppdev->singleBack1OverlayPitch[OGL_STEREO_BUFFER_LEFT] );

            WNDOBJ_cEnumStart(pwo,CT_RECTANGLES,CD_ANY,0) ;
            do 
            {//All cliprects
                CLIPENUM    ce;   // temporary buffer for clip rects

                bMore = WNDOBJ_bEnum(pwo, sizeof(ce), (ULONG *)&ce);

                if (ce.c) // can be 0
                { //Clear the Buffer

                    NV4DmaPushFillSolid( ppdev,
                                          ce.c,
                                          ce.arcl,          // List of rectangles to be filled
                                          0xF0F0,           // PATCOPY
                                          rbc,              // rbc.prb points to brush realization structure
                                          NULL);            // Pattern alignment
                }
            }
            while (bMore) ;
        }

        if(bClearFrontBuffer)
        {
            ppdev->pfnSetDestBase(ppdev, ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT] , ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_LEFT] );

            WNDOBJ_cEnumStart(pwo,CT_RECTANGLES,CD_ANY,0) ;
            do 
            {//All cliprects
                CLIPENUM    ce;   // temporary buffer for clip rects

                bMore = WNDOBJ_bEnum(pwo, sizeof(ce), (ULONG *)&ce);

                if (ce.c) // can be 0
                { //Clear the Buffer

                    NV4DmaPushFillSolid( ppdev,
                                          ce.c,
                                          ce.arcl,          // List of rectangles to be filled
                                          0xF0F0,           // PATCOPY
                                          rbc,              // rbc.prb points to brush realization structure
                                          NULL);            // Pattern alignment
                }
            }
            while (bMore) ;
        }

        // set back offset and pitch of 1st frontbuffer
        ppdev->pfnSetDestBase(ppdev, 0 , ppdev->singleBack1Pitch[OGL_STEREO_BUFFER_LEFT] );

        ppdev->pfnReleaseOglMutex(ppdev);

        bRet = TRUE;
    }

    return bRet;
}

/*
** bCopyMainPlaneFrontToPrimaryForClient
**
** Copies the MainPlaneFront to Primary of delta area of clipped clientrect 
** on overlay buffers ( areas we leave next )
** We need this to get rid of the overlay contents in Primary before primary
** area is saved away by 2D commands
** 
** Return value: FALSE: if no OverlayClient
**               TRUE:  otherwise
**
** MSchwarz 09/27/2000: new
*/
BOOL bCopyMainPlaneFrontToPrimaryForClient(PPDEV ppdev, WNDOBJ *pwo, HWND hWnd)
{
    BOOL                     bRet               = FALSE;
    NV_LAYER_PALETTE         LayerPalette;
    NV_LAYER_PALETTE        *pLayerPalette;
    NV_OPENGL_CLIENT_INFO   *pClientInfo         = NULL;

    ASSERTDD(NULL != pwo,"");
    ASSERTDD(NULL != ppdev,"");

    pLayerPalette       = &LayerPalette;

    if( 0 != hWnd )
    {

        // if hWnd owns a Palette, it belongs to an overlay client.
        if(bOglFindGlobalLayerPaletteHWND( hWnd, &pLayerPalette) )
        {
            BOOL bMore;

            ppdev->pfnAcquireOglMutex(ppdev);
            ppdev->NVFreeCount = 0;
            //**********************************************************************
            // We need to make absolutely sure that the Channel switch
            // from the DMA pusher to the PIO channel has been completed
            //**********************************************************************
            ppdev->pfnWaitForChannelSwitch(ppdev);

            ppdev->pfnSetSourceBase(ppdev, ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT] , ppdev->singleFrontMainPitch[OGL_STEREO_BUFFER_LEFT] );
            ppdev->pfnSetDestBase(ppdev, 0 , ppdev->singleBack1Pitch[OGL_STEREO_BUFFER_LEFT] );

            WNDOBJ_cEnumStart(pwo,CT_RECTANGLES,CD_ANY,0) ;
            do 
            {//All cliprects
                POINTL      ptl;
                CLIPENUM    ce;   // temporary buffer for clip rects

                bMore = WNDOBJ_bEnum(pwo, sizeof(ce), (ULONG *)&ce);

                ptl.x = pwo->rclClient.left;
                ptl.y = pwo->rclClient.top;

                if (ce.c) // can be 0
                { //copy mainplane front to primary

                    NV4DmaPushCopyBlt( ppdev,
                                       ce.c,
                                       ce.arcl,          // List of rectangles to be filled
                                       0xCCCC,           // SrcCopy !!
                                       &ptl,             // Src Ptl 
                                       &pwo->rclClient,   // Dst Ptl
                                       NULL);            // no BlendObj 

                }
            }
            while (bMore) ;

            ppdev->pfnReleaseOglMutex(ppdev);

            bRet = TRUE;

        }// if(bOglFindGlobalLayerPaletteHWND( pClientContextInfo->hWnd, &pLayerPalette)
        else
        {
            DISPDBG((2,"bCopyMainPlaneFrontToPrimaryForClient: Client is no OverlayClient"));
        }

    }//if( 0 != hWnd )

    return bRet;
}



BOOL bIsOverlayClient( PPDEV ppdev, NV_OPENGL_CLIENT_INFO *pClientInfo )
{
    BOOL bIsOverlayClient = FALSE;

    ASSERT(ppdev);

#if DEBUG
    if( OglFindClientListFromClientInfo(ppdev,pClientInfo) )
#else
    if(pClientInfo)
#endif //#if DEBUG
    {
        HWND hWnd;

        if( bGetOglClientInfoHWnd( pClientInfo, &hWnd ) )
        {
            if( 0 != hWnd )
            {
                NV_LAYER_PALETTE *pLayerPalette;
                // if hWnd owns a Palette, it belongs to an overlay client.
                if(bOglFindGlobalLayerPaletteHWND( hWnd, &pLayerPalette) )
                {
                    bIsOverlayClient = TRUE;
                }
            }
        }
    }

    return bIsOverlayClient; 
}


/*
** bCopyIntersectingMainPlaneFrontToPSO
**
** Copies the MainPlaneFront of any intersecting parts of any overlaywindow 
** to destination Surface
** psoDst should point to a surface where the area of a upcoming popup window
** have been saved for later restauration purposes. For Overlay windows we saved
** the Primary including overlays, but we have to extract the overlay part.
** So copy areas of Mainplanbefront over the psoDst.
** 
** Return value: FALSE: if no intersection found
**               TRUE:  if 1 or more intersections found
**
** MSchwarz 10/04/2000: new
*/
BOOL bCopyIntersectingMainPlaneFrontToPSO(PPDEV  ppdev, SURFOBJ *psoSrc, SURFOBJ *psoDst, RECTL* prclDst, POINTL* pptlSrc)
{
    BOOL                        bInterSectionFound  = FALSE;
    BOOL                        bScreenToScreenBlt  = FALSE;
    RECTL                       rclSrc;
    NV_OPENGL_CLIENT_INFO_LIST *pClientInfoList;
    NV_OPENGL_CLIENT_INFO      *pClientInfo         = NULL;

    ASSERT(ppdev);
    ASSERT(psoDst);
    ASSERT(STYPE_DEVICE != psoDst->iType);
    ASSERT(prclDst);
    ASSERT(pptlSrc);

    // calculate src rectangle
    rclSrc.left   = pptlSrc->x;
    rclSrc.right  = rclSrc.left + prclDst->right  - prclDst->left;
    rclSrc.top    = pptlSrc->y;
    rclSrc.bottom = rclSrc.top  + prclDst->bottom - prclDst->top;

    // do we do a Screen to Mem Blt or a Screen to Screen Blt ?
    if(   (psoDst->dhsurf)
        &&(DT_SCREEN == ((DSURF*)psoDst->dhsurf)->dt )
      )
    {
        bScreenToScreenBlt = TRUE;
    }

    pClientInfoList = globalOpenGLData.oglClientListHead.clientList;

    while( pClientInfoList )
    {
        pClientInfo = &pClientInfoList->clientInfo;

        if ( bIsOverlayClient(ppdev, pClientInfo) )
        {
            RECTL   rclIntersectSrc;
            RECTL   rclIntersectDst;
            RECTL  *prcl             = NULL;
            POINTL  ptlIntersectSrc;
            ULONG   cRect            = 0;
            NV_OPENGL_DRAWABLE_INFO *pClientDrawableInfo = pClientInfo->clientDrawableInfo;
            // find intersection with prclDst and copy contents of intersected mainplanefront
            // to prclDst
            if( pClientDrawableInfo->ubbWindowClipFlag )
            {
                cRect = 1;
                prcl = (RECTL*)(&pClientDrawableInfo->rect);
            }
            else
            {
                cRect = pClientDrawableInfo->numClipRects;
                prcl = pClientDrawableInfo->windowRect;
            }

            while( cRect )
            {
                if( bIntersect( &rclSrc, prcl, &rclIntersectSrc) )
                {
                    // calculate intersected pptlSrc / prclDst
                    rclIntersectDst.left   = prclDst->left + ( rclIntersectSrc.left   - pptlSrc->x );
                    rclIntersectDst.right  = prclDst->left + ( rclIntersectSrc.right  - pptlSrc->x );
                    rclIntersectDst.top    = prclDst->top  + ( rclIntersectSrc.top    - pptlSrc->y );
                    rclIntersectDst.bottom = prclDst->top  + ( rclIntersectSrc.bottom - pptlSrc->y );

                    ptlIntersectSrc.x = pptlSrc->x + ( rclIntersectDst.left - prclDst->left );
                    ptlIntersectSrc.y = pptlSrc->y + ( rclIntersectDst.top  - prclDst->top  );

                    // here we have to decide if we use a Screen to Mem or Screen to Screen Blt
                    if( bScreenToScreenBlt )
                    {
                        if( !bInterSectionFound )
                        {
                            (ppdev->pfnSetSourceBase)(ppdev,ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT],ppdev->singleFrontMainPitch[OGL_STEREO_BUFFER_LEFT]);
                            (ppdev->pfnSetDestBase)(ppdev,
                                                    (ULONG)((BYTE *)(((DSURF*)psoDst->dhsurf)->LinearPtr) - ppdev->pjScreen),
                                                    ((DSURF*)psoDst->dhsurf)->LinearStride);
                        }
                        //copy intersection with mainplanefront to prclDst;
                        NV4DmaPushCopyBlt(ppdev, 1,&rclIntersectDst, 0xCCCC,
                                          &ptlIntersectSrc, &rclIntersectDst, NULL);
                    }
                    else// Screen to Mem Blt
                    {
                        DSURF   *pdhsurfSave;
                        VOID    *LinearPtr;
                        ULONG    LinearStride;

                        // save away psoSrc->dhasurf -> linearpointer/stride , replace it with singleFrontMainOffset
                        //, call NV4ScreenToMemBlt and restore values
                        pdhsurfSave = (DSURF *)psoSrc->dhsurf;

                        LinearPtr       = pdhsurfSave->LinearPtr;
                        LinearStride    = pdhsurfSave->LinearStride;
                        pdhsurfSave->LinearPtr      = (PVOID)(ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT] + ppdev->pjScreen );
                        pdhsurfSave->LinearStride   = ppdev->singleFrontMainPitch[OGL_STEREO_BUFFER_LEFT];


                        NV4ScreenToMemBlt(  ppdev,1,&rclIntersectDst,psoSrc,psoDst,
                                            &ptlIntersectSrc,&rclIntersectDst,NULL);

                        pdhsurfSave->LinearPtr      = LinearPtr;
                        pdhsurfSave->LinearStride   = LinearStride;
                    }

                    bInterSectionFound = TRUE;
                }
                prcl++;
                cRect--;
            }
        }

        pClientInfoList = pClientInfoList->next;
    }
    return bInterSectionFound;
}
#if (NVARCH >= 0x4)
/*
** bDoMergeBlitOnIntersectingRectangle
**
** Does a Mergeblit on intersecting parts of any overlaywindow .
** This function has to be called after a saved popup-menue background
** behind an overlaywindow has benn restored with the mainplanefront
** contents => overlay has to be merged with rectangle
** 
** Return value: FALSE: if no intersection found
**               TRUE:  if 1 or more intersections found
**
** MSchwarz 10/05/2000: new
*/
BOOL bDoMergeBlitOnIntersectingRectangle(PPDEV  ppdev, RECTL* prclDst)
{
    BOOL                        bInterSectionFound  = FALSE;
    RECTL                       rclSrc;
    NV_OPENGL_CLIENT_INFO_LIST *pClientInfoList;
    NV_OPENGL_CLIENT_INFO      *pClientInfo         = NULL;

    ASSERT(ppdev);
    ASSERT(prclDst);

    pClientInfoList = globalOpenGLData.oglClientListHead.clientList;

    while( pClientInfoList )
    {
        pClientInfo = &pClientInfoList->clientInfo;

        if ( bIsOverlayClient(ppdev, pClientInfo) )
        {
            RECTL   rclIntersect;
            RECTL  *prcl             = NULL;
            ULONG   cRect            = 0;
            NV_OPENGL_DRAWABLE_INFO *pClientDrawableInfo = pClientInfo->clientDrawableInfo;
            // find intersection with prclDst and copy contents of intersected mainplanefront
            // to prclDst
            if( pClientDrawableInfo->ubbWindowClipFlag )
            {
                cRect = 1;
                prcl = (RECTL*)(&pClientDrawableInfo->rect);
            }
            else
            {
                cRect = pClientDrawableInfo->numClipRects;
                prcl = pClientDrawableInfo->windowRect;
            }

            while( cRect )
            {
                if( bIntersect( prclDst, prcl, &rclIntersect) )
                {
                    __GLNVoverlayMergeBlitInfo MergeInfo;
                    ULONG ulTemp;

                    RtlZeroMemory(&MergeInfo, sizeof(MergeInfo));


                    MergeInfo.dwAction = NV_OVERLAY_MERGE_BLIT    // do merge blit
                                       | NV_OVERLAY_RUN_IN_DD     // use display driver code
                                       | NV_OVERLAY_USE_CLIPRECT; // use user clipping given in x,y,w,h

                    // 
                    // NOTE: User cliping in MergeInfo is client relative!
                    //
                    OglTranslateCachedClipRectList(ppdev,
                                                   &rclIntersect,
                                                   1,
                                                   &rclIntersect,
                                                   &ulTemp,
                                                   (RECTL *) &(pClientDrawableInfo->rect));
                    ASSERT(1==ulTemp);

                    MergeInfo.x        = rclIntersect.left;                    
                    MergeInfo.y        = rclIntersect.top;                    
                    MergeInfo.width    = rclIntersect.right - rclIntersect.left;
                    MergeInfo.height   = rclIntersect.bottom - rclIntersect.top;

                    // Now call mergeblit disptatcher
                    // As this will add the clipping it isn't possible to
                    // directly call bOglSwapMergeBlit.
                    NV_OglOverlayMergeBlit(ppdev,
                        pClientDrawableInfo->hWnd,
                        pClientDrawableInfo->hDC,
                        &MergeInfo);

                    ASSERT(MergeInfo.retStatus==OGL_ERROR_NO_ERROR);
                    bInterSectionFound = TRUE;
                }
                prcl++;
                cRect--;
            }
        }

        pClientInfoList = pClientInfoList->next;
    }
    return bInterSectionFound;
}



//*********************************************************************
// bOglSwapMergeBlit
// 
// Main worker routine managing the overlay merge blit. 
// Depending on the mergeblit data it can do some 
// additional work to clean up overlays.
//
// 1. common setup
// 2. a. Loop over clip rects. 
//    b. If necessary clip against user rect
// For all rects do:
// 3. If needed update overlay front with overlay back
// 4. If needed update main front with main back
// 5. Is overlay update necessary?
//    a. No: Main plane to visible directly
//    b. Yes: Merge blit
//
// NOTE: The rects list (prclClip) must already be adjusted to match the 
//       offsets to the buffers.
//       If a offsetPixelsX adjustment is needed, do it outside! 
//       If a desktop clipping is neccessary, do it outside!
// 
// FNicklisch 11.10.2000: New, derived from __glNVSwapMergeBlit
//*********************************************************************
BOOL bOglSwapMergeBlit(
    PPDEV                ppdev,          // hw state to use for rendering
    __GLMergeBlitData   *pMergeBlitData) // structure with all merge blit information
{
    int             colordepth; // 16 or 32 as bpp colordepth
    unsigned int    ulRects   ; // count of clip rects
    __GLregionRect *pRects    ; // pointer to list of clip rects (read notes!)
    RECTL           rclDst    ;
    BOOL            b1=FALSE, b2=FALSE, b3=FALSE;

    ASSERT(NULL!=ppdev);
    ASSERT(NULL!=pMergeBlitData);
    ASSERT(0==pMergeBlitData->cClip || NULL!=pMergeBlitData->prclClip);

    //
    // 1. setup
    //
    ulRects            = pMergeBlitData->cClip     ;
    pRects             = pMergeBlitData->prclClip  ;
    colordepth         = pMergeBlitData->colordepth;

    ppdev->pfnWaitEngineBusy( ppdev );

    //
    // 2 a. Work done per clip rect
    //
    for (/*ulRects*/ ; ulRects > 0; ulRects--, pRects++) 
    {
        //
        // 2 b. calculate final clip rect against user clipping if necessary
        //
        if (pMergeBlitData->dwAction & NV_OVERLAY_USE_CLIPRECT)
        {
            if (!bIntersect((RECTL*)&pMergeBlitData->rclUser, (RECTL*)pRects, &rclDst))
            {
                continue;
            }
        }
        else
        {
            rclDst = *(RECTL*)pRects;
        }

        //
        // Additional pre surface updates
        //

        // 3. overlay back -> overlay front
        if (pMergeBlitData->dwAction & NV_OVERLAY_UPDATE_OVERLAY)
        {
            ppdev->pfnSetSourceBase(ppdev, pMergeBlitData->OverlayBackOffset , pMergeBlitData->OverlayBackPitch);
            ppdev->pfnSetDestBase(ppdev, pMergeBlitData->OverlayFrontOffset, pMergeBlitData->OverlayFrontPitch);
            NV4DmaPushCopyBlt(ppdev, 1, &rclDst, 0xcccc, (POINTL*)&rclDst, &rclDst, NULL);
        }

        // 4. main back -> main front
        if (pMergeBlitData->dwAction & NV_OVERLAY_UPDATE_MAIN)
        {
            ppdev->pfnSetSourceBase(ppdev, pMergeBlitData->MainBackOffset , pMergeBlitData->MainBackPitch );
            ppdev->pfnSetDestBase(ppdev  , pMergeBlitData->MainFrontOffset, pMergeBlitData->MainFrontPitch);
            NV4DmaPushCopyBlt(ppdev, 1, &rclDst, 0xcccc, (POINTL*)&rclDst, &rclDst, NULL);
        }

        //
        // Make the surface visible to the user
        //
        if (pMergeBlitData->dwAction & NV_OVERLAY_NO_OVERLAY_PLANE)
        {
            // 5 a. This windows doesn't use the overlay plane, so there
            // is no need to do the transparent blit and use the 
            // scratch buffer.

            // main plane to scratch
            ppdev->pfnSetSourceBase(ppdev, pMergeBlitData->MainFrontOffset   , pMergeBlitData->MainFrontPitch   );
            ppdev->pfnSetDestBase(ppdev  , pMergeBlitData->PrimaryFrontOffset, pMergeBlitData->PrimaryFrontPitch);
            NV4DmaPushCopyBlt(ppdev, 1, &rclDst, 0xcccc, (POINTL*)&rclDst, &rclDst, NULL);
        }
        else if (pMergeBlitData->dwAction & NV_OVERLAY_MERGE_BLIT)
        {
            //
            // 5 b. Merge blit
            //

            // main plane to scratch
            ppdev->pfnSetSourceBase(ppdev, pMergeBlitData->MainFrontOffset, pMergeBlitData->MainFrontPitch);
            ppdev->pfnSetDestBase(ppdev  , pMergeBlitData->ScratchOffset  , pMergeBlitData->ScratchPitch  );
            NV4DmaPushCopyBlt(ppdev, 1, &rclDst, 0xcccc, (POINTL*)&rclDst, &rclDst, NULL);

            ppdev->pfnSetSourceBase(ppdev, pMergeBlitData->OverlayFrontOffset, pMergeBlitData->OverlayFrontPitch);
            ppdev->pfnSetDestBase(ppdev  , pMergeBlitData->ScratchOffset     , pMergeBlitData->ScratchPitch     );

            // overlay plane to scratch onto main plane
            NV4DmaPushColorKeyBlt(ppdev, 1, &rclDst, (POINTL*)&rclDst, &rclDst, (ULONG)pMergeBlitData->colorref);

            // scratch to primary
            ppdev->pfnSetSourceBase(ppdev, pMergeBlitData->ScratchOffset     , pMergeBlitData->ScratchPitch     );
            ppdev->pfnSetDestBase(ppdev  , pMergeBlitData->PrimaryFrontOffset, pMergeBlitData->PrimaryFrontPitch);
            NV4DmaPushCopyBlt(ppdev, 1, &rclDst, 0xcccc, (POINTL*)&rclDst, &rclDst, NULL);
        }
    }

    NV4_DmaPushSend(ppdev);
    //ppdev->pfnWaitEngineBusy( ppdev );

    return TRUE;
}
#endif


//
// Next 4 functions provide a simple listfunction to store pointer
// values in, delete them, clear the entire list and check if they 
// are in the list . It is implemented as simple array with  
// MAX_POINTER_LIST_ELEMENTS ( defined in driver .h ) number of elements.
// There is no function to retrieve a pointer back from list.
//
// Implemented Functions so far: 
// -bAddPointerToList
// -bIsPointerInList
// -bRemovePointerFromList
// -bClearPointerList
// -bIsPointerListEmpty
// -ulGetNumberOfPointersInList
//

// At this time ( 10/13/2000 ) these functions are only used for overlay
// applications to detect a popup-window Primary to MEM copy to be able to
// remove and restore the overlay context to look more like a hardware
// overlay

/*
** bAddPointerToList
**
** Stores given pointer in list( implemented as
** simple array ) even if it is already inside.
**
** Return value: FALSE: if no place left for storing
**               TRUE:  if pointer was stored in list
**
** MSchwarz 10/13/2000: new
*/
BOOL bAddPointerToList( PPOINTER_LIST pList, PULONG pulPointer)
{
    BOOL  bAdded = FALSE;
    
    ASSERT(pList);
    
    if(pList->cElements < MAX_POINTER_LIST_ELEMENTS)
    {
        ASSERT(NULL==pList->apulElements[pList->cElements]);
        pList->apulElements[pList->cElements] = pulPointer;
        pList->cElements++;
        bAdded = TRUE;
        DISPDBG((20, "  AP:Pointer: 0x%x, cElements: %d",pulPointer,pList->cElements));                
    }
    return bAdded;
}

/*
** bIsPointerInList
**
** Checks if a given pointer is stored in list ( implemented as
** simple array )
**
** Return value: FALSE: if pointer not found
**               TRUE:  if pointer was found inside list
**
** MSchwarz 10/13/2000: new
*/
BOOL bIsPointerInList( PPOINTER_LIST pList, PULONG pulPointer)
{
    BOOL  bFound = FALSE;
    ULONG ul;
    
    ASSERT(pList);
    
    for(ul=0;ul<pList->cElements;ul++)
    {
        if( pulPointer == pList->apulElements[ul] )
        {
            DISPDBG((20, "  FP:Pointer: 0x%x, at Index: %d found",pulPointer,ul));                
            bFound = TRUE;
            break;
        }
    }

    return bFound;
}

/*
** bRemovePointerFromList
**
** removes a given pointer from list ( implemented as
** simple array ) and shifting empty place to end of list
**
** Return value: FALSE: if pointer not found
**               TRUE:  if pointer was removed from list
**
** MSchwarz 10/13/2000: new
*/
#pragma warning(disable: 4296)

BOOL bRemovePointerFromList( PPOINTER_LIST pList, PULONG pulPointer)
{
    BOOL  bRemoved = FALSE;
    ULONG ul;
    
    ASSERT(pList);

    for(ul=0;ul<pList->cElements;ul++)
    {
        if( pulPointer == pList->apulElements[ul] )
        {
            bRemoved = TRUE;
            // shift emtpy place to end of filled up queue
            for(;ul<pList->cElements-1;ul++)
                pList->apulElements[ul] = pList->apulElements[ul+1];
            //erase shifted element
            pList->apulElements[ul] = NULL;
            pList->cElements--;
            ASSERT(pList->cElements>=0);

            DISPDBG((20, "  RP:Pointer: 0x%x, cElements: %d",pulPointer,pList->cElements));                
        }
    }

    return bRemoved;
}

#pragma warning(default: 4296)

/*
** bClearPointerList
**
** completely clears a given list ( implemented as
** simple array )
**
** Return value: FALSE: no clear possible
**               TRUE:  list cleared
**
** MSchwarz 10/13/2000: new
*/
BOOL bClearPointerList( PPOINTER_LIST pList )
{
    BOOL  bCleared = FALSE;
    ULONG ul;
    
    ASSERT(pList);

    for(ul=0;ul<pList->cElements;ul++)
    {
        pList->apulElements[ul] = NULL;
    }
    pList->cElements = 0;
    bCleared = TRUE;

    DISPDBG((20, "  CP:Pointer: 0x%x, cElements: %d",0,pList->cElements));                

    return bCleared;
}


/*
** bIsPointerListEmpty
**
** returns if given pointerlist contains any
** element or not
**
** Return value: FALSE: no clear possible
**               TRUE:  list cleared
**
** MSchwarz 11/14/2000: new
*/
BOOL bIsPointerListEmpty( PPOINTER_LIST pList )
{
    BOOL bIstEmtpy = FALSE;
    ASSERT(pList);

    if( 0 == pList->cElements )
    {
        bIstEmtpy = TRUE;
    }

    return bIstEmtpy;
}


/*
** bRemoveFirstElementFromList
**
** removes the 1st element ( last recently used )
** from list to make space 
**
** Return value: FALSE: no element removed
**               TRUE:  1st element removed
**
** MSchwarz 11/21/2000: new
*/
BOOL bRemoveFirstElementFromList( PPOINTER_LIST pList )
{
    BOOL bElementRemoved = FALSE;
    ASSERT(pList);

    if( 0 != pList->cElements )
    {
        if( bRemovePointerFromList( pList, (ULONG*)(pList->apulElements[0]) ) )
            bElementRemoved = TRUE;
    }

    return bElementRemoved;
}


/*
** ulGetNumberOfPointersInList
**
** returns number af stored pointers in list
*/
BOOL ulGetNumberOfPointersInList( PPOINTER_LIST pList )
{
    ASSERT(pList);

    return pList->cElements;
}


//*************************************************************************
// bOglIsOverlayClient
// returns TRUE if the given clientInfo describes a client running a 
// overlay pixelformat.
//*************************************************************************
BOOL bOglIsOverlayClient(NV_OPENGL_CLIENT_INFO *pClientInfo)
{
    BOOL bIsOverlayClient;

    bIsOverlayClient =  bOglHasDrawableInfo(pClientInfo)
                    && (pClientInfo->pfd.bReserved != 0);

    return (bIsOverlayClient);
}


//*************************************************************************
// bOglGetNextOverlayClient
//
// finds next OpenGL client running on a overlay pixelformat starting with 
// next client after *ppClientInfo or at top of list if *ppClientInfo==NULL. 
//
// ppClientInfo == NULL: Only check if there is a client
// ppClientInfo != NULL: In addition return found client
//                       *ppClientInfo == NULL: Start search at top of 
//                                              client list
//                       *ppClientInfo != NULL: Start search with given 
//                                              client (not included)
// Returns TRUE if a overlay client was found and returns a pointer to the 
// clientInfo in *ppClientInfo if ppClientInfo!=NULL.
// Returns FALSE if client wasn't found, *ppClientInfo isn't touched!
//*************************************************************************
BOOL bOglGetNextOverlayClient(
    NV_OPENGL_CLIENT_INFO **ppClientInfo) // 
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    BOOL                   bRet = FALSE;

    // use local copy to query bOglGetNextVisibleClient!
    if (NULL==ppClientInfo)
        clientInfo = NULL;
    else
        clientInfo = *ppClientInfo; // can be NULL!

    while (bOglGetNextClient(&clientInfo))
    {
        if (bOglIsOverlayClient(clientInfo))
        {
            // if wanted, return found client
            if (NULL != ppClientInfo) 
            {
                *ppClientInfo = clientInfo;
            }
            bRet = TRUE;
            break;
        }
    }

    return (bRet);
}


//*************************************************************************
// bOglGetNextVisibleOverlayClient
//
// finds next visible OpenGL client running on a overlay pixelformat 
// starting with next client after *ppClientInfo or at top of list 
// if *ppClientInfo==NULL. 
//
// ppClientInfo == NULL: Only check if there is a client
// ppClientInfo != NULL: In addition return found client
//                       *ppClientInfo == NULL: Start search at top of 
//                                              client list
//                       *ppClientInfo != NULL: Start search with given 
//                                              client (not included)
// Returns TRUE if a overlay client was found and returns a pointer to the 
// clientInfo in *ppClientInfo if ppClientInfo!=NULL.
// Returns FALSE if client wasn't found, *ppClientInfo isn't touched!
//*************************************************************************
BOOL bOglGetNextVisibleOverlayClient(
    NV_OPENGL_CLIENT_INFO **ppClientInfo)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    BOOL                   bRet = FALSE;

    // use local copy to query bOglGetNextVisibleClient!
    if (NULL==ppClientInfo)
        clientInfo = NULL;
    else
        clientInfo = *ppClientInfo; // can be NULL!

    while (bOglGetNextOverlayClient(&clientInfo))
    {
        if (bOglIsVisibleClient(clientInfo))
        {
            // if wanted, return found client
            if (NULL != ppClientInfo) 
            {
                *ppClientInfo = clientInfo;
            }
            bRet = TRUE;
            break;
        }
    }

    return (bRet);
}



//*************************************************************************
// bOglIsOverlayModeEnabled
// 
// returns TRUE if driver is in overlay mode and handles overlay and main 
// plane buffers (e.g. double pumping).
//*************************************************************************
BOOL bOglIsOverlayModeEnabled(PPDEV ppdev)
{
    ASSERT(NULL != ppdev);

    return(bDoublePumped(ppdev, ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT]));
}


//*************************************************************************
// bOglOverlayModeEnable
// 
// enables overlay mode. For now this is only double pumping to the 
// overlay front main surface. 
//
// Returns TRUE if succeeded
//*************************************************************************
BOOL bOglOverlayModeEnable(PPDEV ppdev)
{
    BOOL bRet = FALSE;

    ASSERT(NULL != ppdev);

    if (   ppdev->bOglOverlaySurfacesCreated
        && ppdev->bOglOverlayDesired
        && !bOglIsOverlayModeEnabled(ppdev))
    {
        ULONG ulSurfaceOffest;

        ASSERT(ppdev->bOglSingleBackDepthCreated);
        ASSERT(bOglGetNextVisibleOverlayClient(NULL));

        ulSurfaceOffest = ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT];

        if (!bDoublePumped(ppdev, ulSurfaceOffest) )
        {
            NV_AddDoubleBufferSurface(ppdev, ulSurfaceOffest);
        }
        bRet = TRUE;
    }
    return (bRet);
}


//*************************************************************************
// bOglOverlayModeDisable
// 
// disable overlay mode. For now this is only double pumping to the 
// overlay front main surface. 
//
// Returns TRUE if succeeded
//*************************************************************************
BOOL bOglOverlayModeDisable(PPDEV ppdev)
{
    BOOL bRet = FALSE;
    if (   ppdev->bOglOverlaySurfacesCreated
        && ppdev->bOglOverlayDesired
        && bOglIsOverlayModeEnabled(ppdev))
    {
        ULONG ulSurfaceOffest;

        ASSERT(ppdev->bOglSingleBackDepthCreated);

        ulSurfaceOffest = ppdev->singleFrontMainOffset[OGL_STEREO_BUFFER_LEFT];

        if (bDoublePumped(ppdev, ulSurfaceOffest) )
        {
            NV_RemoveDoubleBufferSurface(ppdev, ulSurfaceOffest);
        }
        bRet = TRUE;
    }
    return (bRet);
}


//*************************************************************************
// bOglOverlayModeUpdate
// 
// Update overlay mode. Need to activate double pumping on visible windows.
//
// Returns TRUE if succeeded
//*************************************************************************
BOOL bOglOverlayModeUpdate(PPDEV ppdev)
{
    BOOL                   bRet = FALSE;

    ASSERT(NULL != ppdev);

    if (   ppdev->bOglOverlaySurfacesCreated
        && ppdev->bOglOverlayDesired)
    {
        ASSERT(ppdev->bOglSingleBackDepthCreated);

        // Find next visible overlay client to enable overlay mode
        if (bOglGetNextVisibleOverlayClient(NULL))
        {
            bRet = bOglOverlayModeEnable(ppdev);
        }
        else 
        {
            bRet = bOglOverlayModeDisable(ppdev);
        }
    }

    return bRet;
}


//*************************************************************************
// cOglOverlayClients
// returns the current number of clients running a stereo pixelformat
//*************************************************************************
ULONG cOglOverlayClients(PPDEV ppdev)
{
    NV_OPENGL_CLIENT_INFO *clientInfo;
    ULONG                  cOverlayClients=0;

    clientInfo = NULL;
    while (bOglGetNextOverlayClient(&clientInfo))
    {
        ASSERT(ppdev->bOglOverlayDesired);
        cOverlayClients++;
    }

    return (cOverlayClients);
}


//******************************************************************************
//
//  Function:   bOglOverlaySurfacesAllowed
//
//  Checks 3 things: 1. is board workstation board 
//                   2. do we have enough framebuffer to allocate all overlay buffers
//                   3. are overlay pixelformats allowed by registry   
//
//  Return Value: TRUE:  overlay pixelformats are allowed and there are enough ressources for them
//                FALSE: overlay pixelformats are not allowed
//
//******************************************************************************
BOOL bOglOverlaySurfacesAllowed( PPDEV ppdev )
{
    ASSERT( ppdev );

    ppdev->bOglOverlayDesired = FALSE;

    if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC)
    {
        OglReadRegistry(ppdev);

        // do we have enough frambuffer memory to enable overlay ?
        ASSERT(ppdev->cyScreen > 0);
        ASSERT(ppdev->cyScreen < ppdev->cyMemory);
        //     room for 7 fullscreen buffer:    front+back0+z+mainf+mainb + ovf+ovb
        if(ppdev->cyMemory >= (LONG)(ppdev->cyScreen*7) )
        {
             if (ppdev->oglRegOverlaySupport == NV_REG_OGL_OVERLAY_SUPPORT_ON)
             {
                 ppdev->bOglOverlayDesired = TRUE;
             }
        }
    }
    return (ppdev->bOglOverlayDesired);
}


