//******************************Module*Header***********************************
//
// Module Name: NVPTR.C
//
// This module contains the hardware pointer support for the display
// driver.  Supports the following classes:
//          NV04_VIDEO_LUT_CURSOR_DAC
//          NV05_VIDEO_LUT_CURSOR_DAC
//          NV10_VIDEO_LUT_CURSOR_DAC
//          NV15_VIDEO_LUT_CURSOR_DAC
//
// Does not support NV3!!
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997-2000 NVidia Corporation. All Rights Reserved.          *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"
#include "ddminint.h"

#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
    #include "ddmini.h"
#endif

#include "dmamacro.h"

#include "nvsubch.h"
#include "nvalpha.h"
#include "nvapi.h"
#include "nvcm.h"

#define WORK_AROUND_NV11_DIRTHER                                        \
{                                                                       \
    if(ppdev->ulWorkAroundHwFlag &  NV_WORKAROUND_NV11RevB_DIRTHERING)  \
    {                                                                   \
        return(SPS_DECLINE);                                            \
    }                                                                   \
}

#ifndef NV3

//*****************************************************************************
// Externs
//*****************************************************************************

extern VOID NV4_DmaPushSend(PDEV *);
extern VOID NV_DmaPush_CheckWrapped(PDEV *, ULONG );
extern VOID NV_DmaPush_Wrap(PDEV *, ULONG );

#define CURSOR_PIXEL_SIZE 4

#define DESKTOP_MOVING_RATE 20    

#define USE_CURSOR_REGISTER 1
#ifdef USE_CURSOR_REGISTER //[

/*
** 1/10/01 jbaldwin
** This direct register write gains ~2% performance on 2D Winbench 99 BG.  The
** PRAMDAC_CU_START_POS_Reg register is identical from NV3 thru NV20 so this
** direct register write bypassing the class interface isn't quite as bad as
** it would otherwise be.
*/
#define SET_CURSOR_LOCATION(ppdev, ulHead, x, y)                            \
{                                                                           \
    volatile ULONG * PRAMDAC_CU_START_POS_Reg;                              \
                                                                            \
    PRAMDAC_CU_START_POS_Reg = ppdev->PRAMDACRegs + ((ulHead) * 0x2000/4);  \
                                                                            \
    *PRAMDAC_CU_START_POS_Reg = (((x) & 0xffff) | ((y) << 16));             \
}

#else // USE_CURSOR_REGISTER ][

#define USE_DMC_CURSOR 1
// Send the new location directly to HW; new code
#if USE_DMC_CURSOR
#define SET_CURSOR_LOCATION(ppdev, ulHead, x, y)                            \
if (                                                                        \
    NvDirectMethodCall(                                                     \
        ppdev->hDriver,                                                     \
        ppdev->hClient,                                                     \
        ppdev->hDmaChannel,                                                 \
        NV_VIDEO_LUT_CURSOR_DAC+ulHead,                                     \
        NV046_SET_CURSOR_POINT,                                             \
        (x & 0xffff) | (y << 16)                                            \
    ) != NVOS1D_STATUS_SUCCESS                                              \
)                                                                           \
{                                                                           \
    DISPDBG((2, "NVDD: Cannot perform cursor move on %d head", ulHead));    \
}

#else
// Send the new location of cursor into DMA push buffer;  old code
// !!! This code has not been tested yet !!!
#define SET_CURSOR_LOCATION(ppdev, ulHead, x, y)                                                            \
{                                                                                                           \
    DECLARE_DMA_FIFO;                                                                                       \
    INIT_LOCAL_DMA_FIFO;                                                                                    \
    NV_DMAPUSH_CHECKFREE( ((ULONG)(4)));                                                                    \
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC+ulhead);   \
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CURSOR_POINT, (x & 0xffff) | (y << 16) );        \
    UPDATE_PDEV_DMA_COUNT;                                                                                  \
    NV4_DmaPushSend(ppdev);                                                                                 \
}

#endif

#endif // USE_CURSOR_REGISTER ]

//******************************************************************************
//
//  Function:   NvAllocateCursorMem
//
//  Routine Description:
//          Allocating the memory for cursor
//      
//
//  Arguments:
//          ppdev - display driver structure
//          ulBufIndex - double buffer.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL NvAllocateCursorMem(PDEV* ppdev, ULONG ulBufIndex)
{
    if(!ppdev->ulCursorMemOffset[ulBufIndex])
    {
        NvU32 dwStatus;

        // Don't worry offset alignment because RM should take care TYPE_CURSOR
        NVHEAP_ALLOC(dwStatus, ppdev->ulCursorMemOffset[ulBufIndex],  ppdev->ulMaxHwCursorSize * ppdev->ulMaxHwCursorSize * CURSOR_PIXEL_SIZE, TYPE_CURSOR);

        if (dwStatus!=0)
        {
            DISPDBG((2, "NVEnablePointer: Cannot allocate cursor memory"));
            return (FALSE);
        }

        if(NvAllocContextDma(ppdev->hDriver,
                             ppdev->hClient,
                             NV_CONTEXT_DMA_CURSOR+ulBufIndex,
                             NV01_CONTEXT_DMA,
                             NVOS03_FLAGS_ACCESS_READ_WRITE,
                             (PVOID)(ppdev->pjFrameBufbase + ppdev->ulCursorMemOffset[ulBufIndex]),
                             ppdev->ulMaxHwCursorSize * ppdev->ulMaxHwCursorSize * CURSOR_PIXEL_SIZE - 1
                             ))
        {
            DISPDBG((1, "NVEnablePointer: Cannot allocate dma in memory context for cursor"));
            NVHEAP_FREE(ppdev->ulCursorMemOffset[ulBufIndex]);
            ppdev->ulCursorMemOffset[ulBufIndex] = 0;
            return (FALSE);
        }
    }
    return(TRUE);
}

//******************************************************************************
//
//  Function:   NvFreeCursorMem
//
//  Routine Description:
//         Free the cursor memory
//      
//
//  Arguments:
//          ppdev - display driver structure
//          ulBufIndex - double buffer.
//
//
//  Return Value:
//
//      None.
//
//******************************************************************************
void NvFreeCursorMem(PDEV* ppdev, ULONG ulBufIndex)
{

        if(ppdev->ulCursorMemOffset[ulBufIndex])
        {
            if (!(ppdev->flCaps & CAPS_SW_POINTER))
            {
                ppdev->pfnShowPointer(ppdev,FALSE);
                ppdev->pfnWaitEngineBusy(ppdev);
                if (NvFree(  ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_CONTEXT_DMA_CURSOR+ulBufIndex) !=
                    NVOS00_STATUS_SUCCESS )
                {
                    DISPDBG((0, "EnablePointer: Cannot free dma in memory context for cursor"));
                }
                NVHEAP_FREE(ppdev->ulCursorMemOffset[ulBufIndex]);
                ppdev->ulCursorMemOffset[ulBufIndex] = 0;
            }
        }

}




//******************************************************************************
//
//  Function:   NVEnablePointer
//
//  Routine Description:
//
//      Handling Allocate / Free memory for the cursor
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************
VOID NVEnablePointer
(
PDEV*               ppdev,
BOOL    bEnable
)
{
    ULONG i, nBuffers;
    ULONG ulHead;
    DECLARE_DMA_FIFO;

    INIT_LOCAL_DMA_FIFO;

    if (bEnable)
    {
        for(nBuffers = 0; nBuffers < 2; nBuffers++)
        {
            if(!NvAllocateCursorMem(ppdev, nBuffers))
            {
                ppdev->flCaps |= CAPS_SW_POINTER;
                return;
            }

        	for(i = 0; i < ppdev->ulNumberDacsActive; i++)
        	{
                ulHead = ppdev->ulDeviceDisplay[i];
                NV_DMAPUSH_CHECKFREE( ((ULONG)(4)));  
                NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC + ulHead);
                NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CONTEXT_DMA_CURSOR(nBuffers), NV_CONTEXT_DMA_CURSOR+nBuffers);
            }
        }
        UPDATE_PDEV_DMA_COUNT;

        //******************************************************************
        // Send data on thru to the DMA push buffer
        //******************************************************************

        if(ppdev->bEnabled)
            NV4_DmaPushSend(ppdev);
        
        // Wait for it to complete.
        ppdev->pfnWaitEngineBusy(ppdev);

    }
    else
    {
        for(i = 0; i < 2; i++)
            NvFreeCursorMem(ppdev, i);
    }
}

//******************************************************************************
//
//  Function:   NvCalNewDisplayWindow
//
//  Routine Description:
//
//      Calculating the display window based on the location of cursor pointer.
//
//  Arguments:
//
//  Return Value:
//
//      True - Moved; FALSE - The location is within Window; Not need to move.
//
//******************************************************************************
BOOLEAN NvCalNewDisplayWindow(
LONG x, LONG y, 
LONG lOutsideLeft, LONG lOutsideTop, LONG lOutsideRight, LONG lOutsideBottom,
ULONG ulMoveDisplayDx, ULONG ulMoveDisplayDy,
RECTL * pWindow)
{

    LONG lNewX1, lNewX2, lNewY1, lNewY2 ;
    BOOLEAN bRet = FALSE;

    lNewX2 = lNewY2 = 0;

    if(x == lOutsideLeft)
    {
        // Reset the X coordinate
        pWindow->right = lOutsideLeft + (pWindow->right - pWindow->left);
        pWindow->left  = lOutsideLeft;
        bRet = TRUE;
    }
    else if(x > pWindow->left)
    {
        if(x < pWindow->right)
        {
            // Inside the view of Display Screen;
            // Do nothing for X;
        }
        else
        {
            if(pWindow->right < lOutsideRight)
            {
                // MOVE display to right
                lNewX2 = pWindow->right + ulMoveDisplayDx;
                while(x > lNewX2)
                    lNewX2 += ulMoveDisplayDx;
                if(lNewX2 > lOutsideRight)
                    lNewX2 = lOutsideRight;
                lNewX1 = lNewX2 - (pWindow->right - pWindow->left);
                pWindow->left = lNewX1;
                pWindow->right = lNewX2;
                bRet = TRUE;
            }
        }
    }
    else
    {
        // MOVE display to left
        if(pWindow->left > lOutsideLeft)
        {
            lNewX1 = pWindow->left - ulMoveDisplayDx;
            while(x < lNewX1)
                lNewX1 -= ulMoveDisplayDx;
            if(lNewX1 < lOutsideLeft)
                lNewX1 = lOutsideLeft;
            lNewX2 = lNewX1 + (pWindow->right - pWindow->left);
            pWindow->left = lNewX1;
            pWindow->right = lNewX2;
            bRet = TRUE;
        }
    }

    // HW restriction Enforcement: Since image offset value is a multiple of 4 bytes.  
    // and this rountine does not deal with 8bpp, 16bpp or 32bpp,
    // set to 4 pixels alignment in order to satisfy HW restriction.
    if(bRet)
        pWindow->left = (pWindow->left + 3) & ~(0x03);

    if(y == lOutsideTop)
    {
        // Reset the Y coordinate
        pWindow->bottom = lOutsideTop + (pWindow->bottom - pWindow->top);
        pWindow->top = lOutsideTop;
        bRet = TRUE;
    }
    else if(y > pWindow->top)
    {
        if(y < (pWindow->bottom) )
        {
            // Inside the view of Display Screen;
            // Do nothing for Y;
        }
        else
        {
            if(pWindow->bottom < lOutsideBottom)
            {
                // Move display down
                lNewY2 = pWindow->bottom + ulMoveDisplayDy;
                while(y > lNewY2)
                    lNewY2 += ulMoveDisplayDy;
                if(lNewY2 > lOutsideBottom)
                    lNewY2 = lOutsideBottom;
                lNewY1 = lNewY2 - (pWindow->bottom - pWindow->top);
                pWindow->top = lNewY1;
                pWindow->bottom = lNewY2;
                bRet = TRUE;
            }
        }
    }
    else
    {
        // MOVE display up
        if(pWindow->top > lOutsideTop)
        {
            lNewY1 = pWindow->top - ulMoveDisplayDy;
            while(y < lNewY1)
                lNewY1 -= ulMoveDisplayDy;
            if(lNewY1 < 0)
                lNewY1 = 0;
            lNewY2 = lNewY1 + (pWindow->bottom - pWindow->top);
            pWindow->top = lNewY1;
            pWindow->bottom = lNewY2;
            bRet = TRUE;
        }
    }

    return(bRet);
}


//******************************************************************************
//
//  Function:   NVMoveDisplayWindow
//
//  Routine Description:
//
//      Move the display window to new location.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************
void __cdecl NVMoveDisplayWindow(
PDEV*   ppdev,
ULONG   ulHead,
RECTL * prclCurrentDisplay)
{
    ULONG ulPanOffset;
    DECLARE_DMA_FIFO;
    INIT_LOCAL_DMA_FIFO;

    NV_DMAPUSH_CHECKFREE( ((ULONG)(10)));  
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC+ulHead);
    ulPanOffset = prclCurrentDisplay->left * ppdev->cjPelSize + prclCurrentDisplay->top * ppdev->lDelta 
                                    + ppdev->ulPrimarySurfaceOffset - ppdev->ulHeadDisplayOffset[ulHead];
    while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
    ((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status = NV_IN_PROGRESS;
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_PAN_OFFSET, ulPanOffset);
    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_IMAGE_FORMAT(0), ppdev->lDelta |
        NV046_SET_IMAGE_FORMAT_NOTIFY_WRITE_ONLY << 31);
    //******************************************************************
    // Update global push buffer count
    //******************************************************************
    UPDATE_PDEV_DMA_COUNT;

    //******************************************************************
    // Send data on thru to the DMA push buffer
    //******************************************************************
    NV4_DmaPushSend(ppdev);

    while(((NvNotification *) (&(ppdev->Notifiers->VideoLutCursorDac[NV046_NOTIFIERS_SET_IMAGE(0)*sizeof(NvNotification)])))->status == NV_IN_PROGRESS);
    ppdev->pfnWaitEngineBusy(ppdev);

}

//******************************************************************************
//
//  Function:   NVMovePointer
//
//  Routine Description:
//
//      Move the NV hardware pointer.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NVMovePointer(
PDEV*   ppdev,
LONG    x,
LONG    y)

{
    RECTL * prclCurrentDisplay;
    RECTL * prclDisplayDesktop;
    LONG lOriginalX, lOriginalY;
    ULONG ulHead;
    LONG lOverlayDisabledHead = -1;

    ULONG ulSaveDesktopMode;

    lOriginalX, lOriginalY;

    if(x == -1)
    {
        x  = ppdev->cxScreen + 64;
        y  = ppdev->cyScreen + 64;
    }

    lOriginalX = x;
    lOriginalY = y;

    ulHead = ppdev->ulDeviceDisplay[0];
    prclCurrentDisplay = &ppdev->rclCurrentDisplay[ulHead];

#if (_WIN32_WINNT >= 0x0500) && defined(NVD3D) && !defined(_WIN64)
    ulSaveDesktopMode = ppdev->ulDesktopMode;

    if( (ppdev->pDriverData->vpp.dwOverlayFSNumSurfaces != 0))
    {
        // Disable pan & scan because Video is rendering on the head now.
        lOverlayDisabledHead =  ppdev->pDriverData->vpp.dwOverlayFSHead; 
        if( ulHead == (ULONG) lOverlayDisabledHead)
        {
            // Logical head 0
            ppdev->ulDesktopMode &= ~(NV_VIRTUAL_DESKTOP_1);
        }
        else
        {
            // Logical head 1
            ppdev->ulDesktopMode &= ~(NV_VIRTUAL_DESKTOP_2);
        }
    }
#endif
    if(ppdev->ulDesktopMode & NV_VIRTUAL_DESKTOP_1)
    {
        // Primary DAC
        prclDisplayDesktop = &ppdev->rclDisplayDesktop[ulHead];

        if( (x >= prclDisplayDesktop->left) && (x < prclDisplayDesktop->right) && 
            (y >= prclDisplayDesktop->top)  && (y < prclDisplayDesktop->bottom)) 
        {
            if (NvCalNewDisplayWindow(x,y, prclDisplayDesktop->left,  prclDisplayDesktop->top, 
                                         prclDisplayDesktop->right, prclDisplayDesktop->bottom, 
                                         ppdev->ulMoveDisplayDx, 
                                         ppdev->ulMoveDisplayDy, 
                                         prclCurrentDisplay))
            {
                NVMoveDisplayWindow(ppdev, ulHead, prclCurrentDisplay);
            }
        }
    }

    if(!ppdev->ulPuntCursorToGDI)
    {
        // Don't worry to move the cursor pointer; GDI Software cursor pointer
        x -= ppdev->xPointerHot;
        x -= prclCurrentDisplay->left;
        y -= ppdev->yPointerHot;
        y -= prclCurrentDisplay->top;
        
        if( ulHead != (ULONG) lOverlayDisabledHead)
        {
            SET_CURSOR_LOCATION(ppdev, ulHead, x, y);
        }
    }

    if(ppdev->ulDesktopMode & NV_TWO_DACS)
    {
        x = lOriginalX;
        y = lOriginalY;

        ulHead = ppdev->ulDeviceDisplay[1];
        prclCurrentDisplay = &ppdev->rclCurrentDisplay[ulHead];

        if(ppdev->ulDesktopMode & NV_VIRTUAL_DESKTOP_2)
        {
            // Secondary DAC
            prclDisplayDesktop = &ppdev->rclDisplayDesktop[ulHead];

            if( (x >= prclDisplayDesktop->left) && (x < prclDisplayDesktop->right) && 
                (y >= prclDisplayDesktop->top)  && (y < prclDisplayDesktop->bottom)) 
            {
                if (NvCalNewDisplayWindow(x,y, prclDisplayDesktop->left,  prclDisplayDesktop->top, 
                                         prclDisplayDesktop->right, prclDisplayDesktop->bottom, 
                                         ppdev->ulMoveDisplayDx, 
                                         ppdev->ulMoveDisplayDy, 
                                         prclCurrentDisplay))
                {
                    NVMoveDisplayWindow(ppdev, ulHead, prclCurrentDisplay);
                }
            }
        }

        if(!ppdev->ulPuntCursorToGDI)
        {
            // Don't worry to move the cursor pointer; GDI Software cursor pointer
            x -= ppdev->xPointerHot;
            x -= prclCurrentDisplay->left;
            y -= ppdev->yPointerHot;
            y -= prclCurrentDisplay->top;

            if( ulHead != (ULONG) lOverlayDisabledHead)
            {
                SET_CURSOR_LOCATION(ppdev, ulHead, x, y);
            }
        }

    }

#if (_WIN32_WINNT >= 0x0500) && defined(NVD3D) && !defined(_WIN64)
    // restore the Desktop mode
    ppdev->ulDesktopMode = ulSaveDesktopMode;
#endif
}

//******************************************************************************
//
//  Function:   NvSwMonoToHw16BppCursor
//
//  Routine Description:
//  Handle Mono cursor in 16Bpp HW cursor mode (Alpha1?Cur:Cur^Bkgnd).
//
// USING A1R5G5B5 16 bits format
// Translate to black and white format for DAC
// Expand 32 x 32 x 1  (monochrome source ) to 32 x 32 x 16 CursorColorImage
//
// We're going to take the requested pointer AND masks and XOR
// masks and combine them into our work buffer,
//
//              Monochrome Source
//              -----------------
//
//         Byte0   Byte1   Byte2   Byte3
//  0    |       |       |       |       |
//  1    |       |       |       |       |         (monochrome source)
//  2    |       |       |       |       |     Total of 128 bytes per plane
//  3    |       |       |       |       |
//                      etc..
//
//  31   |       |       |       |       |
//
//
//
//              Color Destination
//              -----------------
//
//         Word0   Word1   ....    Word31
//  0    |       |       |       |       |
//  1    |       |       |       |       |         (color destination)
//  2    |       |       |       |       |      Total 32*32*2 bytes
//  3    |       |       |       |       |
//                      etc..
//
//  31   |       |       |       |       |
//
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************
void Nv1BppToHw16BppCursor(LONG cx, LONG cy, BYTE* pjSrcScan, ULONG *pulDstScan, 
ULONG ulHwCursorWidth, LONG lSrcDelta, LONG lDstDelta)
{
    int i, j, k;
    BYTE    bMask, bMono;
    ULONG ulDwordCount, ulColor, ulTemp, ulTest;
    LONG    lPixelCountPerLine;

    for(i = 0; i < cy; i++)
    { 
        ulDwordCount = 0;
        lPixelCountPerLine = 0;
        for(j = 0; j < lDstDelta && lPixelCountPerLine < cx; j++)            // in term of bytes
        {
            bMask = pjSrcScan[i * lSrcDelta + j];           // Get a Mask byte (8 pixels)
            bMono = pjSrcScan[ (cy + i) * lSrcDelta + j];   // Get a Mono byte (8 pixels)

            //**************************************************************
            // Now combine the masks according to the AND-XOR Microsoft convention
            //
            //  ('mask' value)      ('mono' value)
            //  AND mask value      XOR mask value      Result on screen
            //  --------------      --------------      ----------------
            //          0               0               Black
            //          0               1               White
            //          1               0               Transparent, pixel unchanged
            //          1               1               Inverts the pixel color
            //
            // We'll parse the pixels from right to left in source
            // (Bit 0 is leftmost in mono image).  This will cause
            // us to bit flip the cursor image and draw it correctly
            //**************************************************************

            ulTest = 0x80;
            for (k = 0; k < 8; k++, ulTest >>= 1)
            {
                if (ulTest & bMono)        // Test this bit
                    ulColor = 0x7fff;     // White
                else
                    ulColor = 0;          // Black

                if (!(ulTest & bMask))
                    ulColor |= 0x8000;    // Not transparent


                //**********************************************************
                // Store 16 bits for NV cursor image data
                //**********************************************************
                if(k & 1)
                {
                    ulTemp = ((ULONG) ulColor << 16) | ulTemp;
                    *pulDstScan++ = ulTemp;
                    ulDwordCount++;
                }
                else
                {
                    ulTemp = ulColor;
                }
                // Before next pixel, 1 byte (mono) is for 8 pixels.  
                // When it is over the width of cursor, quit but check last pixel
                // whether it has been written.
                if( (++lPixelCountPerLine) > cx)
                {
                    if(!(k & 1))
                    {
                        *pulDstScan++ = ulTemp;
                        ulDwordCount++;
                    }
                    break;
                }
            }
        }   // End of j loop

        // make HW happy, fill remaining DWORD to ulHwCursorWidth
        while(ulDwordCount < (ulHwCursorWidth / 2))
        {
            *pulDstScan++ = 0x0;
            ulDwordCount++;
        }
    }   // End of i loop
    for(; i < (LONG) ulHwCursorWidth; i++)
    {
        for(k = 0; k < (LONG) ulHwCursorWidth; k++)
        {
            *pulDstScan++ = 0x0;
        }
    }
}

//******************************************************************************
ULONG Nv8BppToHw16BppCursor(PDEV* ppdev, PPALETTEENTRY pPal, XLATEOBJ*   pxlo, LONG cx, LONG cy, 
BYTE* pjSrcColor, BYTE* pjSrcColorMask, ULONG *pulDstScan, ULONG ulHwCursorWidth, 
LONG lSrcDelta, LONG lPitch)
{
    int i, j, k;
    ULONG ulMask;
    ULONG ulDwordCount, ulColor, ulTemp, ulTest;
    WORD    red;
    WORD    green;
    WORD    blue;
    PALETTEENTRY pXlatePal[256];

    //**************************************************************
    // We gotta look up the color from the palette to
    // convert to RGB. Try to get info from the XLATEOBJ
    // first.
    //**************************************************************
//    pPal = ppdev->pPal;     // Default to using pal from PDEV
    if (pxlo != NULL)
    {
        if (XLATEOBJ_cGetPalette(pxlo, XO_SRCPALETTE, 256, NULL) == 256)
        {
            //******************************************************
            // Get the RGB palette.
            //******************************************************
            if (XLATEOBJ_cGetPalette(pxlo, XO_SRCPALETTE, 256, (PULONG) pXlatePal) != 256)
                pPal = pXlatePal;
        }
    }
    for (i = 0; i < cy; i++)
    {
        ulMask = *(PULONG) (pjSrcColorMask + i*lSrcDelta);     // Get 8 AND bits
        ulTest = 0x00000080;
        ulDwordCount = 0;
        for (k = 0; k < cx; k++, ulTest >>=1)
        {
            if (!(k & 7))
                ulTest = 0x00000080 << k;
            ulColor = *((PBYTE) (pjSrcColor + i*lPitch + k)); // Get 15 bits of color
            blue  = pPal[ulColor].peBlue;
            green = pPal[ulColor].peGreen;
            red   = pPal[ulColor].peRed;
            ulColor = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);
            if (!(ulTest & ulMask))
            {
                ulColor |= 0x8000;              // Not transparent
            }
            else
            {
                if(ulColor == 0x7FFF)
                {
                    // inverted background color
                    WORK_AROUND_NV11_DIRTHER;
                }
                else
                {
				        // Transparent
                }
            }

            //******************************************************
            // Store 16 bits for NV cursor image data
            //******************************************************
            if(k & 1)
            {
                ulTemp = ((ULONG) ulColor << 16) | ulTemp;
                *pulDstScan++ = ulTemp;
                ulDwordCount++;
            }
            else
            {
                ulTemp = ulColor;
            }
        }
        // make HW happy, fill remaining DWORD to ulHwCursorWidth
        while(ulDwordCount < (ulHwCursorWidth / 2))
        {
            *pulDstScan++ = 0x0;
            ulDwordCount++;
        }
    }
    for(; i < (LONG) ulHwCursorWidth; i++)
    {
        for(k = 0; k < (LONG) ulHwCursorWidth; k++)
        {
            *pulDstScan++ = 0x0;
        }
    }
    return(SPS_ACCEPT_NOEXCLUDE);
}

//******************************************************************************
ULONG Nv16BppToHw16BppCursor(PDEV* ppdev, LONG cx, LONG cy, BYTE* pjSrcColor, BYTE* pjSrcColorMask, 
    LONG lSrcDelta, LONG lPitch, ULONG*  pulDstScan, ULONG ulHwCursorWidth, FLONG flGreen)
{
    ULONG ulMask, ulTest;
    WORD    red;
    WORD    green;
    WORD    blue;
    LONG    i,j,k;
    ULONG*  pulTemp;
    ULONG   ulTemp, ulColor, ulDwordCount;

    for (i = 0; i < cy; i++)
    {
        ulMask = *(PULONG) (pjSrcColorMask + i*lSrcDelta);     // Get 8 AND bits
        ulTest = 0x00000080;
        ulDwordCount = 0;
        for (k = 0; k < cx; k++, ulTest >>=1)
        {
           if (!(k & 7))
               ulTest = 0x00000080 << k;

           if (flGreen == 0x03e0)
           {
                //**************************************************
                // For 555: We can use the source color as is
                //          since the color cursor is always 555
                //**************************************************
                ulColor = *((PUSHORT) (pjSrcColor + i*lPitch + k*2)); // Get 15 bits of color
           }
           else
           {
                //**************************************************
                // For 565: Convert from 565 bitmap format to 555
                //          since the color cursor is always 555
                //**************************************************
                ulColor = *((PUSHORT) (pjSrcColor + i*lPitch + k*2)); // Get 16 bits of color
                red   = (WORD)(ulColor & 0xf800); // 5 bits
                green = (WORD)(ulColor & 0x07c0); // Just take top 5 of 6 bits !
                blue  = (WORD)(ulColor & 0x001f); // 5 bits
                ulColor = (  (red >> 1 ) | (green >> 1 ) | (blue) );
           }

           if (!(ulTest & ulMask))
                 ulColor |= 0x8000;                // Not transparent
           else
           {
                if(ulColor == 0x7FFF)
                {
                    // inverted background color
                    WORK_AROUND_NV11_DIRTHER;
                }
                else
                {
				        // Transparent
                }
           }
          

           //******************************************************
           // Store 16 bits for NV cursor image data
           //******************************************************
            if(k & 1)
            {
                ulTemp = ((ULONG) ulColor << 16) | ulTemp;
                *pulDstScan++ = ulTemp;
                ulDwordCount++;
            }
            else
            {
                ulTemp = ulColor;
            }
        }
        // make HW happy, fill remaining DWORD to ulHwCursorWidth
        while(ulDwordCount < (ulHwCursorWidth / 2))
        {
            *pulDstScan++ = 0x0;
            ulDwordCount++;
        }
    }
    for(; i < (LONG) ulHwCursorWidth; i++)
    {
        for(k = 0; k < (LONG) ulHwCursorWidth; k++)
        {
            *pulDstScan++ = 0x0;
        }
    }

    return(SPS_ACCEPT_NOEXCLUDE);
}


//******************************************************************************
//
//  Function:   NvHw16BppCursor
//
//  Routine Description:
//
//      The cursor has some pixels to be inverted back ground color
//      handle it in 16BPP mode of cursor
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************
void NvHw16BppCursor(LONG cx, LONG cy, BYTE* pjSrcColor, BYTE* pjSrcColorMask, 
    LONG lSrcDelta, LONG lPitch, ULONG*  pulDstScan, ULONG ulHwCursorWidth, BOOL AlphaBlendedCursor)
{

    BYTE    bMask, bTest;
    WORD    wRed, wGreen, wBlue;
    LONG    i,j,k;
    ULONG*  pulTemp;
    ULONG   ulTemp, ulColor, ulDwordCount;
    LONG    lPixelCountPerLine;


    for (i = 0; i < cy; i++) 
    {
        ulDwordCount = 0;
        lPixelCountPerLine = 0;
        pulTemp = (ULONG *)(pjSrcColor + i * lPitch);
        for (j = 0; j < cx ; j += 8) 
        {
            bMask = 0;
            if(!AlphaBlendedCursor)
            {
                // Not a alpha cursor; use get the mask
                bMask = *(pjSrcColorMask + i*lSrcDelta + j / 8);  
            }

            bTest = 0x80;
            for(k = 0; k < 8; k++, bTest >>= 1) 
            {
                ulColor = *pulTemp++;

                if(AlphaBlendedCursor)
                {
                    // Check alpha value of each pixel to determinie the transparenet
                    if( (ulColor & 0xFF000000) == 0xFF000000)
                    {
                        // New Cursor Color
                    }
                    else
                    {
                        // Really want to use the backgroup color
                        // clear the mask bit so that it will get the backgroup color.
                        bMask |= bTest;
                    }
                }

                if(!(bTest & bMask))
                {
                    wRed   = (WORD)(((ulColor & 0xFF0000) >> 16) * 32 / 256); // 5 bits
                    wRed   <<= 10;
                    wGreen = (WORD)(((ulColor & 0x00FF00) >> 8 ) * 32 / 256); // 5 bits
                    wGreen <<= 5;
                    wBlue  = (WORD)(((ulColor & 0x0000FF)      ) * 32 / 256); // 5 bits
                
                    ulColor = 0x8000 | wRed | wGreen | wBlue;
                }
                else
                {
				    if (ulColor == 0x00FFFFFF)
					{
                        // inverted background color
                        ulColor = 0x7FFF; 
					}
					else 
					{
				        // Transparent
						ulColor = 0;
					}
                }

                //**********************************************************
                // Store 16 bits for NV cursor image data
                //**********************************************************
                if(k & 1)
                {
                    ulTemp = ((ULONG) ulColor << 16) | ulTemp;
                    *pulDstScan++ = ulTemp;
                    ulDwordCount++;
                }
                else
                {
                    ulTemp = ulColor;
                }

                // Before next pixel, 1 byte (mono) is for 8 pixels.  
                // When it is over the width of cursor, quit but check last pixel
                // whether it has been written.
                if( (++lPixelCountPerLine) > cx)
                {
                    if(!(k & 1))
                    {
                        *pulDstScan++ = ulTemp;
                        ulDwordCount++;
                    }
                    break;
                }

            }
        }
        // make HW happy, fill remaining DWORD to ulHwCursorWidth
        while(ulDwordCount < (ulHwCursorWidth / 2))
        {
            *pulDstScan++ = 0x0;
            ulDwordCount++;
        }
    }

    for(; i < (LONG) ulHwCursorWidth; i++)
    {
        for(k = 0; k < (LONG) ulHwCursorWidth; k++)
        {
            *pulDstScan++ = 0x0;
        }
    }
    
}

//******************************************************************************
//
//  Function:   NVSetPointerShapeToHw
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
void NvSetPointerShapeToHw(PDEV * ppdev,  ULONG ulHwCursorWidth, ULONG ulHwCursorHeight, ULONG ulHwCursorFormat)
{
    DECLARE_DMA_FIFO;
    
    ppdev->ulCachedCursorWidth = ulHwCursorWidth;
    ppdev->ulCachedCursorFormat = ulHwCursorFormat;
	INIT_LOCAL_DMA_FIFO;
    NV_DMAPUSH_CHECKFREE( ((ULONG)(6)));  

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC+ppdev->ulDeviceDisplay[0]);

	NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CURSOR_IMAGE_OFFSET(ppdev->ulCursorCurBufIndex), 0);

    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CURSOR_IMAGE_FORMAT(ppdev->ulCursorCurBufIndex), 
    	 (ulHwCursorFormat << 16) |
	     (ulHwCursorHeight << 8) | ulHwCursorWidth);

    if(ppdev->ulDesktopMode & NV_TWO_DACS)
	{
        NV_DMAPUSH_CHECKFREE( ((ULONG)(8)));
	    NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(ppdev->ulCursorCurBufIndex) ,   NV_VIDEO_LUT_CURSOR_DAC+ppdev->ulDeviceDisplay[1]);

        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CURSOR_IMAGE_OFFSET(ppdev->ulCursorCurBufIndex), 0);

    	NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CURSOR_IMAGE_FORMAT(ppdev->ulCursorCurBufIndex), 
        	 (ulHwCursorFormat << 16) |
	         (ulHwCursorHeight << 8) | ulHwCursorWidth);

        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC);
	}

	//******************************************************************
    // Update global push buffer count
	//******************************************************************
    
    UPDATE_PDEV_DMA_COUNT;

	//******************************************************************
    // Send data on thru to the DMA push buffer
	//******************************************************************

	NV4_DmaPushSend(ppdev);
}



//******************************************************************************
//
//  Function:   NVSetPointerShapeOn16BppHwMode
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

ULONG NVSetPointerShapeOn16BppHwMode(
SURFOBJ*    pso,
SURFOBJ*    psoMsk,
SURFOBJ*    psoColor,
XLATEOBJ*   pxlo,
LONG        x,              // Relative coordinates
LONG        y,              // Relative coordinates
LONG        xHot,
LONG        yHot,
LONG        cx,
LONG        cy,
BOOL        AlphaBlendedCursor
)
{
    PDEV*   ppdev;
    
    BYTE    bMono, bMask;
    BYTE    bTest;

    LONG    lPixelCount;
    LONG    i,j,k;
    LONG    lSrcDelta = 0;
    LONG    lDstDelta = 0;
    
    ULONG   ulDwordCount;    
    ULONG   ulTest; 
    ULONG   ulColor;
    ULONG   ulAlpha;
    ULONG   ulTemp;
    ULONG   ulHwCursorWidth;      // HW : 32 or 64
    ULONG   ulHwCursorFormat = NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_ROP1R5G5B5;

    BYTE*    pjSrcScan;           // BYTE * because GDI may give us on byte alignment
    ULONG*   pulDstScan;          // ULONG * for performance

    //**************************************************************************
    // Get pointer to pdev
    //**************************************************************************

    ppdev    = (PDEV*) pso->dhpdev;

    if ( AlphaBlendedCursor && globalOpenGLData.oglDrawableClientCount )
    {
        //******************************************************************
        // For alpha blended cursors when an OpenGL window is open,
        // we convert it to a hardware mono cursor. 
        // if alpha blended cursors come thru with cx > ppdev->ulMaxHwCursorSize or cy > ppdev->ulMaxHwCursorSize, 
        // they'll get clipped
        //******************************************************************

        if (cx > (LONG)(ppdev->ulMaxHwCursorSize) )
            cx = ppdev->ulMaxHwCursorSize;
            
        if (cy > (LONG)(ppdev->ulMaxHwCursorSize) )
            cy = ppdev->ulMaxHwCursorSize;            
    }
    else 
    {
        if(AlphaBlendedCursor)
        {
            // Not alpha cursor support on 16Bpp mode.
            return(SPS_DECLINE);
        }

        if ( (cx > (LONG)(ppdev->ulMaxHwCursorSize))  ||  (cy > (LONG)(ppdev->ulMaxHwCursorSize)) ) {
            DISPDBG((5, "NVSetPointerShape: Unsupported size"));          
            return(SPS_DECLINE);
        }
    }

    if(cx > 32)
        ulHwCursorWidth = 64;
    else
        ulHwCursorWidth = 32; 
    
    pulDstScan = (ULONG *) (ppdev->ulCursorMemOffset[ppdev->ulCursorCurBufIndex] + ppdev->pjFrameBufbase);

    //**************************************************************************
    // Check whether we'll be drawing a MONOCHROME or COLOR cursor
    //**************************************************************************
    if (psoColor == NULL)
    {

        pjSrcScan    = psoMsk->pvScan0;
        lSrcDelta    = psoMsk->lDelta;

        lDstDelta = (cx + 7) / 8;
         
        Nv1BppToHw16BppCursor(cx, cy, pjSrcScan, pulDstScan, ulHwCursorWidth, lSrcDelta, lDstDelta);

        DISPDBG((5, "NVSetPointerShape: Mono Cursor"));

    }   // Draw monochrome cursor (psoColor == NULL)
    else
    {
        ULONG   iSrcBitDepth;
        LONG    lPitch;
        BYTE*   pjSrcColor;
        BYTE*   pjSrcColorMask;
        
        //**********************************************************************
        // Color cursor ( or alpha blended cursor)
        // Calc width of each scan in the color cursor bitmap. Assume the
        // bitmap has the same color format as the display device.
        //
        // The first 32*4 bytes is the AND mask.
        // The next 32 * 32 * bytes/pixel is the cursor image.
        // Translate the image into 1-5-5-5 format for the DAC
        // The AND mask value goes into the upper bit
        // The pixel image gets reduced to 5 bits each color and goes in the lower 15 bits
        // 8 bpp  - 8 bit index into palette.           pixel = 1 byte
        // 16 bpp - 1 bit X, 5 bit R, 5 bit G, 5 bit B. pixel = 2 bytes
        // 32 bit - 8 bit R, 8 bit G, 8 bit B           pixel = 3 bytes unpacked (+unused byte).
        //**********************************************************************

        //**********************************************************************
        // Determine if this is a device bitmap or GDI managed bitmap.
        // Note that the cursor bitmap may be any legal bit depth: it
        // does not necessarily match the current video mode. That's ok
        // since we have to convert it to 16BPP for our color cursor
        // hw.
        //**********************************************************************

        if (psoColor->dhsurf != NULL) {
            
            //******************************************************************
            // Device managed bitmap
            //******************************************************************
            if (((DSURF *) psoColor->dhsurf)->dt == DT_SCREEN) {
                //**************************************************************
                // Ptr surface is in offscreen device memory, need to wait for
                // fifo empty and graphics engine idle prior to copying the
                // pointer bitmap.
                //**************************************************************
                pjSrcColor      = (PBYTE) (((DSURF *) psoColor->dhsurf)->LinearPtr);
                lPitch          = (((DSURF *) psoColor->dhsurf))->LinearStride;
                iSrcBitDepth =  (((DSURF *) psoColor->dhsurf)->ppdev)->iBitmapFormat;
                ppdev->pfnWaitEngineBusy(ppdev);
            } else {
                // (pdsurfSrc->dt == DT_DIB)
                //**************************************************************
                // Device managed DFB in host memory, stored as DIB.
                // Don't bother moving DIB to offscreen memory since whe don't
                // use the graphics engine to draw or translate it.
                //**************************************************************
                psoColor = ((DSURF *) psoColor->dhsurf)->pso;
                lPitch =  psoColor->lDelta;
                iSrcBitDepth = psoColor->iBitmapFormat;
                pjSrcColor = (PBYTE) psoColor->pvScan0;
            }

        } else if (psoColor->pvScan0 != NULL) {
            //******************************************************************
            // GDI managed host memory bitmap
            //******************************************************************
            pjSrcColor      = (PBYTE) psoColor->pvScan0;
            lPitch          = psoColor->lDelta;
            iSrcBitDepth =  psoColor->iBitmapFormat;
        } else {
            //******************************************************************
            // If ptr bmp is neither DFB nor DIB, it's likely an error occurred.
            //******************************************************************
            return(SPS_DECLINE);
        }

        //**********************************************************************
        // Can't use ptr without address of the bitmap
        //**********************************************************************

        if (pjSrcColor == NULL) {
            return(SPS_DECLINE);
        }

        //**********************************************************************
        // psoMsk is NULL when using an Alpha Blended cursor
        //**********************************************************************

        if (!(AlphaBlendedCursor)) {
            pjSrcColorMask  = psoMsk->pvScan0;
            lSrcDelta       = psoMsk->lDelta;
        }
            
        switch (iSrcBitDepth)
        {
            //******************************************************************
            // We gotta look up the color from the palette to convert to RGB
            //******************************************************************

            case BMF_8BPP:
                Nv8BppToHw16BppCursor(ppdev, ppdev->pPal, pxlo, cx, cy, 
                    pjSrcColor, pjSrcColorMask, pulDstScan, ulHwCursorWidth, 
                    lSrcDelta, lPitch);
                break;

            //******************************************************************
            // Screen is currently in 16bpp mode
            //******************************************************************
            case BMF_16BPP:
                Nv16BppToHw16BppCursor(ppdev, cx, cy, pjSrcColor, pjSrcColorMask, 
                     lSrcDelta, lPitch,  pulDstScan,  ulHwCursorWidth, ppdev->flGreen);

                break;
            //******************************************************************
            // Screen is currently in 32bpp mode
            //******************************************************************
            case BMF_32BPP:
                // Converted to 16Bpp for HW.

                NvHw16BppCursor(cx, cy, pjSrcColor, pjSrcColorMask, lSrcDelta, lPitch,
                    (ULONG*) (ppdev->ulCursorMemOffset[ppdev->ulCursorCurBufIndex] + ppdev->pjFrameBufbase), ulHwCursorWidth, AlphaBlendedCursor);
                break;

             default:
                //**************************************************************
                // Can't handle any other bit depths
                //**************************************************************
                DISPDBG((5, "NVSetPointerShape: Unsupported bit depth"));
                return(SPS_DECLINE);

        }   // Switch
    } // psoColor != NULL

    DISPDBG((5, "NVSetPointerShape: Width or format changed.  Reloading."));
    NvSetPointerShapeToHw(ppdev,  ulHwCursorWidth, cy, ulHwCursorFormat);
    ppdev->ulCursorCurBufIndex ^= 1;
	
    //**************************************************************************
    // Get HotSpot
    //**************************************************************************

    ppdev->xPointerHot = xHot;
    ppdev->yPointerHot = yHot;

    ppdev->pfnMovePointer(ppdev, x, y);

    return(SPS_ACCEPT_NOEXCLUDE);

}

//******************************************************************************
//
//  Function:   NVSetPointerShapeOn32BppHwMode
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

ULONG NVSetPointerShapeOn32BppHwMode(
SURFOBJ*    pso,
SURFOBJ*    psoMsk,
SURFOBJ*    psoColor,
XLATEOBJ*   pxlo,
LONG        x,              // Relative coordinates
LONG        y,              // Relative coordinates
LONG        xHot,
LONG        yHot,
LONG        cx,
LONG        cy,
BOOL        AlphaBlendedCursor
)
{
    PDEV*   ppdev;
    
    BYTE    bMono, bMask;
    BYTE    bTest;

    LONG    lPixelCount;
    LONG    i,j,k;
    LONG    lSrcDelta = 0;
    LONG    lDstDelta = 0;
    
    ULONG   ulDwordCount;    
    ULONG   ulTest; 
    ULONG   ulColor;
    ULONG   ulAlpha;
    ULONG   ulTemp;
    ULONG   ulHwCursorWidth;      // HW : 32 or 64
    // All 32Bpp cursor are done on PM_LE_A8R8G8B8 except the inverted background color cursor.
    ULONG   ulHwCursorFormat = NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8; 

    BYTE*    pjSrcScan;           // BYTE * because GDI may give us on byte alignment
    ULONG*   pulDstScan;          // ULONG * for performance

    //**************************************************************************
    // Get pointer to pdev
    //**************************************************************************

    ppdev    = (PDEV*) pso->dhpdev;

    if ( AlphaBlendedCursor && globalOpenGLData.oglDrawableClientCount )
    {
        //******************************************************************
        // For alpha blended cursors when an OpenGL window is open,
        // we convert it to a hardware mono cursor. 
        // if alpha blended cursors come thru with cx > ppdev->ulMaxHwCursorSize or cy > ppdev->ulMaxHwCursorSize, 
        // they'll get clipped
        //******************************************************************

        if (cx > (LONG)(ppdev->ulMaxHwCursorSize) )
            cx = ppdev->ulMaxHwCursorSize;
            
        if (cy > (LONG)(ppdev->ulMaxHwCursorSize) )
            cy = ppdev->ulMaxHwCursorSize;            
    }
    else 
    {
        if ( (cx > (LONG)(ppdev->ulMaxHwCursorSize))  ||  (cy > (LONG)(ppdev->ulMaxHwCursorSize)) ) {
            DISPDBG((5, "NVSetPointerShape: Unsupported size"));
            return(SPS_DECLINE);
        }
    }

    if(cx > 32)
        ulHwCursorWidth = 64;
    else
        ulHwCursorWidth = 32; 
    
    pulDstScan = (ULONG *) (ppdev->ulCursorMemOffset[ppdev->ulCursorCurBufIndex] + ppdev->pjFrameBufbase);

    //**************************************************************************
    // Check whether we'll be drawing a MONOCHROME or COLOR cursor
    //**************************************************************************
    if (psoColor == NULL)
    {
        pjSrcScan    = psoMsk->pvScan0;
        lSrcDelta    = psoMsk->lDelta;

        lDstDelta = (cx + 7) / 8;
         
        for(i = 0; i < cy; i++)
        { 
            ulDwordCount = 0;
            for(j = 0; j < lDstDelta; j++)                 // in term of bytes
            {
                bMask = pjSrcScan[i * lSrcDelta + j];           // Get a Mask byte (8 pixels)
                bMono = pjSrcScan[ (cy + i) * lSrcDelta + j];   // Get a Mono byte (8 pixels)

                //**************************************************************
                // Now combine the masks according to the AND-XOR Microsoft convention
                //
                //  ('mask' value)      ('mono' value)
                //  AND mask value      XOR mask value      Result on screen
                //  --------------      --------------      ----------------
                //          0               0               Black
                //          0               1               White
                //          1               0               Transparent, pixel unchanged
                //          1               1               Inverts the pixel color
                //
                // We'll parse the pixels from right to left in source
                // (Bit 0 is leftmost in mono image).  This will cause
                // us to bit flip the cursor image and draw it correctly
                //**************************************************************

                ulTest = 0x80;
                for (k = 0; k < 8; k++, ulTest >>= 1)
                {
                    if ((ulTest & bMask))
                    {
                        if (ulTest & bMono)             // Test this bit
                        {
                            // inverted background color
                            pulDstScan = (ULONG *) (ppdev->ulCursorMemOffset[ppdev->ulCursorCurBufIndex] + ppdev->pjFrameBufbase);
                            WORK_AROUND_NV11_DIRTHER;
                            Nv1BppToHw16BppCursor(cx, cy, pjSrcScan, 
                                pulDstScan, ulHwCursorWidth, lSrcDelta, lDstDelta);
                            ulHwCursorFormat = NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_ROP1R5G5B5;
                            goto NvCursorDoneImage;
                        }
                        else
                        {
                            ulColor = 0;                // transparent
                        }
                    }
                    else
                    {
                        if (ulTest & bMono)             // Test this bit
                            ulColor = 0xffffffff;         // White
                        else
                            ulColor = 0xff000000;                // Black
                    }

                    *pulDstScan++ = ulColor;
                    ulDwordCount++;
                }
            }   // End of j loop

            // make HW happy, fill remaining DWORD to ulHwCursorWidth
            while(ulDwordCount < ulHwCursorWidth)
            {
                *pulDstScan++ = 0x0;
                ulDwordCount++;
            }
        }   // End of i loop
        DISPDBG((5, "NVSetPointerShape: Mono Cursor"));
    }   // Draw monochrome cursor (psoColor == NULL)
    else
    {
        WORD    red;
        WORD    green;
        WORD    blue;
        WORD    alpha;
        ULONG   ulMask;
        ULONG   iSrcBitDepth;
        LONG    lPitch;
        BYTE*   pjSrcColor;
        BYTE*   pjSrcColorMask;
        ULONG*  pulTemp;
        PALETTEENTRY pXlatePal[256];
        PPALETTEENTRY pPal;
    
        
        //**********************************************************************
        // Color cursor ( or alpha blended cursor)
        // Calc width of each scan in the color cursor bitmap. Assume the
        // bitmap has the same color format as the display device.
        //
        // The first 32*4 bytes is the AND mask.
        // The next 32 * 32 * bytes/pixel is the cursor image.
        // Translate the image into 1-5-5-5 format for the DAC
        // The AND mask value goes into the upper bit
        // The pixel image gets reduced to 5 bits each color and goes in the lower 15 bits
        // 8 bpp  - 8 bit index into palette.           pixel = 1 byte
        // 16 bpp - 1 bit X, 5 bit R, 5 bit G, 5 bit B. pixel = 2 bytes
        // 32 bit - 8 bit R, 8 bit G, 8 bit B           pixel = 3 bytes unpacked (+unused byte).
        //**********************************************************************

        //**********************************************************************
        // Determine if this is a device bitmap or GDI managed bitmap.
        // Note that the cursor bitmap may be any legal bit depth: it
        // does not necessarily match the current video mode. That's ok
        // since we have to convert it to 16BPP for our color cursor
        // hw.
        //**********************************************************************

        if (psoColor->dhsurf != NULL) {
            
            //******************************************************************
            // Device managed bitmap
            //******************************************************************
            if (((DSURF *) psoColor->dhsurf)->dt == DT_SCREEN) {
                //**************************************************************
                // Ptr surface is in offscreen device memory, need to wait for
                // fifo empty and graphics engine idle prior to copying the
                // pointer bitmap.
                //**************************************************************
                pjSrcColor      = (PBYTE) (((DSURF *) psoColor->dhsurf)->LinearPtr);
                lPitch          = (((DSURF *) psoColor->dhsurf))->LinearStride;
                iSrcBitDepth =  (((DSURF *) psoColor->dhsurf)->ppdev)->iBitmapFormat;
                ppdev->pfnWaitEngineBusy(ppdev);
            } else {
                // (pdsurfSrc->dt == DT_DIB)
                //**************************************************************
                // Device managed DFB in host memory, stored as DIB.
                // Don't bother moving DIB to offscreen memory since whe don't
                // use the graphics engine to draw or translate it.
                //**************************************************************
                psoColor = ((DSURF *) psoColor->dhsurf)->pso;
                lPitch =  psoColor->lDelta;
                iSrcBitDepth = psoColor->iBitmapFormat;
                pjSrcColor = (PBYTE) psoColor->pvScan0;
            }

        } else if (psoColor->pvScan0 != NULL) {
            //******************************************************************
            // GDI managed host memory bitmap
            //******************************************************************
            pjSrcColor      = (PBYTE) psoColor->pvScan0;
            lPitch          = psoColor->lDelta;
            iSrcBitDepth =  psoColor->iBitmapFormat;
        } else {
            //******************************************************************
            // If ptr bmp is neither DFB nor DIB, it's likely an error occurred.
            //******************************************************************
            return(SPS_DECLINE);
        }

        //**********************************************************************
        // Can't use ptr without address of the bitmap
        //**********************************************************************

        if (pjSrcColor == NULL) {
            return(SPS_DECLINE);
        }

        //**********************************************************************
        // psoMsk is NULL when using an Alpha Blended cursor
        //**********************************************************************

        if (!(AlphaBlendedCursor)) {
            pjSrcColorMask  = psoMsk->pvScan0;
            lSrcDelta       = psoMsk->lDelta;
        }
            
        switch (iSrcBitDepth)
        {
            //******************************************************************
            // We gotta look up the color from the palette to convert to RGB
            //******************************************************************

            case BMF_8BPP:

            #ifdef HW_SUPPORT_32BPP_XOR
            !!!  Will enable later on for new chip (NV20 and later)
            !!!  set new format for 32Bpp XOR function
            !!!  set the format
            !!!  ulHwCursorFormat = NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8; 
            
        
                //**************************************************************
                // We gotta look up the color from the palette to
                // convert to RGB. Try to get info from the XLATEOBJ
                // first.
                //**************************************************************
                pPal = ppdev->pPal;     // Default to using pal from PDEV
                if (pxlo != NULL)
                {
                    if (XLATEOBJ_cGetPalette(pxlo, XO_SRCPALETTE, 256, NULL) == 256)
                    {
                        //******************************************************
                        // Get the RGB palette.
                        //******************************************************
                        if (XLATEOBJ_cGetPalette(pxlo, XO_SRCPALETTE, 256, (PULONG) pXlatePal) != 256)
                            pPal = pXlatePal;
                    }
                }
                for (i = 0; i < cy; i++)
                {
                    ulMask = *(PULONG) (pjSrcColorMask + i*lSrcDelta);     // Get 8 AND bits
                    ulTest = 0x00000080;
                    ulDwordCount = 0;
                    for (k = 0; k < cx; k++, ulTest >>=1)
                    {
                        if (!(k & 7))
                            ulTest = 0x00000080 << k;
                        ulColor = *((PBYTE) (pjSrcColor + i*lPitch + k)); // Get 15 bits of color
                        blue  = pPal[ulColor].peBlue;
                        green = pPal[ulColor].peGreen;
                        red   = pPal[ulColor].peRed;
                        ulColor = ( (DWORD)(red & 0xf8) << 13) | ((green & 0xf8) << 5) | ((blue & 0xf8) >> 3);

                        if (!(ulTest & ulMask))
                            ulColor |= 0xff000000;      // Not transparent
                        else
                            ulColor = 0;                // transparent

                        *pulDstScan++ = ulColor;
                        ulDwordCount++;
                    }
                    // make HW happy, fill remaining DWORD to ulHwCursorWidth
                    while(ulDwordCount < ulHwCursorWidth)
                    {
                        *pulDstScan++ = 0x0;
                        ulDwordCount++;
                    }
                }
                DISPDBG((5, "NVSetPointerShape: 8bpp Cursor"));
            #else
                if(Nv8BppToHw16BppCursor(ppdev, ppdev->pPal, pxlo, cx, cy, 
                    pjSrcColor, pjSrcColorMask, pulDstScan, ulHwCursorWidth, 
                    lSrcDelta, lPitch)
                     == SPS_DECLINE)
                    return(SPS_DECLINE);
                ulHwCursorFormat = NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_ROP1R5G5B5;
                goto NvCursorDoneImage;
            #endif
                break;

            //******************************************************************
            // Screen is currently in 16bpp mode
            //******************************************************************
            case BMF_16BPP:
            #ifdef HW_SUPPORT_32BPP_XOR
            !!!  Will enable later on for new chip (NV20 and later)
            !!!  set new format for 32Bpp XOR function
            !!!  ulHwCursorFormat = NV07C_SET_CURSOR_IMAGE_FORMAT_COLOR_PM_LE_A8R8G8B8; 
                for (i = 0; i < cy; i++)
                {
                    ulMask = *(PULONG) (pjSrcColorMask + i*lSrcDelta);     // Get 8 AND bits
                    ulTest = 0x00000080;
                    ulDwordCount = 0;
                    for (k = 0; k < cx; k++, ulTest >>=1)
                    {
                       if (!(k & 7))
                           ulTest = 0x00000080 << k;

                       ulColor = *((PUSHORT) (pjSrcColor + i*lPitch + k*2)); // Get 16 bits of color
                       red   = (WORD)((ulColor & 0xf800) >> 11); // 5 bits
                       green = (WORD)((ulColor & 0x07e0) >> 5); // 6 bits
                       blue  = (WORD)(ulColor & 0x001f); // 5 bits
                       ulColor = (  (DWORD)(red << 16) | (green << 8) | (blue) );

                        if (!(ulTest & ulMask))
                            ulColor |= 0xff000000;      // Not transparent
                        else
                            ulColor = 0;                // transparent

                        *pulDstScan++ = ulColor;
                        ulDwordCount++;

                    }
                    // make HW happy, fill remaining DWORD to ulHwCursorWidth
                    while(ulDwordCount < ulHwCursorWidth)
                    {
                        *pulDstScan++ = 0x0;
                        ulDwordCount++;
                    }
                }
                DISPDBG((5, "NVSetPointerShape: 16bpp Cursor"));
            #else
                if(Nv16BppToHw16BppCursor(ppdev, cx, cy, pjSrcColor, pjSrcColorMask, 
                     lSrcDelta, lPitch,  pulDstScan,  ulHwCursorWidth, ppdev->flGreen)
                     == SPS_DECLINE)
                    return(SPS_DECLINE);
                ulHwCursorFormat = NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_ROP1R5G5B5;
                goto NvCursorDoneImage;
            #endif

                break;
            //******************************************************************
            // Screen is currently in 32bpp mode
            //******************************************************************
            case BMF_32BPP:
                if (ppdev->ulMaxHwCursorDepth >= 32) {
                    if(AlphaBlendedCursor) {
                        if (ppdev->bSupportAlphaCursor) {
                            // NV11, NV15 and up have full 32bit, alpha cursor support

                            for (i = 0; i < cy; i++) {
                                pulTemp = (ULONG *)(pjSrcColor + i * lPitch);
                                for (k = 0; k < cx; k++) {
                                    *pulDstScan++ =  *pulTemp++;
                                }
                                // make HW happy, fill remaining DWORD to ulHwCursorWidth
                                for( ; k < (LONG)ulHwCursorWidth; k++) {
                                    *pulDstScan++ = 0x0;
                                }
                            }
                            DISPDBG((5, "NVSetPointerShape: 32bpp Alpha Cursor"));
                        } else {
                            // If we have no alpha cursor support in hardware AND opengl 
                            // or an overlay is present, support the cursor in hardware
                            // by ignoring the alpha componenet.  This prevents the cursor
                            // from disappearing due to a GDI software cursor getting
                            // overwritten by opengl or the overlay.
                            if (globalOpenGLData.oglDrawableClientCount 
#if (_WIN32_WINNT >= 0x0500) && !defined(_WIN64)
                                || ppdev->pDriverData->vpp.dwOverlaySurfaces
#endif          
                                ) {
                                for (i = 0; i < cy; i++) {
                                    pulTemp = (ULONG *)(pjSrcColor + i * lPitch);
                                    for (k = 0; k < cx; k++) {
                                        ulAlpha = *pulTemp;
                                        ulAlpha >>= 24;
                                        // Removed Alpha Value
                                        if(ulAlpha == 0xFF) {
                                            ulColor = *pulTemp | 0xFF000000;
                                        } else {
                                            ulColor = 0x00000000;
                                        }
                                        *pulDstScan++ = ulColor;
                                        pulTemp++;
                                    }
                                    // make HW happy, fill remaining DWORD to ulHwCursorWidth
                                    for( ; k < (LONG)ulHwCursorWidth; k++) {
                                        *pulDstScan++ = 0x0;
                                    }
                                }                        
                            } else {
                                DISPDBG((5, "NVSetPointerShape: No 32bpp Alpha Support"));
                                return(SPS_DECLINE);
                            }
                            DISPDBG((5, "NVSetPointerShape: 32bpp Alpha Cursor w/ no alpha"));
                        }
                    } else {
                        // Regular 32 bit cursor
                        for (i = 0; i < cy; i++) {
                            lPixelCount = 0;
                            pulTemp = (ULONG *)(pjSrcColor + i * lPitch);
                            for (j = 0; j < cx ; j += 8) {
                                bMask = *(pjSrcColorMask + i*lSrcDelta + j / 8);  
                                bTest = 0x80;
                                for(k = 0; k < 8; k++, bTest >>= 1) {
                                    ulColor = *pulTemp++;

                                    if(!(bTest & bMask)) 
                                    {
                                        ulColor |= 0xFF000000;
                                    }
                                    else 
                                    {
                                	    if (ulColor == 0x00FFFFFF)
                    					{
                                            // inverted background color
                                            WORK_AROUND_NV11_DIRTHER;
                                            NvHw16BppCursor(cx, cy, pjSrcColor, pjSrcColorMask, lSrcDelta, lPitch,
                                                (ULONG*) (ppdev->ulCursorMemOffset[ppdev->ulCursorCurBufIndex] + ppdev->pjFrameBufbase), ulHwCursorWidth, AlphaBlendedCursor);
                                            ulHwCursorFormat = NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_ROP1R5G5B5;
                                            goto NvCursorDoneImage;
                    					}
					                    else 
                    					{
				                            // Transparent
                    						ulColor = 0;
					                    }
                                    }

                                    *pulDstScan++ = ulColor;

                                    lPixelCount++;
                                }
                            }
                            // make HW happy, fill remaining DWORD to ulHwCursorWidth
                            for( ; lPixelCount < (LONG)ulHwCursorWidth; lPixelCount++)
                            {
                                *pulDstScan++ = 0x0;
                            }
                        }
                        DISPDBG((5, "NVSetPointerShape: 32bpp Alpha Cursor w/ no alpha"));
                    }
                } 
                else 
                {
                    WORK_AROUND_NV11_DIRTHER;
                    NvHw16BppCursor(cx, cy, pjSrcColor, pjSrcColorMask, lSrcDelta, lPitch,
                        (ULONG*) (ppdev->ulCursorMemOffset[ppdev->ulCursorCurBufIndex] + ppdev->pjFrameBufbase), ulHwCursorWidth, AlphaBlendedCursor);
                    ulHwCursorFormat = NV067_SET_CURSOR_IMAGE_FORMAT_COLOR_LE_ROP1R5G5B5;
                    goto NvCursorDoneImage;
                }
                break;

             default:
                //**************************************************************
                // Can't handle any other bit depths
                //**************************************************************
                DISPDBG((5, "NVSetPointerShape: Unsupported bit depth"));
                return(SPS_DECLINE);

        }   // Switch
    } // psoColor != NULL

    for(; i < (LONG) ulHwCursorWidth; i++)
    {
        for(k = 0; k < (LONG) ulHwCursorWidth; k++)
        {
            *pulDstScan++ = 0x0;
        }
    }

NvCursorDoneImage:

    if(ulHwCursorWidth  != ppdev->ulCachedCursorWidth ||
       ulHwCursorFormat != ppdev->ulCachedCursorFormat)
    {
        DISPDBG((5, "NVSetPointerShape: Width or format changed.  Reloading."));
        NvSetPointerShapeToHw(ppdev,  ulHwCursorWidth, cy, ulHwCursorFormat);
    }
    else
    {
        volatile ULONG * pCursorAddress;
        ULONG ulCursorOffset;
        ULONG i, ulHead;
        
      	for(i = 0; i < ppdev->ulNumberDacsActive; i++)
       	{
            ulHead = ppdev->ulDeviceDisplay[i];
            pCursorAddress = ppdev->NvBaseAddr + 0x0060080c / 4 + ((ulHead) * 0x2000/4);
            ulCursorOffset = ppdev->ulCursorMemOffset[ppdev->ulCursorCurBufIndex];
            *pCursorAddress = ulCursorOffset;
        }
        
    }
    ppdev->ulCursorCurBufIndex ^= 1;
	
    //**************************************************************************
    // Get HotSpot
    //**************************************************************************

    ppdev->xPointerHot = xHot;
    ppdev->yPointerHot = yHot;

    ppdev->pfnMovePointer(ppdev, x, y);

    return(SPS_ACCEPT_NOEXCLUDE);

}


void __cdecl DisableHeadCursor(PDEV *ppdev, ULONG ulHead, ULONG ulDiable)
{
#if !USE_DMC_CURSOR
    DECLARE_DMA_FIFO;
    INIT_LOCAL_DMA_FIFO;
#endif

    if(ulDiable)
    {
#if USE_DMC_CURSOR
        SET_CURSOR_LOCATION(ppdev, ulHead, (ppdev->cxScreen + 64), (ppdev->cyScreen + 64));
#else        
        NV_DMAPUSH_CHECKFREE( ((ULONG)(10)));  
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NVFFF_SET_OBJECT(0) ,   NV_VIDEO_LUT_CURSOR_DAC+ulHead);
        NV_DMAPUSH_WRITE1(LUT_CURSOR_DAC_SUBCHANNEL, NV046_SET_CURSOR_POINT, ((ppdev->cxScreen + 64) & 0xffff) | ((ppdev->cyScreen + 64) << 16) );

        UPDATE_PDEV_DMA_COUNT;
        NV4_DmaPushSend(ppdev);
#endif        
    }
}

#endif // NV3

