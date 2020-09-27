//******************************Module*Header***********************************
//
// Module Name: bank.c
//
// Contains all the banking code for the display driver.
//
// It's helpful not to have to implement all the DDI drawing functionality
// in a driver (who wants to write the code to support true ROP4's with
// arbitrary sized patterns?).  Fortunately, we can punt to GDI for any
// drawing we don't want to do.  And if GDI can write directly on the frame
// buffer bits, performance won't even be toooo bad.
//
// NT's GDI can draw on any standard format frame buffer.  When the entire
// frame buffer can be mapped into main memory, it's very simple to set up:
// the display driver tells GDI the frame buffer format and location, and
// GDI can then just draw directly.
//
// When only one bank of the frame buffer can be mapped into main memory
// at one time (e.g., there is a moveable 64k aperture) things are not
// nearly so easy.  For every bank spanned by a drawing operation, we have
// to set the hardware to the bank, and call back to GDI.  We tell GDI
// to draw only on the mapped-in bank by mucking with the drawing call's
// CLIPOBJ.
//
// This module contains the code for doing all banking support.
//
// This code supports 8, 16 and 32bpp colour depths, arbitrary bank
// sizes, and handles 'broken rasters' (which happens when the bank size
// is not a multiple of the scan length; some scans will end up being
// split over two separate banks).
//
// Note:  If you mess with this code and break it, you can expect to get
//        random access violations on call-backs in internal GDI routines
//        that are very hard to debug.
//
// Copyright (c) 1993-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"


//******************************************************************************
// Extern declearations
//******************************************************************************

VOID NV3ScreenToMemoryDMA(PDEV*,BYTE** ,LONG ,BYTE** ,LONG ,LONG ,LONG ,BOOL );

//******************************Public*Routine**********************************
//
// Function: vAlignedCopy
//
// Routine Description:
//
//      Copies the given portion of a bitmap, using dword alignment for the
//      screen.  Note that this routine has no notion of banking.
//
//      Updates ppjDst and ppjSrc to point to the beginning of the next scan.
//
// Arguments:
//
//      ppdev - Pointer to the physical device structure
//
// Return Value:
//
//******************************************************************************


VOID vAlignedCopy(
PDEV*   ppdev,
BYTE**  ppjDst,
LONG    lDstDelta,
BYTE**  ppjSrc,
LONG    lSrcDelta,
LONG    cjScan,
LONG    cyScan,
BOOL    bDstIsScreen)

    {
    BYTE* pjDst;
    BYTE* pjSrc;
    LONG  cjMiddle;
    LONG  culMiddle;
    LONG  cjStartPhase;
    LONG  cjEndPhase;
    LONG i;

    //**************************************************************************
    // Get source and destination pointers
    //**************************************************************************

    pjSrc = *ppjSrc;
    pjDst = *ppjDst;

    //**************************************************************************
    // If Screen is the SOURCE, then align the copy according to the SOURCE.
    // If Screen is the DEST,   then align the copy according to the DEST.
    //
    // Careful:  Phase is 0 if on a byte 0 boundary.
    //           Phase is 3 if on a byte 1 boundary.
    //           Phase is 2 if on a byte 2 boundary.
    //           Phase is 1 if on a byte 3 boundary.
    //
    // cjStartPhase is the number of bytes we need to output first, before
    // we can start outputting the rest of the bytes.
    //**************************************************************************

    // Be aware of IA64 when doing ptr arithmetic !!
    cjStartPhase = (0 - ((bDstIsScreen) ? (LONG)(ULONG_PTR)pjDst : (LONG)(ULONG_PTR)pjSrc)) & 3;

    //**************************************************************************
    // Calculate Number of bytes remaing after outputting the 'starting' phase bytes.
    // We'll later calculate the final culMiddle and cjEndPhase from cjMiddle.
    //**************************************************************************

    cjMiddle     = cjScan - cjStartPhase;

    //**************************************************************************
    // For cases where cjMiddle is positive:
    //                                                  cjEndPhase      culMiddle
    //   cjScan     cjStartPhase    cjMiddle          (cjMiddle & 3)  (cjMiddle >> 2)
    //   ------     ------------    --------          --------------  ---------------
    //      1           0              1                    1            0
    //      1           1              0                    0            0
    //      1           2             neg                   -            -
    //      1           3             neg                   -            -
    //
    //      2           0              2                    2            0
    //      2           1              1                    1            0
    //      2           2              0                    0            0
    //      2           3             neg                   -            -
    //
    //      3           0              3                    3            0
    //      3           1              2                    2            0
    //      3           2              1                    1            0
    //      3           3              0                    0            0
    //
    //      4           0              4                    0            1
    //      4           1              3                    3            0
    //      4           2              2                    2            0
    //      4           3              1                    1            0
    //
    // The following condition always holds true in the above table.
    //
    //      cjStartPhase + (culMiddle * 4) +cjEndPhase  ==  cjScan
    //
    // EXCEPT for the cases where cjMiddle < 0.  So we must special case them.
    // Here, we'll just get rid of the starting phase.  The end result is that
    // for these case, bytes will only be output in the 'cjEndPhase'.
    //
    //**************************************************************************

    if (cjMiddle < 0)
        {
        cjStartPhase = 0;
        cjMiddle     = cjScan;
        }

    //**************************************************************************
    // Update the number of bytes we need to add to get to the next scanline,
    // after copying 'cjScan' bytes.
    //**************************************************************************

    lSrcDelta -= cjScan;
    lDstDelta -= cjScan;

    //**************************************************************************
    // EndPhase will be copied in bytes.
    // MiddlePhase will always be copied in DWORDS.
    // See description up above.
    //**************************************************************************

    cjEndPhase = cjMiddle & 3;
    culMiddle  = cjMiddle >> 2;

    //**************************************************************************
    // Ensure that graphics engine is idle before touching the framebuffer.
    //**************************************************************************
    ppdev->pfnWaitEngineBusy(ppdev);
    if (bDstIsScreen)

        {
        //**********************************************************************
        // Align to the destination (implying that the source may be
        // unaligned):  Occurs when copying MEMORY to SCREEN (Dest is aligned).
        // Copy scanlines at a time.
        //**********************************************************************

        for (; cyScan > 0; cyScan--)

            {
            //******************************************************************
            // Copy Bytes at a time
            //******************************************************************

            for (i = cjStartPhase; i > 0; i--)
                {
                *pjDst++ = *pjSrc++;
                }

            //******************************************************************
            // Copy Dwords at a time
            //******************************************************************

            for (i = culMiddle; i > 0; i--)
                {
                *((ULONG*) pjDst) = *((ULONG UNALIGNED *) pjSrc);
                pjSrc += sizeof(ULONG);
                pjDst += sizeof(ULONG);
                }

            //******************************************************************
            // Copy Bytes at a time
            //******************************************************************

            for (i = cjEndPhase; i > 0; i--)
                {
                *pjDst++ = *pjSrc++;
                }

            pjSrc += lSrcDelta;
            pjDst += lDstDelta;
            }
        }

    else

        {

        //**********************************************************************
        // Align to the source (implying that the destination may be
        // unaligned):  Occurs when copying SCREEN (Source is aligned) to MEMORY.
        // Copy scanlines at a time.
        //**********************************************************************

        for (; cyScan > 0; cyScan--)

            {
            //******************************************************************
            // Copy Bytes at a time
            //******************************************************************

            for (i = cjStartPhase; i > 0; i--)
                {
                *pjDst++ = *pjSrc++;
                }

            //******************************************************************
            // Copy Dwords at a time
            //******************************************************************

            for (i = culMiddle; i > 0; i--)
                {
                *((ULONG UNALIGNED *) pjDst) = *((ULONG*) (pjSrc));
                pjSrc += sizeof(ULONG);
                pjDst += sizeof(ULONG);
                }

           //*******************************************************************
           // Copy Bytes at a time
           //*******************************************************************

            for (i = cjEndPhase; i > 0; i--)
                {
                *pjDst++ = *pjSrc++;
                }

            pjSrc += lSrcDelta;
            pjDst += lDstDelta;
            }

        }

    //**************************************************************************
    // Save the updated pointers
    //**************************************************************************

    *ppjSrc = pjSrc;
    *ppjDst = pjDst;

    }

//******************************Public*Routine**********************************
//
// Function: vPutBits
//
// Routine Description:
//
//      Copies the bits from the given surface to the screen
//      Must be pre-clipped.  (Implement Memory to Screen blit here for
//      better performance???)
//
// Arguments:
//
//       ppdev - Pointer to the physical device structure
//
// Return Value:
//
//******************************************************************************

VOID vPutBits(
PDEV*       ppdev,
DSURF*      pdsurf,
SURFOBJ*    psoSrc,
RECTL*      prclDst)            // Absolute coordinates!

    {
    RECTL   rclDraw;
    LONG    iBank;
    LONG    cjOffset;
    LONG    cyScan;
    LONG    lDstDelta;
    LONG    lSrcDelta;
    BYTE*   pjDst;
    BYTE*   pjSrc;
    LONG    cjScan;
    LONG    iNewBank;
    LONG    cjRemainder;

    //**************************************************************************
    // We need a local copy of 'rclDraw' because we'll be iteratively
    // modifying 'top' and passing the modified rectangle back into
    // bBankComputeNonPower2:
    //**************************************************************************

    rclDraw = *prclDst;

    //**************************************************************************
    // Calculate the pointer to the upper-left corner of Destination rectangle (DFB)
    //**************************************************************************

    lDstDelta = pdsurf->LinearStride;
    pjDst     = pdsurf->LinearPtr;

    //**************************************************************************
    // Calculate the pointer to the upper-left corner of Source rectangle (DIB)
    //**************************************************************************

    lSrcDelta = psoSrc->lDelta;
    pjSrc     = (BYTE*) psoSrc->pvScan0;

    //**************************************************************************
    // Calculate number of scanlines to copy (cyScan), and
    // Calculate number of bytes to copy for each scanline (cjScan)
    //**************************************************************************

    cjScan = CONVERT_TO_BYTES((rclDraw.right  - rclDraw.left), ppdev);
    cyScan = (rclDraw.bottom - rclDraw.top);

    //**************************************************************************
    // Copy bytes. DIB (Memory) is source. DFB (Framebuffer) is destination
    //**************************************************************************

    vAlignedCopy(ppdev, &pjDst, lDstDelta, &pjSrc, lSrcDelta, cjScan, cyScan,
                     TRUE);             // Screen is the destination
    return;
    }

//******************************Public*Routine**********************************
//
// Function: vGetBits
//
// Routine Description:
//
//       Copies the bits to the given surface from the screen.
//       Currently, just uses straighforward software copy.  (later use DMA?)
//       Must be pre-clipped.
//
// Arguments:
//
//       ppdev - Pointer to the physical device structure
//
// Return Value:
//
//******************************************************************************


VOID vGetBits(
PDEV*       ppdev,
DSURF*      pdsurf,
SURFOBJ*    psoDst,
RECTL*      prclDst)        // Absolute coordinates!

    {
    LONG    iBank;
    LONG    cjOffset;
    LONG    cyScan;
    LONG    lDstDelta;
    LONG    lSrcDelta;
    BYTE*   pjDst;
    BYTE*   pjSrc;
    LONG    cjScan;
    LONG    iNewBank;
    LONG    cjRemainder;
    LONG    width,height;

    //**************************************************************************
    // Get source rectangle (Offscreen DFB rectangle)
    //**************************************************************************

    width  = (prclDst->right  - prclDst->left);
    height = (prclDst->bottom - prclDst->top);

    //**************************************************************************
    // Calculate the pointer to the upper-left corner of Source rectangle (DFB)
    //**************************************************************************

    lSrcDelta = pdsurf->LinearStride;
    pjSrc     = pdsurf->LinearPtr;

    //**************************************************************************
    // Calculate the pointer to the upper-left corner of Destination rectangle (DIB)
    //**************************************************************************

    lDstDelta = psoDst->lDelta;
    pjDst     = (BYTE*) psoDst->pvScan0;

    //**************************************************************************
    // Calculate number of scanlines to copy (cyScan), and
    // Calculate number of bytes to copy for each scanline (cjScan)
    //**************************************************************************

    cjScan = CONVERT_TO_BYTES(width, ppdev);
    cyScan = height;

    //**************************************************************************
    // Copy bytes. DFB (Framebuffer) is source. DIB (memory) is destination
    //**************************************************************************

    vAlignedCopy(ppdev, &pjDst, lDstDelta, &pjSrc, lSrcDelta, cjScan, cyScan, FALSE);

    return;
    }
