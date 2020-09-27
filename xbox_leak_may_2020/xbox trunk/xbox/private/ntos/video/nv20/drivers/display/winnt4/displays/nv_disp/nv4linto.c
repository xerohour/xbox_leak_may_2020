//*****************************Module*Header************************************
// 
// Module Name: NV4LINTO.C
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

#include "nv32.h"
#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"

//*****************************************************************************
// Externs
//*****************************************************************************

extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );

// The our hardware clipping coordinates are limited to 16-bit signed values:



/******************************Public*Routine******************************\
* VOID vLineToTrivial
*
* Draws a single solid integer-only unclipped cosmetic line.
*
\**************************************************************************/

VOID vLineToTrivial(
PDEV*       ppdev,
LONG        x1,              // Passed in x1
LONG        y1,              // Passed in y1
LONG        x2,             // Passed in x2
LONG        y2,             // Passed in y2
ULONG       iSolidColor,
MIX         mix)
{
    ULONG       Rop3;
    
    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************
    
    INIT_LOCAL_DMA_FIFO;
    
    NV_DMAPUSH_CHECKFREE(( (ULONG) (11) ));  

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

    NV_DMAPUSH_WRITE1(SOLID_LIN_SUBCHANNEL, NV05C_COLOR, iSolidColor);

    NV_DMAPUSH_START( 4, SOLID_LIN_SUBCHANNEL, NV05C_LIN32(0));
    NV_DMA_FIFO = x1;
    NV_DMA_FIFO = y1;
    NV_DMA_FIFO = x2;
    NV_DMA_FIFO = y2;
    
    //******************************************************************
    // Update global push buffer count
    //******************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //******************************************************************
    // Send data on thru to the DMA push buffer
    //******************************************************************

    NV4_DmaPushSend(ppdev);
    
    return;
}



//*****************************************************************************
//
//  Function:   NV4DmaPushLineTo
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


BOOL NV4DmaPushLineTo(
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
    SURFOBJ*    TempDstSurfObjPtr;
    SURFOBJ*    PuntPtr;
    
    PDEV*       ppdev;
    DSURF*      pdsurf;
    ULONG       *DstBits;
    ULONG       *ScreenBitmap;
    RECTL*      prclClip;
    ULONG       ulHeight, ulWidth;
    BOOL        bMore;
    CLIPENUM    ce;
    LONG        c;

    
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
        }

    //*************************************************************************
    // Only handle line drawing to VRAM.  Send lines drawn
    // to system memory back to GDI.
    //*************************************************************************

    DstBits         = (ULONG *)(pso->pvBits);
    ScreenBitmap    = (ULONG *)(ppdev->pjScreen);

    if ((pdsurf == 0) && (DstBits != ScreenBitmap))
        goto DMAPUSH_GDI_EngLineToIt;

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    ulHeight = prclBounds->bottom - prclBounds->top;
    ulWidth = prclBounds->right - prclBounds->left;
    
    if ((prclBounds->left   < MIN_INTEGER_BOUND) ||
        (prclBounds->top    < MIN_INTEGER_BOUND) ||
        (ulHeight           > MAX_INTEGER_BOUND) ||
        (ulWidth            > MAX_INTEGER_BOUND))
        {
        goto DMAPUSH_GDI_EngLineToIt;
        }
    if (pco == NULL)
        {
        if (ppdev->NVClipResetFlag)
            {
            NV_DMAPUSH_CHECKFREE(( (ULONG) (3) ));  
            NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
            NV_DMA_FIFO = 0;
            NV_DMA_FIFO = ppdev->dwMaxClip; 
            ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
            }
        
        UPDATE_PDEV_DMA_COUNT;
        vLineToTrivial(ppdev, x1, y1, x2, y2, pbo->iSolidColor, mix);
        
        return(TRUE);
        }
    else if (pco->iDComplexity <= DC_RECT) 
        {
        prclClip = &pco->rclBounds;
        ulHeight = prclClip->bottom - prclClip->top;
        ulWidth = prclClip->right - prclClip->left;

        NV_DMAPUSH_CHECKFREE(( (ULONG) (3) ));  
        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = (( prclClip->top << 16) |  prclClip->left);
        NV_DMA_FIFO = (( ulHeight << 16) | ulWidth ); 
        UPDATE_PDEV_DMA_COUNT;

        vLineToTrivial(ppdev, x1, y1, x2, y2, pbo->iSolidColor, mix);

        ppdev->NVClipResetFlag=1;       // Clipping rectangle has been reset

        return(TRUE);
        }
    else if (pco->iDComplexity == DC_COMPLEX) 
        {
        //**********************************************************
        // Clip region must be enumerated
        //**********************************************************
        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do  {
            bMore = CLIPOBJ_bEnum(pco, sizeof(ce), (ULONG*) &ce);

            c = cIntersect(prclBounds, ce.arcl, ce.c);

            prclClip = ce.arcl;
            while (c--)
                {
                ulHeight = prclClip->bottom - prclClip->top;
                ulWidth = prclClip->right - prclClip->left;

                NV_DMAPUSH_CHECKFREE(( (ULONG) (3) ));  
                NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
                NV_DMA_FIFO = (( prclClip->top << 16) |  prclClip->left);
                NV_DMA_FIFO = (( ulHeight << 16) | ulWidth ); 
                UPDATE_PDEV_DMA_COUNT;

                vLineToTrivial(ppdev, x1, y1, x2, y2, pbo->iSolidColor, mix);
                INIT_LOCAL_DMA_FIFO;
                prclClip++;
                }
            } while (bMore);

        ppdev->NVClipResetFlag=1;       // Clipping rectangle has been reset
        return(TRUE);

        }
DMAPUSH_GDI_EngLineToIt:

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

    
