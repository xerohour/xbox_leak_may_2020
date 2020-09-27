//******************************Module*Header***********************************
// 
// Module Name: NV4TEXTC.C
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
#include "nv32.h"
#include "driver.h"

#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"

//******************************************************************************
// We'll add the 565 bitmap format functions here 
// (We'll use the NV3 functions for 555 format)
//******************************************************************************


//******************************************************************************
// Extern declarations
//******************************************************************************

extern void NV4_DrawFastGlyphs (SURFOBJ*, STROBJ*, FONTOBJ*, RECTL*,
                                GLYPHPOS*, ULONG);
extern VOID vClipSolid(PDEV* ,LONG ,RECTL *,ULONG ,CLIPOBJ*);
extern RECTL grclMax;
extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );


//*****************************************************************************
//
// Function: NV4DmaPushTextOut()
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
//*****************************************************************************


BOOL NV4DmaPushTextOut(
SURFOBJ*  pso,
STROBJ*   pstro,
FONTOBJ*  pfo,
CLIPOBJ*  pco,
RECTL*    prclOpaque,
BRUSHOBJ* pboFore,
BRUSHOBJ* pboOpaque)

    {
    PDEV*           ppdev;
    ULONG           cGlyphOriginal;
    BOOL            bMore;
    BOOL            bMoreGlyphs;
    GLYPHPOS*       pgpOriginal;
    BYTE            iDComplexity;
    CLIPENUM        ce;
    RECTL           rclOpaque;

    RECTL*          prclClip;
    ULONG           Color1;

    ULONG           x,y;
    ULONG           width,height;
    ULONG           AlphaEnableValue;

    DECLARE_DMA_FIFO;
    
    //*************************************************************************
    // Initialize NV specific pointers
    //*************************************************************************

    ppdev = (PDEV*) pso->dhpdev;
    iDComplexity = (pco == NULL) ? DC_TRIVIAL : pco->iDComplexity;

    //*************************************************************************
    // Get push buffer information    
    //*************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //*************************************************************************
    // Determine the ALPHA_CHANNEL enable value
    //*************************************************************************

    AlphaEnableValue = ppdev->AlphaEnableValue;

    //*************************************************************************
    // Make sure ROP is correct.
    // NOTE: needed below in case there is no rectangle.
    //*************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , NV_SRCCOPY);

    //*************************************************************************
    // Check if we need to draw the opaque rectangle first
    //*************************************************************************

    if (prclOpaque != NULL)
        {

        //*********************************************************************
        // Opaque Initialization - No clipping necessary
        //*********************************************************************

        if (iDComplexity == DC_TRIVIAL)
            {

DrawOpaqueRect:

            x = prclOpaque->left;
            y = prclOpaque->top;
            width = prclOpaque->right - x;
            height = prclOpaque->bottom - y;

            NV_DMAPUSH_START (3, RECT_AND_TEXT_SUBCHANNEL, NV04A_COLOR1_A);

            NV_DMA_FIFO = (AlphaEnableValue | pboOpaque->iSolidColor);
            NV_DMA_FIFO = (x << 16) | y;
            NV_DMA_FIFO = (width << (16) | height);
            }

        //*********************************************************************
        // Opaque Initialization - Check for single clipping rectangle
        //*********************************************************************

        else if (iDComplexity == DC_RECT)
            {
            if (bIntersect(prclOpaque, &pco->rclBounds, &rclOpaque))
                {
                prclOpaque = &rclOpaque;
                goto DrawOpaqueRect;
                }
            }

        //*********************************************************************
        // Opaque Initialization - Check for multiple rectangles to be drawn
        //*********************************************************************

        else
            {
            //*****************************************************************
            // WARNING: Make sure to update the global push buffer count here
            //          first, because vClipSolid ALSO updates the buffer Count
            //*****************************************************************


            UPDATE_PDEV_DMA_COUNT;
            vClipSolid(ppdev, 1, prclOpaque, pboOpaque->iSolidColor, pco);

            //*****************************************************************
            // WARNING: Now make sure our local DMA Count has been updated
            //          because we'll use it below!
            //*****************************************************************

            UPDATE_LOCAL_DMA_COUNT;
            }
        }    // if (prclOpaque != NULL)

//*****************************************************************************
//
//      Handles any strings that need to be clipped.
//
//*****************************************************************************

    ASSERTDD(pco != NULL, "Don't expect NULL clip objects here");

    //*************************************************************************
    // Since we are using Class D of the monster text class, we don't
    // have to specify color 0 (It's always transparent)
    // We only need to get and specify the color for the '1' bits.
    //
    // NV4: Class D is now gone in NV4, but we can simulate it by
    //      using Class C and Class E.  Just use Class C but send
    //      SizeIn, SizeOut, and Point to Class E.
    //*************************************************************************

    Color1 = pboFore->iSolidColor | AlphaEnableValue; // Visible

    //*************************************************************************
    // NOTE: ROP set correctly above.
    //*************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(2))); 
    NV_DMAPUSH_WRITE1(RECT_AND_TEXT_SUBCHANNEL, NV04A_COLOR1_C, Color1);

    do  {

        //*********************************************************************
        // pstro->pgp points to the GLYPHPOS array for the whole string.
        // If it is NONZERO, then enumeration is not needed.  There's
        // just one batch of glyphs
        //*********************************************************************

        if (pstro->pgp != NULL)
            {
            //*****************************************************************
            // There's only the one batch of glyphs, so save ourselves
            // a call:
            //
            // pgpOriginal   --> GLYPHPOS (contains glyph definition and src
            //                   origin
            // cGlyphOriginal -> Number of glyphs in the string object
            // bMoreGlyphs    -> Since there's only one batch, don't enumerate
            //*****************************************************************

            pgpOriginal    = pstro->pgp;
            cGlyphOriginal = pstro->cGlyphs;
            bMoreGlyphs    = FALSE;
            }
        else
            {
            //*****************************************************************
            // Get next batch of GLYPHPOS structures
            //*****************************************************************

            bMoreGlyphs = STROBJ_bEnum(pstro, &cGlyphOriginal, &pgpOriginal);
            }

        //*********************************************************************
        // Check if we have any glyphs to output
        //*********************************************************************

        if (cGlyphOriginal > 0)
            {
            //*****************************************************************
            // Simple clipping case - either none or rectangular
            //*****************************************************************

            if (iDComplexity != DC_COMPLEX)
                {
                //*************************************************************
                // grclMax essentially means to ignore clipping
                // Otherwise, use the bounds specified in the CLIP Object
                //*************************************************************
    
                if (iDComplexity == DC_TRIVIAL)
                    prclClip = &grclMax;
                else
                    prclClip = &pco->rclBounds;

                UPDATE_PDEV_DMA_COUNT;

                NV4_DrawFastGlyphs (pso,
                                    pstro,
                                    pfo,
                                    prclClip,
                                    pgpOriginal,
                                    cGlyphOriginal);
            
                NV4_DmaPushSend(ppdev);
                UPDATE_LOCAL_DMA_COUNT;
                }

            //*****************************************************************
            //
            // Complex clipping case
            //
            // pco           -> Clip Object
            // FALSE         -> only parts relevant to the current drawing
            //                  operation should be enumerated.  Otherwise,
            //                  the whole region should be enumerated (TRUE)
            // CT_RECTANGLES -> Region is to be enumerated as a list of
            //                  rectangles
            //*****************************************************************
    
            else
                {
                CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

            //*****************************************************************
            // Get the next clip rectangle to be enumerated from clip region.
            // (ce holds the clip rectangle)
            //*****************************************************************
    
                do  {
                    bMore = CLIPOBJ_bEnum(pco, sizeof(ce), (ULONG*) &ce);

                //**************************************************************
                // Draw glyphs for each clipping rectangle
                //**************************************************************

                    for (prclClip = &ce.arcl[0]; ce.c != 0; ce.c--, prclClip++)
                        {
                            UPDATE_PDEV_DMA_COUNT;

                            NV4_DrawFastGlyphs (pso,
                                                pstro,
                                                pfo,
                                                prclClip,
                                                pgpOriginal,
                                                cGlyphOriginal);
                    
                            NV4_DmaPushSend(ppdev);
                            UPDATE_LOCAL_DMA_COUNT;

                        } // For prclClip...

                    } while (bMore);  // do while loop

                } // else (complex clipping case)

            } // if (cglyphoriginal)...
        
        } while (bMoreGlyphs); // main do while loop

    return (TRUE);

    }

#ifdef _WIN64
//*****************************************************************************
//
// Function: NV4_DrawFastGlyphs()
//
// Routine Description:
//
//   C version for IA64 bringup.
//
// Arguments:
//
// Return Value:
//
//     None.
//
//*****************************************************************************

#define MAX_MONO_DWORDS    128   // sizeof Color1C array in Nv04GdiRectangleText

void NV4_DrawFastGlyphs (
SURFOBJ*  pso,
STROBJ*   pstro,
FONTOBJ*  pfo,
RECTL*    prclClip,
GLYPHPOS* pgpParm,
ULONG     glyphCount)
    {
    PDEV*           ppdev;
    GLYPHPOS*       pgp;
    GLYPHBITS*      pgb;
    POINTL          ptlOrigin;
    ULONG           ulCharInc;
    ULONG           height;
    ULONG           width;
    ULONG           widthByteAligned;
    ULONG           widthBytes;
    ULONG           widthDwords;
    ULONG           glyphDwordsTotal;
    ULONG           glyphDwordsIter;
    ULONG*          glyphData;
    ULONG           i, j;

    DECLARE_DMA_FIFO;
    
    //*************************************************************************
    // Initialize NV specific pointers
    //*************************************************************************

    ppdev = (PDEV*) pso->dhpdev;

    //*************************************************************************
    // Get push buffer information    
    //*************************************************************************

    INIT_LOCAL_DMA_FIFO;
    
    NV_DMAPUSH_CHECKFREE((ULONG)(6 + glyphCount));  

    NV_DMAPUSH_START (2, RECT_AND_TEXT_SUBCHANNEL, NV04A_CLIP_POINT0_C);
    NV_DMA_FIFO = (prclClip->top << 16)    | prclClip->left;
    NV_DMA_FIFO = (prclClip->bottom << 16) | prclClip->right;
    
    //*************************************************************************
    // Trivial rejection
    //*************************************************************************

    if ((prclClip->right  < pstro->rclBkGround.left) ||
        (prclClip->bottom < pstro->rclBkGround.top) ||
        (prclClip->left   > pstro->rclBkGround.right) ||
        (prclClip->top    > pstro->rclBkGround.bottom))
        {
            return;
        }
   
    ulCharInc = pstro->ulCharInc;

    for (i = 0, pgp = pgpParm;
         i < glyphCount;
         i++, pgp = (GLYPHPOS *)((BYTE *)pgp + sizeof(GLYPHPOS)))
        {
            pgb = pgp->pgdf->pgb;
            if ((ulCharInc == 0) || (i == 0))
                {
                ptlOrigin.x = pgb->ptlOrigin.x + pgp->ptl.x;
                ptlOrigin.y = pgb->ptlOrigin.y + pgp->ptl.y;
                }
            else
                {
                ptlOrigin.x += ulCharInc;
                }
            //*****************************************************************
            // Eliminate beginning glyphs that are clipped on the left
            //*****************************************************************
            if ((ptlOrigin.x + pgb->sizlBitmap.cx) <= prclClip->left)
                continue;

            height = pgb->sizlBitmap.cy & 0xffff;            

            if ((height==0) ||
                (ptlOrigin.x > prclClip->right) ||
                (ptlOrigin.y > prclClip->bottom))
                {
                continue;
                }

            width = pgb->sizlBitmap.cx & 0xffff;

            widthByteAligned = (width + 7) & 0xfff8;
            widthBytes = widthByteAligned >> 3;
            glyphDwordsTotal = ((widthBytes * height) + 3) >> 2;
            
            NV_DMAPUSH_CHECKFREE((ULONG)(4));
    
            NV_DMAPUSH_START (3, RECT_AND_TEXT_SUBCHANNEL, NV04A_SIZE_IN_E);
            NV_DMA_FIFO = height << 16 | widthByteAligned;
            NV_DMA_FIFO = height << 16 | width;
            NV_DMA_FIFO = (ptlOrigin.y << 16) | (ptlOrigin.x & 0xffff);
    
            glyphData = (ULONG *)pgb->aj;

            do  {
                glyphDwordsIter = min(glyphDwordsTotal, MAX_MONO_DWORDS);

                NV_DMAPUSH_CHECKFREE((ULONG)(glyphDwordsIter + 1));

                NV_DMAPUSH_START(glyphDwordsIter,
                                 RECT_AND_TEXT_SUBCHANNEL,
                                 NV04A_MONOCHROME_COLOR1_C(0));
                
                for (j = 0; j < glyphDwordsIter; j++)
                    {
                    NV_DMA_FIFO = *glyphData++;
                    }
                glyphDwordsTotal -= glyphDwordsIter;

                } while (glyphDwordsTotal > 0);
        }
    UPDATE_PDEV_DMA_COUNT;
    }
#endif // _WIN64
