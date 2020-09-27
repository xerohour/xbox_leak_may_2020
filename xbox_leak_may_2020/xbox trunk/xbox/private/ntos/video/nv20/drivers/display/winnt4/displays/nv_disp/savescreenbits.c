//************************* Module Header **************************************
//                                                                             *
//  Module Name: SaveScreenBits.c                                              *
//                                                                             *
//  This module contains the DrvSaveScreenBits realization                     *
//                                                                             *
//   Copyright (C) 2000 NVidia Corporation. All Rights Reserved.               *
//                                                                             *
//                                                                             *
//  DrvSaveScreenBits it needed for use of opengl overlay to be able to exract *
//  and to restore the correct overlay contents                                *
//                                                                             *
//  This function is NV4 or better only !                                      * 
//                                                                             *
//******************************************************************************
#include "precomp.h"
#include "driver.h"
#include "oglDD.h"
#include "OglOverlay.h"
#include "oglstereo.h"
#include "oglsync.h"
#include "linkedlist.h"
#include "rectutils.h"
#include "nv32.h"
#include "nvcm.h"


#include "savescreenbits.h"

// Number of maximum stored screenbits before garbage collection is called.
#define MAX_SAVESCREENBITS 20

static BOOL bSaveScreenBitsFree(
    IN PPDEV        ppdev, 
    IN ULONG_PTR    ident, 
    IN SURFOBJ     *psoFree);

//
//  DrvSaveScreenBits
//
//  Saves away contents of the desktop before a popup-menue 
//  will apear and restores it after the popup has gone.
//  Function is mainly needed for opengl overlay apps for whom
//  we have to handle the overlay merging in this call.
//
//  NOTE: GDI has a bug. Starting an application from the start menu
//        will create (SS_SAVE) a couple of ScreenBits but not free 
//        them (SS_RESTORE/SS_FREE). Because of this bug we would 
//        leak memory and therefore we need a garbage collection which
//        is called from time to time. 
//        BTW: If any OpenGL is started, GDI behaves correctly. 
//
//  Return value: FALSE:   no action succeeded
//                TRUE:    if SS_RESTORE or SS_FREE succeeded
//                psoSave: if SS_SAVE succeeded   
//
ULONG_PTR APIENTRY DrvSaveScreenBits(
    SURFOBJ  *pso,
    ULONG    iMode,
    ULONG_PTR ident,
    RECTL    *prcl
    )
{
    ULONG_PTR  ulRet              = FALSE;
    PPDEV      ppdev              = NULL;
    POINTL     ptlSrc             = {0, 0};
    SURFOBJ   *psoSave            = NULL;  
    DSURF*     pdsurf             = NULL;
    HBITMAP    hbmpSave           = NULL;

    ASSERT(pso);

    ppdev = (PPDEV)pso->dhpdev;
    ASSERT(ppdev);

    DISPDBG((3,"DrvSaveScreenBits { pso:0x%p, iMode:%8s, ident:0x%08x, prcl:0x%p",pso,iMode==SS_SAVE?"SAVE":(iMode==SS_RESTORE?"RESTORE":(iMode==SS_FREE?"FREE":"unknown")),ident,prcl));

    // Disable DrvSaveScreenBits for NV4/NV5 for now since this causes
    // a crash in the kernel.  Not sure why but this is an urgent 
    // showstopper so we'll just disable it until we can talk to Yoko
    // about it.  Crash does not occur on NV10 and higher chips so
    // this should not affect overlays since they only run on NV10 and
    // higher.
    
    if (!(   HWGFXCAPS_QUADRO_GENERIC(ppdev)
          && (   bOglIsOverlayModeEnabled(ppdev)
              || bOglIsStereoModeEnabled(ppdev)
             ) 
       ) )
    {
        switch(iMode) 
        {
            case SS_SAVE:
                // Cleanup all collected screenbits as they're not longer used.
                if (   HWGFXCAPS_QUADRO_GENERIC(ppdev)
                    && (ppdev->lSavedScreenBits > 0) )
                {
                    bSaveScreenBitsGarbageCollection(ppdev,0);
                }
                break;
            case SS_RESTORE:
                break;
            case SS_FREE:
                ulRet = TRUE;
                break;
            default:
                ASSERT(SS_SAVE==iMode);
                break;
        }
        goto Exit;
    }

#if (NVARCH >= 0x4)
    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowRect(ppdev,pso,prcl,NULL);        
    }

    ASSERT(NULL!=ppdev->pLinkedListSaveScreenBits)
    ASSERT(NULL!=ppdev->pfnCopyBlt);
    ASSERT(NULL!=ppdev->pfnScreenToMemBlt);
    ASSERT(NULL!=ppdev->pfnMemToScreenBlt);

    switch(iMode)
    {

        // The driver should save the data from the rectangle defined by prcl. 
        // The driver is responsible for managing this data in its off-screen 
        // memory. The ident parameter is ignored. 
        // Upon success, DrvSaveScreenBits should return an identifier for the 
        // saved data. The driver can return a handle or even a pointer to its 
        // off-screen memory. This function returns zero if it fails. 
        case SS_SAVE:
        {
            RECTL     rclTrg;
            SIZEL     sizlRect;
            ULONG     ulIdent;

            ASSERT(NULL!=prcl);
            DISPDBG((20,"DrvSaveScreenBits: iMode = SS_SAVE"));

            ppdev->ulSaveScreenBitsRefCounter++;
            if (0==ppdev->ulSaveScreenBitsRefCounter) // case of wrap around, avoid 0!
                ppdev->ulSaveScreenBitsRefCounter++;

            ulIdent = ppdev->ulSaveScreenBitsRefCounter;

            sizlRect.cx = lRclWidth(prcl);
            sizlRect.cy = lRclHeight(prcl);

            if( bOglIsOverlayModeEnabled(ppdev) )
                sizlRect.cy += lRclHeight(prcl);
            if( bOglIsStereoModeEnabled(ppdev) )
                sizlRect.cy += lRclHeight(prcl);

            // try to use offscreen memory
            hbmpSave = DrvCreateDeviceBitmap((DHPDEV)ppdev, sizlRect, pso->iBitmapFormat);

            if (!hbmpSave)
            {   
                //Not enough room in OFFSCREEN
                DISPDBG((2,"DrvSaveScreenBits: Not enough room in OffScreen for Bitmap")); 

                hbmpSave = EngCreateBitmap(sizlRect, 0, pso->iBitmapFormat, BMF_TOPDOWN, NULL);
                if(hbmpSave)
                {
                    EngAssociateSurface((HSURF)hbmpSave, ppdev->hdevEng, 0);
                }
                else
                {
                    DISPDBG((0, "DrvSaveScreenBits failed to get offscreen and host memory!"));
                    break;
                }
            }

            ASSERT(hbmpSave);
            ptlSrc.x = prcl->left;
            ptlSrc.y = prcl->top;

            psoSave = EngLockSurface((HSURF)hbmpSave);
            pdsurf  = (DSURF*)psoSave->dhsurf;

            rclTrg.left   = 0;
            rclTrg.right  = lRclWidth(prcl);
            rclTrg.top    = 0;
            rclTrg.bottom = lRclHeight(prcl);

            // stereo:  save frontleft + frontrigth
            // overlay: save frontleft (with mainplanefront for overlay clients) + overlayfront 

            // 1st screen to offscreen copy 
            if(IS_DEV_DEVICEBITMAP(psoSave))
            {
                ULONG      ulDstOffset;
                LONG       lDstStride;

                // source is our primary
                ppdev->pfnSetSourceBase(ppdev,ppdev->ulPrimarySurfaceOffset,ppdev->lDelta);
                // destination is device bitmap
                ulDstOffset = (ULONG)((BYTE *)(pdsurf->LinearPtr) - ppdev->pjFrameBufbase);
                lDstStride  = pdsurf->LinearStride;
                ppdev->pfnSetDestBase(ppdev, ulDstOffset , lDstStride);

                ppdev->pfnCopyBlt(ppdev, 1, &rclTrg, 0xcccc, &ptlSrc, &rclTrg, NULL);

                // special case handling for saving away overlay frontbuffer
                if( bOglIsOverlayModeEnabled(ppdev) )
                {
                    // if rclTrg intersects with opengl overlay application,copy frontbuffer
                    // contents without overlay of this client
                    bCopyIntersectingMainPlaneFrontToPSO( ppdev, pso, psoSave, &rclTrg, &ptlSrc);

                    rclTrg.top    += lRclHeight(prcl);
                    rclTrg.bottom += lRclHeight(prcl);

                    ASSERT(rclTrg.bottom <= psoSave->sizlBitmap.cy);

                    // source is our overlay frontbuffer
                    ppdev->pfnSetSourceBase(ppdev,
                                            ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT],
                                            ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_LEFT]);

                    ASSERT(NULL != ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt);
                    ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, 1, &rclTrg, 0xcccc, &ptlSrc, &rclTrg, NULL);
                }
                    
                // special case handling for saving away stereo frontbufferleft
                if( bOglIsStereoModeEnabled(ppdev) )
                {
                    rclTrg.top    += lRclHeight(prcl);
                    rclTrg.bottom += lRclHeight(prcl);

                    ASSERT(rclTrg.bottom <= psoSave->sizlBitmap.cy);

                    // source is our overlay frontbuffer
                    ppdev->pfnSetSourceBase(ppdev,
                                            ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT],
                                            ppdev->singleFrontPitch[OGL_STEREO_BUFFER_RIGHT]);

                    ASSERT( NULL != ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt );
                    ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, 1, &rclTrg, 0xcccc, &ptlSrc, &rclTrg, NULL);
                }

            }
            // screen to mem copy
            else
            {
                ASSERT(!IS_DEVICEBITMAP(psoSave));
                // source is our primary
                ppdev->pfnSetSourceBase(ppdev,ppdev->ulPrimarySurfaceOffset,ppdev->lDelta);
                // destination is mainmemory => no call to ppdev->pfnSetDestBase needed

                ppdev->pfnScreenToMemBlt( ppdev, 1, &rclTrg, pso, psoSave, &ptlSrc, &rclTrg, NULL);

                if( bOglIsOverlayModeEnabled(ppdev) )
                {
                    PVOID pvLastDstOffset;

                    // if rclTrg intersects with opengl overlay application,copy frontbuffer
                    // contents without overlay of this client
                    bCopyIntersectingMainPlaneFrontToPSO( ppdev, pso, psoSave, &rclTrg, &ptlSrc);

                    // we need this structure for NV4ScreenToMemBlt
                    rclTrg.top    += lRclHeight(prcl);
                    rclTrg.bottom += lRclHeight(prcl);

                    ASSERT(rclTrg.bottom <= psoSave->sizlBitmap.cy);

                    // source is overlay frontbuffer
                    ppdev->pfnSetSourceBase(ppdev,
                                            ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT],
                                            ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_LEFT]);

                    // pfnScreenToMemBlt needs pso->dhsurf to determine src offset
                    pvLastDstOffset = ((DSURF*)pso->dhsurf)->LinearPtr;
                    ((DSURF*)pso->dhsurf)->LinearPtr = ppdev->pjFrameBufbase + ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT];
        
                    ASSERT( NULL != ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt );
                    ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt( ppdev, 1, &rclTrg, pso, psoSave, &ptlSrc, &rclTrg, NULL);

                    // repair primary's LinearPtr
                    ((DSURF*)pso->dhsurf)->LinearPtr = pvLastDstOffset;
                }

                // special case handling for saving away stereo frontbufferleft
                if( bOglIsStereoModeEnabled(ppdev) )
                {
                    PVOID pvLastDstOffset;

                    rclTrg.top    += lRclHeight(prcl);
                    rclTrg.bottom += lRclHeight(prcl);

                    ASSERT(rclTrg.bottom <= psoSave->sizlBitmap.cy);

                    // source is our overlay frontbuffer
                    ppdev->pfnSetSourceBase(ppdev,
                                            ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT],
                                            ppdev->singleFrontPitch[OGL_STEREO_BUFFER_RIGHT]);

                    // pfnScreenToMemBlt needs pso->dhsurf to determine src offset
                    pvLastDstOffset = ((DSURF*)pso->dhsurf)->LinearPtr;
                    ((DSURF*)pso->dhsurf)->LinearPtr = ppdev->pjFrameBufbase + ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT];
        
                    ASSERT( NULL != ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt );
                    ppdev->DoubleBuffer.SavedProcs.pfnScreenToMemBlt( ppdev, 1, &rclTrg, pso, psoSave, &ptlSrc, &rclTrg, NULL);

                    // repair primary's LinearPtr
                    ((DSURF*)pso->dhsurf)->LinearPtr = pvLastDstOffset;
                }

            }

            // remember number of actually stored popups to keep opengl overlay happy
            ppdev->lSavedScreenBits++;
            bLinkedListAddElement(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS, ulIdent, psoSave, 0);

            bSaveScreenBitsGarbageCollection(ppdev, MAX_SAVESCREENBITS);

            ulRet = (ULONG_PTR)ulIdent;
            break;
        }

        // The driver should restore the data identified by ident to the 
        // rectangle prcl on the display; that is, the driver should restore 
        // the bitmap to its original position. The driver can assume that 
        // the rectangle at prcl is exactly the same size as the rectangle 
        // that was saved. The data should be discarded after this call. 
        // DrvSaveScreenBits should return TRUE if the data has been restored 
        // to the display, or FALSE if the data cannot be restored. 
        case SS_RESTORE:
        {
            HSURF hsurf;
            DISPDBG((20,"DrvSaveScreenBits: iMode = SS_RESTORE"));

            //
            // special case handling for restoring overlay frontbuffer and stereo
            // frontbuffer left. Overlay/stereo mode may have been switched on 
            // between saving and restoring => check if overlay/stereo has been saved 
            // !!  we ( may get problems with stereo + overlay at the same time )
            //
            ASSERT(NULL!=prcl);
            if (bLinkedListGetElement(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS, ident, &psoSave))
            {
                ASSERT(NULL!= psoSave);
                pdsurf     = (DSURF*)psoSave->dhsurf;

                ptlSrc.x = 0;
                ptlSrc.y = 0;

                // offscreen to primary
                if(IS_DEV_DEVICEBITMAP(psoSave))
                {
                    ULONG      ulSrcOffset;
                    LONG       lSrcStride;

                    // source is device bitmap
                    ulSrcOffset = (ULONG)((BYTE *)(pdsurf->LinearPtr) - ppdev->pjFrameBufbase);
                    lSrcStride  = pdsurf->LinearStride;

                    ppdev->pfnSetSourceBase(ppdev, ulSrcOffset, lSrcStride);
                    // destination is our primary
                    ppdev->pfnSetDestBase(ppdev,ppdev->ulPrimarySurfaceOffset,ppdev->lDelta);

                    // for stereo case we don't allow bufferflipping and have to handle
                    // access to stereo frontbuffer in a NOT doublepumped case

                    // doublepump normal/overlay or flipped case
                    ppdev->pfnCopyBlt(ppdev, 1, prcl, 0xcccc, &ptlSrc, prcl, NULL);

                    if(   (bOglIsOverlayModeEnabled(ppdev))
                        &&( (ptlSrc.y + lRclHeight(prcl)) < psoSave->sizlBitmap.cy)
                       )
                    {
                        // set src pointer to saved away overlay content
                        ptlSrc.y += lRclHeight(prcl);

                        // destination is overlay frontbuffer
                        ppdev->pfnSetDestBase(ppdev,
                                              ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT],
                                              ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_LEFT]);



                        ASSERT(NULL!=ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt)
                        ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, 1, prcl, 0xcccc, &ptlSrc, prcl, NULL);

                        // if prcl intersects with opengl overlay application, do merge
                        // overlay of this client to desktop
                        bDoMergeBlitOnIntersectingRectangle(ppdev, prcl);
                    }

                    if(   (bOglIsStereoModeEnabled(ppdev))
                        &&( (ptlSrc.y + lRclHeight(prcl)) < psoSave->sizlBitmap.cy)
                       )
                    {
                        // set src pointer to saved away stereo content
                        ptlSrc.y += lRclHeight(prcl);

                        // destination is overlay frontbuffer
                        ppdev->pfnSetDestBase(ppdev,
                                              ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT],
                                              ppdev->singleFrontPitch[OGL_STEREO_BUFFER_RIGHT]);

                        ASSERT( NULL != ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt );
                        ppdev->DoubleBuffer.SavedProcs.pfnCopyBlt(ppdev, 1, prcl, 0xcccc, &ptlSrc, prcl, NULL);
                    }

                }
                else
                // mainmemory to primary
                {
                    // destination is our primary
                    ppdev->pfnSetDestBase(ppdev,ppdev->ulPrimarySurfaceOffset,ppdev->lDelta);
                    // straight (maybe double pumped )mem to screen BLT.
                    ppdev->pfnMemToScreenBlt(ppdev, psoSave, &ptlSrc, prcl);

                    if(   (bOglIsOverlayModeEnabled(ppdev))
                        &&( (ptlSrc.y + lRclHeight(prcl)) < psoSave->sizlBitmap.cy)
                       )
                    {
                        // set src pointer to saved away overlay content
                        ptlSrc.y += lRclHeight(prcl);

                        // destination is overlay frontbuffer
                        ppdev->pfnSetDestBase(ppdev,
                                              ppdev->singleFrontOverlayOffset[OGL_STEREO_BUFFER_LEFT],
                                              ppdev->singleFrontOverlayPitch[OGL_STEREO_BUFFER_LEFT]);
                    
                        ASSERT( NULL != ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenBlt );
                        ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenBlt(ppdev, psoSave, &ptlSrc, prcl);

                        // if prcl intersects with opengl overlay application, do merge
                        // overlay of this client to desktop
                        bDoMergeBlitOnIntersectingRectangle(ppdev, prcl);
                    }

                    if(   (bOglIsStereoModeEnabled(ppdev))
                        &&( (ptlSrc.y + lRclHeight(prcl)) < psoSave->sizlBitmap.cy)
                       )
                    {
                        // set src pointer to saved away overlay content
                        ptlSrc.y += lRclHeight(prcl);

                        // destination is overlay frontbuffer
                        ppdev->pfnSetDestBase(ppdev,
                                              ppdev->singleFrontOffset[OGL_STEREO_BUFFER_RIGHT],
                                              ppdev->singleFrontPitch[OGL_STEREO_BUFFER_RIGHT]);

                        ASSERT(NULL!=ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenBlt)
                        ppdev->DoubleBuffer.SavedProcs.pfnMemToScreenBlt(ppdev, psoSave, &ptlSrc, prcl);

                        // if prcl intersects with opengl overlay application, do merge
                        // overlay of this client to desktop
                        bDoMergeBlitOnIntersectingRectangle(ppdev, prcl);
                    }

                }

                // free it now
                ulRet = (ULONG_PTR)bSaveScreenBitsFree(ppdev, ident, psoSave);
            }
            else
            {
                DISPDBG((0, "DrvSaveScreenBits: tried to restore ident 0x%08x which is unknown", ident));
                ulRet = FALSE;
            }

            break;
        }

        
        // The data identified by ident is no longer needed and can be freed. 
        // The value of prcl is undefined and should not be used. The driver 
        // should not restore the saved rectangle to the display. 
        // DrvSaveScreenBits should return TRUE. 
        case SS_FREE:
        {
            DISPDBG((20,"DrvSaveScreenBits: iMode = SS_FREE"));
            
            // find ident and free it
            if (bLinkedListGetElement(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS, ident, &psoSave))
            {
                bSaveScreenBitsFree(ppdev, ident, psoSave);
            }
            else
            {
                DISPDBG((0, "DrvSaveScreenBits: tried to free ident 0x%08x which is unknown", ident));
            }
            ulRet = TRUE;
            break;
        }

        default:
        {
            ASSERT(SS_SAVE==iMode);
            ulRet = FALSE;
            break;
        }
    }

#endif //#if (NVARCH >= 0x4)
Exit:
    DISPDBG((3,"} DrvSaveScreenBits: 0x%08x, lSavedScreenBits:%d", ulRet,ppdev->lSavedScreenBits));
    return ulRet;
}


//*************************************************************************
// bSaveScreenBitsGarbageCollection
// 
// free and remove all saved ScreenBits data and only keep the given number
// of elements.
//*************************************************************************
BOOL bSaveScreenBitsGarbageCollection(
    IN PPDEV ppdev,
    IN ULONG ulKeep) // number of screenbits to keep.
{
    BOOL bRet = TRUE;

    while (ulKeep < ulLinkedListGetCountOfElements(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS))
    {
        ULONG_PTR ulIdent;
        SURFOBJ   *psoFree;

        
        ulIdent = 0;
        psoFree = NULL;
        if (bLinkedListGetOldestElement(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS, &ulIdent, &psoFree))
        {
            ASSERT(NULL != psoFree);
            ASSERT(0    != ulIdent);

            DISPDBG((0, "bSaveScreenBitsGarbageCollection removing 0x%08x", ulIdent));

            bSaveScreenBitsFree(ppdev, ulIdent, psoFree);
        }
        else
        {
            bRet = FALSE;
            break;
        }
    }
    return (bRet);
}


//*************************************************************************
// bSaveScreenBitsFree
// 
// called to free a ScreenBits element and remove it from the linked list. 
// Is called in SS_FREE and the garbage collection routine.
//*************************************************************************
static BOOL bSaveScreenBitsFree(
    IN PPDEV        ppdev,
    IN ULONG_PTR    ident,   // Linked list identifier of node to free
    IN SURFOBJ     *psoFree) // surface to delete
{
    BOOL  bRet = FALSE;
    HSURF hsurf;

    ASSERT(NULL != ppdev);
    ASSERT(0    != ident);
    ASSERT(NULL != psoFree);

    ASSERT(bLinkedListGetElement(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS, ident, &psoFree));

    hsurf = psoFree->hsurf;

#if 0 // This code is necessary if DrvDeleteDeviceBitmap isn't exported!
// It is tested with 1.09 million popups over three days without
// any leakage.
    if(IS_DEVICEBITMAP(psoFree))
    {
        DrvDeleteDeviceBitmap(psoFree->dhsurf);
    }
#endif
    // NOTE: if hsurf is a device bitmap (created through EngCreateDeviceBitmap)
    //   EngDeleteSurface will call DrvDeleteDeviceBitmap if exported, to destroy 
    //   the pdsurf and poh.
    EngUnlockSurface(psoFree);

    EngDeleteSurface((HSURF)hsurf);

    // remove from linked list
    bLinkedListRemoveElement(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS, ident);

    // remember number of actually stored popups to keep opengl overlay happy
    ppdev->lSavedScreenBits--;

    ASSERT(ppdev->lSavedScreenBits >= 0);
    ASSERT(ppdev->lSavedScreenBits==(LONG)ulLinkedListGetCountOfElements(ppdev->pLinkedListSaveScreenBits, LL_CLASS_SAVESCREENBITS));

    bRet = TRUE;
    return (bRet);
}

