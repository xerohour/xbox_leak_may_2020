//*****************************Module*Header************************************
// 
// Module Name: Lineto.c
// 
// DrvLineTo for NV driver
// 
// Copyright (c) 1995-1996 Microsoft Corporation
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
//  Function:   DrvLineTo
//  
//  Routine Description:
//  
//          Draws a single solid integer-only cosmetic line.
//
//  Arguments:
//  
//  Return Value:
//  
//      None.
//  
//*****************************************************************************


BOOL DrvLineTo(
SURFOBJ*    pso,
CLIPOBJ*    pco,
BRUSHOBJ*   pbo,
LONG        x1,
LONG        y1,
LONG        x2,
LONG        y2,
RECTL*      prclBounds,
MIX         mix)

    {
    PDEV*   ppdev;
    DSURF*  pdsurfDst;
    ULONG   DstOffset;
    ULONG   DstStride;
    BOOL    result;
    SURFOBJ*    PuntPtr;

    //**************************************************************************
    // Get ptr to physical device.  We should never get a null surface object
    //**************************************************************************

    ppdev = (PDEV*) pso->dhpdev;

    //**************************************************************************
    // Set linear offset and stride for source and destination buffers
    //**************************************************************************

    pdsurfDst = (DSURF*) pso->dhsurf;    // May be NULL since we created the

#if _WIN32_WINNT >= 0x0500

    //**************************************************************************
    //  Fixes case where driver is called in full screen DOS during STRESS tests.
    //  In this case, GDI is allowed to call us to update DFBs that are stored in
    //  system memory. 
    //**************************************************************************

        if(!ppdev->bEnabled)
            goto DRV_LINE_TO_IT;
#endif

    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowRect(ppdev,pso,prclBounds,pco);
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
    
    (ppdev->pfnSetDestBase)(ppdev,DstOffset , DstStride);


    //*************************************************************************
    // Call the chip specific version of DrvLineTo
    //*************************************************************************

    result =  (ppdev->pfnLineTo)(pso,pco,pbo,x1,y1,x2,y2,prclBounds,mix)  ;

    return(result);

#if _WIN32_WINNT >= 0x0500

DRV_LINE_TO_IT:

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
            PuntPtr->sizlBitmap     = pdsurfDst->pso->sizlBitmap;
            PuntPtr->cjBits         = pdsurfDst->pso->lDelta * pdsurfDst->pso->sizlBitmap.cy;
            PuntPtr->pvBits         = pdsurfDst->pso->pvBits;
            PuntPtr->pvScan0        = pdsurfDst->pso->pvScan0;
            PuntPtr->lDelta         = pdsurfDst->pso->lDelta;
            PuntPtr->iBitmapFormat  = pdsurfDst->pso->iBitmapFormat;
            }
        }
    ppdev->pfnWaitEngineBusy(ppdev);    
    return(ppdev->pfnEngLineTo(PuntPtr, pco, pbo, x1, y1, x2, y2, prclBounds, mix));
#endif

    }
    
    

