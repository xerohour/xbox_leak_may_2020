//******************************Module*Header***********************************
//
// Module Name: Brush.c
//
// Handles all brush/pattern initialization and realization.
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/


#include "precomp.h"
#include "driver.h"

/******************************Public*Routine**********************************\
* VOID vRealizeDitherPattern
*
* Generates an 8x8 dither pattern, in our internal realization format, for
* the colour ulRGBToDither.  Note that the high byte of ulRGBToDither does
* not need to be set to zero, because vComputeSubspaces ignores it.
\******************************************************************************/

VOID vRealizeDitherPattern(
RBRUSH*     prb,
ULONG       ulRGBToDither)
{
    ULONG           ulNumVertices;
    VERTEX_DATA     vVertexData[4];
    VERTEX_DATA*    pvVertexData;
    LONG            i;

    // Calculate what colour subspaces are involved in the dither:

    pvVertexData = vComputeSubspaces(ulRGBToDither, vVertexData);

    // Now that we have found the bounding vertices and the number of
    // pixels to dither for each vertex, we can create the dither pattern

    ulNumVertices = (ULONG)(pvVertexData - vVertexData);
                      // # of vertices with more than zero pixels in the dither

    // Do the actual dithering:

    vDitherColor(&prb->aulPattern[0], vVertexData, pvVertexData, ulNumVertices);

    // Initialize the fields we need:

    prb->ptlBrushOrg.x = LONG_MIN;
    prb->fl            = 0;

    for (i = 0; i < MAX_BOARDS; i++)
    {
        prb->apbe[i] = NULL;
    }
}

//******************************************************************************
//
//  Function:   DrvRealizeBrush()
//
//  Routine Description:
//
//     This function allows us to convert GDI brushes into an internal form
//     we can use.  It may be called directly by GDI at SelectObject time, or
//     it may be called by GDI as a result of us calling BRUSHOBJ_pvGetRbrush
//     to create a realized brush in a function like DrvBitBlt.
//
//     Note that we have no way of determining what the current Rop or brush
//     alignment are at this point.
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL DrvRealizeBrush(
BRUSHOBJ*   pbo,
SURFOBJ*    psoDst,
SURFOBJ*    psoPattern,
SURFOBJ*    psoMask,
XLATEOBJ*   pxlo,
ULONG       iHatch)

    {
    PDEV*       ppdev;
    ULONG       iPatternFormat;
    BYTE*       pjSrc;
    BYTE*       pjDst;
    LONG        lSrcDelta;
    LONG        cj;
    LONG        i;
    LONG        j;
    RBRUSH*     prb;
    ULONG*      pulXlate;
    SURFOBJ*    psoPunt;
    RECTL       rclDst;
    BOOL        b;

    ppdev = (PDEV*) psoDst->dhpdev;


    //**************************************************************************
    // We only handle brushes if we have an off-screen brush cache
    // available.  If there isn't one, we can simply fail the realization,
    // and eventually GDI will do the drawing for us (although a lot
    // slower than we could have done it):
    //
    // NOTE: Currently, BRUSH_CACHE is always disabled in Enable.c
    //       For now, we're just going to get the pattern straight from
    //       the brush structure and not cache it....eventually
    //       we'll store it in offscreen memory...but for now...we just
    //       want to get brush functionality working. (without caching)
    //**************************************************************************

    //**************************************************************************
    //    if (!(ppdev->flStatus & STAT_BRUSH_CACHE))
    //        goto ReturnFalse;
    //**************************************************************************

    //**************************************************************************
    // We have a fast path for dithers when we set GCAPS_DITHERONREALIZE:
    // For now, just return false.
    //**************************************************************************

    if (iHatch & RB_DITHERCOLOR)
        {
        //**********************************************************************
        // Implementing DITHERONREALIZE increased our score on a certain
        // unmentionable benchmark by 0.4 million 'megapixels'.  Too bad
        // this didn't work in the first version of NT.
        //**********************************************************************

        //**********************************************************************
        // NV: This case doesn't seem to buy us that much of an improvement.
        //     Leaving out for now...
        //
        //        prb = BRUSHOBJ_pvAllocRbrush(pbo,
        //               sizeof(RBRUSH) + CONVERT_TO_BYTES(TOTAL_BRUSH_SIZE, ppdev));
        //        if (prb == NULL)
        //            goto ReturnFalse;
        //
        //        vRealizeDitherPattern(prb, iHatch);
        //        goto ReturnTrue;
        //**********************************************************************

        goto ReturnFalse;

        }

    //**************************************************************************
    // We only accelerate 8x8 patterns.  Since Win3.1 and Chicago don't
    // support patterns of any other size, it's a safe bet that 99.9%
    // of the patterns we'll ever get will be 8x8:
    //**************************************************************************

    if ((psoPattern->sizlBitmap.cx != 8) ||
        (psoPattern->sizlBitmap.cy != 8))
        goto ReturnFalse;

    iPatternFormat = psoPattern->iBitmapFormat;

    //**************************************************************************
    // Allocate memory for our RBRUSH structure.  The actual
    // pattern bits are stored at the end of this structure.
    //**************************************************************************

    prb = BRUSHOBJ_pvAllocRbrush(pbo,
          sizeof(RBRUSH) + CONVERT_TO_BYTES(TOTAL_BRUSH_SIZE, ppdev));
    if (prb == NULL)
        goto ReturnFalse;

    //**************************************************************************
    // Initialize the fields we need:
    // BrushOrg.X will be initialized to the smallest value possible.
    // Flags will be init to NULL.
    //**************************************************************************

    prb->ptlBrushOrg.x = LONG_MIN;
    prb->fl            = 0;

    //**************************************************************************
    // For now, multi board functionality isn't important to us.
    // MAX_BOARDS should be 1.
    //**************************************************************************

    for (i = 0; i < MAX_BOARDS; i++)
        {
        prb->apbe[i] = NULL;
        }

    //**************************************************************************
    // Now let's take a look at the pattern bitmap.
    // Get the pitch and ptr to the pattern bitmap
    // Also get a ptr to the aulpattern array where we'll store the pattern bits.
    //**************************************************************************

    lSrcDelta = psoPattern->lDelta;
    pjSrc     = (BYTE*) psoPattern->pvScan0;
    pjDst     = (BYTE*) &prb->aulPattern[0];

    //**************************************************************************
    // Handle simple case where pattern bitmap format is same
    // as current screen bitmap format, and palette translation is not required.
    //**************************************************************************

    if ((ppdev->iBitmapFormat == iPatternFormat) &&
        ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL)))
        {
        DISPDBG((2, "Realizing un-translated brush"));

        //**********************************************************************
        // The pattern is the same colour depth as the screen, and
        // there's no translation to be done:
        //**********************************************************************

        cj = CONVERT_TO_BYTES(8, ppdev);  // Every pattern is 8 pels wide

        //**********************************************************************
        // RtlCopyMemory is a RunTime library routine (Destination,Src,Length in bytes)
        //**********************************************************************

        for (i = 8; i != 0; i--)
            {
            RtlCopyMemory(pjDst, pjSrc, cj);

            pjSrc += lSrcDelta;
            pjDst += cj;
            }

        }

    //**************************************************************************
    // Don't do monochrome expansion on 24 bpp due to s3 968 feature.
    // For NV, we currently don't support 24bpp anyway.
    // All monochrome expansion will get done here.
    //**************************************************************************

    else if ((iPatternFormat == BMF_1BPP) && (ppdev->iBitmapFormat != BMF_24BPP))

        {
        DISPDBG((2, "Realizing 1bpp brush"));

        //**********************************************************************
        // We word align the monochrome bitmap so that every row starts
        // on a new word (so that we can do word writes later to transfer
        // the bitmap):
        //
        // NV: For NV, we'll just set the pattern object to be an 8x8 pattern.
        //     2 patterns registers (2 dwords) need to be set as follows:
        //                               -------------------------------------
        //          Pattern register 0  | byte 3 |  byte 2 |  byte 1 | byte 0 |
        //                               -------------------------------------
        //                               -------------------------------------
        //          Pattern register 1  | byte 7 |  byte 6 |  byte 5 | byte 4 |
        //                               -------------------------------------
        //
        //     This defines a total of 64 bits (8x8 monochrome pattern)
        //
        //**********************************************************************

        for (i = 8; i != 0; i--)
            {
            *pjDst = *pjSrc;
            pjDst ++;                           // Store NV monochrome pattern bytes
            pjSrc += lSrcDelta;                 // Usually src pattern is dword aligned
            }

        pulXlate         = pxlo->pulXlate;
        prb->fl         |= RBRUSH_2COLOR;
        prb->ulForeColor = pulXlate[1];
        prb->ulBackColor = pulXlate[0];

        }

    //**************************************************************************
    // Pattern is 4bpp, device is currently at 8bpp
    //**************************************************************************

    else if ((iPatternFormat == BMF_4BPP) && (ppdev->iBitmapFormat == BMF_8BPP))

        {
        //**********************************************************************
        // NV: This case doesn't have a signficant impact on performance
        //     so we'll skip it for now.
        //**********************************************************************

        //**********************************************************************
        //        DISPDBG((2, "Realizing 4bpp brush"));
        //
        //        //************************************************************
        //        // The screen is 8bpp and the pattern is 4bpp:
        //        //************************************************************
        //
        //        ASSERTDD((ppdev->iBitmapFormat == BMF_8BPP) &&
        //                 (iPatternFormat == BMF_4BPP),
        //                 "Messed up brush logic");
        //
        //        pulXlate = pxlo->pulXlate;
        //
        //        for (i = 8; i != 0; i--)
        //            {
        //            //********************************************************
        //            // Inner loop is repeated only 4 times because each loop
        //            // handles 2 pixels:
        //            //********************************************************
        //
        //            for (j = 4; j != 0; j--)
        //                {
        //                *pjDst++ = (BYTE) pulXlate[*pjSrc >> 4];
        //                *pjDst++ = (BYTE) pulXlate[*pjSrc & 15];
        //                pjSrc++;
        //                }
        //
        //            pjSrc += lSrcDelta - 4;
        //            }
        //
        //**********************************************************************

        goto ReturnFalse;

        }


    //**************************************************************************
    // NV: Unrecognized/ brush format.  Just punt back to GDI
    //**************************************************************************

    else

        {
        //**********************************************************************
        // We've got a brush whose format we haven't special cased.  No
        // problem, we can have GDI convert it to our device's format.
        // We simply use a temporary surface object that was created with
        // the same format as the display, and point it to our brush
        // realization:
        //**********************************************************************

        DISPDBG((5, "Realizing funky brush"));

        psoPunt          = ppdev->psoPunt3;
        psoPunt->pvBits  = pjDst;
        psoPunt->pvScan0 = pjDst;
        psoPunt->lDelta  = CONVERT_TO_BYTES(8, ppdev);

        rclDst.left      = 0;
        rclDst.top       = 0;
        rclDst.right     = 8;
        rclDst.bottom    = 8;

        ppdev->pfnWaitEngineBusy(ppdev);
        b = ppdev->pfnEngCopyBits(psoPunt, psoPattern, NULL, pxlo,
                                  &rclDst, (POINTL*) &rclDst);

        if (!b)
            {
            goto ReturnFalse;
            }

        }

//**************************************************************************
// ReturnTrue:
//**************************************************************************

    //**************************************************************************
    // NV: Skip following code for now... (we currently don't store
    //     patterns in offscreen memory
    //**************************************************************************

    //**************************************************************************
    //    if (!(ppdev->flCaps & CAPS_HW_PATTERNS))
    //    {
    //        // The last time I checked, GDI took some 500 odd instructions to
    //        // get from here back to whereever we called 'BRUSHOBJ_pvGetRbrush'.
    //        // We can at least use this time to get some overlap between the
    //        // CPU and the display hardware: we'll initialize the 72x72 off-
    //        // screen cache entry now, which will keep the accelerator busy for
    //        // a while.
    //        //
    //        // We don't do this if we have hardware patterns because:
    //        //
    //        //   a) S3 hardware patterns require that the off-screen cached
    //        //      brush be correctly aligned, and at this point we don't have
    //        //      access to the 'pptlBrush' brush origin (although we could
    //        //      have copied it into the PDEV before calling
    //        //      BRUSHOBJ_pvGetRbrush).
    //        //
    //        //   b) S3 hardware patterns require only an 8x8 copy of the
    //        //      pattern; it is not expanded to 72x72, so there isn't even
    //        //      any opportunity for CPU/accelerator processing overlap.
    //
    //        vIoSlowPatRealize(ppdev, prb, FALSE);
    //    }
    //**************************************************************************

    return(TRUE);

ReturnFalse:


    //**************************************************************************
    // NV: Failed call, let GDI handle it
    //**************************************************************************

    if (psoPattern != NULL)
        {
        DISPDBG((2, "Failed realization -- Type: %li Format: %li cx: %li cy: %li",
                    psoPattern->iType, psoPattern->iBitmapFormat,
                    psoPattern->sizlBitmap.cx, psoPattern->sizlBitmap.cy));
        }

    return(FALSE);
    }

