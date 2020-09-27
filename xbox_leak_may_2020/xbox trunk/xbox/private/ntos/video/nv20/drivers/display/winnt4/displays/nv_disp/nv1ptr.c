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
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"

#include "sgs_dac.h"


#define OFFSET_SGS_DAC_UPORT_WRITE_PAL_ADDR    0
#define OFFSET_SGS_DAC_UPORT_COLOR             1
#define OFFSET_SGS_DAC_UPORT_PIXEL_MASK        2
#define OFFSET_SGS_DAC_UPORT_READ_PAL_ADDR     3
#define OFFSET_SGS_DAC_UPORT_INDEX_LO          4
#define OFFSET_SGS_DAC_UPORT_INDEX_HI          5
#define OFFSET_SGS_DAC_UPORT_INDEX_DATA        6
#define OFFSET_SGS_DAC_UPORT_GAMEPORT          7


//******************************************************************************
//
//  Function:   NV1EnablePointer
//
//  Routine Description:
//
//      Get the hardware ready to use the Brooktree 485 hardware pointer.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV1EnablePointer(
PDEV*               ppdev,
BOOL    bEnable
)
    {
    // Nothing to do, already initialized in miniport
    }


//******************************************************************************
//
//  Function:   NV1MovePointer
//
//  Routine Description:
//
//      Move the NV1 hardware pointer.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV1MovePointer(
PDEV*   ppdev,
LONG    x,
LONG    y)

    {
    LONG    dx;
    LONG    dy;

    volatile ULONG *DACRegs;
    volatile ULONG *SGS_DAC_Index_Lo;
    volatile ULONG *SGS_DAC_Index_Hi;
    volatile ULONG *SGS_DAC_Index_Data;
    ULONG OutData;

    //**************************************************************************
    // Adjustment for cursor Hot spot
    //**************************************************************************

    x -= ppdev->xPointerHot;
    y -= ppdev->yPointerHot;

    //**************************************************************************
    // Get pointer to important SGS Dac cursor registers
    //**************************************************************************

    DACRegs = ppdev->DACRegs;

    SGS_DAC_Index_Lo = (volatile ULONG *)(DACRegs+OFFSET_SGS_DAC_UPORT_INDEX_LO);
    SGS_DAC_Index_Hi = (volatile ULONG *)(DACRegs+OFFSET_SGS_DAC_UPORT_INDEX_HI);
    SGS_DAC_Index_Data = (volatile ULONG *)(DACRegs+OFFSET_SGS_DAC_UPORT_INDEX_DATA);

    //**************************************************************************
    // Set cursor position X coordinate
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_POS_X_LO & 0xff);
    *SGS_DAC_Index_Lo = OutData;

    OutData=((SGS_DAC_CURSOR_POS_X_LO>>8) & 0xff);
    *SGS_DAC_Index_Hi = OutData;

    OutData=x;
    *SGS_DAC_Index_Data= OutData;
    *SGS_DAC_Index_Data= OutData>>8;

    //**************************************************************************
    // Set cursor position Y coordinate
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_POS_Y_LO & 0xff);
    *SGS_DAC_Index_Lo = OutData;

    OutData=((SGS_DAC_CURSOR_POS_Y_LO>>8) & 0xff);
    *SGS_DAC_Index_Hi = OutData;

    OutData=y;
    *SGS_DAC_Index_Data= OutData;
    *SGS_DAC_Index_Data= OutData>>8;

    return;

    }

//******************************************************************************
//
//  Function:   NV1ShowPointer
//
//  Routine Description:
//
//      Show or hide the NV1 hardware pointer.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV1ShowPointer(
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

    NV1MovePointer(ppdev,x,y);
    }


//******************************************************************************
//
//  Function:   NV1SetPointerShape
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

VOID NV1SetPointerShape(
SURFOBJ*    pso,
LONG        x,              // Relative coordinates
LONG        y,              // Relative coordinates
LONG        xHot,
LONG        yHot,
BYTE*       pjShape)

    {
    PDEV*   ppdev;
    ULONG*  pulSrc;
    ULONG*  pulDst;
    LONG    i;

    volatile ULONG *DACRegs;
    volatile ULONG *SGS_DAC_Index_Lo;
    volatile ULONG *SGS_DAC_Index_Hi;
    volatile ULONG *SGS_DAC_Index_Data;
    ULONG OutData;
    BYTE*   pbImage;

    ppdev    = (PDEV*) pso->dhpdev;

    DACRegs = ppdev->DACRegs;

    SGS_DAC_Index_Lo = (volatile ULONG *)(DACRegs+OFFSET_SGS_DAC_UPORT_INDEX_LO);
    SGS_DAC_Index_Hi = (volatile ULONG *)(DACRegs+OFFSET_SGS_DAC_UPORT_INDEX_HI);
    SGS_DAC_Index_Data = (volatile ULONG *)(DACRegs+OFFSET_SGS_DAC_UPORT_INDEX_DATA);

    //**************************************************************************
    // Set SGS cursor image plane 1 (AND MASK plane)
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_PLANE_1_WRITE_0 & 0xff);
    *SGS_DAC_Index_Lo = OutData;

    OutData=((SGS_DAC_CURSOR_PLANE_1_WRITE_0>>8) & 0xff);
    *SGS_DAC_Index_Hi = OutData;

    pbImage = pjShape;
    for (i=0;i<128;i++)
        *SGS_DAC_Index_Data= *pbImage++;

    //**************************************************************************
    // Set SGS cursor image plane 0 (XOR MASK plane)
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_PLANE_0_WRITE_0 & 0xff);
    *SGS_DAC_Index_Lo = OutData;

    OutData=((SGS_DAC_CURSOR_PLANE_0_WRITE_0>>8) & 0xff);
    *SGS_DAC_Index_Hi = OutData;

    for (i=0;i<128;i++)
        *SGS_DAC_Index_Data= *pbImage++;

    //**************************************************************************
    // 1. Hide the current pointer.
    //**************************************************************************

    //**************************************************************************
    // 2. Wait until the vertical retrace is done.
    // --
    //
    // If we don't wait for vertical retrace here, the S3 sometimes ignores
    // the setting of the new pointer position:
    //**************************************************************************

    //**************************************************************************
    // 3. Set the new pointer position.
    // --
    //**************************************************************************

    //**************************************************************************
    // 4. Download the new pointer shape.
    //**************************************************************************

    ppdev->xPointerHot = xHot;
    ppdev->yPointerHot = yHot;

    NV1MovePointer(ppdev, x, y);


    }

