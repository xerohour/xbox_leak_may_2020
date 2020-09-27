//******************************************************************************
//
// Module Name: Lines.c
//
// Contains most of the required GDI line support.  Supports drawing
// lines in short 'strips' when clipping is complex or coordinates
// are too large to be drawn by the line hardware.
//
// Copyright (c) 1990-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"
#include "oglsync.h"

//*****************************************************************************
//
//  Function:   DrvStrokePath
//
//  Routine Description:
//
//              Strokes the path.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//*****************************************************************************

BOOL DrvStrokePath(
    SURFOBJ*   pso,
    PATHOBJ*   ppo,
    CLIPOBJ*   pco,
    XFORMOBJ*  pxo,
    BRUSHOBJ*  pbo,
    POINTL*    pptlBrush,
    LINEATTRS* pla,
    MIX        mix)


    {
    PDEV*   ppdev;
    DSURF*  pdsurfDst;
    ULONG   DstOffset;
    ULONG   DstStride;
    BOOL    result;
    SURFOBJ*    PuntPtr;
    ULONG       *DstBits;
    ULONG       *ScreenBitmap;


    //**************************************************************************
    // Get ptr to physical device.  We should never get a null surface object
    //**************************************************************************

    ppdev = (PDEV*) pso->dhpdev;

    //**************************************************************************
    // Set linear offset and stride for destination buffers
    //**************************************************************************

    pdsurfDst = (DSURF*) pso->dhsurf;    // May be NULL since we created the


    //**************************************************************************
    //  Fixes case where driver is called in full screen DOS during STRESS tests.
    //  In this case, GDI is allowed to call us to update DFBs that are stored in
    //  system memory. 
    //**************************************************************************

    if(!ppdev->bEnabled)
        {
        goto ENG_STROKE_PATH_IT;        
        }


    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowPath(ppdev,pso,ppo,pco);
    }

    
    if (  (pdsurfDst != 0) && (pdsurfDst->dt == DT_SCREEN)  )
        {
        DstOffset = (ULONG)((BYTE *)(pdsurfDst->LinearPtr) - ppdev->pjFrameBufbase);
        DstStride = pdsurfDst->LinearStride;
        }

    else
        {
        DstOffset = ppdev->ulPrimarySurfaceOffset;
        DstStride = ppdev->lDelta;
        }

    //**************************************************************************
    // Check if this is a device managed surface
    //**************************************************************************

    if (pdsurfDst !=0)
    {

        //**********************************************************************
        // Pass the surface off to GDI if it's a device bitmap that we've
        // converted to a DIB:
        //**********************************************************************

        if (pdsurfDst->dt == DT_DIB)
        {
            //******************************************************************
            // NOTE: We use pdsurf->pso, NOT just pso, because this is
            //       a DFB that exists in system memory.
            //******************************************************************
            ppdev->pfnWaitEngineBusy(ppdev);
            return(ppdev->pfnEngStrokePath(pdsurfDst->pso, ppo, pco, pxo, pbo, pptlBrush,
                                           pla, mix));
        }
    }

    //**************************************************************************
    // Only handle line drawing to VRAM.  Send lines to system memory back to GDI.
    //**************************************************************************

    DstBits         = (ULONG *)(pso->pvBits);
    ScreenBitmap    = (ULONG *)(ppdev->pjScreen);
    if ((pdsurfDst == 0) && (DstBits != ScreenBitmap)) 
    {
        goto ENG_STROKE_PATH_IT;
    }

    (ppdev->pfnSetDestBase)(ppdev,DstOffset , DstStride);

    //*************************************************************************
    // Call the chip specific version of DrvStrokePath
    //*************************************************************************

    result =  (ppdev->pfnStrokePath)(pso,ppo,pco,pxo,pbo,pptlBrush,pla,mix)  ;

    return(result);

ENG_STROKE_PATH_IT:

//**************************************************************************
    // This 'punt' surface object is used only if we need to use GDI to
    // draw to an offscreen DFB.  Since the primary surface is already
    // a GDI-managed surface, we can kick back any line drawn to the screen.
    //**************************************************************************

    PuntPtr = pso;

    //**************************************************************************
    // Check if destination bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can write on
    //**************************************************************************
    
    if (pdsurfDst !=0 )
        {
        PuntPtr         = ppdev->psoPunt;
        PuntPtr->dhsurf = 0;                    // Behave like a standard DIB
        
        if (pdsurfDst->dt == DT_SCREEN)
            {
            //*************************************************************
            // Set linear offset and stride for source and destination buffers
            //*************************************************************
            PuntPtr->sizlBitmap  = pdsurfDst->sizl;
            PuntPtr->cjBits  = pdsurfDst->LinearStride * pdsurfDst->sizl.cy;
            PuntPtr->pvBits  = pdsurfDst->LinearPtr;
            PuntPtr->pvScan0 = pdsurfDst->LinearPtr;
            PuntPtr->lDelta  = pdsurfDst->LinearStride;
            PuntPtr->iBitmapFormat = ppdev->iBitmapFormat;
            }
        else
            {
            //*************************************************************
            // This is a DFB which is stored in main memory as a DIB
            //*************************************************************
            PuntPtr->sizlBitmap  = pdsurfDst->pso->sizlBitmap;
            PuntPtr->cjBits      = pdsurfDst->pso->lDelta * pdsurfDst->pso->sizlBitmap.cy;
            PuntPtr->pvBits  = pdsurfDst->pso->pvBits;
            PuntPtr->pvScan0 = pdsurfDst->pso->pvScan0;
            PuntPtr->lDelta  = pdsurfDst->pso->lDelta;
            PuntPtr->iBitmapFormat = pdsurfDst->pso->iBitmapFormat;
            }
        }
    ppdev->pfnWaitEngineBusy(ppdev);
    return(ppdev->pfnEngStrokePath(PuntPtr, ppo, pco, pxo, pbo, pptlBrush, pla, mix));

    }


