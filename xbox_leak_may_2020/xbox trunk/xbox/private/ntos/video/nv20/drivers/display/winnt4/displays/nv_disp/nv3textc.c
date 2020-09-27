//******************************Module*Header***********************************
//
// Module Name: NV3TEXTC.C
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

#ifdef NV3
#include "oldnv332.h"
#define NV32_INCLUDED // Prevents re-inclusion of real nv32 header
#include "driver.h"

#include "nvsubch.h"
#include "nvalpha.h"

//******************************************************************************
// Extern declarations
//******************************************************************************

// IA64 Assembly is NOT feasible under IA64
#ifndef _WIN64
extern void NV3_Transfer_Text_Data(ULONG, USHORT *, ULONG *, Nv3ChannelPio *);
#endif

extern VOID vClipSolid(PDEV* ,LONG ,RECTL *,ULONG ,CLIPOBJ*);
extern RECTL grclMax;

//******************************************************************************
//
//  Function:   NV3GeneralText
//
//  Routine Description:
//
//      Handles any strings that need to be clipped, using the 'glyph
//      expansion' method.
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

VOID NV3GeneralText(
PDEV*     ppdev,
STROBJ*   pstro,
CLIPOBJ*  pco,
BRUSHOBJ* pboFore)

    {
    BOOL        bMoreGlyphs;
    ULONG       cGlyphOriginal;
    ULONG       cGlyph;
    GLYPHPOS*   pgpOriginal;
    GLYPHPOS*   pgp;
    GLYPHBITS*  pgb;
    POINTL      ptlOrigin;
    BOOL        bMore;
    CLIPENUM    ce;
    RECTL*      prclClip;
    ULONG       ulCharInc;
    LONG        cxGlyph;
    LONG        cyGlyph;
    BYTE*       pjGlyph;
    DWORD*      myGlyph;
    LONG        xLeft;
    LONG        yTop;
    LONG        xRight;
    LONG        yBottom;
    LONG        cx;
    LONG        cy;
    BYTE        iDComplexity;

    Nv3ChannelPio   *nv = (Nv3ChannelPio *)ppdev->pjMmBase;
    ULONG       width,height;
    ULONG       SizeInWidth;
    ULONG       NumDwords;
    ULONG       NumBytesPerScan;
    ULONG       Color1;
    USHORT      FreeCount;
    ULONG       NumDwordsPerGlyph;
    ULONG       AlphaEnableValue;
    ULONG       LocalClipResetFlag;

    ASSERTDD(pco != NULL, "Don't expect NULL clip objects here");

    //**************************************************************************
    // Get pixel depth for the current mode.
    // Then determine the ALPHA_CHANNEL enable value
    //**************************************************************************

    AlphaEnableValue = ppdev->AlphaEnableValue;

    //**************************************************************************
    // Get first freecount
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Since we are using Class D of the monster text class, we don't
    // have to specify color 0 (It's always transparent)
    // We only need to get and specify the color for the '1' bits.
    //**************************************************************************

    Color1 = pboFore->iSolidColor   | ( AlphaEnableValue); // Visible

    //**************************************************************************
    // Must make sure that ROP is set correctly
    //**************************************************************************

    while (FreeCount < 1*4)
         FreeCount = nv->subchannel[ROP_SOLID_SUBCHANNEL].control.Free;
    FreeCount -= 1*4;

    nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = NV_SRCCOPY ;

    //**************************************************************************
    // Performance Optimization: Only specify the clipping rectangle
    // if necessary.  A nonzero value means that we need to resend it to hardware.
    //**************************************************************************

    LocalClipResetFlag = 1;

    do  {

        //**********************************************************************
        // pstro->pgp points to the GLYPHPOS array for the whole string.
        // If it is NONZERO, then enumeration is not needed.  There's
        // just one batch of glyphs
        //**********************************************************************

        if (pstro->pgp != NULL)

            {
            //******************************************************************
            // There's only the one batch of glyphs, so save ourselves
            // a call:
            //
            // pgpOriginal   --> GLYPHPOS (contains glyph definition and src origin
            // cGlyphOriginal -> Number of glyphs in the string object
            // bMoreGlyphs    -> Since there's only one batch, don't enumerate
            //******************************************************************

            pgpOriginal    = pstro->pgp;
            cGlyphOriginal = pstro->cGlyphs;
            bMoreGlyphs    = FALSE;
            }

        else

            {
            //******************************************************************
            // Get next batch of GLYPHPOS structures
            //******************************************************************
            bMoreGlyphs = STROBJ_bEnum(pstro, &cGlyphOriginal, &pgpOriginal);
            }

        //**********************************************************************
        // Check if we have any glyphs to output
        //**********************************************************************

        if (cGlyphOriginal > 0)
            {
            //******************************************************************
            // If ulCharInc is NONZERO, then we have a fixed-pitch
            // (monospace) font.  Just add this value to get position of next char
            //******************************************************************

            ulCharInc = pstro->ulCharInc;

            //******************************************************************
            // Check whether we need to do clipping or not
            //******************************************************************

            iDComplexity = (pco == NULL) ? DC_TRIVIAL : pco->iDComplexity;

            if (iDComplexity != DC_COMPLEX)
                {
                //**************************************************************
                // We could call 'cEnumStart' and 'bEnum' when the clipping is
                // DC_RECT, but the last time I checked, those two calls took
                // more than 150 instructions to go through GDI.  Since
                // 'rclBounds' already contains the DC_RECT clip rectangle,
                // and since it's such a common case, we'll special case it:
                //**************************************************************

                //**************************************************************
                // ce => Clip enumeration structure, consists of c (count)
                //       and an array of rectangle structures
                // Specify bmore = FALSE because we have one clipping structure
                //**************************************************************

                bMore = FALSE;
                ce.c  = 1;

                //**************************************************************
                // grclMax essentially means to ignore clipping
                // Otherwise, use the bounds specified in the CLIP Object
                //**************************************************************

                if (iDComplexity == DC_TRIVIAL)
                    prclClip = &grclMax;
                else
                    prclClip = &pco->rclBounds;

                //**************************************************************
                // Skip ahead and handle one clipping rectangle
                //**************************************************************
                goto SingleRectangle;
                }

            //******************************************************************
            // pco           -> Clip Object
            // FALSE         -> only parts relevant to the current drawing
            //                  operation should be enumerated.  Otherwise,
            //                  the whole region should be enumerated (TRUE)
            // CT_RECTANGLES -> Region is to be enumerated as a list of
            //                  rectangles
            //******************************************************************

            CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

            //******************************************************************
            // Get the next clip rectangle to be enumerated from clip region.
            // (ce holds the clip rectangle)
            //******************************************************************

            do  {
                bMore = CLIPOBJ_bEnum(pco, sizeof(ce), (ULONG*) &ce);

                //**************************************************************
                // Draw glyphs for each clipping rectangle
                //**************************************************************

                for (prclClip = &ce.arcl[0]; ce.c != 0; ce.c--, prclClip++)

                    {
SingleRectangle:
                    pgp    = pgpOriginal;           // GLYPHPOS structure
                    cGlyph = cGlyphOriginal;        // Number of glyphs
                    pgb    = pgp->pgdf->pgb;        // Ptr to GLYPHBITS structure

                    //**********************************************************
                    // Origin = Origin of char in bitmap + position on screen
                    //**********************************************************

                    ptlOrigin.x = pgb->ptlOrigin.x + pgp->ptl.x;
                    ptlOrigin.y = pgb->ptlOrigin.y + pgp->ptl.y;

                    //**********************************************************
                    // Loop through all the glyphs for this rectangle:
                    //**********************************************************

                    while (TRUE)
                        {
                        //******************************************************
                        // Get the size (in pixels) of the monochrome bitmap
                        //******************************************************

                        cxGlyph = pgb->sizlBitmap.cx;
                        cyGlyph = pgb->sizlBitmap.cy;

                        //******************************************************
                        // Get pointer to BYTE aligned glyph bitmap
                        //******************************************************

                        pjGlyph = pgb->aj;

                        //******************************************************
                        // Check if this glyph lies completely within
                        // the clipping rectangle.
                        //******************************************************

                        if ((prclClip->left   <= ptlOrigin.x) &&
                            (prclClip->top    <= ptlOrigin.y) &&
                            (prclClip->right  >= ptlOrigin.x + cxGlyph) &&
                            (prclClip->bottom >= ptlOrigin.y + cyGlyph))
                            {

                            //**************************************************
                            // Unclipped glyph
                            //**************************************************

                            //**************************************************
                            // Specify SizeInWidth as multiple of 32 since
                            // we always output DWORDS at a time. The excess
                            // pixels will get clipped according to the width
                            // we specify in Size. SizeInWidth corresponds to the
                            // amount of data that the NV Engine expects.
                            //**************************************************

                            width = cxGlyph;
                            height= cyGlyph;
                            SizeInWidth = (width + 31) & 0xffe0;
                            NumBytesPerScan = (width+7) >> 3;   // Divide by 8

                            //**************************************************
                            // As far as I can tell, the beginning of the
                            // glyph (according to the GLYPHBITS structure)
                            // appears to always start on a DWORD boundary.
                            // In addition, the DDK specifies that the bits
                            // will always be DWORD padded at the end.
                            // So let's just deal with DWORDS for performance.
                            // Otherwise, use the slow byte-by-byte routine.
                            //**************************************************

                            //**************************************************
                            // Calculate total number of DWORDS NV engine expects
                            // to receive for this glyph.  Add 3 so that
                            // 1->4 bytes means 1 dword, 5-8 bytes means 2 dwords..etc..
                            //**************************************************

                            NumDwordsPerGlyph = ((NumBytesPerScan*height)+3) >> 2 ; // Divide by 4

                            //**************************************************
                            // Specify SizeInWidth as multiple of 8 since
                            // the incoming glyph data is always row ordered
                            // and BYTE packed. We will always output up to a
                            // BYTE boundary. The excess pixels will get clipped
                            // according to the width we specify in Size.
                            // 'SizeIn' corresponds to the amount of data
                            // that the NV Engine expects to receive, and it
                            // specifies how the data will be layed out on the
                            // screen.. 'Size' just corresponds to the clipping
                            // rectangle. Add 7 so that 1->8 means 8, 9-16 means 16, etc...
                            //**************************************************

                            SizeInWidth = (width + 7) & 0xfff8;

                            //**************************************************
                            // Only send the clip rectangle if this is the first
                            // time, or we've changed the state of the hardware
                            // (by using another class).  Otherwise, we don't
                            // have to constantly send these values thru the FIFO.
                            //**************************************************

                            if (LocalClipResetFlag)
                                {
                                while (FreeCount < 3*4)
                                    FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
                                FreeCount -= 3*4;

                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.ClipPoint0D = ((0 << 16) | 0);
                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.ClipPoint1D = ( ((ppdev->cyMemory)<<16) | (ppdev->cxMemory) ) ;
                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.Color1D = Color1;

                                LocalClipResetFlag=0;
                                }


                            while (FreeCount < 3*4)
                               FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
                            FreeCount -= 3*4;

                            nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.SizeInD = ( (height <<16) | SizeInWidth );
                            nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.SizeOutD = ( (height <<16) | width );
                            nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.PointD =  ( ((ptlOrigin.y) <<16) | ((ptlOrigin.x) & 0xffff) );


                            //**************************************************
                            // Get pointer to monochrome bitmap
                            //**************************************************

                            myGlyph = (DWORD *)pjGlyph;

                            //**************************************************
                            // Check if Bitmap pointer is DWORD aligned
                            //**************************************************

                            ASSERTDD( (((ULONG_PTR)pjGlyph & 3) == 0), "NOT dword aligned !!!");

                            //**************************************************
                            // Blast out the data as fast as possible
                            // in assembly routine
                            //**************************************************
// IA64 Assembly is NOT feasible under IA64
#ifndef _WIN64
                            NV3_Transfer_Text_Data(NumDwordsPerGlyph,&FreeCount,&myGlyph[0],nv);
#endif

                            }

                        else

                            {

                            //**************************************************
                            // Clipped glyph
                            //**************************************************

                            //**************************************************
                            // Find the intersection of the glyph rectangle
                            // and the clip rectangle:
                            //**************************************************

                            xLeft   = max(prclClip->left,   ptlOrigin.x);
                            yTop    = max(prclClip->top,    ptlOrigin.y);
                            xRight  = min(prclClip->right,  ptlOrigin.x + cxGlyph);
                            yBottom = min(prclClip->bottom, ptlOrigin.y + cyGlyph);

                            //**************************************************
                            // Check for trivial rejection:
                            //**************************************************

                            if (((cx = xRight - xLeft) > 0) &&
                                ((cy = yBottom - yTop) > 0))
                                {
                                //**********************************************
                                // Specify the hardware clipping rectangle
                                //**********************************************

                                while (FreeCount < 2*4)
                                   FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
                                FreeCount -= 2*4;

                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.ClipPoint0D = (((prclClip->top) << 16) | ((prclClip->left)&0xffff) );
                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.ClipPoint1D = ((prclClip->bottom <<16) | ((prclClip->right&0xffff)));


                                //**********************************************
                                // The clipping rectangle has changed.
                                // We need to re-specify it if we subsequently
                                // draw unclipped text
                                //**********************************************

                                LocalClipResetFlag=1;


                                width = cxGlyph;
                                height= cyGlyph;
                                SizeInWidth = (width + 31) & 0xffe0;
                                NumBytesPerScan = (width+7) >> 3;   // Divide by 8
                                NumDwords = (width + 31) >> 5;      // Divide by 32

                                //**********************************************
                                // As far as I can tell, the beginning of the
                                // glyph (according to the GLYPHBITS structure)
                                // appears to always start on a DWORD boundary.
                                // In addition, the DDK specifies that the bits
                                // will always be DWORD padded at the end.
                                // So let's just deal with DWORDS for performance.
                                // Otherwise, use the slow byte-by-byte routine.
                                //**********************************************

                                //**********************************************
                                // Calculate total number of DWORDS NV engine expects
                                // to receive for this glyph.  Add 3 so that
                                // 1->4 bytes means 1 dword, 5-8 bytes means 2 dwords..etc..
                                //**********************************************

                                NumDwordsPerGlyph = ((NumBytesPerScan*height)+3) >> 2; // Divide by 4

                                //**********************************************
                                // Specify SizeInWidth as multiple of 8 since
                                // the incoming glyph data is always row ordered
                                // and BYTE packed. We will always output up to a
                                // BYTE boundary. The excess pixels will get clipped
                                // according to the width we specify in Size.
                                // 'SizeIn' corresponds to the amount of data
                                // that the NV Engine expects to receive, and it
                                // specifies how the data will be layed out on the
                                // screen.. 'Size' just corresponds to the clipping
                                // rectangle. Add 7 so that 1->8 means 8, 9-16 means 16, etc...
                                //**********************************************

                                SizeInWidth = (width + 7) & 0xfff8;

                                //**********************************************
                                // Set the ALPHA bits to make the color visible.
                                // Clear the ALPHA bits to make the color transparent.
                                // 8bpp is in  X16A8Y8 format
                                // 16bpp is in A1R5G5B5 format
                                // 32bpp is in A8R8G8B8 format
                                //**********************************************

                                while (FreeCount < 4*4)
                                   FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
                                FreeCount -= 4*4;

                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.Color1D = Color1;
                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.SizeInD = ( (height <<16) | SizeInWidth );
                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.SizeOutD = ( (height <<16) | width );
                                nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.PointD =  ( ((ptlOrigin.y) <<16) | ((ptlOrigin.x) & 0xffff) );

                                //**********************************************
                                // Get pointer to monochrome bitmap
                                //**********************************************

                                myGlyph = (DWORD *)pjGlyph;

                                //**********************************************
                                // Check if Bitmap pointer is DWORD aligned
                                //**********************************************

                                ASSERTDD( (((ULONG_PTR)pjGlyph & 3) == 0), "NOT dword aligned !!!");

                                //**********************************************
                                // Blast out the data as fast as possible
                                // in assembly routine
                                //**********************************************

// IA64 Assembly is NOT feasible under IA64
#ifndef _WIN64
                                NV3_Transfer_Text_Data(NumDwordsPerGlyph,&FreeCount,&myGlyph[0],nv);
#endif

                                }  // cx=xright-xleft

                            } // clipped glyph

                        if (--cGlyph == 0)
                            break;

                        //******************************************************
                        // Get ready for next glyph:
                        //******************************************************

                        pgp++;
                        pgb = pgp->pgdf->pgb;

                        if (ulCharInc == 0)
                            {
                            ptlOrigin.x = pgp->ptl.x + pgb->ptlOrigin.x;
                            ptlOrigin.y = pgp->ptl.y + pgb->ptlOrigin.y;
                            }
                        else
                            {
                            ptlOrigin.x += ulCharInc;
                            }

                        }  // while TRUE...

                    } // For prclClip...

                } while (bMore);  // do while loop

            } // if (cglyphoriginal)...

        } while (bMoreGlyphs); // main do while loop

    //**************************************************************************
    // Make sure to update the global cached free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;

    }



//******************************************************************************
//
// Function: NV3TextOut()
//
// Routine Description:
//
//   Outputs text using the 'buffer expansion' method.  The CPU draws to a
//   1bpp buffer, and the result is colour-expanded to the screen using the
//   hardware.
//
//   Note that this is x86 only ('vFastText', which draws the glyphs to the
//   1bpp buffer, is writen in Asm).
//
//   If you're just getting your driver working, this is the fastest way to
//   bring up working accelerated text.  All you have to do is write the
//   'Xfer1bpp' function that's also used by the blt code.  This
//   'bBufferExpansion' routine shouldn't need to be modified at all.
//
// Arguments:
//
// Return Value:
//
//     None.
//
//******************************************************************************


BOOL NV3TextOut(
SURFOBJ*  pso,
STROBJ*   pstro,
FONTOBJ*  pfo,
CLIPOBJ*  pco,
RECTL*    prclOpaque,
BRUSHOBJ* pboFore,
BRUSHOBJ* pboOpaque)

    {
    PDEV*           ppdev;
    BYTE            iDComplexity;
    RECTL           rclOpaque;

    Nv3ChannelPio   *nv;
    ULONG   x,y;
    ULONG   width,height;
    USHORT  FreeCount;
    ULONG   AlphaEnableValue;

    //*************************************************************************
    // Initialize NV specific pointers
    //*************************************************************************

    ppdev = (PDEV*) pso->dhpdev;
    iDComplexity = (pco == NULL) ? DC_TRIVIAL : pco->iDComplexity;

    //*************************************************************************
    // Initialize NV specific registers
    //*************************************************************************

    nv = (Nv3ChannelPio *)ppdev->pjMmBase;

    //*************************************************************************
    // Get first FreeCount
    //*************************************************************************

    FreeCount = ppdev->NVFreeCount;

    //*************************************************************************
    // Determine the ALPHA_CHANNEL enable value
    //**************************************************************************

    AlphaEnableValue = ppdev->AlphaEnableValue;


    //*************************************************************************
    // Check if we need to draw the opaque rectangle first
    //*************************************************************************

    if (prclOpaque != NULL)
        {

        //**********************************************************************
        // Opaque Initialization - No clipping necessary
        //**********************************************************************

        while (FreeCount < 2*4)
             FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
        FreeCount -= 2*4;

        nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = NV_SRCCOPY ;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.Color1A = AlphaEnableValue | pboOpaque->iSolidColor;


        if (iDComplexity == DC_TRIVIAL)
            {

DrawOpaqueRect:

            x = prclOpaque->left;
            y = prclOpaque->top;
            width = prclOpaque->right - prclOpaque->left;
            height = prclOpaque->bottom - prclOpaque->top;

            while (FreeCount < 2*4)
                FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
            FreeCount -= 2*4;

            nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.UnclippedRectangle[0].point = ( ((x) << 16) | (y) );
            nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.UnclippedRectangle[0].size = ( (width << 16) | height );

            }

        //**********************************************************************
        // Opaque Initialization - Check for single clipping rectangle
        //**********************************************************************

        else if (iDComplexity == DC_RECT)

            {
            if (bIntersect(prclOpaque, &pco->rclBounds, &rclOpaque))
                {
                prclOpaque = &rclOpaque;
                goto DrawOpaqueRect;
                }
            }

        else

        //**********************************************************************
        // Opaque Initialization - Check for multiple rectangles to be drawn
        //**********************************************************************

            {
            //******************************************************************
            // WARNING: Make sure to update global freecount here first,because
            //          vClipSolid ALSO updates the FreeCount!
            //******************************************************************

            ppdev->NVFreeCount = FreeCount;

            vClipSolid(ppdev, 1, prclOpaque, pboOpaque->iSolidColor, pco);

            //******************************************************************
            // WARNING: Now make sure our local FreeCount has been updated
            //          because we'll use it below!
            //******************************************************************

            FreeCount = ppdev->NVFreeCount;

            }


        }

    //**************************************************************************
    // Make sure to update global freecount here first, because
    // NV1GeneralText relies on it being set correctly
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;

    NV3GeneralText(ppdev, pstro, pco, pboFore);

    return(TRUE);

    }

#endif // NV3
