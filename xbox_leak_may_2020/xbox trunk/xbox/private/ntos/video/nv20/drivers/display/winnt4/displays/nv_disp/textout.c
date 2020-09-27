//******************************Module*Header***********************************
//
// Module Name: textout.c
//
// On every TextOut, GDI provides an array of 'GLYPHPOS' structures
// for every glyph to be drawn.  Each GLYPHPOS structure contains a
// glyph handle and a pointer to a monochrome bitmap that describes
// the glyph.  (Note that unlike Windows 3.1, which provides a column-
// major glyph bitmap, Windows NT always provides a row-major glyph
// bitmap.)  As such, there are three basic methods for drawing text
// with hardware acceleration:
//
// 1) Glyph caching -- Glyph bitmaps are cached by the accelerator
//       (probably in off-screen memory), and text is drawn by
//       referring the hardware to the cached glyph locations.
//
// 2) Glyph expansion -- Each individual glyph is colour-expanded
//       directly to the screen from the monochrome glyph bitmap
//       supplied by GDI.
//
// 3) Buffer expansion -- The CPU is used to draw all the glyphs into
//       a 1bpp monochrome bitmap, and the hardware is then used
//       to colour-expand the result.
//
// The fastest method depends on a number of variables, such as the
// colour expansion speed, bus speed, CPU speed, average glyph size,
// and average string length.
//
// For the S3 with normal sized glyphs, I've found that caching the
// glyphs in off-screen memory is typically the slowest method.
// Buffer expansion is typically fastest on the slow ISA bus (or when
// memory-mapped I/O isn't available on the x86), and glyph expansion
// is best on fast buses such as VL and PCI.
//
// Glyph expansion is typically faster than buffer expansion for very
// large glyphs, even on the ISA bus, because less copying by the CPU
// needs to be done.  Unfortunately, large glyphs are pretty rare.
//
// An advantange of the buffer expansion method is that opaque text will
// never flash -- the other two methods typically need to draw the
// opaquing rectangle before laying down the glyphs, which may cause
// a flash if the raster is caught at the wrong time.
//
// This driver implements glyph expansion and buffer expansion --
// methods 2) and 3).  Depending on the hardware capabilities at
// run-time, we'll use whichever one will be faster.
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
#include "oglsync.h"


RECTL grclMax = { 0, 0, 0x7fff, 0x7fff };



//******************************************************************************
//
//  Function:   vClipSolid
//
//  Routine Description:
//
//      Fills the specified rectangles with the specified colour, honouring
//      the requested clipping.  No more than four rectangles should be passed in.
//      Intended for drawing the areas of the opaquing rectangle that extend
//      beyond the text box.  The rectangles must be in left to right, top to
//      bottom order.  Assumes there is at least one rectangle in the list.
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


VOID vClipSolid(
PDEV*       ppdev,
LONG        crcl,
RECTL*      prcl,
ULONG       iColor,
CLIPOBJ*    pco)

    {
    BOOL            bMore;              // Flag for clip enumeration
    CLIPENUM        ce;                 // Clip enumeration object
    ULONG           i;
    ULONG           j;
    RECTL           arclTmp[4];
    ULONG           crclTmp;
    RECTL*          prclTmp;
    RECTL*          prclClipTmp;
    LONG            iLastBottom;
    RECTL*          prclClip;
    RBRUSH_COLOR    rbc;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD((crcl > 0) && (crcl <= 4), "Expected 1 to 4 rectangles");
    ASSERTDD((pco != NULL) && (pco->iDComplexity != DC_TRIVIAL),
                       "Expected a non-null clip object");

    rbc.iSolidColor = iColor;

    //**********************************************************************
    // Bottom of last rectangle to fill
    //**********************************************************************

    iLastBottom = prcl[crcl - 1].bottom;

    //**********************************************************************
    // Initialize the clip rectangle enumeration to right-down so we can
    // take advantage of the rectangle list being right-down:
    //**********************************************************************

    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_RIGHTDOWN, 0);

    //**********************************************************************
    // Scan through all the clip rectangles, looking for intersects
    // of fill areas with region rectangles:
    //**********************************************************************

    do {
        //******************************************************************
        // Get a batch of region rectangles:
        //******************************************************************

        bMore = CLIPOBJ_bEnum(pco, sizeof(ce), (VOID*)&ce);

        //******************************************************************
        // Clip the rect list to each region rect:
        //******************************************************************

        for (j = ce.c, prclClip = ce.arcl; j-- > 0; prclClip++)
            {
            //**************************************************************
            // Since the rectangles and the region enumeration are both
            // right-down, we can zip through the region until we reach
            // the first fill rect, and are done when we've passed the
            // last fill rect.
            //**************************************************************

            if (prclClip->top >= iLastBottom)
                {
                //**********************************************************
                // Past last fill rectangle; nothing left to do:
                //**********************************************************

                return;
                }

            //**************************************************************
            // Do intersection tests only if we've reached the top of
            // the first rectangle to fill:
            //**************************************************************

            if (prclClip->bottom > prcl->top)
                {
                //**********************************************************
                // We've reached the top Y scan of the first rect, so
                // it's worth bothering checking for intersection.
                //**********************************************************

                //**********************************************************
                // Generate a list of the rects clipped to this region
                // rect:
                //**********************************************************

                prclTmp     = prcl;
                prclClipTmp = arclTmp;

                for (i = crcl, crclTmp = 0; i-- != 0; prclTmp++)
                    {
                    //******************************************************
                    // Intersect fill and clip rectangles
                    //******************************************************

                    if (bIntersect(prclTmp, prclClip, prclClipTmp))
                        {
                        //**************************************************
                        // Add to list if anything's left to draw:
                        //**************************************************

                        crclTmp++;
                        prclClipTmp++;
                        }
                    }

                //**********************************************************
                // Draw the clipped rects
                //**********************************************************

                if (crclTmp != 0)
                    {
                    (ppdev->pfnFillSolid)(ppdev, crclTmp, &arclTmp[0],
                                          0xf0f0, rbc, NULL);
                    }
                }
            }
        } while (bMore);
    }


//******************************************************************************
//
//  Function:   DrvTextOut
//
//  Routine Description:
//
//      Calls the appropriate text drawing routine.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL DrvTextOut(
SURFOBJ*  pso,
STROBJ*   pstro,
FONTOBJ*  pfo,
CLIPOBJ*  pco,
RECTL*    prclExtra,    // If we had set GCAPS_HORIZSTRIKE, we would have
                        //   to fill these extra rectangles (it is used
                        //   largely for underlines).  It's not a big
                        //   performance win (GDI will call our DrvBitBlt
                        //   to draw the extra rectangles).
RECTL*    prclOpaque,
BRUSHOBJ* pboFore,
BRUSHOBJ* pboOpaque,
POINTL*   pptlBrush,    // Always unused, unless GCAPS_ARBRUSHOPAQUE set
MIX       mix)          // Always a copy mix -- 0x0d0d

    {
    PDEV*       ppdev;
    DSURF*      pdsurfDst;
    SURFOBJ*    TempDstSurfObjPtr;
    ULONG*      DstBits;
    ULONG*      ScreenBitmap;
    BOOL        result;
    ULONG       DstOffset;
    ULONG       DstStride;

    //**************************************************************************
    // Get variables to determine if we're drawing to system memory or VRAM
    //**************************************************************************

    pdsurfDst = (DSURF*) pso->dhsurf;
    ppdev  = (PDEV*) pso->dhpdev;

    ASSERTDD(ppdev != NULL, "Null ppdev in DrvTextOut");

    // NV HW is not enabled.
    if(!ppdev->bEnabled)
        goto Do_EngTextOut;


    //************************************************************************** 
    // Need to do smart sync between OpenGL and GDI depending on 
    // clip intersection.
    //**************************************************************************
    if (OglIsEnabled(ppdev)) // fast ogl check
    {
        bOglSyncIfGdiInOglWindowRect(ppdev,pso, (NULL != prclOpaque) ? prclOpaque : &pstro->rclBkGround, pco);        
    }

        
    //**************************************************************************
    // There will always be a destination so we don't have to check
    // for psoDst == NULL.
    //**************************************************************************

    DstBits         = (ULONG *)(pso->pvBits);
    ScreenBitmap    = (ULONG *)(ppdev->pjScreen);

    //**************************************************************************
    // Set linear offset and stride for source and destination buffers
    //**************************************************************************

    if ((pdsurfDst != NULL) && (pdsurfDst->dt == DT_SCREEN))     // offscreen bitmap
        {
        DstOffset = (ULONG)((BYTE *)(pdsurfDst->LinearPtr) - ppdev->pjFrameBufbase);
        DstStride = pdsurfDst->LinearStride;
        }
    else if ((pdsurfDst == NULL) && (DstBits == ScreenBitmap))  // screen bitmap
        {
        DstOffset = ppdev->ulPrimarySurfaceOffset;
        DstStride = ppdev->lDelta;
        }
        
    //**************************************************************************
    // Only handle cases where destination is VRAM.  This must be a DFB that is
    // stored in system memory. We need to send this case to GDI.
    // GDI is allowed to call us to update DFBs that are stored in system
    // memory when in Full Screen DOS (typically during a STRESS test). Note that
    // the ppdev value might not be valid, and might contain bad data.
    //**************************************************************************

    else
        goto Do_EngTextOut;

//**************************************************************************
    // The DDI spec says we'll only ever get foreground and background
    // mixes of R2_COPYPEN:
    //**************************************************************************

    ASSERTDD(mix == 0x0d0d, "GDI should only give us a copy mix");

    (ppdev->pfnSetDestBase)(ppdev,DstOffset , DstStride);

    result = (ppdev->pfnTextOut(pso, pstro, pfo, pco, prclOpaque, pboFore, pboOpaque));

    return(result);


Do_EngTextOut:

    //**************************************************************************
    // Default to using original SURFOBJ. This will work for GDI managed surfaces
    // which is the case for all bitmaps stored in on-screen VRAM. 
    //**************************************************************************

    TempDstSurfObjPtr = pso;

    //**************************************************************************
    // Check whether surface is a DFB or the primary screen surface. DFB's stored
    // in off screen VRAM need to use a "wrapper" object so GDI will understand it.
    //**************************************************************************

    if (pdsurfDst != NULL)
        {
        if (pdsurfDst->dt == DT_SCREEN)
        
            {
        //**********************************************************************
        // If this is a DFB, and we want to use GDI to draw, use the 'wrapper'
        // surface object.  (The surface object must be locked)
        //**********************************************************************

        //**********************************************************************
        // SurfOBJ that will be used if we are drawing to a DFB
        //**********************************************************************

            TempDstSurfObjPtr = ppdev->psoPunt;

            TempDstSurfObjPtr->dhsurf       = 0;                    // Behave like a standard DIB
            TempDstSurfObjPtr->sizlBitmap   = pdsurfDst->sizl;
            TempDstSurfObjPtr->cjBits       = pdsurfDst->LinearStride * pdsurfDst->sizl.cy;
            TempDstSurfObjPtr->pvBits       = pdsurfDst->LinearPtr;
            TempDstSurfObjPtr->pvScan0      = pdsurfDst->LinearPtr;
            TempDstSurfObjPtr->lDelta       = pdsurfDst->LinearStride;

            TempDstSurfObjPtr->iBitmapFormat = ppdev->iBitmapFormat;

            }
        else
            {
            //**********************************************************************
            // This is a DFB which is stored in system memory as a DIB. If we are called
            // when in Full Screen DOS, ppdev might not be valid, so just return here
            // without waiting for the engine to be busy.
            //**********************************************************************
            TempDstSurfObjPtr = pdsurfDst->pso;
            ppdev->pfnWaitEngineBusy(ppdev);
            return (ppdev->pfnEngTextOut(TempDstSurfObjPtr, pstro, pfo, pco, prclExtra, prclOpaque,
                                         pboFore, pboOpaque, pptlBrush, mix));
            }
        }

    //**************************************************************************
    // Let the DIBEngine draw it
    //**************************************************************************
    ppdev->pfnWaitEngineBusy(ppdev);
    return (ppdev->pfnEngTextOut(TempDstSurfObjPtr, pstro, pfo, pco, prclExtra, prclOpaque,
                                 pboFore, pboOpaque, pptlBrush, mix));
    }

//******************************************************************************
//
//  Function:   bEnableText
//
//  Routine Description:
//
//      Performs the necessary setup for the text drawing subcomponent.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL bEnableText(
PDEV*   ppdev)
    {
    SIZEL   sizl;
    HBITMAP hbm;

    //**************************************************************************
    // We don't do glyph caching for NV
    //**************************************************************************

    return(TRUE);
    }

//******************************************************************************
//
//  Function:   vDisableText
//
//  Routine Description:
//
//      Performs the necessary clean-up for the text drawing subcomponent.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID vDisableText(PDEV* ppdev)

    {
    HSURF       hsurf;
    SURFOBJ*    psoText;

    //**************************************************************************
    // Here we free any stuff allocated in 'bEnableText'.
    // We don't do glyph caching for NV
    //**************************************************************************


    }

//******************************************************************************
//
//  Function:   vAssertModeText
//
//  Routine Description:
//
//      Disables or re-enables the text drawing subcomponent in preparation for
//      full-screen entry/exit.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID vAssertModeText(
PDEV*   ppdev,
BOOL    bEnable)
    {
    //**************************************************************************
    // If we were to do off-screen glyph caching, we would probably want
    // to invalidate our cache here, because it will get destroyed when
    // we switch to full-screen.
    //**************************************************************************
    }

//******************************************************************************
//
//  Function:   DrvDestroyFont
//
//  Routine Description:
//
//      Note: Don't forget to export this call in 'enable.c', otherwise you'll
//              get some pretty big memory leaks!
//
//      We're being notified that the given font is being deallocated; clean up
//      anything we've stashed in the 'pvConsumer' field of the 'pfo'.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID DrvDestroyFont(
FONTOBJ*    pfo)

    {
    CACHEDFONT* pcf;

    pcf = pfo->pvConsumer;
    if (pcf != NULL)
        {
//        vFreeCachedFont(pcf);
        pfo->pvConsumer = NULL;
        }
    }
