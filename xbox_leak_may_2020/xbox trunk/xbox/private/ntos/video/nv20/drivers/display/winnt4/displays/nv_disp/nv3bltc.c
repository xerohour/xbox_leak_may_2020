//******************************Module*Header***********************************
//
// Module Name: NV3BLTC.C
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

#include "nvsubch.h"
#define NV32_INCLUDED // Prevents re-inclusion of real nv32 header
#include "driver.h"

#ifdef _WIN32_WINNT     // If is >= NT 4.x

//******************************************************************************
// Externs
//******************************************************************************

ULONG nvBrushToSrcRopTable[64] = {
0x00000000, 0x00000011, 0x00000022, 0x00000033,
0x00000000, 0x00000011, 0x00000022, 0x00000033,
0x00000000, 0x00000011, 0x00000022, 0x00000033,
0x00000000, 0x00000011, 0x00000022, 0x00000033,
0x00000044, 0x00000055, 0x00000066, 0x00000077,
0x00000044, 0x00000055, 0x00000066, 0x00000077,
0x00000044, 0x00000055, 0x00000066, 0x00000077,
0x00000044, 0x00000055, 0x00000066, 0x00000077,
0x00000088, 0x00000099, 0x000000AA, 0x000000BB,
0x00000088, 0x00000099, 0x000000AA, 0x000000BB,
0x00000088, 0x00000099, 0x000000AA, 0x000000BB,
0x00000088, 0x00000099, 0x000000AA, 0x000000BB,
0x000000CC, 0x000000DD, 0x000000EE, 0x000000FF,
0x000000CC, 0x000000DD, 0x000000EE, 0x000000FF,
0x000000CC, 0x000000DD, 0x000000EE, 0x000000FF,
0x000000CC, 0x000000DD, 0x000000EE, 0x000000FF
};

#ifdef _X86_

//******************************************************************************
// X86 Assembly routine to xfer host bitmap->screen
//******************************************************************************

void NV1_Transfer_MemToScreen_Data(ULONG, USHORT *, ULONG, ULONG *,LONG, Nv3ChannelPio *);

#endif

//******************************************************************************
// Macros used by FillPatFast (optimization)
//******************************************************************************


#define BUILD_PIXEL_GROUP(GROUP_WIDTH,BLIT_SIZE,NUM_BLITS) \
{\
if (RemainingPixels>=(GROUP_WIDTH))\
    {\
    for (i=0; i<(NUM_BLITS) ;i++)\
        {\
        while (FreeCount < 3*4)\
            FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;\
        FreeCount -= 3*4;\
\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (CurrentYdst << 16) | (xdst & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( ( (CurrentYdst ) << 16) | ((CurrentXdst ) & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( (1 << 16) | (BLIT_SIZE) );\
\
        RemainingPixels -= (BLIT_SIZE);\
        CurrentXdst     += (BLIT_SIZE);\
        }\
    }\
}


#define COPY_PIXEL_GROUP(GROUP_WIDTH,NUM_BLITS) \
{\
if (RemainingPixels>= (GROUP_WIDTH) )\
    {\
    NumberOfBlits = (NUM_BLITS);\
\
    for (i=0; i< (NumberOfBlits) ;i++)\
        {\
        while (FreeCount < 3*4)\
            FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;\
        FreeCount -= 3*4;\
\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (CurrentYdst << 16) | (xdst & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( ( (CurrentYdst ) << 16) | ((CurrentXdst ) & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( (1 << 16) | (GROUP_WIDTH) );\
\
        RemainingPixels -= (GROUP_WIDTH);\
        CurrentXdst     += (GROUP_WIDTH);\
        }\
    }\
}


#define BUILD_PIXEL_BLOCK(GROUP_HEIGHT,GROUP_WIDTH,BLIT_HEIGHT,NUM_BLITS) \
{\
if (RemainingScanlines>=(GROUP_HEIGHT))\
    {\
    for (i=0; i<(NUM_BLITS) ;i++)\
        {\
        while (FreeCount < 3*4)\
            FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;\
        FreeCount -= 3*4;\
\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (ydst << 16) | ((CurrentXdst) & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( ( (CurrentYdst ) << 16) | ((CurrentXdst ) & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( ((BLIT_HEIGHT) << 16) | (GROUP_WIDTH) );\
\
        RemainingScanlines -= (BLIT_HEIGHT);\
        CurrentYdst        += (BLIT_HEIGHT);\
        }\
    }\
}


#define COPY_PIXEL_BLOCK(GROUP_HEIGHT,GROUP_WIDTH,NUM_BLITS) \
{\
if (RemainingScanlines>= (GROUP_HEIGHT) )\
    {\
    NumberOfBlits = (NUM_BLITS);\
\
    for (i=0; i< (NumberOfBlits) ;i++)\
        {\
        while (FreeCount < 3*4)\
            FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;\
        FreeCount -= 3*4;\
\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (ydst << 16) | ((CurrentXdst) & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( ( (CurrentYdst ) << 16) | ((CurrentXdst ) & 0xffff) );\
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( ((GROUP_HEIGHT) << 16) | (GROUP_WIDTH) );\
\
        RemainingScanlines -= (GROUP_HEIGHT);\
        CurrentYdst        += (GROUP_HEIGHT);\
        }\
    }\
}


//******************************************************************************
//
//  Function:   NV3FillSolid
//
//  Routine Description:
//
//      Fills a list of rectangles with a solid colour.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV3FillSolid(              // Type FNFILL
PDEV*           ppdev,
LONG            c,              // Can't be zero
RECTL*          prcl,           // List of rectangles to be filled, in relative
                                //   coordinates
ULONG           rop4,           // rop4
RBRUSH_COLOR    rbc,            // Drawing colour is rbc.iSolidColor
POINTL*         pptlBrush)      // Not used

    {
    Nv3ChannelPio  *nv  = (Nv3ChannelPio *)ppdev->pjMmBase;

    ULONG   ulHwForeMix;
    ULONG  width,height;
    USHORT FreeCount;
    ULONG Rop3;
    ULONG temp;
    ULONG AlphaEnableValue;
    ULONG PixelDepth;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(c > 0, "Can't handle zero rectangles");

    //**************************************************************************
    // Get pixel depth for the current mode.
    // Then determine the ALPHA_CHANNEL enable value
    //**************************************************************************

    AlphaEnableValue = ppdev->AlphaEnableValue;

// ???
///    ulHwForeMix = gaulHwMixFromRop2[(rop4 >> 2) & 0xf];

    //**************************************************************************
    // Equation to convert to Rop4 to Rop3 (We don't get patterns here)
    //
    // Rop4:
    //                 Truth table for a Rop 3.
    //                 ------------------------
    //                                    R
    //                                    O
    //                        (SD)        P
    //                  11  10  01  00    3
    //                  --------------
    //          (P) 1  |         x   x    High nibble    Example Value = 0xf0
    //              0  | x   x            Low nibble
    //
    //
    // After rop4 >> 2:
    //
    //                 Truth table for a Rop 3.
    //                 ------------------------
    //                                    R
    //                                    O
    //                        (SD)        P
    //                  11  10  01  00    3
    //                  --------------
    //          (P) 1  |                  High nibble    Value = 0x3c
    //              0  | x   x   x   x    Low nibble
    //
    //
    // After (rop4 >> 2 ) & 0xf:
    //
    //                                                   Value = 0x0c
    //
    // Final Rop3 equivalent value of a Rop2 value = 0xcc
    //
    //         This value does not depend on the Pattern
    //**************************************************************************

    temp = ((rop4 >> 2) & 0xf);         // Get bottom nibble
    Rop3 = temp | (temp << 4);        // Duplicate in top nibble

    //**************************************************************************
    // Init cached free count
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

    while (FreeCount < 1*4)
        FreeCount = nv->subchannel[ROP_SOLID_SUBCHANNEL].control.Free;
    FreeCount -= 1*4;

    nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = Rop3 ;

    //**************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        while (FreeCount < 2*4)
           FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
        FreeCount -= 2*4;

        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((0 << 16) | 0 );
        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (( (ppdev->cyMemory)<<16) | (0x7fff));

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }

    while(TRUE)
        {

        //**********************************************************************
        // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
        // That is, the lower right coordinate is NOT included
        // For NV, the width/height actually means the number of pixels drawn.
        // So, adjusting by 1 pixel is NOT necessary when calculating width and height.
        //**********************************************************************

        width = prcl->right - prcl->left;
        height = prcl->bottom - prcl->top;

        //**********************************************************************
        // Wait for FIFO to be ready, then draw
        // We know that this is a straight SOLID FILL with no pattern
        //***********************************************************************

        while (FreeCount < 3*4)
            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.Free ;
        FreeCount -=3*4;

        nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Color = (AlphaEnableValue | rbc.iSolidColor);
        nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].point = (( (prcl->top) << 16) | ( (prcl->left) & 0xffff));
        nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].size = ( (height << 16) | width );

        if (--c == 0)
            {
            //******************************************************************
            // Update global free count
            //******************************************************************
            ppdev->NVFreeCount = FreeCount;

            return;
            }

        prcl++;
        }

    }


//******************************************************************************
//
//  Function:   NV3FillPatFast
//
//  Routine Description:
//
//      This routine uses the NV Image_Mono_From_CPU and Image_From_CPU
//      object clases to draw a patterned list of rectangles.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV3FillPatFast(            // Type FNFILL
PDEV*           ppdev,
LONG            c,              // Can't be zero
RECTL*          prcl,           // List of rectangles to be filled, in relative
                                //   coordinates
ULONG           rop4,           // rop4
RBRUSH_COLOR    rbc,            // rbc.prb points to brush realization structure
POINTL*         pptlBrush)      // Pattern alignment

    {
    BOOL        bTransparent;
    ULONG       ulHwForeMix;
    BRUSHENTRY* pbe;        // Pointer to brush entry data, which is used
                            //   for keeping track of the location and status
                            //   of the pattern bits cached in off-screen
                            //   memory

    Nv3ChannelPio  *nv  = (Nv3ChannelPio *)ppdev->pjMmBase;
    ULONG Color0;
    ULONG Color1;
    ULONG AlphaEnableValue;
    ULONG PixelDepth;
    ULONG width;
    ULONG height;
    USHORT FreeCount;
    ULONG rop3;
    LONG  xShift;
    LONG  yShift;
    ULONG BytesPerDstScan;
    ULONG DwordsPerDstScan;
    ULONG SizeInWidth;
    LONG BytesPerSrcScan;
    BYTE*  SrcBits;
    BYTE*  DstBits;
    ULONG *ScanPtr;
    ULONG TempPixelData1;
    ULONG TempPixelData2;
    RBRUSH* prb;
    ULONG PatternYCount;
    ULONG *NextScan;
    ULONG *EndOfScanPtr;
    LONG ScanInc;
    ULONG DwordCount;
    ULONG yscan;
    ULONG xdst,ydst;
    LONG  cjLeft;
    LONG  cjRight;
    ULONG   i;

    ULONG CurrentXdst;
    ULONG CurrentYdst;
    ULONG RemainingPixels;
    ULONG SrcOffset;
    ULONG SrcStride;
    ULONG NumberOfBlits;
    ULONG RemainingScanlines;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(c > 0, "Can't handle zero rectangles");

    //**************************************************************************
    // TODO: Do I need to do anything special for transparent cases?
    //**************************************************************************
    rop3 = (ULONG)((BYTE) (rop4 & 0xff));
    bTransparent = (((rop4 >> 8) & 0xff) != rop3);

    //**************************************************************************
    // Get pointer to brush entry
    //**************************************************************************

    prb = rbc.prb;
    pbe = rbc.prb->apbe[IBOARD(ppdev)];

    //**************************************************************************
    // Need to add if statement if brush pattern hasn't changed
    // For now, just always re-calculate brush pattern
    // Performance boost of how much?
    //**************************************************************************

    //**************************************************************************
    // Init FreeCount variable
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Get pixel depth for the current mode.
    // Then determine the ALPHA_CHANNEL enable value
    //**************************************************************************

    PixelDepth = ppdev->cBitsPerPel;

    AlphaEnableValue = ppdev->AlphaEnableValue;

    //**************************************************************************
    // Restore clip rect
    //**************************************************************************
    if (ppdev->NVClipResetFlag)
        {
        while(FreeCount < 2*4)
            FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
        FreeCount -= 2*4;

        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((0 << 16) | 0 );
        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (((ppdev->cyMemory)<<16) | (0x7fff));

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }


    //**************************************************************************
    // I considered doing the colour expansion for 1bpp brushes in
    // software, but by letting the hardware do it, we don't have
    // to do as many OUTs to transfer the pattern.
    //**************************************************************************

    if (prb->fl & RBRUSH_2COLOR)
        {

        //**********************************************************************
        // Because we handle only 8x8 brushes, it is easy to compute the
        // number of pels by which we have to rotate the brush pattern
        // right and down.  Note that if we were to handle arbitrary sized
        // patterns, this calculation would require a modulus operation.
        //
        // The brush is aligned in absolute coordinates, so we have to add
        // in the surface offset:
        //
        // NV: We DON'T need to manually account for the position of the pattern
        //     on the screen, like we do for the Memory to Screen blits for
        //     multi-color patterns (below).
        //**********************************************************************

        //**********************************************************************
        // Rotate pattern 'xShift' pels right
        // Rotate pattern 'yShift' pels down
        //**********************************************************************

        xShift = pptlBrush->x & 7;
        yShift = pptlBrush->y & 7;

        //**********************************************************************
        // Shift Mono pattern bits appropriately (vertically)
        // First we'll align it properly by copying it to a temporary buffer:
        //**********************************************************************

        SrcBits = (BYTE*) &prb->aulPattern[0];          // Copy from brush buffer
        DstBits = (BYTE*) &ppdev->AlignedBrush[0];      // Copy to our aligned buffer

        DstBits += yShift;                              // Starting yShift rows
        i = 8 - yShift;                                 //  down for 8 - yShift rows

        //**********************************************************************
        // Now copy the pattern , row by row (byte by byte) and
        // align it vertically and horizontally at the same time
        //**********************************************************************

        do  {
            *DstBits = ((*SrcBits >> xShift) | (*SrcBits << (8-xShift)));
            DstBits ++;
            SrcBits ++;

            } while (--i != 0);

        //**********************************************************************
        // Now copy the 'top' part
        //**********************************************************************

        DstBits -=8;                    // Move back to beginning of destination

        for (; yShift != 0; yShift--)
            {
            *DstBits = ((*SrcBits >> xShift) | (*SrcBits << (8-xShift)));
            DstBits ++;
            SrcBits ++;
            }

        //**********************************************************************
        // Init foreground and background color
        //**********************************************************************

        Color1 = rbc.prb->ulForeColor | (AlphaEnableValue); // Background color
        Color0 = rbc.prb->ulBackColor | (AlphaEnableValue); // Foreground color

        while (FreeCount < 6*4)
            FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.Free;
        FreeCount -= 6*4;

        nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = rop3 ;

        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeShape = NV018_SET_MONOCHROME_SHAPE_8X_8Y;
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeColor0 = Color0;
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeColor1 = Color1;
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromePattern0 = ppdev->AlignedBrush[0];
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromePattern1 = ppdev->AlignedBrush[1];


        do {
            //******************************************************************
            // Get width and height of the destination rectangle
            //******************************************************************

            width = prcl->right - prcl->left;
            height= prcl->bottom - prcl->top;

            //******************************************************************
            // Wait for FIFO to be ready, then draw
            //******************************************************************

            while (FreeCount < 3*4)
                FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.Free;
            FreeCount -= 3*4;

            nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Color = rbc.prb->ulForeColor | AlphaEnableValue;
            nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].point = ((prcl->top << 16) | ((prcl->left) & 0xffff));
            nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].size = ( (height << 16) | width );


            prcl++;

            } while (--c != 0);

        //**********************************************************************
        // Notify the rest of the driver that we changed the pattern
        //**********************************************************************

        ppdev->NVPatternResetFlag=1;

        }


    else

        {

        //**********************************************************************
        // Loop thru all the clipping rectangles
        //**********************************************************************

        do  {

            //******************************************************************
            // Handle multi-color patterns.  For now, we just use the
            // ImageFromCpu class (memory to screen blit).
            //******************************************************************

            //******************************************************************
            // Because we handle only 8x8 brushes, it is easy to compute the
            // number of pels by which we have to rotate the brush pattern
            // right and down.  Note that if we were to handle arbitrary sized
            // patterns, this calculation would require a modulus operation.
            //
            // The brush is aligned in absolute coordinates, so we have to add
            // in the surface offset:
            //
            // NV: Since we are using Memory to Screen blits, we need to manually
            //     account for the position of the pattern on screen. So, not only
            //     do we need to factor in pptrlBrush.x/y, but also prcl->left/top;
            //     We don't care what the alignment is offscreen, because offscreen
            //     position is NOT the final destination/position.
            //******************************************************************

            xShift = pptlBrush->x - prcl->left;
            yShift = pptlBrush->y - prcl->top;

            //******************************************************************
            // Rotate pattern 'xShift' pels right
            // Rotate pattern 'yShift' pels down
            //******************************************************************

            xShift &= 7;
            yShift &= 7;

            //******************************************************************
            // We're going to do a straight of the Xbpp 8x8 pattern to the screen.
            // First we'll align it properly by copying it to a temporary buffer:
            //******************************************************************

            cjLeft  = CONVERT_TO_BYTES(xShift, ppdev);      // Number of bytes pattern
                                                            //   is shifted to the right
            cjRight = CONVERT_TO_BYTES(8, ppdev) - cjLeft;  // Number of bytes pattern
                                                            // is shifted to the left

            SrcBits = (BYTE*) &prb->aulPattern[0];          // Copy from brush buffer
            DstBits = (BYTE*) &ppdev->AlignedBrush[0];      // Copy to our aligned buffer

            DstBits += yShift * CONVERT_TO_BYTES(8, ppdev); // Starting yShift rows
            i = 8 - yShift;                                 //  down for 8 - yShift rows

            //******************************************************************
            // Now copy the pattern and align it, in 2 steps.
            // The 'left' part, and 'right' part (as well as the 'bottom' and 'top')
            // We'll start copying the 'bottom' part first.
            //******************************************************************

            do  {
                RtlCopyMemory(DstBits + cjLeft, SrcBits, cjRight);
                RtlCopyMemory(DstBits, SrcBits + cjRight, cjLeft);

                DstBits += cjLeft + cjRight;
                SrcBits += cjLeft + cjRight;

                } while (--i != 0);

            //******************************************************************
            // Now copy the 'top' part
            //******************************************************************

            DstBits = (BYTE*) &ppdev->AlignedBrush[0];      // Move to the beginning of destination

            for (; yShift != 0; yShift--)
                {
                RtlCopyMemory(DstBits + cjLeft, SrcBits, cjRight);
                RtlCopyMemory(DstBits, SrcBits + cjRight, cjLeft);

                DstBits += cjLeft + cjRight;
                SrcBits += cjLeft + cjRight;
                }

            //******************************************************************
            // Get width and height of the destination rectangle
            //******************************************************************

            width = prcl->right - prcl->left;
            height= prcl->bottom - prcl->top;

            //******************************************************************
            // Top left coordinate of destination on destination surface
            //******************************************************************

            xdst = prcl->left;
            ydst = prcl->top;

            //******************************************************************
            // Prepare to do an IMAGE_FROM_CPU blit
            //******************************************************************

            while (FreeCount < 4*4)
               FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -=4*4;

            //******************************************************************
            // rop3 is a pat rop. Since we don't have color patterns in hw, we
            // treat pattern as source and thus convert to the corresponding source
            // rop. e.g. - convert PATCOPY->SRCCOPY, PATINVERT->SRCINVERT.
            //******************************************************************
            nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 =
                nvBrushToSrcRopTable[rop3 >> 2];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Point =  ( ((ydst) <<16) | ((xdst) & 0xffff) );
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeOut = ( (height <<16) | width );

            //******************************************************************
            // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
            // ignore/clip the extra pixel that we send it, so that it won't
            // get included as part of the next scanline. Since we always
            // send DWords at a time, we may actually send more data to the NV
            // engine than is necessary.  So clip it by specifying a larger SizeIn
            // That is, we always send DWORDS at a time for each scanline.
            // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
            //******************************************************************

            if (PixelDepth == 8)
                SizeInWidth = ((width + 3) & 0xfffc);
            else if (PixelDepth == 16)
                SizeInWidth = ((width + 1) & 0xfffe);
            else
                SizeInWidth = width;

            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeIn = ( (height <<16) | (SizeInWidth) );

            //******************************************************************
            // Calculate number of dwords to output for scanline
            // This is the actual amount of data that the NV engine expects per scanline.
            // SizeInWidth is guaranteed to be a multiple of 2 (for 16bpp) and
            // a multiple of 4 (for 32bpp).  So we'll always be outputting DWORDS.
            // (No leftover words or bytes, they get clipped)
            //******************************************************************

            BytesPerDstScan = SizeInWidth * PixelDepth/8;
            DwordsPerDstScan = BytesPerDstScan / 4 ;

            //******************************************************************
            // Ptr to pattern data
            //******************************************************************

            SrcBits = (BYTE*) &ppdev->AlignedBrush[0];      // Move to the beginning of destination

            //******************************************************************
            // Get ptr to start of SRC
            //******************************************************************

            NextScan = (ULONG *)(SrcBits);

            //******************************************************************
            // We only ever support 8x8 patterns
            //******************************************************************

            BytesPerSrcScan = 8 * ppdev->cjPelSize;

            //******************************************************************
            // Amount to increment for each scanline (in dwords)
            //******************************************************************

            ScanInc = (BytesPerSrcScan+3) >> 2;      // Number of dwords to increment


            //******************************************************************
            // Output one scan at a time
            //******************************************************************

            PatternYCount = 0;

            //******************************************************************
            // 8bpp
            //******************************************************************

            if (ppdev->cjPelSize == 1)  // 8bpp

                {
                for (yscan=0;yscan < height; yscan++)
                    {
                    DwordCount  = DwordsPerDstScan;
                    ScanPtr     = NextScan;
                    EndOfScanPtr= NextScan + (2 * ppdev->cjPelSize);

                    //**********************************************************
                    // 4 Dwords at a time
                    //**********************************************************

                    while (DwordCount >= 4 )
                        {
                        while (FreeCount < 4*4)
                            FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                        FreeCount -= 4*4;

                        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] = ScanPtr[0];
                        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] = ScanPtr[1];

                        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] = ScanPtr[0];
                        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] = ScanPtr[1];

                        DwordCount-=4;
                        }

                    //**********************************************************
                    // 1 Dword at a time
                    //**********************************************************

                    while (DwordCount > 0 )
                        {
                        while (FreeCount < 1*4)
                            FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                        FreeCount -= 1*4;

                        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] = ScanPtr[0];

                        ScanPtr ++;                  // Advance 1 dwords

                        DwordCount--;

                        //******************************************************
                        // See if we've gone past 8 pixels worth (in dwords)
                        // We only handle 8x8 patterns
                        //******************************************************

                        if (ScanPtr >= EndOfScanPtr)
                            ScanPtr = NextScan;     // Reset back to beginning
                        }


                    //**********************************************************
                    // Advance to next pattern scanline
                    //**********************************************************

                    NextScan+=ScanInc;

                    //**********************************************************
                    // Check if we need to wraparound back to beginning of pattern
                    // If so, reset pattern pointer back to the beginning
                    //**********************************************************

                    PatternYCount++;
                    if (PatternYCount>=8)
                        {
                        NextScan = (ULONG *)SrcBits;
                        PatternYCount=0;
                        }

                    }

                }

            else

                {

                //**************************************************************
                // 16bpp or 32bpp -> Use Fast optimized blit version
                //**************************************************************

                //**************************************************************
                // Use Screen to Screen blits to speed up pattern copy
                // 2 main cases:
                //    Wide blits (greater than/equal 32 pixels wide)
                //    Narrow blits (less than 32 pixels wide),
                //**************************************************************

                if ( (rop3==0xf0) || (rop3==0xcc) )

                    {
                    //**********************************************************
                    // We'll be using image blit, so we need to
                    // set linear stride and offset for the source!
                    // Make sure to set it the same as the destination
                    // (Offscreen bitmap or Onscreen)
                    //**********************************************************

                    SrcOffset = ppdev->CurrentDestOffset;
                    SrcStride = ppdev->CurrentDestPitch;
                    (ppdev->pfnSetSourceBase)(ppdev,SrcOffset,SrcStride);


                    //**********************************************************
                    // Copy the pattern on screen, then use imageblit
                    // to copy the rest of it (faster than imagefromcpu)
                    //**********************************************************

                    yscan=0;
                    RemainingScanlines=height;

                    //**********************************************************
                    // rop3 is a pat rop. Since we don't have color patterns in hw, we
                    // treat pattern as source and thus convert to the corresponding source
                    // rop. e.g. - convert PATCOPY->SRCCOPY, PATINVERT->SRCINVERT.
                    //**********************************************************

                    while (FreeCount < 1*4)
                       FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                    FreeCount -=1*4;

                    nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 =
                        nvBrushToSrcRopTable[rop3 >> 2];

                    //**********************************************************
                    // Do the first 8 scanlines of this pattern blit
                    //**********************************************************

                    while ( (yscan < 8) && (yscan < height))
                        {

                        //******************************************************
                        // Get remaining number of pixels to do on this scanline
                        // Also get start of current pattern scanline
                        //******************************************************

                        RemainingPixels = width;
                        ScanPtr         = NextScan;

                        //******************************************************
                        // Get blit destination coordinate
                        //******************************************************

                        CurrentXdst = xdst;
                        CurrentYdst = ydst+yscan;

                        //******************************************************
                        // Speed up WIDE blits
                        //******************************************************

                        if ( width >=32 )
                            {
                            //**************************************************
                            // Prepare to do an IMAGE_FROM_CPU blit
                            // We'll be using it to copy the first 8 pixels
                            //**************************************************

                            while (FreeCount < 3*4)
                               FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                            FreeCount -=3*4;

                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Point   = ( ((CurrentYdst) <<16) | ((xdst) & 0xffff) );
                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeOut = ( (1 <<16) | 8 );
                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeIn  = ( (1 <<16) | 8 );

                            //**************************************************
                            // Copy first 8 pixels using ImageFromCpu (we only handle 8x8 brushes)
                            //**************************************************


                            if (ppdev->cjPelSize == 2)  // 16bpp
                                {
                                while (FreeCount < 4*4)
                                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                                FreeCount -= 4*4;

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] = ScanPtr[0];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] = ScanPtr[1];

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] = ScanPtr[2];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] = ScanPtr[3];

                                CurrentXdst         += 8;
                                RemainingPixels     -= 8;
                                }
                            else
                                {                       // 32bpp
                                while (FreeCount < 8*4)
                                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                                FreeCount -= 8*4;

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] = ScanPtr[0];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] = ScanPtr[1];

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] = ScanPtr[2];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] = ScanPtr[3];

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[4] = ScanPtr[4];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[5] = ScanPtr[5];

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[6] = ScanPtr[6];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[7] = ScanPtr[7];

                                CurrentXdst         += 8;
                                RemainingPixels     -= 8;
                                }


                            //**************************************************
                            // Set Rop to srccopy
                            //**************************************************

                            while (FreeCount < 1*4)
                                FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;
                            FreeCount -= 1*4;

                            nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = 0xcccc ;

                            //**************************************************
                            // We've already blitted the first 8 pixels
                            // We only need to blit 24 more pixels to create
                            // the 32 pixel group.  ( 8 pixels * 3 times = 24)
                            //**************************************************

                            BUILD_PIXEL_GROUP(24,8,3);

                            //**************************************************
                            // 32pixel group is now available
                            // See if we can create the 128 pixel group
                            //**************************************************

                            BUILD_PIXEL_GROUP(128,32,4);

                            //**************************************************
                            // More than 128 pixels remaining? (7 -> Divide by 128)
                            // If so, we can blit it all at once
                            //**************************************************

                            COPY_PIXEL_GROUP(128, RemainingPixels>>7);

                            //**************************************************
                            // More than 32 pixels remaining? (5 -> Divide by 32)
                            // If so, we can blit it all at once
                            //**************************************************

                            COPY_PIXEL_GROUP(32, RemainingPixels>>5);

                            //**************************************************
                            // More than 8 pixels remaining? (3 -> Divide by 8)
                            // If so, we can blit it all at once
                            //**************************************************

                            COPY_PIXEL_GROUP(8, RemainingPixels>>3);

                            //**************************************************
                            // Check for remaining pixels
                            //**************************************************

                            if (RemainingPixels)
                                {
                                //**********************************************
                                // Copy  remaining pixels
                                //**********************************************

                                while (FreeCount < 3*4)
                                    FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;
                                FreeCount -= 3*4;

                                nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (CurrentYdst << 16) | (xdst & 0xffff) );
                                nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( ( (CurrentYdst ) << 16) | ((CurrentXdst ) & 0xffff) );
                                nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( (1 << 16) | (RemainingPixels) );
                                }
                            }


                        else

                            {

                            //**************************************************
                            // Speed up TALL blits (less than 32 pixels wide)
                            //**************************************************

                            while (FreeCount < 3*4)
                               FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                            FreeCount -=3*4;


                            //**************************************************
                            // Make sure to adjust the SizeInWidth appropriately
                            //**************************************************

                            if (PixelDepth == 16)
                                SizeInWidth = ((width + 1) & 0xfffe);
                            else  // Pixel Depth == 32
                                SizeInWidth = width;

                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Point   = ( ((CurrentYdst) <<16) | ((xdst) & 0xffff) );
                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeOut = ( (1 <<16) | (width) );
                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeIn  = ( (1 <<16) | (SizeInWidth) );

                            //**************************************************
                            // Copy pixels using ImageFromCpu
                            // CAREFUL: !! FreeCount can NOT go past 128-4 bytes !!
                            //             (Maximum width here is 31)
                            //**************************************************


                            if (ppdev->cjPelSize == 2)  // 16bpp
                                {
                                while (FreeCount < (USHORT)(SizeInWidth>>1)*4)
                                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                                FreeCount -= (USHORT)(SizeInWidth>>1)*4;

                                for (i=0;i<(SizeInWidth>>1);i++)
                                    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[i] = ScanPtr[(i & 0x3)];
                                }
                            else
                                {                       // 32bpp
                                while (FreeCount < (USHORT)width*4)
                                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                                FreeCount -= (USHORT)width*4;

                                for (i=0;i<width;i++)
                                    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[i] = ScanPtr[(i & 0x7)];
                                }

                            }


                        //******************************************************
                        // Advance to next pattern scanline
                        //******************************************************

                        NextScan+=ScanInc;

                        //******************************************************
                        // Check if we need to wraparound back to beginning of pattern
                        // If so, reset pattern pointer back to the beginning
                        //******************************************************

                        PatternYCount++;
                        if (PatternYCount>=8)
                            {
                            NextScan = (ULONG *)SrcBits;
                            PatternYCount=0;
                            }

                        //******************************************************
                        // Advance to next scanline
                        //******************************************************

                        yscan++;
                        RemainingScanlines--;
                        }


                    //**********************************************************
                    // Only need to reset the ROP if this is the tall case
                    //**********************************************************

                    if ( width < 32 )
                        {
                        //******************************************************
                        // Set Rop to srccopy
                        //******************************************************

                        while (FreeCount < 1*4)
                            FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;
                        FreeCount -= 1*4;

                        nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = 0xcccc ;
                        }

                    //**********************************************************
                    // If there are more then 8 scanlines, use imageblit to
                    // blit the rest
                    //**********************************************************

                    if (RemainingScanlines)

                        {
                        //******************************************************
                        // Update CurrentX and CurrentY
                        //******************************************************

                        CurrentXdst = xdst;
                        CurrentYdst = ydst+yscan;

                        //******************************************************
                        // We've already blitted the first 8 scanlines.
                        // We only need to blit 24 more scanlines to create
                        // the 32 pixel block.  ( 8 pixels * 3 times = 24)
                        //******************************************************

                        BUILD_PIXEL_BLOCK(24,width,8,3);

                        //******************************************************
                        // 32pixel group is now available
                        // See if we can create the 128 pixel block
                        //******************************************************

                        BUILD_PIXEL_BLOCK(128,width,32,4);

                        //******************************************************
                        // More than 128 scanlines remaining? (7 -> Divide by 128)
                        // If so, we can blit it all at once
                        //******************************************************

                        COPY_PIXEL_BLOCK(128, width, RemainingScanlines>>7);

                        //******************************************************
                        // More than 32 scanlines remaining? (5 -> Divide by 32)
                        // If so, we can blit it all at once
                        //******************************************************

                        COPY_PIXEL_BLOCK(32, width, RemainingScanlines>>5);

                        //******************************************************
                        // More than 8 scanlines remaining? (3 -> Divide by 8)
                        // If so, we can blit it all at once
                        //******************************************************

                        COPY_PIXEL_BLOCK(8, width, RemainingScanlines>>3);

                        //******************************************************
                        // Check for remaining scanlines
                        //******************************************************

                        if (RemainingScanlines)
                            {
                            //**************************************************
                            // Copy remaining Scanlines
                            //**************************************************

                            while (FreeCount < 3*4)
                                FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;
                            FreeCount -= 3*4;

                            nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (ydst << 16) | ((CurrentXdst) & 0xffff) );
                            nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( ( (CurrentYdst ) << 16) | ((CurrentXdst ) & 0xffff) );
                            nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( ((RemainingScanlines) << 16) | (width) );
                            }


                        }


                    }


                else


                    {

                    //**********************************************************
                    // 16bpp or 32bpp -> Use slower ImageFromCpu version
                    //**********************************************************

                    for (yscan=0;yscan < height; yscan++)
                        {

                        //******************************************************
                        // Init values per each pattern scanline
                        // 8bpp  -> 8 pixels * 1 dword/4 pixels = 2 dwords per pattern scanline
                        // 16bpp -> 8 pixels * 1 dword/2 pixels = 4 dwords per pattern scanline
                        // 32bpp -> 8 pixels * 1 dword/1 pixel  = 8 dwords per pattern scanline
                        //    or just multiply 2 * Bytes Per Pixel for the same result
                        //******************************************************

                        DwordCount  = DwordsPerDstScan;
                        ScanPtr     = NextScan;
                        EndOfScanPtr= NextScan + (2 * ppdev->cjPelSize);

                        //******************************************************
                        // 8x8 color pattern
                        // We can only send 4 dwords at a time for 16bpp
                        // But we can blast out 8 dwords at a time for 32bpp
                        // (16bpp-> 2 pixels per dword, 32bpp -> 1 pixel per dword)
                        //******************************************************

                        if (ppdev->cjPelSize == 4)
                            {
                            //**************************************************
                            // 8 Dwords at a time (Valid for 32bpp mode ONLY!!)
                            //**************************************************

                            while (DwordCount >= 8 )
                                {
                                while (FreeCount < 8*4)
                                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                                FreeCount -= 8*4;

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] = ScanPtr[0];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] = ScanPtr[1];

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] = ScanPtr[2];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] = ScanPtr[3];

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[4] = ScanPtr[4];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[5] = ScanPtr[5];

                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[6] = ScanPtr[6];
                                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[7] = ScanPtr[7];

                                DwordCount-=8;
                                }
                            }


                        //******************************************************
                        // 4 Dwords at a time
                        //******************************************************

                        while (DwordCount >= 4 )
                            {
                            while (FreeCount < 4*4)
                                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                            FreeCount -= 4*4;

                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] = ScanPtr[0];
                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] = ScanPtr[1];

                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] = ScanPtr[2];
                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] = ScanPtr[3];

                            DwordCount-=4;
                            }

                        //******************************************************
                        // Output any remaining dwords...
                        // Need to unroll and make more efficient !!!
                        //******************************************************

                        while (DwordCount > 0 )
                            {

                            while (FreeCount < 1*4)
                                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                            FreeCount -= 1*4;

                            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] = ScanPtr[0];

                            ScanPtr ++;                  // Advance 1 dwords

                            DwordCount--;

                            //**************************************************
                            // See if we've gone past 8 pixels worth (in dwords)
                            // We only handle 8x8 patterns
                            //**************************************************

                            if (ScanPtr >= EndOfScanPtr)
                                ScanPtr = NextScan;     // Reset back to beginning
                            }

                        //******************************************************
                        // Advance to next pattern scanline
                        //******************************************************

                        NextScan+=ScanInc;

                        //******************************************************
                        // Check if we need to wraparound back to beginning of pattern
                        // If so, reset pattern pointer back to the beginning
                        //******************************************************

                        PatternYCount++;
                        if (PatternYCount>=8)
                            {
                            NextScan = (ULONG *)SrcBits;
                            PatternYCount=0;
                            }

                        } // for yscan...


                    }  // slow image from cpu ...


                }  // 32bpp


            //******************************************************************
            // Onto the next clipping rectangle
            //******************************************************************

            prcl++;

            } while (--c != 0);

        } // Multi-color pattern

    //**************************************************************************
    // Update the global free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;

    } // End of FillPattern routine



//******************************************************************************
//
//  Function:   NV3MonoScanlineImageTransfer
//
//  Routine Description:
//
//       Low-level routine for transferring a bitmap image via RENDER_GDI0_RECT_AND_TEXT
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

VOID NV3MonoScanlineImageTransfer(
PDEV*   ppdev,
BYTE*   pjSrc,              // Source pointer
ULONG   NumDwordsPerScan,
USHORT* FreeCountPtr)


    {
    Nv3ChannelPio  *nv  = (Nv3ChannelPio *)ppdev->pjMmBase;
    USHORT FreeCount;
    ULONG MonoPixelData1;
    ULONG MonoPixelData2;
    ULONG *SrcDataPtr;

    //**************************************************************************
    // Init FreeCount variable
    //**************************************************************************

    FreeCount=(*FreeCountPtr);

    //**************************************************************************
    // Get Dword Ptr to Src Data
    //**************************************************************************

    SrcDataPtr = (DWORD *)pjSrc;

    //**************************************************************************
    // Handle groups of 16 dwords at a time
    //**************************************************************************

    while (NumDwordsPerScan>= 16)
        {

        while (FreeCount < 16*4)
           FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
        FreeCount -= 16*4;

        MonoPixelData1 = SrcDataPtr[0];
        MonoPixelData2 = SrcDataPtr[1];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[0] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[1] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[2];
        MonoPixelData2 = SrcDataPtr[3];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[2] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[3] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[4];
        MonoPixelData2 = SrcDataPtr[5];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[4] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[5] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[6];
        MonoPixelData2 = SrcDataPtr[7];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[6] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[7] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[8];
        MonoPixelData2 = SrcDataPtr[9];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[8] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[9] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[10];
        MonoPixelData2 = SrcDataPtr[11];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[10] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[11] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[12];
        MonoPixelData2 = SrcDataPtr[13];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[12] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[13] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[14];
        MonoPixelData2 = SrcDataPtr[15];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[14] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[15] = MonoPixelData2;

        NumDwordsPerScan-=16;
        SrcDataPtr += 16;

        }

    //**************************************************************************
    // Handle groups of 8 dwords at a time
    //**************************************************************************

    while (NumDwordsPerScan>= 8)
        {

        while (FreeCount < 8*4)
           FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
        FreeCount -= 8*4;

        MonoPixelData1 = SrcDataPtr[0];
        MonoPixelData2 = SrcDataPtr[1];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[0] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[1] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[2];
        MonoPixelData2 = SrcDataPtr[3];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[2] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[3] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[4];
        MonoPixelData2 = SrcDataPtr[5];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[4] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[5] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[6];
        MonoPixelData2 = SrcDataPtr[7];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[6] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[7] = MonoPixelData2;

        NumDwordsPerScan -=8;
        SrcDataPtr += 8;

        }

    //**************************************************************************
    // Handle groups of 4 dwords at a time
    //**************************************************************************

    while (NumDwordsPerScan>= 4)
        {

        while (FreeCount < 4*4)
           FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
        FreeCount -= 4*4;

        MonoPixelData1 = SrcDataPtr[0];
        MonoPixelData2 = SrcDataPtr[1];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[0] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[1] = MonoPixelData2;

        MonoPixelData1 = SrcDataPtr[2];
        MonoPixelData2 = SrcDataPtr[3];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[2] = MonoPixelData1;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[3] = MonoPixelData2;

        NumDwordsPerScan-=4;
        SrcDataPtr += 4;

        }

    //**************************************************************************
    // Handle any remaining dwords
    //**************************************************************************

    while (NumDwordsPerScan > 0)
        {
        while (FreeCount < 4)
            FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
        FreeCount -= 4;

        MonoPixelData1 = SrcDataPtr[0];
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.MonochromeColor01E[0] = MonoPixelData1;

        NumDwordsPerScan--;

        SrcDataPtr ++;
        }

    //**************************************************************************
    // Update cached FreeCount variable
    //**************************************************************************

    *FreeCountPtr = FreeCount;
    }





//******************************************************************************
//
//  Function:   NV3Xfer1bpp
//
//  Routine Description:
//
//     This routine colour expands a monochrome bitmap, possibly with different
//     Rop2's for the foreground and background.  It will be called in the
//     following cases:
//
//     1) To colour-expand the monochrome text buffer for the vFastText routine.
//     2) To blt a 1bpp source with a simple Rop2 between the source and
//        destination.
//  x  3) To blt a true Rop3 when the source is a 1bpp bitmap that expands to
//        white and black, and the pattern is a solid colour.
//     4) To handle a true Rop4 that works out to be two Rop2's between the
//        pattern and destination.
//
//     Needless to say, making this routine fast can leverage a lot of
//     performance.
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

VOID NV3Xfer1bpp(       // Type FNXFER
PDEV*       ppdev,
LONG        c,          // Count of rectangles, can't be zero
RECTL*      prcl,       // List of destination rectangles, in relative
                        //   coordinates
ROP4        rop4,       // rop4
SURFOBJ*    psoSrc,     // Source surface
POINTL*     pptlSrc,    // Original unclipped source point
RECTL*      prclDst,    // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG       PatternColor,     // Solid Color of pattern
BLENDOBJ*   pBlendObj)

    {
    ULONG   ulHwForeMix;
    ULONG   ulHwBackMix;
    LONG    dxSrc;
    LONG    dySrc;
    LONG    cx;
    LONG    cy;
    LONG    lSrcDelta;
    BYTE*   pjSrcScan0;
    BYTE*   pjSrc;
    LONG    cjSrc;
    LONG    xLeft;
    LONG    yTop;
    LONG    xBias;

    Nv3ChannelPio  *nv  = (Nv3ChannelPio *)ppdev->pjMmBase;
    ULONG Color0;
    ULONG Color1;
    ULONG AlphaEnableValue;
    ULONG NumBytesPerScan;
    ULONG NumDwordsPerScan;
    ULONG OffsetOfLastDword;
    ULONG LastDword;
    ULONG PixelDepth;
    ULONG width;
    ULONG height;
    USHORT FreeCount;
    ULONG SizeInWidth;
    ULONG rop3;
    ULONG ClipLeft;
    ULONG ClipTop;
    ULONG ClipWidth;
    ULONG ClipHeight;
    RECTL DstRect;

    //**************************************************************************
    // Make sure following conditions are true before continuing on
    //**************************************************************************

    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(pptlSrc != NULL && psoSrc != NULL, "Can't have NULL sources");
    ASSERTDD(((((rop4 & 0xff00) >> 8) == (rop4 & 0xff)) || (rop4 == 0xaacc)),
             "Expect weird rops only when opaquing");

    //**************************************************************************
    // Get pixel depth for the current mode.
    // Then determine the ALPHA_CHANNEL enable value
    //**************************************************************************

    AlphaEnableValue = ppdev->AlphaEnableValue;

    //**************************************************************************
    // Init foreground and background color
    //**************************************************************************

    Color0 = pxlo->pulXlate[0] | (AlphaEnableValue); // Background color
    Color1 = pxlo->pulXlate[1] | (AlphaEnableValue); // Foreground color

    //**************************************************************************
    // Find the pitch and starting location of the source mono (pattern) bitmap
    //
    // NOTE:  Normally, we'd need to check if this source SURFOBJ was a
    //        standard DIB or a DFB that was moved to system memory.
    //        However, since we don't store 1bpp SURFOBJs in offscreen memory
    //        (see drvCreateDeviceBitmap), we know that the this source SURFOBJ
    //        is just a plain standard DIB. (i.e.  we don't need to get the
    //        'lDelta' and 'pvScan0' values from psoSrc->dhsurf->pso->lDelta
    //        and psoSrc->dhsurf->pso->pvScan0.
    //
    //**************************************************************************

    lSrcDelta  = psoSrc->lDelta;
    pjSrcScan0 = psoSrc->pvScan0;

    //**************************************************************************
    // Safety check  ...check and see if source bitmap is dword aligned
    // This is a requirement....bitmap should never start on non-dword alignment..
    // Otherwise, that will invalidate some assumptions made...we currently
    // always read dword multiples...at dword alignments...if source bitmap
    // doesn't start on a dword boundary, then we might not be able to get
    // the first dword of data on a dword boundary ...Send back to DIBengine?
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) pjSrcScan0 & 0x3), "Unaligned source bitmap");

    //**********************************************************************
    // Init FreeCount variable
    //**********************************************************************

    FreeCount = ppdev->NVFreeCount;

    //**********************************************************************
    // Adjust dest rect in case it is in offscreen memory
    //**********************************************************************

    DstRect.top = prclDst->top;
    DstRect.left = prclDst->left;
    DstRect.right = prclDst->right;
    DstRect.bottom = prclDst->bottom;

    do  {
        //**********************************************************************
        //
        //  First, we need to determine the XBIAS required (where the starting bit
        //  of the monochrome source bitmap is located).  XBIAS is affected
        //  by 3 values as follows:
        //
        //          1) pptlSrc->x    ( value x1 shown below )
        //
        //                  This is simply the bit at which we want to
        //                  start in the monochrome pattern bitmap
        //
        //          2) prclDst->x    ( value x2 shown below )
        //
        //                  Location of the original unclipped destination rectangle
        //
        //          3) prcl->x       ( value x3 shown below)
        //
        //                  Current clip rectangle with coordinates relative
        //                  to prclDst
        //
        //  Screen Origin
        //  +--------------------------------------------------------------->
        //  |
        //  |
        //  |           x2 = prclDst->left
        //  |           +-----------------------------------------------
        //  |           |                             ^                 |-Original
        //  |           |                             | dySrc           |  Unclipped
        //  |           |                             |                 |   Destination
        //  |           |         x3 = prcl->left     v                 |    Rectangle
        //  |           |         +------------       -                 |
        //  |           |         |            |                        |
        //  |           |<-dxSrc->|            |                        |
        //  |           |         |            |                        |
        //  |           |         |            |-Current                |
        //  |           |         |            |  Clip                  |
        //  |           |          ------------    Rectangle            |
        //  |           |                                               |
        //  |           |                                               |
        //  |           |                                               |
        //  |            -----------------------------------------------
        //  |
        //  v
        //
        //            Source MonoChrome Bitmap
        //          +--------+--------+--------+------->
        //          |00011001|00010010|00010011|
        //          |00011001|00010010|00010011|
        //          |00011001|00010010|00010011|
        //          |00011001|00010010|00010011|
        //          |           ^
        //          v           |
        //                      |
        //                      |
        //          |-- x1 ---->|
        //
        //**********************************************************************

        //**********************************************************************
        //
        // Once we determine the starting bit in the monochrome bitmap,
        // we can calculate the XBIAS value.  The left portion of the 1st source
        // dword that we will read will end up getting clipped by hardware.
        // This will allow us to NOT have to align subsequent source data.
        //
        //    xbias =  ( x1 + (x3 - x2) ) & 0x1f
        //
        //    xbias =  ( x1 + (dxSrc)   ) & 0x1f  ( starting bit position in dword)
        //                                                 from 0 - 31
        //
        //           --------------------------
        //          |00011001|00010010|00010011|  1st dword of source data
        //           --------------------------
        //          |               |          |
        //          |<--- xbias --->|          |
        //          |               |   This   |
        //          | This region is| region is|
        //          |   clipped     |  drawn   |
        //          v               v          v
        //
        //**********************************************************************

        //**********************************************************************
        // Set initial destination coordinates of blit
        //**********************************************************************

        yTop  = prcl->top;
        xLeft = prcl->left;

        //**********************************************************************
        // Init dimensions of clip rectangle.
        //**********************************************************************

        ClipTop = prcl->top;
        ClipLeft = prcl->left;

        ClipHeight = prcl->bottom - prcl->top;
        ClipWidth = prcl->right - prcl->left;

        //**********************************************************************
        // Find starting bit location of pattern.  We really only care about
        // the dword alignment (bottom 5 bits) of this value.
        //**********************************************************************

        dxSrc = prcl->left - DstRect.left;
        dySrc = prcl->top  - DstRect.top;

        //**********************************************************************
        // Determine the bias of the bitmap
        //**********************************************************************

        xBias = ( (pptlSrc->x + dxSrc) & 0x1f);     // Value from 0 thru 31
        if (xBias != 0)
            {
            //******************************************************************
            // NV: Set the hardware clip rectangle so we don't have to
            //     manually align the pattern bitmap.  Then make sure
            //     to adjust the starting point of the blit to account
            //     for this xbias.
            //
            // We should probably remove the  if (xBias !=0) statement
            // since it's not necessary.
            //******************************************************************

            xLeft -= xBias;
            }

        //**********************************************************************
        // Get width and height of the destination rectangle
        // Then calculate the DWORDS we'll send to the NV engine
        // (we always output DWORDS at a time), and the number of bytes per scan
        // SizeInWidth will always be a multiple of 32 pixels (1 dword's worth)
        //
        // Remember, xLeft has now been adjusted to account for the XBias value
        //**********************************************************************

        width = prcl->right - xLeft;
        height= prcl->bottom - yTop;

        //**********************************************************************
        // Get values required by NV hardware engine
        //**********************************************************************

        SizeInWidth = (width + 31) & 0xffe0;    // Always a multiple of 32 pixels
        NumBytesPerScan = SizeInWidth >> 3;     // Divide mono pixels by 8 to get bytes


        //**********************************************************************
        // MUST set the pattern appropriately!
        //**********************************************************************

        while (FreeCount < 5*4)
           FreeCount = nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].control.Free;
        FreeCount -= 5*4;

        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeShape = NV018_SET_MONOCHROME_SHAPE_64X_1Y;
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeColor0 = ( PatternColor | AlphaEnableValue );
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeColor1 = ( PatternColor | AlphaEnableValue );
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromePattern0 = 0xffffffff;
        nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromePattern1 = 0xffffffff;


        //**********************************************************************
        // Get Rop3 value from Rop4
        //**********************************************************************

        rop3 = (ULONG)((BYTE)rop4);

        while (FreeCount < 1*4)
            FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
        FreeCount -= 1*4;

        nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = rop3;

        //**********************************************************************
        // Prep the monster class text object (rendergdi0RectAndText)
        //**********************************************************************

        while (FreeCount < 7*4)
            FreeCount = nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].control.Free;
        FreeCount -= 7*4;

        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.ClipPoint0E = ((ClipTop << 16) | (ClipLeft & 0xffff) );
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.ClipPoint1E = (((ClipTop+ClipHeight)<<16) | (ClipLeft+ClipWidth));

        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.Color0E = Color0;
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.Color1E = Color1;

        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.SizeInE = ( (height <<16) | SizeInWidth );
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.SizeOutE = ( (height <<16) | width );
        nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nv3GdiRectangleText.PointE =  ( (yTop <<16) | (xLeft & 0xffff) );

        //**********************************************************************
        // Get starting byte address of source pattern bitmap
        // Once we have the starting byte address, we can get the starting dword address
        //
        // ?: Is it possible that pptlSrc->y + dySrc will go past
        //    end of Source Bitmap??
        //**********************************************************************

        pjSrc = pjSrcScan0 + (pptlSrc->y + dySrc) * lSrcDelta
                           + ((pptlSrc->x + dxSrc) >> 3);

        (ULONG_PTR)pjSrc &= ~0x3;

        //**********************************************************************
        // Get values pertinent for each scanline
        //**********************************************************************

        NumDwordsPerScan = SizeInWidth / 32;
        OffsetOfLastDword = NumDwordsPerScan << 2;

        //**********************************************************************
        //
        // For performance, we have 2 main objectives as follows:
        //
        //   a) When reading src data, we'd like it to be dword aligned
        //   b) When outputting data, we'd like to output dwords as much as possible
        //
        // With that in mind, we accomplish objective (a) by calculating an
        // xbias value and reading the dword that the first byte of source data
        // is contained in.  That is, we'll grab the first dword of source data
        // and clip the left part of it using the hardware clip rectangle.
        // As a result, any subsequent reads of src data will not require
        // alignment.
        //
        // Example:
        //          Src data begins here --+
        //          at this bit            |
        //                                 v
        //           -----------------------------------
        //          |        |        |        |        |   1st dword of src data
        //           -----------------------------------
        //            byte 0    byte1    byte2    byte3
        //
        //          |<------ xbias ------->|
        //                                 |
        //                                 |<-- used -->|
        //                                 |    data
        //          |<- hardware clipped ->|
        //
        //
        // Objective (b) is accomplished by examining how many dwords we
        // need to output.  The key here is to make sure we don't read past
        // the end of the source bitmap. If the number of bytes we need to
        // output is a dword multiple, then we don't need to worry about it.
        // However, if it's not, we need to check the last couple of bytes
        // and not read past the end of the src data. Otherwise, we'll
        // generate a protection fault. This is done simply by using
        // a switch statement that handles 0,1,2, or 3 remaining bytes of src data.
        //
        //          Switch (Number Of src bytes left)
        //              0:  We are dword aligned already
        //              1:  Just read 1 more byte
        //              2:  Just read 2 more bytes
        //              3:  just read 3 more bytes
        //
        //**********************************************************************


        //**********************************************************************
        // Get src data, depending on how many bytes we will output
        // TO DO:  Since we now read the starting dword of the source bitmap
        //         (instead of the starting byte), isn't it true that
        //         we don't need to worry about straggling bytes going past
        //         end of bitmap...i.e. bitmaps are always allocated as
        //         multiples of dwords?  VERIFY!
        //
        //**********************************************************************


        switch (NumBytesPerScan & 3)
            {

            //******************************************************************
            // Output dwords at a time, then output last 3 bytes of data (in last dword)
            //******************************************************************

            case 3:
                do  {
                    if (NumDwordsPerScan > 0)
                        NV3MonoScanlineImageTransfer(ppdev, pjSrc, NumDwordsPerScan,&FreeCount);

                    //**********************************************************
                    // Store bytes in reverse order (not necessary for NV?)
                    //**********************************************************

                    LastDword = (ULONG) (*(pjSrc + OffsetOfLastDword))  |
                            (*(pjSrc + OffsetOfLastDword + 1) << 8) |
                            (*(pjSrc + OffsetOfLastDword + 2) << 16);

                    NV3MonoScanlineImageTransfer(ppdev, (BYTE *)&LastDword, 1,&FreeCount);
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            //******************************************************************
            // Output dwords at a time, then output last 2 bytes of data (in last dword)
            //******************************************************************

            case 2:
                do  {
                    if (NumDwordsPerScan > 0)
                        NV3MonoScanlineImageTransfer(ppdev, pjSrc, NumDwordsPerScan,&FreeCount);

                    //**********************************************************
                    // Store bytes in reverse order (not necessary for NV?)
                    //**********************************************************

                    LastDword = (ULONG) (*(pjSrc + OffsetOfLastDword))  |
                            (*(pjSrc + OffsetOfLastDword + 1) << 8);

                    NV3MonoScanlineImageTransfer(ppdev, (BYTE *)&LastDword, 1,&FreeCount);
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            //******************************************************************
            // Output dwords at a time, then output last byte of data (in last dword)
            //******************************************************************

            case 1:
                do  {
                    if (NumDwordsPerScan > 0)
                        NV3MonoScanlineImageTransfer(ppdev, pjSrc, NumDwordsPerScan,&FreeCount);

                    //**********************************************************
                    // Store bytes in reverse order (not necessary for NV?)
                    //**********************************************************

                    LastDword = (ULONG) (*(pjSrc + OffsetOfLastDword));

                    NV3MonoScanlineImageTransfer(ppdev, (BYTE *)&LastDword, 1,&FreeCount);
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            //******************************************************************
            // The amount of data we will send is already a multiple of 4.
            // We will blast out all data in dwords
            //******************************************************************

            case 0:
                do  {
                    NV3MonoScanlineImageTransfer(ppdev, pjSrc,NumDwordsPerScan,&FreeCount);
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            } // Switch (NumBytesPerScan)

        prcl++;
        } while (--c != 0); // Next Clipping rectangle


    //**************************************************************************
    // Update global free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;

    }


//******************************************************************************
//
//  Function:   NV3_Send_Data
//
//  Routine Description:
//
//       Low-level routine for transferring a bitmap image via IMAGE_MONO_FROM_CPU
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

VOID NV3_Send_Data(
PDEV*   ppdev,
ULONG*  SrcDataPtr,              // Source pointer
ULONG   PixelCount,
USHORT* FreeCountPtr)


    {
    Nv3ChannelPio  *nv  = (Nv3ChannelPio *)ppdev->pjMmBase;
    USHORT FreeCount;
    ULONG TempPixelData1;
    ULONG TempPixelData2;
    ULONG DwordCount;

    DwordCount = (PixelCount+3)/4;

    //**************************************************************************
    // Read FreeCount for the first time
    //**************************************************************************

    FreeCount = *FreeCountPtr;

    //**************************************************************************
    // Output next group of 16 dwords
    //**************************************************************************

    while (DwordCount >= 16)
        {
        DwordCount -=16;

        while (FreeCount < 16*4)
            FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
        FreeCount -=16*4;

        TempPixelData1 = SrcDataPtr[0];
        TempPixelData2 = SrcDataPtr[1];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[2];
        TempPixelData2 = SrcDataPtr[3];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[4];
        TempPixelData2 = SrcDataPtr[5];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[4] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[5] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[6];
        TempPixelData2 = SrcDataPtr[7];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[6] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[7] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[8];
        TempPixelData2 = SrcDataPtr[9];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[8] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[9] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[10];
        TempPixelData2 = SrcDataPtr[11];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[10] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[11] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[12];
        TempPixelData2 = SrcDataPtr[13];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[12] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[13] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[14];
        TempPixelData2 = SrcDataPtr[15];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[14] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[15] =  TempPixelData2;

        SrcDataPtr +=16;           // Advance 16 dwords
        }

    //**************************************************************************
    // Output next group of 4 dwords
    //**************************************************************************

    while (DwordCount >= 4)
        {
        DwordCount -=4;

        while (FreeCount < 4*4)
            FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
        FreeCount -=4*4;

        TempPixelData1 = SrcDataPtr[0];
        TempPixelData2 = SrcDataPtr[1];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
        TempPixelData1 = SrcDataPtr[2];
        TempPixelData2 = SrcDataPtr[3];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;

        SrcDataPtr +=4;            // Advance 4 dwords

        }


    //**************************************************************************
    // Output any remaining dwords
    //**************************************************************************

    while (DwordCount >0 )
        {
        DwordCount--;

        while (FreeCount < 4)
            FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
        FreeCount -= 4;

        TempPixelData1 = SrcDataPtr[0];
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;

        SrcDataPtr ++;             // Advance 1 dword

        }
    *FreeCountPtr = FreeCount;
    }


//******************************************************************************
//
//  Function:   NV3Xfer4bpp
//
//  Routine Description:
//
//      Does a 4bpp transfer from a bitmap to the screen.
//
//      NOTE: The screen must be 8bpp for this function to be called!
//
//      The reason we implement this is that a lot of resources are kept as 4bpp,
//      and used to initialize DFBs, some of which we of course keep off-screen.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


//******************************************************************************
// XLATE_BUFFER_SIZE defines the size of the stack-based buffer we use
// for doing the translate.  Note that in general stack buffers should
// be kept as small as possible.  The OS guarantees us only 8k for stack
// from GDI down to the display driver in low memory situations; if we
// ask for more, we'll access violate.  Note also that at any time the
// stack buffer cannot be larger than a page (4k) -- otherwise we may
// miss touching the 'guard page' and access violate then too.
//
// NV_HALF_BUFFER_SIZE -> NV fifo can hold up to 32 DWORDS (128 bytes)
//                        Here, we output the data when we've reach half of that
//******************************************************************************

#define XLATE_BUFFER_SIZE 256
#define NV_HALF_FIFO_SIZE 16*4

VOID NV3Xfer4bpp(     // Type FNXFER
PDEV*       ppdev,
LONG        c,          // Count of rectangles, can't be zero
RECTL*      prcl,       // Array of relative coordinates destination rectangles
ROP4        rop4,       // rop4
SURFOBJ*    psoSrc,     // Source surface
POINTL*     pptlSrc,    // Original unclipped source point
RECTL*      prclDst,    // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG PatternColor,       // Not used
BLENDOBJ*   pBlendObj)

    {
    LONG    dx;
    LONG    dy;
    LONG    cx;
    LONG    cy;
    LONG    lSrcDelta;
    BYTE*   pjSrcScan0;
    BYTE*   pjSrc;
    LONG    cjSrc;

    Nv3ChannelPio   *nv = (Nv3ChannelPio *)ppdev->pjMmBase;
    ULONG depth;
    LONG BytesPerSrcScan;
    ULONG Rop3;
    ULONG temp;
    ULONG xsrc,ysrc;
    ULONG xdst,ydst;
    BYTE *NextScan;
    ULONG yscan;
    ULONG WordCount;
    BYTE *ScanPtr;
    LONG ScanInc;
    USHORT FreeCount;
    ULONG OnePixel;
    BYTE *DstBits;
    ULONG SizeInWidth;
    ULONG WordsPerDstScan;
    ULONG clip_x,clip_y;
    ULONG clip_width,clip_height;
    BYTE TempPixelData[XLATE_BUFFER_SIZE];
    DSURF*          pdsurfSrc;
    ULONG*  pulXlate;
    BYTE  PixelDataByte;
    ULONG PixelDataDword;
    ULONG   LoopCount;
    ULONG PixelCount;
    ULONG i;
    ULONG DwordCheck;

    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**********************************************************************
    // Get ready to translate the 4bpp colors
    //**********************************************************************

    ASSERTDD(pxlo != NULL, "Xfer4BPP, NULL XLATEOBJ");
    pulXlate  =  pxlo->pulXlate;

    //**************************************************************************
    // Get pointer to start of source bitmap
    // Need to be careful about this!  This function gets called
    // when copying a system memory bitmap to VRAM.  However, the SOURCE
    // surfobj can be a standard DIB or it can be a DFB that's been moved
    // from offscreen VRAM to system memory.  If this is the case, then
    // we need to make sure and get the source ptr from the correct place.
    //**************************************************************************

    pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    //**************************************************************************
    // If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the source ptr from pdsurfSrc->pso->pvScan0.
    // Otherwise, just get it from psoSrc->pvScan0 (standard DIB)
    //**************************************************************************

    if (pdsurfSrc != NULL)
        pjSrcScan0 = pdsurfSrc->pso->pvScan0;
    else
        pjSrcScan0 = psoSrc->pvScan0;

    //**************************************************************************
    // Verify that src bitmap is DWORD aligned.  As far as I can tell, this
    // appears to always be the case.
    //**************************************************************************
    ASSERTDD(!((ULONG_PTR) pjSrcScan0 & 0x3),
        "Unaligned source bitmap");// ACK!....src bitmap is not dword aligned !!!

    //**************************************************************************
    // Get Rop3 value
    //**************************************************************************

    Rop3 = (rop4 & 0xff);               // Get bottom byte

    //**************************************************************************
    // Current pixel depth
    //**************************************************************************

    depth = ppdev->cBitsPerPel;

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    // CAREFUL: If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the 'ldelta' value from pdsurfSrc->pso->lDelta.  Source DFB
    // will NEVER come from offscreen VRAM (DT_SCREEN) so we don't have to even
    // check for dt == DT_DIB.  Otherwise, if we're dealing with a standard
    // system memory DIB, just get 'lDelta' from psoSrc->lDelta.
    //**************************************************************************

    if (pdsurfSrc != NULL)
        BytesPerSrcScan = pdsurfSrc->pso->lDelta;
    else
        BytesPerSrcScan = psoSrc->lDelta;

    //**************************************************************************
    // Verify that src bitmap pitch is a multiple of 4 (for dwords)
    //**************************************************************************
    ASSERTDD(!((ULONG) BytesPerSrcScan & 0x3),
        "Pitch is not dword aligned"); // ACK!....pitch is not dword aligned !!!

    //**************************************************************************
    // Amount to increment for each scanline
    //**************************************************************************

    ScanInc = BytesPerSrcScan ;      // Number of bytes to increment

    //**************************************************************************
    // Read FreeCount for the first time
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

   //***************************************************************************
   // Reset clipping rectangle to full screen extents if necessary
   // Resetting the clipping rectangle causes delays so we want to do it
   // as little as possible!
   //***************************************************************************

   if (ppdev->NVClipResetFlag)
       {
       while (FreeCount < 2*4)
          FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
       FreeCount -= 2*4;

       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((0 << 16) | 0 );
       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (((ppdev->cyMemory)<<16) | (0x7fff));

       ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
       }

    //**************************************************************************
    // Blit for each clipping rectangle
    //**************************************************************************

    while(TRUE)
        {

        //**********************************************************************
        // Top left coordinate of source bitmap on source surface
        //**********************************************************************

        xsrc = pptlSrc->x + prcl->left - prclDst->left;
        ysrc = pptlSrc->y + prcl->top  - prclDst->top;

        //**********************************************************************
        // Get ptr to start of SRC (Remember, there are 2 pixels per byte!)
        //**********************************************************************

        NextScan = (&(pjSrcScan0[(xsrc>>1) + ysrc*BytesPerSrcScan]));

        //**********************************************************************
        // Top left coordinate of destination on destination surface
        //**********************************************************************

        clip_x = prcl->left;
        clip_y = prcl->top;

        //**********************************************************************
        // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
        // That is, the lower right coordinate is NOT included
        // For NV, the width/height actually means the number of
        // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
        // when calculating width and height.
        //**********************************************************************

        clip_width = prcl->right - prcl->left;
        clip_height = prcl->bottom - prcl->top;

        while (FreeCount < 4*4)
            FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
        FreeCount -=4*4;

        //**********************************************************************
        // Setup NV Blit parameters for CPU to SCREEN blit
        //**********************************************************************

        nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = Rop3 ;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Point =  ( ( (clip_y) <<16) | ((clip_x) & 0xffff) );
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeOut = ( (clip_height <<16) | clip_width );

        //**********************************************************************
        // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
        // ignore/clip the extra pixel that we send it, so that it won't
        // get included as part of the next scanline. Since we always
        // send DWords at a time, we may actually send more data to the NV
        // engine than is necessary.  So clip it by specifying a larger SizeIn
        // That is, we always send DWORDS at a time for each scanline.
        // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
        //**********************************************************************

        if (depth == 8)
            SizeInWidth = ((clip_width + 3) & 0xfffc);
        else if (depth == 16)
            SizeInWidth = ((clip_width + 1) & 0xfffe);
        else
            SizeInWidth = clip_width;

        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeIn = ( (clip_height <<16) | (SizeInWidth) );

        //******************************************************************
        //
        // 4bpp Source Bitmap:
        //
        // nibble| hi lo | hi  lo| hi lo | hi lo | hi  lo|
        //        -----------------------------------------------
        //       | Byte0 | Byte1 | Byte2 | Byte3 | Byte4 | etc..
        //        -----------------------------------------------
        // Pixels  0  1    2  3     4  5    6  7    8  9
        //
        // Pixels are displayed on screen as UPPER NYBBLE, then LOWER NYBBLE
        //
        // We must be careful not to read past end of 4bpp bitmap
        // We do this by breaking up the operation into 2 cases:
        //
        // If the starting pixel is ODD, then we need to read
        // the LOWER nybble of the source byte first, and proceed from there.
        // If the starting pixel is EVEN, then we need to read
        // the UPPER nybble of the source byte first, and proceed from there.
        //
        //******************************************************************

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

        if ( xsrc & 1)
            {
            for (yscan=0;yscan < clip_height; yscan++)
                {
                ScanPtr = NextScan;
                PixelCount=0;
                //**************************************************************
                // For each byte, we read 2 pixels.  We will count by bytes.
                // Since we started on an ODD pixel, we read the LOWER
                // nibble of the current source byte FIRST, then we
                // read the UPPER nibble
                //**************************************************************

                LoopCount= clip_width>>1 ;      // Odd pel handled separately

                //**************************************************************
                // Get the current 2 pixels of data
                //**************************************************************

                PixelDataByte = *ScanPtr;

                while (LoopCount-- > 0)
                    {
                    TempPixelData[PixelCount++] = (BYTE) pulXlate[PixelDataByte & 0xf];
                    ScanPtr++;

                    PixelDataByte =*ScanPtr ;
                    TempPixelData[PixelCount++] = (BYTE) pulXlate[PixelDataByte >> 4];

                    //**********************************************************
                    // Check if we need to flush the data and
                    // send it to NV hardware
                    //**********************************************************

                    if (PixelCount >= NV_HALF_FIFO_SIZE)
                        {
                        NV3_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount,&FreeCount);
                        PixelCount=0;
                        }
                    }

                //**************************************************************
                // Check for any straggling pixel leftover
                //**************************************************************

                if (clip_width & 1)
                    TempPixelData[PixelCount++] = (BYTE) pulXlate[PixelDataByte & 0xf];

                //**************************************************************
                // Flush any remaining pixel data
                //**************************************************************

                if (PixelCount!=0)
                    NV3_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount,&FreeCount);

                NextScan+=ScanInc;
                }
            }
        else
            {
            for (yscan=0;yscan < clip_height; yscan++)
                {
                ScanPtr = NextScan;
                PixelCount=0;
                //**************************************************************
                // Pixels are displayed on screen as UPPER NYBBLE, then LOWER NYBBLE
                //**************************************************************

                LoopCount = (clip_width + 1) >> 1;  // Each loop handles 2 pels

                do  {
                    //**********************************************************
                    // See if we can speed things up a bit by reading DWORDS
                    //**********************************************************

                    if (LoopCount>=4)               // 8 pels or more to go?
                        {
                        PixelDataDword = *((DWORD *)ScanPtr);
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword >> 4) & 0xf];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword     ) & 0xf];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword >>12) & 0xf];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword >> 8) & 0xf];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword >>20) & 0xf];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword >>16) & 0xf];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword >>28) & 0xf];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataDword >>24) & 0xf];

                        ScanPtr+=4;                 // Advance by 8 source pels

                        LoopCount-=4;               // 4 groups of 2 pels = 8 pels
                        }

                    else
                        {
                        PixelDataByte = *ScanPtr;

                        TempPixelData[PixelCount++] = (BYTE) pulXlate[PixelDataByte >> 4];
                        TempPixelData[PixelCount++] = (BYTE) pulXlate[PixelDataByte & 0xf];

                        ScanPtr++;                  // Advance by 2 source pels

                        LoopCount--;                // 1 group of 2 pels = 2 pels
                        }

                    //**********************************************************
                    // Check if we need to flush the data and
                    // send it to NV hardware
                    //**********************************************************

                    if (PixelCount >= NV_HALF_FIFO_SIZE)
                        {
                        NV3_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount,&FreeCount);
                        PixelCount=0;
                        }

                    } while (LoopCount != 0);

                //**************************************************************
                // Flush any remaining pixel data
                //**************************************************************

                if (PixelCount!=0)
                    NV3_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount,&FreeCount);


                NextScan+=ScanInc;
                }
            }

        if (--c == 0)
            {
            //******************************************************************
            // Update global free count
            //******************************************************************
            ppdev->NVFreeCount = FreeCount;

            return;
            }

        prcl++;

        } // while (TRUE)...

    }



//******************************************************************************
//
//  Function:   NV3XferNative
//
//      Transfers a bitmap that is the same colour depth as the display to
//      the screen via the data transfer register, with no translation.
//
//  Routine Description:
//
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV3XferNative(     // Type FNXFER
PDEV*       ppdev,
LONG        c,          // Count of rectangles, can't be zero
RECTL*      prcl,       // Array of relative coordinates destination rectangles
ROP4        rop4,       // rop4
SURFOBJ*    psoSrc,     // Source surface
POINTL*     pptlSrc,    // Original unclipped source point
RECTL*      prclDst,    // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG PatternColor,       // Not used
BLENDOBJ*   pBlendObj)
{
    LONG    dx;
    LONG    dy;
    LONG    cx;
    LONG    cy;
    LONG    lSrcDelta;
    BYTE*   SrcBits;
    BYTE*   pjSrc;
    LONG    cjSrc;

    Nv3ChannelPio   *nv = (Nv3ChannelPio *)ppdev->pjMmBase;
    ULONG depth;
    LONG BytesPerSrcScan;
    ULONG Rop3;
    ULONG temp;
    ULONG xsrc,ysrc;
    ULONG *NextScan;
    ULONG BytesPerDstScan;
    ULONG DwordsPerDstScan;
    ULONG yscan;
    ULONG WordCount;
    ULONG *ScanPtr;
    LONG ScanInc;
    USHORT FreeCount;
    ULONG OnePixel;
    BYTE *DstBits;
    ULONG SizeInWidth;
    ULONG WordsPerDstScan;
    ULONG clip_x,clip_y;
    ULONG clip_width,clip_height;
    ULONG TempPixelData1;
    ULONG TempPixelData2;
    DSURF*          pdsurfSrc;
    ULONG   ByteCount;
    ULONG   i;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL) ||
             ((pxlo->flXlate == 0x10) && (OglIsEnabled(ppdev))),
            "Can handle trivial xlate only");
    ASSERTDD(psoSrc->iBitmapFormat == ppdev->iBitmapFormat,
            "Source must be same colour depth as screen");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Get pointer to start of source bitmap
    // Need to be careful about this!  This function gets called
    // when copying a system memory bitmap to VRAM.  However, the SOURCE
    // surfobj can be a standard DIB or it can be a DFB that's been moved
    // from offscreen VRAM to system memory.  If this is the case, then
    // we need to make sure and get the source ptr from the correct place.
    //**************************************************************************

    pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    //**************************************************************************
    // If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the source ptr from pdsurfSrc->pso->pvScan0.
    // Otherwise, just get it from psoSrc->pvScan0 (standard DIB)
    //**************************************************************************

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    // CAREFUL: If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the 'ldelta' value from pdsurfSrc->pso->lDelta.  Source DFB
    // will NEVER come from offscreen VRAM (DT_SCREEN) so we don't have to even
    // check for dt == DT_DIB.  Otherwise, if we're dealing with a standard
    // system memory DIB, just get 'lDelta' from psoSrc->lDelta.
    //**************************************************************************

    if (pdsurfSrc != NULL)
        {
        SrcBits         = pdsurfSrc->pso->pvScan0;
        BytesPerSrcScan = pdsurfSrc->pso->lDelta;
        }
    else
        {
        SrcBits         = psoSrc->pvScan0;
        BytesPerSrcScan = psoSrc->lDelta;
        }

    //**************************************************************************
    // Verify that src bitmap is DWORD aligned.  As far as I can tell, this
    // appears to always be the case.
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) SrcBits & 0x3),
        "Src address is not dword aligned"); // ACK!..src addr is not dword aligned !!!

    //**************************************************************************
    // Verify that src bitmap pitch is a multiple of 4 (for dwords)
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) BytesPerSrcScan & 0x3),
        "Pitch is not dword aligned"); // ACK!....pitch is not dword aligned !!!

    //**************************************************************************
    // Current pixel depth
    //**************************************************************************

    depth = ppdev->cBitsPerPel;

    //**************************************************************************
    // Read FreeCount for the first time
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        while (FreeCount < 2*4)
            FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
        FreeCount -= 2*4;

        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((0 << 16) | 0 );
        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (((ppdev->cyMemory)<<16) | (0x7fff));

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }

    //**************************************************************************
    // Setup NV Blit parameters for CPU (MEMORY to SCREEN) blit
    //**************************************************************************

    Rop3 = (rop4 & 0xff);               // Get bottom byte

    //**************************************************************************
    // Enumerate the clipping regions
    //**************************************************************************

    while(TRUE)
        {
        //**********************************************************************
        // Top left coordinate of destination on destination surface
        //**********************************************************************

        clip_x = prcl->left;
        clip_y = prcl->top;

        //**********************************************************************
        // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
        // That is, the lower right coordinate is NOT included
        // For NV, the width/height actually means the number of
        // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
        // when calculating width and height.
        //**********************************************************************

        if (prclDst->right - prclDst->left < prcl->right - prcl->left)
        {
            clip_width = prclDst->right - prclDst->left;
        }
        else
        {
            clip_width = prcl->right - prcl->left;
        }
        if (prclDst->bottom - prclDst->top < prcl->bottom - prcl->top)
        {
            clip_height = prclDst->bottom - prclDst->top;
        }
        else
        {
            clip_height = prcl->bottom - prcl->top;
        }

        //**********************************************************************
        // Top left coordinate of source bitmap on source surface
        //**********************************************************************

        xsrc = pptlSrc->x + prcl->left - prclDst->left;
        ysrc = pptlSrc->y + prcl->top - prclDst->top;

        //**********************************************************************
        // Initialize the ImageFromCpu object
        //**********************************************************************

        while (FreeCount < 4*4)
            FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
        FreeCount -=4*4;

        nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = Rop3 ;
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Point =  ( ( (clip_y) <<16) | ((clip_x) & 0xffff) );
        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeOut = ( (clip_height <<16) | clip_width );

        //**********************************************************************
        // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
        // ignore/clip the extra pixel that we send it, so that it won't
        // get included as part of the next scanline. Since we always
        // send DWords at a time, we may actually send more data to the NV
        // engine than is necessary.  So clip it by specifying a larger SizeIn
        // That is, we always send DWORDS at a time for each scanline.
        // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
        //**********************************************************************

        if (depth == 8)
            SizeInWidth = ((clip_width + 3) & 0xfffc);
        else if (depth == 16)
            SizeInWidth = ((clip_width + 1) & 0xfffe);
        else
            SizeInWidth = clip_width;

        nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeIn = ( (clip_height <<16) | SizeInWidth );

        //**********************************************************************
        // Calculate number of dwords to output for scanline
        // This is the actual amount of data that the NV engine expects per scanline.
        // SizeInWidth is guaranteed to be a multiple of 2 (for 16bpp) and
        // a multiple of 4 (for 32bpp).  So we'll always be outputting DWORDS.
        // (No leftover words or bytes, they get clipped)
        //**********************************************************************

        BytesPerDstScan = clip_width * depth/8;
        DwordsPerDstScan = BytesPerDstScan / 4 + ((BytesPerDstScan % 4) ? 1 : 0);
        //**********************************************************************
        // Get ptr to start of SRC
        //**********************************************************************

        NextScan = (ULONG *)(&(SrcBits[(xsrc*depth/8) + ysrc*BytesPerSrcScan]));

        //**********************************************************************
        // Amount to increment for each scanline
        //**********************************************************************

        ScanInc = (BytesPerSrcScan+3) >> 2;      // Number of dwords to increment

#ifdef _X86_
        //**********************************************************************
        // Use assembly to output as fast as possible
        // We can still use the NV1 assembly routine, since we're still
        // using the NV1 version of the imagefromcpu class.
        //**********************************************************************

        NV1_Transfer_MemToScreen_Data(BytesPerDstScan,&FreeCount,clip_height,NextScan,ScanInc,nv);

#else

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

        for (yscan=0;yscan < clip_height; yscan++)
            {
            ByteCount=BytesPerDstScan;
            ScanPtr = NextScan;

            //******************************************************************
            // Output next group of 16 dwords
            //******************************************************************

            while (ByteCount >= 16 * sizeof(ULONG))
                {
                ByteCount -= (16 * sizeof(ULONG));

                while (FreeCount < 16*4)
                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                FreeCount -=16*4;

                TempPixelData1 = ScanPtr[0];
                TempPixelData2 = ScanPtr[1];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
                TempPixelData1 = ScanPtr[2];
                TempPixelData2 = ScanPtr[3];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;
                TempPixelData1 = ScanPtr[4];
                TempPixelData2 = ScanPtr[5];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[4] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[5] =  TempPixelData2;
                TempPixelData1 = ScanPtr[6];
                TempPixelData2 = ScanPtr[7];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[6] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[7] =  TempPixelData2;
                TempPixelData1 = ScanPtr[8];
                TempPixelData2 = ScanPtr[9];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[8] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[9] =  TempPixelData2;
                TempPixelData1 = ScanPtr[10];
                TempPixelData2 = ScanPtr[11];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[10] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[11] =  TempPixelData2;
                TempPixelData1 = ScanPtr[12];
                TempPixelData2 = ScanPtr[13];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[12] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[13] =  TempPixelData2;
                TempPixelData1 = ScanPtr[14];
                TempPixelData2 = ScanPtr[15];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[14] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[15] =  TempPixelData2;

                ScanPtr +=16;           // Advance 16 dwords
                }

            //******************************************************************
            // Output next group of 4 dwords
            //******************************************************************

            while (ByteCount >= 4 * sizeof(ULONG))
                {
                ByteCount -= (4 * sizeof(ULONG));

                while (FreeCount < 4*4)
                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                FreeCount -=4*4;

                TempPixelData1 = ScanPtr[0];
                TempPixelData2 = ScanPtr[1];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
                TempPixelData1 = ScanPtr[2];
                TempPixelData2 = ScanPtr[3];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;

                ScanPtr +=4;            // Advance 4 dwords

                }


            //******************************************************************
            // Output any remaining dwords
            //******************************************************************

            while (ByteCount >= sizeof(ULONG) )
                {
                ByteCount -= sizeof(ULONG);

                while (FreeCount < 4)
                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                FreeCount -= 4;

                TempPixelData1 = ScanPtr[0];
                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;

                ScanPtr ++;             // Advance 1 dword

                }



            //
            // Pad src scanline.
            //
            if (ByteCount)
            {
                TempPixelData1 = 0;
                for (i = 0; i < ByteCount; i++)
                {
                    TempPixelData1 |= ((PBYTE) (ScanPtr))[i] << (8 * i);
                }

                while (FreeCount < 4)
                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                FreeCount -= 4;

                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
            }

            NextScan+=ScanInc;
            }


#endif


        if (--c == 0)
            {
            //******************************************************************
            // Update global free count
            //******************************************************************
            ppdev->NVFreeCount = FreeCount;

            return;
            }

        prcl++;

        } // while (TRUE)...

    }


//******************************************************************************
//
//  Function:   NV3MemToScreenBlit
//
//  Routine Description:
//
//      Does a memory-to-screen blt
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV3MemToScreenBlt(
PDEV*   ppdev,
SURFOBJ* psoSrc,
POINTL* pptlSrc,
RECTL*  prclDst)

    {
    LONG    dx;
    LONG    dy;     // Add delta to destination to get source
    LONG    cx;
    LONG    cy;     // Size of current rectangle - 1

    Nv3ChannelPio   *nv = (Nv3ChannelPio *)ppdev->pjMmBase;

    ULONG width,height;
    ULONG xscan,yscan;
    ULONG xsrc,ysrc;
    ULONG xdst,ydst;
    ULONG depth;
    LONG BytesPerSrcScan;
    ULONG ByteCount;
    ULONG SizeInWidth;
    ULONG BytesPerDstScan;
    ULONG DwordsPerDstScan;
    ULONG WordsPerDstScan;
    ULONG WordCount;
    WORD OnePixel;
    ULONG PixelData;
    ULONG TempPixelData1;
    ULONG TempPixelData2;
    ULONG *ScanPtr;
    ULONG *NextScan;
    LONG ScanInc;
    USHORT FreeCount;
    BYTE* SrcBits;
    DSURF* pdsurfSrc;

    //**************************************************************************
    // This function is exactly the same as XferNative, except
    // that 1) Rop = SRCCOPY always
    //      2) No clipping is done here
    //      3) No color translation
    //**************************************************************************

    //**************************************************************************
    // Get pointer to start of source bitmap
    // Need to be careful about this!  This function gets called
    // when copying a system memory bitmap to VRAM.  However, the SOURCE
    // surfobj can be a standard DIB or it can be a DFB that's been moved
    // from offscreen VRAM to system memory.  If this is the case, then
    // we need to make sure and get the source ptr from the correct place.
    //**************************************************************************

    pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    //**************************************************************************
    // If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the source ptr from pdsurfSrc->pso->pvScan0.
    // Otherwise, just get it from psoSrc->pvScan0 (standard DIB)
    //**************************************************************************

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    // CAREFUL: If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the 'ldelta' value from pdsurfSrc->pso->lDelta.  Source DFB
    // will NEVER come from offscreen VRAM (DT_SCREEN) so we don't have to even
    // check for dt == DT_DIB.  Otherwise, if we're dealing with a standard
    // system memory DIB, just get 'lDelta' from psoSrc->lDelta.
    //**************************************************************************

    if (pdsurfSrc != NULL)
        {
        SrcBits         = pdsurfSrc->pso->pvScan0;
        BytesPerSrcScan = pdsurfSrc->pso->lDelta;
        }
    else
        {
        SrcBits         = psoSrc->pvScan0;
        BytesPerSrcScan = psoSrc->lDelta;
        }

    //**************************************************************************
    // Verify that src bitmap is DWORD aligned.  As far as I can tell, this
    // appears to always be the case.
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) SrcBits & 0x3),
        "Src address is not dword aligned"); // ACK!..src addr is not dword aligned !!!

    //**************************************************************************
    // Verify that src bitmap pitch is a multiple of 4 (for dwords)
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) BytesPerSrcScan & 0x3),
        "Pitch is not dword aligned"); // ACK!....pitch is not dword aligned !!!

    //**************************************************************************
    // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
    // That is, the lower right coordinate is NOT included
    // For NV, the width/height actually means the number of
    // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
    // when calculating width and height.
    //**************************************************************************

    width = prclDst->right - prclDst->left;
    height = prclDst->bottom - prclDst->top;

    //**************************************************************************
    // Top left coordinate of destination on destination surface
    //**************************************************************************

    xdst = prclDst->left;
    ydst = prclDst->top;

    //**************************************************************************
    // Top left coordinate of source bitmap on source surface
    //**************************************************************************

    xsrc = pptlSrc->x;
    ysrc = pptlSrc->y;

    //**************************************************************************
    // Current pixel depth
    //**************************************************************************

    depth = ppdev->cBitsPerPel;

    //**************************************************************************
    // Read FreeCount for the first time
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

   //***************************************************************************
   // Reset clipping rectangle to full screen extents if necessary
   // Resetting the clipping rectangle causes delays so we want to do it
   // as little as possible!
   //***************************************************************************

   if (ppdev->NVClipResetFlag)
       {
       while (FreeCount < 2*4)
          FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
       FreeCount -= 2*4;

       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((0 << 16) | 0 );
       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (((ppdev->cyMemory)<<16) | (0x7fff));

       ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
       }

   //***************************************************************************
   // Initialize the ImageFromCpu object
   //***************************************************************************

    while (FreeCount < 4*4)
       FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
    FreeCount -=4*4;

    nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = NV_SRCCOPY ;
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Point =  ( ((ydst) <<16) | ((xdst) & 0xffff) );
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeOut = ( (height <<16) | width );

    //**************************************************************************
    // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
    // ignore/clip the extra pixel that we send it, so that it won't
    // get included as part of the next scanline. Since we always
    // send DWords at a time, we may actually send more data to the NV
    // engine than is necessary.  So clip it by specifying a larger SizeIn
    // That is, we always send DWORDS at a time for each scanline.
    // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
    //**************************************************************************

    if (depth == 8)
        SizeInWidth = ((width + 3) & 0xfffc);
    else if (depth == 16)
        SizeInWidth = ((width + 1) & 0xfffe);
    else
        SizeInWidth = width;

    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeIn = ( (height <<16) | (SizeInWidth) );

    //**************************************************************************
    // Calculate number of dwords to output for scanline.
    //**************************************************************************
    BytesPerDstScan = width * depth/8;
    DwordsPerDstScan = BytesPerDstScan / 4 + ((BytesPerDstScan % 4) ? 1 : 0);

    //**************************************************************************
    // Get ptr to start of SRC
    //**************************************************************************

    NextScan = (ULONG *)(&(SrcBits[(xsrc*depth/8) + (ysrc*BytesPerSrcScan)]));

    //**************************************************************************
    // Amount to increment for each scanline
    //**************************************************************************

        ScanInc = (BytesPerSrcScan+3) >> 2;
        
#ifdef _X86_
    //**************************************************************************
    // Use assembly to output as fast as possible
    // We can still use the NV3 assembly routine, since we're still
    // using the NV3 version of the imagefromcpu class
    //**************************************************************************

    NV1_Transfer_MemToScreen_Data(BytesPerDstScan,&FreeCount,height,NextScan,ScanInc,nv);

#else
    //**************************************************************************
    // The following C code is the exact equivalent of
    // NV1_Transfer_MemToScreen_Data in assembly.
    //**************************************************************************

   //************************************************************************
   // Output one scan at a time
   //************************************************************************

   for (yscan=0;yscan < height; yscan++)
        {
        ByteCount = BytesPerDstScan;

        ScanPtr = NextScan;

        //********************************************************************
        // Output next group of 16 dwords
        //********************************************************************

        while (ByteCount >= (16 * sizeof(ULONG)))
            {
            ByteCount -= (16 * sizeof(ULONG));

            while (FreeCount < 16*4)
                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -=16*4;

            TempPixelData1 = ScanPtr[0];
            TempPixelData2 = ScanPtr[1];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
            TempPixelData1 = ScanPtr[2];
            TempPixelData2 = ScanPtr[3];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;
            TempPixelData1 = ScanPtr[4];
            TempPixelData2 = ScanPtr[5];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[4] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[5] =  TempPixelData2;
            TempPixelData1 = ScanPtr[6];
            TempPixelData2 = ScanPtr[7];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[6] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[7] =  TempPixelData2;
            TempPixelData1 = ScanPtr[8];
            TempPixelData2 = ScanPtr[9];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[8] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[9] =  TempPixelData2;
            TempPixelData1 = ScanPtr[10];
            TempPixelData2 = ScanPtr[11];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[10] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[11] =  TempPixelData2;
            TempPixelData1 = ScanPtr[12];
            TempPixelData2 = ScanPtr[13];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[12] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[13] =  TempPixelData2;
            TempPixelData1 = ScanPtr[14];
            TempPixelData2 = ScanPtr[15];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[14] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[15] =  TempPixelData2;

            ScanPtr +=16;           // Advance 16 dwords
            }

        //********************************************************************
        // Output next group of 4 dwords
        //********************************************************************

        while (ByteCount >= (4 * sizeof(ULONG)))
            {
            ByteCount -= (4 * sizeof(ULONG));

            while (FreeCount < 4*4)
                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -=4*4;

            TempPixelData1 = ScanPtr[0];
            TempPixelData2 = ScanPtr[1];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
            TempPixelData1 = ScanPtr[2];
            TempPixelData2 = ScanPtr[3];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;

            ScanPtr +=4;            // Advance 4 dwords

            }

        //********************************************************************
        // Output any remaining dwords
        //********************************************************************

        while (ByteCount >= sizeof(ULONG))
            {
            ByteCount -= sizeof(ULONG);

            while (FreeCount < 4)
                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -= 4;

            TempPixelData1 = ScanPtr[0];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;

            ScanPtr ++;             // Advance 1 dword

            }

        //********************************************************************
        // Pad src scanline.
        //********************************************************************
        if (ByteCount)
            {
            ULONG   i;
            TempPixelData1 = 0;
            for (i = 0; i < ByteCount; i++)
                {
                TempPixelData1 |= ((PBYTE) (ScanPtr))[i] << (8 * i);
                }

            while (FreeCount < 4)
                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -= 4;

            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
            }


        NextScan+=ScanInc;

        }
#endif  // _X86_

    //**************************************************************************
    // Update global free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;



    }

//******************************************************************************
//
//  Function:   NV3MemToScreenWithPatternBlt()
//
//  Routine Description:
//
//              Does a memory-to-screen with pattern blt
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV3MemToScreenWithPatternBlt(
PDEV*   ppdev,
SURFOBJ* psoSrc,
POINTL* pptlSrc,
RECTL*  prclDst,
ULONG rop3,
RBRUSH_COLOR   *rbc,
RECTL*  prclClip
)


    {
    LONG    dx;
    LONG    dy;     // Add delta to destination to get source
    LONG    cx;
    LONG    cy;     // Size of current rectangle - 1

    Nv3ChannelPio   *nv = (Nv3ChannelPio *)ppdev->pjMmBase;

    ULONG width,height;
    ULONG xscan,yscan;
    ULONG xsrc,ysrc;
    ULONG xdst,ydst;
    ULONG depth;
    LONG BytesPerSrcScan;
    ULONG ByteCount;
    ULONG SizeInWidth;
    ULONG BytesPerDstScan;
    ULONG DwordsPerDstScan;
    ULONG WordsPerDstScan;
    ULONG WordCount;
    WORD OnePixel;
    ULONG PixelData;
    ULONG TempPixelData1;
    ULONG TempPixelData2;
    ULONG *ScanPtr;
    ULONG *NextScan;
    LONG ScanInc;
    USHORT FreeCount;
    BYTE* SrcBits;
    DSURF* pdsurfSrc;
    ULONG   i;
    ULONG PixelDepth;
    ULONG AlphaEnableValue;

    //**************************************************************************
    // Get pointer to start of source bitmap
    // Need to be careful about this!  This function gets called
    // when copying a system memory bitmap to VRAM.  However, the SOURCE
    // surfobj can be a standard DIB or it can be a DFB that's been moved
    // from offscreen VRAM to system memory.  If this is the case, then
    // we need to make sure and get the source ptr from the correct place.
    //**************************************************************************

    pdsurfSrc = (DSURF*) psoSrc->dhsurf;

    //**************************************************************************
    // If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the source ptr from pdsurfSrc->pso->pvScan0.
    // Otherwise, just get it from psoSrc->pvScan0 (standard DIB)
    //**************************************************************************

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    // CAREFUL: If this is a DFB which has been moved to system memory (DT_DIB),
    // then get the 'ldelta' value from pdsurfSrc->pso->lDelta.  Source DFB
    // will NEVER come from offscreen VRAM (DT_SCREEN) so we don't have to even
    // check for dt == DT_DIB.  Otherwise, if we're dealing with a standard
    // system memory DIB, just get 'lDelta' from psoSrc->lDelta.
    //**************************************************************************

    if (pdsurfSrc != NULL)
        {
        SrcBits         = pdsurfSrc->pso->pvScan0;
        BytesPerSrcScan = pdsurfSrc->pso->lDelta;
        }
    else
        {
        SrcBits         = psoSrc->pvScan0;
        BytesPerSrcScan = psoSrc->lDelta;
        }

    //**************************************************************************
    // Verify that src bitmap is DWORD aligned.  As far as I can tell, this
    // appears to always be the case.
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) SrcBits & 0x3),
        "Src address is not dword aligned"); // ACK!..src addr is not dword aligned !!!

    //**************************************************************************
    // Verify that src bitmap pitch is a multiple of 4 (for dwords)
    //**************************************************************************

    ASSERTDD(!((ULONG) BytesPerSrcScan & 0x3),
        "Pitch is not dword aligned"); // ACK!....pitch is not dword aligned !!!

    //**************************************************************************
    // Get pixel depth for the current mode.
    // Then determine the ALPHA_CHANNEL enable value
    //**************************************************************************

    AlphaEnableValue = ppdev->AlphaEnableValue;

    //**************************************************************************
    // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
    // That is, the lower right coordinate is NOT included
    // For NV, the width/height actually means the number of
    // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
    // when calculating width and height.
    //**************************************************************************

    width = prclDst->right - prclDst->left;
    height = prclDst->bottom - prclDst->top;

    //**************************************************************************
    // Top left coordinate of source bitmap on source surface
    //**************************************************************************

    xsrc = pptlSrc->x;
    ysrc = pptlSrc->y;

    //**************************************************************************
    // Top left coordinate of destination on destination surface
    //**************************************************************************

    xdst = prclDst->left;
    ydst = prclDst->top;

    //**************************************************************************
    // Current pixel depth
    //**************************************************************************

    depth = ppdev->cBitsPerPel;

    //**************************************************************************
    // Read FreeCount for the first time
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

   //***************************************************************************
   // Reset clipping rectangle to full screen extents if necessary
   // Resetting the clipping rectangle causes delays so we want to do it
   // as little as possible!
   //***************************************************************************

    if((prclClip == NULL) && (ppdev->NVClipResetFlag))
    {
       while (FreeCount < 2*4)
          FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
       FreeCount -= 2*4;

       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((0 << 16) | 0 );
       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (((ppdev->cyMemory)<<16) | (0x7fff));

       ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
    }
    else if(prclClip != NULL)
    {

       while (FreeCount < 2*4)
          FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
       FreeCount -= 2*4;

       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((prclClip->top << 16) | prclClip->left );
       nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (((prclClip->bottom - prclClip->top) << 16) 
                    | (prclClip->right - prclClip->left) ); 

       ppdev->NVClipResetFlag=1;       // Clipping rectangle has been reset
    }

    //**************************************************************************
    // Wait for FIFO, then set pattern registers
    //**************************************************************************

    while (FreeCount < 5*4)
       FreeCount = nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].control.Free;
    FreeCount -=5*4;

    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeShape = NV018_SET_MONOCHROME_SHAPE_64X_1Y ;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeColor0 = (ULONG)(rbc->iSolidColor | AlphaEnableValue);
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromeColor1 = (ULONG)(rbc->iSolidColor | AlphaEnableValue);
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromePattern0 = 0xffffffff;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nv1ImagePattern.SetMonochromePattern1 = 0xffffffff;

    //**************************************************************************
    // Wait for FIFO, then set IMAGE_FROM_CPU registers
    //**************************************************************************

    while (FreeCount < 4*4)
       FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
    FreeCount -=4*4;

    nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = rop3 ;
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Point =  ( ((ydst) <<16) | ((xdst) & 0xffff) );
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeOut = ( (height <<16) | width );

    //**************************************************************************
    // Specifying 1 more pixel (example: in 16bpp) for SizeIn causes the NV engine to
    // ignore/clip the extra pixel that we send it, so that it won't
    // get included as part of the next scanline. Since we always
    // send DWords at a time, we may actually send more data to the NV
    // engine than is necessary.  So clip it by specifying a larger SizeIn
    // That is, we always send DWORDS at a time for each scanline.
    // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
    // SizeIn is specified in pixels.
    //**************************************************************************

    if (depth == 8)
        SizeInWidth = ((width + 3) & 0xfffc);       // We always send 4 pixels at a time
    else if (depth == 16)
        SizeInWidth = ((width + 1) & 0xfffe);       // We always send 2 pixels at a time
    else
        SizeInWidth = width;                        // We always send 1 pixel at a time

    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.SizeIn = ( (height <<16) | (SizeInWidth) );

    //**************************************************************************
    // Calculate number of dwords to output per scanline
    // This is the actual amount of data that the NV engine expects per scanline.
    // SizeInWidth is guaranteed to be a multiple of 2 (for 16bpp) and
    // a multiple of 4 (for 8bpp).  So we'll always be outputting DWORDS.
    // (No leftover words or bytes, they get clipped)
    //**************************************************************************

    BytesPerDstScan = width * depth/8;
    DwordsPerDstScan = BytesPerDstScan / 4 + ((BytesPerDstScan % 4) ? 1 : 0);

    //**************************************************************************
    // Get ptr to start of SRC
    //**************************************************************************

    NextScan = (ULONG *)(&(SrcBits[(xsrc*depth/8) + (ysrc*BytesPerSrcScan)]));

    //**************************************************************************
    // Amount to increment for each scanline
    //**************************************************************************

    ScanInc = (BytesPerSrcScan+3) >> 2;      // Number of dwords to increment

    //**************************************************************************
    // Output one scan at a time
    //**************************************************************************

    for (yscan=0;yscan < height; yscan++)
        {
        ByteCount=BytesPerDstScan;

        ScanPtr = NextScan;

        //**********************************************************************
        // Output next group of 16 dwords
        //**********************************************************************

        while (ByteCount >= 16 * sizeof(ULONG))
            {
            ByteCount -= (16 * sizeof(ULONG));

            while (FreeCount < 16*4)
                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -=16*4;

            TempPixelData1 = ScanPtr[0];
            TempPixelData2 = ScanPtr[1];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
            TempPixelData1 = ScanPtr[2];
            TempPixelData2 = ScanPtr[3];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;
            TempPixelData1 = ScanPtr[4];
            TempPixelData2 = ScanPtr[5];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[4] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[5] =  TempPixelData2;
            TempPixelData1 = ScanPtr[6];
            TempPixelData2 = ScanPtr[7];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[6] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[7] =  TempPixelData2;
            TempPixelData1 = ScanPtr[8];
            TempPixelData2 = ScanPtr[9];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[8] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[9] =  TempPixelData2;
            TempPixelData1 = ScanPtr[10];
            TempPixelData2 = ScanPtr[11];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[10] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[11] =  TempPixelData2;
            TempPixelData1 = ScanPtr[12];
            TempPixelData2 = ScanPtr[13];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[12] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[13] =  TempPixelData2;
            TempPixelData1 = ScanPtr[14];
            TempPixelData2 = ScanPtr[15];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[14] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[15] =  TempPixelData2;

            ScanPtr +=16;           // Advance 16 dwords
            }

        //**********************************************************************
        // Output next group of 4 dwords
        //**********************************************************************

        while (ByteCount >= 4 * sizeof(ULONG))
            {
            ByteCount -= (4 * sizeof(ULONG));
            while (FreeCount < 4*4)
                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -=4*4;

            TempPixelData1 = ScanPtr[0];
            TempPixelData2 = ScanPtr[1];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[1] =  TempPixelData2;
            TempPixelData1 = ScanPtr[2];
            TempPixelData2 = ScanPtr[3];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[2] =  TempPixelData1;
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[3] =  TempPixelData2;

            ScanPtr +=4;            // Advance 4 dwords

            }

        //**********************************************************************
        // Output any remaining dwords
        //**********************************************************************

        while (ByteCount >= sizeof(ULONG) )
            {
            ByteCount -= sizeof(ULONG);

            while (FreeCount < 4)
                FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
            FreeCount -= 4;

            TempPixelData1 = ScanPtr[0];
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;

            ScanPtr ++;             // Advance 1 dword

            }
            //
            // Pad src scanline.
            //
            if (ByteCount)
            {
                TempPixelData1 = 0;
                for (i = 0; i < ByteCount; i++)
                {
                    TempPixelData1 |= ((PBYTE) (ScanPtr))[i] << (8 * i);
                }

                while (FreeCount < 4)
                    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.Free;
                FreeCount -= 4;

                nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nv1ImageFromCpu.Color[0] =  TempPixelData1;
            }


        NextScan+=ScanInc;

        }

    //**************************************************************************
    // Notify the rest of the driver that we changed the pattern
    //**************************************************************************

    ppdev->NVPatternResetFlag=1;

    //**************************************************************************
    // Update global free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;


    }



//******************************************************************************
//
//  Function:   NV3CopyBlit
//
//  Routine Description:
//
//      Does a screen-to-screen blt of a list of rectangles.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV3CopyBlt(    // Type FNCOPY
    PDEV*   ppdev,
LONG    c,          // Can't be zero
RECTL*  prcl,       // Array of relative coordinates destination rectangles
ULONG   rop4,       // rop4
POINTL* pptlSrc,    // Original unclipped source point
RECTL*  prclDst,    // Original unclipped destination rectangle
BLENDOBJ*   pBlendObj)
    {
    LONG    dx;
    LONG    dy;     // Add delta to destination to get source
    LONG    cx;
    LONG    cy;     // Size of current rectangle - 1

    Nv3ChannelPio  *nv  = (Nv3ChannelPio*) ppdev->pjMmBase;

    ULONG width,height;
    ULONG xsrc,ysrc;
    ULONG xdst,ydst;
    USHORT FreeCount;
    ULONG Rop3;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Initialize the cached free count
    //**************************************************************************

    FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        while (FreeCount < 2*4)
            FreeCount = nv->subchannel[CLIP_SUBCHANNEL].control.Free;
        FreeCount -= 2*4;

        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = ((0 << 16) | 0 );
        nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (((ppdev->cyMemory)<<16) | (0x7fff));

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }

    Rop3 = (rop4 & 0xff);

    //**************************************************************************
    // Do a copy blit for each clip rectangle
    //**************************************************************************

    do  {

        //**********************************************************************
        // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
        // That is, the lower right coordinate is NOT included
        // For NV, the width/height actually means the number of
        // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
        // when calculating width and height.
        //**********************************************************************

        width = prcl->right - prcl->left;
        height = prcl->bottom - prcl->top;

        dx = prcl->left - prclDst->left;    // prcl is always within prcldst???
        dy = prcl->top  - prclDst->top;     // prcl is always within prcldst???

        xsrc = pptlSrc->x + dx;
        ysrc = pptlSrc->y + dy;

        xdst = prcl->left;
        ydst = prcl->top;

        //**********************************************************************
        // Wait for FIFO to be ready, then draw
        //**********************************************************************

        while (FreeCount < 4*4)
            FreeCount = nv->subchannel[BLIT_SUBCHANNEL].control.Free;
        FreeCount -= 4*4;

        nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = Rop3 ;

        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (ysrc << 16) | (xsrc & 0xffff) );
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( ( (ydst) << 16) | ((xdst) & 0xffff) );
        nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( (height << 16) | width );

        prcl++;

        } while (--c != 0);

    //**************************************************************************
    // Update global free count
    //**************************************************************************

    ppdev->NVFreeCount = FreeCount;

    }

#endif _WIN32_WINNT     // If is >= NT 4.x
#endif // NV3
