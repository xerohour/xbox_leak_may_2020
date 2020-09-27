//******************************Module*Header***********************************
//
// Module Name: NV4BLTC.C
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

#include "nv32.h"
#include "dmamacro.h"

#include "nvsubch.h"

//******************************************************************************
// NV4 vs. NV3
//
// This file is almost exactly the same as the NV3 version.
// The difference, is that since we'll be using 565 bitmap format on NV4, 
// we need to use NV4 specific objects (The NV4 RECTANGLE_TEXT object has 
// changed significantly from the NV3 RECTANGLE_TEXT object... the method
// offsets are *NOT* the same as NV3).
//
// (i.e. we can only use the original NV3 functions on NV4 when running in 555 format)
//******************************************************************************

#define NV_MTMF_FORMAT_INPUT_INC_1     0x00000001
#define NV_MTMF_FORMAT_OUTPUT_INC_1    0x00000100

//******************************************************************************
// Externs
//******************************************************************************

extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );

#ifdef _X86_

//******************************************************************************
// X86 Assembly routine to xfer host bitmap->screen
//******************************************************************************

void NV4_DmaPush_Transfer_MemToScreen_Data(PDEV *, LONG, ULONG, ULONG *,LONG);

#endif

#if 1 // Enable xlate caching
//******************************************************************************
// Replacing all (BYTE)XLATEOBJ_iXlate(pxlo,DWORD) with a macro that is caching
// the last value.
// The call to XLATEOBJ_iXlate is expensive. Until now it was done per pixel. 
// This caused our NV4ScreenToMem32to8bppBlt to be slower than a return FALSE 
// and punt to Eng. 
// Now we cache the last xlated value and index and only xlate if necessary. 
// This gives us 25 % speedup in SolidWorks2000 I/O bench (1280TC) and some 
// points in WinBench99 (NT4 1024TC).
//
// Because the entire Xfer stuff is a macro it is very hard to debug or modify 
// code. so I choose a macro, too :-(
// Everywhere NV_XLATEOBJ_IXLATE is used the NV_XLATE_INIT macro must be added 
// to define and intialize the cache variables:
//******************************************************************************
//
    // Macro to initialize cached xlate values
    #define NV_XLATE_INIT                                                \
        DWORD cachedXlIndex = 0;                                         \
        BYTE  jXlCached     = (BYTE)XLATEOBJ_iXlate(pxlo,cachedXlIndex);


    #if DBG
        // In a debug environment we count the misses and hits to the cached value.
        DWORD cacheXlHits = 0; // counter for xlated cache hits
        DWORD cacheXlMiss = 0; // counter for xlated cache misses

        #define NV_XLATEOBJ_IXLATE(pxlo,cIndex) ( ((cIndex) == cachedXlIndex) ?                                       \
            ( cacheXlHits++, jXlCached ) :                                                                            \
            ( cacheXlMiss++, (cachedXlIndex = (cIndex)), (jXlCached = (BYTE)XLATEOBJ_iXlate((pxlo),cachedXlIndex))) )
    #else
        // free build
        // if (index==prev index) {take cached xlated value} else {remember this index, calculate and remember xlated value}
        #define NV_XLATEOBJ_IXLATE(pxlo,cIndex) ( ((cIndex) == cachedXlIndex) ?                                       \
            (                jXlCached ) :                                                                            \
            (                (cachedXlIndex = (cIndex)), (jXlCached = (BYTE)XLATEOBJ_iXlate((pxlo),cachedXlIndex))) )
    #endif

#else
    // original 1:1 xlate without caching
    #define NV_XLATE_INIT
    #define NV_XLATEOBJ_IXLATE(pxlo,cIndex) XLATEOBJ_iXlate(pxlo,cIndex)
#endif

//******************************************************************************
//
//  Function:   NV4DmaPushFillSolid
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

VOID NV4DmaPushFillSolid(       // Type FNFILL
PDEV*           ppdev,
LONG            c,              // Can't be zero
RECTL*          prcl,           // List of rectangles to be filled, in relative
                                //   coordinates
ULONG           rop4,           // rop4
RBRUSH_COLOR    rbc,            // Drawing colour is rbc.iSolidColor
POINTL*         pptlBrush)      // Not used

    {
    LONG  xDst;
    LONG  yDst;
    ULONG width,height;
    ULONG Rop3;
    ULONG temp;
    ULONG AlphaWithColor;

    DECLARE_DMA_FIFO;
    
    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(c > 0, "Can't handle zero rectangles");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;
    
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

    AlphaWithColor = ((ppdev->AlphaEnableValue) | rbc.iSolidColor);
    temp = ((rop4 >> 2) & 0xf);         // Get bottom nibble
    Rop3 = temp | (temp << 4);        // Duplicate in top nibble

    //**************************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(5+(4*c))));  

    //**************************************************************************
    // Set Rop
    //**************************************************************************

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

    //**************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {

        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }

    //**********************************************************************
    // Since the sub-channel is same in the while loop, the color is set 
    // out of the while loop.
    //**********************************************************************
    NV_DMAPUSH_START (1, RECT_AND_TEXT_SUBCHANNEL, NV04A_COLOR1_A)
    NV_DMA_FIFO = AlphaWithColor;
    
    while(TRUE)
        {

        //**********************************************************************
        // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
        // That is, the lower right coordinate is NOT included
        // For NV, the width/height actually means the number of pixels drawn.
        // So, adjusting by 1 pixel is NOT necessary when calculating width and height.
        //**********************************************************************

        xDst = prcl->left;
        yDst = prcl->top;
        width = prcl->right - xDst;
        height = prcl->bottom - yDst;

        NV_DMAPUSH_START (2, RECT_AND_TEXT_SUBCHANNEL, NV04A_UNCLIPPED_RECTANGLE(0))
        NV_DMA_FIFO = (xDst << 16) | (yDst & 0xffff);
        NV_DMA_FIFO = (width << (16) | (height & 0xffff));
        
        if (--c == 0)
            {
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

        prcl++;
        }

    }


//******************************************************************************
//
//  Function:   NV4DmaPushCopyBlit
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

VOID NV4DmaPushCopyBlt(    // Type FNCOPY
    PDEV*   ppdev,
    LONG    c,          // Can't be zero
    RECTL*  prcl,       // Array of relative coordinates destination rectangles
    ULONG   rop4,       // rop4
    POINTL* pptlSrc,    // Original unclipped source point
    RECTL*  prclDst,    // Original unclipped destination rectangle
    BLENDOBJ *pBlendObj)
    {
    LONG    dx;
    LONG    dy;     // Add delta to destination to get source

    ULONG   width,height;
    ULONG   xsrc,ysrc;
    ULONG   xdst,ydst;
    ULONG   Rop3;
    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),"Expect only a rop2");
    ASSERTDD((pBlendObj==NULL), "Must use NV4DmaPushCopyAlphaBlt for blended CopyBlt");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************************
    NV_DMAPUSH_CHECKFREE((ULONG)(5 + (4*c)));

    //**************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        
        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }

    //**************************************************************************
    // Set Rop
    //**************************************************************************

    Rop3 = (rop4 & 0xff);

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

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

        xdst = prcl->left;
        ydst = prcl->top;

        width = prcl->right - xdst;
        height = prcl->bottom - ydst;

        dx = xdst - prclDst->left;      // prcl is always within prcldst???
        dy = ydst - prclDst->top;       // prcl is always within prcldst???

        xsrc = pptlSrc->x + dx;
        ysrc = pptlSrc->y + dy;

        //**********************************************************************
        // Wait for FIFO to be ready, then draw

        //**********************************************************************
        // Set methods for image_blit:
        //    NV05F_CONTROL_POINT_IN 
        //    NV05F_CONTROL_POINT_OUT
        //    NV05F_SIZE             
        //**********************************************************************

        NV_DMAPUSH_START(3, BLIT_SUBCHANNEL, NV05F_CONTROL_POINT_IN );
        NV_DMA_FIFO = ( (ysrc << 16) | (xsrc & 0xffff) );
        NV_DMA_FIFO = ( (ydst << 16) | (xdst & 0xffff) );
        NV_DMA_FIFO = ( (height << 16) | (width & 0xffff));

        prcl++;

    } while (--c != 0);

    //**************************************************************************
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

    }




//******************************************************************************
//
//  Function:   NV4DmaScreenToScreenWithPatBlt
//
//  Routine Description:
//
//      Does a screen-to-screen blt of a list of rectangles, supporting brushes
//      and all rops.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV4ScreenToScreenWithPatBlt(
    
    PDEV*       ppdev,              // PDEV associated with the SRC device
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      parcl,              // Array of destination rectangles (inside prclDst)
    POINTL*     pptlSrc,            // Upper left corner of SRC retcl in SRC surface
    RECTL*      prclDst,            // Rectangle to be modified in the DST surface
    BRUSHOBJ*   pbo,                // Brush object
    POINTL*     pptlBrush,          // Brush start coordinate in the pattern bits.
    ROP4        rop4)               // Op that describes the blit (must be 1 byte max)

{
    ULONG       rop3;
    RBRUSH*     prb;
    
    ULONG       xsrc,ysrc;

    ULONG       Color0;
    ULONG       Color1;
    ULONG       width;
    ULONG       height;
    LONG        xShift;
    LONG        yShift;
    LONG        BytesPerSrcScan;
    BYTE*       SrcBits;
    BYTE*       DstBits;
    ULONG*      ScanPtr;
    ULONG*      NextScan;
    LONG        ScanInc;
    LONG        cjLeft;
    LONG        cjRight;
    ULONG       i, ix,iy;


    DECLARE_DMA_FIFO;

    
    //**************************************************************************
    // Validate parameters first
    //**************************************************************************

    ASSERTDD( ppdev != NULL,
              "NV4DmaScreenToScreenWithPatBlt: NULL PDEV supplied !" );

    ASSERTDD( c > 0,
              "NV4DmaScreenToScreenWithPatBlt: Can't handle zero rectangles" ); // ok

    ASSERTDD( parcl != NULL,
              "NV4DmaScreenToScreenWithPatBlt: NULL destination rectangles supplied !" );
        
    ASSERTDD( pptlSrc != NULL,
              "NV4DmaScreenToScreenWithPatBlt: NULL source upper left point supplied !" );

    ASSERTDD( prclDst != NULL,
              "NV4DmaScreenToScreenWithPatBlt: NULL destination rectangle supplied !" );
    
    ASSERTDD( pbo != NULL,
              "NV4DmaScreenToScreenWithPatBlt: NULL brush object supplied !" );
    
    ASSERTDD( pptlBrush != NULL,
              "NV4DmaScreenToScreenWithPatBlt: NULL upper left point of brush supplied !" );

    ASSERTDD( ( (rop4 & 0xff00) >> 8 ) == (rop4 & 0xff),
              "NV4DmaScreenToScreenWithPatBlt: The rop has more than one byte set");


    //**************************************************************************
    // Init the local dma variables with the ones in ppdev.
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Setup variables
    //**************************************************************************
    
    rop3 = (ULONG)(rop4 & 0xff);            /* Get ROP3 (1 byte only)        */
    prb  = (RBRUSH*) pbo->pvRbrush;         /* Get pointer to realized brush */
    
    //**************************************************************************
    // Check if there is enough free space in the push buffer. We check ONE
    // time because this seems to be a great deal of overhead.  Furthermore,
    // we check for the largest possible amount we will use in the rest of the
    // function...
    //**************************************************************************
    
    NV_DMAPUSH_CHECKFREE((  (ULONG)(  3+8+65+ (c<<2)  )   ));  
    
    
    /* Max between:                                                            */
    /*                                                                         */
    /*  Solid case:    3+12+4*c                                                */
    /*                                                                         */
    /*                  and                                                    */
    /*                                                                         */
    /*  NonSolid case: 3+12+4*c                                                */
    /*                                                                         */
    /*                                                                         */
    /*                  and                                                    */
    /*                                                                         */
    /*                 3+8+65+4*c                                              */
    /*                                                                         */
    
    
    //**************************************************************************
    // Restore clip rect if there is need to...
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
    {
        //NV_DMAPUSH_CHECKFREE(((ULONG)(3)));  

        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

        ppdev->NVClipResetFlag = 0;       // Clear the "clipper is not to max"
                                          // flag
    }

    
    if(prb != NULL)  goto NV4StoSPat_NonSolid;

    //**************************************************************************
    //***[ BEGIN "Brush is Solid" ]*********************************************
    //**************************************************************************
       
    //**************************************************************************
    // 
    // Set the pattern object to all the same color (I'm not sure this is the
    // best way to do it, there might be another way to setup the NV4_IMAGE_BLIT
    // object so that it understands that the brush is solid, and that we only
    // need to give it the solid color...)
    // 
    // We are going to write a monochrome pattern to the pattern object, but all
    // "111..." so that we only have to set the foreground color.  This is better
    // than realizing a color pattern because it will take less data output to
    // the hardware
    //**************************************************************************


    // Determine the color format of our frame buffer and set the format of the
    // pattern object accordingly

    switch (ppdev->iBitmapFormat)
    {
        case BMF_32BPP:
           NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
           break;
        case BMF_16BPP:
           NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
           break;
        default:
        case BMF_8BPP:
           // Must be set to a legal value but hardware ignores it otherwise 
           NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
           break;
    }            
        
    //**********************************************************************
    // Set the following methods for context_pattern
    //    NV044_SET_MONOCHROME_FORMAT 
    //    NV044_SET_MONOCHROME_SHAPE  
    //    NV044_SET_PATTERN_SELECT    
    //    NV044_SET_MONOCHROME_COLOR0  
    //    NV044_SET_MONOCHROME_COLOR1,  
    //    NV044_SET_MONOCHROME_PATTERN0
    //    NV044_SET_MONOCHROME_PATTERN1
    //**********************************************************************

    NV_DMAPUSH_START(7, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT);
    NV_DMA_FIFO = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
    NV_DMA_FIFO = NV044_SET_MONOCHROME_SHAPE_8X_8Y;
    NV_DMA_FIFO = NV044_SET_PATTERN_SELECT_MONOCHROME;
    NV_DMA_FIFO = 0;                                             // Not used (color0)
    NV_DMA_FIFO = (pbo->iSolidColor | ppdev->AlphaEnableValue);  // Color to use when bit is "1" 

    NV_DMA_FIFO = (ULONG) 0xFFFFFFFF;                  // 8x8 = 64 bit pattern
    NV_DMA_FIFO = (ULONG) 0xFFFFFFFF;                  // " "

    
    // Set the rop
    
    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop3);   

    
    /***********************************************************************/
    /* Blit all rectangle regions                                          */
    /***********************************************************************/

    do 
    {
        //******************************************************************
        // Get width and height of the destination rectangle
        //******************************************************************

        width = parcl->right - parcl->left;
        height= parcl->bottom - parcl->top;


        xsrc = pptlSrc->x + (parcl->left - prclDst->left);
        ysrc = pptlSrc->y + (parcl->top - prclDst->top);


        //******************************************************************
        // Set methods for image_blit:
        //    NV05F_CONTROL_POINT_IN 
        //    NV05F_CONTROL_POINT_OUT
        //    NV05F_SIZE             
        //******************************************************************

        NV_DMAPUSH_START(3, BLIT_SUBCHANNEL, NV05F_CONTROL_POINT_IN );
        NV_DMA_FIFO = ( (ysrc << 16) | (xsrc & 0xffff) );
        NV_DMA_FIFO = ( (parcl->top << 16) | (parcl->left & 0xffff) );
        NV_DMA_FIFO = ( (height << 16) | (width & 0xffff));


        parcl++;

    } while (--c != 0);
    
    
    //**************************************************************************
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

    
    return;  

    
    //**************************************************************************
    //***[ END OF "Brush is Solid" ]********************************************
    //**************************************************************************

NV4StoSPat_NonSolid:
    

    //**************************************************************************
    // 
    // Set the pattern object with the data of our brush
    // (if we end here, the brush pattern is required, the brush is nonsolid)
    //
    //**************************************************************************

    if (prb->fl & RBRUSH_2COLOR)
    {
        /* Brush is monochrome                                                */
        /*                                                                    */
        /* Note: this code is copy pasted from fillpatfast for brush pattern  */
        /*       setup. if we see it's worth it, we might try to find a way   */
        /*       to optimize the alignement procedure (this seems it could    */
        /*       be done) or better yet, use brush caching)                   */
        

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

        do  
        {
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

        Color1 = prb->ulForeColor | (ppdev->AlphaEnableValue); // Background color
        Color0 = prb->ulBackColor | (ppdev->AlphaEnableValue); // Foreground color

        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        //NV_DMAPUSH_CHECKFREE(((ULONG)((c<<2)+12)));  

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop3);

        // Determine the color format
        switch (ppdev->iBitmapFormat)
        {
            case BMF_32BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            case BMF_16BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
               break;
            default:
            case BMF_8BPP:
               // Must be set to a legal value but hardware ignores it otherwise 
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
        }            

        //**********************************************************************
        // Set the following methods for context_pattern
        //    NV044_SET_MONOCHROME_FORMAT 
        //    NV044_SET_MONOCHROME_SHAPE  
        //    NV044_SET_PATTERN_SELECT    
        //    NV044_SET_MONOCHROME_COLOR0  
        //    NV044_SET_MONOCHROME_COLOR,  
        //    NV044_SET_MONOCHROME_PATTERN0
        //    NV044_SET_MONOCHROME_PATTERN1
        //**********************************************************************

        NV_DMAPUSH_START(7, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT);
        NV_DMA_FIFO = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
        NV_DMA_FIFO = NV044_SET_MONOCHROME_SHAPE_8X_8Y;
        NV_DMA_FIFO = NV044_SET_PATTERN_SELECT_MONOCHROME;
        NV_DMA_FIFO = Color0; 
        NV_DMA_FIFO = Color1;
        NV_DMA_FIFO = ppdev->AlignedBrush[0];
        NV_DMA_FIFO = ppdev->AlignedBrush[1];
        
                
        /***********************************************************************/
        /* Blit all rectangle regions                                          */
        /***********************************************************************/
        
        do 
        {
            //******************************************************************
            // Get width and height of the destination rectangle
            //******************************************************************

            width = parcl->right - parcl->left;
            height= parcl->bottom - parcl->top;

            
            xsrc = pptlSrc->x + (parcl->left - prclDst->left);
            ysrc = pptlSrc->y + (parcl->top - prclDst->top);
            
            
            //******************************************************************
            // Set methods for image_blit:
            //    NV05F_CONTROL_POINT_IN 
            //    NV05F_CONTROL_POINT_OUT
            //    NV05F_SIZE             
            //******************************************************************
            
            NV_DMAPUSH_START(3, BLIT_SUBCHANNEL, NV05F_CONTROL_POINT_IN );
            NV_DMA_FIFO = ( (ysrc << 16) | (xsrc & 0xffff) );
            NV_DMA_FIFO = ( (parcl->top << 16) | (parcl->left & 0xffff) );
            NV_DMA_FIFO = ( (height << 16) | (width & 0xffff));

            
            parcl++;

        } while (--c != 0);

    }

    else
    {
        /*******************************************************************/
        /* Else we have a color brush                                      */
        /*                                                                 */
        /* Note that the code to setup the pattern is copy pasted from     */
        /* fillpatfast                                                     */
        /*******************************************************************/

        //******************************************************************
        // Because we handle only 8x8 brushes, it is easy to compute the
        // number of pels by which we have to rotate the brush pattern
        // right and down.  Note that if we were to handle arbitrary sized
        // patterns, this calculation would require a modulus operation.
        //
        // NV4: Colored Pattern fills are now supported in hardware
        //******************************************************************

        xShift = pptlBrush->x;
        yShift = pptlBrush->y;

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

        do  
        {
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

        BytesPerSrcScan = ppdev->cjPelSize << 3; /* cjPelsize * 8 */

        //******************************************************************
        // Amount to increment for each scanline (in dwords)
        //******************************************************************

        ScanInc = (BytesPerSrcScan+3) >> 2;      // Number of dwords to increment

        //******************************************************************
        // Check if we've got enough room in the push buffer for max amount
        // of data possible for the later code...
        //******************************************************************

        //NV_DMAPUSH_CHECKFREE(((ULONG)(8+65+4*c)));  

        //******************************************************************
        // Send the Rop straight on through 
        // (We now support colored patterns in hardware)
        //******************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop3);

        //******************************************************************
        // Init color pattern in hardware
        //******************************************************************

        // Determine the color format
        switch (ppdev->iBitmapFormat)
        {
            case BMF_32BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            case BMF_16BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
               break;
            default:
            case BMF_8BPP:
               // Must be set to a legal value but hardware ignores it otherwise 
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
        }            


        //******************************************************************
        // Set the following methods for context_pattern
        //    NV044_SET_MONOCHROME_FORMAT 
        //    NV044_SET_MONOCHROME_SHAPE  
        //    NV044_SET_PATTERN_SELECT    
        //******************************************************************

        NV_DMAPUSH_START(3, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT);
        NV_DMA_FIFO = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
        NV_DMA_FIFO = NV044_SET_MONOCHROME_SHAPE_8X_8Y;
        NV_DMA_FIFO = NV044_SET_PATTERN_SELECT_COLOR;


        //******************************************************************
        // Init the 8x8 pattern for 8,16, or 32bpp 
        //******************************************************************

        if (ppdev->cjPelSize == 4)  // 32bpp
        {
            //**************************************************************
            // Init the 8x8 colored pattern
            //**************************************************************

            NV_DMAPUSH_START(64, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_X8R8G8B8(0));

            for (iy=0;iy<8;iy++)
            {
                //**********************************************************
                // Read one scanline at a time
                //**********************************************************

                ScanPtr = NextScan;

                for (ix=0;ix<8;ix++)
                {                    
                    NV_DMA_FIFO = ScanPtr[ix];
                }
                
                //**********************************************************
                // Advance to next pattern scanline
                //**********************************************************

                NextScan+=ScanInc;
            }
        }
        
        else if (ppdev->cjPelSize == 2) // 16bpp
        {
            //**************************************************************
            // 16bpp
            //**************************************************************

            //**************************************************************
            // Init the 8x8 colored pattern for 16bpp
            //**************************************************************

            NV_DMAPUSH_START(32, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_R5G6B5(0));
                       

            for (iy=0;iy<8;iy++)
            {
                //**********************************************************
                // Read one scanline at a time
                //**********************************************************

                ScanPtr = NextScan;

                for (ix=0;ix<4;ix++)
                {                    
                    NV_DMA_FIFO = ScanPtr[ix];  // 2 16bpp pixels at a time
                }
                
                //**********************************************************
                // Advance to next pattern scanline
                //**********************************************************

                NextScan+=ScanInc;
            }
        }
                                         

        else
        {
            //**************************************************************
            // 8bpp
            //**************************************************************

            //**************************************************************
            // Init the 8x8 colored pattern for 8bpp
            //**************************************************************

            NV_DMAPUSH_START(16, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_Y8(0));

            for (iy=0;iy<8;iy++)
            {
                //**********************************************************
                // Read one scanline at a time
                //**********************************************************

                ScanPtr = NextScan;

                for (ix=0;ix<2;ix++)
                {                    
                    NV_DMA_FIFO = ScanPtr[ix];  // 4 8bpp pixels at a time
                }
                
                //**********************************************************
                // Advance to next pattern scanline
                //**********************************************************

                NextScan+=ScanInc;
            }
                                                    
            
        }
            
        //**********************************************************************
        // Loop thru all the clipping rectangles
        //**********************************************************************
    
        do  
        {
            //******************************************************************
            // Get width and height of the destination rectangle
            //******************************************************************
    
            width = parcl->right - parcl->left;
            height= parcl->bottom - parcl->top;
    

            xsrc = pptlSrc->x + (parcl->left - prclDst->left);
            ysrc = pptlSrc->y + (parcl->top - prclDst->top);

            
            //******************************************************************
            // Set methods for image_blit:
            //    NV05F_CONTROL_POINT_IN 
            //    NV05F_CONTROL_POINT_OUT
            //    NV05F_SIZE             
            //******************************************************************

            NV_DMAPUSH_START(3, BLIT_SUBCHANNEL, NV05F_CONTROL_POINT_IN );
            NV_DMA_FIFO = ( (ysrc << 16) | (xsrc & 0xffff) );
            NV_DMA_FIFO = ( (parcl->top << 16) | (parcl->left & 0xffff) );
            NV_DMA_FIFO = ( (height << 16) | (width & 0xffff));
            
            //******************************************************************
            // Onto the next clipping rectangle
            //******************************************************************
    
            parcl++;
    
        } while (--c != 0);

    } // ...else the brush pattern is color (not mono)

    
    //**************************************************************************
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

}
//**************** End of function: NV4DmaScreenToScreenWithPatBlt *************
//******************************************************************************





//******************************************************************************
//
//  Function:   NV4DmaPushFillPatFast
//
//  Routine Description:
//      Using NV04_GDI_RECTANGLE_TEXT class to fill with patterns.
//      The IMAGE_APTTERN_SUBCHANNEL set up the pattern first,
//      and then the RECT_AND_TEXT_SUBCHANNEL fill up the rectangle with 
//      the pattern.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV4DmaPushFillPatFast(            // Type FNFILL
PDEV*           ppdev,
LONG            c,              // Can't be zero
RECTL*          prcl,           // List of rectangles to be filled, in relative
                                //   coordinates
ULONG           rop4,           // rop4
RBRUSH_COLOR    rbc,            // rbc.prb points to brush realization structure
POINTL*         pptlBrush)      // Pattern alignment

    {
    //BOOL        bTransparent;
    //BRUSHENTRY* pbe;          // Pointer to brush entry data, which is used
                                //   for keeping track of the location and status
                                //   of the pattern bits cached in off-screen
                                //   memory

    ULONG       Color0;
    ULONG       Color1;
    ULONG       AlphaEnableValue;
    ULONG       width;
    ULONG       height;
    ULONG       rop3;
    LONG        xShift;
    LONG        yShift;
    LONG        BytesPerSrcScan;
    BYTE*       SrcBits;
    BYTE*       DstBits;
    ULONG*      ScanPtr;
    RBRUSH*     prb;
    ULONG*      NextScan;
    LONG        ScanInc;
    LONG        cjLeft;
    LONG        cjRight;
    ULONG       i, ix,iy;


    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(c > 0, "Can't handle zero rectangles");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // TODO: Do I need to do anything special for transparent cases?
    //**************************************************************************
    
    rop3 = (ULONG)(rop4 & 0xff);
    
    //bTransparent = (((rop4 >> 8) & 0xff) != rop3);

    //**************************************************************************
    // Get pointer to brush entry
    //**************************************************************************

    prb = rbc.prb;
    
    //pbe = prb->apbe[IBOARD(ppdev)];

    //**************************************************************************
    // Need to add if statement if brush pattern hasn't changed
    // For now, just always re-calculate brush pattern
    // Performance boost of how much?
    //**************************************************************************

    //**************************************************************************
    // Get pixel depth for the current mode.
    // Then determine the ALPHA_CHANNEL enable value
    //**************************************************************************

    AlphaEnableValue = ppdev->AlphaEnableValue;

    
    //**************************************************************************
    // Check if there is enough free space in the push buffer. We check ONE
    // time because this is faster.  Furthermore, we check for the largest 
    // possible amount we will use in the rest of the function...
    //**************************************************************************


    //MAX of :  NV_DMAPUSH_CHECKFREE(((ULONG)(3+14+3*c     )));   and
    //          NV_DMAPUSH_CHECKFREE(((ULONG)(3+8+65+3*c     )));  

    NV_DMAPUSH_CHECKFREE(((ULONG)( 3+8+65+3*c )));  

    
    
    //**************************************************************************
    // Restore clip rect if we need to
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

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

        
        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop3);

        // Determine the color format
        switch (ppdev->iBitmapFormat)
            {
            case BMF_32BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            case BMF_16BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
               break;
            default:
            case BMF_8BPP:
               // Must be set to a legal value but hardware ignores it otherwise 
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            }            

        //**********************************************************************
        // Set the following methods for context_pattern
        //    NV044_SET_MONOCHROME_FORMAT 
        //    NV044_SET_MONOCHROME_SHAPE  
        //    NV044_SET_PATTERN_SELECT    
        //    NV044_SET_MONOCHROME_COLOR0  
        //    NV044_SET_MONOCHROME_COLOR,  
        //    NV044_SET_MONOCHROME_PATTERN0
        //    NV044_SET_MONOCHROME_PATTERN1
        //**********************************************************************

        NV_DMAPUSH_START(7, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT);
        NV_DMA_FIFO = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
        NV_DMA_FIFO = NV044_SET_MONOCHROME_SHAPE_8X_8Y;
        NV_DMA_FIFO = NV044_SET_PATTERN_SELECT_MONOCHROME;
        NV_DMA_FIFO = Color0; 
        NV_DMA_FIFO = Color1;
        NV_DMA_FIFO = ppdev->AlignedBrush[0];
        NV_DMA_FIFO = ppdev->AlignedBrush[1];
        
        //**********************************************************************
        // Since the sub-channel is same in the do-while loop, the color is set 
        // out of the do-while loop. (value can be anything tough, it's never
        // used anyway but method state must be valid, and it is volatile, being
        // invalidated when we use another subchannel or switch channels) - steph
        //**********************************************************************
        
        NV_DMAPUSH_START (1, RECT_AND_TEXT_SUBCHANNEL, NV04A_COLOR1_A);
        NV_DMA_FIFO = 0;

        do {
            //******************************************************************
            // Get width and height of the destination rectangle
            //******************************************************************

            width = prcl->right - prcl->left;
            height= prcl->bottom - prcl->top;

            NV_DMAPUSH_START (2, RECT_AND_TEXT_SUBCHANNEL, NV04A_UNCLIPPED_RECTANGLE(0))
            NV_DMA_FIFO = (prcl->left << 16) | prcl->top;
            NV_DMA_FIFO = (width << (16) | height);

            prcl++;

            } while (--c != 0);

        }


    else

        {
        //******************************************************************
        // Handle multi-color patterns in hardware (new for NV4)
        //******************************************************************

        //******************************************************************
        // Because we handle only 8x8 brushes, it is easy to compute the
        // number of pels by which we have to rotate the brush pattern
        // right and down.  Note that if we were to handle arbitrary sized
        // patterns, this calculation would require a modulus operation.
        //
        // NV4: Colored Pattern fills are now supported in hardware
        //******************************************************************

        xShift = pptlBrush->x;
        yShift = pptlBrush->y;

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
        // Send the Rop straight on through 
        // (We now support colored patterns in hardware)
        //******************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop3);

        //******************************************************************
        // Init color pattern in hardware
        //******************************************************************

        // Determine the color format
        switch (ppdev->iBitmapFormat)
            {
            case BMF_32BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            case BMF_16BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
               break;
            default:
            case BMF_8BPP:
               // Must be set to a legal value but hardware ignores it otherwise 
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            }            


        //******************************************************************
        // Set the following methods for context_pattern
        //    NV044_SET_MONOCHROME_FORMAT 
        //    NV044_SET_MONOCHROME_SHAPE  
        //    NV044_SET_PATTERN_SELECT    
        //******************************************************************

        NV_DMAPUSH_START(3, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT);
        NV_DMA_FIFO = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
        NV_DMA_FIFO = NV044_SET_MONOCHROME_SHAPE_8X_8Y;
        NV_DMA_FIFO = NV044_SET_PATTERN_SELECT_COLOR;

        //******************************************************************
        // Init the 8x8 pattern for 8,16, or 32bpp 
        //******************************************************************

        if (ppdev->cjPelSize == 4)  // 32bpp
            {
            //**************************************************************
            // Init the 8x8 colored pattern
            //**************************************************************

            NV_DMAPUSH_START(64, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_X8R8G8B8(0));

            for (iy=0;iy<8;iy++)
                {
                //**********************************************************
                // Read one scanline at a time
                //**********************************************************

                ScanPtr = NextScan;

                for (ix=0;ix<8;ix++)
                    {                    
                    NV_DMA_FIFO = ScanPtr[ix];
                    }
                
                //**********************************************************
                // Advance to next pattern scanline
                //**********************************************************

                NextScan+=ScanInc;
                }
            }
        else if (ppdev->cjPelSize == 2) // 16bpp
        
            {
            //**************************************************************
            // 16bpp
            //**************************************************************

            //**************************************************************
            // Init the 8x8 colored pattern for 16bpp
            //**************************************************************

            NV_DMAPUSH_START(32, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_R5G6B5(0));
                       

            for (iy=0;iy<8;iy++)
                {
                //**********************************************************
                // Read one scanline at a time
                //**********************************************************

                ScanPtr = NextScan;

                for (ix=0;ix<4;ix++)
                    {                    
                    NV_DMA_FIFO = ScanPtr[ix];  // 2 16bpp pixels at a time
                    }
                
                //**********************************************************
                // Advance to next pattern scanline
                //**********************************************************

                NextScan+=ScanInc;
                }
                                                    
            }
                                         

        else
        
            {
            //**************************************************************
            // 8bpp
            //**************************************************************

            //**************************************************************
            // Init the 8x8 colored pattern for 8bpp
            //**************************************************************

            NV_DMAPUSH_START(16, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_PATTERN_Y8(0));
                       

            for (iy=0;iy<8;iy++)
                {
                //**********************************************************
                // Read one scanline at a time
                //**********************************************************

                ScanPtr = NextScan;

                for (ix=0;ix<2;ix++)
                    {                    
                    NV_DMA_FIFO = ScanPtr[ix];  // 4 8bpp pixels at a time
                    }
                
                //**********************************************************
                // Advance to next pattern scanline
                //**********************************************************

                NextScan+=ScanInc;
                }
                                                    
            
            }
        
        //**********************************************************************
        //* Set the color method even tough we don't use it
        //**********************************************************************

        NV_DMAPUSH_START (1, RECT_AND_TEXT_SUBCHANNEL, NV04A_COLOR1_A);  
        NV_DMA_FIFO = 0;
        
        //**********************************************************************
        // Loop thru all the clipping rectangles
        //**********************************************************************
        do  {
            //******************************************************************
            // Get width and height of the destination rectangle
            //******************************************************************

            width = prcl->right - prcl->left;
            height= prcl->bottom - prcl->top;                             

            NV_DMAPUSH_START (2, RECT_AND_TEXT_SUBCHANNEL, NV04A_UNCLIPPED_RECTANGLE(0))
            NV_DMA_FIFO = (prcl->left << 16) | prcl->top;
            NV_DMA_FIFO = (width << (16) | height);

            //******************************************************************
            // Onto the next clipping rectangle
            //******************************************************************

            prcl++;

            } while (--c != 0);


        } // Multi-color pattern

    //**************************************************************************
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

} // End of FillPatFast routine



//******************************************************************************
//
//  Function:   NV4DmaPushMonoScanlineImageTransfer
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

#ifdef UNUSED_FUNC

VOID NV4DmaPushMonoScanlineImageTransfer(
PDEV*   ppdev,
BYTE*   pjSrc,              // Source pointer
ULONG   NumDwordsPerScan)
    {
    ULONG *SrcDataPtr;
    ULONG i;
    
    DECLARE_DMA_FIFO;
    //**************************************************************************
    // Init dma pusher variables
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Get Dword Ptr to Src Data
    //**************************************************************************

    SrcDataPtr = (DWORD *)pjSrc;

    //**************************************************************************
    // Class 'E' can output a maximum of 128 dwords at a time
    // If we've got less than 128 dwords, blast them out all at once.
    //**************************************************************************

    while (NumDwordsPerScan >= 128)

        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(129)));  

        //**********************************************************************
        // Blast out all glyph data to push buffer
        //**********************************************************************

        NV_DMAPUSH_START(128, RECT_AND_TEXT_SUBCHANNEL, NV04A_MONOCHROME_COLOR01_E(0) );
    
        //**********************************************************************
        // Set the following methods for RECT_AND_TEXT
        //     NV04A_MONOCHROME_COLOR01_E(0)
        //     NV04A_MONOCHROME_COLOR01_E(1)
        //     NV04A_MONOCHROME_COLOR01_E(2)
        //      ...
        //**********************************************************************
    
        for (i=0 ; i< 128; i++)
            NV_DMA_FIFO = SrcDataPtr[i];    

        NumDwordsPerScan-=128;
        SrcDataPtr += 128;

        }
                

    //**************************************************************************
    // Handle remaining dwords
    //**************************************************************************

    if (NumDwordsPerScan>0)
        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(NumDwordsPerScan+1)));  

        //**********************************************************************
        // Blast out all glyph data to push buffer
        //**********************************************************************

        NV_DMAPUSH_START(NumDwordsPerScan, RECT_AND_TEXT_SUBCHANNEL, NV04A_MONOCHROME_COLOR01_E(0) );
    
        //**********************************************************************
        // Set the following methods for RECT_AND_TEXT
        //     NV04A_MONOCHROME_COLOR01_E(0)
        //     NV04A_MONOCHROME_COLOR01_E(1)
        //     NV04A_MONOCHROME_COLOR01_E(2)
        //      ...
        //**********************************************************************
    
        for (i=0;i<NumDwordsPerScan;i++)
            NV_DMA_FIFO = SrcDataPtr[i];    

        }


    //**************************************************************************
    // Update cached dma count variable
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    }
    
#endif    

#define DWORD_BLK_XFER                                                                                  \
{                                                                                                       \
    ULONG i;                                                                                            \
    ULONG *SrcDataPtr;                                                                                  \
                                                                                                        \
    SrcDataPtr = (DWORD *)pjSrc;                                                                        \
    while (NumDwordsPerScan >= 128)                                                                     \
    {                                                                                                   \
        NV_DMAPUSH_CHECKFREE(((ULONG)(129)));                                                           \
                                                                                                        \
        NV_DMAPUSH_START(128, RECT_AND_TEXT_SUBCHANNEL, NV04A_MONOCHROME_COLOR01_E(0) );                \
                                                                                                        \
        for (i=0 ; i< 128; i++)                                                                         \
            NV_DMA_FIFO = SrcDataPtr[i];                                                                \
                                                                                                        \
        NumDwordsPerScan-=128;                                                                          \
        SrcDataPtr += 128;                                                                              \
    }                                                                                                   \
                                                                                                        \
    NV_DMAPUSH_CHECKFREE(((ULONG)(NumBytesPerScan)));                                                   \
    if (NumDwordsPerScan>0)                                                                             \
    {                                                                                                   \
        NV_DMAPUSH_START(NumDwordsPerScan, RECT_AND_TEXT_SUBCHANNEL, NV04A_MONOCHROME_COLOR01_E(0) );   \
                                                                                                        \
        for (i=0;i<NumDwordsPerScan;i++)                                                                \
            NV_DMA_FIFO = SrcDataPtr[i];                                                                \
                                                                                                        \
    }                                                                                                   \
}

#define LAST_DWORD_XFER                                                                                 \
    NV_DMAPUSH_CHECKFREE(((ULONG)(2)));                                                                 \
    NV_DMAPUSH_START(1, RECT_AND_TEXT_SUBCHANNEL, NV04A_MONOCHROME_COLOR01_E(0) );                      \
    NV_DMA_FIFO = LastDword;    


//******************************************************************************
//
//  Function:   NV4DmaPushXfer1bpp
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

VOID NV4DmaPushXfer1bpp(       // Type FNXFER
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
    LONG    dxSrc;
    LONG    dySrc;
    LONG    lSrcDelta;
    BYTE*   pjSrcScan0;
    BYTE*   pjSrc;
    LONG    cjSrc;
    LONG    xLeft;
    LONG    yTop;
    LONG    xBias;

    ULONG Color0;
    ULONG Color1;
    ULONG AlphaEnableValue;
    ULONG NumBytesPerScan;
    ULONG NumDwordsPerScan;
    ULONG OffsetOfLastDword;
    ULONG LastDword;
    ULONG width;
    ULONG height;
    ULONG SizeInWidth;
    ULONG rop3;
    ULONG ClipLeft;
    ULONG ClipTop;
    ULONG ClipWidth;
    ULONG ClipHeight;
    RECTL DstRect;
    
    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Make sure following conditions are true before continuing on
    //**************************************************************************

    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(pptlSrc != NULL && psoSrc != NULL, "Can't have NULL sources");
    ASSERTDD(((((rop4 & 0xff00) >> 8) == (rop4 & 0xff)) || (rop4 == 0xaacc)),
             "Expect weird rops only when opaquing");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

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

    //**************************************************************************
    // Adjust dest rect in case it is in offscreen memory
    //**************************************************************************

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
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(20)));  

        // Determine the color format
        switch (ppdev->iBitmapFormat)
            {
            case BMF_32BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            case BMF_16BPP:
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
               break;
            default:
            case BMF_8BPP:
               // Must be set to a legal value but hardware ignores it otherwise 
               NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
               break;
            }            

        //**********************************************************************
        // Set the following methods for context_pattern
        //    NV044_SET_MONOCHROME_FORMAT 
        //    NV044_SET_MONOCHROME_SHAPE  
        //    NV044_SET_PATTERN_SELECT    
        //    NV044_SET_MONOCHROME_COLOR0  
        //    NV044_SET_MONOCHROME_COLOR,  
        //    NV044_SET_MONOCHROME_PATTERN0
        //    NV044_SET_MONOCHROME_PATTERN1
        //**********************************************************************

        NV_DMAPUSH_START(7, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT);
        NV_DMA_FIFO = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
        NV_DMA_FIFO = NV044_SET_MONOCHROME_SHAPE_64X_1Y;
        NV_DMA_FIFO = NV044_SET_PATTERN_SELECT_MONOCHROME;
        NV_DMA_FIFO = (PatternColor | AlphaEnableValue); 
        NV_DMA_FIFO = (PatternColor | AlphaEnableValue);
        NV_DMA_FIFO = 0xffffffff;
        NV_DMA_FIFO = 0xffffffff;

        //**********************************************************************
        // Notify the rest of the driver that we changed the pattern
        //**********************************************************************

        ppdev->NVPatternResetFlag=1;

        //**********************************************************************
        // Get Rop3 value from Rop4
        //**********************************************************************

        rop3 = (ULONG)((BYTE)rop4);

        //**********************************************************************
        // Set Rop
        //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop3);

        //**********************************************************************
        // Set the following methods for RECT_AND_TEXT
        //      NV04A_CLIP_POINT0_E  
        //      NV04A_CLIP_POINT1_E  
        //      NV04A_COLOR0_E      
        //      NV04A_COLOR1_E      
        //      NV04A_SIZE_IN_E     
        //      NV04A_SIZE_OUT_E    
        //      NV04A_POINT_E       
        //**********************************************************************

        NV_DMAPUSH_START(7, RECT_AND_TEXT_SUBCHANNEL, NV04A_CLIP_POINT0_E );
        NV_DMA_FIFO = (  (ClipTop << 16) | (ClipLeft & 0xffff) );
        NV_DMA_FIFO = (((ClipTop+ClipHeight)<<16) | (ClipLeft+ClipWidth) );
        NV_DMA_FIFO = Color0 ;
        NV_DMA_FIFO = Color1 ;
        NV_DMA_FIFO = ( (height <<16) | SizeInWidth  );
        NV_DMA_FIFO = ( (height <<16) | width      );
        NV_DMA_FIFO = ( (yTop <<16) | (xLeft & 0xffff) );

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

        NumDwordsPerScan = SizeInWidth >> 5; //SizeInWidth / 32;
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
                        DWORD_BLK_XFER;

                    //**********************************************************
                    // Store bytes in reverse order (not necessary for NV?)
                    //**********************************************************

                    LastDword = (ULONG) (*(pjSrc + OffsetOfLastDword))  |
                            (*(pjSrc + OffsetOfLastDword + 1) << 8) |
                            (*(pjSrc + OffsetOfLastDword + 2) << 16);
                      
                    LAST_DWORD_XFER;        
                    
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            //******************************************************************
            // Output dwords at a time, then output last 2 bytes of data (in last dword)
            //******************************************************************

            case 2:
                do  {
                    if (NumDwordsPerScan > 0)
                        DWORD_BLK_XFER;


                    //**********************************************************
                    // Store bytes in reverse order (not necessary for NV?)
                    //**********************************************************

                    LastDword = (ULONG) (*(pjSrc + OffsetOfLastDword))  |
                            (*(pjSrc + OffsetOfLastDword + 1) << 8);

                    LAST_DWORD_XFER;        
                    
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            //******************************************************************
            // Output dwords at a time, then output last byte of data (in last dword)
            //******************************************************************

            case 1:
                do  {
                    if (NumDwordsPerScan > 0)
                        DWORD_BLK_XFER;

                    //**********************************************************
                    // Store bytes in reverse order (not necessary for NV?)
                    //**********************************************************

                    LastDword = (ULONG) (*(pjSrc + OffsetOfLastDword));

                    LAST_DWORD_XFER;        
                    
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            //******************************************************************
            // The amount of data we will send is already a multiple of 4.
            // We will blast out all data in dwords
            //******************************************************************

            case 0:
                do  {
                    DWORD_BLK_XFER;
                    
                    pjSrc += lSrcDelta;

                    } while (--height != 0);
                break;

            } // Switch (NumBytesPerScan)

        prcl++;
        } while (--c != 0); // Next Clipping rectangle

    //**************************************************************************
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

    }



//******************************************************************************
//
//  Function:   NV4_DmaPush_Send_Data
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

VOID NV4_DmaPush_Send_Data(
PDEV*   ppdev,
ULONG*  SrcDataPtr,              // Source pointer
ULONG   PixelCount)


    {
    ULONG DwordCount;
    ULONG i;
    
    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    DwordCount = (PixelCount+3)>>2; // was /4 

    //**************************************************************************
    // IMAGE_FROM_CPU can output a maximum of 1792 dwords at a time
    // If we've got less than that, blast them out all at once.
    //**************************************************************************

    while (DwordCount >= 1024)

        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(1025)));  

        NV_DMAPUSH_START(1024, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );
    
        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_COLOR(0)
        //     NV061_COLOR(1)
        //     NV061_COLOR(2)
        //      ...
        //**********************************************************************
    
        for (i=0 ; i< 1024; i++)
            NV_DMA_FIFO = SrcDataPtr[i];    

        DwordCount-=1024;
        SrcDataPtr += 1024;

        }
                

    //**************************************************************************
    // Handle remaining dwords
    //**************************************************************************

    if (DwordCount>0)
        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(DwordCount+1)));  

        //**********************************************************************
        // Blast out remaining data
        //**********************************************************************

        NV_DMAPUSH_START(DwordCount, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );
    
        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_COLOR(0)
        //     NV061_COLOR(1)
        //     NV061_COLOR(2)
        //      ...
        //**********************************************************************
    
        for (i=0;i<DwordCount;i++)
            NV_DMA_FIFO = SrcDataPtr[i];    

        }


    //**************************************************************************
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;


    }

//******************************************************************************
//
//  Function:   NV4DmaPushXfer4bpp
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

VOID NV4DmaPushXfer4bpp(     // Type FNXFER
PDEV*       ppdev,
LONG        c,          // Count of rectangles, can't be zero
RECTL*      prcl,       // Array of relative coordinates destination rectangles
ROP4        rop4,       // rop4
SURFOBJ*    psoSrc,     // Source surface
POINTL*     pptlSrc,    // Original unclipped source point
RECTL*      prclDst,    // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG PatternColor,
BLENDOBJ*   pBlendObj)  // Not used

    {
    LONG    lSrcDelta;
    BYTE*   pjSrcScan0;
    BYTE*   pjSrc;
    LONG    cjSrc;

    ULONG depth;
    LONG BytesPerSrcScan;
    ULONG Rop3;
    LONG xsrc,ysrc;
    BYTE *NextScan;
    ULONG yscan;
    BYTE *ScanPtr;
    LONG ScanInc;
    BYTE *DstBits;
    ULONG SizeInWidth;
    LONG clip_x,clip_y;
    ULONG clip_width,clip_height;
    BYTE TempPixelData[XLATE_BUFFER_SIZE];
    DSURF*  pdsurfSrc;
    ULONG*  pulXlate;
    BYTE  PixelDataByte;
    ULONG PixelDataDword;
    ULONG   LoopCount;
    ULONG PixelCount;
    ULONG i;

    DECLARE_DMA_FIFO;
    
    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Get ready to translate the 4bpp colors
    //**************************************************************************

    ASSERTDD(pxlo != NULL, "Xfer4BPP, NULL XLATEOBJ");
    pulXlate  =  pxlo->pulXlate;

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

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
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //***************************************************************************

    if (ppdev->NVClipResetFlag)
       {
       //***********************************************************************
       // Check if we've got enough room in the push buffer
       //***********************************************************************

       NV_DMAPUSH_CHECKFREE(((ULONG)(3)));  

       //***********************************************************************
       // Set Point and Size methods for clip:
       //    NV019_SET_POINT
       //    NV019_SET_SIZE
       //***********************************************************************

       NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
       NV_DMA_FIFO = 0;
       NV_DMA_FIFO = ppdev->dwMaxClip; 

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

        NextScan = &(pjSrcScan0[(LONG)((xsrc>>1) + ysrc*BytesPerSrcScan)]);

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

        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  

        //**********************************************************************
        // Setup NV Blit parameters for CPU to SCREEN blit
        //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_POINT     
        //     NV061_SIZE_OUT  
        //     NV061_SIZE_IN 
        //**********************************************************************

        NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
        NV_DMA_FIFO = ( ( clip_y << 16) | ( clip_x & 0xffff)  );
        NV_DMA_FIFO = ( (clip_height <<16) | clip_width  );

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

        //*********************************************************************
        //     NV061_SIZE_IN 
        //*********************************************************************

        NV_DMA_FIFO = ( (clip_height <<16) | (SizeInWidth)  );

        //*********************************************************************
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
        //*********************************************************************

        //*********************************************************************
        //
        // The IA64 will fault on unaligned addresses.  Therefore use
        // the "odd" path in these cases to prevent DWORD sized accesses
        // that have this problem. Note that xsrc being divisible by 8
        // implies that the source address is divisible by 4.
        //
        //*********************************************************************

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

#ifdef _WIN64
        if ( xsrc & 0x7)
#else
        if ( xsrc & 0x1)
#endif
            {

            for (yscan=0;yscan < clip_height; yscan++)
                {
                ScanPtr = NextScan;
                PixelCount=0;
                //**************************************************************
                // For each byte, we read 2 pixels.  We will count by bytes.
                // If xsrc is ODD, we start on the ODD pixel so we read the LOWER
                // nibble of the current source byte FIRST, then we read the UPPER nibble
                // Vice Versa if xsrc is Even (which it can be for Win64's enforcement of DWord aligned access)
                // 
                //**************************************************************

                LoopCount= clip_width>>1 ;      // Odd pel handled separately

                //**************************************************************
                // Get the current 2 pixels of data
                //**************************************************************

                PixelDataByte = *ScanPtr;

                while (LoopCount-- > 0)
                    {
                    // If xsrc is ODD - no shift since first read in loop is lower nibble of current BYTE
                    TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataByte >> ((xsrc & 0x1) ? 0 : 4)) & 0xf];

                    // If xsrc is ODD, prior read was lower nibble of BYTE, so advance to next BYTE
                    if (xsrc & 0x1) 
                        {
                        ScanPtr++;
                        PixelDataByte =*ScanPtr ;
                        }
                    
                    // If xsrc is ODD - shift since second read in loop is upper nibble of new byte read above
                    TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataByte >> ((xsrc & 0x1) ? 4 : 0)) & 0xf];

                    // If xsrc is EVEN, prior read was lower nibble of BYTE, so advance to next BYTE
                    if (!(xsrc & 0x1))
                        {
                        ScanPtr++;
                        PixelDataByte =*ScanPtr ;
                        }


                    //**********************************************************
                    // Check if we need to flush the data and
                    // send it to NV hardware
                    //**********************************************************

                    if (PixelCount >= NV_HALF_FIFO_SIZE)
                        {
                        UPDATE_PDEV_DMA_COUNT;
                        NV4_DmaPush_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount);
                        UPDATE_LOCAL_DMA_COUNT;
                        PixelCount=0;
                        }
                    }

                //**************************************************************
                // Check for any straggling pixel leftover
                // If xsrc is ODD - no shift since we want lower nibble of current BYTE
                //**************************************************************
                if (clip_width & 1)
                    TempPixelData[PixelCount++] = (BYTE) pulXlate[(PixelDataByte >> ((xsrc & 0x1) ? 0 : 4)) & 0xf];

                //**************************************************************
                // Flush any remaining pixel data
                //**************************************************************

                if (PixelCount!=0)
                    {
                    UPDATE_PDEV_DMA_COUNT;
                    NV4_DmaPush_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount);
                    UPDATE_LOCAL_DMA_COUNT;
                    }
                    
                NextScan+=ScanInc;

            //******************************************************************
            // Kickoff buffer after each scanline
            //******************************************************************

                UPDATE_PDEV_DMA_COUNT;
                NV4_DmaPushSend(ppdev);             // Time to kickoff the buffer
                UPDATE_LOCAL_DMA_COUNT;

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
                        UPDATE_PDEV_DMA_COUNT;
                        NV4_DmaPush_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount);
                        UPDATE_LOCAL_DMA_COUNT;
                        PixelCount=0;
                        }

                    } while (LoopCount != 0);

                //**************************************************************
                // Flush any remaining pixel data
                //**************************************************************

                if (PixelCount!=0)
                    {
                    UPDATE_PDEV_DMA_COUNT;
                    NV4_DmaPush_Send_Data(ppdev,(ULONG *)TempPixelData,PixelCount);
                    UPDATE_LOCAL_DMA_COUNT;
                    }
                    

                NextScan+=ScanInc;
                
            //******************************************************************
            // Kickoff buffer after each scanline
            //******************************************************************

                UPDATE_PDEV_DMA_COUNT;
                NV4_DmaPushSend(ppdev);             // Time to kickoff the buffer
                UPDATE_LOCAL_DMA_COUNT;
                }
            }

        if (--c == 0)
            {
            return;
            }

        prcl++;

        } // while (TRUE)...




    }


//******************************************************************************
//
//  Function:   NV4DmaPushXferNative
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


VOID NV4DmaPushXferNative(     // Type FNXFER
PDEV*       ppdev,
LONG        c,          // Count of rectangles, can't be zero
RECTL*      prcl,       // Array of relative coordinates destination rectangles
ROP4        rop4,       // rop4
SURFOBJ*    psoSrc,     // Source surface
POINTL*     pptlSrc,    // Original unclipped source point
RECTL*      prclDst,    // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG       PatternColor,
BLENDOBJ*   pBlendObj)
{
    LONG    lSrcDelta;
    BYTE*   SrcBits;
    BYTE*   pjSrc;
    LONG    cjSrc;

    ULONG   BytesPerSrcPixel;
    LONG    BytesPerSrcScan;
    ULONG   Rop3;
    LONG    xsrc,ysrc;
    ULONG   xsrcOffset;
    ULONG   AlignMask;
    ULONG*  NextScan;
    ULONG   BytesPerDstScan;
    ULONG   yscan;
    ULONG*  ScanPtr;
    LONG    ScanInc;
    BYTE*   DstBits;
    ULONG   SizeInWidth;
    LONG    clip_x,clip_y;
    ULONG   clip_width,clip_height;
    DSURF*  pdsurfSrc;
    ULONG   ByteCount;
    ULONG   i;
    ULONG   TempPixelData1;
    ULONG   BytesWritten;

    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD((pxlo == NULL) || !pxlo->flXlate || (pxlo->flXlate & XO_TRIVIAL) ||
            ((pxlo->flXlate == 0x10) && (OglIsEnabled(ppdev))),
            "Can handle trivial xlate only");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff), "Expect only a rop2");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

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
    // Setup Blender if needed  -paul
    // Only supported on Win2k
    //**************************************************************************
    
#if (_WIN32_WINNT >= 0x0500)
    
    if (pBlendObj) 
        {
        //**************************************************************************
        // Need to load alpha value into all bytes of the word since the hardware
        // uses each byte independently as an alpha value
        //**************************************************************************
        ULONG constAlpha = 0x00000000;
        
        constAlpha = pBlendObj->BlendFunction.SourceConstantAlpha & 0x000000FF;
        constAlpha |=constAlpha <<  8;
        constAlpha |=constAlpha <<  16;

        NV_DMAPUSH_CHECKFREE(((ULONG)(5*2)));
        
        if (ppdev->dDrawSpareSubchannelObject != NV_DD_CONTEXT_BETA4)
            {
            NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), NV_DD_CONTEXT_BETA4);
            ppdev->dDrawSpareSubchannelObject = NV_DD_CONTEXT_BETA4;
            }
        
        NV_DMAPUSH_WRITE1(DD_SPARE, NV072_SET_BETA_FACTOR, constAlpha);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_OPERATION, NV061_SET_OPERATION_BLEND_PREMULT);
        
        // If Alpha in source pixels, change source format - default 061 format has no alpha
        if (pBlendObj->BlendFunction.AlphaFormat & AC_SRC_ALPHA) 
            {
            // 32bpp source if it has alpha
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, NV061_SET_COLOR_FORMAT_LE_A8R8G8B8);
            }
            else
            {
            // If no alpha, set to 16bpp or 32bpp format, w/ no src alpha.  HW then treats alpha as 0xff (1.0)
            // Note caller may have called AlphaPreserve to set source (061) format to format w/ alpha
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, 
                              (psoSrc->iBitmapFormat == BMF_16BPP) ? NV061_SET_COLOR_FORMAT_LE_R5G6B5 : 
                                                                     NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
            }
        
        if ((ppdev->iBitmapFormat == BMF_16BPP) &&
            (ppdev->CurrentClass.ContextSurfaces2D != NV04_CONTEXT_SURFACES_2D))
            {
            // For post NV4 hw, we turn dither off for the blend
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV065_SET_COLOR_CONVERSION, NV065_SET_COLOR_CONVERSION_TRUNCATE);
            }

        }
    else if (psoSrc->iBitmapFormat != ppdev->iBitmapFormat)
        {
        // Some non-blend Native Xfers support source format different from vidmem (eg. TransparentBlt)
        NV_DMAPUSH_CHECKFREE(((ULONG)(1*2)));
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, 
                      (psoSrc->iBitmapFormat == BMF_16BPP) ? NV061_SET_COLOR_FORMAT_LE_R5G6B5 : 
                                                             NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
        }
#endif //(_WIN32_WINNT >= 0x0500)
    
    //**************************************************************************
    // Setup NV Blit parameters for CPU (MEMORY to SCREEN) blit
    //**************************************************************************

    Rop3 = (rop4 & 0xff);               // Get bottom byte

    //**********************************************************************
    // Compute the alignment mask for rounding up SizeIn and for computing
    // correctly aligned addresses for IA64.
    //**********************************************************************

    if (psoSrc->iBitmapFormat == BMF_8BPP)
        {
        BytesPerSrcPixel = 1;
        AlignMask = 0x3;
        }
    else if (psoSrc->iBitmapFormat == BMF_16BPP)
        {
        BytesPerSrcPixel = 2;
        AlignMask = 0x1;
        }
    else
        {
        BytesPerSrcPixel = 4;
        AlignMask = 0x0;
        }

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

        if ((prclDst->right - prclDst->left) < (prcl->right - clip_x))
            {
            clip_width = prclDst->right - prclDst->left;
            }
        else
            {
            clip_width = prcl->right - clip_x;
            }
        if ((prclDst->bottom - prclDst->top) < (prcl->bottom - clip_y))
            {
            clip_height = prclDst->bottom - prclDst->top;
            }
        else
            {
            clip_height = prcl->bottom - clip_y;
            }

        //**********************************************************************
        // Top left coordinate of source bitmap on source surface
        //**********************************************************************

        xsrc = pptlSrc->x + clip_x - prclDst->left;
        ysrc = pptlSrc->y + clip_y - prclDst->top;

        //**********************************************************************
        // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
        // ignore/clip the extra pixel that we send it, so that it won't
        // get included as part of the next scanline. Since we always
        // send DWords at a time, we may actually send more data to the NV
        // engine than is necessary.  So clip it by specifying a larger SizeIn
        // That is, we always send DWORDS at a time for each scanline.
        // For 8bpp, we may specify up to 3 additional (ignored) pixels
        //**********************************************************************

        //**********************************************************************
        // The IA64 architecture provides an additional complication: unaligned
        // accesses cause the process to fault.  Detect the unaligned cases and
        // mask off the offending address bits; once again we'll be sending more
        // bits to the NV engine as necessary.  Rely on the clipper (in this
        // case only) to drop the leading left edge of pixels.
        //**********************************************************************


        //**********************************************************************
        // Determine whether we will have a misaligned access on the IA64.
        // Alignment poses no problem for the X86 so clear the adjustment.
        //**********************************************************************

#ifdef _WIN64
        xsrcOffset = xsrc & AlignMask;
#else
        xsrcOffset = 0;
#endif

        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(12)));

#if defined(_WIN64)

        if (xsrcOffset)
            {
            //**********************************************************************
            // For unaligned (DWORD) accesses, set the clipping region to the
            // original dest size to prevent drawing any more than requested.
            //**********************************************************************

            //******************************************************************
            // Set Point and Size methods for clip:
            //    NV019_SET_POINT
            //    NV019_SET_SIZE
            //******************************************************************

            NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
            NV_DMA_FIFO = ((0 << 16) |  clip_x);
            NV_DMA_FIFO = ((ppdev->cyMemory << 16) | clip_width); 

            ppdev->NVClipResetFlag=1;       // Clipping rectangle has been reset

            // adjust src x to DWORD boundary
            xsrc -= xsrcOffset;

            // adjust clip_x, clip_width - used for Dst x and width from here on
            clip_x -= xsrcOffset;
            clip_width += xsrcOffset;
            }
    else
#endif
        //**********************************************************************
        // Reset clipping rectangle to full screen extents if necessary
        // Resetting the clipping rectangle causes delays so we want to do it
        // as little as possible!
        //**********************************************************************

        if (ppdev->NVClipResetFlag)
            {

            //******************************************************************
            // Set Point and Size methods for clip:
            //    NV019_SET_POINT
            //    NV019_SET_SIZE
            //******************************************************************

            NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
            NV_DMA_FIFO = 0;
            NV_DMA_FIFO = ppdev->dwMaxClip; 

            ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
            }

        //**********************************************************************
        // Initialize the ImageFromCpu object
        //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_POINT     
        //     NV061_SIZE_OUT  
        //     NV061_SIZE_IN 
        //**********************************************************************

        NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
        NV_DMA_FIFO = ( (clip_y <<16) | (clip_x & 0xffff) );
        NV_DMA_FIFO = ( (clip_height <<16) | clip_width );

        //**********************************************************************
        // Calculate the number of DWORDs to output for each scanline and
        // compute the address to the start of the source.  If the original 
        // value would have created an unaligned source address, back up the
        // left margin and increase the width to compensate.  In either case
        // round the SizeIn argument as described above.
        //**********************************************************************
    
        SizeInWidth = ((clip_width + AlignMask) & ~AlignMask);
        NextScan = (ULONG *)(&(SrcBits[(LONG)((xsrc*BytesPerSrcPixel) +
                                              (ysrc*BytesPerSrcScan))]));

        BytesPerDstScan = clip_width * BytesPerSrcPixel;

        //**********************************************************************
        //     NV061_SIZE_IN 
        //**********************************************************************

        NV_DMA_FIFO = ( (clip_height <<16) | SizeInWidth  );

        //**********************************************************************
        // Amount to increment for each scanline
        //**********************************************************************

        ScanInc = (BytesPerSrcScan+3) >> 2;      // Number of dwords to increment

#ifdef _X86_
        //**********************************************************************
        // Use assembly to output as fast as possible
        //**********************************************************************

        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPush_Transfer_MemToScreen_Data(ppdev,BytesPerDstScan,clip_height,NextScan,ScanInc);
        UPDATE_LOCAL_DMA_COUNT;

#else   // ifdef _x86_

        //*********************************************************************
        // The following C code is the exact equivalent of
        // NV4_Transfer_MemToScreen_Data in assembly.
        //*********************************************************************

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

        for (yscan=0;yscan < clip_height; yscan++)
            {
            ByteCount=BytesPerDstScan; 
            ScanPtr = NextScan;

            //******************************************************************
            // IMAGE_FROM_CPU can output a maximum of 1792 dwords at a time
            // If we've got less than that, blast them out all at once.
            //******************************************************************

            while (ByteCount >= 1024 * sizeof(ULONG))

                {
                //**************************************************************
                // Check if we've got enough room in the push buffer
                //**************************************************************

                NV_DMAPUSH_CHECKFREE(((ULONG)(1025)));  

                NV_DMAPUSH_START(1024, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );
    
                //**************************************************************
                // Set the following methods for IMAGE_FROM_CPU
                //     NV061_COLOR(0)
                //     NV061_COLOR(1)
                //     NV061_COLOR(2)
                //      ...
                //**************************************************************
            
                for (i=0 ; i< 1024; i++)
                    NV_DMA_FIFO = ScanPtr[i];    

                ByteCount -= (1024 * sizeof(ULONG));
                ScanPtr += 1024;

                }


            //******************************************************************
            // Handle remaining dwords
            //******************************************************************

            //**************************************************************
            // Check if we've got enough room in the push buffer
            //**************************************************************

            NV_DMAPUSH_CHECKFREE(((ULONG)((ByteCount>>2) + 3)));  

            if (ByteCount>=4)
                {

                //**************************************************************
                // Blast out remaining data
                //**************************************************************

                NV_DMAPUSH_START((ByteCount>>2), IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );
    
                //**************************************************************
                // Set the following methods for IMAGE_FROM_CPU
                //     NV061_COLOR(0)
                //     NV061_COLOR(1)
                //     NV061_COLOR(2)
                //      ...
                //**************************************************************
        
                for (i=0;i<(ByteCount>>2);i++)
                    NV_DMA_FIFO = ScanPtr[i];    
        
                ScanPtr += (ByteCount>>2);
                ByteCount -= ((ByteCount>>2) * sizeof(ULONG));

                }

            //
            // Pad src scanline.
            //
            if (ByteCount)
                {
                //**************************************************************
                // Check if we've got enough room in the push buffer
                //**************************************************************

                TempPixelData1 = 0;
                for (i = 0; i < ByteCount; i++)
                {
                    TempPixelData1 |= ((PBYTE) (ScanPtr))[i] << (8 * i);
                }

                NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) ,  TempPixelData1  );

                }

            NextScan+=ScanInc;

            //******************************************************************
            // Kickoff buffer after each scanline
            //******************************************************************

            UPDATE_PDEV_DMA_COUNT;
            NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
            UPDATE_LOCAL_DMA_COUNT;

            }
#endif  // ifdef else _X86_

        if (--c == 0)
            {
            goto All_Done;
            }

        prcl++;

        } // while (TRUE)...
All_Done:
    
#if (_WIN32_WINNT >= 0x0500)
                                                                     
    // If blending, restore Object state to defaults before return
    if (pBlendObj) 
        {
        NV_DMAPUSH_CHECKFREE((ULONG)(3*2));  
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_OPERATION, NV061_SET_OPERATION_ROP_AND);
        // Restore color format which was changed to be the source image format
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, 
                              (ppdev->iBitmapFormat == BMF_16BPP) ? NV061_SET_COLOR_FORMAT_LE_R5G6B5 : 
                                                                    NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
        if ((ppdev->iBitmapFormat == BMF_16BPP) &&
            (ppdev->CurrentClass.ContextSurfaces2D != NV04_CONTEXT_SURFACES_2D))
            {
            // For post NV4 hw, we turned dither off for the blend, so restore here
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV065_SET_COLOR_CONVERSION, NV065_SET_COLOR_CONVERSION_DITHER);
            }

        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
        }   
    else if (psoSrc->iBitmapFormat != ppdev->iBitmapFormat)
        {
        // Restore source format if earlier changed since is different from vidmem format
        NV_DMAPUSH_CHECKFREE(((ULONG)(1*2)));
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, 
                              (ppdev->iBitmapFormat == BMF_16BPP) ? NV061_SET_COLOR_FORMAT_LE_R5G6B5 : 
                                                                    NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
        }

#endif //(_WIN32_WINNT >= 0x0500)
 
    return;
}

//******************************************************************************
//
//  Function:   NV4DmaPushMemToScreenBlit
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

VOID NV4DmaPushMemToScreenBlt(
PDEV*   ppdev,
SURFOBJ* psoSrc,
POINTL* pptlSrc,
RECTL*  prclDst)

    {
    ULONG width,height;
    ULONG xscan,yscan;
    ULONG xsrc,ysrc;
    ULONG xdst,ydst;
    ULONG xsrcOffset;
    ULONG depth;
    ULONG AlignMask;
    LONG BytesPerSrcScan;
    ULONG ByteCount;
    ULONG SizeInWidth;
    ULONG BytesPerDstScan;
    ULONG *ScanPtr;
    ULONG *NextScan;
    LONG ScanInc;
    BYTE* SrcBits;
    DSURF* pdsurfSrc;
    ULONG TempPixelData1;
    ULONG BytesWritten;
    ULONG i;

    DECLARE_DMA_FIFO;
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
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

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
    // Amount to increment for each scanline
    //**************************************************************************

    ScanInc = (BytesPerSrcScan+3) >> 2;
        
    //**************************************************************************
    // Top left coordinate of destination on destination surface
    //**************************************************************************

    xdst = prclDst->left;
    ydst = prclDst->top;

    //**************************************************************************
    // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
    // That is, the lower right coordinate is NOT included
    // For NV, the width/height actually means the number of
    // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
    // when calculating width and height.
    //**************************************************************************

    width = prclDst->right - xdst;
    height = prclDst->bottom - ydst;

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
    // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
    // ignore/clip the extra pixel that we send it, so that it won't
    // get included as part of the next scanline. Since we always
    // send DWords at a time, we may actually send more data to the NV
    // engine than is necessary.  So clip it by specifying a larger SizeIn
    // That is, we always send DWORDS at a time for each scanline.
    // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
    //**************************************************************************

    //**************************************************************************
    // The IA64 architecture provides an additional complication: unaligned
    // accesses cause the process to fault.  Detect the unaligned cases and
    // mask off the offending address bits; once again we'll be sending more
    // bits to the NV engine as necessary.  Rely on the clipper (in this case
    // only) to drop the leading left edge of pixels.
    //**************************************************************************

    //**************************************************************************
    // Compute the alignment mask for rounding up SizeIn and for computing
    // correctly aligned addresses for IA64.
    //**************************************************************************

    if (depth == 8)
        AlignMask = 0x3;
    else if (depth == 16)
        AlignMask = 0x1;
    else
        AlignMask = 0x0;

    //**************************************************************************
    // Determine whether we will have a misaligned access on the IA64.
    // Alignment poses no problem for the X86 so clear the offset.
    //**************************************************************************

#ifdef _WIN64
    xsrcOffset = xsrc & AlignMask;
#else
    xsrcOffset = 0;
#endif

    //**************************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(9)));

#if defined(_WIN64)
   //***************************************************************************
   // For unaligned (DWORD) accesses, set the clipping region to the
   // original dest size to prevent drawing any more than requested.
   //***************************************************************************

   if (xsrcOffset)
       {
       //***********************************************************************
       // Set Point and Size methods for clip:
       //    NV019_SET_POINT
       //    NV019_SET_SIZE
       //***********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = ((0 << 16) |  xdst);
        NV_DMA_FIFO = ((ppdev->cyMemory << 16) | width); 
        
        // adjust src x to DWORD boundary
        xsrc -= xsrcOffset;

        // adjust xdst, width - used for Dst x and width from here on
        xdst -= xsrcOffset;
        width += xsrcOffset;

        ppdev->NVClipResetFlag=1;       // Clipping rectangle has been reset
       }
   else
#endif
   //***************************************************************************
   // Reset clipping rectangle to full screen extents if necessary
   // Resetting the clipping rectangle causes delays so we want to do it
   // as little as possible!
   //***************************************************************************

   if (ppdev->NVClipResetFlag)
       {
       //***********************************************************************
       // Set Point and Size methods for clip:
       //    NV019_SET_POINT
       //    NV019_SET_SIZE
       //***********************************************************************

       NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
       NV_DMA_FIFO = 0;
       NV_DMA_FIFO = ppdev->dwMaxClip; 

       ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
       }

    //**************************************************************************
    // Initialize the ImageFromCpu object
    //**************************************************************************

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , NV_SRCCOPY);

    //**************************************************************************
    // Set the following methods for IMAGE_FROM_CPU
    //     NV061_POINT     
    //     NV061_SIZE_OUT  
    //     NV061_SIZE_IN 
    //**************************************************************************

    NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
    NV_DMA_FIFO = ( (ydst <<16) | (xdst & 0xffff) );
    NV_DMA_FIFO = ( (height <<16) | width );

    //**************************************************************************
    // Calculate the number of DWORDs to output for each scanline and
    // compute the address to the start of the source.  If the original 
    // value would have created an unaligned source address, back up the
    // left margin and increase the width to compensate.  In either case
    // round the SizeIn argument as described above.
    //**************************************************************************
    
    SizeInWidth = ((width + AlignMask) & ~AlignMask);
    BytesPerDstScan = width * (depth>>3);
    NextScan = (ULONG *)(&(SrcBits[(LONG)((xsrc*(depth>>3)) +
                                          (ysrc*BytesPerSrcScan))]));

    //**************************************************************************
    //     NV061_SIZE_IN 
    //**************************************************************************

    NV_DMA_FIFO = ( (height <<16) | (SizeInWidth) );

#ifdef _X86_
    //**************************************************************************
    // Use assembly to output as fast as possible
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPush_Transfer_MemToScreen_Data(ppdev,BytesPerDstScan,height,NextScan,ScanInc);

#else   // ifdef _x86_

    //**************************************************************************
    // The following C code is the exact equivalent of
    // NV4_Transfer_MemToScreen_Data in assembly.
    //**************************************************************************

    //***************************************************************************
    // Output one scan at a time
    //***************************************************************************

    for (yscan=0;yscan < height; yscan++)
        {
        ByteCount = BytesPerDstScan;

        ScanPtr = NextScan;

        //**********************************************************************
        // IMAGE_FROM_CPU can output a maximum of 1792 dwords at a time
        // If we've got less than that, blast them out all at once.
        //**********************************************************************

        while (ByteCount >= 1024 * sizeof(ULONG))

            {
            //******************************************************************
            // Check if we've got enough room in the push buffer
            //******************************************************************

            NV_DMAPUSH_CHECKFREE(((ULONG)(1025)));  


            NV_DMAPUSH_START(1024, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //******************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //******************************************************************
        
            for (i=0 ; i< 1024; i++)
                NV_DMA_FIFO = ScanPtr[i];    

            ByteCount -= (1024 * sizeof(ULONG));
            ScanPtr += 1024;

            }


        //******************************************************************
        // Check if we've got enough room in the push buffer
        //******************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)((ByteCount>>2) + 3)));  

        //**********************************************************************
        // Handle remaining dwords
        //**********************************************************************

        if (ByteCount>=4)
            {
            //******************************************************************
            // Blast out remaining data
            //******************************************************************

            NV_DMAPUSH_START((ByteCount>>2), IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //******************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //******************************************************************
    
            for (i=0;i<(ByteCount>>2);i++)
                NV_DMA_FIFO = ScanPtr[i];    
    
            ScanPtr += (ByteCount>>2);
            ByteCount -= ((ByteCount>>2) * sizeof(ULONG));
            }


        //**********************************************************************
        // Pad src scanline.
        //**********************************************************************
        if (ByteCount)
            {
            ULONG   i;
            TempPixelData1 = 0;
            for (i = 0; i < ByteCount; i++)
                {
                TempPixelData1 |= ((PBYTE) (ScanPtr))[i] << (8 * i);
                }

            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) , TempPixelData1  );
            }

        NextScan+=ScanInc;

        //*********************************************************************
        // Kickoff buffer after each scanline
        //*********************************************************************

        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
        UPDATE_LOCAL_DMA_COUNT;
        }

#endif  // _X86_

    }

//******************************************************************************
//
//  Function:   NV4DmaPushMemToScreenWithPatternBlt()
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


VOID NV4DmaPushMemToScreenWithPatternBlt(
PDEV*   ppdev,
SURFOBJ* psoSrc,
POINTL* pptlSrc,
RECTL*  prclDst,
ULONG rop3,
RBRUSH_COLOR   *rbc,
RECTL*  prclClip
)


    {
    ULONG width,height;
    ULONG xscan,yscan;
    ULONG xsrc,ysrc;
    ULONG xdst,ydst;
    ULONG depth;
    ULONG xsrcOffset;
    LONG BytesPerSrcScan;
    ULONG ByteCount;
    ULONG SizeInWidth;
    ULONG BytesPerDstScan;
    ULONG TempPixelData1;
    ULONG *ScanPtr;
    ULONG *NextScan;
    LONG ScanInc;
    BYTE* SrcBits;
    DSURF* pdsurfSrc;
    ULONG   i;
    ULONG AlphaEnableValue;
    DECLARE_DMA_FIFO;


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
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

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
    // Determine whether we will have a misaligned access on the IA64.
    // Alignment poses no problem for the X86 so clear the offset.
    //**************************************************************************

#ifdef _WIN64
    if (depth == 8)
        xsrcOffset = xsrc & 0x3;
    else if (depth == 16)
        xsrcOffset = xsrc & 0x1;
    else
        xsrcOffset = 0x0;
#else
    xsrcOffset = 0;
#endif

    //***********************************************************************
    // Check if we've got enough room in the push buffer
    //***********************************************************************
 
    NV_DMAPUSH_CHECKFREE(((ULONG)(19)));  


#if defined(_WIN64)
   //***************************************************************************
   // For unaligned (DWORD) accesses, set the clipping region to the
   // original dest size to prevent drawing any more than requested.
   //***************************************************************************

   if (xsrcOffset)
       {
       //***********************************************************************
       // Set Point and Size methods for clip:
       //    NV019_SET_POINT
       //    NV019_SET_SIZE
       //***********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = ((0 << 16) |  xdst);
        NV_DMA_FIFO = ((ppdev->cyMemory << 16) | width); 
        
        // adjust src x to DWORD boundary
        xsrc -= xsrcOffset;

        // adjust xdst, width - used for Dst x and width from here on
        xdst -= xsrcOffset;
        width += xsrcOffset;

        ppdev->NVClipResetFlag=1;       // Clipping rectangle has been reset
       }
   else
#endif


   //***************************************************************************
   // Reset clipping rectangle to full screen extents if necessary
   // Resetting the clipping rectangle causes delays so we want to do it
   // as little as possible!
   //***************************************************************************

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

    //**************************************************************************
    // Wait for FIFO, then set pattern registers
    //**************************************************************************

    // Determine the color format
    switch (ppdev->iBitmapFormat)
        {
        case BMF_32BPP:
           NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
           break;
        case BMF_16BPP:
           NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A16R5G6B5);
           break;
        default:
        case BMF_8BPP:
           // Must be set to a legal value but hardware ignores it otherwise 
           NV_DMAPUSH_WRITE1(IMAGE_PATTERN_SUBCHANNEL, NV044_SET_COLOR_FORMAT, NV044_SET_COLOR_FORMAT_LE_A8R8G8B8);
           break;
        }            

    //**************************************************************************
    // Set the following methods for context_pattern
    //    NV044_SET_MONOCHROME_FORMAT 
    //    NV044_SET_MONOCHROME_SHAPE  
    //    NV044_SET_PATTERN_SELECT    
    //    NV044_SET_MONOCHROME_COLOR0  
    //    NV044_SET_MONOCHROME_COLOR,  
    //    NV044_SET_MONOCHROME_PATTERN0
    //    NV044_SET_MONOCHROME_PATTERN1
    //**************************************************************************

    NV_DMAPUSH_START(7, IMAGE_PATTERN_SUBCHANNEL, NV044_SET_MONOCHROME_FORMAT);
    NV_DMA_FIFO = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
    NV_DMA_FIFO = NV044_SET_MONOCHROME_SHAPE_64X_1Y;
    NV_DMA_FIFO = NV044_SET_PATTERN_SELECT_MONOCHROME;
    NV_DMA_FIFO = (ULONG)(rbc->iSolidColor | AlphaEnableValue); 
    NV_DMA_FIFO = (ULONG)(rbc->iSolidColor | AlphaEnableValue);
    NV_DMA_FIFO = 0xffffffff;
    NV_DMA_FIFO = 0xffffffff;

    //**************************************************************************
    // Notify the rest of the driver that we changed the pattern
    //**************************************************************************

    ppdev->NVPatternResetFlag=1;

    //**************************************************************************
    // Wait for FIFO, then set IMAGE_FROM_CPU registers
    //**************************************************************************

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop3);

    //**************************************************************************
    // Set the following methods for IMAGE_FROM_CPU
    //     NV061_POINT     
    //     NV061_SIZE_OUT  
    //     NV061_SIZE_IN 
    //**************************************************************************

    NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
    NV_DMA_FIFO = ( (ydst <<16) | (xdst & 0xffff) );
    NV_DMA_FIFO = ( (height <<16) | width );

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

    //**************************************************************************
    // Calculate number of dwords to output per scanline
    // This is the actual amount of data that the NV engine expects per scanline.
    // SizeInWidth is guaranteed to be a multiple of 2 (for 16bpp) and
    // a multiple of 4 (for 8bpp).  So we'll always be outputting DWORDS.
    // (No leftover words or bytes, they get clipped)
    //**************************************************************************

    //**************************************************************************
    // Get ptr to start of SRC
    //**************************************************************************

    if (depth == 8)
        {        
        SizeInWidth = ((width + 3) & 0xfffc);       // We always send 4 pixels at a time
        BytesPerDstScan = width;
        NextScan = (ULONG *)(&(SrcBits[(LONG)((xsrc) + (ysrc*BytesPerSrcScan))]));
        }
    else if (depth == 16)
        {        
        SizeInWidth = ((width + 1) & 0xfffe);       // We always send 2 pixels at a time
        BytesPerDstScan = width * 2;
        NextScan = (ULONG *)(&(SrcBits[(LONG)((xsrc*2) + (ysrc*BytesPerSrcScan))]));
        }
    else
        {        
        SizeInWidth = width;                        // We always send 1 pixel at a time
        BytesPerDstScan = width * 4;
        NextScan = (ULONG *)(&(SrcBits[(LONG)((xsrc*4) + (ysrc*BytesPerSrcScan))]));
        }

    //**************************************************************************
    //     NV061_SIZE_IN 
    //**************************************************************************

    NV_DMA_FIFO = ( (height <<16) | (SizeInWidth)  );

    //**************************************************************************
    // Amount to increment for each scanline
    //**************************************************************************

    ScanInc = (BytesPerSrcScan+3) >> 2;      // Number of dwords to increment

#ifdef _X86_

    //**********************************************************************
    // Use assembly to output as fast as possible
    //**********************************************************************

    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPush_Transfer_MemToScreen_Data(ppdev,BytesPerDstScan,height,NextScan,ScanInc);

#else   // ifdef _x86_

    //**************************************************************************
    // Output one scan at a time
    //**************************************************************************

    for (yscan=0;yscan < height; yscan++)
        {
        ByteCount=BytesPerDstScan;

        ScanPtr = NextScan;

        //**********************************************************************
        // IMAGE_FROM_CPU can output a maximum of 1792 dwords at a time
        // If we've got less than that, blast them out all at once.
        //**********************************************************************

        while (ByteCount >= 1024 * sizeof(ULONG))

            {

            //******************************************************************
            // Check if we've got enough room in the push buffer
            //******************************************************************
 
            NV_DMAPUSH_CHECKFREE(((ULONG)(1025)));  


            NV_DMAPUSH_START(1024, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //******************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //******************************************************************
        
            for (i=0 ; i< 1024; i++)
                NV_DMA_FIFO = ScanPtr[i];    

            ByteCount -= (1024 * sizeof(ULONG));
            ScanPtr += 1024;

            }


        //******************************************************************
        // Check if we've got enough room in the push buffer
        //******************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(ByteCount>>2) + 3));  

        //**********************************************************************
        // Handle remaining dwords
        //**********************************************************************

        if (ByteCount >= 4)
            {
            //******************************************************************
            // Blast out remaining data
            //******************************************************************

            NV_DMAPUSH_START((ByteCount>>2), IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //******************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //******************************************************************
    
            for (i=0;i<(ByteCount>>2);i++)
                NV_DMA_FIFO = ScanPtr[i];    
    
            ScanPtr += (ByteCount>>2);
            ByteCount -= ((ByteCount>>2) * sizeof(ULONG));
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

            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0), TempPixelData1);
            }


        NextScan+=ScanInc;
        
        //******************************************************************
        // Kickoff buffer after each scanline
        //******************************************************************

        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
        UPDATE_LOCAL_DMA_COUNT;


        }

#endif  // ifdef else _X86_

    //**************************************************************************
    // Notify the rest of the driver that we changed the pattern
    //**************************************************************************

    ppdev->NVPatternResetFlag=1;

    }

//******************************************************************************
//
//  Function:   NV4ScreenTo1bppMemBlit
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

//*****************************Public*Table*************************************
// BYTE gajLeftMask[] and BYTE gajRightMask[]
//
// Edge tables for vXferScreenTo1bpp.
//******************************************************************************

BYTE gajLeftMask[]  = { 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
BYTE gajRightMask[] = { 0xff, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe };

BOOL NV4ScreenTo1bppMemBlt(
PDEV*   ppdev,
LONG        c,                  // Count of rectangles, can't be zero
RECTL*      prcl,               // List of destination rectangles, in relative
SURFOBJ* psoSrc,
SURFOBJ* psoDst,
POINTL* pptlSrc,
RECTL*  prclDst,
XLATEOBJ*   pxlo)               // Provides colour-compressions information

    {
    DSURF*  pdsurfSrc;
    ULONG   width,height;
    ULONG*  pulXlate;
    SURFOBJ soTmp;
    ULONG   ulForeColor;
    POINTL  ptlSrc;
    RECTL   rclTmp;
    VOID*   pfnCompute;
    BYTE*   pjDst;
    LONG    cjPelSize;
    LONG    cyToGo;
    BYTE    jLeftMask;
    BYTE    jRightMask;
    BYTE    jNotLeftMask;
    BYTE    jNotRightMask;
    LONG    cjMiddle;
    LONG    lDstDelta;
    LONG    lSrcDelta;
    ULONG   numbytes;
    RECTL*  ptmp_rcl;
    LONG    tmp_c;
    BOOL    status;
    BYTE*   jSrcPtr;
    USHORT* sSrcPtr;
    ULONG*  lSrcPtr;

    
    //**************************************************************************
    // Validate parameters first
    //**************************************************************************

    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(psoDst->iBitmapFormat == BMF_1BPP, "Only 1bpp destinations");
    ASSERTDD(MEM_TO_MEM_BUFFER_SIZE >= (ppdev->cxMemory * ppdev->cjPelSize),
                "Temp buffer has to be larger than widest possible scan");

    //**************************************************************************
    // Assume call succeeded
    //**************************************************************************

    status = TRUE;
    
    //**************************************************************************
    // Get pointer to start of source bitmap
    // Need to be careful about this!  This function gets called
    // when copying a system memory bitmap to VRAM.  However, the SOURCE
    // surfobj can be a standard DIB or it can be a DFB that's been moved
    // from offscreen VRAM to system memory.  If this is the case, then
    // we need to make sure and get the source ptr from the correct place.
    //**************************************************************************

    pdsurfSrc = (DSURF*)psoSrc->dhsurf;

    //**************************************************************************
    // Get a ptr to the clip list and the count of clip rectangles
    //**************************************************************************

    ptmp_rcl = prcl;
    tmp_c    = c;

    //**********************************************************************
    // Destination pel size is either 1,2 or 4
    //**********************************************************************

    cjPelSize = ppdev->cjPelSize;

    
    //**************************************************************************
    // Check that the temporary buffer is large enough
    //**************************************************************************

    do  {

        //**********************************************************************
        // Get height, and the number of bytes required to hold the bitmap
        //
        // Number of bytes in bitmap is  width * height * bytesPerPixel.
        //**********************************************************************

        height = ptmp_rcl->bottom - ptmp_rcl->top;
        width = (((ptmp_rcl->right + 7L) & ~7L) - (ptmp_rcl->left & ~7L));

        numbytes = width * height * cjPelSize;
        
        //**********************************************************************
        // Check if our temporary buffer was large enough.
        // If not, then punt the call back to GDI.
        //**********************************************************************

        if (numbytes >= ppdev->MemToMemBufferSize)
            return(FALSE);            

        ptmp_rcl++;
        tmp_c--;
        } while (tmp_c != 0);
    

    //**************************************************************************
    // When the destination is a 1bpp bitmap, the foreground colour
    // maps to '1', and any other colour maps to '0'.
    //**************************************************************************

    if (ppdev->iBitmapFormat == BMF_8BPP)

        {

        //**********************************************************************
        // When the source is 8bpp or less, we find the foreground colour
        // by searching the translate table for the only '1':
        //**********************************************************************

        pulXlate = pxlo->pulXlate;
        while (*pulXlate != 1)
            pulXlate++;

        //**********************************************************************
        // Get color index (from 0 to 255)
        //**********************************************************************

        ulForeColor = (ULONG)(pulXlate - pxlo->pulXlate);

        }

    else

        {
        ASSERTDD((ppdev->iBitmapFormat == BMF_16BPP) ||
                 (ppdev->iBitmapFormat == BMF_32BPP),
                 "This routine only supports 8, 16 or 32bpp");

        //**********************************************************************
        // When the source has a depth greater than 8bpp, the foreground
        // colour will be the first entry in the translate table we get
        // from calling 'piVector':
        //**********************************************************************

        pulXlate = XLATEOBJ_piVector(pxlo);

        ulForeColor = 0;
        if (pulXlate != NULL)           // This check isn't really needed...
            ulForeColor = pulXlate[0];
        }


    //**************************************************************************
    // We use the temporary buffer to keep a copy of the source
    // rectangle:
    //**************************************************************************

    soTmp.pvScan0 = ppdev->pMemToMemBuffer;

    do  {
        
        //**********************************************************************
        //
        //          1) pptlSrc->x    ( value x1 shown below )
        //
        //          2) prclDst->x    ( value x2 shown below )
        //
        //                  Location of the original unclipped destination rectangle
        //
        //          3) prcl->x       ( value x3 shown below)
        //
        //                  Current clip rectangle with coordinates relative to prclDst
        //
        //
        //   Origin  (Dst bitmap 1bpp)
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
        //
        //            Source Bitmap Color
        //          +--------+--|-----+--------+------->
        //          |           x1
        //          |
        //          |
        //          |
        //          |           ^
        //          v           |
        //                      |
        //                      |
        //          |-- x1 ---->|   x1 = pptlSrc->x
        //
        //**********************************************************************

        //**********************************************************************
        // ptlSrc points to the upper-left corner of the src screen rectangle
        //**********************************************************************

        ptlSrc.x = pptlSrc->x +  prcl->left - prclDst->left;
        ptlSrc.y = pptlSrc->y +  prcl->top  - prclDst->top;

        //**********************************************************************
        // Determine ptr for Destination 1bpp bitmap (we are actually determining
        // a byte address but the x position actually correponds to a bit address)
        //**********************************************************************

        pjDst = (BYTE*) psoDst->pvScan0 + (prcl->top * psoDst->lDelta)
                                        + (prcl->left >> 3);

        //**********************************************************************
        // Here, we determine the number of bytes required per each SRC scanline
        // that we will read into the temporary buffer (from the source screen bitmap)
        // (Right pel is exclusive)
        //
        // Left Pel Address         Right Pel address        
        // ----------------         -----------------        
        // 0->7    maps to 0         0     maps to 0         
        // 8->15   maps to 8         1->8  maps to 8         
        // 16->24  maps to 16        9->16 maps to 16        
        //
        // The following calculation should give us the amount
        // of bytes needed to hold a scanline of pixels, while also being a multiple
        // of 8 pixels.
        //**********************************************************************

        soTmp.lDelta = (((prcl->right + 7L) & ~7L) - (prcl->left & ~7L))
                       * cjPelSize;

        //**********************************************************************
        // cyToGo is the total number of scans we have to do for this rectangle.
        //**********************************************************************

        cyToGo     = prcl->bottom - prcl->top;

        //**********************************************************************
        // rclTmp is the temporary buffer destination rectangle
        // Initialize variables that don't change within the loop:
        // (rclTmp is passed to vGetBits)
        // Left destination coordinate is byte aligned
        //**********************************************************************

        rclTmp.top    = 0;
        rclTmp.left   = prcl->left & 7L;    
        rclTmp.right  = (prcl->right - prcl->left) + rclTmp.left;
        rclTmp.bottom = cyToGo;

        //**********************************************************************
        // Note that we have to be careful with the right mask so that it
        // isn't zero.  A right mask of zero would mean that we'd always be
        // touching one byte past the end of the scan (even though we
        // wouldn't actually be modifying that byte), and we must never
        // access memory past the end of the bitmap (because we can access
        // violate if the bitmap end is exactly page-aligned).
        //
        // That is, the gajRightMask table data has been adjusted to account for
        // rclTmp.right being right-exclusive.  If rcl.right == 8, then
        // we would only access byte 0 , and not byte 1.  Therefore, the right
        // mask = 0xff for rcl.right = 8.
        //**********************************************************************

        jLeftMask     = gajLeftMask[rclTmp.left & 7];
        jRightMask    = gajRightMask[rclTmp.right & 7];

        //**********************************************************************
        // Since the destination is mono, we need to calculate the number of bytes
        // that will hold the corresponding monochrome bits (shift right by 3)
        //
        // |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|  pixel position
        //     |             | |
        //     |             | v
        //     |             |  right
        //     |             |     
        //     v             v
        //  (rclTmp.left)  (rclTmp.right - 1)   This is exclusive 
        //
        //
        //
        // 
        // Shift by 3 (divide by 8) to determine the which byte contains the position        
        // This will give us a 'left byte' address and a 'right' byte address
        //
        //       Left Byte  = (rclTmp.left)      >> 3
        //       Right Byte = (rclTmp.right - 1) >> 3
        //
        //
        //
        // This then leaves us with 3 cases:
        //
        //
        // - Case 1:  Left Byte and Right Byte are the same byte
        // 
        //   |        |        |        |        |        |
        //   | byte 0 | byte 1 | byte 2 | byte 3 | byte 4 | etc....
        //   |        |        |        |        |        |
        //      ^  ^
        //      |  |                    no 'middle' bytes
        //      L  R
        //
        //
        // - Case 2:  Left Byte and Right Byte are next to each other
        // 
        //   |        |        |        |        |        |
        //   | byte 0 | byte 1 | byte 2 | byte 3 | byte 4 | etc....
        //   |        |        |        |        |        |
        //         
        //      ^       ^
        //      |       |               no 'middle' bytes
        //      L       R
        //
        //
        // - Case 3:  Left Byte and Right Byte are more than 1 byte away
        //            from each other
        // 
        //   |        |        |        |        |        |
        //   | byte 0 | byte 1 | byte 2 | byte 3 | byte 4 | etc....
        //   |        |        |        |        |        |
        //         
        //      ^                 ^
        //      |                 |      some 'middle' bytes
        //      L                 R
        // 
        //**********************************************************************

        cjMiddle      = ((rclTmp.right - 1) >> 3) - (rclTmp.left >> 3) - 1;

        //**********************************************************************
        // We are left with the following cases:
        //
        //   right - left   = 0     No middle byte
        //   right - left   = 1     No middle byte, just left and right bytes
        //   right - left   > 1     Middle byte present
        //
        // This can be simplified by subtracting 1 as follows:
        //
        //   right - left - 1  = negative     No middle byte, blt starts and ends in same byte
        //   right - left - 1  = 0            No middle byte, just left and right bytes
        //   right - left - 1  > 0            Middle byte present
        //
        //**********************************************************************

        if (cjMiddle < 0)
            {
            //******************************************************************
            // The blt starts and ends in the same byte:
            //******************************************************************

            jLeftMask &= jRightMask;
            jRightMask = 0;
            cjMiddle   = 0;
            }

        //**********************************************************************
        // Calculate the 'NOT' masks ahead of time
        //**********************************************************************

        jNotLeftMask  = ~jLeftMask;
        jNotRightMask = ~jRightMask;

        //**********************************************************************
        // Mono destination bitmap delta:
        // Delta from the END of one scan to the START on the next scan,
        // accounting for 'left' and 'right' bytes 
        //**********************************************************************

        lDstDelta     = psoDst->lDelta - cjMiddle - 2;
                                
        //**********************************************************************
        // Color source bitmap delta: (temporary buffer)
        // Compute source delta for special cases like when cjMiddle gets bumped
        // up to '0', and to correct aligned cases
        // (Delta from the END of one scan to the START on the next scan)
        //**********************************************************************

        lSrcDelta     = soTmp.lDelta - ((8 * (cjMiddle + 2)) * cjPelSize);

        //**********************************************************************
        // Copy a rectangle of data from SCREEN and read it to the temporary buffer
        // Specify SRC PDSURF,  Dst surfobj (temporary buffer),  dest rectangle,
        // src x and y (pptlsrc)
        //**********************************************************************

        (ppdev->pfnGetScreenBits)(ppdev, pdsurfSrc, &soTmp, &rclTmp, &ptlSrc);
                                
        //**********************************************************************
        // Convert the color bitmap to mono bitmap scanline by scanline
        //**********************************************************************

#ifndef _WIN64
        do  {

           _asm {
                mov     eax,ulForeColor     ; eax = foreground colour
                                            ; ebx = temporary storage
                                            ; ecx = count of middle dst bytes
                                            ; dl  = destination byte accumulator
                                            ; dh  = temporary storage
                mov     esi,soTmp.pvScan0   ; esi = source pointer (from temp buffer)
                mov     edi,pjDst           ; edi = destination pointer

                ;***************************************************************
                ; Figure out the appropriate compute routine:
                ;***************************************************************

                mov     ebx,cjPelSize       ; 1,2 or 4
                mov     pfnCompute,offset Compute_Destination_Byte_From_8bpp
                dec     ebx
                jz      short Do_Left_Byte
                mov     pfnCompute,offset Compute_Destination_Byte_From_16bpp
                dec     ebx
                jz      short Do_Left_Byte
                mov     pfnCompute,offset Compute_Destination_Byte_From_32bpp

                ;***************************************************************
                ; Convert any 'left' bytes present
                ;***************************************************************

            Do_Left_Byte:
                call    pfnCompute          ; DL = 8 bits of monochrome data
                and     dl,jLeftMask        ; Mask off unnecessary bits
                mov     dh,jNotLeftMask     ; Get mask for destination data
                and     dh,[edi]            ; Get bits from dest that we won't touch
                or      dh,dl               ; Get resultant 8 bits of mono data
                mov     [edi],dh            ; Update the destination bitmap
                inc     edi                 ; Advance dst ptr by a byte
                mov     ecx,cjMiddle        ; Check for middle bytes
                dec     ecx                 ; We can skip it if there are none
                jl      short Do_Right_Byte ; Skip middle bytes code

            Do_Middle_Bytes:
                call    pfnCompute          ; DL = 8 bits of monochrome data
                mov     [edi],dl            ; Store the data directly into dst
                inc     edi                 ; Advance dst ptr by a byte
                dec     ecx                 ; Check for remaining middle bytes
                jge     short Do_Middle_Bytes ; Continue processing bitmap

            Do_Right_Byte:
                call    pfnCompute          ; DL = 8 bits of monochrome data
                and     dl,jRightMask       ; Mask off unnecessary bits
                mov     dh,jNotRightMask    ; Get mask for destination data
                and     dh,[edi]            ; Get bits from dest that we won't touch  
                or      dh,dl               ; Get resultant 8 bits of mono data
                mov     [edi],dh            ; Update the destination bitmap
                inc     edi                 ; Advance the dst ptr by a byte

                add     edi,lDstDelta       ; Advance dst bitmap ptr by a scanline
                add     esi,lSrcDelta       ; Advance src (tmp buffer) ptr by a scanline
                dec     cyToGo              ; Continue converting next scanline              
                jnz     short Do_Left_Byte  ; Check if we're done

                jmp     All_Done            ; We're done

                ;***************************************************************
                ; Convert 8bpp color bitmap to 1bpp monochrome
                ;***************************************************************

            Compute_Destination_Byte_From_8bpp:

                mov     bl,[esi]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 0

                mov     bl,[esi+1]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 1

                mov     bl,[esi+2]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 2

                mov     bl,[esi+3]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 3

                mov     bl,[esi+4]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 4

                mov     bl,[esi+5]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 5

                mov     bl,[esi+6]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 6

                mov     bl,[esi+7]
                sub     bl,al
                cmp     bl,1
                adc     dl,dl               ; Bit 7

                add     esi,8               ; Advance the source
                ret

                ;***************************************************************
                ; Convert 16bpp color bitmap to 1bpp monochrome
                ;***************************************************************

            Compute_Destination_Byte_From_16bpp:
                mov     bx,[esi]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 0

                mov     bx,[esi+2]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 1

                mov     bx,[esi+4]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 2

                mov     bx,[esi+6]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 3

                mov     bx,[esi+8]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 4

                mov     bx,[esi+10]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 5

                mov     bx,[esi+12]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 6

                mov     bx,[esi+14]
                sub     bx,ax
                cmp     bx,1
                adc     dl,dl               ; Bit 7

                add     esi,16              ; Advance the source
                ret

                ;***************************************************************
                ; Convert 32bpp color bitmap to 1bpp monochrome
                ;***************************************************************

            Compute_Destination_Byte_From_32bpp:

                mov     ebx,[esi]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 0

                mov     ebx,[esi+4]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 1

                mov     ebx,[esi+8]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 2

                mov     ebx,[esi+12]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 3

                mov     ebx,[esi+16]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 4

                mov     ebx,[esi+20]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 5

                mov     ebx,[esi+24]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 6

                mov     ebx,[esi+28]
                sub     ebx,eax
                cmp     ebx,1
                adc     dl,dl               ; Bit 7

                add     esi,32              ; Advance the source
                ret

All_Done:
                }   

            } while (cyToGo > 0);

#else // _WIN64

#define CONVERT_TO_MONO(Src, Dst)                                         \
    {                                                                     \
    BYTE jVal = 0;                                                        \
    BYTE *DstPtr = Dst;                                                   \
    LONG i, j;                                                            \
                                                                          \
    do  {                                                                 \
        for (i = 0; i < 8; i++)                                           \
            jVal = (jVal << 1) + (*(Src)++ == ulForeColor);               \
        *DstPtr++ = (jVal & jLeftMask) | (*DstPtr & jNotLeftMask);        \
        for (j = 0; j < cjMiddle; j++)  {                                 \
            for (i = 0; i < 8; i++)                                       \
                jVal = (jVal << 1) + (*(Src)++ == ulForeColor);           \
            *DstPtr++ = jVal;                                             \
            }                                                             \
        for (i = 0; i < 8; i++)                                           \
            jVal = (jVal << 1) + (*(Src)++ == ulForeColor);               \
        *DstPtr++ = (jVal & jRightMask) | (*DstPtr & jNotRightMask);      \
        (BYTE *)(Src) += lSrcDelta;                                       \
        DstPtr += lDstDelta;                                              \
        } while (--cyToGo > 0);                                           \
    }

        switch (cjPelSize)
            {
            case 1:
               jSrcPtr = (BYTE *)soTmp.pvScan0;
               CONVERT_TO_MONO(jSrcPtr, pjDst);
               break;
            case 2:
               sSrcPtr = (USHORT *)soTmp.pvScan0;
               CONVERT_TO_MONO(sSrcPtr, pjDst);
               break;
            case 4:
               lSrcPtr = (ULONG *)soTmp.pvScan0;
               CONVERT_TO_MONO(lSrcPtr, pjDst);
               break;
            }

#endif // _WIN64

        prcl++;
        } while (--c != 0);


    return(status);

    }
    


    
//******************************************************************************
//
//  Function:   NV4ScreenToMemBlt
//
//  Routine Description:
//
//      Does a screen-to-memory blt (called when bitmap formats from src to 
//      dest are the same; THERE IS NO COLOR TRANSLATION GOING ON.)
//
//      Ask a DMA transfer to the hardware;
//      The Xfer is from VRAM to the locked MEM2MEM buffer in 
//      PDEV. Then, it transfers the content of the buffer to the actual dest. surface
//      (with normal, cpu transfer).
//      (The SRC bitmap must be in VRAM.)
//
//      Since there is no translation going on, if we could lock the dest. surface
//      and *quickly* create a dma context on-the-fly out of this surface, we 
//      could avoid using a intermediate buffer.  But DMA context allocation is
//      slow so...   
//
//      ***IMPORTANT*** Read comments about assumptions this function makes
//
//
//  Arguments:
//
//      (see header)
//
//  Return Value:
//
//      BOOL :  TRUE if call succeeded, else, FALSE.
//
//  Comments:
//
//      At the time of it's writing, this function makes these asumptions:
//
//          - A surface that has it's bitmap in VRAM can be a GDI managed DIB
//            (no DSURF associated, psoSrc->dhsurf == NULL )
//            if and only if it is the primary surface (what most
//            comments in the whole display driver code call the "SCREEN"), that
//            is, the surface which has PDEV.pjScreen as it's associated bitmap.
//          - Otherwise the surface is device managed and has a DSURF struct
//            associated with it
//          - We use PDEV.cjPelSize to know the number of bytes per pixel
//            EVEN when dealing with a src surface that is NOT the primary
//            one (the one which is actually associated with the PDEV).  So,
//            how come can we say that cjPelSize FROM PDEV is really the format
//            of our src bitmap ?  Because right now, the display memory must
//            contain only bitmaps of the same format.
//
//      (this is the way the whole NV display driver is done at this time)
//  
//          - Like ScreenTo1Bpp, this function assumes that the array of rectangles
//            that constrain (clip) the DST rectangle are already INSIDE this 
//            dst rectangle (so, already clipped by it).
//
//          - Like DMAGetScreenBits, this function makes assumption about the 
//            the value of the static data of the MEM_TO_MEM nv object.
//            It assumes that the SetContextDmaBufferIn() has been set to
//            the VRAM dma context, and that SetContextDmaBufferOut() has
//            been set to the mem2mem DMA buffer
//
//      Instead of using a temp SURFOBJ and passing the call to GetScreenBits
//      like in ScreenTo1Bpp, I have coded a different algo here, where I 
//      directly initiate
//      the transfer. That way, we don't have to use a temp SURFOBJ, and
//      we have more control over the DMA transfer. That is, we 
//      initiate transfer for a certain number of bytes (or pixels) and 
//      wait for a notifier event. When this first part of the transfer is 
//      finished, we initiate the next transfer and start copying from 
//      the mem2mem locked buffer to the final dest surface. This enable
//      parallel tranfer from/to the intermediate mem2mem buffer and almost
//      eleminate the overhead of this inter. buffer.  In fact, only the first
//      fill of this buffer would be overhead in the ideal case. But to really
//      be optimal we would have to determine HOW MANY pixels/bytes
//      to copy at each tranfer ? ( I've done a math model for this xfer case
//      that I tried to optimize after that.  From this, I did the whole 
//      calibration, and optbytesperxfer calcultion algorythm.  This is the 
//      only thing in this function that is not too well documented)
//
//        - Stephane
//
//
//******************************************************************************
                                         

#define  CALIBRATION_ENABLED        // Enable calibration

    
#ifdef  CALIBRATION_ENABLED
    
//****************************************
//    For fast MSBit position localization
//****************************************

BYTE Byte2_MSBitPos_LUT[256] = {

    0,  0,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  
    4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  
    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
    6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7
};
    

//*****************************************
//    For fast int squareroot approximation
//*****************************************

ULONG BitPos2SquRtAprox_LUT[32] = {

           1,           2,           2,           3,           5,           7,
          10,          14,          20,          28,          39,          55,
          78,         111,         157,         222,         314,         443,
         627,         887,        1254,        1774,        2508,        3547,
        5017,        7094,       10033,       14189,       20066,       28378,
       40132,       56756

};

#endif



BOOL NV4ScreenToMemBlt(

    PDEV*       ppdev,              // PDEV associated with the SRC device
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      parcl,              // Array of destination rectangles (inside prclDst)
    SURFOBJ*    psoSrc,             // Source surface
    SURFOBJ*    psoDst,             // Dest surface
    POINTL*     pptlSrc,            // Upper left corner of SRC retcl in SRC surface
    RECTL*      prclDst,            // Rectangle to be modified in the DST surface
    XLATEOBJ*   pxlo)               // Provides color translation info
    
{
    
#if DBG        // only allocate those for convenience (for the next assert) on checked builds

    ULONG*      DstBits;
    ULONG*      SrcBits;
    ULONG*      ScreenBitmap;
    DSURF*      pdsurfDst;
    DSURF*      pdsurfSrc;   

#endif

#ifdef  CALIBRATION_ENABLED
    
    ULONG       tempUlong;
    BYTE        msbitPos;

#endif
    

    LONG            i;
    ULONG           j;
    
    BYTE*           limit;

    ULONG           width,height;

    ULONG           linesToGo;                // Total number of lines left to Xfer

    ULONG           bytes2Xfer;               // Total amount of bytes to Xfer for current rectangle
    
    ULONG           bytesPerLine;             // Width of one line of the current rectangle, in bytes

    ULONG           optBytesPerXfer;          // Optimal bytes per Xfer (calculated based on mathematic model 
                                              // and profiling data, see notes)
    ULONG           linesPerXfer;             // Number of lines to transfer by Xfer (based on bytesPerLine and OptBytesPerXfer)

    ULONG           DstDMAincPerXfer;         // linesPerXfer * bytesPerline
    ULONG           SrcDMAincPerXfer;         // (lSrcPitch+bytesPerLine)*linesPerXfer

    LONG            lSrcPitch;                // Pitch of the source surface
    
    LONG            lDstPitch;                // Pitch of the final dest surface

    LONG            FinalDstJump2NextLineBeg; // DstPitch - byterperline (see where it is used)

    BYTE*           pjSrcBits;                // Pointer to the beginning of the source bits (for the Xfer)
    BYTE*           pjSrcRclBits;             // Pointer to the beginning of the source cliped rectangle bits

    BYTE*           pjFinalDstWrite;          // Pointer to the beg. of the dest. bits (in the final surface)
                                              // (will also be incremented to advance in the destination bits during xfer)
    
    BYTE*           pjDmaRead;                // Pointer in the mem2mem dma buffer that we are going to use in the
                                              // CPU-Xfers to keep track of where we are currently reading 
    
    
    BYTE*           pjDmaRead_lastXfer;       // Var to hold where to read the last CPU-Xfer

    BYTE*           pjDmaRead_start;          // Start of the Mem2Mem buffer

    BYTE*           pjDmaRead_2ndPart;        // Address of 2nd part of Mem2Mem buffer

    POINTL          ptlSrcRegion;             // Left top corner of current clipped region/rectangle in Src surface
                                              // to xfer
    
    ULONG           DstDMAOffset;             // Offset (relative to DMA base)
                                              // of the destination bytes in the dst DMA region
    ULONG           SrcDMAOffset;             // Offset of the source bytes in the src DMA region
    
    ULONG           DstDMA2ndPartOffset;      // Offset of the 2nd part of the Mem2Mem buffer
       
    NvNotification* pNotifier;                // Pointer to MemToMem notifier array

    DECLARE_DMA_FIFO;                         // Local fifo var declare (ULONG nvDmaCount, * nvDmaFifo;)
                                          
    //**************************************************************************
    // Initialize notifier pointer to second notification structure
    //**************************************************************************
    
    pNotifier = (NvNotification *) (&(ppdev->Notifiers->MemToMem[NV039_NOTIFIERS_BUFFER_NOTIFY*sizeof(NvNotification)]));

    //**************************************************************************
    // Get push buffer global information (from pdev) into local variables
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;            // local fifo var = pdev fifo var;
                
    //**************************************************************************
    // Validate parameters first
    //**************************************************************************

    ASSERTDD( ppdev != NULL,
              "NV4ScreenToMemBlt: NULL PDEV supplied !" );

    ASSERTDD( c > 0,
              "NV4ScreenToMemBlt: Can't handle zero rectangles" ); // ok

    ASSERTDD( parcl != NULL,
              "NV4ScreenToMemBlt: NULL destination rectangles supplied !" );
    
    ASSERTDD( psoSrc != NULL,
              "NV4ScreenToMemBlt: NULL source surface supplied !" );

    ASSERTDD( psoDst != NULL,
              "NV4ScreenToMemBlt: NULL destination surface supplied !" );
    
    ASSERTDD( pptlSrc != NULL,
              "NV4ScreenToMemBlt: NULL source upper left point supplied !" );

    ASSERTDD( prclDst != NULL,
              "NV4ScreenToMemBlt: NULL destination rectangle supplied !" );

    ASSERTDD( psoDst->iBitmapFormat == ppdev->iBitmapFormat,
              "NV4ScreenToMemBlt: Src and Dst bitmap formats must be the same" ); // ok

    ASSERTDD( (pxlo==NULL) || (pxlo->flXlate & XO_TRIVIAL),
              "NV4ScreenToMemBlt: Non-trivial color Xlation asked"); // ok

    

#if DBG // we dont put asserts in if DBG, the actual macro definition, ASSERTDD is omited when DBG defined.
    
    DstBits                = (ULONG *) psoDst->pvBits;
    SrcBits                = (ULONG *) psoSrc->pvBits;
    ScreenBitmap           = (ULONG *) ppdev->pjScreen;
    pdsurfDst              = (DSURF *) psoDst->dhsurf;
    pdsurfSrc              = (DSURF *) psoSrc->dhsurf;
    
#endif         
    
    // Assert that this is a VRAM to SYSRAM blit ( we wont check it with an if, this
    // function is solely made for this case anyway ) 

    ASSERTDD( 
              //SRC is VRAM:       
              (
                ( SrcBits == ScreenBitmap) ||              
                ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN) )
              ) &&
              
              //DEST is SYSRAM:
              (
                (DstBits != ScreenBitmap) &&
                ( (pdsurfDst == NULL) || (pdsurfDst->dt == DT_DIB) )
              ),

              "NV4ScreenToMemBlt: Expected a VRAM to SYSRAM blit !"
             
            );

    
    //**************************************************************************
    // Set memory to memory object in spare subchannel
    //**************************************************************************


    NV_DMAPUSH_CHECKFREE(((ULONG)(2)));

    if (ppdev->dDrawSpareSubchannelObject != MY_MEMORY_TO_MEMORY) {
        
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), MY_MEMORY_TO_MEMORY);
        ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;
    }

    //UPDATE_PDEV_DMA_COUNT;      // Update global push buffer count
    //NV4_DmaPushSend(ppdev);     // Send data on thru to the DMA push buffer                      
    //
    // let it be there until the next xfer...
    //

    //**************************************************************************
    // Check if source bitmap (from video mem) is offscreen or onscreen
    // and set pitch and src ptr from the appropriate descriptive structure
    //**************************************************************************

    if ( psoSrc->dhsurf != NULL )               // The DSURF obj is not NULL, so
                                                // this srfc is device-managed
                                                // so it is in VRAM, offscreen
        {
        lSrcPitch  = ( (DSURF *) psoSrc->dhsurf )->LinearStride;
        pjSrcBits  = ( (DSURF *) psoSrc->dhsurf )->LinearPtr;
        }
    else
        { // Onscreen (primary surface, GDI managed)
        
        lSrcPitch   = ppdev->lDelta;
        pjSrcBits   = ppdev->pjScreen;
        }
    
    lDstPitch = psoDst->lDelta ;  // Also set the final dest. surface pitch

    
    // Set the offset (relative to the beginning of the dma mem2mem buffer)
    // of the middle of the mem2mem buffer
    // (See further, we use this as the address of the 2nd part of the mem2mem
    // buffer - one part is being written to, while one part is being read from)
     
    DstDMA2ndPartOffset = (ppdev->MemToMemBufferSize >> 1) ;
    
    // Also get the start and middle adress of the mem2membuffer

    pjDmaRead_start   = ppdev->pMemToMemBuffer;  
    pjDmaRead_2ndPart = (BYTE *)(ppdev->pMemToMemBuffer) + (ppdev->MemToMemBufferSize >> 1);
    
    
    //**************************************************************************
    //  Transfer all rectangles regions in the array
    //**************************************************************************
    
    for(i=0; i < c; i++) // For all rectangles
    {
        // Calculate the number of bytes it takes to hold this bitmap that
        // we want to transfer.

        height = parcl[i].bottom - parcl[i].top;        // parcl[i]
        width  = parcl[i].right - parcl[i].left;
               
        bytesPerLine = width * ppdev->cjPelSize;        // This value is reused later
        bytes2Xfer   = height * bytesPerLine;
        
        // ( We check if the Mem2Mem buffer is large enough later )


        //**********************************************************************
        //
        // Decide the number of lines to Xfer at a time. We must round to a #
        // of lines for each rectangle because MEM2MEM objects Xfer by count 
        // of lines, so we can't stop in the middle
        // of a line, and start a Xfer with the width of the first line to xfer
        // being different from the width of all of the lines following it.
        //
        //**********************************************************************

        // (Use optimal equation with profiling data)
        //
        // I have modified the Newton method using a lut so that 
        // I can have a nearly perfect INT result of the square root
        // with only 2 iterations.
        //
        // to calculate optBytesPerXfer = squareroot(bytes2Xfer*overhead_of_setup*average_speed_of_dma_xfer)
        //
        // We already have squareroot(overhead_of_setup*average_speed_of_dma_xfer) in the global
        // var "globalXferOverheadFactor"
        //

        //optBytesPerXfer = bytesPerLine;      // Force perscanline xfers for the moment (test)

        //optBytesPerXfer = bytes2Xfer;        // Force non-overlapping xfer (test)



        #ifdef  CALIBRATION_ENABLED

        // We are going to compute a fast squareroot (of a 32bit int)
        // (see notes for more detail -stephane)
        
        // - First, find the position of the MSBit in the 32 bits

        if ( (bytes2Xfer & 0xFFFF0000 ) != 0 ) 
        {
            // Search in the MSWord

            tempUlong = (bytes2Xfer & 0xFF000000);

            if ( tempUlong != 0 ) 
            {
                // the MSBit is in this MSByte, let the LUT do the rest of the job
                
                // ( bytes2Xfer >> 24 ) slide the MSByte in the LSByte position
                
                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer >> 24 ] + 24;
            }
            else // the  LSByte of the MSWord contains the bit we are looking for
            {
                // Our number is 0x00??????
                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer >> 16 ] + 16;
            }

        }
        else
        {
            // Search the LSWord
            
            tempUlong = (bytes2Xfer & 0x0000FF00);

            if ( tempUlong != 0 ) 
            {
                // the MSBit is in this MSByte of the LSWord let the LUT do the rest of the job

                // ( bytes2Xfer >> 8 ) slide the MSByte in the LSByte position

                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer >> 8 ] + 8;
            }
            else // the  LSByte of the LSWord contains the bit we are looking for
            {
                // Our number is 0x000000??
                
                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer ];
            }
        
        } // ... else the MSBit is in the LSWord of our 32 bit "bytes2Xfer" number

        //
        // We now have msbitPos, plug it into the other LUT to get a very good
        // initial approximation of the squareroot of bytes2Xfer.
        //

        tempUlong = BitPos2SquRtAprox_LUT[ msbitPos ];
        
        // Do Newton 2 times 
        // We could actually use only 1 iteration and it would be enough for our
        // needs.

        tempUlong = (tempUlong + bytes2Xfer/tempUlong) >> 1 ;
        tempUlong = (tempUlong + bytes2Xfer/tempUlong) >> 1 ;
        
        // We now have our squareroot(bytes2Xfer), all we need is to mul it
        // by our profiled constant

        optBytesPerXfer = tempUlong * ppdev->globalXferOverheadFactor;
                                

        #else  // else if NOT #ifdef  CALIBRATION_ENABLED
        
        optBytesPerXfer = bytesPerLine << 3;   //Force per-8-scanline xfers
        
        //optBytesPerXfer = bytes2Xfer;        // Force non-overlapping xfer (test)

        #endif // ...#ifdef  CALIBRATION_ENABLED        
        

        //**********************************************************************
        // We have our optBytesPerXfer, now convert it in linesPerXfer
        //**********************************************************************

        // But first check if the Mem2Mem buffer is large enough to hold 2 chunks
        // of this size

        // NOTE:
        //
        // ( We try to reduce the Xfer size to 1 scanline (the minimum);
        //   we could try reducing it by half and recheck, but it's not
        //   really worth it, if mem2mem buffer is too small, we are
        //   already in a case that will be handled more slowly )
        
        
        // Check if we are not in the special case where it is more
        // efficient to do the xfer without overlapping ( in that case,
        // we need "bytes2Xfer" bytes in the mem2mem buffer)
        // (Clamp optBytesPerXfer to bytes2Xfer in that case)

        if( optBytesPerXfer > bytes2Xfer )                  
        {
            optBytesPerXfer = bytes2Xfer;  // clamp the size of the chunk
                                           // to total bytes to xfer

            if( optBytesPerXfer > ppdev->MemToMemBufferSize )
            {
                // We must do one xfer, but mem2membuffer is too small.
                // Reduce it to the minimum
    
                optBytesPerXfer = bytesPerLine;
    
                if ( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )                
                    return(FALSE);  // we can't even store 2 scanlines!
            }
        
        } // ...if optxfersize > total2xfer
        
        //  Else, check if we need to clamp to at least 1 scanline
        // (indeed, if optBytesPerXfer calculated is
        //  lower than a scanline, we are going to use
        //  a scanline-based transfer anyway, so we
        //  must check the size of mem2mem buffer VS 
        //  at least a scanline )
        
        else if( optBytesPerXfer < bytesPerLine )
        {                
            optBytesPerXfer = bytesPerLine;
            
            if ( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )                
                return(FALSE);  // we can't even store 2 scanlines!
        }            

        // Else optBytesPerXfer is within boundaries, check if we can
        // store two chunks of that size

        else if( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )
        {
            // Mem2Mem buffer size is too small for the size of 2 Xfer chunks.
            // Try to reduce the xfer chunk to the minimum
        
            optBytesPerXfer = bytesPerLine;
        
            if ( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )                
                return(FALSE);  // we can't even store 2 scanlines!
        }


        // There are many ways to round: clamp to unit by truncating, or clamp to
        // higher unit as soon as there is a rest, etc.  Simplest is truncating
        //
        
        linesPerXfer = ( optBytesPerXfer / bytesPerLine );

       
        // Clamp to higher unit instead of truncate with ( bytesPerXfer / bytesPerLine );
        //
        // Let's limit bytersPerXfer to 33 MB, so we have 7 bits (MSBits) out of
        // 32 to use to do fixed point math. ( 32-7 = 25, 2^25 = 33 MB )
        //
        // [ (bytesPerXfer << 7 ) / ( bytesPerLine << 7 )  + 0x7F (which is 1111111b) ]  >> 7        
                

        

        //**********************************************************************
        //  An object of NV_MEM_TO_MEM class must be attached to 2 DMA contexts.
        //  The src and dst must be given through method writes, and these src
        //  and dst ptr must be in offset RELATIVE to the address of the BASE
        //  (so relative to the beginning) of the DMA region.
        //
        //  So, we are going to:
        //
        //      - First, find the coordinates of the current clip rectangle
        //        in the SRC RECTL. (see below)
        //      - Find the offset in byte of these coordinates, relative to the
        //        beginning of the source bits, and find the absolute address 
        //        of these coordinates by adding the base of the source bits.        
        //      - Substract the base of the DMA region from this ptr to obtain
        //        an offset relative to the base.
        //
        //**********************************************************************

        
        //**********************************************************************
        //      - First, find the coordinates of the current clip rectangle
        //        in the SRC RECTL. (see below)
        //**********************************************************************

        
        //**********************************************************************
        //
        //          1) pptlSrc->x    ( value x1 shown below )
        //
        //          2) prclDst->x    ( value x2 shown below )
        //
        //                  Location of the original unclipped destination rectangle
        //
        //          3) parcl[i]       ( value x3 shown below)
        //
        //                  Current clip rectangle with coordinates relative to origin
        //                                                                  of Dst
        //
        //   Origin  (Destination bitmap )
        //  +--------------------------------------------------------------->
        //  |
        //  |
        //  |           x2 = prclDst->left
        //  |           +-----------------------------------------------
        //  |           |                              ^                |-Original
        //  |           |                              | dySrc          |  Unclipped
        //  |           |                              |                |   Destination
        //  |           |         x3 = parcl[i].left   v                |    Rectangle
        //  |           |         +------------        -                |
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
        //
        //           Origin (Source bitmap)
        //          +--------+--|-----+--------+------->
        //          |           x1
        //          |
        //          |
        //          |
        //          |           ^
        //          v           |
        //          |           |
        //          |           |
        //          |-- x1 ---->|   x1 = pptlSrc->x
        //          |           +-----------------------------------------------
        //          |           |                             ^                 |-Source
        //          |           |                             | dySrc           | Rectangle 
        //          |           |                             |                 |   
        //          |           |         x? = x1 + dxSrc     v                 |    
        //          |           |         +------------       -                 |
        //          |           |         |            |                        |
        //          |           |<-dxSrc->|            |                        |
        //          |           |         |            |                        |
        //          |           |         |            |-Current                |
        //          |           |         |            |  Clip                  |
        //          |           |          ------------    Rectangle            |
        //          |           |                        ( but in SRC surface)  |
        //          |           |                                               |
        //          |           |                                               |
        //          |            -----------------------------------------------
        //
        //**********************************************************************


        ptlSrcRegion.x  = pptlSrc->x + (parcl[i].left - prclDst->left);
        ptlSrcRegion.y  = pptlSrc->y + (parcl[i].top  - prclDst->top);

        //**************************************************************************
        //      - Find the offset in byte of these coordinates, relative to the
        //        beginning of the source bits, and find the absolute address 
        //        of these coordinates by adding the base of the source bits.        
        //
        // (Calculate the pointer to the upper-left corner of Source clipped region)
        //**************************************************************************
        
        pjSrcRclBits    =     pjSrcBits                             // base of Src Bits
                              + ( ptlSrcRegion.y  * lSrcPitch ) 
                              + ptlSrcRegion.x * ppdev->cjPelSize ;   
                                // CONVERT_TO_BYTES(x,pdev) == (x) * pdev->cjPelSize        

        
        // We don't have to calculate the dst address for the DMA transfer: 
        // we have it, it is the dst
        // of the mem2mem DMA buffer, but we have to calculate the address in
        // the final destination surface, for the cpu transfer

        //**************************************************************************
        // Calculate the pointer to the upper-left corner of Destination clipped region
        // (THE FINAL DESTINATION, not the MEM2MEM buffer)
        //**************************************************************************

        pjFinalDstWrite =  (BYTE*) psoDst->pvScan0 
                          + ( parcl[i].top  * lDstPitch )
                          + ( parcl[i].left * ppdev->cjPelSize );
                            
                            // Remember, we don't do any format conversion, and the
                            // src is in screen mem, and the format descriptor of
                            // the src mem is the pdev. That's why we can use
                            // pdev->cjPelSize to
                            // calculate an "x" offset from pixel to bytes.
        
        //**************************************************************************
        //
        //  Now, we need to setup the first Xfer, wait for it to end, and enter
        //  the main Xfer loop, were we kick a Xfer for the next chunk, and Xfer
        //  the last chunk from mem2mem buffer to final sys mem destination.
        //  Finally, we are going to Xfer the remaining data 
        //  ( total_lines_to_xfer % optimal_lines_by_xfer )
        //  
        //**************************************************************************


        //**************************************************************************
        // Calculate the offsets of the transfer src and dst from the beginning of 
        // their respective DMA region base
        //**************************************************************************

        DstDMAOffset = 0;         // Init the current DMAoffset where to write
        
        
                                  // Init current source DMAoffset where to read

        SrcDMAOffset = (ULONG)( pjSrcRclBits - ppdev->pjFrameBufbase ); 
                                  // We are assuming that base of VRAM DMA context
                                  // is == pjFrameBufbase, that is, that the DMAoffset of pjFrameBufbase
                                  // is always 0. That's why we use ppdev->pjFrameBufbase for
                                  // base of DMA region
        
        //**********************************************************************
        //
        // Precalculate the num of bytes to add to the SrcDmaOffset for each 
        // transfer and the address of the second chunk in the DstDMA 
        // (the mem2mem buffer) 
        //
        // ( We could use linesPerXfer * bytesPerLine,
        // and that would also be the number of bytes to add to the
        // DstDmaOffset if we would be user a mem2mem buffer the size of the
        // whole transfer. In other words, not using the strict minimum to be
        // able to do overlapping, that is 2 chunks in the buffer (one to which
        // DMA write, the other to which CPU read, and then, the opposite...)
        // 
        // Instead we use ppdev->MemToMemBufferSize/2, because that value never
        // changes (for any xfer size), so we can put that calculation out of
        // the rectangle loop. (for all rectangles)
        //
        //
        // Also init some variables before going on to the Xfers
        //
        // Reminder: the pitch include the linelenght (bytesperline)!
        //**********************************************************************
        
        
        //DstDMA2ndPartOffset   =  (ppdev->MemToMemBufferSize >> 1);  does not change
        SrcDMAincPerXfer      = linesPerXfer * lSrcPitch;

        
        // Calculate the jump to get to the next line of the 
        // final destination rectangle (in the final Dst surface)
        // AFTER having transfered "bytesPerLine" (in other words,
        // pitch is the num of bytes to the jump AT THE SAME POSITION
        // in the bitmap but one line lower; what we want is the jump
        // value to jump from the end of a line TO THE BEGINNING
        // of the next line )
        
        FinalDstJump2NextLineBeg = lDstPitch - bytesPerLine;

        
        //pjDmaRead_start        = ppdev->pMemToMemBuffer;  // These var will not change for different rectangles

        //pjDmaRead_2ndPart      = ppdev->pMemToMemBuffer + (ppdev->MemToMemBufferSize >> 1);

        
        pjDmaRead                = pjDmaRead_start;  // Init the reading ptr for the second
                                                     // transfer "pipe" (the CPU-Xfers)
        
        linesToGo                = height;           // Init the number of lines left to transfer


        
        
        //**********************************************************************
        //                      Setup the first transfer
        //
        //  NOTE: Somebody looking at this code (further below) might think:
        //        why doing this first xfer outside of the main xfer loop ? 
        //        "Everything looks the same inside the loop"  This is not
        //        exactly right: the loop is done so when it rolls we wait 
        //        for the last notifier being setup, just before resetting 
        //        this notifier and kicking off the new transfer. If we would
        //        remove that special-casing of the first xfer, we would be 
        //        waiting inside the start of the xfer loop forever because
        //        no xfer has been setup yet...
        //
        //
        //**********************************************************************

        //**********************************************************************
        // Set methods for memory_to_memory dma blit
        //   NV039_OFFSET_IN                                            
        //   NV039_OFFSET_OUT                                           
        //   NV039_PITCH_IN                                             
        //   NV039_PITCH_OUT                                            
        //   NV039_LINE_LENGTH_IN                                       
        //   NV039_LINE_COUNT                                           
        //   NV039_FORMAT                                               
        //   NV039_BUFFER_NOTIFY                                        
        //**********************************************************************

        //**************************************************************************
        // Check if we've got enough room in the push buffer
        //**************************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(9)));

        //**************************************************************************
        // Set notifier to 'BUSY' value (use second notification structure)
        //**************************************************************************

        pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;

        NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
        NV_DMA_FIFO = SrcDMAOffset;                             // Offset In
        NV_DMA_FIFO = DstDMAOffset;                             // Offset Out
        NV_DMA_FIFO = lSrcPitch;                                // Pitch In
        NV_DMA_FIFO = bytesPerLine;                               // Pitch Out == bytesPerLine because
                                                                  // we dont want any memory "hole" between
                                                                  // two contiguous lines in the mem2mem buffer

        NV_DMA_FIFO = bytesPerLine;                             // Line Length in bytes
        NV_DMA_FIFO = linesPerXfer;                             // Line Count (num to copy)
        NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | 
                      NV_MTMF_FORMAT_OUTPUT_INC_1;              // Format
        NV_DMA_FIFO = 0;                                        // Set Buffer Notify and kickoff

        //**************************************************************************
        // Update global push buffer count
        //**************************************************************************

        UPDATE_PDEV_DMA_COUNT;

        //**************************************************************************
        // Send data on thru to the DMA push buffer
        //**************************************************************************

        NV4_DmaPushSend(ppdev);

        //**************************************************************************
        // Here we incremented the offsets before the Xfer completes, so we can
        // cover a bit the processing (make these calculations during the time
        // the first Xfer execute)
        //**************************************************************************

        linesToGo       -= linesPerXfer;          // Susbtract the lines that have been done.
        SrcDMAOffset    += SrcDMAincPerXfer;      // Advance in the source DMA region
        DstDMAOffset     = DstDMA2ndPartOffset;   // Advance in the dest. DMA region
                                                  // (the mem2mem buffer) so we use the 
                                                  // 2nd part the next time.

        //
        // REMINDER: ANY CODE FOLLOWING THIS MUST CHECK THE NOTIFIER
        // IF IT WANTS TO BE SURE THAT THE MEM2MEM BUFFER DATA IS VALID
        //
        
        //***[ Make sure there are still enough lines to Xfer before starting  *****
        //*                                                  another DMA Xfer ]    *
        
        if ( linesToGo < linesPerXfer )  // if there are not "linesPerXfer" lines to Xfer
        {
            // Now what is left is a CPU Xfer of "linesPerXfer" plus at most
            // one DMA+CPU Xfers of size " height % (modulo) linesPerXfer " ...
            //
            // We save the value to which we must set the position of pjDmaRead
            // for the very last CPU read ( of size height % linesPerXfer)
    
            pjDmaRead_lastXfer = pjDmaRead_2ndPart;
        

        } // ...if there are less than linesPerXfer left to Xfer
        
        else
        {
            //  else, enter the main Xfer loop.
        
            //**************************************************************************
            //                Main transfer loop with overlapping Xfers
            //
            //  Note that we are starting to setup the next DMA
            //  xfer even before checking if the last one is finished. We only do so
            //  at the last minute, before resetting the notifier, and kicking the next
            //  DMA Xfer for maximum overlapping of processing
            //
            //
            //**************************************************************************                        
        
            for(;;)   // (for all Xfers that can be done in linesPerXfer chunks):
                      // - this is a 2 part loop with 
                      //   2 breakout cases, so that's why we use a for(;;))
                      //   (2 cases because we are doing either one or the other part of
                      //   the mem2mem buffer)
            {
                //
                // START another DMA-Xfer:    remember, we use ( 2*bytesperxfer ) bytes
                //                            in our Mem2Mem buffer. (So we alternate
                //                            using it's two parts )
                //
                //                            We started Xfering to the first part
                //                            (before entering the loop, and at the end
                //                            of this for(;;) loop),
                //                            (the NV hw is writting to the first part)
                //                            so we are going to read from that first part
                //                            while we kick another Xfer in the second
                //                            part.
                //
                // (Note that the first time we enter this loop, we will wait without
                //  overlapping: this is normal, we need to wait for the **FIRST** xfer
                //  to be done, because in the other part of the 2part buffer, there
                //  isn't any data available yet for our CPU.)
                //          

                //---[ So now: ]-------------------------
                //                                      -
                // DstDMAOffset == DstDMA2ndPartOffset; -
                //    pjDMAread == pjDmaRead_start;     -
                //                                      -
                //---------------------------------------

                //
                // START another DMA-Xfer
                //
    
    
                NV_DMAPUSH_CHECKFREE(((ULONG)(9)));  // Check if we've got enough room in the push buffer  

                NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
                NV_DMA_FIFO = SrcDMAOffset;                             // Offset In
                NV_DMA_FIFO = DstDMAOffset;                             // Offset Out
                NV_DMA_FIFO = lSrcPitch;                                // Pitch In
                NV_DMA_FIFO = bytesPerLine;                               // Pitch Out == bytesPerLine because
                                                                          // we dont want any memory "hole" between
                                                                          // two contiguous lines in the mem2mem buffer

                NV_DMA_FIFO = bytesPerLine;                             // Line Length in bytes
                NV_DMA_FIFO = linesPerXfer;                             // Line Count (num to copy)
                NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | 
                              NV_MTMF_FORMAT_OUTPUT_INC_1;              // Format
                NV_DMA_FIFO = 0;                                        // Set Buffer Notify and kickoff
    
                //**************************************************************************
                // Wait for the last DMA-Xfer kicked to be completed (use second notification structure)
                // ( if it is not yet completed )
                //**************************************************************************
    
                while ( pNotifier->status != 0 );
    
    
                //**************************************************************************
                // Set notifier to 'BUSY' value (use second notification structure)
                //**************************************************************************
    
                pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;
    
                UPDATE_PDEV_DMA_COUNT;      // Update global push buffer count
                NV4_DmaPushSend(ppdev);     // Send data on thru to the DMA push buffer
    
    
                linesToGo    -= linesPerXfer;          // Susbtract the lines that have been done.
                SrcDMAOffset += SrcDMAincPerXfer;      // Advance in the source DMA region (VRAM)
                DstDMAOffset  = 0;                     // Go back to the first part of the mem2mem
                                                       // DMA buffer in the next DMA Xfer.
    
                //**************************************************************************
                //  In the meantime, CPU-Xfer what has been DMA-Xfered.
                //
                //  Note that here we have two chases that can go on: if DMA-Xfer is faster,
                //  the while(!notified) active-wait will always be true by the time we
                //  get here. We could, to be fancier, check after a certain number of bytes
                //  CPU-Xfered, if the notification has been trigered (using the profiling
                //  data, we can do it), but it's not really worth it. Indeed, if the DMA-Xfer
                //  is faster, and we launch more than 1 per CPU-Xfered-chunk, (let's say 1.2
                //  on average), at the end, all the DMA-Xfers will be over, yes, and the 
                //  hardware will be idle yes, but we will still have to finish here, in this
                //  function, with our CPU transfer, and THAT is going to be the limiting speed.
                //  (And since the driver is not re-entrant, the idle hardware won't be used
                //  anyway during this time)
                //  Repeat a similar reasoning for the other chase condition, that is,
                //  if the CPU-Xfers are faster than the DMA-Xfers.
                //**************************************************************************
    
                //
                // NOTE: The next loop can be optimized...
                //
    
                // Xfer all lines
                //
                for(j=0; j < linesPerXfer; j++)
                {            
                    // Xfer a whole line
                    //
                    for(limit = pjDmaRead + bytesPerLine; 
                        pjDmaRead < limit; 
                        pjDmaRead++, pjFinalDstWrite++ )
                    {
                        *pjFinalDstWrite = *pjDmaRead ;
                    }
    
                    pjFinalDstWrite += FinalDstJump2NextLineBeg; // Switch to beginning of the
                                                                 // next line in the dest surface
    
                } // ...for all bytes in chunk to cpu-xfer
    
                // Set our pjDmaRead ptr at the beginning of the second part of our mem2mem buffer
                // (For the next CPU-Xfer)

                pjDmaRead = pjDmaRead_2ndPart;

                //
                // ... CPU Xfer done.                    
                
                
                //***[ Make sure there are still lines to Xfer before starting         *****
                //*                                                  another DMA Xfer ]    *

                if ( linesToGo < linesPerXfer )  // if there are no more chunk to Xfer
                {
                    
                    // Now what is left is a CPU Xfer of "linesPerXfer" plus at most
                    // one DMA+CPU Xfers of size " height % (modulo) linesPerXfer " ...
                    //
                    // We save the value to which we must set the position of pjDmaRead
                    // for the very last CPU read ( of size height % linesPerXfer)
    
                    pjDmaRead_lastXfer = pjDmaRead_start;
                    
                    break;          // break out of the Xfer loop (for(;;))
                }
                //*                                                                        *
                //**************************************************************************
                                    
                //
                // START another DMA-Xfer:    Remember, we use ( 2*bytesPerXfer ) bytes
                //                            in our Mem2Mem buffer. (So we alternate
                //                            using it's two parts )
                //
                //                            The last Xfer kicked used the 2nd part.
                //                            Next Xfer is going to use the first part
                //                            again.
        
                //---[ So now: ]-------------------------
                //                                      -
                // DstDMAOffset == 0;                   -
                //    pjDMAread == pjDmaRead_2ndPart;   -
                //                                      -
                //---------------------------------------

        
                NV_DMAPUSH_CHECKFREE(((ULONG)(9)));  // Check if we've got enough room in the push buffer  
        
                NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
                NV_DMA_FIFO = SrcDMAOffset;                         // Offset In
                NV_DMA_FIFO = DstDMAOffset;                         // Offset Out
                NV_DMA_FIFO = lSrcPitch;                            // Pitch In
                NV_DMA_FIFO = bytesPerLine;                         // Pitch Out == bytesPerLine because
                                                                      // we dont want any memory "hole" between
                                                                      // two contiguous lines in the mem2mem buffer
                NV_DMA_FIFO = bytesPerLine;                         // Line Length in bytes
                NV_DMA_FIFO = linesPerXfer;                         // Line Count (num to copy)
                NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | 
                              NV_MTMF_FORMAT_OUTPUT_INC_1;          // Format
                NV_DMA_FIFO = 0;                                    // Set Buffer Notify and kickoff
        
        
                //**************************************************************************
                // Wait for the last DMA-Xfer kicked to be completed (use second notification structure)
                // ( if it is not yet completed )
                //**************************************************************************
        
                while ( pNotifier->status != 0 );
        
        
                //**************************************************************************
                // Set notifier to 'BUSY' value (use second notification structure)
                //**************************************************************************
        
                pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;
        
                UPDATE_PDEV_DMA_COUNT;      // Update global push buffer count
                NV4_DmaPushSend(ppdev);     // Send data on thru to the DMA push buffer
        
        
                linesToGo       -= linesPerXfer;          // Susbtract the lines that have been done.
                SrcDMAOffset    += SrcDMAincPerXfer;      // Advance in the source DMA region (VRAM)                    
                DstDMAOffset     = DstDMA2ndPartOffset;   // Advance in the dest. DMA region
                                                          // (the mem2mem buffer) so we use the 
                                                          // 2nd part the next time.
        
                //**************************************************************************
                //  In the meantime, CPU-Xfer what has been DMA-Xfered in the 2nd part
                //**************************************************************************
                
                // Xfer all lines
                //
                for(j=0; j < linesPerXfer; j++)
                {            
                    // Xfer a whole line
                    //
                    for(limit = pjDmaRead + bytesPerLine; 
                        pjDmaRead < limit; 
                        pjDmaRead++, pjFinalDstWrite++ )
                    {
                        *pjFinalDstWrite = *pjDmaRead ;
                    }
    
                    pjFinalDstWrite += FinalDstJump2NextLineBeg; // Switch to beginning of the
                                                                 // next line in the dest surface
    
                } // ...for all bytes in chunk to cpu-xfer
                
                // Set our pjDmaRead ptr at the beginning of the first part of our mem2mem buffer
                
                pjDmaRead = pjDmaRead_start;                 // Reset the read ptr at the 
                                                             // first part (beginning) of the
                                                             // MEM2MEM buffer
                // ... CPU Xfer Done.
                

                //***[ Make sure there are still lines to Xfer before starting         *****
                //*                                                  another DMA Xfer ]    *
                
                if ( linesToGo < linesPerXfer )  // if there are no more lines to Xfer
                {
                    
                    // Now what is left is a CPU Xfer of "linesPerXfer" plus at most
                    // one DMA+CPU Xfers of size " height % (modulo) linesPerXfer " ...
                    //
                    // We save the value to which we must set the position of pjDmaRead
                    // for the very last CPU read ( of size height % linesPerXfer)
    
                    pjDmaRead_lastXfer = pjDmaRead_2ndPart;

                    break;          // break out of the Xfer loop (for(;;))

                } // ...if there are less than "linesPerXfer" left to Xfer
                
                //*                                                                        *
                //**************************************************************************

    
            } // ...for(;;)  ( while(linesToGo >= linesPerXfer )  )
              // 
              // (this is a special cased 2 part loop)

        
        } //  ...else enter the main Xfer loop (if there are at least linesPerXfer left to Xfer)

        //**************************************************************************
        //
        //                Last transfer: Time to execute the last transfer
        //
        //  There is at most one DMA transfer left to execute, and at most 2 mem2mem
        //  chunks to transfer (the last DMA that is about to be completed, and the 
        //  one that we are about to start, if there is one)
        //
        //**************************************************************************

        if ( linesToGo == 0 ) // only 1 CPU-Xfer left to execute
                              // (that means that totalLinesToXfer was a multiple
                              // of linesPerXfer, => total % linesPerXfer == 0
        {
            //**************************************************************************
            // Wait for the last DMA to be completed (use second notification structure)
            //**************************************************************************

            while ( pNotifier->status != 0 );

            // Xfer the last "linesPerXfer" lines
            //
            for(j=0; j < linesPerXfer; j++)
            {            
                // Xfer a whole line
                //
                for(limit = pjDmaRead + bytesPerLine; 
                    pjDmaRead < limit; 
                    pjDmaRead++, pjFinalDstWrite++ )
                {
                    *pjFinalDstWrite = *pjDmaRead ;
                }

                pjFinalDstWrite += FinalDstJump2NextLineBeg; // Switch to beginning of the
                                                             // next line in the dest surface

            } // ...for all bytes in chunk to cpu-xfer

            //****************************************
            // And this rectangle is done, at last...
            //****************************************

        } // ...if ( linesToGo == 0 )

        else // else, setup one last DMA Xfer
        {
            // This last DMA-Xfer is different in that the line count is
            // linesToGo (the rest of the lines, which is < than our linesPerXfer calculated )
            //

            NV_DMAPUSH_CHECKFREE(((ULONG)(9)));  // Check if we've got enough room in the push buffer  

            NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
            NV_DMA_FIFO = SrcDMAOffset;                    // Offset In
            NV_DMA_FIFO = DstDMAOffset;                    // Offset Out
            NV_DMA_FIFO = lSrcPitch;                       // Pitch In
            NV_DMA_FIFO = bytesPerLine;                      // Pitch Out == bytesPerLine because
                                                             // we dont want any memory "hole" between
                                                             // two contiguous lines in the mem2mem buffer
            NV_DMA_FIFO = bytesPerLine;                    // Line Length in bytes
            NV_DMA_FIFO = linesToGo;                       // Line Count (num to copy)
            NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1; // Format
            NV_DMA_FIFO = 0;                               // Set Buffer Notify and kickoff

            //**************************************************************************
            // Wait for the last DMA-Xfer kicked to be completed (use second notification structure)
            // ( if it is not yet completed )
            //**************************************************************************

            while ( pNotifier->status != 0 );


            //**************************************************************************
            // Set notifier to 'BUSY' value (use second notification structure)
            //**************************************************************************

            pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;

            UPDATE_PDEV_DMA_COUNT;      // Update global push buffer count
            NV4_DmaPushSend(ppdev);     // Send data on thru to the DMA push buffer                      

            
            //**************************************************************************
            // Now that this last DMA-Xfer is running, transfer the last "linesPerXfer"
            // long chunk. (this is our last but one CPU-Xfer)
            //                
            // We might think of one complication here: we do not know where we broke in
            // our main for(;;) loop.  So we do not know to which part of the mem2mem 
            // buffer we must read our last cpu xfer. (In other words, where to put the 
            // pjDmaRead ptr. This is solved in the loop by saving the next value to which 
            // it must be set (this is to avoid doing an additionnal "IF" here, to know 
            // were we broke in the for(;;) )
            //
            //**************************************************************************

            // Xfer the "linesPerXfer" lines
            //
            for(j=0; j < linesPerXfer; j++)
            {            
                // Xfer a whole line
                //
                for(limit = pjDmaRead + bytesPerLine; 
                    pjDmaRead < limit; 
                    pjDmaRead++, pjFinalDstWrite++ )
                {
                    *pjFinalDstWrite = *pjDmaRead ;
                }

                pjFinalDstWrite += FinalDstJump2NextLineBeg; // Switch to beginning of the
                                                             // next line in the dest surface
            } // ...for all bytes in chunk to cpu-xfer

            // Set our pjDmaRead ptr at the beginning of the ? part of our mem2mem buffer
            // (the right part for our last cpu xfer; this was selected in the for(;;)

            pjDmaRead = pjDmaRead_lastXfer;

            
            //**************************************************************************
            // Wait for the LAST DMA-Xfer to be completed
            // ( if it is not yet completed ) and CPU-Xfer these remainder lines
            //**************************************************************************

            while ( pNotifier->status != 0 );


            for(j=0; j < linesToGo; j++)
            {            
                // Xfer a whole line
                //
                for(limit = pjDmaRead + bytesPerLine; 
                    pjDmaRead < limit; 
                    pjDmaRead++, pjFinalDstWrite++ )
                {
                    *pjFinalDstWrite = *pjDmaRead ;
                }

                pjFinalDstWrite += FinalDstJump2NextLineBeg; // Switch to beginning of the
                                                             // next line in the dest surface

            } // ...for all lines left

            //****************************************
            // And this rectangle is done, at last...
            //****************************************

        } // ...else ( linesToGo != 0 )  ( one last DMA xfer to setup )

    
    } // ...for(i=0; i < c; i++) // For all rectangles
    
    return TRUE;
}

// ...End of NV4ScreenToMem()
    


//******************************************************************************
//  ------------------------------------------------------------------
//            BEGIN NV4ScreenToMem*to*bppBlt FUNCTIONS
//  ------------------------------------------------------------------
//******************************************************************************

//******************************************************************************
//
//  NV4ScreenToMem*to*bppBlt functions and macro definitions for these
//  ------------------------------------------------------------------
//
//
//  NOTE: These macros are NOT meant to be used alone. These uses local variables
//        of NV4ScreenToMem*to*bppBlt functions, and the order in which they are
//        used is obviously important. ( For example, ScreenToMemXbpp_Common must
//        be executed before "height, width, ..." are usable; See ScreenToMem4bpp
//        for an example)
//
//  After the macros, these functions are defined:
//
//      NV4ScreenToMem32to4bppBlt
//      NV4ScreenToMem32to8bppBlt
//      NV4ScreenToMem16to4bppBlt
//      NV4ScreenToMem16to8bppBlt
//
//  These functions do a ScreenToMem Blt (like NV4ScreenToMemBlt) but with
//  color translation between non indexed (high/true color) to indexed color
//  formats (4bpp and 8bpp).  (It seems that this read from frame buffer happens
//  every time a window with very small icons pop, like the advance display applet
//  with the small NV logo, or when going to display properties and listing all
//  available patterns and bmp for the desktop (the icons there) )
//
//  The reason why these different functions are aliased instead of having one
//  NV4ScreenToMemWithXlate function is that we don't want to have a big "IF"
//  in the high level code (bitblt) and then, recheck again the same variables
//  but this time to isolate the cases of the "big IF". (This is similar to
//  the PushXfer??to?? functions...
//
//
//
//      ***IMPORTANT*** Read comments about assumptions these function makes
//                      (further below, after macro descriptions - these 
//                      are the same comments then in NV4ScreenToMemBlt)
//
//
//
//  ---------------------------------------------------------------------------          
//  Macro:                  [ GiveOptBytesPerXfer( optBytesPerXfer ) ]
//
//  Macro Description:      optBytesPerXfer will = optimal num. of bytes/xfer
//
//  ---------------------------------------------------------------------------          
//  Macro:                  [ ScreenToMemXbpp_Common ]
//
//  Macro Description:      Insert the code common to all core ScreenToMemXbpp
//                          functions in the beginning of the for(all rectangles)
//                          loop.
//  ---------------------------------------------------------------------------          
//  Macro:                  [ ScreenToMemXbpp_Common2 ]
//
//  Macro Description:      Insert the code common to all core ScreenToMemXbpp
//                          functions.  This contain the core of the transfer
//                          code. The 3th parameter is the name of the macro
//                          to use for all CPUXFER. This macro must accept 
//                          2 params, as described in CpuXferTo4bpp for example
//  ---------------------------------------------------------------------------          
//  Macro:                  [ PushSendDMAXfer_forXlines(linesPerXfer) ]
//
//  Macro Description:      Insert the code to push and send a DMA Xfer for
//                          "linesPerXfer" lines. (Does not update the variables
//                          it uses like "SrcDMAOffset", etc.)
//  ---------------------------------------------------------------------------          
//
//  Macro:                  [ PushWaitSendDMAXfer_forXlines(linesPerXfer) ]
//
//  Macro Description:      Insert the code to push, wait for the last DMA Xfer
//                          to end, reset the notifier and send a DMA Xfer for
//                          "linesPerXfer" lines. (Does not update the variables
//                          it uses like "SrcDMAOffset", etc.)
//  ---------------------------------------------------------------------------          
//
//  Macro:                  [ CpuXferTo4bpp(pxxDmaRead,linesPerXfer) ]
//                          
//  Macro Description:      Core of a CPU Xfer to convert a part of the MemToMem
//                          DMA buffer to 4bpp and write it in the final surface;
//                          This macro is used when we have at least 2 bytes per
//                          scanline (the case when there is a width of 1 or 2       
//                          pixels inside THE SAME byte must be special cased)
//                          
//                          pxxDmaRead is ULONG* if the source is 32bpp, USHORT*
//                          if the source is 16bpp, etc.
//
//                          (Source must be converted from non-indexed,
//                          to indexed)
//
//  ---------------------------------------------------------------------------          
//  Macro:                  [ CpuXferTo4bpp_1ByteWidth(pxxDmaRead,linesPerXfer) ]
//                          
//  Macro Description:      See CpuXferTo4bpp...
//                          
//  ---------------------------------------------------------------------------          
//  Macro:                  [ CpuXferTo8bpp(pxxDmaRead,linesPerXfer) ]
//                          
//  Macro Description:      Core of a CPU transfer when destination is 8bpp 
//                          (and source must be converted from non-indexed,
//                          to indexed)
//  ---------------------------------------------------------------------------          
//  Macro:                  [ ScreenToMem4bpp_Core(pxxDmaRead, cast) ]
//                          
//  Macro Description:      Core of a ScreenToMem with non-indexed to indexed
//                          translation, when dest. is 4bpp (and scanline is
//                          at least 2 bytes wide) (See CpuXferTo4bpp)
//
//                          The "cast" parameter MUST MATCH the pointer type:
//                          "cast" is (ULONG*) when "pxxDmaRead" is a (ULONG*)
//  ---------------------------------------------------------------------------          
//  Macro:                  [ ScreenToMem4bpp_Core_1ByteWidth(pxxDmaRead, cast) ]
//                          
//  Macro Description:      Core of a ScreenToMem with non-indexed to indexed
//                          translation, when dest. is 4bpp (and scanline is
//                          1 byte wide) (See CpuXferTo4bpp_1ByteWidth)
//
//                          The "cast" parameter MUST MATCH the pointer type:
//                          "cast" is (ULONG*) when "pxxDmaRead" is a (ULONG*)
//  ---------------------------------------------------------------------------          
//  Macro:                  [ ScreenToMem4bpp(pxxDmaRead,cast) ]
//                          
//  Macro Description:      Upper level macro to do a StoMblt with non-indexed
//                          to indexed translation, when dest. is 4bpp.
//                          Basically, it has the "for all rectangles" loop,
//                          calculate the "middleWidth" of a scanline and 
//                          exec the appropriate ScreenToMem4bpp_Core macro.
//
//                          The "cast" parameter MUST MATCH the pointer type:
//                          "cast" is (ULONG*) when "pxxDmaRead" is a (ULONG*)
//  ---------------------------------------------------------------------------          
//  Macro:                  [ ScreenToMem8bpp(pxxDmaRead,cast) ]
//                          
//  Macro Description:      Similar to ScreenToMem4bpp...
//                          
//                          
//  ---------------------------------------------------------------------------          
//
//  Here is a figure that shows a "macro-dependancy" example:
//
//                         ScreenToMem32to4bppBlt
//                         |
//                         |
//                         v
//                         ScreenToMem4bpp
//                           |
//                           |
//                           v
//                          .ScreenToMemXbpp_Common
//                          .       |
//                          .       v
//                          .       GiveOptBytesPerXfer
//                          .ScreenToMem4bpp_Core    
//                            |
//                            |
//                            v
//                            ScreenToMemXbpp_Common2
//                              |
//                              |
//                              v
//                             .PushSendDMAXfer_forXlines
//                             .PushWaitSendDMAXfer_forXlines
//                             .CpuXferTo4bpp
//
//  
//  Comments:
//
//      At the time of it's writing, this function makes these asumptions:
//
//          - A surface that has it's bitmap in VRAM can be a GDI managed DIB
//            (no DSURF associated, psoSrc->dhsurf == NULL )
//            if and only if it is the primary surface (what most
//            comments in the whole display driver code call the "SCREEN"), that
//            is, the surface which has PDEV.pjScreen as it's associated bitmap.
//          - Otherwise the surface is device managed and has a DSURF struct
//            associated with it
//          - We use PDEV.cjPelSize to know the number of bytes per pixel
//            EVEN when dealing with a src surface that is NOT the primary
//            one (the one which is actually associated with the PDEV).  So,
//            how come can we say that cjPelSize FROM PDEV is really the format
//            of our src bitmap ?  Because right now, the display memory must
//            contain only bitmaps of the same format.
//
//      (this is the way the whole NV display driver is done at this time)
//  
//          - Like ScreenTo1Bpp, this function assumes that the array of rectangles
//            that constrain (clip) the DST rectangle are already INSIDE this 
//            dst rectangle (so, already clipped by it).
//
//          - Like DMAGetScreenBits, this function makes assumption about the 
//            the value of the static data of the MEM_TO_MEM nv object.
//            It assumes that the SetContextDmaBufferIn() has been set to
//            the VRAM dma context, and that SetContextDmaBufferOut() has
//            been set to the mem2mem DMA buffer
//
//
//******************************************************************************

//******************************************************************************
// To get an approximation of the optimal "BytesPerXfer":
//
// We use the same ppdev->globalXferOverheadFactor than in ScreenToMem.  This is
// wrong, because this factor is based on calibration data that is calculated 
// in DrvEnableSurface based on the code of screentomem.  We have specified there,
// that any changes in the screentomem code must me made to the xfer loop being
// calibrated, otherwise, the calibration data is faulty.  So, ideally, calib
// data would have to give one factor for each different cpu/dma xfer algo.
// But since this calibration doesn't seem to affect benchmarks that much 
// after all (except for blitting lots of small icons ?), we use the same factor
//******************************************************************************


#ifdef  CALIBRATION_ENABLED
                                                                                                                  
#define GiveOptBytesPerXfer( optBytesPerXfer )                                                                    \
{                                                                                                                 \
        /* We are going to compute a fast squareroot (of a 32bit int)   */                                        \
        /* (see notes for more detail -stephane)                        */                                        \
                                                                                                                  \
        /* - First, find the position of the MSBit in the 32 bits       */                                        \
                                                                                                                  \
        if ( (bytes2Xfer & 0xFFFF0000 ) != 0 )                                                                    \
        {                                                                                                         \
            /* Search in the MSWord */                                                                            \
                                                                                                                  \
            tempUlong = (bytes2Xfer & 0xFF000000);                                                                \
                                                                                                                  \
            if ( tempUlong != 0 )                                                                                 \
            {                                                                                                     \
                /* the MSBit is in this MSByte, let the LUT do the rest of the job  */                            \
                                                                                                                  \
                /* ( bytes2Xfer >> 24 ) slide the MSByte in the LSByte position     */                            \
                                                                                                                  \
                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer >> 24 ] + 24;                                           \
            }                                                                                                     \
            else /* the  LSByte of the MSWord contains the bit we are looking for   */                            \
            {                                                                                                     \
                /* Our number is 0x00??????  */                                                                   \
                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer >> 16 ] + 16;                                           \
            }                                                                                                     \
                                                                                                                  \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            /* Search the LSWord  */                                                                              \
                                                                                                                  \
            tempUlong = (bytes2Xfer & 0x0000FF00);                                                                \
                                                                                                                  \
            if ( tempUlong != 0 )                                                                                 \
            {                                                                                                     \
                /* the MSBit is in this MSByte of the LSWord let the LUT do the rest of the job  */               \
                                                                                                                  \
                /* ( bytes2Xfer >> 8 ) slide the MSByte in the LSByte position  */                                \
                                                                                                                  \
                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer >> 8 ] + 8;                                             \
            }                                                                                                     \
            else /* the  LSByte of the LSWord contains the bit we are looking for  */                             \
            {                                                                                                     \
                /* Our number is 0x000000??  */                                                                   \
                                                                                                                  \
                msbitPos = Byte2_MSBitPos_LUT[ bytes2Xfer ];                                                      \
            }                                                                                                     \
                                                                                                                  \
        } /* ... else the MSBit is in the LSWord of our 32 bit "bytes2Xfer" number  */                            \
                                                                                                                  \
        /*                                                                        */                              \
        /* We now have msbitPos, plug it into the other LUT to get a very good    */                              \
        /* initial approximation of the squareroot of bytes2Xfer.                 */                              \
        /*                                                                        */                              \
                                                                                                                  \
        tempUlong = BitPos2SquRtAprox_LUT[ msbitPos ];                                                            \
                                                                                                                  \
        /* Do Newton 2 times                                                      */                              \
        /* We could actually use only 1 iteration and it would be enough for our  */                              \
        /* needs.                                                                 */                              \
                                                                                                                  \
        tempUlong = (tempUlong + bytes2Xfer/tempUlong) >> 1 ;                                                     \
        tempUlong = (tempUlong + bytes2Xfer/tempUlong) >> 1 ;                                                     \
                                                                                                                  \
        /* We now have our squareroot(bytes2Xfer), all we need is to mul it       */                              \
        /* by our profiled constant                                               */                              \
                                                                                                                  \
        optBytesPerXfer = tempUlong * ppdev->globalXferOverheadFactor;                                            \
                                                                                                                  \
                                                                                                                  \
}                                                                                                                 \
/******************************* End of GiveOptBytesPerXfer( optBytesPerXfer ) ***********************************/

#else  /* else if NOT #ifdef  CALIBRATION_ENABLED */
                                                                                                                  
            
#define GiveOptBytesPerXfer( optBytesPerXfer )                                                                    \
{                                                                                                                 \
        optBytesPerXfer = bytesPerLine << 3;   /* Force per-8-scanline xfers */                                   \
                                                                                                                  \
        /* optBytesPerXfer = bytes2Xfer;  */   /* Force non-overlapping xfer (test) */                            \
}                                                                                                                 \
/******************************* End of GiveOptBytesPerXfer( optBytesPerXfer ) ***********************************/


#endif /* ...#ifdef  CALIBRATION_ENABLED         */






#define ScreenToMemXbpp_Common                                                                                    \
{                                                                                                                 \
                                                                                                                  \
                                                                                                                  \
        /* Calculate the number of bytes it takes to hold this bitmap that   */                                   \
        /* we want to transfer.                                              */                                   \
        /*                                                                   */                                   \
                                                                                                                  \
        height = parcl[i].bottom - parcl[i].top;        /* parcl[i]  */                                           \
        width  = parcl[i].right - parcl[i].left;                                                                  \
                                                                                                                  \
        bytesPerLine = width * ppdev->cjPelSize;        /* This value is reused later */                          \
        bytes2Xfer   = height * bytesPerLine;                                                                     \
                                                                                                                  \
                                                                                                                  \
        /* ( We check if the Mem2Mem buffer is large enough later )   */                                          \
                                                                                                                  \
                                                                                                                  \
        /************************************************************************/                                \
        /*                                                                      */                                \
        /* Decide the number of lines to Xfer at a time. We must round to a #   */                                \
        /* of lines for each rectangle because MEM2MEM objects Xfer by count    */                                \
        /* of lines, so we can't stop in the middle                             */                                \
        /* of a line, and start a Xfer with the width of the first line to xfer */                                \
        /* being different from the width of all of the lines following it.     */                                \
        /*                                                                      */                                \
        /************************************************************************/                                \
                                                                                                                  \
        /* (Use optimal equation with profiling data)                                                        */   \
        /*                                                                                                   */   \
        /* I have modified the Newton method using a lut so that                                             */   \
        /* I can have a nearly perfect INT result of the square root                                         */   \
        /* with only 2 iterations.                                                                           */   \
        /*                                                                                                   */   \
        /* to calculate optBytesPerXfer = squareroot(bytes2Xfer*overhead_of_setup*average_speed_of_dma_xfer) */   \
        /*                                                                                                   */   \
        /* We already have squareroot(overhead_of_setup*average_speed_of_dma_xfer) in the global             */   \
        /* var "globalXferOverheadFactor"                                                                    */   \
        /*                                                                                                   */   \
                                                                                                                  \
        /* optBytesPerXfer = bytesPerLine; */     /* Force perscanline xfers for the moment (test)   */           \
                                                                                                                  \
        /* optBytesPerXfer = bytes2Xfer;   */     /* Force non-overlapping xfer (test)               */           \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
         GiveOptBytesPerXfer( optBytesPerXfer );                                                                  \
                                                                                                                  \
        /************************************************************************/                                \
        /* We have our optBytesPerXfer, now convert it in linesPerXfer          */                                \
        /************************************************************************/                                \
                                                                                                                  \
        /* But first check if the Mem2Mem buffer is large enough to hold 2 chunks */                              \
        /* of this size                                                           */                              \
                                                                                                                  \
        /* NOTE:                                                                  */                              \
        /*                                                                        */                              \
        /* ( We try to reduce the Xfer size to 1 scanline (the minimum);          */                              \
        /*   we could try reducing it by half and recheck, but it's not           */                              \
        /*   really worth it, if mem2mem buffer is too small, we are              */                              \
        /*   already in a case that will be handled more slowly )                 */                              \
                                                                                                                  \
                                                                                                                  \
        /* Check if we are not in the special case where it is more               */                              \
        /* efficient to do the xfer without overlapping ( in that case,           */                              \
        /* we need "bytes2Xfer" bytes in the mem2mem buffer)                      */                              \
        /* (Clamp optBytesPerXfer to bytes2Xfer in that case)                     */                              \
                                                                                                                  \
        if( optBytesPerXfer > bytes2Xfer )                                                                        \
        {                                                                                                         \
            optBytesPerXfer = bytes2Xfer;  /* clamp the size of the chunk */                                      \
                                           /* to total bytes to xfer      */                                      \
                                                                                                                  \
            if( optBytesPerXfer > ppdev->MemToMemBufferSize )                                                     \
            {                                                                                                     \
                /* We must do one xfer, but mem2membuffer is too small. */                                        \
                /* Reduce it to the minimum                             */                                        \
                                                                                                                  \
                optBytesPerXfer = bytesPerLine;                                                                   \
                                                                                                                  \
                if ( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )                                       \
                    return(FALSE);  /* we can't even store 2 scanlines! */                                        \
            }                                                                                                     \
                                                                                                                  \
        } /* ...if optxfersize > total2xfer  */                                                                   \
                                                                                                                  \
        /*  Else, check if we need to clamp to at least 1 scanline   */                                           \
        /* (indeed, if optBytesPerXfer calculated is                 */                                           \
        /*  lower than a scanline, we are going to use               */                                           \
        /*  a scanline-based transfer anyway, so we                  */                                           \
        /*  must check the size of mem2mem buffer VS                 */                                           \
        /*  at least a scanline )                                    */                                           \
                                                                                                                  \
        else if( optBytesPerXfer < bytesPerLine )                                                                 \
        {                                                                                                         \
            optBytesPerXfer = bytesPerLine;                                                                       \
                                                                                                                  \
            if ( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )                                           \
                return(FALSE);  /* we can't even store 2 scanlines!   */                                          \
        }                                                                                                         \
                                                                                                                  \
        /* Else optBytesPerXfer is within boundaries, check if we can */                                          \
        /* store two chunks of that size                              */                                          \
                                                                                                                  \
        else if( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )                                           \
        {                                                                                                         \
            /* Mem2Mem buffer size is too small for the size of 2 Xfer chunks. */                                 \
            /* Try to reduce the xfer chunk to the minimum                     */                                 \
                                                                                                                  \
            optBytesPerXfer = bytesPerLine;                                                                       \
                                                                                                                  \
            if ( (optBytesPerXfer << 1)  >  ppdev->MemToMemBufferSize )                                           \
                return(FALSE);  /* we can't even store 2 scanlines!     */                                        \
        }                                                                                                         \
                                                                                                                  \
                                                                                                                  \
        /* There are many ways to round: clamp to unit by truncating, or clamp to */                              \
        /* higher unit as soon as there is a rest, etc.  Simplest is truncating   */                              \
        /*                                                                        */                              \
                                                                                                                  \
        linesPerXfer = ( optBytesPerXfer / bytesPerLine );                                                        \
                                                                                                                  \
                                                                                                                  \
        /* Clamp to higher unit instead of truncate with ( bytesPerXfer / bytesPerLine );       */                \
        /*                                                                                      */                \
        /* Let's limit bytersPerXfer to 33 MB, so we have 7 bits (MSBits) out of                */                \
        /* 32 to use to do fixed point math. ( 32-7 = 25, 2^25 = 33 MB )                        */                \
        /*                                                                                      */                \
        /* [ (bytesPerXfer << 7 ) / ( bytesPerLine << 7 )  + 0x7F (which is 1111111b) ]  >> 7   */                \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
        /************************************************************************/                                \
        /*  An object of NV_MEM_TO_MEM class must be attached to 2 DMA contexts.*/                                \
        /*  The src and dst must be given through method writes, and these src  */                                \
        /*  and dst ptr must be in offset RELATIVE to the address of the BASE   */                                \
        /*  (so relative to the beginning) of the DMA region.                   */                                \
        /*                                                                      */                                \
        /*  So, we are going to:                                                */                                \
        /*                                                                      */                                \
        /*      - First, find the coordinates of the current clip rectangle     */                                \
        /*        in the SRC RECTL. (see below)                                 */                                \
        /*      - Find the offset in byte of these coordinates, relative to the */                                \
        /*        beginning of the source bits, and find the absolute address   */                                \
        /*        of these coordinates by adding the base of the source bits.   */                                \
        /*      - Substract the base of the DMA region from this ptr to obtain  */                                \
        /*        an offset relative to the base.                               */                                \
        /*                                                                      */                                \
        /************************************************************************/                                \
                                                                                                                  \
                                                                                                                  \
        /************************************************************************/                                \
        /*      - First, find the coordinates of the current clip rectangle     */                                \
        /*        in the SRC RECTL. (see below)                                 */                                \
        /************************************************************************/                                \
                                                                                                                  \
                                                                                                                  \
        /************************************************************************/                                \
        /*                                                                      */                                \
        /*          1) pptlSrc->x    ( value x1 shown below )                   */                                \
        /*                                                                      */                                \
        /*          2) prclDst->x    ( value x2 shown below )                   */                                \
        /*                                                                      */                                \
        /*             Location of the original unclipped destination rectangle */                                \
        /*                                                                      */                                \
        /*          3) parcl[i]       ( value x3 shown below)                   */                                \
        /*                                                                      */                                \
        /*            Current clip rectangle with coordinates relative to origin*/                                \
        /*                                                                of Dst*/                                \
        /*                                                                      */                                \
        /*   Origin  (Destination bitmap )                                      */                                \
        /*  +--------------------------------------------------------------->   */                                \
        /*  |                                                                   */                                \
        /*  |                                                                   */                                \
        /*  |           x2 = prclDst->left                                      */                                \
        /*  |           +-----------------------------------------------                  */                      \
        /*  |           |                              ^                |-Original        */                      \
        /*  |           |                              | dySrc          |  Unclipped      */                      \
        /*  |           |                              |                |   Destination   */                      \
        /*  |           |         x3 = parcl[i].left   v                |    Rectangle    */                      \
        /*  |           |         +------------        -                |                 */                      \
        /*  |           |         |            |                        |       */                                \
        /*  |           |<-dxSrc->|            |                        |       */                                \
        /*  |           |         |            |                        |       */                                \
        /*  |           |         |            |-Current                |       */                                \
        /*  |           |         |            |  Clip                  |       */                                \
        /*  |           |          ------------    Rectangle            |       */                                \
        /*  |           |                                               |       */                                \
        /*  |           |                                               |       */                                \
        /*  |           |                                               |       */                                \
        /*  |            -----------------------------------------------        */                                \
        /*  |                                                                   */                                \
        /*  v                                                                   */                                \
        /*                                                                      */                                \
        /*                                                                      */                                \
        /*           Origin (Source bitmap)                                     */                                \
        /*          +--------+--|-----+--------+------->                        */                                \
        /*          |           x1                                              */                                \
        /*          |                                                           */                                \
        /*          |                                                           */                                \
        /*          |                                                           */                                \
        /*          |           ^                                               */                                \
        /*          v           |                                               */                                \
        /*          |           |                                               */                                \
        /*          |           |                                                           */                    \
        /*          |-- x1 ---->|   x1 = pptlSrc->x                                         */                    \
        /*          |           +-----------------------------------------------            */                    \
        /*          |           |                             ^                 |-Source    */                    \
        /*          |           |                             | dySrc           | Rectangle */                    \
        /*          |           |                             |                 |           */                    \
        /*          |           |         x? = x1 + dxSrc     v                 |           */                    \
        /*          |           |         +------------       -                 |           */                    \
        /*          |           |         |            |                        |           */                    \
        /*          |           |<-dxSrc->|            |                        |           */                    \
        /*          |           |         |            |                        |           */                    \
        /*          |           |         |            |-Current                |           */                    \
        /*          |           |         |            |  Clip                  |           */                    \
        /*          |           |          ------------    Rectangle            |           */                    \
        /*          |           |                        ( but in SRC surface)  |           */                    \
        /*          |           |                                               |           */                    \
        /*          |           |                                               |           */                    \
        /*          |            -----------------------------------------------            */                    \
        /*                                                                                  */                    \
        /***********************************************************************            */                    \
                                                                                                                  \
                                                                                                                  \
        ptlSrcRegion.x  = pptlSrc->x + (parcl[i].left - prclDst->left);                                           \
        ptlSrcRegion.y  = pptlSrc->y + (parcl[i].top  - prclDst->top);                                            \
                                                                                                                  \
        /****************************************************************************/                            \
        /*      - Find the offset in byte of these coordinates, relative to the     */                            \
        /*        beginning of the source bits, and find the absolute address       */                            \
        /*        of these coordinates by adding the base of the source bits.       */                            \
        /*                                                                          */                            \
        /* (Calculate the pointer to the upper-left corner of Source clipped region)*/                            \
        /****************************************************************************/                            \
                                                                                                                  \
        pjSrcRclBits    =     pjSrcBits                             /* base of Src Bits */                        \
                              + ( ptlSrcRegion.y  * lSrcPitch )                                                   \
                              + ptlSrcRegion.x * ppdev->cjPelSize ;                                               \
                                /* CONVERT_TO_BYTES(x,pdev) == (x) * pdev->cjPelSize    */                        \
                                                                                                                  \
                                                                                                                  \
        /* We don't have to calculate the dst address for the DMA transfer:    */                                 \
        /* we have it, it is the dst                                           */                                 \
        /* of the mem2mem DMA buffer, but we have to calculate the address in  */                                 \
        /* the final destination surface, for the cpu transfer                 */                                 \
                                                                                                                  \
                                                                                                                  \
        /****************************************************************************/                            \
        /*                                                                          */                            \
        /*  Now, we need to setup the first Xfer, wait for it to end, and enter     */                            \
        /*  the main Xfer loop, were we kick a Xfer for the next chunk, and Xfer    */                            \
        /*  the last chunk from mem2mem buffer to final sys mem destination.        */                            \
        /*  Finally, we are going to Xfer the remaining data                        */                            \
        /*  ( total_lines_to_xfer % optimal_lines_by_xfer )                         */                            \
        /*                                                                          */                            \
        /****************************************************************************/                            \
                                                                                                                  \
                                                                                                                  \
        /****************************************************************************/                            \
        /* Calculate the offsets of the transfer src and dst from the beginning of  */                            \
        /* their respective DMA region base                                         */                            \
        /****************************************************************************/                            \
                                                                                                                  \
        DstDMAOffset = 0;         /* Init the current DMAoffset where to write    */                              \
                                                                                                                  \
                                                                                                                  \
                                  /* Init current source DMAoffset where to read  */                              \
                                                                                                                  \
        SrcDMAOffset = (ULONG)(pjSrcRclBits - ppdev->pjFrameBufbase);                                                   \
                                  /* We are assuming that base of VRAM DMA context             */                 \
                                  /* is == pjFrameBufbase, that is, that the DMAoffset of pjFrameBufbase   */                 \
                                  /* is always 0. That's why we use ppdev->pjFrameBufbase for        */                 \
                                  /* base of DMA region                                        */                 \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
        /************************************************************************/                                \
        /*                                                                      */                                \
        /* Precalculate the num of bytes to add to the SrcDmaOffset for each    */                                \
        /* transfer and the address of the second chunk in the DstDMA           */                                \
        /* (the mem2mem buffer)                                                 */                                \
        /*                                                                      */                                \
        /* ( We could use linesPerXfer * bytesPerLine,                          */                                \
        /* and that would also be the number of bytes to add to the             */                                \
        /* DstDmaOffset if we would be user a mem2mem buffer the size of the    */                                \
        /* whole transfer. In other words, not using the strict minimum to be   */                                \
        /* able to do overlapping, that is 2 chunks in the buffer (one to which */                                \
        /* DMA write, the other to which CPU read, and then, the opposite...)   */                                \
        /*                                                                      */                                \
        /* Instead we use ppdev->MemToMemBufferSize/2, because that value never */                                \
        /* changes (for any xfer size), so we can put that calculation out of   */                                \
        /* the rectangle loop. (for all rectangles)                             */                                \
        /*                                                                      */                                \
        /*                                                                      */                                \
        /* Also init some variables before going on to the Xfers                */                                \
        /*                                                                      */                                \
        /* Reminder: the pitch include the linelenght (bytesperline)!           */                                \
        /************************************************************************/                                \
                                                                                                                  \
                                                                                                                  \
        SrcDMAincPerXfer         = linesPerXfer * lSrcPitch;                                                      \
                                                                                                                  \
                                                                                                                  \
        linesToGo                = height;      /* Init the number of lines left to transfer  */                  \
                                                                                                                  \
                                                                                                                  \
}                                                                                                                 \
/*************************************** End of ScreenToMemXbpp_Common *******************************************/






#define ScreenToMemXbpp_Common2(pxxDmaRead, cast, CpuXferMACRONAME)                                               \
{                                                                                                                 \
                                                                                                                  \
        pxxDmaRead               = cast(pulDmaRead_start); /* Init the reading ptr for the second  */             \
                                                           /* transfer "pipe" (the CPU-Xfers)      */             \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
        /***********************************************************************************/                     \
        /*                      Setup the first transfer                                   */                     \
        /*                                                                                 */                     \
        /*  NOTE: Somebody looking at this code (further below) might think:               */                     \
        /*        why doing this first xfer outside of the main xfer loop ?                */                     \
        /*        "Everything looks the same inside the loop"  This is not                 */                     \
        /*        exactly right: the loop is done so when it rolls we wait                 */                     \
        /*        for the last notifier being setup, just before resetting                 */                     \
        /*        this notifier and kicking off the new transfer. If we would              */                     \
        /*        remove that special-casing of the first xfer, we would be                */                     \
        /*        waiting inside the start of the xfer loop forever because                */                     \
        /*        no xfer has been setup yet...                                            */                     \
        /*                                                                                 */                     \
        /*                                                                                 */                     \
        /***********************************************************************************/                     \
                                                                                                                  \
        PushSendDMAXfer_forXlines(linesPerXfer);                                                                  \
                                                                                                                  \
        /***********************************************************************************/                     \
        /* Here we incremented the offsets before the Xfer completes, so we can            */                     \
        /* cover a bit the processing (make these calculations during the time             */                     \
        /* the first Xfer execute)                                                         */                     \
        /***********************************************************************************/                     \
                                                                                                                  \
        linesToGo       -= linesPerXfer;          /* Susbtract the lines that have been done. */                  \
        SrcDMAOffset    += SrcDMAincPerXfer;      /* Advance in the source DMA region         */                  \
        DstDMAOffset     = DstDMA2ndPartOffset;   /* Advance in the dest. DMA region          */                  \
                                                  /* (the mem2mem buffer) so we use the       */                  \
                                                  /* 2nd part the next time.                  */                  \
                                                                                                                  \
        /*                                                                  */                                    \
        /* REMINDER: ANY CODE FOLLOWING THIS MUST CHECK THE NOTIFIER        */                                    \
        /* IF IT WANTS TO BE SURE THAT THE MEM2MEM BUFFER DATA IS VALID     */                                    \
        /*                                                                  */                                    \
                                                                                                                  \
        /***[ Make sure there are still enough lines to Xfer before starting  *****/                              \
        /*                                                  another DMA Xfer ]    */                              \
                                                                                                                  \
        if ( linesToGo < linesPerXfer )  /* if there are not "linesPerXfer" lines to Xfer     */                  \
        {                                                                                                         \
            /* Now what is left is a CPU Xfer of "linesPerXfer" plus at most      */                              \
            /* one DMA+CPU Xfers of size " height % (modulo) linesPerXfer " ...   */                              \
            /*                                                                    */                              \
            /* We save the value to which we must set the position of pulDmaRead  */                              \
            /* for the very last CPU read ( of size height % linesPerXfer)        */                              \
                                                                                                                  \
            pulDmaRead_lastXfer = pulDmaRead_2ndPart;                                                             \
                                                                                                                  \
                                                                                                                  \
        } /* ...if there are less than linesPerXfer left to Xfer                  */                              \
                                                                                                                  \
        else                                                                                                      \
        {                                                                                                         \
            /*  else, enter the main Xfer loop. */                                                                \
                                                                                                                  \
            /*******************************************************************************/                     \
            /*                Main transfer loop with overlapping Xfers                    */                     \
            /*                                                                             */                     \
            /*  Note that we are starting to setup the next DMA                            */                     \
            /*  xfer even before checking if the last one is finished. We only do so       */                     \
            /*  at the last minute, before resetting the notifier, and kicking the next    */                     \
            /*  DMA Xfer for maximum overlapping of processing                             */                     \
            /*                                                                             */                     \
            /*                                                                             */                     \
            /*******************************************************************************/                     \
                                                                                                                  \
            for(;;)   /* (for all Xfers that can be done in linesPerXfer chunks):         */                      \
                      /* - this is a 2 part loop with                                     */                      \
                      /*   2 breakout cases, so that's why we use a for(;;))              */                      \
                      /*   (2 cases because we are doing either one or the other part of  */                      \
                      /*   the mem2mem buffer)                                            */                      \
            {                                                                                                     \
                /*                                                                          */                    \
                /* START another DMA-Xfer:    remember, we use ( 2*bytesperxfer ) bytes     */                    \
                /*                            in our Mem2Mem buffer. (So we alternate       */                    \
                /*                            using it's two parts )                        */                    \
                /*                                                                          */                    \
                /*                            We started Xfering to the first part          */                    \
                /*                            (before entering the loop, and at the end     */                    \
                /*                            of this for(;;) loop),                        */                    \
                /*                            (the NV hw is writting to the first part)     */                    \
                /*                            so we are going to read from that first part  */                    \
                /*                            while we kick another Xfer in the second      */                    \
                /*                            part.                                         */                    \
                /*                                                                          */                    \
                /* (Note that the first time we enter this loop, we will wait without       */                    \
                /*  overlapping: this is normal, we need to wait for the **FIRST** xfer     */                    \
                /*  to be done, because in the other part of the 2part buffer, there        */                    \
                /*  isn't any data available yet for our CPU.)                              */                    \
                /*                                                                          */                    \
                                                                                                                  \
                /*---[ So now: ]------------------------*/                                                        \
                /*                                      */                                                        \
                /* DstDMAOffset == DstDMA2ndPartOffset; */                                                        \
                /*   pulDmaRead == pulDmaRead_start;    */                                                        \
                /*                                      */                                                        \
                /*--------------------------------------*/                                                        \
                                                                                                                  \
                /*                                                                                    */          \
                /* START another DMA-Xfer (but this time with wait for notification before resetting  */          \
                /*                         the notifier and kicking it)                               */          \
                /*                                                                                    */          \
                                                                                                                  \
                                                                                                                  \
                PushWaitSendDMAXfer_forXlines(linesPerXfer);                                                      \
                                                                                                                  \
                linesToGo    -= linesPerXfer;          /* Susbtract the lines that have been done.  */            \
                SrcDMAOffset += SrcDMAincPerXfer;      /* Advance in the source DMA region (VRAM)   */            \
                DstDMAOffset  = 0;                     /* Go back to the first part of the mem2mem  */            \
                                                       /* DMA buffer in the next DMA Xfer.          */            \
                                                                                                                  \
                /***************************************************************************/                     \
                /*  In the meantime, CPU-Xfer what has been DMA-Xfered.                    */                     \
                /*                                                                         */                     \
                /*  Note that here we have two chases that can go on: if DMA-Xfer is faster,     */               \
                /*  the while(!notified) active-wait will always be true by the time we          */               \
                /*  get here. We could, to be fancier, check after a certain number of bytes     */               \
                /*  CPU-Xfered, if the notification has been trigered (using the profiling       */               \
                /*  data, we can do it), but it's not really worth it. Indeed, if the DMA-Xfer   */               \
                /*  is faster, and we launch more than 1 per CPU-Xfered-chunk, (let's say 1.2    */               \
                /*  on average), at the end, all the DMA-Xfers will be over, yes, and the        */               \
                /*  hardware will be idle yes, but we will still have to finish here, in this    */               \
                /*  function, with our CPU transfer, and THAT is going to be the limiting speed. */               \
                /*  (And since the driver is not re-entrant, the idle hardware won't be used     */               \
                /*  anyway during this time)                                                     */               \
                /*  Repeat a similar reasoning for the other chase condition, that is,     */                     \
                /*  if the CPU-Xfers are faster than the DMA-Xfers.                        */                     \
                /***************************************************************************/                     \
                                                                                                                  \
                CpuXferMACRONAME(pxxDmaRead,linesPerXfer);                                                        \
                                                                                                                  \
                /* Set our pjDmaRead ptr at the beginning of the second part of our mem2mem buffer */             \
                /* (For the next CPU-Xfer)                                                         */             \
                                                                                                                  \
                pxxDmaRead = cast(pulDmaRead_2ndPart);                                                            \
                                                                                                                  \
                /*                              */                                                                \
                /* ... CPU Xfer done.           */                                                                \
                                                                                                                  \
                                                                                                                  \
                /***[ Make sure there are still lines to Xfer before starting         *****/                      \
                /*                                                  another DMA Xfer ]    */                      \
                                                                                                                  \
                if ( linesToGo < linesPerXfer )  /* if there are no more chunk to Xfer    */                      \
                {                                                                                                 \
                                                                                                                  \
                    /* Now what is left is a CPU Xfer of "linesPerXfer" plus at most      */                      \
                    /* one DMA+CPU Xfers of size " height % (modulo) linesPerXfer " ...   */                      \
                    /*                                                                    */                      \
                    /* We save the value to which we must set the position of pjDmaRead   */                      \
                    /* for the very last CPU read ( of size height % linesPerXfer)        */                      \
                                                                                                                  \
                    pulDmaRead_lastXfer = pulDmaRead_start;                                                       \
                                                                                                                  \
                    break;          /* break out of the Xfer loop (for(;;))  */                                   \
                }                                                                                                 \
                /*                                                                        */                      \
                /**************************************************************************/                      \
                                                                                                                  \
                /*                                                                        */                      \
                /* START another DMA-Xfer:    Remember, we use ( 2*bytesPerXfer ) bytes   */                      \
                /*                            in our Mem2Mem buffer. (So we alternate     */                      \
                /*                            using it's two parts )                      */                      \
                /*                                                                        */                      \
                /*                            The last Xfer kicked used the 2nd part.     */                      \
                /*                            Next Xfer is going to use the first part    */                      \
                /*                            again.                                      */                      \
                                                                                                                  \
                /*---[ So now: ]------------------------*/                                                        \
                /*                                      */                                                        \
                /* DstDMAOffset == 0;                   */                                                        \
                /*   pulDMAread == pulDmaRead_2ndPart;  */                                                        \
                /*                                      */                                                        \
                /*--------------------------------------*/                                                        \
                                                                                                                  \
                PushWaitSendDMAXfer_forXlines(linesPerXfer);                                                      \
                                                                                                                  \
                linesToGo       -= linesPerXfer;          /* Susbtract the lines that have been done. */          \
                SrcDMAOffset    += SrcDMAincPerXfer;      /* Advance in the source DMA region (VRAM)  */          \
                DstDMAOffset     = DstDMA2ndPartOffset;   /* Advance in the dest. DMA region          */          \
                                                          /* (the mem2mem buffer) so we use the       */          \
                                                          /* 2nd part the next time.                  */          \
                                                                                                                  \
                /***************************************************************************/                     \
                /*  In the meantime, CPU-Xfer what has been DMA-Xfered in the 2nd part     */                     \
                /***************************************************************************/                     \
                                                                                                                  \
                                                                                                                  \
                CpuXferMACRONAME(pxxDmaRead,linesPerXfer);                                                        \
                                                                                                                  \
                                                                                                                  \
                /* Set our pjDmaRead ptr at the beginning of the first part of our mem2mem buffer  */             \
                                                                                                                  \
                pxxDmaRead = cast(pulDmaRead_start);         /* Reset the read ptr at the        */               \
                                                             /* first part (beginning) of the    */               \
                                                             /* MEM2MEM buffer                   */               \
                /* ... CPU Xfer Done.  */                                                                         \
                                                                                                                  \
                                                                                                                  \
                /***[ Make sure there are still lines to Xfer before starting         *****/                      \
                /*                                                  another DMA Xfer ]    */                      \
                                                                                                                  \
                if ( linesToGo < linesPerXfer )  /* if there are no more lines to Xfer    */                      \
                {                                                                                                 \
                                                                                                                  \
                    /* Now what is left is a CPU Xfer of "linesPerXfer" plus at most      */                      \
                    /* one DMA+CPU Xfers of size " height % (modulo) linesPerXfer " ...   */                      \
                    /*                                                                    */                      \
                    /* We save the value to which we must set the position of pjDmaRead   */                      \
                    /* for the very last CPU read ( of size height % linesPerXfer)        */                      \
                                                                                                                  \
                    pulDmaRead_lastXfer = pulDmaRead_2ndPart;                                                     \
                                                                                                                  \
                    break;          /* break out of the Xfer loop (for(;;)) */                                    \
                                                                                                                  \
                } /* ...if there are less than "linesPerXfer" left to Xfer  */                                    \
                                                                                                                  \
                /*                                                                        */                      \
                /**************************************************************************/                      \
                                                                                                                  \
                                                                                                                  \
            } /* ...for(;;)  ( while(linesToGo >= linesPerXfer )  )  */                                           \
              /*                                                     */                                           \
              /* (this is a special cased 2 part loop)               */                                           \
                                                                                                                  \
                                                                                                                  \
        } /*  ...else enter the main Xfer loop (if there are at least linesPerXfer left to Xfer) */               \
                                                                                                                  \
        /***********************************************************************************/                     \
        /*                                                                                 */                     \
        /*                Last transfer: Time to execute the last transfer                 */                     \
        /*                                                                                 */                     \
        /*  There is at most one DMA transfer left to execute, and at most 2 mem2mem       */                     \
        /*  chunks to transfer (the last DMA that is about to be completed, and the        */                     \
        /*  one that we are about to start, if there is one)                               */                     \
        /*                                                                                 */                     \
        /***********************************************************************************/                     \
                                                                                                                  \
        if ( linesToGo == 0 ) /* only 1 CPU-Xfer left to execute                   */                             \
                              /* (that means that totalLinesToXfer was a multiple  */                             \
                              /* of linesPerXfer, => total % linesPerXfer == 0     */                             \
        {                                                                                                         \
            /*******************************************************************************/                     \
            /* Wait for the last DMA to be completed (use second notification structure)   */                     \
            /*******************************************************************************/                     \
                                                                                                                  \
            while ( pNotifier->status != 0 );                                                                     \
                                                                                                                  \
            CpuXferMACRONAME(pxxDmaRead,linesPerXfer);                                                            \
                                                                                                                  \
            /******************************************/                                                          \
            /* And this rectangle is done, at last... */                                                          \
            /******************************************/                                                          \
                                                                                                                  \
        } /* ...if ( linesToGo == 0 ) */                                                                          \
                                                                                                                  \
        else /* else, setup one last DMA Xfer */                                                                  \
        {                                                                                                         \
            /* This last DMA-Xfer is different in that the line count is                        */                \
            /* linesToGo (the rest of the lines, which is < than our linesPerXfer calculated )  */                \
            /*                                                                                  */                \
                                                                                                                  \
            PushWaitSendDMAXfer_forXlines(linesToGo);                                                             \
                                                                                                                  \
            /*******************************************************************************/                     \
            /* Now that this last DMA-Xfer is running, transfer the last "linesPerXfer"    */                     \
            /* long chunk. (this is our last but one CPU-Xfer)                             */                     \
            /*                                                                             */                     \
            /* We might think of one complication here: we do not know where we broke in   */                     \
            /* our main for(;;) loop.  So we do not know to which part of the mem2mem      */                     \
            /* buffer we must read our last cpu xfer. (In other words, where to put the    */                     \
            /* pjDmaRead ptr. This is solved in the loop by saving the next value to which */                     \
            /* it must be set (this is to avoid doing an additionnal "IF" here, to know    */                     \
            /* were we broke in the for(;;) )                                              */                     \
            /*                                                                             */                     \
            /*******************************************************************************/                     \
                                                                                                                  \
            CpuXferMACRONAME(pxxDmaRead,linesPerXfer);                                                            \
                                                                                                                  \
            /* Set our pjDmaRead ptr at the beginning of the ? part of our mem2mem buffer  */                     \
            /* (the right part for our last cpu xfer; this was selected in the for(;;)     */                     \
                                                                                                                  \
            pxxDmaRead = cast(pulDmaRead_lastXfer);                                                               \
                                                                                                                  \
                                                                                                                  \
            /*******************************************************************************/                     \
            /* Wait for the LAST DMA-Xfer to be completed                                  */                     \
            /* ( if it is not yet completed ) and CPU-Xfer these remainder lines           */                     \
            /*******************************************************************************/                     \
                                                                                                                  \
            while ( pNotifier->status != 0 );                                                                     \
                                                                                                                  \
            CpuXferMACRONAME(pxxDmaRead,linesToGo);                                                               \
                                                                                                                  \
            /*****************************************/                                                           \
            /* And this rectangle is done, at last...*/                                                           \
            /*****************************************/                                                           \
                                                                                                                  \
        } /* ...else ( linesToGo != 0 )  ( one last DMA xfer to setup )   */                                      \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
}                                                                                                                 \
/************************ End of ScreenToMemXbpp_Common2(pxxDmaRead,cast,CpuXferMACRONAME) ***********************/





#define PushSendDMAXfer_forXlines(linesPerXfer)                                                                   \
{                                                                                                                 \
    /***********************************************************************/                                     \
    /* Set methods for memory_to_memory dma blit                           */                                     \
    /*   NV039_OFFSET_IN                                                   */                                     \
    /*   NV039_OFFSET_OUT                                                  */                                     \
    /*   NV039_PITCH_IN                                                    */                                     \
    /*   NV039_PITCH_OUT                                                   */                                     \
    /*   NV039_LINE_LENGTH_IN                                              */                                     \
    /*   NV039_LINE_COUNT                                                  */                                     \
    /*   NV039_FORMAT                                                      */                                     \
    /*   NV039_BUFFER_NOTIFY                                               */                                     \
    /***********************************************************************/                                     \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Set notifier to 'BUSY' value (use second notification structure)        */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;                                                    \
                                                                                                                  \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Check if we've got enough room in the push buffer                       */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    NV_DMAPUSH_CHECKFREE(((ULONG)(9)));                                                                           \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
    NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );                                                              \
    NV_DMA_FIFO = SrcDMAOffset;                         /* Offset In  */                                          \
    NV_DMA_FIFO = DstDMAOffset;                         /* Offset Out */                                          \
    NV_DMA_FIFO = lSrcPitch;                            /* Pitch In   */                                          \
    NV_DMA_FIFO = bytesPerLine;                         /* Pitch Out == bytesPerLine because          */          \
                                                        /* we dont want any memory "hole" between     */          \
                                                        /* two contiguous lines in the mem2mem buffer */          \
                                                                                                                  \
    NV_DMA_FIFO = bytesPerLine;                         /* Line Length in bytes          */                       \
    NV_DMA_FIFO = linesPerXfer;                         /* Line Count (num to copy)      */                       \
    NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 |                                                                    \
                  NV_MTMF_FORMAT_OUTPUT_INC_1;          /* Format                        */                       \
    NV_DMA_FIFO = 0;                                    /* Set Buffer Notify and kickoff */                       \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Update global push buffer count                                         */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    UPDATE_PDEV_DMA_COUNT;                                                                                        \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Send data on thru to the DMA push buffer                                */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    NV4_DmaPushSend(ppdev);                                                                                       \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
}                                                                                                                 \
/*********************** End of PushSendDMAXfer_forXlines(linesPerXfer) ******************************************/
                                                                                                                   


#define PushWaitSendDMAXfer_forXlines(linesPerXfer)                                                               \
{                                                                                                                 \
    /***********************************************************************/                                     \
    /* Set methods for memory_to_memory dma blit                           */                                     \
    /*   NV039_OFFSET_IN                                                   */                                     \
    /*   NV039_OFFSET_OUT                                                  */                                     \
    /*   NV039_PITCH_IN                                                    */                                     \
    /*   NV039_PITCH_OUT                                                   */                                     \
    /*   NV039_LINE_LENGTH_IN                                              */                                     \
    /*   NV039_LINE_COUNT                                                  */                                     \
    /*   NV039_FORMAT                                                      */                                     \
    /*   NV039_BUFFER_NOTIFY                                               */                                     \
    /***********************************************************************/                                     \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Check if we've got enough room in the push buffer                       */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    NV_DMAPUSH_CHECKFREE(((ULONG)(9)));                                                                           \
                                                                                                                  \
                                                                                                                  \
    NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );                                                              \
    NV_DMA_FIFO = SrcDMAOffset;                         /* Offset In  */                                          \
    NV_DMA_FIFO = DstDMAOffset;                         /* Offset Out */                                          \
    NV_DMA_FIFO = lSrcPitch;                            /* Pitch In   */                                          \
    NV_DMA_FIFO = bytesPerLine;                         /* Pitch Out == bytesPerLine because          */          \
                                                        /* we dont want any memory "hole" between     */          \
                                                        /* two contiguous lines in the mem2mem buffer */          \
                                                                                                                  \
    NV_DMA_FIFO = bytesPerLine;                         /* Line Length in bytes          */                       \
    NV_DMA_FIFO = linesPerXfer;                         /* Line Count (num to copy)      */                       \
    NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 |                                                                    \
                  NV_MTMF_FORMAT_OUTPUT_INC_1;          /* Format                        */                       \
    NV_DMA_FIFO = 0;                                    /* Set Buffer Notify and kickoff */                       \
                                                                                                                  \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Wait for the last DMA-Xfer kicked to be completed (use 2nd notif.struct)*/                                 \
    /* ( if it is not yet completed )                                          */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    while ( pNotifier->status != 0 );                                                                             \
                                                                                                                  \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Set notifier to 'BUSY' value (use second notification structure)        */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;                                                    \
                                                                                                                  \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Update global push buffer count                                         */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    UPDATE_PDEV_DMA_COUNT;                                                                                        \
                                                                                                                  \
    /***************************************************************************/                                 \
    /* Send data on thru to the DMA push buffer                                */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    NV4_DmaPushSend(ppdev);                                                                                       \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
                                                                                                                  \
}                                                                                                                 \
/********************************* End of PushWaitSendDMAXfer_forXlines ******************************************/


// CpuXferTo4bpp(pxxDmaRead,linesPerXfer)
//
// Here we do:
//
//  For the left 
//
//   1) Read the final dest byte (containing the left pixel)
//      by masking also the bits we will modifie (read with an AND
//      between our mask and dest byte)
//
//   2) Read and translate the pixel (32bit) in the DMA mem2mem buffer
//   3) Shift translated byte to put it's 4 bits in the correct part
//      of the byte
//   4) Write the 4bits of the xlated byte (this is our new pixel!)
//      inside the byte "created" at step 1. (by doing step1_byte |= xlated_byte;
//   5) Write back the resulting byte from step 4 in it's final destination
//      (same address then the one we read from at step 1)
//
//   After doing the left byte, we do the middle bytes in a similar way
//   except that we don't have to mask because all the byte must be overwritten.
//   Then we do the right byte in a similar way than the left byte.

// ********************
// Do the Left byte
// ********************

// If the left coordinate was starting on the low nibble of the byte containing
// the pixel, we need to process another pixel before going on to the middle bytes

// (We could have done this to avoid doing this if:
//
//   (similar to the ScreenTo1Bpp code )
//
// Transfer more than we need (by clamping the right coordinate
// to the highest even number, and clamping the left one the lowest even)
// This is done to avoid special casing when we translate the LEFT byte
// in the CPU-Xfer loop: if the left coordinate start in the low nibble
// of a byte in the dest scanline, this means that we are going to read
// only 1 32bit pixel in the mem2mem buffer, before doing the middle
// bytes. But if the left coordinate start on the high nibble, we will
// have to convert 2 pixels from the mem2mem buffer and combine them
// in 1 byte to write before going on the middle bytes. (We would also use
// a real mask, pretty much like ScreenTo1Bpp but only a 2 entry LUT, we have 4bpp)
//
//
// But is it really worth it ?  Because when u think of it: you have more overhead
// by reading more than you need (insignificant for the DMA I agree), overhead to
// clamp the coordinates (insignificant), and (more significant) overhead of always
// Xlating a pixel for nothing (in the case where there is a rest)
//  (call to the Xlate macro). (Because this pixel is not in the destination region
// to modifie and it will get masked out anyway by our mask, if there's a rest)
//
// SO, to summarize, we use "IFs" instead of a general algo like 
// " transfer an even number of byte + always convert all pixels in the byte + and only
// write back, at the dest, the pixels we really need to modifie " we use IF's to avoid
// converting pixels for nothing (because this is expensive)

            
#define CpuXferTo4bpp(pxxDmaRead,linesPerXfer)                                                                    \
{                                                                                                                 \
    /* Xfer all lines    */                                                                                       \
    /*                   */                                                                                       \
    for(j=0; j < linesPerXfer; j++)                                                                               \
    {                                                                                                             \
        /*************************/                                                                               \
        /* Do the Left byte      */                                                                               \
        /*************************/                                                                               \
                                                                                                                  \
        if ( leftRest == 0) /* There is no rest, so that we must modifie the whole left byte */                   \
        {                                                                                                         \
            XlatedByte   = (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead); /* xlate first pixel             */   \
            pxxDmaRead++;                                                                                         \
            XlatedByte <<= 4;                                           /* place the nibble in the MS Nibble */   \
            XlatedByte  |= (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead); /* xlate & combine second pixel  */   \
            pxxDmaRead++;                                                                                         \
                                                                                                                  \
                /* NOTE: Here we know the destination is 4Bpp,                       */                           \
                /*       so even if the macro returns a ULONG,                       */                           \
                /*       it will only have it's lower 4 bits                         */                           \
                /*       equal to the color, the rest of them                        */                           \
                /*       will be 0. If they are not 0, we will                       */                           \
                /*       have garbage in XlatedByte, and will                        */                           \
                /*       have a faulty convertion.  We assume                        */                           \
                /*       that the macro function correctly, but                      */                           \
                /*       a defensive code would be:                                  */                           \
                /*  |= (BYTE) ( XLATEOBJ_iXlate(pxlo, *pulDmaRead++) & 0x0000000F)   */                           \
                                                                                                                  \
            *pjFinalDstWrite++ = XlatedByte; /* Write the byte in the dest region */                              \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            /* Else there is a rest, we must modifie the LOW Nibble only */                                       \
                                                                                                                  \
            TempByte   = *pjFinalDstWrite;                                                                        \
            TempByte  &= 0xF0;            /* TempByte = DestSurfByte & Mask (clear the bits         */            \
                                          /*                                 we are going to mod.)  */            \
                                                                                                                  \
            XlatedByte = (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead); /* Translate source pixel     */        \
            pxxDmaRead++;                                                                                         \
                                                                                                                  \
            TempByte  |= XlatedByte;                                                                              \
                                                                                                                  \
            *pjFinalDstWrite++ = TempByte; /* Write back the left byte.    */                                     \
                                                                                                                  \
        }                                                                                                         \
                                                                                                                  \
        /*************************/                                                                               \
        /* Do the Middle byte(s) */                                                                               \
        /*************************/                                                                               \
                                                         /* Reminder of ptr arythmetics (if ptr is ULONG*): */    \
        for(limit = (BYTE*)( pxxDmaRead + middleWidth);  /* limit = the ptr + width*4 bytes/pixel           */    \
            (BYTE*) pxxDmaRead < limit;                                                                           \
            pjFinalDstWrite++ )                                                                                   \
        {                                                                                                         \
            XlatedByte   = (BYTE) NV_XLATEOBJ_IXLATE(pxlo,*pxxDmaRead);                                        \
            pxxDmaRead++;                                                                                         \
            XlatedByte <<= 4;                                                                                     \
            XlatedByte  |= (BYTE) NV_XLATEOBJ_IXLATE(pxlo,*pxxDmaRead);                                        \
            pxxDmaRead++;                                                                                         \
                                                                                                                  \
            *pjFinalDstWrite = XlatedByte;                                                                        \
        }                                                                                                         \
                                                                                                                  \
        /*************************/                                                                               \
        /* Do the Right byte     */                                                                               \
        /*************************/                                                                               \
                                                                                                                  \
        if( rightRest != 0 ) /* there is a rest */                                                                \
        {                                                                                                         \
            /* Modifie the HIGH NIBBLE of the right byte (the LOW one, corresponding to  */                       \
            /* the coordinate, is exclusive)                                             */                       \
                                                                                                                  \
            TempByte   = *pjFinalDstWrite;                                                                        \
            TempByte  &= 0x0F;             /* TempByte = DestSurfByte & Mask (clear the bits          */          \
                                           /*                                 we are going to mod.)   */          \
                                                                                                                  \
            XlatedByte   = (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead);  /* Translate source pixel    */      \
            pxxDmaRead++;                                                                                         \
                                                                                                                  \
            XlatedByte <<= 4;             /* Place the pixel in the high nibble (filling in zeros     */          \
                                          /* in the low nibble so we don't modifie the low nibble     */          \
                                          /* when ORing next)                                         */          \
                                                                                                                  \
            TempByte |= XlatedByte;       /* Combine the translated pixel to the pixel we don't */                \
                                          /* want to modifie                                    */                \
                                                                                                                  \
            *pjFinalDstWrite++ = TempByte;   /* Write back the right byte. */                                     \
                                                                                                                  \
                                                                                                                  \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            /* Else, we do a full byte: Indeed, in our calculation for "middleWidth", middleWidth = number of */  \
            /* FULL bytes AFTER the left byte (containing the right coordinate) minus 1.  Thus, if the scan-  */  \
            /* line ends with a FULL byte to modifie (rightRest = 0), this full byte is not included in the   */  \
            /* "middleWidth" and is considered the right byte (so if there's only 2 bytes, all in all, in the */  \
            /* scanline, even if they are full, they are considered the "left and right bytes"                */  \
            /*                                                                                                */  \
            /* ALSO, we might think "what if the byte previous (left) to the one containing the right coordi- */  \
            /* nate, is not one full byte ?" Well, the only way this can happen is to have the computation    */  \
            /* of "middleWidth" to be -1  (see in the code ScreenToMem?bpp), and we handle those cases with   */  \
            /* a different function (yes, it could be the same, like in ScreenTo1Bpp, but it is faster to do  */  \
            /* this a different way, because we do not convert a bunch of pixels for nothing                  */  \
                                                                                                                  \
            XlatedByte       = (BYTE) NV_XLATEOBJ_IXLATE(pxlo,*pxxDmaRead);                                    \
            pxxDmaRead++;                                                                                         \
            XlatedByte     <<= 4;                                                                                 \
            XlatedByte      |= (BYTE) NV_XLATEOBJ_IXLATE(pxlo,*pxxDmaRead);                                    \
            pxxDmaRead++;                                                                                         \
            *pjFinalDstWrite++ = XlatedByte;                                                                      \
                                                                                                                  \
        }                                                                                                         \
                                                                                                                  \
        pjFinalDstWrite += FinalDstJump2NextLineBeg;  /* Switch to beginning of the    */                         \
                                                      /* next line in the dest surface */                         \
    } /* ...for all bytes in chunk to cpu-xfer  */                                                                \
                                                                                                                  \
}                                                                                                                 \
/******************************** End of CpuXferTo4bpp(pxxDmaRead,linesPerXfer) **********************************/


#define CpuXferTo4bpp_1ByteWidth(pxxDmaRead,linesPerXfer)                                                         \
{                                                                                                                 \
    /* Xfer all lines    */                                                                                       \
    /*                   */                                                                                       \
    for(j=0; j < linesPerXfer; j++)                                                                               \
    {                                                                                                             \
        /*************************/                                                                               \
        /* Do the Left byte      */                                                                               \
        /*************************/                                                                               \
                                                                                                                  \
        if ( leftRest == 0 )  /* There is no rest */                                                              \
        {                                                                                                         \
            if ( rightRest == 0 )   /* AND the right coord. has no rest (even). We have 2 pixels */               \
            {                                                                                                     \
                XlatedByte   = (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead);  /* xlate first pixel */          \
                pxxDmaRead++;                                                                                     \
                                                                                                                  \
                XlatedByte <<= 4;     /* place the nibble in the MS Nibble */                                     \
                                                                                                                  \
                XlatedByte  |= (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead);/* xlate & combine second pixel */ \
                pxxDmaRead++;                                                                                     \
                                                                                                                  \
                *pjFinalDstWrite++ = XlatedByte;  /* Write the byte in the dest region */                         \
            }                                                                                                     \
            else                                                                                                  \
            {                                                                                                     \
                /* Else just do the high nibble */                                                                \
                                                                                                                  \
                TempByte   = *pjFinalDstWrite;                                                                    \
                TempByte  &= 0x0F;             /* TempByte = DestSurfByte & Mask (clear the bits        */        \
                                               /*                                 we are going to mod.) */        \
                                                                                                                  \
                XlatedByte   = (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead); /* Translate source pixel   */    \
                pxxDmaRead++;                                                                                     \
                                                                                                                  \
                XlatedByte <<= 4;              /* Place the pixel in the high nibble (filling in zeros  */        \
                                               /* in the low nibble so we don't modifie the low nibble  */        \
                                               /* when ORing next)                                      */        \
                                                                                                                  \
                TempByte |= XlatedByte;        /* Combine the translated pixel to the pixel we don't    */        \
                                               /* want to modifie                                       */        \
                                                                                                                  \
                *pjFinalDstWrite++ = TempByte;  /* Write back the right byte.                           */        \
                                                                                                                  \
            }                                                                                                     \
        }                                                                                                         \
        else                                                                                                      \
        {                                                                                                         \
            /* Else there is a rest, we must modifie the LOW Nibble only and we are finished!    */               \
                                                                                                                  \
            TempByte   = *pjFinalDstWrite;                                                                        \
            TempByte  &= 0xF0;             /* TempByte = DestSurfByte & Mask (clear the bits            */        \
                                           /*                                 we are going to mod.)     */        \
                                                                                                                  \
            XlatedByte = (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead);   /* Translate source pixel       */    \
            pxxDmaRead++;                                                                                         \
                                                                                                                  \
            TempByte  |= XlatedByte;                                                                              \
                                                                                                                  \
            *pjFinalDstWrite++ = TempByte;   /* Write back the left byte. */                                      \
                                                                                                                  \
        }                                                                                                         \
                                                                                                                  \
                                                                                                                  \
        pjFinalDstWrite += FinalDstJump2NextLineBeg;   /* Switch to beginning of the              */              \
                                                       /* next line in the dest surface           */              \
    } /* ...for all bytes in chunk to cpu-xfer */                                                                 \
}                                                                                                                 \
/**************************** End of CpuXferTo4bpp_1ByteWidth(pxxDmaRead,linesPerXfer) ***************************/


#define CpuXferTo8bpp(pxxDmaRead,linesPerXfer)                                                                    \
{                                                                                                                 \
    for(j=0; j < linesPerXfer; j++)                                                                               \
    {                                                                                                             \
        /* Xfer a whole line */                                                                                   \
        /*                   */                                                                                   \
                                                         /* Reminder of ptr arythmetics (if ptr is ULONG*): */    \
        for(limit = (BYTE*)( pxxDmaRead + width);        /* limit = the ptr + width*4 bytes/pixel           */    \
            (BYTE*) pxxDmaRead < limit;                                                                           \
            pxxDmaRead++, pjFinalDstWrite++ )                                                                     \
        {                                                                                                         \
            *pjFinalDstWrite = (BYTE) NV_XLATEOBJ_IXLATE(pxlo, *pxxDmaRead); /* Translate source pixel & write */ \
        }                                                                                                         \
                                                                                                                  \
        pjFinalDstWrite += FinalDstJump2NextLineBeg;    /* Switch to beginning of the    */                       \
                                                        /* next line in the dest surface */                       \
                                                                                                                  \
    } /* ...for all bytes in chunk to cpu-xfer */                                                                 \
}                                                                                                                 \
/****************************** End of CpuXferTo8bpp(pxxDmaRead,linesPerXfer) ************************************/





#define ScreenToMem4bpp(pxxDmaRead,cast)                                                                          \
{                                                                                                                 \
    /***************************************************************************/                                 \
    /*  Transfer all rectangles regions in the array                           */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    for(i=0; i < c; i++) /* For all rectangles */                                                                 \
    {                                                                                                             \
                                                                                                                  \
        /* Calculate the number of full bytes we have per scanline              */                                \
        /* (this will need to be clamped to 0 if it is negative; we do that     */                                \
        /*  later because this value will make us decide which function         */                                \
        /*  we use in case the destination is 4Bpp)                             */                                \
                                                                                                                  \
        /*  Explanation for this formula: the right coordinate is exclusive, so we substract 1,    */             \
        /*  and clamp it to the lowest multiple of 2. We clamp the left coordinate to the lowest   */             \
        /*  multiple of 2 also.  (This is because we have 2 pixel / bytes when the dest is 4BPP)   */             \
        /*  (We only use this "middleWidth" when dest is 4 BPP.) We substract both of them, and    */             \
        /*  -1 also, because we want 0 => when there is a left byte and a right byte (2 byte that  */             \
        /*  contains the pixels), -1 when the width is at most 1 byte (the pixel(s) of one         */             \
        /*  scanline are in a single byte), and else, "middleWidth" gives us the number of bytes   */             \
        /*  taking aside the first byte we process and the last one. We multiply by 2 the whole    */             \
        /*  formula ( << 1 at the end) because we want middleWidth in NUMBER OF PIXELS.            */             \
                                                                                                                  \
        middleWidth = ( ((parcl[i].right - 1) >> 1) - (parcl[i].left >> 1) - 1 ) << 1 ;                           \
                                                                                                                  \
        /* Calculate the rest of the left and right coordinates (this is out of ScreenToMemXbpp_Common */         \
        /* because for 8bpp destination, we don't need this )                                          */         \
                                                                                                                  \
        leftRest  = (BYTE)( parcl[i].left & 1 );        /* leftRest = 1 if left coordinate is odd  */             \
        rightRest = (BYTE)( parcl[i].right & 1 );                                                                 \
                                                                                                                  \
        ScreenToMemXbpp_Common;    /* This is the beginning of the Xfer loop (for all rectangles) common */       \
                                   /* to all ScreenToMemXbpp functions.                                  */       \
                                   /* In this macro, we setup the linesPerXfer, and all variables needed */       \
                                   /* after this point in the for(all rectangles) (dma offsets, read/    */       \
                                   /* write ptr addresses, ... )                                         */       \
                                                                                                                  \
        /* Now we must chose the core routine for our ScreenToMem4bpp blit:     */                                \
                                                                                                                  \
        if(middleWidth < 0)                                                                                       \
        {                                                                                                         \
            /*middleWidth = 0;   No need to, in CpuXferTo4bpp_1ByteWidth, we don't use it  */                     \
            ScreenToMem4bpp_Core_1ByteWidth(pxxDmaRead, cast);                                                    \
        }                                                                                                         \
        else ScreenToMem4bpp_Core(pxxDmaRead, cast);                                                              \
                                                                                                                  \
    } /* ...for(i=0; i < c; i++)  For all rectangles  */                                                          \
                                                                                                                  \
                                                                                                                  \
}                                                                                                                 \
/********************************** End of ScreenToMem4bpp(pxxDmaRead,cast) **************************************/





//
// Usage: ScreenToMem4bpp_Core( pjDmaread, (BYTE*))
//

    
#define ScreenToMem4bpp_Core(pxxDmaRead,cast)                                                                     \
{                                                                                                                 \
                                                                                                                  \
                                                                                                                  \
        /***************************************************************************    */                        \
        /* Calculate the pointer to the upper-left corner of Destination clipped region */                        \
        /* (THE FINAL DESTINATION, not the MEM2MEM buffer)                              */                        \
        /***************************************************************************    */                        \
                                                                                                                  \
        pjFinalDstWrite =  (BYTE*) psoDst->pvScan0                                                                \
                          + ( parcl[i].top  * lDstPitch )                                                         \
                          + ( parcl[i].left >> 1 );                                                               \
                                                                                                                  \
                                                                                                                  \
        /* Calculate the jump to get to the next line of the             */                                       \
        /* final destination rectangle (in the final Dst surface)        */                                       \
        /* AFTER having transfered "bytesPerLine" (in other words,       */                                       \
        /* pitch is the num of bytes to the jump AT THE SAME POSITION    */                                       \
        /* in the bitmap but one line lower; what we want is the jump    */                                       \
        /* value to jump from the end of a line TO THE BEGINNING         */                                       \
        /* of the next line )                                            */                                       \
        /*                                                               */                                       \
        /* We add 1 to width so that we round to the upper multiple      */                                       \
        /* of 2 (we have 2 pixels per byte, and the scanline must be     */                                       \
        /* of at least 1 byte wide even if it is 1 pixel wide only...    */                                       \
                                                                                                                  \
        FinalDstJump2NextLineBeg = lDstPitch - ((width+1) >> 1);                                                  \
                                                                                                                  \
                                                                                                                  \
        /* Do the rest of the loop (all the core of the xfer/convertion  */                                       \
                                                                                                                  \
                                                                                                                  \
        ScreenToMemXbpp_Common2(pxxDmaRead, cast,CpuXferTo4bpp);                                                  \
                                                                                                                  \
}                                                                                                                 \
/****************************** End of ScreenToMem4bpp_Core(pxxDmaRead,cast) *************************************/




#define ScreenToMem4bpp_Core_1ByteWidth(pxxDmaRead,cast)                                                          \
{                                                                                                                 \
                                                                                                                  \
                                                                                                                  \
        /***************************************************************************    */                        \
        /* Calculate the pointer to the upper-left corner of Destination clipped region */                        \
        /* (THE FINAL DESTINATION, not the MEM2MEM buffer)                              */                        \
        /***************************************************************************    */                        \
                                                                                                                  \
        pjFinalDstWrite =  (BYTE*) psoDst->pvScan0                                                                \
                          + ( parcl[i].top  * lDstPitch )                                                         \
                          + ( parcl[i].left >> 1 );                                                               \
                                                                                                                  \
                                                                                                                  \
        /* Calculate the jump to get to the next line of the             */                                       \
        /* final destination rectangle (in the final Dst surface)        */                                       \
        /* AFTER having transfered "bytesPerLine" (in other words,       */                                       \
        /* pitch is the num of bytes to the jump AT THE SAME POSITION    */                                       \
        /* in the bitmap but one line lower; what we want is the jump    */                                       \
        /* value to jump from the end of a line TO THE BEGINNING         */                                       \
        /* of the next line )                                            */                                       \
        /*                                                               */                                       \
        /* We add 1 to width so that we round to the upper multiple      */                                       \
        /* of 2 (we have 2 pixels per byte, and the scanline must be     */                                       \
        /* of at least 1 byte wide even if it is 1 pixel wide only...    */                                       \
                                                                                                                  \
        FinalDstJump2NextLineBeg = lDstPitch - 1;                                                                 \
                                                                                                                  \
        /* Do the rest of the loop (all the core of the xfer/convertion  */                                       \
                                                                                                                  \
        ScreenToMemXbpp_Common2(pxxDmaRead, cast, CpuXferTo4bpp_1ByteWidth);                                      \
                                                                                                                  \
                                                                                                                  \
}                                                                                                                 \
/*************************** End of ScreenToMem4bpp_Core_1ByteWidth(pxxDmaRead,cast) *****************************/






#define ScreenToMem8bpp(pxxDmaRead,cast)                                                                          \
{                                                                                                                 \
    /***************************************************************************/                                 \
    /*  Transfer all rectangles regions in the array                           */                                 \
    /***************************************************************************/                                 \
                                                                                                                  \
    for(i=0; i < c; i++) /* For all rectangles */                                                                 \
    {                                                                                                             \
                                                                                                                  \
        ScreenToMemXbpp_Common;    /* This is the beginning of the Xfer loop (for all rectangles) common */       \
                                   /* to all ScreenToMemXbpp functions.                                  */       \
                                   /* In this macro, we setup the linesPerXfer, and all variables needed */       \
                                   /* after this point in the for(all rectangles) (dma offsets, read/    */       \
                                   /* write ptr addresses, ... )                                         */       \
                                                                                                                  \
                                                                                                                  \
        /***************************************************************************    */                        \
        /* Calculate the pointer to the upper-left corner of Destination clipped region */                        \
        /* (THE FINAL DESTINATION, not the MEM2MEM buffer)                              */                        \
        /***************************************************************************    */                        \
                                                                                                                  \
        pjFinalDstWrite =  (BYTE*) psoDst->pvScan0                                                                \
                          + ( parcl[i].top  * lDstPitch )                                                         \
                          + ( parcl[i].left );                                                                    \
                                                                                                                  \
                                                                                                                  \
        /* Calculate the jump to get to the next line of the             */                                       \
        /* final destination rectangle (in the final Dst surface)        */                                       \
        /* AFTER having transfered "bytesPerLine" (in other words,       */                                       \
        /* pitch is the num of bytes to the jump AT THE SAME POSITION    */                                       \
        /* in the bitmap but one line lower; what we want is the jump    */                                       \
        /* value to jump from the end of a line TO THE BEGINNING         */                                       \
        /* of the next line )                                            */                                       \
        /*                                                               */                                       \
        /* (We use width directly because if we are here, we know        */                                       \
        /*  we have 8bpp in the destination, 1 byte per pixel...)        */                                       \
                                                                                                                  \
        FinalDstJump2NextLineBeg = lDstPitch - width;                                                             \
                                                                                                                  \
        /* Do the rest of the loop (all the core of the xfer/convertion  */                                       \
                                                                                                                  \
        ScreenToMemXbpp_Common2(pxxDmaRead, cast, CpuXferTo8bpp);                                                 \
                                                                                                                  \
    } /* ...for(i=0; i < c; i++)  For all rectangles  */                                                          \
                                                                                                                  \
}                                                                                                                 \
/********************************** End of ScreenToMem8bpp(pxxDmaRead,cast) **************************************/
    
    



BOOL NV4ScreenToMem32to4bppBlt(

    PDEV*       ppdev,              // PDEV associated with the SRC device
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      parcl,              // Array of destination rectangles (inside prclDst)
    SURFOBJ*    psoSrc,             // Source surface
    SURFOBJ*    psoDst,             // Dest surface
    POINTL*     pptlSrc,            // Upper left corner of SRC retcl in SRC surface
    RECTL*      prclDst,            // Rectangle to be modified in the DST surface
    XLATEOBJ*   pxlo)               // Provides color translation info
    
{
    
#if DBG        // only allocate those for convenience (for the next assert) on checked builds

    ULONG*      DstBits;
    ULONG*      SrcBits;
    ULONG*      ScreenBitmap;
    DSURF*      pdsurfDst;
    DSURF*      pdsurfSrc;   

#endif

#ifdef  CALIBRATION_ENABLED
    
    ULONG       tempUlong;
    BYTE        msbitPos;

#endif
    

    LONG            i;
    ULONG           j;
    
    BYTE*           limit;

    ULONG           width,height;

    ULONG           linesToGo;           // Total number of lines left to Xfer

    ULONG           bytes2Xfer;          // Total amount of bytes to Xfer for current rectangle
    
    ULONG           bytesPerLine;        // Width of one line of the current rectangle, in bytes

    ULONG           optBytesPerXfer;     // Optimal bytes per Xfer (calculated based on mathematic model 
                                         // and profiling data, see notes)
    ULONG           linesPerXfer;        // Number of lines to transfer by Xfer (based on bytesPerLine and OptBytesPerXfer)

    ULONG           DstDMAincPerXfer;    // linesPerXfer * bytesPerline
    ULONG           SrcDMAincPerXfer;    // (lSrcPitch+bytesPerLine)*linesPerXfer

    LONG            lSrcPitch;           // Pitch of the source surface
    
    LONG            lDstPitch;           // Pitch of the final dest surface

    LONG            FinalDstJump2NextLineBeg; // DstPitch - byterperline (see where it is used)

    BYTE*           pjSrcBits;           // Pointer to the beginning of the source bits (for the Xfer)
    BYTE*           pjSrcRclBits;        // Pointer to the beginning of the source cliped rectangle bits

    BYTE*           pjFinalDstWrite;     // Pointer to the beg. of the dest. bits (in the final surface)
                                         // (will also be incremented to advance in the destination bits during xfer)
    
    ULONG*          pulDmaRead;          // Pointer in the mem2mem dma buffer that we are going to use in the
                                         // CPU-Xfers to keep track of where we are currently reading 
    
    //USHORT*         psDmaRead;
    //BYTE*           pjDmaRead;
    
    BYTE            XlatedByte, TempByte;

    ULONG*          pulDmaRead_lastXfer; // Var to hold where to read the last CPU-Xfer

    ULONG*          pulDmaRead_start;    // Start of the Mem2Mem buffer

    ULONG*          pulDmaRead_2ndPart;  // Address of 2nd part of Mem2Mem buffer

    POINTL          ptlSrcRegion;        // Left top corner of current clipped region/rectangle in Src surface
                                         // to xfer
    
    ULONG           DstDMAOffset;        // Offset (relative to DMA base)
                                         // of the destination bytes in the dst DMA region
    ULONG           SrcDMAOffset;        // Offset of the source bytes in the src DMA region
    
    ULONG           DstDMA2ndPartOffset; // Offset of the 2nd part of the Mem2Mem buffer
       
    BYTE            leftRest, rightRest; // Rest of ( %(modulo) 2 ) of the left and right coordinates 

    LONG            middleWidth;         // Number of bytes to process with the "middle" loop (when Dest == 4 bpp)
    
    NvNotification* pNotifier;           // Pointer to MemToMem notifier array
    NV_XLATE_INIT                        // Initialize cached xlate values
    DECLARE_DMA_FIFO;                    // Local fifo var declare (ULONG nvDmaCount, * nvDmaFifo;)
                                          

    //**************************************************************************
    // Initialize notifier pointer to second notification structure
    //**************************************************************************
    
    pNotifier = (NvNotification *) (&(ppdev->Notifiers->MemToMem[NV039_NOTIFIERS_BUFFER_NOTIFY*sizeof(NvNotification)]));
        
    //**************************************************************************
    // Get push buffer global information (from pdev) into local variables
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;            // local fifo var = pdev fifo var;
                
    
    //**************************************************************************
    // Validate parameters first
    //**************************************************************************

    ASSERTDD( ppdev != NULL,
              "NV4ScreenToMem32to4bppBlt: NULL PDEV supplied !" );

    ASSERTDD( c > 0,
              "NV4ScreenToMem32to4bppBlt: Can't handle zero rectangles" ); // ok

    ASSERTDD( parcl != NULL,
              "NV4ScreenToMem32to4bppBlt: NULL destination rectangles supplied !" );
    
    ASSERTDD( psoSrc != NULL,
              "NV4ScreenToMem32to4bppBlt: NULL source surface supplied !" );

    ASSERTDD( psoDst != NULL,
              "NV4ScreenToMem32to4bppBlt: NULL destination surface supplied !" );
    
    ASSERTDD( pptlSrc != NULL,
              "NV4ScreenToMem32to4bppBlt: NULL source upper left point supplied !" );

    ASSERTDD( prclDst != NULL,
              "NV4ScreenToMem32to4bppBlt: NULL destination rectangle supplied !" );

    ASSERTDD( (psoDst->iBitmapFormat == BMF_4BPP),
              "NV4ScreenToMem32to4bppBlt: Dst bitmap format must be 4bpp" );

    ASSERTDD( (psoSrc->iBitmapFormat == BMF_32BPP),
              "NV4ScreenToMem32to4bppBlt: Src bitmap format must be 32bpp" );

    ASSERTDD( pxlo != NULL,
              "NV4ScreenToMem32to4bppBlt: NULL translation object");

    

#if DBG // we dont put asserts in if DBG, the actual macro definition, ASSERTDD is omited when DBG is not defined.
    
    DstBits                = (ULONG *) psoDst->pvBits;
    SrcBits                = (ULONG *) psoSrc->pvBits;
    ScreenBitmap           = (ULONG *) ppdev->pjScreen;
    pdsurfDst              = (DSURF *) psoDst->dhsurf;
    pdsurfSrc              = (DSURF *) psoSrc->dhsurf;
    
#endif         
    
    // Assert that this is a VRAM to SYSRAM blit ( we wont check it with an if, this
    // function is solely made for this case anyway ) 

    ASSERTDD( 
              //SRC is VRAM:       
              (
                ( SrcBits == ScreenBitmap) ||              
                ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN) )
              ) &&
              
              //DEST is SYSRAM:
              (
                (DstBits != ScreenBitmap) &&
                ( (pdsurfDst == NULL) || (pdsurfDst->dt == DT_DIB) )
              ),

              "NV4ScreenToMem32to4bppBlt: Expected a VRAM to SYSRAM blit !"
             
            );

    
    //**************************************************************************
    // Set memory to memory object in spare subchannel
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(2)));

    if (ppdev->dDrawSpareSubchannelObject != MY_MEMORY_TO_MEMORY) {
        
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), MY_MEMORY_TO_MEMORY);
        ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;
    }


    //**************************************************************************
    // Check if source bitmap (from video mem) is offscreen or onscreen
    // and set pitch and src ptr from the appropriate descriptive structure
    //**************************************************************************

    if ( psoSrc->dhsurf != NULL )               // The DSURF obj is not NULL, so
                                                // this srfc is device-managed
                                                // so it is in VRAM, offscreen
        {
        lSrcPitch  = ( (DSURF *) psoSrc->dhsurf )->LinearStride;
        pjSrcBits  = ( (DSURF *) psoSrc->dhsurf )->LinearPtr;
        }
    else
        { // Onscreen (primary surface, GDI managed)
        
        lSrcPitch   = ppdev->lDelta;
        pjSrcBits   = ppdev->pjScreen;
        }
    
    lDstPitch = psoDst->lDelta ;  // Also set the final dest. surface pitch

    
    // Set the offset (relative to the beginning of the dma mem2mem buffer)
    // of the middle of the mem2mem buffer
    // (See further, we use this as the address of the 2nd part of the mem2mem
    // buffer - one part is being written to, while one part is being read from)
     
    DstDMA2ndPartOffset = (ppdev->MemToMemBufferSize >> 1) ;
    
    // Also get the start and middle adress of the mem2membuffer

    pulDmaRead_start   = (ULONG *)( ppdev->pMemToMemBuffer  );
    pulDmaRead_2ndPart = (ULONG *)( (BYTE*)(ppdev->pMemToMemBuffer) + (ppdev->MemToMemBufferSize >> 1) );
            

    /***********************************************************************/
    /*                                                                     */
    /*  We are ready to start the transfer                                 */
    /*                                                                     */
    /***********************************************************************/
    
    pulDmaRead = pulDmaRead_start;

    ScreenToMem4bpp(pulDmaRead, (ULONG*));
    
    return TRUE;
}
            

BOOL NV4ScreenToMem32to8bppBlt(

    PDEV*       ppdev,              // PDEV associated with the SRC device
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      parcl,              // Array of destination rectangles (inside prclDst)
    SURFOBJ*    psoSrc,             // Source surface
    SURFOBJ*    psoDst,             // Dest surface
    POINTL*     pptlSrc,            // Upper left corner of SRC retcl in SRC surface
    RECTL*      prclDst,            // Rectangle to be modified in the DST surface
    XLATEOBJ*   pxlo)               // Provides color translation info
    
{
    
#if DBG        // only allocate those for convenience (for the next assert) on checked builds

    ULONG*      DstBits;
    ULONG*      SrcBits;
    ULONG*      ScreenBitmap;
    DSURF*      pdsurfDst;
    DSURF*      pdsurfSrc;   

#endif

#ifdef  CALIBRATION_ENABLED
    
    ULONG       tempUlong;
    BYTE        msbitPos;

#endif
    

    LONG            i;
    ULONG           j;
    
    BYTE*           limit;

    ULONG           width,height;

    ULONG           linesToGo;           // Total number of lines left to Xfer

    ULONG           bytes2Xfer;          // Total amount of bytes to Xfer for current rectangle
    
    ULONG           bytesPerLine;        // Width of one line of the current rectangle, in bytes

    ULONG           optBytesPerXfer;     // Optimal bytes per Xfer (calculated based on mathematic model 
                                         // and profiling data, see notes)
    ULONG           linesPerXfer;        // Number of lines to transfer by Xfer (based on bytesPerLine and OptBytesPerXfer)

    ULONG           DstDMAincPerXfer;    // linesPerXfer * bytesPerline
    ULONG           SrcDMAincPerXfer;    // (lSrcPitch+bytesPerLine)*linesPerXfer

    LONG            lSrcPitch;           // Pitch of the source surface
    
    LONG            lDstPitch;           // Pitch of the final dest surface

    LONG            FinalDstJump2NextLineBeg; // DstPitch - byterperline (see where it is used)

    BYTE*           pjSrcBits;           // Pointer to the beginning of the source bits (for the Xfer)
    BYTE*           pjSrcRclBits;        // Pointer to the beginning of the source cliped rectangle bits

    BYTE*           pjFinalDstWrite;     // Pointer to the beg. of the dest. bits (in the final surface)
                                         // (will also be incremented to advance in the destination bits during xfer)
    
    ULONG*          pulDmaRead;          // Pointer in the mem2mem dma buffer that we are going to use in the
                                         // CPU-Xfers to keep track of where we are currently reading 
    
    //USHORT*         psDmaRead;
    //BYTE*           pjDmaRead;
    
    BYTE            XlatedByte, TempByte;

    ULONG*          pulDmaRead_lastXfer; // Var to hold where to read the last CPU-Xfer

    ULONG*          pulDmaRead_start;    // Start of the Mem2Mem buffer

    ULONG*          pulDmaRead_2ndPart;  // Address of 2nd part of Mem2Mem buffer

    POINTL          ptlSrcRegion;        // Left top corner of current clipped region/rectangle in Src surface
                                         // to xfer
    
    ULONG           DstDMAOffset;        // Offset (relative to DMA base)
                                         // of the destination bytes in the dst DMA region
    ULONG           SrcDMAOffset;        // Offset of the source bytes in the src DMA region
    
    ULONG           DstDMA2ndPartOffset; // Offset of the 2nd part of the Mem2Mem buffer
       
    BYTE            leftRest, rightRest; // Rest of ( %(modulo) 2 ) of the left and right coordinates 

    LONG            middleWidth;         // Number of bytes to process with the "middle" loop (when Dest == 4 bpp)
    
    NvNotification* pNotifier;           // Pointer to MemToMem notifier array
    NV_XLATE_INIT                        // Initialize cached xlate values
    DECLARE_DMA_FIFO;                    // Local fifo var declare (ULONG nvDmaCount, * nvDmaFifo;)

    DISPDBG((9, "S32M8: %d (%d*%d) %s src:%s %s (%d,%d), dst:%s %s (%d,%d %d*%d)",
        c, parcl->right-parcl->left,parcl->bottom-parcl->top,
        pxlo?szDbg_XLATEOBJflXlate(pxlo->flXlate):"XO_trivial",
        psoSrc?szDbg_SURFOBJiBitmapFormat(psoSrc->iBitmapFormat):"NULL", szDbg_SURFOBJ_surf(psoSrc), pptlSrc->x, pptlSrc->y, 
        psoDst?szDbg_SURFOBJiBitmapFormat(psoDst->iBitmapFormat):"NULL", szDbg_SURFOBJ_surf(psoDst), prclDst->left, prclDst->top, prclDst->right-prclDst->left, prclDst->bottom-prclDst->top
        ));


    //**************************************************************************
    // Initialize notifier pointer to second notification structure
    //**************************************************************************
    
    pNotifier = (NvNotification *) (&(ppdev->Notifiers->MemToMem[NV039_NOTIFIERS_BUFFER_NOTIFY*sizeof(NvNotification)]));
        
    //**************************************************************************
    // Get push buffer global information (from pdev) into local variables
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;            // local fifo var = pdev fifo var;
                
    
    //**************************************************************************
    // Validate parameters first
    //**************************************************************************

    ASSERTDD( ppdev != NULL,
              "NV4ScreenToMem32to8bppBlt: NULL PDEV supplied !" );

    ASSERTDD( c > 0,
              "NV4ScreenToMem32to8bppBlt: Can't handle zero rectangles" ); // ok

    ASSERTDD( parcl != NULL,
              "NV4ScreenToMem32to8bppBlt: NULL destination rectangles supplied !" );
    
    ASSERTDD( psoSrc != NULL,
              "NV4ScreenToMem32to8bppBlt: NULL source surface supplied !" );

    ASSERTDD( psoDst != NULL,
              "NV4ScreenToMem32to8bppBlt: NULL destination surface supplied !" );
    
    ASSERTDD( pptlSrc != NULL,
              "NV4ScreenToMem32to8bppBlt: NULL source upper left point supplied !" );

    ASSERTDD( prclDst != NULL,
              "NV4ScreenToMem32to8bppBlt: NULL destination rectangle supplied !" );

    ASSERTDD( (psoDst->iBitmapFormat == BMF_8BPP),
              "NV4ScreenToMem32to8bppBlt: Dst bitmap format must be 8bpp" );

    ASSERTDD( (psoSrc->iBitmapFormat == BMF_32BPP),
              "NV4ScreenToMem32to8bppBlt: Src bitmap format must be 32bpp" );

    ASSERTDD( pxlo != NULL,
              "NV4ScreenToMem32to8bppBlt: NULL translation object");

    

#if DBG // we dont put asserts in if DBG, the actual macro definition, ASSERTDD is omited when DBG is not defined.
    
    DstBits                = (ULONG *) psoDst->pvBits;
    SrcBits                = (ULONG *) psoSrc->pvBits;
    ScreenBitmap           = (ULONG *) ppdev->pjScreen;
    pdsurfDst              = (DSURF *) psoDst->dhsurf;
    pdsurfSrc              = (DSURF *) psoSrc->dhsurf;
    
#endif         
    
    // Assert that this is a VRAM to SYSRAM blit ( we wont check it with an if, this
    // function is solely made for this case anyway ) 

    ASSERTDD( 
              //SRC is VRAM:       
              (
                ( SrcBits == ScreenBitmap) ||              
                ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN) )
              ) &&
              
              //DEST is SYSRAM:
              (
                (DstBits != ScreenBitmap) &&
                ( (pdsurfDst == NULL) || (pdsurfDst->dt == DT_DIB) )
              ),

              "NV4ScreenToMem32to8bppBlt: Expected a VRAM to SYSRAM blit !"
             
            );

    
    //**************************************************************************
    // Set memory to memory object in spare subchannel
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(2)));

    if (ppdev->dDrawSpareSubchannelObject != MY_MEMORY_TO_MEMORY) {
        
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), MY_MEMORY_TO_MEMORY);
        ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;
    }


    //**************************************************************************
    // Check if source bitmap (from video mem) is offscreen or onscreen
    // and set pitch and src ptr from the appropriate descriptive structure
    //**************************************************************************

    if ( psoSrc->dhsurf != NULL )               // The DSURF obj is not NULL, so
                                                // this srfc is device-managed
                                                // so it is in VRAM, offscreen
        {
        lSrcPitch  = ( (DSURF *) psoSrc->dhsurf )->LinearStride;
        pjSrcBits  = ( (DSURF *) psoSrc->dhsurf )->LinearPtr;
        }
    else
        { // Onscreen (primary surface, GDI managed)
        
        lSrcPitch   = ppdev->lDelta;
        pjSrcBits   = ppdev->pjScreen;
        }
    
    lDstPitch = psoDst->lDelta ;  // Also set the final dest. surface pitch

    
    // Set the offset (relative to the beginning of the dma mem2mem buffer)
    // of the middle of the mem2mem buffer
    // (See further, we use this as the address of the 2nd part of the mem2mem
    // buffer - one part is being written to, while one part is being read from)
     
    DstDMA2ndPartOffset = (ppdev->MemToMemBufferSize >> 1) ;
    
    // Also get the start and middle adress of the mem2membuffer

    pulDmaRead_start   = (ULONG *)( ppdev->pMemToMemBuffer  );
    pulDmaRead_2ndPart = (ULONG *)( (BYTE*)(ppdev->pMemToMemBuffer) + (ppdev->MemToMemBufferSize >> 1) );
            

    /***********************************************************************/
    /*                                                                     */
    /*  We are ready to start the transfer                                 */
    /*                                                                     */
    /***********************************************************************/
    
    pulDmaRead = pulDmaRead_start;

    ScreenToMem8bpp(pulDmaRead, (ULONG*));
    
    return TRUE;
}



BOOL NV4ScreenToMem16to4bppBlt(

    PDEV*       ppdev,              // PDEV associated with the SRC device
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      parcl,              // Array of destination rectangles (inside prclDst)
    SURFOBJ*    psoSrc,             // Source surface
    SURFOBJ*    psoDst,             // Dest surface
    POINTL*     pptlSrc,            // Upper left corner of SRC retcl in SRC surface
    RECTL*      prclDst,            // Rectangle to be modified in the DST surface
    XLATEOBJ*   pxlo)               // Provides color translation info
    
{
    
#if DBG        // only allocate those for convenience (for the next assert) on checked builds

    ULONG*      DstBits;
    ULONG*      SrcBits;
    ULONG*      ScreenBitmap;
    DSURF*      pdsurfDst;
    DSURF*      pdsurfSrc;   

#endif

#ifdef  CALIBRATION_ENABLED
    
    ULONG       tempUlong;
    BYTE        msbitPos;

#endif
    

    LONG            i;
    ULONG           j;
    
    BYTE*           limit;

    ULONG           width,height;

    ULONG           linesToGo;           // Total number of lines left to Xfer

    ULONG           bytes2Xfer;          // Total amount of bytes to Xfer for current rectangle
    
    ULONG           bytesPerLine;        // Width of one line of the current rectangle, in bytes

    ULONG           optBytesPerXfer;     // Optimal bytes per Xfer (calculated based on mathematic model 
                                         // and profiling data, see notes)
    ULONG           linesPerXfer;        // Number of lines to transfer by Xfer (based on bytesPerLine and OptBytesPerXfer)

    ULONG           DstDMAincPerXfer;    // linesPerXfer * bytesPerline
    ULONG           SrcDMAincPerXfer;    // (lSrcPitch+bytesPerLine)*linesPerXfer

    LONG            lSrcPitch;           // Pitch of the source surface
    
    LONG            lDstPitch;           // Pitch of the final dest surface

    LONG            FinalDstJump2NextLineBeg; // DstPitch - byterperline (see where it is used)

    BYTE*           pjSrcBits;            // Pointer to the beginning of the source bits (for the Xfer)
    BYTE*           pjSrcRclBits;         // Pointer to the beginning of the source cliped rectangle bits

    BYTE*           pjFinalDstWrite;      // Pointer to the beg. of the dest. bits (in the final surface)
                                          // (will also be incremented to advance in the destination bits during xfer)
    
    //ULONG*          pulDmaRead;         // Pointer in the mem2mem dma buffer that we are going to use in the
                                          // CPU-Xfers to keep track of where we are currently reading 
    
    USHORT*         psDmaRead;
    //BYTE*           pjDmaRead;
    
    BYTE            XlatedByte, TempByte;

    ULONG*          pulDmaRead_lastXfer; // Var to hold where to read the last CPU-Xfer

    ULONG*          pulDmaRead_start;    // Start of the Mem2Mem buffer

    ULONG*          pulDmaRead_2ndPart;  // Address of 2nd part of Mem2Mem buffer

    POINTL          ptlSrcRegion;        // Left top corner of current clipped region/rectangle in Src surface
                                         // to xfer
    
    ULONG           DstDMAOffset;        // Offset (relative to DMA base)
                                         // of the destination bytes in the dst DMA region
    ULONG           SrcDMAOffset;        // Offset of the source bytes in the src DMA region
    
    ULONG           DstDMA2ndPartOffset; // Offset of the 2nd part of the Mem2Mem buffer
       
    BYTE            leftRest, rightRest; // Rest of ( %(modulo) 2 ) of the left and right coordinates 

    LONG            middleWidth;         // Number of bytes to process with the "middle" loop (when Dest == 4 bpp)
    
    NV_XLATE_INIT                        // Initialize cached xlate values
    NvNotification* pNotifier;           // Pointer to MemToMem notifier array
    DECLARE_DMA_FIFO;                    // Local fifo var declare (ULONG nvDmaCount, * nvDmaFifo;)
                                          

    //**************************************************************************
    // Initialize notifier pointer to second notification structure
    //**************************************************************************
    
    pNotifier = (NvNotification *) (&(ppdev->Notifiers->MemToMem[NV039_NOTIFIERS_BUFFER_NOTIFY*sizeof(NvNotification)]));
        
    //**************************************************************************
    // Get push buffer global information (from pdev) into local variables
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;            // local fifo var = pdev fifo var;
    
    //**************************************************************************
    // Validate parameters first
    //**************************************************************************

    ASSERTDD( ppdev != NULL,
              "NV4ScreenToMem16to4bppBlt: NULL PDEV supplied !" );

    ASSERTDD( c > 0,
              "NV4ScreenToMem16to4bppBlt: Can't handle zero rectangles" ); // ok

    ASSERTDD( parcl != NULL,
              "NV4ScreenToMem16to4bppBlt: NULL destination rectangles supplied !" );
    
    ASSERTDD( psoSrc != NULL,
              "NV4ScreenToMem16to4bppBlt: NULL source surface supplied !" );

    ASSERTDD( psoDst != NULL,
              "NV4ScreenToMem16to4bppBlt: NULL destination surface supplied !" );
    
    ASSERTDD( pptlSrc != NULL,
              "NV4ScreenToMem16to4bppBlt: NULL source upper left point supplied !" );

    ASSERTDD( prclDst != NULL,
              "NV4ScreenToMem16to4bppBlt: NULL destination rectangle supplied !" );

    ASSERTDD( (psoDst->iBitmapFormat == BMF_4BPP),
              "NV4ScreenToMem16to4bppBlt: Dst bitmap format must be 4bpp" );

    ASSERTDD( (psoSrc->iBitmapFormat == BMF_16BPP),
              "NV4ScreenToMem16to4bppBlt: Src bitmap format must be 16bpp" );

    ASSERTDD( pxlo != NULL,
              "NV4ScreenToMem16to4bppBlt: NULL translation object");

    

#if DBG // we dont put asserts in if DBG, the actual macro definition, ASSERTDD is omited when DBG is not defined.
    
    DstBits                = (ULONG *) psoDst->pvBits;
    SrcBits                = (ULONG *) psoSrc->pvBits;
    ScreenBitmap           = (ULONG *) ppdev->pjScreen;
    pdsurfDst              = (DSURF *) psoDst->dhsurf;
    pdsurfSrc              = (DSURF *) psoSrc->dhsurf;
    
#endif         
    
    // Assert that this is a VRAM to SYSRAM blit ( we wont check it with an if, this
    // function is solely made for this case anyway ) 

    ASSERTDD( 
              //SRC is VRAM:       
              (
                ( SrcBits == ScreenBitmap) ||              
                ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN) )
              ) &&
              
              //DEST is SYSRAM:
              (
                (DstBits != ScreenBitmap) &&
                ( (pdsurfDst == NULL) || (pdsurfDst->dt == DT_DIB) )
              ),

              "NV4ScreenToMem16to4bppBlt: Expected a VRAM to SYSRAM blit !"
             
            );

    
    //**************************************************************************
    // Set memory to memory object in spare subchannel
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(2)));

    if (ppdev->dDrawSpareSubchannelObject != MY_MEMORY_TO_MEMORY) {
        
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), MY_MEMORY_TO_MEMORY);
        ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;
    }


    //**************************************************************************
    // Check if source bitmap (from video mem) is offscreen or onscreen
    // and set pitch and src ptr from the appropriate descriptive structure
    //**************************************************************************

    if ( psoSrc->dhsurf != NULL )               // The DSURF obj is not NULL, so
                                                // this srfc is device-managed
                                                // so it is in VRAM, offscreen
        {
        lSrcPitch  = ( (DSURF *) psoSrc->dhsurf )->LinearStride;
        pjSrcBits  = ( (DSURF *) psoSrc->dhsurf )->LinearPtr;
        }
    else
        { // Onscreen (primary surface, GDI managed)
        
        lSrcPitch   = ppdev->lDelta;
        pjSrcBits   = ppdev->pjScreen;
        }
    
    lDstPitch = psoDst->lDelta ;  // Also set the final dest. surface pitch

    
    // Set the offset (relative to the beginning of the dma mem2mem buffer)
    // of the middle of the mem2mem buffer
    // (See further, we use this as the address of the 2nd part of the mem2mem
    // buffer - one part is being written to, while one part is being read from)
     
    DstDMA2ndPartOffset = (ppdev->MemToMemBufferSize >> 1) ;
    
    // Also get the start and middle adress of the mem2membuffer

    pulDmaRead_start   = (ULONG *)( ppdev->pMemToMemBuffer  );
    pulDmaRead_2ndPart = (ULONG *)( (BYTE*)(ppdev->pMemToMemBuffer) + (ppdev->MemToMemBufferSize >> 1) );
            

    /***********************************************************************/
    /*                                                                     */
    /*  We are ready to start the transfer                                 */
    /*                                                                     */
    /***********************************************************************/
    
    psDmaRead = (USHORT*) pulDmaRead_start;

    ScreenToMem4bpp(psDmaRead, (USHORT*));
    
    return TRUE;
}



BOOL NV4ScreenToMem16to8bppBlt(

    PDEV*       ppdev,              // PDEV associated with the SRC device
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      parcl,              // Array of destination rectangles (inside prclDst)
    SURFOBJ*    psoSrc,             // Source surface
    SURFOBJ*    psoDst,             // Dest surface
    POINTL*     pptlSrc,            // Upper left corner of SRC retcl in SRC surface
    RECTL*      prclDst,            // Rectangle to be modified in the DST surface
    XLATEOBJ*   pxlo)               // Provides color translation info
    
{
    
#if DBG        // only allocate those for convenience (for the next assert) on checked builds

    ULONG*      DstBits;
    ULONG*      SrcBits;
    ULONG*      ScreenBitmap;
    DSURF*      pdsurfDst;
    DSURF*      pdsurfSrc;   

#endif

#ifdef  CALIBRATION_ENABLED
    
    ULONG       tempUlong;
    BYTE        msbitPos;

#endif
    

    LONG            i;
    ULONG           j;
    
    BYTE*           limit;

    ULONG           width,height;

    ULONG           linesToGo;           // Total number of lines left to Xfer

    ULONG           bytes2Xfer;          // Total amount of bytes to Xfer for current rectangle
    
    ULONG           bytesPerLine;        // Width of one line of the current rectangle, in bytes

    ULONG           optBytesPerXfer;     // Optimal bytes per Xfer (calculated based on mathematic model 
                                         // and profiling data, see notes)
    ULONG           linesPerXfer;        // Number of lines to transfer by Xfer (based on bytesPerLine and OptBytesPerXfer)

    ULONG           DstDMAincPerXfer;    // linesPerXfer * bytesPerline
    ULONG           SrcDMAincPerXfer;    // (lSrcPitch+bytesPerLine)*linesPerXfer

    LONG            lSrcPitch;           // Pitch of the source surface
    
    LONG            lDstPitch;           // Pitch of the final dest surface

    LONG            FinalDstJump2NextLineBeg; // DstPitch - byterperline (see where it is used)

    BYTE*           pjSrcBits;           // Pointer to the beginning of the source bits (for the Xfer)
    BYTE*           pjSrcRclBits;        // Pointer to the beginning of the source cliped rectangle bits

    BYTE*           pjFinalDstWrite;     // Pointer to the beg. of the dest. bits (in the final surface)
                                         // (will also be incremented to advance in the destination bits during xfer)
    
    //ULONG*          pulDmaRead;        // Pointer in the mem2mem dma buffer that we are going to use in the
                                         // CPU-Xfers to keep track of where we are currently reading 
    
    USHORT*         psDmaRead;
    //BYTE*           pjDmaRead;
    
    BYTE            XlatedByte, TempByte;

    ULONG*          pulDmaRead_lastXfer; // Var to hold where to read the last CPU-Xfer

    ULONG*          pulDmaRead_start;    // Start of the Mem2Mem buffer

    ULONG*          pulDmaRead_2ndPart;  // Address of 2nd part of Mem2Mem buffer

    POINTL          ptlSrcRegion;        // Left top corner of current clipped region/rectangle in Src surface
                                         // to xfer
    
    ULONG           DstDMAOffset;        // Offset (relative to DMA base)
                                         // of the destination bytes in the dst DMA region
    ULONG           SrcDMAOffset;        // Offset of the source bytes in the src DMA region
    
    ULONG           DstDMA2ndPartOffset; // Offset of the 2nd part of the Mem2Mem buffer
       
    BYTE            leftRest, rightRest; // Rest of ( %(modulo) 2 ) of the left and right coordinates 

    LONG            middleWidth;         // Number of bytes to process with the "middle" loop (when Dest == 4 bpp)
    
    NV_XLATE_INIT                        // Initialize cached xlate values
    NvNotification* pNotifier;           // Pointer to MemToMem notifier array
    DECLARE_DMA_FIFO;                    // Local fifo var declare (ULONG nvDmaCount, * nvDmaFifo;)
                                          

    //**************************************************************************
    // Initialize notifier pointer to second notification structure
    //**************************************************************************
    
    pNotifier = (NvNotification *) (&(ppdev->Notifiers->MemToMem[NV039_NOTIFIERS_BUFFER_NOTIFY*sizeof(NvNotification)]));
        
    //**************************************************************************
    // Get push buffer global information (from pdev) into local variables
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;            // local fifo var = pdev fifo var;
                
    
    //**************************************************************************
    // Validate parameters first
    //**************************************************************************

    ASSERTDD( ppdev != NULL,
              "NV4ScreenToMem16to8bppBlt: NULL PDEV supplied !" );

    ASSERTDD( c > 0,
              "NV4ScreenToMem16to8bppBlt: Can't handle zero rectangles" ); // ok

    ASSERTDD( parcl != NULL,
              "NV4ScreenToMem16to8bppBlt: NULL destination rectangles supplied !" );
    
    ASSERTDD( psoSrc != NULL,
              "NV4ScreenToMem16to8bppBlt: NULL source surface supplied !" );

    ASSERTDD( psoDst != NULL,
              "NV4ScreenToMem16to8bppBlt: NULL destination surface supplied !" );
    
    ASSERTDD( pptlSrc != NULL,
              "NV4ScreenToMem16to8bppBlt: NULL source upper left point supplied !" );

    ASSERTDD( prclDst != NULL,
              "NV4ScreenToMem16to8bppBlt: NULL destination rectangle supplied !" );

    ASSERTDD( (psoDst->iBitmapFormat == BMF_8BPP),
              "NV4ScreenToMem16to8bppBlt: Dst bitmap format must be 8bpp" );

    ASSERTDD( (psoSrc->iBitmapFormat == BMF_16BPP),
              "NV4ScreenToMem16to8bppBlt: Src bitmap format must be 16bpp" );

    ASSERTDD( pxlo != NULL,
              "NV4ScreenToMem16to8bppBlt: NULL translation object");

    

#if DBG // we dont put asserts in if DBG, the actual macro definition, ASSERTDD is omited when DBG is not defined.
    
    DstBits                = (ULONG *) psoDst->pvBits;
    SrcBits                = (ULONG *) psoSrc->pvBits;
    ScreenBitmap           = (ULONG *) ppdev->pjScreen;
    pdsurfDst              = (DSURF *) psoDst->dhsurf;
    pdsurfSrc              = (DSURF *) psoSrc->dhsurf;
    
#endif         
    
    // Assert that this is a VRAM to SYSRAM blit ( we wont check it with an if, this
    // function is solely made for this case anyway ) 

    ASSERTDD( 
              //SRC is VRAM:       
              (
                ( SrcBits == ScreenBitmap) ||              
                ( (pdsurfSrc != NULL) && (pdsurfSrc->dt == DT_SCREEN) )
              ) &&
              
              //DEST is SYSRAM:
              (
                (DstBits != ScreenBitmap) &&
                ( (pdsurfDst == NULL) || (pdsurfDst->dt == DT_DIB) )
              ),

              "NV4ScreenToMem16to8bppBlt: Expected a VRAM to SYSRAM blit !"
             
            );

    
    //**************************************************************************
    // Set memory to memory object in spare subchannel
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(2)));

    if (ppdev->dDrawSpareSubchannelObject != MY_MEMORY_TO_MEMORY) {
        
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), MY_MEMORY_TO_MEMORY);
        ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;
    }


    //**************************************************************************
    // Check if source bitmap (from video mem) is offscreen or onscreen
    // and set pitch and src ptr from the appropriate descriptive structure
    //**************************************************************************

    if ( psoSrc->dhsurf != NULL )               // The DSURF obj is not NULL, so
                                                // this srfc is device-managed
                                                // so it is in VRAM, offscreen
        {
        lSrcPitch  = ( (DSURF *) psoSrc->dhsurf )->LinearStride;
        pjSrcBits  = ( (DSURF *) psoSrc->dhsurf )->LinearPtr;
        }
    else
        { // Onscreen (primary surface, GDI managed)
        
        lSrcPitch   = ppdev->lDelta;
        pjSrcBits   = ppdev->pjScreen;
        }
    
    lDstPitch = psoDst->lDelta ;  // Also set the final dest. surface pitch

    
    // Set the offset (relative to the beginning of the dma mem2mem buffer)
    // of the middle of the mem2mem buffer
    // (See further, we use this as the address of the 2nd part of the mem2mem
    // buffer - one part is being written to, while one part is being read from)
     
    DstDMA2ndPartOffset = (ppdev->MemToMemBufferSize >> 1) ;
    
    // Also get the start and middle adress of the mem2membuffer

    pulDmaRead_start   = (ULONG *)( ppdev->pMemToMemBuffer  );
    pulDmaRead_2ndPart = (ULONG *)( (BYTE*)(ppdev->pMemToMemBuffer) + (ppdev->MemToMemBufferSize >> 1) );
            

    /***********************************************************************/
    /*                                                                     */
    /*  We are ready to start the transfer                                 */
    /*                                                                     */
    /***********************************************************************/
    
    psDmaRead = (USHORT*) pulDmaRead_start;

    ScreenToMem8bpp(psDmaRead, (USHORT*));
    
    return TRUE;
}


//******************************************************************************
//  ------------------------------------------------------------------
//              END OF NV4ScreenToMem*to*bppBlt FUNCTIONS
//  ------------------------------------------------------------------
//******************************************************************************

    
    
//******************************Public*Routine**********************************
//
// Function: NV4DmaPushDMAGetScreenBits
//
// Routine Description:
//
// Arguments:
//
//       ppdev - Pointer to the physical device structure
//
// Return Value:
//
//******************************************************************************


VOID NV4DmaPushDMAGetScreenBits(
PDEV*       ppdev,
DSURF*      pdsurfSrc,
SURFOBJ*    psoDst,
RECTL*      prclDst,
POINTL*     pptlSrc)        // Absolute coordinates!

    {
    LONG            cyScan;
    LONG            lDstDelta;
    LONG            lSrcDelta;
    BYTE*           pjDst;
    BYTE*           pjSrc;
    LONG            cjScan;
    LONG            width,height;
    RECTL           rclSrc;
    ULONG           OffsetFromSource;
    ULONG           DstOffset;
    ULONG           SrcOffset;
    NvNotification* pNotifier;

    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Initialize notifier pointer to second notification structure
    //**************************************************************************
    
    pNotifier = (NvNotification *) (&(ppdev->Notifiers->MemToMem[NV039_NOTIFIERS_BUFFER_NOTIFY*sizeof(NvNotification)]));
        
    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Setup the 'source' rectangle to read from
    // Note, the prclDst that is passed into here has already
    // been derived from the clip rectangle (no clipping necessary in this function)
    //**************************************************************************

    rclSrc.left   = pptlSrc->x;
    rclSrc.top    = pptlSrc->y;
    rclSrc.right  = pptlSrc->x + (prclDst->right  - prclDst->left);
    rclSrc.bottom = pptlSrc->y + (prclDst->bottom - prclDst->top);

    //**************************************************************************
    // Get width and height of source rectangle
    //**************************************************************************

    width  = (rclSrc.right  - rclSrc.left);
    height = (rclSrc.bottom - rclSrc.top);


    //**************************************************************************
    // Check if source bitmap (from video mem) is offscreen or onscreen
    //**************************************************************************

    if (pdsurfSrc != NULL)
        {
        lSrcDelta  = pdsurfSrc->LinearStride;
        pjSrc      = pdsurfSrc->LinearPtr;
        }
    else
        {
        lSrcDelta   = ppdev->lDelta;
        pjSrc       = ppdev->pjScreen;
        }

    //**************************************************************************
    // Calculate the pointer to the upper-left corner of Source rectangle 
    //**************************************************************************

    OffsetFromSource = ( (rclSrc.top  * lSrcDelta) + CONVERT_TO_BYTES(rclSrc.left, ppdev)) ;   
    pjSrc += OffsetFromSource;
                
    //**************************************************************************
    // Calculate the pointer to the upper-left corner of Destination rectangle 
    //**************************************************************************

    lDstDelta = psoDst->lDelta;
    pjDst     = (BYTE*) psoDst->pvScan0 + (prclDst->top  * lDstDelta)
                                          + CONVERT_TO_BYTES(prclDst->left, ppdev);
                                          
    //**************************************************************************
    // Calculate number of scanlines to copy (cyScan), and
    // Calculate number of bytes to copy for each scanline (cjScan)
    //**************************************************************************

    cjScan = CONVERT_TO_BYTES(width, ppdev);
    cyScan = height;

    //**************************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************************

    NV_DMAPUSH_CHECKFREE(((ULONG)(11)));  

    //**************************************************************************
    // Calculate the offset from the beginning of the DMA Buffer Base
    //**************************************************************************

    DstOffset = (ULONG)(pjDst - (BYTE *)ppdev->pMemToMemBuffer);
    SrcOffset = (ULONG)(pjSrc - ppdev->pjFrameBufbase);

    //**************************************************************************
    // Set memory to memory object in spare subchannel
    //**************************************************************************

    if (ppdev->dDrawSpareSubchannelObject != MY_MEMORY_TO_MEMORY)
        {
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), MY_MEMORY_TO_MEMORY);
        ppdev->dDrawSpareSubchannelObject = MY_MEMORY_TO_MEMORY;
        }

    //**********************************************************************
    // Set methods for memory_to_memory dma blit
    //   NV039_OFFSET_IN                                            
    //   NV039_OFFSET_OUT                                           
    //   NV039_PITCH_IN                                             
    //   NV039_PITCH_OUT                                            
    //   NV039_LINE_LENGTH_IN                                       
    //   NV039_LINE_COUNT                                           
    //   NV039_FORMAT                                               
    //   NV039_BUFFER_NOTIFY                                        
    //**********************************************************************

    //**************************************************************************
    // Set notifier to 'BUSY' value (use second notification structure)
    //**************************************************************************

    pNotifier->status = NV039_NOTIFICATION_STATUS_IN_PROGRESS;

    NV_DMAPUSH_START(8,DD_SPARE , NV039_OFFSET_IN );
    NV_DMA_FIFO = SrcOffset;        // Offset In
    NV_DMA_FIFO = DstOffset;        // Offset Out
    NV_DMA_FIFO = lSrcDelta;        // Pitch In
    NV_DMA_FIFO = lDstDelta;        // Pitch Out
    NV_DMA_FIFO = cjScan;           // Line Length In
    NV_DMA_FIFO = cyScan;           // Line Count
    NV_DMA_FIFO = NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1; // Format
    NV_DMA_FIFO = 0;                // Set Buffer Notify and kickoff

    //**************************************************************************
    // Update global push buffer count
    //**************************************************************************

    UPDATE_PDEV_DMA_COUNT;

    //**************************************************************************
    // Send data on thru to the DMA push buffer
    //**************************************************************************

    NV4_DmaPushSend(ppdev);

    //**************************************************************************
    // Wait for DMA to be completed (use second notification structure)
    //**************************************************************************

    while ( pNotifier->status != 0 );
    

    return;
    }

//******************************************************************************
//
//  Function:   NV4DmaPushXfer8to32bpp
//
//  Routine Description: Does 8bpp to 32bpp translation and puts the translated
//      image in the DMA push buffer. Does not use indexed image.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV4DmaPushXfer8to32bpp(     // Type FNXFER
PDEV*       ppdev,
LONG        c,                  // Count of rectangles, can't be zero
RECTL*      prcl,               // Array of relative coordinates destination rectangles
ROP4        rop4,               // rop4
SURFOBJ*    psoSrc,             // Source surface
POINTL*     pptlSrc,            // Original unclipped source point
RECTL*      prclDst,            // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG       PatternColor,       // Solid Color of pattern - NOT USED
BLENDOBJ*   pBlendObj)

    {
    ULONG   *pulXlate;
    BYTE    *SrcBits;
    LONG    BytesPerSrcScan;
    ULONG   Rop3;
    LONG    xsrc,ysrc;
    BYTE    *NextScan;
    ULONG   yscan;
    BYTE    *SrcPtr;
    ULONG   DwordCount;
    LONG    clip_x,clip_y;
    ULONG   clip_width,clip_height;
    ULONG   i;

    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    //**************************************************************************

    pulXlate        = pxlo->pulXlate;
    SrcBits         = psoSrc->pvScan0;
    BytesPerSrcScan = psoSrc->lDelta;

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
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(3)));  

        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

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

        clip_width = prcl->right - clip_x;
        clip_height = prcl->bottom - clip_y;

        //**********************************************************************
        // Top left coordinate of source bitmap on source surface
        //**********************************************************************

        xsrc = pptlSrc->x + clip_x - prclDst->left;
        ysrc = pptlSrc->y + clip_y - prclDst->top;

        NextScan = (&(SrcBits[(LONG)(xsrc + (ysrc*BytesPerSrcScan))]));

        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  

        //**********************************************************************
        // Initialize the ImageFromCpu object
        //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_POINT     
        //     NV061_SIZE_OUT  
        //     NV061_SIZE_IN 
        //**********************************************************************

        NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
        NV_DMA_FIFO = ( (clip_y <<16) | (clip_x & 0xffff)  );
        NV_DMA_FIFO = ( (clip_height <<16) | clip_width  );
        NV_DMA_FIFO = ( (clip_height <<16) | clip_width  );

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

        for (yscan=0; yscan < clip_height; yscan++)
            {

            DwordCount = clip_width;
            SrcPtr = NextScan;

            //**************************************************************
            // Check if we've got enough room in the push buffer
            //**************************************************************

            NV_DMAPUSH_CHECKFREE(((ULONG)(DwordCount+2)));  

            if (DwordCount >= MAX_INDEXED_IMAGE_DWORDS)

                {
                NV_DMAPUSH_START(MAX_INDEXED_IMAGE_DWORDS, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );
    
                //**************************************************************
                // Set the following methods for IMAGE_FROM_CPU
                //     NV061_COLOR(0)
                //     NV061_COLOR(1)
                //     NV061_COLOR(2)
                //      ...
                //**************************************************************

                for (i=0; i < MAX_INDEXED_IMAGE_DWORDS; i++)
                    {
                    NV_DMA_FIFO = pulXlate[*SrcPtr++];    
                    }
                DwordCount -= MAX_INDEXED_IMAGE_DWORDS;
                }

            if (DwordCount>0)
                {
                
                NV_DMAPUSH_START(DwordCount, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );
    
                //**************************************************************
                // Set the following methods for IMAGE_FROM_CPU
                //     NV061_COLOR(0)
                //     NV061_COLOR(1)
                //     NV061_COLOR(2)
                //      ...
                //**************************************************************

                while (DwordCount > 0)
                    {
                    NV_DMA_FIFO = pulXlate[*SrcPtr++];
                    DwordCount--;
                    }
                }

            NextScan+=BytesPerSrcScan;

            //******************************************************************
            // Kickoff buffer after each scanline
            //******************************************************************

            UPDATE_PDEV_DMA_COUNT;
            NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
            UPDATE_LOCAL_DMA_COUNT;

            }

        if (--c == 0)
            return;

        prcl++;

        } // while (TRUE)...

    }

//******************************************************************************
//
//  Function:   NV4DmaPushXfer4to32bpp
//
//  Routine Description: Does 4bpp to 32bpp translation and puts the translated
//      image in the DMA push buffer. Does not use indexed image. 
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV4DmaPushXfer4to32bpp(     // Type FNXFER
PDEV*       ppdev,
LONG        c,                  // Count of rectangles, can't be zero
RECTL*      prcl,               // Array of relative coordinates destination rectangles
ROP4        rop4,               // rop4
SURFOBJ*    psoSrc,             // Source surface
POINTL*     pptlSrc,            // Original unclipped source point
RECTL*      prclDst,            // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG       PatternColor,       // Solid Color of pattern - NOT USED
BLENDOBJ*   pBlendObj)

    {
    ULONG   *pulXlate;
    BYTE    *SrcBits;
    LONG    BytesPerSrcScan;
    ULONG   Rop3;
    LONG    xsrc,ysrc;
    BYTE    *NextScan;
    ULONG   yscan;
    BYTE    *SrcPtr;
    ULONG   DwordCount;
    LONG    clip_x,clip_y;
    ULONG   clip_width,clip_height;
    ULONG   i;

    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    //**************************************************************************

    pulXlate        = pxlo->pulXlate;
    SrcBits         = psoSrc->pvScan0;
    BytesPerSrcScan = psoSrc->lDelta;

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
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(3)));  

        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

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

        clip_width = prcl->right - clip_x;
        clip_height = prcl->bottom - clip_y;

        //**********************************************************************
        // Top left coordinate of source bitmap on source surface
        //**********************************************************************

        xsrc = pptlSrc->x + clip_x - prclDst->left;
        ysrc = pptlSrc->y + clip_y - prclDst->top;

        //**********************************************************************
        // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
        // ignore/clip the extra pixel that we send it, so that it won't
        // get included as part of the next scanline. Since we always
        // send DWords at a time, we may actually send more data to the NV
        // engine than is necessary.  So clip it by specifying a larger SizeIn
        // That is, we always send DWORDS at a time for each scanline.
        // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
        //**********************************************************************

        NextScan = &(SrcBits[(LONG)(((xsrc)>>1) + (ysrc*BytesPerSrcScan))]);

        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  

        //**********************************************************************
        // Initialize the ImageFromCpu object
        //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_POINT     
        //     NV061_SIZE_OUT  
        //     NV061_SIZE_IN 
        //**********************************************************************

        NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
        NV_DMA_FIFO = ((clip_y <<16) | (clip_x & 0xffff));
        NV_DMA_FIFO = ((clip_height <<16) | clip_width);
        NV_DMA_FIFO = ((clip_height <<16) | clip_width);

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

        for (yscan=0; yscan < clip_height; yscan++)
            {
            DwordCount = clip_width;
            SrcPtr = NextScan;

            NV_DMAPUSH_CHECKFREE(((ULONG)(DwordCount+4)));  
            
            if (xsrc & 1)
                {
                NV_DMAPUSH_START(1, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );
                NV_DMA_FIFO = pulXlate[(*(SrcPtr)) & 0xf];
                DwordCount--;
                SrcPtr++;
                }

            if (DwordCount >= MAX_INDEXED_IMAGE_DWORDS)

                {
            //**************************************************************
            // Check if we've got enough room in the push buffer
            //**************************************************************

                NV_DMAPUSH_START(MAX_INDEXED_IMAGE_DWORDS, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //**************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //**************************************************************

                for (i=0; i < ((MAX_INDEXED_IMAGE_DWORDS)/2) ; i++)
                    {
                    NV_DMA_FIFO = pulXlate[(*SrcPtr) >> 4];    
                    NV_DMA_FIFO = pulXlate[(*SrcPtr) & 0xf];
                    SrcPtr++;
                    }
                DwordCount -= MAX_INDEXED_IMAGE_DWORDS;
                }

            if (DwordCount>0)
                {
            
            //**************************************************************
            // Check if we've got enough room in the push buffer
            //**************************************************************

                NV_DMAPUSH_START(DwordCount, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //**************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //**************************************************************

                while (DwordCount >= 2)
                    {
                    NV_DMA_FIFO = pulXlate[(*SrcPtr) >> 4];    
                    NV_DMA_FIFO = pulXlate[(*SrcPtr) & 0xf];
                    SrcPtr++;
                    DwordCount -= 2;
                    }

                if (DwordCount > 0)
                    {
                    NV_DMA_FIFO = pulXlate[(*SrcPtr) >> 4];
                    DwordCount--;
                    }
                }

            NextScan+=BytesPerSrcScan;

        //******************************************************************
        // Kickoff buffer after each scanline
        //******************************************************************

            UPDATE_PDEV_DMA_COUNT;
            NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
            UPDATE_LOCAL_DMA_COUNT;

            }
        if (--c == 0)
            return;

        prcl++;

        } // while (TRUE)...
    }


//******************************************************************************
//
//  Function:   NV4DmaPushXfer8to16bpp
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


VOID NV4DmaPushXfer8to16bpp(     // Type FNXFER
PDEV*       ppdev,
LONG        c,                  // Count of rectangles, can't be zero
RECTL*      prcl,               // Array of relative coordinates destination rectangles
ROP4        rop4,               // rop4
SURFOBJ*    psoSrc,             // Source surface
POINTL*     pptlSrc,            // Original unclipped source point
RECTL*      prclDst,            // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG       PatternColor,       // Solid Color of pattern - NOT USED
BLENDOBJ*   pBlendObj
)
    {
    ULONG   *pulXlate;
    BYTE    *SrcBits;
    LONG    BytesPerSrcScan;
    ULONG   Rop3;
    ULONG   temp;
    LONG    xsrc,ysrc;
    BYTE    *NextScan;
    ULONG   yscan;
    BYTE    *SrcPtr;
    ULONG   WordCount;
    ULONG   ByteCount;
    LONG    clip_x,clip_y;
    ULONG   clip_width,clip_height;
    ULONG   i;
    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    //**************************************************************************

    pulXlate        = pxlo->pulXlate;
    SrcBits         = psoSrc->pvScan0;
    BytesPerSrcScan = psoSrc->lDelta;

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
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(3)));  

        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

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

        clip_width = prcl->right - clip_x;
        clip_height = prcl->bottom - clip_y;

        //**********************************************************************
        // Top left coordinate of source bitmap on source surface
        //**********************************************************************

        xsrc = pptlSrc->x + clip_x - prclDst->left;
        ysrc = pptlSrc->y + clip_y - prclDst->top;

        //**********************************************************************
        // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
        // ignore/clip the extra pixel that we send it, so that it won't
        // get included as part of the next scanline. Since we always
        // send DWords at a time, we may actually send more data to the NV
        // engine than is necessary.  So clip it by specifying a larger SizeIn
        // That is, we always send DWORDS at a time for each scanline.
        // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
        //**********************************************************************

        NextScan = (&(SrcBits[xsrc + (ysrc*BytesPerSrcScan)]));

        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  

        //**********************************************************************
        // Initialize the ImageFromCpu object
        //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_POINT     
        //     NV061_SIZE_OUT  
        //     NV061_SIZE_IN 
        //**********************************************************************

        NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
        NV_DMA_FIFO = ( (clip_y <<16) | (clip_x & 0xffff)  );
        NV_DMA_FIFO = ( (clip_height <<16) | clip_width  );
        NV_DMA_FIFO = ( (clip_height <<16) | (clip_width+1)&0xfffe);

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

        for (yscan=0; yscan < clip_height; yscan++)
            {

            ByteCount = clip_width;
            WordCount = ByteCount>>1;
            SrcPtr = NextScan;

            //**************************************************************
            // Check if we've got enough room in the push buffer
            //**************************************************************

            NV_DMAPUSH_CHECKFREE(((ULONG)((ByteCount+1)>>1)+1));  
            NV_DMAPUSH_START((ByteCount+1)>>1, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //**************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //**************************************************************

            while (WordCount > 0)
                {
                temp = pulXlate[*SrcPtr++];
                NV_DMA_FIFO = pulXlate[*SrcPtr++]<<16 | temp;
                WordCount--;
                }
                
            if (ByteCount&1)
                {
                temp = pulXlate[*SrcPtr];
                NV_DMA_FIFO = temp;
                }

            NextScan+=BytesPerSrcScan;

            //******************************************************************
            // Kickoff buffer after each scanline
            //******************************************************************

            UPDATE_PDEV_DMA_COUNT;
            NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
            UPDATE_LOCAL_DMA_COUNT;

            }

        if (--c == 0)
            return;

        prcl++;

        } // while (TRUE)...

    }


//******************************************************************************
//
//  Function:   NV4DmaPushXfer4to16bpp
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

VOID NV4DmaPushXfer4to16bpp(     // Type FNXFER
PDEV*       ppdev,
LONG        c,                  // Count of rectangles, can't be zero
RECTL*      prcl,               // Array of relative coordinates destination rectangles
ROP4        rop4,               // rop4
SURFOBJ*    psoSrc,             // Source surface
POINTL*     pptlSrc,            // Original unclipped source point
RECTL*      prclDst,            // Original unclipped destination rectangle
XLATEOBJ*   pxlo,
ULONG       PatternColor,       // Solid Color of pattern - NOT USED
BLENDOBJ*   pBlendObj
)
    {
    ULONG   *pulXlate;
    BYTE    *SrcBits;
    LONG    BytesPerSrcScan;
    ULONG   Rop3;
    ULONG   temp;
    LONG    xsrc,ysrc;
    BYTE    *NextScan;
    ULONG   yscan;
    BYTE    *SrcPtr;
    ULONG   ByteCount;
    LONG    clip_x,clip_y;
    ULONG   clip_width,clip_height;
    ULONG   i;
    DECLARE_DMA_FIFO;


    //**************************************************************************
    // Safety checks
    //**************************************************************************

    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    //**************************************************************************

    pulXlate        = pxlo->pulXlate;
    SrcBits         = psoSrc->pvScan0;
    BytesPerSrcScan = psoSrc->lDelta;

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

        clip_width = prcl->right - clip_x;
        clip_height = prcl->bottom - clip_y;

        //**********************************************************************
        // Top left coordinate of source bitmap on source surface
        //**********************************************************************

        xsrc = pptlSrc->x + clip_x - prclDst->left;
        ysrc = pptlSrc->y + clip_y - prclDst->top;

        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(9)));  

        //************************************************************************
        // Check if 4bpp bitmap starting x is odd. In this case, we will add 1 to
        // width and start drawing the bitmap as if it were even, and use hardware
        // clipping to take care of it.
        //************************************************************************
    
        if ((xsrc&1))
            {
        //*********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //*********************************************************************

       
            NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
            NV_DMA_FIFO = ((0 << 16) | clip_x);
            NV_DMA_FIFO = (((ppdev->cyMemory)<<16) | clip_width);

            ppdev->NVClipResetFlag=TRUE;
            clip_width++;
            clip_x--;
            xsrc--;
            }

        //********************************************************************
        // Reset clipping rectangle to full screen extents if necessary
        // Resetting the clipping rectangle causes delays so we want to do it
        // as little as possible!
        //********************************************************************

        else if (ppdev->NVClipResetFlag)
            {

        //*********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //*********************************************************************

            NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
            NV_DMA_FIFO = 0;
            NV_DMA_FIFO = ppdev->dwMaxClip; 

            ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
            }

        //**********************************************************************
        // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
        // ignore/clip the extra pixel that we send it, so that it won't
        // get included as part of the next scanline. Since we always
        // send DWords at a time, we may actually send more data to the NV
        // engine than is necessary.  So clip it by specifying a larger SizeIn
        // That is, we always send DWORDS at a time for each scanline.
        // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
        //**********************************************************************

        NextScan = (&(SrcBits[(LONG)((xsrc>>1) + (ysrc*BytesPerSrcScan))]));

        //**********************************************************************
        // Initialize the ImageFromCpu object
        //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

        //**********************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_POINT     
        //     NV061_SIZE_OUT  
        //     NV061_SIZE_IN 
        //**********************************************************************

        NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
        NV_DMA_FIFO = ((clip_y <<16) | (clip_x & 0xffff));
        NV_DMA_FIFO = ((clip_height <<16) | clip_width);
        NV_DMA_FIFO = ( (clip_height <<16) | (clip_width+1)&0xfffe);

        //**********************************************************************
        // Output one scan at a time
        //**********************************************************************

        for (yscan=0; yscan < clip_height; yscan++)
            {
            ByteCount = clip_width>>1;
            SrcPtr = NextScan;


            //**************************************************************
            // Check if we've got enough room in the push buffer
            //**************************************************************

            NV_DMAPUSH_CHECKFREE(((ULONG)((clip_width+1)>>1)+1));  
            NV_DMAPUSH_START((clip_width+1)>>1, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

            //**************************************************************
            // Set the following methods for IMAGE_FROM_CPU
            //     NV061_COLOR(0)
            //     NV061_COLOR(1)
            //     NV061_COLOR(2)
            //      ...
            //**************************************************************

            while (ByteCount > 0)
                {
                temp = pulXlate[(*SrcPtr) >> 4];    
                NV_DMA_FIFO = (pulXlate[(*SrcPtr) & 0xf]) <<16 | temp;
                SrcPtr++;
                ByteCount--;
                }

            if (clip_width&1)
                {
                temp = pulXlate[(*SrcPtr) >> 4];    
                NV_DMA_FIFO = temp;
                }

            NextScan+=BytesPerSrcScan;

        //******************************************************************
        // Kickoff buffer after each scanline
        //******************************************************************

            UPDATE_PDEV_DMA_COUNT;
            NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
            UPDATE_LOCAL_DMA_COUNT;

            }
        if (--c == 0)
            return;

        prcl++;

        } // while (TRUE)...
    }

//******************************************************************************
//
//  Function:   NV4DmaPushFastXfer8to32
//
//  Routine Description: Does 8bpp to 32bpp translation and puts the translated
//      image in the DMA push buffer. Does not use indexed image.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV4DmaPushFastXfer8to32(   // Type FastXFER
PDEV*       ppdev,
RECTL*      prcl,               // Array of relative coordinates destination rectangles
SURFOBJ*    psoSrc,             // Source surface
POINTL*     pptlSrc,            // Original unclipped source point
XLATEOBJ*   pxlo)

    {
    ULONG   *pulXlate;
    BYTE    *SrcBits;
    BYTE    *SrcPtr;
    ULONG   clip_x;
    ULONG   clip_width;
    ULONG   i;

    DECLARE_DMA_FIFO;
    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    //**************************************************************************

    pulXlate        = pxlo->pulXlate;
    SrcBits         = psoSrc->pvScan0;

    //**************************************************************************
    // Verify that src bitmap is DWORD aligned.  As far as I can tell, this
    // appears to always be the case.
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) SrcBits & 0x3),
        "Src address is not dword aligned"); // ACK!..src addr is not dword aligned !!!

    //**************************************************************************
    // Verify that src bitmap pitch is a multiple of 4 (for dwords)
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) (psoSrc->lDelta) & 0x3),
        "Pitch is not dword aligned"); // ACK!....pitch is not dword aligned !!!

    //**************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(3)));  

        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }

    //**********************************************************************
    // Top left coordinate of destination on destination surface
    //**********************************************************************

    clip_x = prcl->left;

    //**********************************************************************
    // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
    // That is, the lower right coordinate is NOT included
    // For NV, the width/height actually means the number of
    // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
    // when calculating width and height.
    //**********************************************************************

    clip_width = (prcl->right - prcl->left) & 0xffff;

    //**********************************************************************
    // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
    // ignore/clip the extra pixel that we send it, so that it won't
    // get included as part of the next scanline. Since we always
    // send DWords at a time, we may actually send more data to the NV
    // engine than is necessary.  So clip it by specifying a larger SizeIn
    // That is, we always send DWORDS at a time for each scanline.
    // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
    //**********************************************************************

    SrcPtr = (BYTE *)(&(SrcBits[pptlSrc->x + ((pptlSrc->y)*(psoSrc->lDelta))]));

    //**********************************************************************
    // Check if we've got enough room in the push buffer
    //**********************************************************************

    NV_DMAPUSH_CHECKFREE((ULONG)(clip_width + 7));  

    //**********************************************************************
    // Initialize the ImageFromCpu object
    //**********************************************************************

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , NV_SRCCOPY);

    //**********************************************************************
    // Set the following methods for IMAGE_FROM_CPU
    //     NV061_POINT     
    //     NV061_SIZE_OUT  
    //     NV061_SIZE_IN 
    //**********************************************************************

    NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
    NV_DMA_FIFO = (((prcl->top) << 16) | (clip_x & 0xffff));
    NV_DMA_FIFO = ((1 << 16) | clip_width);
    NV_DMA_FIFO = ((1 << 16) | clip_width);

    //**********************************************************************
    // Output one scanline - only 1
    //**********************************************************************

    if (clip_width >= MAX_INDEXED_IMAGE_DWORDS)

        {
        //**************************************************************
        // Check if we've got enough room in the push buffer
        //**************************************************************

        NV_DMAPUSH_START(MAX_INDEXED_IMAGE_DWORDS, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

        //**************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_COLOR(0)
        //     NV061_COLOR(1)
        //     NV061_COLOR(2)
        //      ...
        //**************************************************************

        for (i=0; i < MAX_INDEXED_IMAGE_DWORDS; i++)
            {
            NV_DMA_FIFO = pulXlate[*SrcPtr++];    
            }
        clip_width -= MAX_INDEXED_IMAGE_DWORDS;
        }

    if (clip_width > 0)
        {
        
        //**************************************************************
        // Check if we've got enough room in the push buffer
        //**************************************************************

        NV_DMAPUSH_START(clip_width, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

        //**************************************************************
        // Set the following methods for IMAGE_FROM_CPU
        //     NV061_COLOR(0)
        //     NV061_COLOR(1)
        //     NV061_COLOR(2)
        //      ...
        //**************************************************************

        while (clip_width > 0)
            {
            NV_DMA_FIFO = pulXlate[*SrcPtr++];
            clip_width--;
            }

        }

    //******************************************************************
    // Kickoff buffer
    //******************************************************************

    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer

    }

//******************************************************************************
//
//  Function:   NV4DmaPushFastXfer8to16
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


VOID NV4DmaPushFastXfer8to16(   // Type FastXFER
PDEV*       ppdev,
RECTL*      prcl,               // Array of relative coordinates destination rectangles
SURFOBJ*    psoSrc,             // Source surface
POINTL*     pptlSrc,            // Original unclipped source point
XLATEOBJ*   pxlo)

    {
    ULONG   *pulXlate;
    BYTE    *SrcBits;
    ULONG   temp;
    BYTE    *SrcPtr;
    ULONG   WordCount;
    ULONG   ByteCount;
    LONG    clip_x;
    ULONG   clip_width;
    DECLARE_DMA_FIFO;

    //**************************************************************************
    // Get push buffer information    
    //**************************************************************************

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Get Number of bytes to advance to next scanline in SOURCE
    //**************************************************************************

    pulXlate        = pxlo->pulXlate;
    SrcBits         = psoSrc->pvScan0;

    //**************************************************************************
    // Verify that src bitmap is DWORD aligned.  As far as I can tell, this
    // appears to always be the case.
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) SrcBits & 0x3),
        "Src address is not dword aligned"); // ACK!..src addr is not dword aligned !!!

    //**************************************************************************
    // Verify that src bitmap pitch is a multiple of 4 (for dwords)
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) (psoSrc->lDelta & 0x3)),
        "Pitch is not dword aligned"); // ACK!....pitch is not dword aligned !!!

    //**************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //**************************************************************************

    if (ppdev->NVClipResetFlag)
        {
        //**********************************************************************
        // Check if we've got enough room in the push buffer
        //**********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(3)));  

        //**********************************************************************
        // Set Point and Size methods for clip:
        //    NV019_SET_POINT
        //    NV019_SET_SIZE
        //**********************************************************************

        NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
        NV_DMA_FIFO = 0;
        NV_DMA_FIFO = ppdev->dwMaxClip; 

        ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
        }

    //**********************************************************************
    // Top left coordinate of destination on destination surface
    //**********************************************************************

    clip_x = prcl->left;

    //**********************************************************************
    // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
    // That is, the lower right coordinate is NOT included
    // For NV, the width/height actually means the number of
    // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
    // when calculating width and height.
    //**********************************************************************

    clip_width = prcl->right - clip_x;
    WordCount = clip_width>>1;

    //**********************************************************************
    // Specifying 1 more pixel (in 16bpp) for SizeIn causes the NV engine to
    // ignore/clip the extra pixel that we send it, so that it won't
    // get included as part of the next scanline. Since we always
    // send DWords at a time, we may actually send more data to the NV
    // engine than is necessary.  So clip it by specifying a larger SizeIn
    // That is, we always send DWORDS at a time for each scanline.
    // For 8bpp, we may specify up to 3 additional pixels (which get ignored)
    //**********************************************************************

    SrcPtr = (BYTE *)(&(SrcBits[pptlSrc->x + ((pptlSrc->y)*(psoSrc->lDelta))]));

    //**********************************************************************
    // Check if we've got enough room in the push buffer
    //**********************************************************************

    NV_DMAPUSH_CHECKFREE((ULONG)(WordCount+7));  

    //**********************************************************************
    // Initialize the ImageFromCpu object
    //**********************************************************************

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , NV_SRCCOPY);

    //**********************************************************************
    // Set the following methods for IMAGE_FROM_CPU
    //     NV061_POINT     
    //     NV061_SIZE_OUT  
    //     NV061_SIZE_IN 
    //**********************************************************************

    NV_DMAPUSH_START(3, IMAGE_FROM_CPU_SUBCHANNEL, NV061_POINT );
    NV_DMA_FIFO = (((prcl->top) <<16) | (clip_x & 0xffff));
    NV_DMA_FIFO = ((1 << 16) | clip_width);
    NV_DMA_FIFO = ((1 << 16) | (clip_width+1)&0xfffe);

    //**************************************************************
    // Check if we've got enough room in the push buffer
    //**************************************************************

    NV_DMAPUSH_START((clip_width+1)>>1, IMAGE_FROM_CPU_SUBCHANNEL, NV061_COLOR(0) );

    //**************************************************************
    // Set the following methods for IMAGE_FROM_CPU
    //     NV061_COLOR(0)
    //     NV061_COLOR(1)
    //     NV061_COLOR(2)
    //      ...
    //**************************************************************

    while (WordCount > 0)
        {
        temp = pulXlate[*SrcPtr++];
        NV_DMA_FIFO = pulXlate[*SrcPtr++]<<16 | temp;
        WordCount--;
        }
        
    if (clip_width&1)
        {
        temp = pulXlate[*SrcPtr];
        NV_DMA_FIFO = temp;
        }

    //******************************************************************
    // Kickoff buffer after each scanline
    //******************************************************************

    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer

    }

//******************************************************************************
//
//  Function:   NV4DmaPushIndexedImage
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

VOID NV4DmaPushIndexedImage(    // Type FNXFER

    PDEV*       ppdev,
    LONG        c,                  // Count of rectangles, can't be zero
    RECTL*      prcl,               // Array of relative coordinates destination rectangles
    ROP4        rop4,               // rop4
    SURFOBJ*    psoSrc,             // Source surface
    POINTL*     pptlSrc,            // Original unclipped source point
    RECTL*      prclDst,            // Original unclipped destination rectangle
    XLATEOBJ*   pxlo,
    ULONG       PatternColor,       // Solid Color of pattern
    BLENDOBJ*   pBlendObj)

    {

    BYTE* pjSrcScan0;

    ULONG lutEntries;
    ULONG sourcePixelDepth;
    LONG  BytesPerSrcScan;
    ULONG BytesPerDstScan;
    ULONG ScanInc;
    ULONG Rop3;
    ULONG temp;
    LONG  xsrc,ysrc;
    ULONG xsrcOffset;
    ULONG AlignMask;
    BYTE  *NextScan;
    ULONG *ScanPtr;
    ULONG yscan;
    ULONG SizeInWidth;
    LONG  clip_x,clip_y;
    ULONG clip_width,clip_height;
    ULONG *pulXlate;
    ULONG *currentLut;
    ULONG i;
    ULONG TempPixelData;
    ULONG LoopCount;
    ULONG DwordCount;
    ULONG ByteCount;
    ULONG tempId;
    ULONG savedId;
    ULONG lutOffset;
    ULONG notifierIndex;
    ULONG red5;
    ULONG green6;
    ULONG blue5;

    DECLARE_DMA_FIFO;
    
    ASSERTDD(pBlendObj == NULL, "Can't handle Alpha Blends");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),
             "Expect only a rop2");

    //**************************************************************************
    // Get ready to translate the 4bpp colors
    //**************************************************************************

    ASSERTDD(pxlo != NULL, "Xfer4BPP, NULL XLATEOBJ");

    //**************************************************************************
    // Force channelsync          
    // BugFix for Indexed image   
    // blits when OGL running,    
    // for NV < NV10              
    // bug #20000720-213841       
    //**************************************************************************
    if ( OglIsEnabled(ppdev) && 
         ppdev->CurrentClass.ChannelDMA == NV04_CHANNEL_DMA ) 
    {
        ppdev->oglLastChannel = -1;           
        ppdev->pfnWaitForChannelSwitch(ppdev);
    }                                         

    //**************************************************************************
    // Current pixel depth
    //**************************************************************************

    sourcePixelDepth = psoSrc->iBitmapFormat;

    //**************************************************************************
    // Get pointer to start of source bitmap
    //**************************************************************************

    pjSrcScan0 = psoSrc->pvScan0;
    BytesPerSrcScan = psoSrc->lDelta;
    ScanInc = (BytesPerSrcScan+3) >> 2;      // Number of dwords to increment

    //**************************************************************************
    // Verify that src bitmap is DWORD aligned.  As far as I can tell, this
    // appears to always be the case.
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) pjSrcScan0 & 0x3),
        "Unaligned source bitmap");// ACK!....src bitmap is not dword aligned !!!

    //**************************************************************************
    // Verify that src bitmap pitch is a multiple of 4 (for dwords)
    //**************************************************************************

    ASSERTDD(!((ULONG_PTR) BytesPerSrcScan & 0x3),
        "Pitch is not dword aligned"); // ACK!....pitch is not dword aligned !!!

    //**************************************************************************
    // Get Rop3 value
    //**************************************************************************
    Rop3 = (rop4 & 0xff);               // Get bottom byte

    //**************************************************************************
    // Each LUT is associated with an indexed image operation. To improve
    // performance, there are multiple indexed image objects with an associated
    // LUT. This is necessary because the LUT can't be updated before an operation
    // has completed; otherwise it will be corrupted. There is a notifier assoicated
    // with each indexed image operation. This code checks for a completed
    // operation so it can use that indexed image object and associated LUT. This
    // is much faster than waiting for the engine idle, or even waiting for the
    // previous indexed image operation to be completed.
    //**************************************************************************

    //**************************************************************************
    // Wait for completed indexed image object.
    //**************************************************************************

    notifierIndex = (ppdev->NvLastIndexedImageNotifier + 1)&(MAX_INDEXED_IMAGE_NOTIFIERS-1);
    
    while (TRUE)
        {
        if (((NvNotification *) (&(ppdev->Notifiers->DmaToMemIndexedImage[notifierIndex*sizeof(NvNotification)])))->status
                                             != NV064_NOTIFICATION_STATUS_IN_PROGRESS)
            break;      // operation has completed - free to re-use.
        notifierIndex = (notifierIndex+1)&(MAX_INDEXED_IMAGE_NOTIFIERS-1); // check next indexed image object
        }

    //**************************************************************************
    // Load LUT associated with indexed image object.
    //**************************************************************************

    ppdev->NvLastIndexedImageNotifier = notifierIndex;
    lutOffset = notifierIndex << 10;        // each LUT is max of 1024 bytes

    pulXlate  =  pxlo->pulXlate;
    currentLut = (ULONG *) ((BYTE*)(ppdev->NvDmaIndexedImageLut) + lutOffset);
    lutEntries = pxlo->cEntries;
    
    if (ppdev->iBitmapFormat == BMF_32BPP)  // destination pixel format 32bpp
        {
        while (lutEntries--)
            *currentLut++ = *pulXlate++;
        }
    else
        {
        //**********************************************************************
        // In 16bpp, indexed image hardware will dither when using R5G6B5. 
        // So we must use X8R8G8B8 instead to get the exact desired results
        //**********************************************************************
           
        while (lutEntries--)
            {     
            temp    = *pulXlate++;
            blue5   = temp & 0x001f;            // Get bottom 5 bits
            green6  = temp & 0x07e0;            // Get middle 6 bits
            red5    = temp & 0xf800;            // Get top 5 bits
            
            blue5   <<=3;                       // Convert R5G6B5 to X8R8G8B8
            green6  <<=5;                       
            red5    <<=8;                        
 
            *currentLut++ = (red5 | green6 | blue5);    // Store LUT data
            }
        }

    //************************************************************************
    // Get push buffer information    
    //************************************************************************

    INIT_LOCAL_DMA_FIFO;

    while (TRUE)
        {
        
    //**********************************************************************
    // Top left coordinate of destination on destination surface
    //**********************************************************************

        clip_x = prcl->left;
        clip_y = prcl->top;

    //**********************************************************************
    // Top left coordinate of source bitmap on source surface
    //**********************************************************************

        xsrc = pptlSrc->x + clip_x - prclDst->left;
        ysrc = pptlSrc->y + clip_y  - prclDst->top;

    //**********************************************************************
    // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
    // That is, the lower right coordinate is NOT included
    // For NV, the width/height actually means the number of
    // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
    // when calculating width and height.
    //**********************************************************************

        clip_width = prcl->right - clip_x;
        clip_height = prcl->bottom - clip_y;

    //***********************************************************************
    // Check if we've got enough room in the push buffer
    //***********************************************************************

        NV_DMAPUSH_CHECKFREE(((ULONG)(13))); 

    //************************************************************************
    // For the X86, check if 4bpp bitmap starting x is odd. In this case, we
    // will add 1 to width and start drawing the bitmap as if it were even,
    // and use hardware clipping to take care of it.
    //************************************************************************

    //************************************************************************
    // The IA64 has a different problem, but can use the same solution.
    // The processor will fault on unaligned addresses.  So reuse the code,
    // but with more stringent alignment requirements.
    //************************************************************************

#ifdef _WIN64
        if (sourcePixelDepth == BMF_4BPP)
           AlignMask = 0x7;
        else
           AlignMask = 0x3;
#else
        if (sourcePixelDepth == BMF_4BPP)
           AlignMask = 0x1;
        else
           AlignMask = 0x0;
#endif
        xsrcOffset = xsrc & AlignMask;
        if (xsrcOffset)
           {
           xsrc &= ~AlignMask;
        
       //*********************************************************************
       // Set Point and Size methods for clip:
       //    NV019_SET_POINT
       //    NV019_SET_SIZE
       //*********************************************************************

           NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
           NV_DMA_FIFO = ((0 << 16) | clip_x);
           NV_DMA_FIFO = (((ppdev->cyMemory)<<16) | clip_width);

           ppdev->NVClipResetFlag=TRUE;

           // adjust clip_x, clip_width - used for Dst x and width from here on
           clip_x -= xsrcOffset;
           clip_width += xsrcOffset;
           } 

    //************************************************************************
    // Reset clipping rectangle to full screen extents if necessary
    // Resetting the clipping rectangle causes delays so we want to do it
    // as little as possible!
    //************************************************************************

        else if (ppdev->NVClipResetFlag)
           {

       //*********************************************************************
       // Set Point and Size methods for clip:
       //    NV019_SET_POINT
       //    NV019_SET_SIZE
       //*********************************************************************

           NV_DMAPUSH_START(2, CLIP_SUBCHANNEL, NV019_SET_POINT);
           NV_DMA_FIFO = 0;
           NV_DMA_FIFO = ppdev->dwMaxClip; 

           ppdev->NVClipResetFlag=0;       // Clipping rectangle has been reset
           }

    //**********************************************************************
    // Get ptr to start of SRC. For 4bpp case, there are 2 pixels per byte.
    //**********************************************************************

        if (sourcePixelDepth == BMF_4BPP)
            {
            SizeInWidth = ((clip_width + 7) & 0xfff8);
            NextScan = (&(pjSrcScan0[(LONG)((xsrc>>1) + ysrc*BytesPerSrcScan)]));
            LoopCount = (clip_width+1) >> 3;    // Each loop handles 8 pels
            ByteCount = ((clip_width+1) & 7) >>1;   // extra bytes in bitmap
            }
        else
            {
            SizeInWidth = ((clip_width + 3) & 0xfffc);
            NextScan = (&(pjSrcScan0[(LONG)((xsrc) + ysrc*BytesPerSrcScan)]));
            LoopCount = clip_width >> 2;        // Each loop handles 4 pels
            ByteCount = clip_width & 3;         // extra bytes in bitmap
            }
        
        BytesPerDstScan = (LoopCount*4) + ByteCount;            
        
    //**********************************************************************
    // Set the following methods for INDEXED_IMAGE_FROM_CPU
    //     ROP
    //     SET_OBJECT and restore later
    //**********************************************************************

        NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , Rop3);

        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV064_SET_OBJECT, DD_INDEXED_IMAGE1_FROM_CPU+notifierIndex);

    //**********************************************************************
    // Set the following methods for INDEXED_IMAGE_FROM_CPU
    //     NV064_INDEX_FORMAT
    //     NV064_LUT_OFFSET
    //     NV064_POINT     
    //     NV064_SIZE_OUT  
    //     NV064_SIZE_IN 
    //**********************************************************************

        NV_DMAPUSH_START(5, IMAGE_FROM_CPU_SUBCHANNEL, NV064_INDEX_FORMAT);
        if (sourcePixelDepth == BMF_4BPP)
            NV_DMA_FIFO = NV064_INDEX_FORMAT_SVGA65_I4;
        else
            NV_DMA_FIFO = NV064_INDEX_FORMAT_LE_I8;
        NV_DMA_FIFO = lutOffset;        // byte offset

        NV_DMA_FIFO = ((clip_y << 16) | (clip_x & 0xffff));
        NV_DMA_FIFO = ((clip_height <<16) | clip_width);
        NV_DMA_FIFO = ((clip_height <<16) | SizeInWidth);

    //**********************************************************************
    // Output 1 scanline at a time.
    //**********************************************************************

#ifdef _X86_
        //**********************************************************************
        // Use assembly to output as fast as possible
        //**********************************************************************

        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPush_Transfer_MemToScreen_Data(ppdev,BytesPerDstScan,clip_height, (ULONG *)NextScan,ScanInc);
        UPDATE_LOCAL_DMA_COUNT;

#else   // ifdef _x86_

        for (yscan=0; yscan < clip_height; yscan++)
            {
            DwordCount = LoopCount;
            ScanPtr = (ULONG*) NextScan;
            
            
        //**********************************************************************
        // IMAGE_FROM_CPU can output a maximum of 1792 dwords at a time.
        // For very high resolutions, there is the possibility of exceeding this
        // maximum once per scanline; typically, it will never happen.
        //**********************************************************************

            if (DwordCount >= MAX_INDEXED_IMAGE_DWORDS)
                {

        //**********************************************************************
        // Check if we've got enough room in the push buffer and write 1792 dwords
        //**********************************************************************
        
                NV_DMAPUSH_CHECKFREE(((ULONG)(MAX_INDEXED_IMAGE_DWORDS+1)));  

                NV_DMAPUSH_START(MAX_INDEXED_IMAGE_DWORDS, IMAGE_FROM_CPU_SUBCHANNEL, NV064_INDICES(0) );
    
        //**********************************************************************
        // Set the following methods for INDEXED_IMAGE_FROM_CPU
        //     NV064_INDICES(0)
        //     NV064_INDICES(1)
        //     NV064_INDICES(2)
        //      ...
        //**********************************************************************
    
                for (i=0; i < MAX_INDEXED_IMAGE_DWORDS; i++)
                    NV_DMA_FIFO = ScanPtr[i];    
                DwordCount -= MAX_INDEXED_IMAGE_DWORDS;
                ScanPtr += MAX_INDEXED_IMAGE_DWORDS;

                }

            if (DwordCount > 0)
                {
                NV_DMAPUSH_CHECKFREE(((ULONG)(DwordCount+1)));  

                NV_DMAPUSH_START(DwordCount, IMAGE_FROM_CPU_SUBCHANNEL, NV064_INDICES(0) );
    
        //**********************************************************************
        // Set the following methods for INDEXED_IMAGE_FROM_CPU
        //     NV064_INDICES(0)
        //     NV064_INDICES(1)
        //     NV064_INDICES(2)
        //      ...
        //**********************************************************************
    
                for (i=0; i<DwordCount; i++)
                    NV_DMA_FIFO = ScanPtr[i];    
                }
                
            if (ByteCount > 0)
                {
                ScanPtr += DwordCount;
                TempPixelData = 0;
                for (i=0; i<ByteCount; i++)
                    TempPixelData |= ((PBYTE) (ScanPtr))[i] << (8 * i);
                NV_DMAPUSH_CHECKFREE(((ULONG)(2)));  

                NV_DMAPUSH_START(1, IMAGE_FROM_CPU_SUBCHANNEL, NV064_INDICES(0) );
                NV_DMA_FIFO = TempPixelData;
                }
                
            NextScan += BytesPerSrcScan;
            UPDATE_PDEV_DMA_COUNT;
            NV4_DmaPushSend(ppdev);                 // Time to kickoff the buffer
            UPDATE_LOCAL_DMA_COUNT;
            }

#endif  // ifdef else _X86_

        //******************************************************************
        // Restore channel and send data on thru to the DMA push buffer
        //******************************************************************

            if (--c == 0)
                {

        //*******************************************************************
        // Use a notifier so that the next time we get into indexed image
        // we can check if this is finished instead of doing an engine wait.
        //*******************************************************************

                ((NvNotification *) (&(ppdev->Notifiers->DmaToMemIndexedImage[notifierIndex*sizeof(NvNotification)])))->status = NV064_NOTIFICATION_STATUS_IN_PROGRESS;

                NV_DMAPUSH_CHECKFREE(((ULONG)(6)));  
                NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV064_NOTIFY       , NV064_NOTIFY_WRITE_ONLY);
                NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV064_NO_OPERATION , 0x00000000);

                NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_OBJECT, DD_IMAGE_FROM_CPU);

                UPDATE_PDEV_DMA_COUNT;
                NV4_DmaPushSend(ppdev);
                return;
                }

            prcl++;

        } // while (TRUE)
    }


//******************************************************************************
//
//  Function:   NV4DmaPushStretchCopy
//
//  Routine Description:
//      Private escape function for Desktop Manager zoom feature.
//      Performs a vidmem-to-vidmem unclipped SRCCOPY stretched blit.
//      Implements the ESC_NV_DESKMGR_ZOOMBLIT escape.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL NV4DmaPushStretchCopy(     // Type FNSTRETCHCOPY

    PDEV*       ppdev,
    ULONG       uSmooth,        // Non-zero for a smoothed copy.
    int         iScale,         // Scale factor.
    RECTL*      prclSrc,        // Source rectangle.
    RECTL*      prclDst,        // Destination rectangle.
    int         iClips,         // Number of destination clips (0 is OK).
    RECTL*      prclClips)      // Destination clips (NULL OK if zero clips).

    {
    unsigned uSrcColorFormat;
    unsigned uDstPoint, uDstSize;
    unsigned uClipPoint, uClipSize;
    unsigned uDSDX;
    unsigned uSrcPoint, uSrcSize, uSrcOffset;
    unsigned uInFormat;
    unsigned x, y, i;
    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    // The modeswitch counter is odd whenever modeset/powersave is in progress.
    if (ppdev->dwGlobalModeSwitchCount & 1)
        return TRUE;

    // Safety check.
    ASSERTDD(ppdev->CurrentClass.ScaledImageFromMemory != 0,
             "No scaled image class available in NV4DmaPushStretchCopy");

    // Compute the private color format.
    if (ppdev->iBitmapFormat == BMF_32BPP)
        uSrcColorFormat = NV089_SET_COLOR_FORMAT_LE_A8R8G8B8;
    else if (ppdev->iBitmapFormat == BMF_16BPP)
        uSrcColorFormat = NV089_SET_COLOR_FORMAT_LE_R5G6B5;
    else
        // Unsupported color format: bail.
        return FALSE;

    // Compute destination point and size.
    uDstPoint = (((unsigned) prclDst->top) << 16) |
                    (unsigned) prclDst->left;
    x = (unsigned) (prclDst->right - prclDst->left);
    y = (unsigned) (prclDst->bottom - prclDst->top);
    uDstSize = (y << 16) | x;
    // Compute scale factor.
    uDSDX = (unsigned) ((0x100000 + (iScale >> 1)) / iScale);

    // Compute source point, size, and offset.
    // There are significant restrictions on the range of the source size
    // so we adjust the source offset and point to compensate.
    // Adjust the source offset.
    x = (unsigned) (prclSrc->left & ~0x0000001f);
    y = (unsigned) (prclSrc->top & ~0x00000001);
    uSrcOffset = y * (unsigned) ppdev->lDelta +
                 x * (unsigned) ppdev->cjPelSize;
    // Compute the new source size.
    x = (((unsigned) prclSrc->right) - x + 1) & ~1;
    y = ((unsigned) prclSrc->bottom) - y;
    uSrcSize = (y << 16) | x;
    // Adjust the source point.
    x = (unsigned) (prclSrc->left & 0x0000001f);
    y = (unsigned) (prclSrc->top & 0x00000001);
    uSrcPoint = (y << 20) | (x << 4);

    // Compute the image in format.
    uInFormat = ((unsigned) ppdev->lDelta) |
                (NV089_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16);
    if (uSmooth)
        uInFormat |= (NV089_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24);
    else
        uInFormat |= (NV089_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24);

    // Specify source and destination buffers--in this case the primary buffer.
    ppdev->pfnSetSourceBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);
    ppdev->pfnSetDestBase(ppdev, ppdev->ulPrimarySurfaceOffset, ppdev->lDelta);

    // Sync our locals back after ppdev has been modified by the above calls.
    INIT_LOCAL_DMA_FIFO;

    // Get enough room in the pushbuffer; err on the side of caution.
    NV_DMAPUSH_CHECKFREE(((ULONG)(16 * iClips + 24)));

    // Load our object into the spare subchannel.
    if (ppdev->dDrawSpareSubchannelObject != DD_SCALED_IMAGE_FROM_MEMORY)
        {
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),
                          DD_SCALED_IMAGE_FROM_MEMORY);
        ppdev->dDrawSpareSubchannelObject = DD_SCALED_IMAGE_FROM_MEMORY;
        }

    // Program the object's non-volatile methods.
    NV_DMAPUSH_WRITE1(DD_SPARE, NV089_SET_CONTEXT_DMA_NOTIFIES,
                      NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV089_SET_CONTEXT_DMA_IMAGE,
                      DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV089_SET_CONTEXT_SURFACE,
                      DD_PRIMARY_IMAGE_IN_MEMORY);

    // Loop over all the destination clip rectangles.
    i = 0;
    do
        {
        // Compute clip point and size.
        if (iClips)
            {
            uClipPoint = (((unsigned) prclClips[i].top) << 16) |
                            (unsigned) prclClips[i].left;
            x = (unsigned) (prclClips[i].right - prclClips[i].left);
            y = (unsigned) (prclClips[i].bottom - prclClips[i].top);
            uClipSize = (y << 16) | x;
            }
        // If no clips are specified then use the destination rectangle.
        else
            {
            uClipPoint = uDstPoint;
            uClipSize = uDstSize;
            }

        NV_DMAPUSH_START(8, DD_SPARE, NV089_SET_COLOR_FORMAT);
        NV_DMA_FIFO = uSrcColorFormat;              // NV089_SET_COLOR_FORMAT
        NV_DMA_FIFO = NV089_SET_OPERATION_SRCCOPY;  // NV089_SET_OPERATION
        NV_DMA_FIFO = uClipPoint;                   // NV089_CLIP_POINT
        NV_DMA_FIFO = uClipSize;                    // NV089_CLIP_SIZE
        NV_DMA_FIFO = uDstPoint;                    // NV089_IMAGE_OUT_POINT
        NV_DMA_FIFO = uDstSize;                     // NV089_IMAGE_OUT_SIZE
        NV_DMA_FIFO = uDSDX;                        // NV089_DS_DX
        NV_DMA_FIFO = uDSDX;                        // NV089_DT_DY
        NV_DMAPUSH_START(4, DD_SPARE, NV089_IMAGE_IN_SIZE);
        NV_DMA_FIFO = uSrcSize;                     // NV089_IMAGE_IN_SIZE
        NV_DMA_FIFO = uInFormat;                    // NV089_IMAGE_IN_FORMAT
        NV_DMA_FIFO = uSrcOffset;                   // NV089_IMAGE_IN_OFFSET
        NV_DMA_FIFO = uSrcPoint;                    // NV089_IMAGE_IN

        } while (++i < (unsigned) iClips);

    // Kick off the pushbuffer.
    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPushSend(ppdev);

    // Success!
    return TRUE;
    }


//*********************************************************************
//
//  Function:   NV4DmaPushColorKeyBlt
//
//  Routine Description:
//      Colorkey blit on a list of rectangles. Especially used in
//      the OpenGL overlay mergeblit.
//
//      NOTE: Only initialized in 16 and 32 bpp!
//
//      NOTE: Needs surface src and dst to be set outside!
//
//  Arguments:
//
//  Return Value:
//
//      None.
// 
//*********************************************************************
VOID NV4DmaPushColorKeyBlt(
    PPDEV        ppdev,
    LONG         c,            // Number of rects in prcl
    RECTL       *prcl,         // Array of relative coordinates destination rectangles
    POINTL      *pptlSrc,      // Original unclipped source point
    RECTL       *prclDst,      // Original unclipped destination rectangle
    ULONG        ulColorRef)   // 32bit ARGB color key reference value
{  
    NvU32   ulColorFormat; // Class 57 color format of colorkey
    NvU32   ulColorKey;    // Class 57 colorkey
    ULONG   red, green, blue;
    LONG    dx;            // Add delta to destination to get source
    LONG    dy;           
    ULONG   width,height;
    ULONG   xsrc,ysrc;
    ULONG   xdst,ydst;
  
    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    ASSERT(0!=c); // don't call me if you don't need
    ASSERT(NULL!=prcl   );
    ASSERT(NULL!=pptlSrc);
    ASSERT(NULL!=prclDst);

    //
    // setup color key object
    //
    if (2==ppdev->cjPelSize)
    {
        blue        = (ulColorRef & 0x00FF0000) >> (16 + 3);   // 5 bits of B            
        green       = (ulColorRef & 0x0000FF00) >> (8 + 2);    // 6 bits of G                       
        red         = (ulColorRef & 0x000000FF) >> 3;          // 5 bits of R                 

        ulColorKey    = (NvU32)( ((red << 11) | (green << 5) | blue) | 0xFFFF0000 );
        ulColorFormat = NV057_SET_COLOR_FORMAT_LE_A16R5G6B5;
    } 
    else 
    {
        ASSERT(4==ppdev->cjPelSize);

        blue        = (ulColorRef & 0x00FF0000) >> 16;               
        green       = (ulColorRef & 0x0000FF00) >> 8;               
        red         = (ulColorRef & 0x000000FF);               

        ulColorKey    = (NvU32)( ((red << 16) | (green << 8) | blue | 0xFF000000) );
        ulColorFormat = NV057_SET_COLOR_FORMAT_LE_A8R8G8B8;
    }

    NV_DMAPUSH_CHECKFREE((ULONG)(6));

    NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),    DD_IMAGE_SOLID);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR_FORMAT, ulColorFormat)
    NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR,        ulColorKey);

    for ( /* c */; c > 0; c--)
    {
        //**********************************************************************
        // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
        // That is, the lower right coordinate is NOT included
        // For NV, the width/height actually means the number of
        // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
        // when calculating width and height.
        //**********************************************************************

        xdst = prcl->left;
        ydst = prcl->top;

        width = prcl->right - xdst;
        height = prcl->bottom - ydst;

        dx = xdst - prclDst->left;      // prcl is always within prcldst???
        dy = ydst - prclDst->top;       // prcl is always within prcldst???

        xsrc = pptlSrc->x + dx;
        ysrc = pptlSrc->y + dy;

        NV_DMAPUSH_CHECKFREE((ULONG)(8));
        //
        // do the transparent blt
        //
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), DD_IMAGE_BLIT_COLORKEY);
        NV_DMAPUSH_START(3, DD_SPARE, NV05F_CONTROL_POINT_IN);
        NV_DMA_FIFO = ( (ysrc << 16) | (xsrc & 0xffff) );              // NV05F_CONTROL_POINT_IN 
        NV_DMA_FIFO = ( (ydst << 16) | (xdst & 0xffff) );              // NV05F_CONTROL_POINT_OUT
        NV_DMA_FIFO = ( (height << 16) | (width & 0xffff));            // NV05F_SIZE             

        prcl++;
    }

    NV_DMAPUSH_CHECKFREE((ULONG)(4));
    //
    // disable colorkey again
    //
    NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), DD_IMAGE_SOLID);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR, 0); // color key disabled

    ppdev->dDrawSpareSubchannelObject = DD_IMAGE_SOLID;

    UPDATE_PDEV_DMA_COUNT;
    NV4_DmaPushSend(ppdev);
}


//******************************************************************************
//
// Function: NV4DmaPushAlphaPreserve
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//     bStart - True if beginning AlphaPreserveMode, False if ending
//     bFromCpu - True if blt source is system memory
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV4DmaPushAlphaPreserve(
    PPDEV       ppdev,
    BOOL        bStart,
    BOOL        bFromCpu)
{
    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    NV_DMAPUSH_CHECKFREE((ULONG)(8));

    if (bStart) 
        {
        if (bFromCpu) 
            {
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_OPERATION, NV061_SET_OPERATION_SRCCOPY);
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, NV061_SET_COLOR_FORMAT_LE_A8R8G8B8);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),    DD_PRIMARY_IMAGE_IN_MEMORY);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT, NV042_SET_COLOR_FORMAT_LE_A8R8G8B8);
            }
        else
            {
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_OPERATION, NV05F_SET_OPERATION_SRCCOPY);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),    DD_PRIMARY_IMAGE_IN_MEMORY);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT, NV042_SET_COLOR_FORMAT_LE_A8R8G8B8);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0), DD_IMAGE_BLIT);
            }
        }
    else
        {
        if (bFromCpu) 
            {
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_OPERATION, NV061_SET_OPERATION_ROP_AND);
            NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_COLOR_FORMAT, NV061_SET_COLOR_FORMAT_LE_X8R8G8B8);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT, NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0), DD_IMAGE_BLIT);
            }
        else
            {
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_OPERATION, NV05F_SET_OPERATION_ROP_AND);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),    DD_PRIMARY_IMAGE_IN_MEMORY);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT, NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
            NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0), DD_IMAGE_BLIT);   
            }
        }

    UPDATE_PDEV_DMA_COUNT;

    // For performance reasons, don't kick off this tiny bit of data now (NV4_DmaPushSend(ppdev);)
}

#if (_WIN32_WINNT >= 0x0500)

//******************************************************************************
//
//  Function:   NV4CheckAlphaBlendOK
//
//  Routine Description:
//
//      For given state in PDEV, check if HW can do the blend 
//
//  Arguments:
//
//     ppdev - Pointer to the physical device structure
//
//  Return Value: 
//
//      TRUE if HW capable, FALSE if not
//
//******************************************************************************

BOOL NV4CheckAlphaBlendOK(
    PDEV*   ppdev)
{
    // Check for state/hw combinations which don't support alpha blending
    // In 16bpp, need better than NV04 so we can turn dither off
    // Note NV04_IMAGE_FROM_CPU works fine for 32bpp, but not 16bpp, since it has no way to turn off
    //   dither.  So we support alpha in 32bpp and can punt (by returning FALSE here) in 16bpp.  
    // HOWEVER, there are no DCT failures on pre-NV10 hw in 16bpp even though we can't disable dither.
    //   THEREFORE, the punt is disabled. The code remains in case we find a failure and want to punt for old HW.
    #if 0
    if ((ppdev->CurrentClass.ContextSurfaces2D == NV04_CONTEXT_SURFACES_2D) &&
        (ppdev->iBitmapFormat == BMF_16BPP))
        {
        return FALSE;
        }
    else
    #endif
        {
        // all else, we can do
        return TRUE;
        }
}   


//******************************************************************************
//
//  Function:   NV4DmaPushCopyAlphaBlit
//
//  Routine Description:
//
//      Does a screen-to-screen blt of a list of alpha blended rectangles.
//      A modified version of NV4DmaPushCopyBlt, that uses class 077 (NV4_SCALED_IMAGE_FROM_MEMORY)
//
//  Arguments:
//
//  Return Value: 
//
//      TRUE if successful, FALSE if caller must punt operation to GDI
//
//******************************************************************************

BOOL NV4DmaPushCopyAlphaBlt(    // Type FNCOPY
    PDEV*   ppdev,
    LONG    c,          // Can't be zero
    RECTL*  prcl,       // Array of relative coordinates destination rectangles
    ULONG   rop4,       // rop4
    POINTL* pptlSrc,    // Original unclipped source point
    RECTL*  prclDst,    // Original unclipped destination rectangle
    BLENDOBJ* pBlendObj)
{
    LONG    clipx,clipy,clipwidth,clipheight;
    LONG    xdst,ydst,width,height;
    LONG    dx,dy;
    LONG    xsrc,ysrc;
    ULONG   constAlpha;

    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    //**************************************************************************
    // Safety checks
    //**************************************************************************
    ASSERTDD(pBlendObj, "Expect valid pBlendObj on alphablend blts");
    ASSERTDD(c > 0, "Can't handle zero rectangles");
    ASSERTDD(((rop4 & 0xff00) >> 8) == (rop4 & 0xff),"Expect only a rop2");

    // DD_ALPHA_IMAGE_FROM_MEMORY is either NV04_SCALED_IMAGE_FROM_MEMORY(077) or NV10_SCALED_IMAGE_FROM_MEMORY(089)
    // Routine uses NV077 offsets for methods common to each class, and 089 for NV10-only features
    // We should return an error here if we need an NV10 feature but the class is pre-NV10 (meaning hw is pre-NV10)
    //   Caller knows error return means not-supported and will punt to GDI
    // Note NV04_SCALED_IMAGE_FROM_MEMORY works fine for 32bpp, but not 16bpp, since it has no way to turn off
    //   dither.  We support alpha in 32bpp and can punt (by returning FALSE here) in 16bpp.  
    // HOWEVER, there are no DCT failures on pre-NV10 hw in 16bpp even though we can't disable dither.
    //   THEREFORE, the punt is disabled. The code remains in case we find a failure and want to punt for old HW.
    // Finally, note it turns out we could call NV4CheckAlphaBlendOK since it and the check for NV04_SCALED_IMAGE are 
    //   the same for current hardware.  However, we leave the separate specific check SCALED_IMAGE support since it 
    //   was already there and in case future hardware has varying SCALED_IMAGE support
    #if 0 // Disable punt : let pre-NV10 proceed w/ dither on
    if ((ppdev->CurrentClass.AlphaImageFromMemory == NV04_SCALED_IMAGE_FROM_MEMORY) &&
        (ppdev->iBitmapFormat == BMF_16BPP))
        {
        return FALSE;
        }
    #endif
            
    // Check if we've got enough room in the push buffer for largest possible output
    NV_DMAPUSH_CHECKFREE((ULONG)((13*2) + (12*c)));

    NV_DMAPUSH_WRITE1(ROP_SOLID_SUBCHANNEL, NV043_SET_ROP5 , rop4 & 0xff);

    // Setup Blender - load alpha value into all bytes since hw uses each byte as an independent alpha value
    constAlpha = pBlendObj->BlendFunction.SourceConstantAlpha & 0x000000FF;
    constAlpha |= constAlpha << 8;
    constAlpha |= constAlpha << 16;

    NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),    NV_DD_CONTEXT_BETA4);
    NV_DMAPUSH_WRITE1(DD_SPARE, NV072_SET_BETA_FACTOR,  constAlpha);

    // if 32bpp, use color format with valid alpha for destination (primary)
    if (ppdev->iBitmapFormat==BMF_32BPP) 
        {
        // set back to default LE_X8R8G8B8 (32bpp) at the end
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),     DD_PRIMARY_IMAGE_IN_MEMORY);
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT,  NV042_SET_COLOR_FORMAT_LE_A8R8G8B8);
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),     DD_IMAGE_BLIT);
        }

    // Load Alpha image object into the spare subchannel
    NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),    DD_ALPHA_IMAGE_FROM_MEMORY);
    ppdev->dDrawSpareSubchannelObject = DD_ALPHA_IMAGE_FROM_MEMORY;

    NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_CONTEXT_SURFACE,      DD_PRIMARY_IMAGE_IN_MEMORY);

    if (pBlendObj->BlendFunction.AlphaFormat & AC_SRC_ALPHA) 
        {
        // Source has valid alpha
        ASSERTDD(ppdev->iBitmapFormat==BMF_32BPP, "Alphablend CopyBlts w/ SrcAlpha must be 32bpp");
        NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_COLOR_FORMAT, NV077_SET_COLOR_FORMAT_LE_A8R8G8B8);
        }
    else
        {
        // Source has no alpha - use format having no alpha, where source alpha defaults to 0xff by HW
        if (ppdev->iBitmapFormat==BMF_32BPP) 
            {
            NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_COLOR_FORMAT, NV077_SET_COLOR_FORMAT_LE_X8R8G8B8);
            }
            else
            {
            NV_DMAPUSH_WRITE1(DD_SPARE, NV077_SET_COLOR_FORMAT, NV077_SET_COLOR_FORMAT_LE_R5G6B5);
            // For post NV4 hw, turn dither off for the blend
            if (ppdev->CurrentClass.AlphaImageFromMemory != NV04_SCALED_IMAGE_FROM_MEMORY)
                {
                NV_DMAPUSH_WRITE1(DD_SPARE, NV089_SET_COLOR_CONVERSION, NV089_SET_COLOR_CONVERSION_TRUNCATE);
                }
            
            }
        }

    // Do a copy blit for each clip rectangle
    do  {

        //**********************************************************************
        // GDI passes the lower right and bottom coordinate as EXCLUSIVE.
        // That is, the lower right coordinate is NOT included
        // For NV, the width/height actually means the number of
        // pixels drawn.  So, adjusting by 1 pixel is NOT necessary
        // when calculating width and height.
        //**********************************************************************
        
        // Set Clip point and size for Screen boundaries
        clipx = prcl->left;
        if (prcl->right > ppdev->cxScreen)
            {
            clipwidth = ppdev->cxScreen - clipx; 
            }
        else
            {
            clipwidth = prcl->right - clipx;
           
            if (clipx < 0)
                {
                clipwidth += clipx; // subtract (add neg. x) distance from x start to screen left
                clipx = 0;
                }
            }

        clipy = prcl->top;
        if (prcl->bottom > ppdev->cyScreen)
            {
            clipheight = ppdev->cyScreen - clipy; 
            }
        else
            {
            clipheight = prcl->bottom - clipy;
            
            if (clipy < 0)
                {
                clipheight += clipy; // subtract (add neg. y) distance from y start to screen top
                clipy = 0;
                }
            }

        NV_DMAPUSH_START(6, DD_SPARE, NV077_CLIP_POINT);
        NV_DMA_FIFO = ((clipy << 16)|(clipx & 0xffff));         // NV077_CLIP_POINT
        NV_DMA_FIFO = ((clipheight << 16)|(clipwidth & 0xffff));// NV077_CLIP_SIZE

        // Use original unclipped origin, size for image size
        xdst = prcl->left;
        ydst = prcl->top;
        width = prcl->right - xdst;
        height = prcl->bottom - ydst;
        
        NV_DMA_FIFO = ((ydst << 16)|(xdst & 0xffff));   // NV077_IMAGE_OUT_POINT
        NV_DMA_FIFO = ((height << 16)|(width & 0xffff));// NV077_IMAGE_OUT_SIZE
        NV_DMA_FIFO = 0x100000;                         // NV077_DELTA_DU_DX = 1.0 - no scaled blts supported yet
        NV_DMA_FIFO = 0x100000;                         // NV077_DELTA_DV_DY = 1.0 - no scaled blts supported yet

        dx = xdst - prclDst->left;      // prcl is always within prcldst???
        dy = ydst - prclDst->top;       // prcl is always within prcldst???

        xsrc = pptlSrc->x + dx;
        ysrc = pptlSrc->y + dy;

        // width, height are w.r.t SrcOffset, so add src x,y
        width  += xsrc;
        height += ysrc;
        
        // Class restriction - width must be a multiple of 2
        width = (width + 1) & ~1;

        NV_DMAPUSH_START(4, DD_SPARE, NV077_IMAGE_IN_SIZE);
        NV_DMA_FIFO = (height << 16) | width;           // NV077_IMAGE_IN_SIZE
        NV_DMA_FIFO = ((unsigned) ppdev->CurrentSourcePitch) |  // NV077_IMAGE_IN_FORMAT
                        (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16) |
                        (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24);
        NV_DMA_FIFO = ppdev->CurrentSourceOffset;       // NV077_IMAGE_IN_OFFSET
        NV_DMA_FIFO = (ysrc << 20) | (xsrc << 4);       // NV077_IMAGE_IN

        prcl++;

    } while (--c != 0);

    // if 32bpp, restore color format which was changed at entry
    if (ppdev->iBitmapFormat==BMF_32BPP) 
        {
        // restore defaults
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),     DD_PRIMARY_IMAGE_IN_MEMORY);
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV042_SET_COLOR_FORMAT,  NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8);
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NVFFF_SET_OBJECT(0),     DD_IMAGE_BLIT);
        }

    // Update global push buffer count
    UPDATE_PDEV_DMA_COUNT;

    // Send data on thru to the DMA push buffer
    NV4_DmaPushSend(ppdev);

    return TRUE;

}


//******************************************************************************
//
// Function: NV4DmaPushSetColorKey
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//     colorformat - non-zero colorformat format if Set key, 0 if disabling color keying
//     iTransColor - The color key 
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV4DmaPushSetColorKey(
    PPDEV       ppdev,
    ULONG       colorformat, // 0 means disable
    ULONG       iTransColor)
{
    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    NV_DMAPUSH_CHECKFREE((ULONG)(10));

    if (colorformat)
        {
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_COLOR_KEY, DD_IMAGE_SOLID);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_COLOR_KEY, DD_IMAGE_SOLID);
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0),    DD_IMAGE_SOLID);
        NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR_FORMAT, colorformat ==  BMF_32BPP ?
                                                            NV057_SET_COLOR_FORMAT_LE_A8R8G8B8 :
                                                            NV057_SET_COLOR_FORMAT_LE_A16R5G6B5);
        // iTransColor already in surface's color format - set Alpha non-zero to activate color key check in NV hardware
        NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR,        iTransColor | (0xff << 24));
        }
    else
        {
        NV_DMAPUSH_WRITE1(DD_SPARE, NVFFF_SET_OBJECT(0), DD_IMAGE_SOLID);
        NV_DMAPUSH_WRITE1(DD_SPARE, NV057_SET_COLOR, 0); // color key disabled
        NV_DMAPUSH_WRITE1(BLIT_SUBCHANNEL, NV05F_SET_CONTEXT_COLOR_KEY, 0);
        NV_DMAPUSH_WRITE1(IMAGE_FROM_CPU_SUBCHANNEL, NV061_SET_CONTEXT_COLOR_KEY, 0);
        ppdev->dDrawSpareSubchannelObject = DD_IMAGE_SOLID;
        }

    UPDATE_PDEV_DMA_COUNT;

    // For performance reasons, don't kick off this tiny bit of data now (NV4_DmaPushSend(ppdev);)
}


#endif // (_WIN32_WINNT >= 0x0500)
