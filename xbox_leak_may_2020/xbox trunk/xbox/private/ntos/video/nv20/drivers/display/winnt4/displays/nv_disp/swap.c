/*
** Copyright 1991, Silicon Graphics, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Revision: 1.53 $
** $Date: 1993/06/03 17:06:13 $
*/

#include "precomp.h"
#include "driver.h"

#include "excpt.h"
#pragma hdrstop
#include "oglDD.h"
#include "oglsync.h"

BOOL APIENTRY DrvSwapBuffers(SURFOBJ *psoDst, WNDOBJ *pwo)
{
    PDEV  *ppdev = NULL;
    NV_OPENGL_CLIENT_INFO *clientInfo = NULL;
    NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo = NULL;
    POINTL  ptlSrc;
    RECTL   prclDst;
    RECTL   rcl;
    LONG                       status;

    ppdev = (PDEV *)pwo->psoOwner->dhpdev;

    if (!ppdev->bEnabled)
    {
        DBG_ERROR("DrvSwapBuffers called in VGA mode -> ignoring");
        return TRUE;
    }

    //**************************************************************
    // Actually do the swap
    //**************************************************************
    try // just in case...
    {
        clientInfo = (NV_OPENGL_CLIENT_INFO *)pwo->pvConsumer;
        if (OglFindClientListFromClientInfo(ppdev, clientInfo) != NULL)
        {
            clientDrawableInfo = clientInfo->clientDrawableInfo;

            // if the swap was already done by the client, don't do it again
            if (clientDrawableInfo->ulSwapDoneByClient)
            {
                // nothing to do, swap taken care of by ICD
                clientDrawableInfo->ulSwapDoneByClient = 0;
                return TRUE;
            }

            if (OglValidateDrawableInfo(clientDrawableInfo) == TRUE)
            {
                ULONG ulView; // stereo loop enumerator

                // If the window is not visible, stop
                if (clientDrawableInfo->clipFlags == NV_WINDOW_CLIP_OCCLUDED ||
                    clientDrawableInfo->numClipRects == 0)
                {
                    return TRUE;
                }

                // Currently only handle display driver channel swaps with UBB
                // enabled.
                if (!ppdev->bOglSingleBackDepthCreated) 
                {
                    return TRUE;
                }

                //************************************************************************** 
                // Need to do smart sync between OpenGL and GDI depending on 
                // clip intersection.
                //**************************************************************************
                bOglSyncOglIfGdiRenderedToWindow(ppdev);

                //**************************************************
                // We now have screen relative clips for the swap
                //**************************************************
                rcl.left   = 0;
                rcl.top    = 0;
                rcl.right  = ppdev->cxScreen;
                rcl.bottom = ppdev->cyScreen;
                ptlSrc.x   = 0;
                ptlSrc.y   = 0;

                // need to wait until ICD has finished rendering
                ASSERT(NULL!=ppdev->pfnWaitEngineBusy);
                ppdev->pfnWaitEngineBusy(ppdev);

                for( ulView=0; ulView < ppdev->ulOglActiveViews; ulView++ )
                {
                    ULONG ulPrimarySurfaceOffset;
                    LONG  lPrimarySurfacePitch;

                    if (OGL_STEREO_BUFFER_LEFT == ulView)
                    {
                        ulPrimarySurfaceOffset = ppdev->ulPrimarySurfaceOffset;
                        lPrimarySurfacePitch   = ppdev->lDelta;
                    }
                    else
                    {
                        ulPrimarySurfaceOffset = ppdev->singleFrontOffset[ulView];
                        lPrimarySurfacePitch   = ppdev->singleFrontPitch[ulView];
                    }

                    // perhaps previous swaps succeeded in the ICD but we're displaying back,
                    // then we need to blit from primary to back
                    if (OGL_FLIPPED())
                    {
                        ppdev->pfnSetSourceBase(ppdev, ulPrimarySurfaceOffset, lPrimarySurfacePitch);
                        ppdev->pfnSetDestBase(ppdev, ppdev->singleBack1Offset[ulView], ppdev->singleBack1Pitch[ulView]);
                    }
                    else
                    {
                        ppdev->pfnSetSourceBase(ppdev, ppdev->singleBack1Offset[ulView], ppdev->singleBack1Pitch[ulView]);
                        ppdev->pfnSetDestBase(ppdev, ulPrimarySurfaceOffset, lPrimarySurfacePitch);
                    }
                    ppdev->pfnCopyBlt(ppdev, 
                                      clientDrawableInfo->numClipRects,
                                      clientDrawableInfo->copyWindowRect, 
                                      0xCCCC, &ptlSrc, &rcl, NULL);
                }
                ppdev->pfnWaitEngineBusy(ppdev);
            }
            else
            {
                DISPDBG((1, "DrvSwapBuffers: BOGUS clientDrawableInfo"));
            }
        }
        else
        {
            DISPDBG((1, "DrvSwapBuffers: BOGUS clientInfo"));
        }
    }
    except(EXCEPTION_EXECUTE_HANDLER)
    {
        status = GetExceptionCode();
        DISPDBG((1, "DrvSwapBuffers: WOC_RGN_CLIENT: exception = 0x%lx", status));
    }

    return TRUE;
}

