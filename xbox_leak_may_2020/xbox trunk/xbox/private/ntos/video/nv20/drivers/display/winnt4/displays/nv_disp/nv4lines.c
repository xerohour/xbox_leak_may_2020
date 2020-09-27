//******************************************************************************
//
// Module Name: NV4LINES.C
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

#include "nv32.h"
#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"

#define PO_ALL_INTEGERS     0x00000004
#define PO_ENUM_AS_INTEGERS 0x00000008

#define POINT_OUTOFBOUNDS(x,y) \
(\
  ((x) ^ ((int)(x) >> 31) |  \
   (y) ^ ((int)(y) >> 31)  ) \
  & 0xFFFF8000               \
)
//*****************************************************************************
// Externs
//*****************************************************************************

extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );


    
//*****************************************************************************
//  Function:     vector_has_tiebreaker_pixels
//  Routine Description: 
//                Checks to see if the line contains pixels which will be
//                affected by tiebreaker rules
//  Arguments:    All coordinates are integer.
//  Return Value: TRUE if tiebreaker pixels exist, FALSE otherwise
//*****************************************************************************
BOOL vector_has_tiebreaker_pixels
(
    LONG x1,
    LONG y1,
    LONG x2,
    LONG y2
)
{
    LONG    delta_x = x2 - x1;
    LONG    delta_y = y2 - y1;

    /*
    ** No tiebreakers in vectors with no minor steps.
    */
    if ((delta_x==0) || (delta_y==0))
        return FALSE;

    /*
    ** Calculate magnitudes of delta_major and delta_minor.
    */
    if (delta_x < 0)
        delta_x = -delta_x;

    if (delta_y < 0)
        delta_y = -delta_y;

    /*
    ** A bresenham pattern repeats "common factors" times over the length of
    ** the vector where "common factors" is the product of the common factors
    ** in delta_x and delta_y.  We don't care about odd common factors because
    ** that case can never create a tiebreaker situation.  We'll refer to this
    ** result of eliminating the 2^n factor as the "2^n reduced slope".
    **
    ** If the "2^n reduced slope" is such that reduced delta_major is EVEN and
    ** reduced delta_minor is ODD then the bresenham will step at least one
    ** case were the pure (not tiebreaker biased) bresenham error will be 0.
    ** 
    ** We don't need the actual reduced delta values.  Since we have already
    ** eliminated the '0' cases, taking the largest 2^n common factor out the
    ** delta_* values results in at least one of them being ODD.  The only way
    ** then for the reduced delta_major to be EVEN and the reduced delta_minor
    ** to be ODD is if delta_major has a larger 2^n factor than delta_minor.
    **
    ** ANDing a number and its 2's complement gives the highest 2^n factor.
    */

    if (delta_x > delta_y)
    {
        // x major vector

        if ((delta_x & -delta_x) > (delta_y & -delta_y))
            return TRUE;
    }
    else
    {
        // y major vector

        if ((delta_y & -delta_y) > (delta_x & -delta_x))
            return TRUE;
    }

    return FALSE;
}

//*****************************************************************************
//
//  Function:   vLinesSimple
//
//  Routine Description:
//
//              Strokes the path.
//
//  Arguments:  All coordinates are integer.
//
//  Return Value:
//
//      None.
//
//*****************************************************************************
VOID vLinesSimple(
PDEV*       ppdev,
PATHOBJ*    ppo,
BRUSHOBJ*  pbo,
RECTL*  prclClip
)
{
    BOOL        bMore;
    PATHDATA    pd;
    LONG        cVertex;
    POINTFIX*   pptfx;
    POINTFIX    ptfxStartFigure = {0, 0};
    ULONG       i;
    DECLARE_DMA_FIFO;
 
    INIT_LOCAL_DMA_FIFO;

    PATHOBJ_vEnumStart(ppo);    
    ppo->fl |= PO_ENUM_AS_INTEGERS;
    
    //**********************************************************
    // Set Point and Size methods for clip:
    //    NV019_SET_POINT
    //    NV019_SET_SIZE
    //**********************************************************
    NV_DMAPUSH_CHECKFREE( ((ULONG)(5)));  
    
    if((prclClip == NULL) && (ppdev->NVClipResetFlag))
    {
        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 
        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
    }
    else if(prclClip != NULL)
    {
        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = (( prclClip->top << 16) |  prclClip->left);
        NV_DMA_FIFO = (((prclClip->bottom - prclClip->top) << 16) 
                    | (prclClip->right - prclClip->left) ); 
        ppdev->NVClipResetFlag=1;       // Clipping rectangle has been reset
    }
    
    
    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_COLOR, pbo->iSolidColor);
    do {
    
        bMore  = PATHOBJ_bEnum(ppo, &pd);
        cVertex = pd.count;
        pptfx  = pd.pptfx;

        //**************************************************************
        // Check if we've got enough room in the push buffer
        //**************************************************************
        
        NV_DMAPUSH_CHECKFREE( ((ULONG)( 4 + cVertex*2 + cVertex / 16 )));  
        
        if (pd.flags & PD_BEGINSUBPATH)
        {
            // New path; can be multiple paths
            ptfxStartFigure.x = pptfx->x;
            ptfxStartFigure.y = pptfx->y;
        
            cVertex--;
            pptfx++;
        }
  
        NV_DMAPUSH_START( 4, SOLID_LIN_SUBCHANNEL, NV05C_LIN32(0));
        NV_DMA_FIFO = ptfxStartFigure.x;
        NV_DMA_FIFO = ptfxStartFigure.y;
        NV_DMA_FIFO = pptfx->x;
        NV_DMA_FIFO = pptfx->y;
        
        if(--cVertex)
            pptfx++;
        
        while(cVertex > 16)
        {
            NV_DMAPUSH_START(32, SOLID_LIN_SUBCHANNEL, NV05C_POLY_LIN32(0));
        
            for(i = 0; i < 16; i++, --cVertex)
            {
                NV_DMA_FIFO = pptfx->x;
                NV_DMA_FIFO = pptfx->y;
        
                pptfx++;
            }
        }
        
        if(cVertex > 0)
        {
            NV_DMAPUSH_START( 2*cVertex, SOLID_LIN_SUBCHANNEL, NV05C_POLY_LIN32(0));
            while (cVertex > 0)
            {
                NV_DMA_FIFO = pptfx->x;
                NV_DMA_FIFO = pptfx->y;
                if(--cVertex)
                    pptfx++;
            }
        }

        if (pd.flags & PD_CLOSEFIGURE)
        {
            pd.flags &= ~PD_CLOSEFIGURE;
            NV_DMAPUSH_START( 2, SOLID_LIN_SUBCHANNEL, NV05C_POLY_LIN32(0));
            NV_DMA_FIFO = ptfxStartFigure.x;
            NV_DMA_FIFO = ptfxStartFigure.y;
        }
        
        if(!bMore) break;
        
        // Multiple paths
        ptfxStartFigure.x = pptfx->x;
        ptfxStartFigure.y = pptfx->y;
            
    } while (1);
    
    //******************************************************************
    // Update global push buffer count
    //******************************************************************
    
    UPDATE_PDEV_DMA_COUNT;

    //******************************************************************
    // Send data on thru to the DMA push buffer
    //******************************************************************

    NV4_DmaPushSend(ppdev);
}


//*****************************************************************************
//
//  Function:   NV4DmaPushStrokePath
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

BOOL NV4DmaPushStrokePath(
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

    BOOL        bMore;
    ULONG       VertexCount;
    POINTFIX    *CurrentVertexPtr;
    POINTFIX    *FirstVertexPtr;
    POINTFIX    *NextVertexPtr;
    POINTFIX    *LastVertexPtr;

    POINTFIX    *CurrPt = NULL;
    POINTFIX    *PrevPt = NULL;
    POINTFIX    *FrstPt = NULL;
    POINTFIX    *LastPt = NULL;

    LONG        x0,y0;
    LONG        x1,y1;
    ULONG       width,height;
    ULONG       AlphaEnableValue;
    PATHDATA    pd;
    ULONG       temp;
    ULONG       LineVisibleFlag;
    ULONG       Rop3;
    ULONG       i;
    LONG        curX, curY;
    BYTE        jClip;
    RECTFX      BoundingRect;

    DECLARE_DMA_FIFO;


    ASSERTDD(((mix >> 8) & 0xff) == (mix & 0xff),
             "GDI gave us an improper mix");

    //**************************************************************************
    // Get ptr to physical device.  We should never get a null surface object
    //**************************************************************************

    ppdev = (PDEV*) pso->dhpdev;

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

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

    pdsurf = (DSURF*) pso->dhsurf;

    //**************************************************************************
    // As of NT 4.0 SP2, the rasterization convention has been loosened
    // for paths that have the PO_ALL_INTEGERS flag set.  For those paths,
    // you may do the 'tie-breaker' pixels anyway you like, which usually
    // means that you can use the point-to-point line drawing capabilities
    // of the hardware.
    //**************************************************************************
    if ((ppo->fl & PO_ALL_INTEGERS) &&      // Points must be integer
        !(pla->fl & LA_STYLED))             // Solid lines, no styles
    {
        CLIPENUM  ce;
    
        //**************************************************************************
        //  Check that the points sent to us are not going to be out
        //  of range for our hardware. Also check that the lines to be drawn
        //  don't have tiebreakers, since hw handles tiebreakers differently from
        //  NT4/W2K gdi.  
        //**************************************************************************
        PATHOBJ_vEnumStart(ppo);
        ppo->fl |= PO_ENUM_AS_INTEGERS;
        do
        {
            pd.flags = 0;
            bMore = PATHOBJ_bEnum(ppo, &pd);
            CurrPt = &(pd.pptfx[0]);
            if (POINT_OUTOFBOUNDS(CurrPt->x,CurrPt->y))
                goto DMAPUSH_GDI_StrokePathIt;


            if (pd.flags & PD_BEGINSUBPATH || LastPt == NULL)
            {
                FrstPt = CurrPt;
                PrevPt = CurrPt;
                CurrPt++;
                pd.count --;
            }
            else
            {
                FrstPt = LastPt;
                PrevPt = LastPt;
            }

            // check first line to get count/ptrs coordinated. There will always be at
            // least one line
            if (vector_has_tiebreaker_pixels(CurrPt->x,CurrPt->y,PrevPt->x,PrevPt->y) ||
                POINT_OUTOFBOUNDS(CurrPt->x,CurrPt->y))
            {
                goto DMAPUSH_GDI_StrokePathIt;
            }

            pd.count --;
            PrevPt = CurrPt;
            while (pd.count)
            {
                CurrPt++;
                if (vector_has_tiebreaker_pixels(CurrPt->x,CurrPt->y,PrevPt->x,PrevPt->y) ||
                    POINT_OUTOFBOUNDS(CurrPt->x,CurrPt->y))
                {
                    goto DMAPUSH_GDI_StrokePathIt;
                }
                PrevPt++;
                pd.count--;
            }

            if (pd.flags & PD_CLOSEFIGURE)
            {
                if (vector_has_tiebreaker_pixels(CurrPt->x,CurrPt->y,FrstPt->x,FrstPt->y))
                {
                    goto DMAPUSH_GDI_StrokePathIt;
                }
            }
            LastPt = CurrPt;

        } while (bMore);

        NV_DMAPUSH_CHECKFREE(( (ULONG) (4) ));  
    
        if(ppdev->dDrawSpareSubchannelObject != DD_IMAGE_ROP_AND_LIN)
        {
            NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NVFFF_SET_OBJECT(0), DD_IMAGE_ROP_AND_LIN);
            ppdev->dDrawSpareSubchannelObject     = DD_IMAGE_ROP_AND_LIN;
        }

        //**************************************************************
        // Reset clipping rectangle to full screen extents if necessary
        // Resetting the clipping rectangle causes delays so we want to
        // do it as little as possible!
        //**************************************************************
    
                                  
        Rop3 = (mix & 0x0f) - 1;
        Rop3 |= (Rop3 << 4);
    
        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);
    
    
        UPDATE_PDEV_DMA_COUNT;
    
        if (pco->iDComplexity == DC_TRIVIAL)
        {
            vLinesSimple(ppdev, ppo, pbo, NULL);
            return(TRUE);
        }
        else
        {
            if (pco->iDComplexity == DC_RECT)
            {
                vLinesSimple(ppdev, ppo, pbo, &pco->rclBounds);
            }
            else
            {
                CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_RIGHTDOWN,
                                   0);

                do {
                    // Get a batch of region rectangles:

                    bMore = CLIPOBJ_bEnum(pco, sizeof(ce), (VOID*) &ce);
                    for (i = 0; i < (ULONG)ce.c; i++)
                    {
                        vLinesSimple(ppdev, ppo, pbo, &ce.arcl[i]);
                    }
                } while (bMore);
            }
            return(TRUE);
        }
        return(TRUE);
    }

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
                            goto DMAPUSH_GDI_StrokePathIt;
                        else
                            {
                            curX = pd.pptfx[i].x >> 4;
                            curY = pd.pptfx[i].y >> 4;
                            }
                    }
                    if (pd.flags & PD_BEGINSUBPATH)
                    {
                        //**********************************************************
                        // Get ptr to coordinates of the first vertex
                        //**********************************************************
                        FirstVertexPtr      = pd.pptfx;
                    }
                    if (pd.flags & PD_CLOSEFIGURE)
                    {
                        if(!((FirstVertexPtr->x >> 4) == curX || (FirstVertexPtr->y >> 4) == curY))
                            goto DMAPUSH_GDI_StrokePathIt;
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
            // Convert mix to a Rop3 value.
            //******************************************************************
            Rop3 = (mix & 0x0f) - 1;
            Rop3 |= (Rop3 << 4);

            NV_DMAPUSH_CHECKFREE(( (ULONG) (7) ));  
                
            NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

            //**************************************************************
            // Reset clipping rectangle to full screen extents if necessary
            // Resetting the clipping rectangle causes delays so we want to
            // do it as little as possible!
            //**************************************************************
    
            if (ppdev->NVClipResetFlag)
                {
                //**********************************************************
                // Set Point and Size methods for clip:
                //    NV019_SET_POINT
                //    NV019_SET_SIZE
                //**********************************************************
    
                NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
                NV_DMA_FIFO = 0;
                NV_DMA_FIFO = ppdev->dwMaxClip; 
    
                ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
                }

            NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL,
                                  NV04A_COLOR1_A,
                                  pbo->iSolidColor);
    
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
                // Check if we've got enough room in the push buffer
                //**************************************************************
    
                NV_DMAPUSH_CHECKFREE(((ULONG)(5 + (VertexCount*4))));  
    
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

                    CurrentVertexPtr    = LastVertexPtr;
                    NextVertexPtr       = pd.pptfx;

                    // We didn't get a BEGIN_SUBPATH.  Since the next PATHDATA
                    // retreived from the PATHOBJ could have only 1 point in it
                    // (a case that occurs in w2k), if we don't inc the VertexCount,
                    // we will skip a line segment (the joint between the lastvertex
                    // from the last PATHDATA and this 1 point) because the linedrawing
                    // loop will immediatly exit (VertexCount will be < 2).
                    
                    VertexCount++;
                    }


                //**************************************************************
                // Exit if there aren't at least 2 vertices
                //**************************************************************

                if (VertexCount < 2)
                    goto DMAPUSH_NV_LinesAreDone;

                //**************************************************************
                //
                // Save a ptr to the last vertex in this record
                //
                // ( there is no chance of getting a dangling reference because
                //  to pointer saved points to an element of the PATHDATA we
                //  are currently processing. In the next fetch from the PATHOBJ
                // (with PATHOBJ_bEnum), we will get a new PATHDATA, and the 
                // pointer will be invalid, but is not used before we get here
                // again. (And when we get here, we make the ptr valid) )
                //**************************************************************

                
                LastVertexPtr = &( pd.pptfx[ pd.count - 1 ] );


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
                        goto DMAPUSH_GDI_StrokePathIt;

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
                        goto DMAPUSH_GDI_StrokePathIt;

                        }

                    //**********************************************************
                    // Time to output a straight line if visible
                    //**********************************************************

                    if (LineVisibleFlag == TRUE)
                        {
                        DISPDBG((6,"x,y: %x %x h,w: %x %x", x0, y0, height, width));

                        //******************************************************
                        // Set Point and Size methods for rectangle:
                        //******************************************************

                        NV_DMAPUSH_START (2, RECT_AND_TEXT_SUBCHANNEL,
                                          NV04A_UNCLIPPED_RECTANGLE_POINT(0))

                        NV_DMA_FIFO = (x0 << 16) | (y0 & 0xffff);
                        NV_DMA_FIFO = ( width << (16) | height );
                        
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

                    x0 = LastVertexPtr->x;
                    y0 = LastVertexPtr->y;

                    x1 = FirstVertexPtr->x;
                    y1 = FirstVertexPtr->y;

                    //**********************************************************
                    // Send to GDI if we get a non-integer line
                    //**********************************************************

                    if (((x0 | y0 | x1 | y1) & 0xf) != 0)
                        goto DMAPUSH_GDI_StrokePathIt;

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
                        goto DMAPUSH_GDI_StrokePathIt;

                        }


                    //**********************************************************
                    // Time to output a straight line if visible
                    //**********************************************************

                    if (LineVisibleFlag == TRUE)
                        {
                        DISPDBG((6,"x,y: %x %x h,w: %x %x", x0, y0, height, width));

                        //******************************************************
                        // Set Point and Size methods for rectangle:
                        //******************************************************

                        NV_DMAPUSH_START (2, RECT_AND_TEXT_SUBCHANNEL,
                                          NV04A_UNCLIPPED_RECTANGLE_POINT(0))

                        NV_DMA_FIFO = (x0 << 16) | (y0 & 0xffff);
                        NV_DMA_FIFO = ( width << (16) | height );
                        
                        }

                    }

                } while (bMore) ;

DMAPUSH_NV_LinesAreDone:
            //******************************************************************
            // Update global push buffer count
            //******************************************************************

            UPDATE_PDEV_DMA_COUNT;
            
            //******************************************************************
            // Send data on thru to the DMA push buffer
            //******************************************************************

            NV4_DmaPushSend(ppdev);

            return(TRUE);

            } // Only handle non-clipped lines or lines clipped to a single rectangle


        } // Only handle solid-lines


    //**************************************************************************
    // Send all other lines thru GDI
    //**************************************************************************

DMAPUSH_GDI_StrokePathIt:

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
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;
    
    NV4_DmaPushSend(ppdev);

    ppdev->pfnWaitEngineBusy(ppdev);
    return(EngStrokePath(TempDstSurfObjPtr, ppo, pco, pxo, pbo, pptlBrush,
                             pla, mix));


    }

