//******************************************************************************
//
// Module Name: NV1LINES.C
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
#include "nv1_32.h"
#include "driver.h"
#include "nv1c_ref.h"
#include "nvsubch.h"
#include "nvalpha.h"


//*****************************************************************************
//
//  Function:   NV1StrokePath
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

BOOL NV1StrokePath(
    SURFOBJ*   pso,
    PATHOBJ*   ppo,
    CLIPOBJ*   pco,
    XFORMOBJ*  pxo,
    BRUSHOBJ*  pbo,
    POINTL*    pptlBrush,
    LINEATTRS* pla,
    MIX        mix)

    {
    PDEV*       ppdev;
    DSURF*      pdsurf;
    RECTL       arclClip[4];                  // For rectangular clipping

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
    LONG        x0,y0;
    LONG        x1,y1;
    ULONG       width,height;
    ULONG       AlphaEnableValue;
    PATHDATA    pd;
    ULONG       temp;
    ULONG       LineVisibleFlag;
    ULONG       *DstBits;
    ULONG       *ScreenBitmap;
    ULONG       Rop3;
    ULONG       i;
    LONG        curX, curY;
    BYTE        jClip;


    ASSERTDD(((mix >> 8) & 0xff) == (mix & 0xff),
             "GDI gave us an improper mix");

    //**************************************************************************
    // Get ptr to physical device.  We should never get a null surface object
    //**************************************************************************

    ppdev = (PDEV*) pso->dhpdev;

    //**************************************************************************
    // Get the FreeCount
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
    // Check if this is a device managed surface
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
            return(EngStrokePath(pdsurf->pso, ppo, pco, pxo, pbo, pptlBrush,
                             pla, mix));
            }
        }

    //**************************************************************************
    // Only handle line drawing to VRAM.  Send lines to system memory back to GDI.
    //**************************************************************************

    DstBits         = (ULONG *)(pso->pvBits);
    ScreenBitmap    = (ULONG *)(ppdev->pjScreen);

    if ((pdsurf == 0) && (DstBits != ScreenBitmap))
        goto GDI_StrokePathIt;

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
    // ONLY handle solid single pixel wide lines
    // Line Style is solid if pstyle array ptr is NULL
    //**************************************************************************

    if (pco == NULL)
        jClip = DC_TRIVIAL;
    else
        jClip = pco->iDComplexity;

    if ((pla->pstyle == (FLOAT_LONG*) NULL) && !(pla->fl & (LA_GEOMETRIC | LA_ALTERNATE)))
        {

        //**********************************************************************
        // ONLY handle non-clipped lines or single rectangle clipped lines
        //**********************************************************************

        if ((jClip == DC_TRIVIAL) || (jClip == DC_RECT))
            {
            //******************************************************************
            // If this is a mix which uses the destination, then we cannot punt to GDI
            // in the middle of a path enumeration since GDI will draw the same lines
            // which we have already drawn. So we MUST prescan the list of lines in the
            // path to ensure that none of the vertices in the path have a fractional
            // component, and that ALL of the lines are either horiz or vert. For now
            // we'll just check for a SRCCOPY mix.
            //******************************************************************
            if ((mix & 0xf) != R2_COPYPEN)
                {
                PATHOBJ_vEnumStart(ppo);
                do
                    {
                    pd.flags = 0;
                    bMore = PATHOBJ_bEnum(ppo, &pd);
                    curX = pd.pptfx[0].x >> 4;
                    curY = pd.pptfx[0].y >> 4;

                    for (i = 0; i < pd.count; i++)
                        {
                        if ((((pd.pptfx[i].x | pd.pptfx[i].y) & 0xf) != 0) ||
                            !((pd.pptfx[i].x >> 4) == curX || (pd.pptfx[i].y >> 4) == curY))
                            goto GDI_StrokePathIt;
                        else
                            {
                            curX = pd.pptfx[i].x >> 4;
                            curY = pd.pptfx[i].y >> 4;
                            }

                        }
                    } while (bMore);
                }

            //******************************************************************
            // First, copy the clipping rectangle if it's present
            //******************************************************************

            if (jClip == DC_RECT)
                {
                arclClip[0] = pco->rclBounds;
                }

            //******************************************************************
            // Check free count and prepare rectangle engine
            //******************************************************************

            while (FreeCount < 2*4)
                FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free ;
            FreeCount -=2*4;

            //******************************************************************
            // Convert mix to a Rop3 value.
            //******************************************************************
            Rop3 = (mix & 0x0f) - 1;
            Rop3 |= (Rop3 << 4);

            nv->subchannel[ROP_SOLID_SUBCHANNEL].ropSolid.SetRop = Rop3;
            nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Color = pbo->iSolidColor;

            //**************************************************************************
            // Reset clipping rectangle to full screen extents if necessary
            // Resetting the clipping rectangle causes delays so we want to do it
            // as little as possible!
            //**************************************************************************
            if (ppdev->NVClipResetFlag)
                {
                while (FreeCount < 2*4)
                    FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.free;
                FreeCount -= 2*4;

                nv->subchannel[CLIP_SUBCHANNEL].imageBlackRectangle.SetRectangle.y_x = ((0 << 16) | 0 );
                nv->subchannel[CLIP_SUBCHANNEL].imageBlackRectangle.SetRectangle.height_width = (((ppdev->cyMemory)<<16) | (ppdev->cxMemory));

                ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
                }

            PATHOBJ_vEnumStart(ppo);
            do  {

                pd.flags = 0;

                //**************************************************************
                // Get the current group of lines
                //**************************************************************

                bMore = PATHOBJ_bEnum(ppo, &pd);

                //**************************************************************
                // Get number of vertices in this record.
                //**************************************************************

                VertexCount = pd.count;

                //**************************************************************
                // Check to see if we need to 'reset' the starting vertex
                //**************************************************************

                if (pd.flags & PD_BEGINSUBPATH)
                    {
                    //**********************************************************
                    // Get ptr to coordinates of the first vertex
                    //**********************************************************

                    CurrentVertexPtr    = pd.pptfx;
                    NextVertexPtr       = pd.pptfx+1;
                    FirstVertexPtr      = pd.pptfx;
                    }

                else

                    {
                    //**********************************************************
                    // Get coordinates of first vertex.
                    // Since we didn't get a BEGIN_SUBPATH notification,
                    // use the last vertex from the last line record
                    // as the first vertex.
                    //**********************************************************

                    CurrentVertexPtr    = &LastVertex;
                    NextVertexPtr       = pd.pptfx;
                    FirstVertexPtr      = &LastVertex;
                    }

                //**************************************************************
                // If we DIDN'T get a BEGIN_SUBPATH notification,
                // do we need to increment VertexCount???
                // We'll be using LastVertex, which isn't part of the
                // current list of lines???
                //**************************************************************

//           VertexCount++;

                //**************************************************************
                // Exit if there aren't at least 2 vertices
                //**************************************************************

                if (VertexCount < 2)
                    goto NV_LinesAreDone;

                //**************************************************************
                // Copy the last vertex in this record
                //**************************************************************

                LastVertex = pd.pptfx[pd.count - 1];

                //**************************************************************
                // Need a minimum of 2 vertices per line
                //**************************************************************

                do  {
                    //**********************************************************
                    // Check each line and see if it's an integer and straight
                    // Get the first PATHDATA record
                    //**********************************************************

                    x0 = CurrentVertexPtr->x;
                    y0 = CurrentVertexPtr->y;

                    x1 = NextVertexPtr->x;
                    y1 = NextVertexPtr->y;

                    //**********************************************************
                    // Send to GDI if we get a non-integer line
                    //**********************************************************

                    if (((x0 | y0 | x1 | y1) & 0xf) != 0)
                        goto GDI_StrokePathIt;

                    //**********************************************************
                    // Get rid of fixed fractional component
                    //**********************************************************

                    x0 >>=4;
                    y0 >>=4;
                    x1 >>=4;
                    y1 >>=4;

                    //**********************************************************
                    // Default to always drawing the line
                    //**********************************************************

                    LineVisibleFlag = TRUE;

                    //**********************************************************
                    // Only draw line if it's horizontal or vertical
                    //**********************************************************

                    if (x0==x1)

                        {
                        //******************************************************
                        // We're dealing with a VERTICAL line
                        // Make sure to calculate a positive height and
                        // swap the coordinates if necessary
                        //******************************************************

                        width=1;
                        if (y0 > y1)
                            {
                            temp=y0;
                            y0=y1+1;
                            y1=temp+1;
                            }

                        //******************************************************
                        // Vertical Line Clipping:
                        // At this point, we have the following variables to
                        // work with:       x,y,  and  width,height
                        //
                        // Now we can do single rectangle clipping very easily
                        //******************************************************

                        if (jClip == DC_RECT)
                            {

                            //**************************************************
                            // Check if is line is visible or completely clipped
                            //**************************************************

                            if ((x0 < arclClip[0].left) || (x0 >= arclClip[0].right) ||
                                (y1 <= arclClip[0].top) || (y0 >= arclClip[0].bottom))
                                LineVisibleFlag = FALSE;
                            else
                                {
                                //**********************************************
                                // If line is visible, check if it needs to be clipped
                                // If it is clipped , then update the height.
                                //**********************************************

                                if (y1 > arclClip[0].bottom)
                                    y1 = arclClip[0].bottom;

                                if (y0 < arclClip[0].top)
                                    y0 = arclClip[0].top;
                                }
                            }
                        height = y1 - y0;
                        }

                   else if (y0==y1)

                        {
                        //******************************************************
                        // We're dealing with a HORIZONTAL line
                        // Make sure to calculate a positive width and
                        // swap the coordinates if necessary
                        //******************************************************

                        height=1;
                        if (x0 > x1)
                            {
                            temp=x0;
                            x0=x1+1;
                            x1=temp+1;
                            }

                        //******************************************************
                        // Horizontal Line Clipping:
                        // At this point, we have the following variables to
                        // work with:       x,y,  and  width,height
                        //
                        // Now we can do single rectangle clipping very easily
                        //******************************************************

                        if (jClip == DC_RECT)
                            {

                            //**************************************************
                            // Check if is line is visible or completely clipped
                            //**************************************************

                            if ((y0 < arclClip[0].top) || (y0 >= arclClip[0].bottom) ||
                                (x1 <= arclClip[0].left) || (x0 >= arclClip[0].right))
                                LineVisibleFlag = FALSE;
                            else
                                {
                                //**********************************************
                                // If line is visible, check if it needs to be clipped
                                // If it is clipped , then update the width.
                                //**********************************************

                                if (x1 > arclClip[0].right)
                                    x1 = arclClip[0].right;

                                if (x0 < arclClip[0].left)
                                    x0 = arclClip[0].left;

                                }
                            }
                        width = x1 - x0;
                        }

                    else

                        {
                        //******************************************************
                        // This group of lines contains a NON straight line.
                        // Just punt it back to GDI
                        //******************************************************
                        goto GDI_StrokePathIt;

                        }

                    //**********************************************************
                    // Time to output a straight line if visible
                    //**********************************************************

                    if (LineVisibleFlag == TRUE)
                        {
                        DISPDBG((6,"x,y: %lx %lx h,w: %lx %lx", x0, y0, height, width));
                        while (FreeCount < 2*4)
                            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free ;
                        FreeCount -=2*4;

                        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ( (y0<<16) | (x0 & 0xffff) );
                        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = ( (height << 16) | (width & 0xffff) );

                        //******************************************************
                        // Advance to next line (must have at least 2 vertices)
                        //******************************************************
                        }


                    VertexCount--;

                    CurrentVertexPtr = NextVertexPtr;
                    NextVertexPtr++;

                    } while (VertexCount >= 2);

                //**************************************************************
                // Draw closing line if specified and if it's straight
                //**************************************************************

                if (pd.flags & PD_CLOSEFIGURE)
                    {

                    x0 = LastVertex.x;
                    y0 = LastVertex.y;

                    x1 = FirstVertexPtr->x;
                    y1 = FirstVertexPtr->y;

                    //**********************************************************
                    // Send to GDI if we get a non-integer line
                    //**********************************************************

                    if (((x0 | y0 | x1 | y1) & 0xf) != 0)
                        goto GDI_StrokePathIt;

                    //**********************************************************
                    // Get rid of fixed fractional component
                    //**********************************************************

                    x0 >>=4;
                    y0 >>=4;
                    x1 >>=4;
                    y1 >>=4;

                    //**********************************************************
                    // Default to always drawing the closing line
                    //**********************************************************

                    LineVisibleFlag = TRUE;

                    //**********************************************************
                    // Make sure line is straight!
                    //**********************************************************

                    if (x0==x1)

                        {
                        //******************************************************
                        // We're dealing with a VERTICAL line
                        // Make sure to calculate a positive height and
                        // swap the coordinates if necessary
                        //******************************************************

                        width=1;
                        if (y0 > y1)
                            {
                            temp=y0;
                            y0=y1+1;
                            y1=temp+1;
                            }

                        //******************************************************
                        // Vertical Line Clipping:
                        // At this point, we have the following variables to
                        // work with:       x,y,  and  width,height
                        //
                        // Now we can do single rectangle clipping very easily
                        //******************************************************

                        if (jClip == DC_RECT)
                            {

                            //**************************************************
                            // Check if is line is visible or completely clipped
                            //**************************************************

                            if ((x0 < arclClip[0].left) || (x0 >= arclClip[0].right) ||
                                (y1 <= arclClip[0].top) || (y0 >= arclClip[0].bottom))
                                LineVisibleFlag = FALSE;
                            else
                                {
                                //**********************************************
                                // If line is visible, check if it needs to be clipped
                                // If it is clipped , then update the height.
                                //**********************************************

                                if (y0 < arclClip[0].top)
                                    y0 = arclClip[0].top;

                                if (y1 > arclClip[0].bottom)
                                    y1 = arclClip[0].bottom;
                                }
                            }
                        height = y1 - y0;
                        }

                   else if (y0==y1)

                        {
                        //******************************************************
                        // We're dealing with a HORIZONTAL line
                        // Make sure to calculate a positive width and
                        // swap the coordinates if necessary
                        //******************************************************

                        height=1;
                        if (x0 > x1)
                            {
                            temp=x0;
                            x0=x1+1;
                            x1=temp+1;
                            }

                        //******************************************************
                        // Horizontal Line Clipping:
                        // At this point, we have the following variables to
                        // work with:       x,y,  and  width,height
                        //
                        // Now we can do single rectangle clipping very easily
                        //******************************************************

                        if (jClip == DC_RECT)
                            {

                            //**************************************************
                            // Check if is line is visible or completely clipped
                            //**************************************************

                            if ((y0 < arclClip[0].top) || (y0 >= arclClip[0].bottom) ||
                                (x1 <= arclClip[0].left) || (x0 >= arclClip[0].right))
                                LineVisibleFlag = FALSE;
                            else
                                {
                                //**********************************************
                                // If line is visible, check if it needs to be clipped
                                // If it is clipped , then update the width.
                                //**********************************************

                                if (x0 < arclClip[0].left)
                                    x0 = arclClip[0].left;

                                if (x1 > arclClip[0].right)
                                    x1 = arclClip[0].right;
                                }
                            }
                        width = x1 - x0;
                        }


                    else

                        {
                        //******************************************************
                        // This group of lines contains a NON straight line.
                        // Just punt it back to GDI
                        //******************************************************
                        goto GDI_StrokePathIt;

                        }


                    //**********************************************************
                    // Time to output a straight line if visible
                    //**********************************************************

                    if (LineVisibleFlag == TRUE)
                        {
                        DISPDBG((6,"x,y: %lx %lx h,w: %lx %lx", x0, y0, height, width));
                        while (FreeCount < 2*4)
                            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free ;
                        FreeCount -=2*4;
                        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ( (y0<<16) | (x0 & 0xffff) );
                        nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = ( (height << 16) | (width & 0xffff) );
                        }

                    }

                } while (bMore) ;

NV_LinesAreDone:
            //******************************************************************
            // Update global free count
            //******************************************************************

            ppdev->NVFreeCount = FreeCount;

            return(TRUE);

            } // Only handle non-clipped lines or lines clipped to a single rectangle


        } // Only handle solid-lines


    //**************************************************************************
    // Send all other lines thru GDI
    //**************************************************************************

GDI_StrokePathIt:

    if (pdsurf !=0)

        {

        //******************************************************************
        // This is an offscreen DFB (DT_SCREEN).  Let GDI draw the line
        // using the wrapper 'punt' surface object
        //******************************************************************

        PuntPtr->dhsurf = 0;               // Behave like a standard DIB
        PuntPtr->sizlBitmap = pdsurf->sizl;

        //******************************************************************
        // Set linear offset and stride for source and destination buffers
        //******************************************************************

        PuntPtr->cjBits  = pdsurf->LinearStride * pdsurf->sizl.cy;
        PuntPtr->pvBits  = pdsurf->LinearPtr;
        PuntPtr->pvScan0 = pdsurf->LinearPtr;
        PuntPtr->lDelta  = pdsurf->LinearStride;

        PuntPtr->iBitmapFormat = ppdev->iBitmapFormat;

        TempDstSurfObjPtr = PuntPtr;

        }

    //**************************************************************************
    // Update global free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;

    ppdev->pfnWaitEngineBusy(ppdev);
    return(EngStrokePath(TempDstSurfObjPtr, ppo, pco, pxo, pbo, pptlBrush,
                             pla, mix));


    }




