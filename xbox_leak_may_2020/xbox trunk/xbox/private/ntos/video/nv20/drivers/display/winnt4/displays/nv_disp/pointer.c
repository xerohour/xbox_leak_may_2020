//******************************Module*Header***********************************
//
// Module Name: pointer.c
//
// This module contains the hardware pointer support for the display
// driver.  This supports both the built-in S3 hardware pointer and
// some common DAC hardware pointers.
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 - 2000 NVidia Corporation. All Rights Reserved.        *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"
#include "nv32.h"

//******************************************************************************
// External declarations
//******************************************************************************

VOID ShowPointer(PDEV*   ppdev, BOOL bShow);

#ifdef NV3
extern VOID NV3MovePointer(PDEV *,LONG,LONG);
extern VOID NV3EnablePointer(PDEV *,BOOL);
extern ULONG NV3SetPointerShape(SURFOBJ *,SURFOBJ*,SURFOBJ*,XLATEOBJ*,LONG,LONG,LONG,LONG,LONG,LONG,BOOL);
#else
extern VOID NVMovePointer(PDEV *,LONG,LONG);
extern VOID NVEnablePointer(PDEV *,BOOL);
extern ULONG NVSetPointerShapeOn16BppHwMode(SURFOBJ *,SURFOBJ*,SURFOBJ*,XLATEOBJ*,LONG,LONG,LONG,LONG,LONG,LONG,BOOL);
extern ULONG NVSetPointerShapeOn32BppHwMode(SURFOBJ *,SURFOBJ*,SURFOBJ*,XLATEOBJ*,LONG,LONG,LONG,LONG,LONG,LONG,BOOL);
#endif
//******************************************************************************
//
//  Function:   DrvMovePointer
//
//  Routine Description:
//
//  NOTE: Because we have set GCAPS_ASYNCMOVE, this call may occur at any
//        time, even while we're executing another drawing call!
//
//        Consequently, we have to explicitly synchronize any shared
//        resources.  In our case, since we touch the CRTC register here
//        and in the banking code, we synchronize access using a critical
//        section.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID DrvMovePointer(
SURFOBJ*    pso,
LONG        x,
LONG        y,
RECTL*      prcl)
{
    PDEV*   ppdev;
    BOOL    bRet;

    ppdev = (PDEV*) pso->dhpdev;
    if (x == -1)
    {
    //**************************************************************************
    // This is a request to hide the cursor.
    //**************************************************************************
    ppdev->pfnShowPointer(ppdev, FALSE);
    }
    else
    {
        //**************************************************************************
        // Move cursor to appropriate location
        //**************************************************************************
        ppdev->pfnMovePointer(ppdev, x, y);

#if _WIN32_WINNT >= 0x0500
        if(ppdev->ulPuntCursorToGDI)
            {
            ppdev->pfnWaitEngineBusy(ppdev);
            EngMovePointer(pso, x, y, prcl);
            }
#endif

    }

}

//******************************************************************************
//
//  Function:   DrvSetPointerShape
//
//  Routine Description:
//
//      Sets the new pointer shape.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

ULONG DrvSetPointerShape(
SURFOBJ*    pso,
SURFOBJ*    psoMsk,
SURFOBJ*    psoColor,
XLATEOBJ*   pxlo,
LONG        xHot,
LONG        yHot,
LONG        x,
LONG        y,
RECTL*      prcl,
FLONG       fl)

    {
    PDEV*   ppdev;
    DWORD*  pul;
    ULONG   cx;
    ULONG   cy;
    BOOL    AlphaBlendedCursor;
    ULONG   ulReturnValue;
        
    ppdev = (PDEV*) pso->dhpdev;

    ulReturnValue = SPS_DECLINE;

    if (ppdev->flCaps & CAPS_SW_POINTER || !ppdev->bEnabled)
        goto DoneDrvSetPointerShape;

#if _WIN32_WINNT >= 0x0500

    if ( (fl & SPS_ALPHA) && (psoMsk == NULL) && (psoColor != NULL) )
        {
        //**********************************************************************
        // Handle Win2k Alpha blended cursors (ONLY if OpenGL is enabled)
        // Normally, we wouldn't attempt to handle alpha blended cursors because
        // our hardware doesn't support alpha blended cursors.
        // Unfortunately, when OpenGL is enabled, the alpha blended software cursor
        // is not visible on top of an OpenGL app.  So we're forced to 'handle' 
        // this case with our opaque hardware cursor, so that we can see the cursor.
        //
        // 2 side effects:
        //      1) Hardware only supports 32x32 cursors (up to NV5)
        //         So Alpha Blended cursors which width > 32 and height > 32
        //         may get clipped abruptly.
        //
        //      2) We ignore the alpha blended component of the bitmap
        //          and just make it transparent.
        //**********************************************************************

        cx = psoColor->sizlBitmap.cx;           
        cy = psoColor->sizlBitmap.cy;           
        AlphaBlendedCursor = TRUE;

        }    


    else

#endif
    
        {

        //**********************************************************************
        // Our old documentation says that 'psoMsk' may be NULL, which means
        // that the pointer is transparent.  Well, trust me, that's wrong.
        // I've checked GDI's code, and it will never pass us a NULL psoMsk:
        //**********************************************************************

        AlphaBlendedCursor = FALSE;

        // psoMsk is for monochrome cursors
        //
        cx = psoMsk->sizlBitmap.cx;             // Note that 'sizlBitmap.cy' accounts
        cy = psoMsk->sizlBitmap.cy >> 1;        //   for the double height due to the
                                                //   inclusion of both the AND masks
                                                //   and the XOR masks.  For now, we're
                                                //   only interested in the true
                                                //   pointer dimensions, so we divide
                                                //   by 2.
        }
        
    ulReturnValue = (ULONG)(ppdev->pfnSetPointerShape(pso, psoMsk, psoColor, pxlo, x,y, xHot, yHot, cx, cy, AlphaBlendedCursor));

    if(ulReturnValue == SPS_DECLINE)
    {
        //**************************************************************************
        // Hide HW cursor going to SW cursor
        //**************************************************************************

        ppdev->pfnShowPointer(ppdev, FALSE);
    }

DoneDrvSetPointerShape:

    if( ulReturnValue == SPS_DECLINE)
    {
        ppdev->ulDriverNotSupportPanScan = TRUE;
        ppdev->bIsSWPointer              = TRUE;

#if _WIN32_WINNT >= 0x0500
        // punt back to GDI and keep track of mouse movement in DrvMovePointer
        if(EngSetPointerShape(pso, psoMsk, psoColor, pxlo, xHot, yHot, x, y, prcl, fl) != SPS_ACCEPT_EXCLUDE)
        {
            ppdev->ulPuntCursorToGDI = FALSE;
        }
        else
        {
            ulReturnValue = SPS_ACCEPT_NOEXCLUDE;
            ppdev->ulDriverNotSupportPanScan = FALSE;
            ppdev->ulPuntCursorToGDI = TRUE;
        }
#endif

    }
    else
    {
        ppdev->ulDriverNotSupportPanScan = FALSE;
        ppdev->bIsSWPointer              = FALSE;
    }

    return(ulReturnValue);

    }

//******************************************************************************
//
//  Function:   vDisablePointer
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID vDisablePointer(
PDEV*   ppdev)
    {
        if (ppdev->flCaps & CAPS_SW_POINTER)
            return;

        if(ppdev->pfnEnablePointer)
        {
            ppdev->pfnEnablePointer(ppdev, FALSE);
        }
    }

//******************************************************************************
//
//  Function:   vAssertModePointer
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID vAssertModePointer(
PDEV*   ppdev,
BOOL    bEnable)

    {
    ULONG*  pulDst;
    LONG    i;
    LONG    lPointerShape;

    //**************************************************************************
    // With a software pointer, we don't have to do anything.
    //**************************************************************************

    if (ppdev->flCaps & CAPS_SW_POINTER)
        return;

    //**************************************************************************
    // Turn off the hardware cursor
    //**************************************************************************

    if(ppdev->pfnEnablePointer)
        ppdev->pfnEnablePointer(ppdev, bEnable);

    }

//******************************************************************************
//
//  Function:   bEnablePointer
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL bEnablePointer(
PDEV*   ppdev)

    {
    RECTL       rclDraw;
    RECTL       rclBank;
    LONG        iBank;
    LONG        cjOffset;
    LONG        cjOffsetInBank;

    //**************************************************************************
    // With a software pointer, we don't have to do anything.
    //**************************************************************************

    if (ppdev->flCaps & CAPS_SW_POINTER)
        return(TRUE);

    //**************************************************************************
    // Check what type of hardware cursor to enable (NV1/NV3)
    //**************************************************************************

    ppdev->pfnShowPointer     = ShowPointer;
#ifdef NV3
    ppdev->pfnMovePointer     = NV3MovePointer;
    ppdev->pfnSetPointerShape = NV3SetPointerShape;
    ppdev->pfnEnablePointer   = NV3EnablePointer;
#else
    if (ppdev->CurrentClass.VideoLutCursorDac == NV15_VIDEO_LUT_CURSOR_DAC ||
        ppdev->CurrentClass.VideoLutCursorDac == NV10_VIDEO_LUT_CURSOR_DAC ||
        ppdev->CurrentClass.VideoLutCursorDac == NV05_VIDEO_LUT_CURSOR_DAC ||
        ppdev->CurrentClass.VideoLutCursorDac == NV04_VIDEO_LUT_CURSOR_DAC) {
        ppdev->pfnMovePointer     = NVMovePointer;
        ppdev->pfnEnablePointer   = NVEnablePointer;
    } else {
        // Error
        ASSERTDD(0,"No supported VIDEO_LUT_CURSOR_DAC object created");
    }

    switch(ppdev->CurrentClass.VideoLutCursorDac)
    {
        case NV10_VIDEO_LUT_CURSOR_DAC:
        case NV05_VIDEO_LUT_CURSOR_DAC:
        case NV04_VIDEO_LUT_CURSOR_DAC:
            ppdev->pfnSetPointerShape = NVSetPointerShapeOn16BppHwMode;
            break;
        default:
            // all NV newer chips should be able to handle 32Bpp cursor.
            ppdev->pfnSetPointerShape = NVSetPointerShapeOn32BppHwMode;
            break;
    }
#endif // NV3
    
    //**************************************************************************
    // Actually turn on the pointer:
    //**************************************************************************

    vAssertModePointer(ppdev, TRUE);

    DISPDBG((5, "Passed bEnablePointer"));

    return(TRUE);
    }



//******************************************************************************
//
//  Function:   ShowPointer
//
//  Routine Description:
//
//      Show or hide the hardware pointer.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID ShowPointer(
PDEV*   ppdev,
BOOL    bShow)      // If TRUE, show the pointer.  If FALSE, hide the pointer.

    {
    LONG    x;
    LONG    y;

    if (bShow)
        {
        //**********************************************************************
        // Make the hardware pointer visible:
        //**********************************************************************

        x  = ppdev->xPointer;
        y  = ppdev->yPointer;
        }

    else

        {
        //**********************************************************************
        // Move the hardware pointer off-screen so that it doesn't flash
        // in the old position when we finally turn it back on:
        //**********************************************************************

        x  = ppdev->cxScreen + 64;
        y  = ppdev->cyScreen + 64;
        }

    //*************************************************************************
    // Enable/Disable cursor by moving to new position
    //*************************************************************************

    ppdev->pfnMovePointer(ppdev, x, y);
    }

