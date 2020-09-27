//*****************************Module*Header************************************
//
// Module Name: NV1LINTO.C
//
// DrvLineTo for S3 driver
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

#include "nv1_32.h"
#include "nv1c_ref.h"
#include "nvsubch.h"
#include "nvalpha.h"


//*****************************************************************************
//
//  Function:   NV1LineTo
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


BOOL NV1LineTo(
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
    DSURF*  pdsurf;
    LONG    xOffset;
    LONG    yOffset;
    BOOL    bRet;

    SURFOBJ*    TempDstSurfObjPtr;
    SURFOBJ*    PuntPtr;

    NvChannel   *nv;
    USHORT      FreeCount;
    BOOL        bMore;
    ULONG       VertexCount;
    POINTFIX    *CurrentVertexPtr;
    POINTFIX    *FirstVertexPtr;
    POINTFIX    *NextVertexPtr;
    POINTFIX    LastVertex;
    ULONG       width,height;
    ULONG       AlphaEnableValue;
    ULONG       PixelDepth;
    PATHDATA    pd;
    ULONG       temp;
    ULONG       LineVisibleFlag;
    ULONG       *DstBits;
    ULONG       *ScreenBitmap;
    ULONG       Rop3;
    ULONG SrcOffset;
    ULONG DstOffset;
    ULONG SrcStride;
    ULONG DstStride;

    ASSERTDD(((mix >> 8) & 0xff) == (mix & 0xff),
             "GDI gave us an improper mix");

    //**************************************************************************
    // Get ptr to physical device.  We should never get a null surface object
    //**************************************************************************

    ppdev = (PDEV*) pso->dhpdev;

    //**************************************************************************
    // Init FreeCount and prepare rectangle engine
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;
    
    //**************************************************************************
    // Default to the original surface object passed in to us
    //**************************************************************************

    TempDstSurfObjPtr = pso;

    //**************************************************************************
    // This 'punt' surface object is used only if we need to use GDI to
    // draw to an offscreen DFB.  Since the primary surface is already
    // a GDI-managed surface, we can kick back any line drawn to the screen.
    //**************************************************************************

    PuntPtr = ppdev->psoPunt;

    //**************************************************************************
    // Pass the surface off to GDI if it's a device bitmap that we've
    // converted to a DIB:
    //**************************************************************************

    pdsurf = (DSURF*) pso->dhsurf;
    if (pdsurf !=0)
        {

        //**********************************************************************
        // Pass the surface off to GDI if it's a device bitmap that we've
        // converted to a DIB:
        //**********************************************************************

        if (pdsurf->dt == DT_DIB)
            {
            //******************************************************************
            // NOTE: We use pdsurf->pso, NOT just pso, because this is
            //       a DFB that exists in system memory.
            //******************************************************************
            ppdev->pfnWaitEngineBusy(ppdev);
            return(EngLineTo(pdsurf->pso, pco, pbo, x1, y1, x2, y2, prclBounds, mix));

            }

        else
            {

            //******************************************************************
            //  Offset values must be zero now, for linear memory management !!!
            //******************************************************************

            ppdev->xOffset = 0;
            ppdev->yOffset = 0;

            }

        }
    else
        {
        //**************************************************************************
        // Reset the 'offscreen' offsets to zero just to be safe
        //**************************************************************************
        ppdev->xOffset = 0;
        ppdev->yOffset = 0;
        }


    //*************************************************************************
    // Only handle line drawing to VRAM.  Send lines drawn
    // to system memory back to GDI.
    //*************************************************************************

    DstBits         = (ULONG *)(pso->pvBits);
    ScreenBitmap    = (ULONG *)(ppdev->pjScreen);

    if ((pdsurf == 0) && (DstBits != ScreenBitmap))
        goto GDI_EngLineToIt;


    //**************************************************************************
    // Only SIMPLE, STRAIGHT lines implemented, the rest go thru GDI
    //
    // If we get to this point, the line is to be drawn onscreen
    // We'll currently use NV hardware to ONLY draw straight integer lines.
    // (Because #1) The line object is not yet implemented, just the solid
    //              rectangle object, so we'll use that to draw only straight lines
    //          #2) Currently don't have time to verify the lines are drawn
    //              exactly correct.  We'll just handle the extremely simple
    //              lines, and let GDI worry about the rest.
    //**************************************************************************

    //**************************************************************************
    // Init the pointer to NV hardware
    //**************************************************************************

    nv = (NvChannel *)ppdev->pjMmBase;

    //**************************************************************************
    // Restore clip rect
    //**************************************************************************
    if (ppdev->NVClipResetFlag)
        {
        while(FreeCount < 2*4)
            FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.free;
        FreeCount -= 2*4;

        nv->subchannel[CLIP_SUBCHANNEL].imageBlackRectangle.SetRectangle.y_x = ((0 << 16) | 0 );
        nv->subchannel[CLIP_SUBCHANNEL].imageBlackRectangle.SetRectangle.height_width = (((ppdev->cyMemory)<<16) | (ppdev->cxMemory));

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }


    //**************************************************************************
    // ONLY handle non-clipped lines or single rectangle clipped lines
    //**************************************************************************

    if ((pco == NULL) || (pco->iDComplexity == DC_TRIVIAL) )
        {

        //**********************************************************************
        // Get pixel depth for the current mode.
        // Then determine the ALPHA_CHANNEL enable value
        //**********************************************************************

        PixelDepth = ppdev->cBitsPerPel;

        AlphaEnableValue = ppdev->AlphaEnableValue;

        while (FreeCount < 2*4)
            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free ;
        FreeCount -=2*4;

        //******************************************************************
        // Convert mix to a Rop3 value.
        //******************************************************************
        Rop3 = (mix & 0xf) - 1;
        Rop3 |= (Rop3 << 4);

        nv->subchannel[ROP_SOLID_SUBCHANNEL].ropSolid.SetRop = Rop3 ;
        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Color = pbo->iSolidColor | AlphaEnableValue;


        //**********************************************************************
        // Only draw line if it's horizontal or vertical
        // Init default values for width and height
        //**********************************************************************

        width=1;
        height=1;

        if (x1==x2)

            {
            //******************************************************************
            // We're dealing with a VERTICAL line
            // Make sure to calculate a positive height and
            // swap the coordinates if necessary
            //******************************************************************

            if (y1 < y2)
                height=y2-y1;
            else if (y1 > y2)
                {
                height=y1-y2;
                y1=y2+1;
                }
            }

        else if (y1==y2)

            {
            //******************************************************************
            // We're dealing with a HORIZONTAL line
            // Make sure to calculate a positive width and
            // swap the coordinates if necessary
            //******************************************************************

            if (x1 < x2)
                width=x2-x1;
            else if (x1 > x2)
                {
                width=x1-x2;
                x1=x2+1;
                }
            }
        else

            {
            //******************************************************************
            // This group of lines contains a NON straight line.
            // Just punt it back to GDI
            //******************************************************************

            //******************************************************************
            // Update global free count
            //******************************************************************

            ppdev->NVFreeCount = FreeCount;

            goto GDI_EngLineToIt;

            }

        //**********************************************************************
        // Use hardware to draw the line
        //**********************************************************************

        while (FreeCount < 2*4)
            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free ;
        FreeCount -=2*4;

        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ( ((y1 + ppdev->yOffset)<<16) | ((x1 + ppdev->xOffset) & 0xffff) );
        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = ( (height << 16) | width );

        //**********************************************************************
        // Update global free count
        //**********************************************************************

        ppdev->NVFreeCount = FreeCount;

        return(TRUE);
        }


    else if (pco->iDComplexity == DC_RECT)

        {
        //**********************************************************************
        // This code is fairly re-dundant, but to squeeze as much performance
        // as possible, we separate it into 2 cases (non-clipped and single-clipped)
        //**********************************************************************
        //**********************************************************************
        // Get pixel depth for the current mode.
        // Then determine the ALPHA_CHANNEL enable value
        //**********************************************************************

        PixelDepth = ppdev->cBitsPerPel;

        AlphaEnableValue = ppdev->AlphaEnableValue;

        while (FreeCount < 2*4)
            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free ;
        FreeCount -=2*4;

        //******************************************************************
        // Convert mix to a Rop3 value.
        //******************************************************************
        Rop3 = (mix & 0xf) - 1;
        Rop3 |= (Rop3 << 4);

        nv->subchannel[ROP_SOLID_SUBCHANNEL].ropSolid.SetRop = Rop3 ;
        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Color = pbo->iSolidColor | AlphaEnableValue;

        //**********************************************************************
        // Default drawing the line
        //**********************************************************************

        LineVisibleFlag = TRUE;

        //**********************************************************************
        // Only draw line if it's horizontal or vertical
        // Init default values for width and height
        //**********************************************************************

        width=1;
        height=1;

        if (x1==x2)

            {
            //******************************************************************
            // We're dealing with a VERTICAL line
            // Make sure to calculate a positive height and
            // swap the coordinates if necessary
            //******************************************************************

            if (y1 < y2)
                height=y2-y1;
            else if (y1 > y2)
                {
                height=y1-y2;
                temp=y1;
                y1=y2+1;
                y2=temp;
                }


            //******************************************************************
            // Check if is line is visible or completely clipped
            //******************************************************************

            if ((x1 < pco->rclBounds.left) || (x1 > pco->rclBounds.right))
                LineVisibleFlag = FALSE;
            else if ((y2 < pco->rclBounds.top) || (y1 > pco->rclBounds.bottom))
                LineVisibleFlag = FALSE;
            else
                {
                //**************************************************************
                // If line is visible, check if it needs to be clipped
                // If it is clipped , then update the height.
                //**************************************************************

                if (y2 > pco->rclBounds.bottom)
                    y2 = pco->rclBounds.bottom;

                if (y1 < pco->rclBounds.top)
                    y1 = pco->rclBounds.top;

                height = y2 - y1;


                }
            }

        else if (y1==y2)

            {
            //******************************************************************
            // We're dealing with a HORIZONTAL line
            // Make sure to calculate a positive width and
            // swap the coordinates if necessary
            //******************************************************************

            if (x1 < x2)
                width=x2-x1;
            else if (x1 > x2)
                {
                width=x1-x2;
                temp=x1;
                x1=x2+1;
                x2=temp;
                }


            //******************************************************************
            // Check if is line is visible or completely clipped
            //******************************************************************

            if ((y1 < pco->rclBounds.top) || (y1 > pco->rclBounds.bottom))
                LineVisibleFlag = FALSE;
            else if ((x2 < pco->rclBounds.left) || (x1 > pco->rclBounds.right))
                LineVisibleFlag = FALSE;
            else
                {
                //**************************************************************
                // If line is visible, check if it needs to be clipped
                // If it is clipped , then update the width.
                //**************************************************************

                if (x2 > pco->rclBounds.right)
                    x2 = pco->rclBounds.right;

                if (x1 < pco->rclBounds.left)
                    x1 = pco->rclBounds.left;

                width = x2 - x1;

                }
            }

        else

            {
            //******************************************************************
            // This group of lines contains a NON straight line.
            // Just punt it back to GDI
            //******************************************************************

            //******************************************************************
            // Update global free count
            //******************************************************************

            ppdev->NVFreeCount = FreeCount;

            goto GDI_EngLineToIt;

            }

        //**********************************************************************
        // Use hardware to draw the line
        //**********************************************************************

        if (LineVisibleFlag == TRUE)
            {
            while (FreeCount < 2*4)
                FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free ;
            FreeCount -=2*4;

            nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ( ((y1+ppdev->yOffset)<<16) | ((x1 + ppdev->xOffset) & 0xffff) );
            nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = ( (height << 16) | width );
            }

        //**********************************************************************
        // Update global free count
        //**********************************************************************

        ppdev->NVFreeCount = FreeCount;

        return(TRUE);
        }


    //**************************************************************************
    // Send all other lines thru GDI
    //**************************************************************************

GDI_EngLineToIt:

    //**************************************************************************
    // Check if destination bitmap is a DFB.  If so, create a temporary surfobj
    // (mapped to the screen) that GDI can write on
    //**************************************************************************
    if (pdsurf !=0 )
        {
             PuntPtr->dhsurf      = 0;                    // Behave like a standard DIB
             if (pdsurf->dt == DT_SCREEN)
                 {
                 PuntPtr->sizlBitmap  = pdsurf->sizl;

                 //*************************************************************
                 // Set linear offset and stride for source and destination buffers
                 //*************************************************************

                 PuntPtr->cjBits  = pdsurf->LinearStride * pdsurf->sizl.cy;
                 PuntPtr->pvBits  = pdsurf->LinearPtr;
                 PuntPtr->pvScan0 = pdsurf->LinearPtr;
                 PuntPtr->lDelta  = pdsurf->LinearStride;

                 PuntPtr->iBitmapFormat = ppdev->iBitmapFormat;

                 }
             else
                 {
                 //*************************************************************
                 // This is a DFB which is stored in main memory as a DIB
                 //*************************************************************
                 PuntPtr->sizlBitmap  = pdsurf->pso->sizlBitmap;
                 PuntPtr->cjBits      = pdsurf->pso->lDelta * pdsurf->pso->sizlBitmap.cy;
                 PuntPtr->pvBits  = pdsurf->pso->pvBits;
                 PuntPtr->pvScan0 = pdsurf->pso->pvScan0;
                 PuntPtr->lDelta  = pdsurf->pso->lDelta;
                 PuntPtr->iBitmapFormat = pdsurf->pso->iBitmapFormat;
                 }

        //**********************************************************************
        // Use the 'wrapper' surface, instead of the original DFB surfobj for GDI
        //**********************************************************************

        TempDstSurfObjPtr       = PuntPtr;
        }


    ppdev->pfnWaitEngineBusy(ppdev);
    return(EngLineTo(TempDstSurfObjPtr, pco, pbo, x1, y1, x2, y2, prclBounds, mix));

    }


