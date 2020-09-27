//******************************Module*Header***********************************
//
// Module Name: NV3PTR.C
//
// This module contains the hardware pointer support for the display
// driver.
//
// Copyright (c) 1992-1996 Microsoft Corporation
//
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *  Copyright (C) 1997 - 2000 NVidia Corporation. All Rights Reserved.         *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"

#include "nvsubch.h"
#include "oldnv332.h"
#define NV32_INCLUDED // Prevents re-inclusion of real nv32 header
#include "driver.h"
#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"

#ifdef NV3

#define OFFSET_PRMCIO_CRX__COLOR_REG                        0x3d4
#define OFFSET_PRMCIO_CR__COLOR_REG                         0x3d5
#define OFFSET_PRMVIO_SRX_REG                               0x3c4
#define OFFSET_PRMVIO_SR_LOCK_REG                           0x3c5
#define OFFSET_PRMVIO_MISC_READ_REG                         0x3cc
#define OFFSET_PRAMDAC_CU_START_POS_REG                     0x0
#define NV_PRMVIO_SR_LOCK_INDEX                             6

#define NV_SR_UNLOCK_VALUE                                  0x00000057
#define NV_SR_LOCK_VALUE                                    0x00000099

#define NV_CIO_CRE_HCUR_ADDR0_INDEX                 0x00000030
#define NV_CIO_CRE_HCUR_ADDR1_INDEX                 0x00000031

extern ULONG NV3CursorChecksum(BYTE *);
extern VOID  NV3LoadCursorData(BYTE *,BYTE *);
extern VOID  NV3Load16x16CursorData(BYTE *,BYTE *);

//******************************************************************************
//
//  Function:   NV3_SetHwCursor
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
VOID NV3_SetHwCursor(
PDEV*   ppdev,
LONG    x,
LONG    y,
LONG    xHot,
LONG    yHot,
BYTE *  pjShape
)
{
    ULONG RowCount, ColumnCount;
    volatile ULONG *PRAMDACRegs;
    volatile ULONG*  CursorImageDataPtr;
    ULONG * pImage;
    
    //**************************************************************************
    // Get HotSpot
    //**************************************************************************

    ppdev->xPointerHot = xHot;
    ppdev->yPointerHot = yHot;

    //**************************************************************************
    // Init access to cursor registers
    //**************************************************************************
    PRAMDACRegs = ppdev->PRAMDACRegs;

    //**************************************************************************
    // Load a test pattern into cursor image area (2k worth of data) (32x32 16bpp)
    //**************************************************************************

    CursorImageDataPtr = ppdev->PRAMINRegs;

    //**************************************************************************
    // Each cursor color bitmap takes up 2k.  However, to fix up noise which
    // occurs when we load cursor shapes, we also allocate the subsequent 2k
    // in cursor memory, and clear it.  So essentially, each cursor
    // image takes up 4k. The 1st 2k is the cursor image, and the
    // following 2k is zero'd out (made transparent)
    //**************************************************************************

    ((PUCHAR)(CursorImageDataPtr)) += NUM_CACHED_CURSORS * 2048 * 2;

    pImage =(ULONG *)pjShape;

    for (RowCount=0;RowCount < 32;RowCount++)
        for (ColumnCount=0; ColumnCount < 16 ; ColumnCount ++)
            {
            //******************************************************************
            // ColumnCount goes from 0 thru 16 because we'll be outputting
            // DWORDS at a time (instead of words)
            // Remember, RowCount and DwordCount are multiplied by 4 after
            // C Compilation, because CursorImageDataPtr is a pointer to Dwords
            // Disable Transparency (0x80008000) and specify GREEN as
            // a test pattern for the color cursor
            //******************************************************************

            *(CursorImageDataPtr + RowCount*16 + ColumnCount) =  *pImage;
            pImage++;
            }

    //**************************************************************************
    // Update cursor position
    //**************************************************************************
    ppdev->pfnMovePointer(ppdev, x, y);

}

//******************************************************************************
//
//  Function:   NV3_TVCopyCursorData
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

VOID NV3_TVCopyCursorData(
BYTE    *srcptr,
BYTE    *dstptr)

    {
    LONG   LineCount;
    LONG   RowCount;

    //**************************************************************************
    // Just copy the cursor bitmap as is (it's already been prepped for hardware)
    //**************************************************************************

    for (LineCount=0; LineCount <32 ;LineCount++)
        {
        for (RowCount=0;RowCount<64;RowCount+=4)
            {
            *(ULONG *)(dstptr + LineCount*64 + RowCount) =
                        *(ULONG *)(srcptr + LineCount*64 + RowCount);
            }
        }

    }

//******************************************************************************
//
//  Function:   NV3_TVClipCursorData
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



VOID NV3_TVClipCursorData(
BYTE *dstptr,
LONG ypos,
LONG height
)

    {
    LONG   LineCount;
    LONG   RowCount;
    LONG   dy;

    //**************************************************************************
    // Get bottom of cursor and figure out # of pixels past the end
    //**************************************************************************

    dy = ypos + 32;                         // Bottom of cursor
//    dy -= 476;                              // # of pixels past end
      dy -= (height);                         // # of pixels past end

    //**************************************************************************
    // Only clip cursor if it's going past the end of the screen
    //**************************************************************************

    if (dy>0)
        {
        //**********************************************************************
        // Clear out portion of cursor AFTER tv scanline 478
        //**********************************************************************

        for (LineCount=31;(LineCount > (31-dy)) && (LineCount>=0) ;LineCount--)
            {
            for (RowCount=0;RowCount<64;RowCount+=4)
                {
                *(ULONG *)(dstptr + LineCount*64 + RowCount) =  0x00000000;
                }
            }
        }

    }



//******************************************************************************
//
//  Function:   NV3EnablePointer
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

VOID NV3EnablePointer(
PDEV*               ppdev,
BOOL    bEnable
)
{
    ULONG status;

    if (bEnable)
    {
        //**********************************************************************
        // Init access to cursor registers
        //**********************************************************************
        UCHAR lock;
        volatile UCHAR *PRMVIO_SRX_Reg;
        volatile UCHAR *PRMVIO_SR_LOCK_Reg;
        volatile UCHAR *PRMVIORegs = ppdev->PRMVIORegs;
        volatile ULONG*  CursorImageDataPtr;
        ULONG i;

        //**********************************************************************
        // Each cursor color bitmap takes up 2k.  However, to fix up noise which
        // occurs when we load cursor shapes, we also allocate the subsequent 2k
        // in cursor memory, and clear it.  So essentially, each cursor
        // image takes up 4k. The 1st 2k is the cursor image, and the
        // following 2k is zero'd out (made transparent)
        //
        // Here, we allocate memory for the cached monochrome cursors,
        // and the color cursor.
        //**********************************************************************

        ppdev->ulCursorMemOffset = (NUM_CACHED_CURSORS*2 + 2) * 2048;

        ppdev->hCursor = DD_CURSOR_OBJECT_HANDLE;
        status = NvAllocMemory( ppdev->hDriver,
                                ppdev->hClient,
                                ppdev->hDevice,
                                ppdev->hCursor,
                                NV01_MEMORY_LOCAL_PRIVILEGED,
                                0,
                                (PVOID)&ppdev->PRAMINRegs,
                                &ppdev->ulCursorMemOffset);

        if (status != NVOS02_STATUS_SUCCESS)
        /* jsw...
        status = NvAllocMemory( ppdev->hDriver,
                                "cursor",
                                (PVOID)&ppdev->PRAMINRegs,
                                &ppdev->ulCursorMemOffset);

        if (status != NV000E_ERROR_OK)
        ...jsw */
            {
            DISPDBG((2, "NV3EnablePointer: Cannot allocate cursor memory"));
            ppdev->flCaps |= CAPS_SW_POINTER;
            return;
            }

        PRMVIO_SRX_Reg = (volatile UCHAR *)(PRMVIORegs+OFFSET_PRMVIO_SRX_REG);
        PRMVIO_SR_LOCK_Reg = (volatile UCHAR *)(PRMVIORegs+OFFSET_PRMVIO_SR_LOCK_REG);

        PRMVIO_SRX_Reg = (volatile UCHAR *)(PRMVIORegs+OFFSET_PRMVIO_SRX_REG);

        *PRMVIO_SRX_Reg = NV_PRMVIO_SR_LOCK_INDEX;
        *PRMVIO_SR_LOCK_Reg = NV_SR_UNLOCK_VALUE;

        //**********************************************************************
        // Unlock CRTC extended regs
        //
        // NV_PRMVIO_SRX            = Sequencer Index Register (3c4)
        // NV_PRMVIO_SR_LOCK        = Data is written/ readm from register (3c5)
        // NV_PRMVIO_SR_LOCK_INDEX  = This indexed register (5) locks/unlocks
        //                            all extended registers.  When written with
        //                            value of 57, all extended registers are UNLOCKED.
        //                            Otherwise, all extended registers are LOCKED.
        //
        //                            When value = 0, extended register are in a locked state
        //                            When value = 1, extended registers are in an unlocked state
        //
        //**********************************************************************

        //**********************************************************************
        // Make sure to clear out cursor memory first. Otherwise,
        // we may see some noise when switching cursor shapes.
        //**********************************************************************

        CursorImageDataPtr = ppdev->PRAMINRegs;

        //**********************************************************************
        // Clear out cursor memory for all of the cached cursors and 1 color cursor.
        // Divide by 4 because we output dwords (4 bytes at a time)
        //**********************************************************************

        for (i=0;i < (((NUM_CACHED_CURSORS+1)*4096)>>2) ;i++)
            *(CursorImageDataPtr + i ) = 0x00000000;

        //**********************************************************************
        // Hide the pointer, since no cursor shape has been defined yet
        //**********************************************************************

        ppdev->pfnShowPointer(ppdev,FALSE);

    }
    else
    {
        //**********************************************************************
        // Hide the pointer.
        //**********************************************************************
        if (!(ppdev->flCaps & CAPS_SW_POINTER))
        {
            
            if(ppdev->ulCursorMemOffset)
            {
                ppdev->pfnShowPointer(ppdev, bEnable);
                status = NvFree( ppdev->hDriver,
                             ppdev->hClient,
                             ppdev->hDevice,
                             ppdev->hCursor);
                ASSERTDD(status == NVOS00_STATUS_SUCCESS,
                /*
                status = NvFreeMemory( ppdev->hDriver,
                                   (PVOID)ppdev->PRAMINRegs);
                ASSERTDD(status == NV000E_ERROR_OK,
                */
                    "Nv3EnablePointer: Cannot free cursor memory");
                ppdev->ulCursorMemOffset = 0;
            }
        }
    }
}


//******************************************************************************
//
//  Function:   NV3MovePointer
//
//  Routine Description:
//
//      Move the NV3 hardware pointer.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV3MovePointer(
PDEV*   ppdev,
LONG    x,
LONG    y)

    {
    LONG    dx;
    LONG    dy;
    volatile ULONG *PRAMDACRegs;
    volatile ULONG *PRAMDAC_CU_START_POS_Reg;
    BYTE *CursorImageDataPtr;
    BYTE *pjDstScan;
    BYTE *pjSrcScan;

    if (x != -1)
    {
    //**************************************************************************
    // Get pointer to RAMDAC register
    //**************************************************************************

    PRAMDACRegs = ppdev->PRAMDACRegs;
    PRAMDAC_CU_START_POS_Reg = (volatile ULONG *)(PRAMDACRegs+OFFSET_PRAMDAC_CU_START_POS_REG);

    //**************************************************************************
    // Adjustment for cursor Hot spot
    //**************************************************************************

    x -= ppdev->xPointerHot;
    y -= ppdev->yPointerHot;


    //**********************************************************************
    // On modes below 400 lines, scanline doubling is used.
    // We need to multiply cursor y position by 2
    //**********************************************************************

    if (ppdev->cyScreen < 400)
        y <<=1;

    //**************************************************************************
    // If TV is enabled, adjust the cursor position (monitor type is nonzero)
    //**************************************************************************

    if (ppdev->MonitorType)
        {
        x += ppdev->TVCursorMin;

        //**********************************************************************
        // If TV is enabled AND filtering is NOT enabled (i.e. 8bpp), we need to 'clip'
        // the cursor in cursor memory. Otherwise, smearing will occur at the bottom.
        // In order to do this, we won't implement cursor caching for this case.
        //**********************************************************************

        if (!(ppdev->FilterEnable))
            {
            //******************************************************************
            // First take the current cursor bitmap (which is already
            // formatted for our hardware) and store it in a temporary buffer.
            //******************************************************************

            pjSrcScan = &ppdev->NV_CursorImageData[0];
            pjDstScan = &ppdev->NV_TVCursorImageData[0];

            NV3_TVCopyCursorData(pjSrcScan,pjDstScan);

            //******************************************************************
            // Clip the cursor manually so that smearing doesn't occur with TV
            //******************************************************************

            NV3_TVClipCursorData(pjDstScan , y, ppdev->cyScreen );

            //******************************************************************
            // Each cursor color bitmap takes up 2k.  However, to fix up noise which
            // occurs when we load cursor shapes, we also allocate the subsequent 2k
            // in cursor memory, and clear it.  So essentially, each cursor
            // image takes up 4k. The 1st 2k is the cursor image, and the
            // following 2k is zero'd out (made transparent)
            //******************************************************************

            CursorImageDataPtr = (BYTE *)ppdev->PRAMINRegs;
            CursorImageDataPtr += NUM_CACHED_CURSORS * 2048 * 2;

            //******************************************************************
            // Update PRAMIN with the updated/clipped cursor image
            //******************************************************************

            NV3_TVCopyCursorData(pjDstScan,CursorImageDataPtr);

            }

        //**********************************************************************
        // If filtering is enabled, we also need to scale the position as well!!
        //**********************************************************************

        if (ppdev->FilterEnable)
            {
            x <<=20;
            x /= ppdev->Scale_x;
            y <<=20;
            y /= ppdev->Scale_y;
            }


        }

    //**************************************************************************
    // Set hardware cursor position
    //**************************************************************************

    *PRAMDAC_CU_START_POS_Reg = ((x & 0xffff) | (y << 16) );

    return;
    }

    }

//******************************************************************************
//
//  Function:   NV3SetPointerShape
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

ULONG NV3SetPointerShape(
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
    
    BYTE    CurrentValue;
    BYTE    mono;
    BYTE    ajBuf[NV1_POINTER_TOTAL_SIZE];
    
    BYTE*   pjSrcScan;
    BYTE*   pjDstScan;
    BYTE*   pbSrc;
    BYTE*   pbDst;

    LONG    i,j,k;    
    LONG    lSrcDelta;
    LONG    lDstDelta;
    LONG    CachedCursorIndex;
    
    ULONG   Checksum;
    ULONG   CursorAddress;
    ULONG   test, mask;
    ULONG   color;
    
    volatile UCHAR *PRMCIO_CRX_COLOR_Reg;
    volatile UCHAR *PRMCIO_CR_COLOR_Reg;
    volatile UCHAR *PRMVIO_SRX_Reg;
    volatile UCHAR *PRMVIO_SR_LOCK_Reg;

    //**************************************************************************
    // Get pointer to pdev
    //**************************************************************************

    ppdev    = (PDEV*) pso->dhpdev;


    //**************************************************************************
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
    //**************************************************************************

    if ( AlphaBlendedCursor ) 
        {

        if (globalOpenGLData.oglDrawableClientCount
#if (_WIN32_WINNT >= 0x0500) && !defined(NV3) && !defined(_WIN64)
            || ppdev->pDriverData->dwOverlaySurfaces
#endif      
        )
            {
            //******************************************************************
            // For alpha blended cursors when OpenGL is enabled,
            // we convert it to a hardware mono cursor.  NV4/NV5 only support
            // 32x32 hardware cursors, so if alpha blended cursors come thru
            // with cx>32 or cy>32, they'll get clipped
            //******************************************************************

            if (cx > 32 )
                cx = 32;
                
            if (cy > 32 )
                cy = 32;            
            }
            
        else
                    
            {
            //******************************************************************
            // Punt all Alpha blended cursors, except when OpenGL is enabled
            //******************************************************************

            return(SPS_DECLINE);
            
            }
                                
        }
        
    //**************************************************************************
    // Only handle 32x32 and 16x16 mono cursors. Send the rest thru DrvCopyBits
    //**************************************************************************

    else if ( !(((cx == 16) && (cy == 16) && (psoColor==NULL))  ||   
           ((cx == 32) && (cy == 32))) )
        {
        //
        // Can't handle this ptr in hw.
        //
        return(SPS_DECLINE);
        }


    
    //**************************************************************************
    // Check whether we'll be drawing a MONOCHROME or COLOR cursor
    //**************************************************************************
    if (psoColor == NULL)

        {

        //**********************************************************************
        // Translate to black and white format for DAC
        // Expand 32 x 32 x 1  (monochrome source ) to 32 x 32 x 16 CursorColorImage
        //
        // We're going to take the requested pointer AND masks and XOR
        // masks and combine them into our work buffer,
        //
        // We currently ONLY allow 32x32 cursors !!!
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
        //**********************************************************************

        pjSrcScan    = psoMsk->pvScan0;
        lSrcDelta    = psoMsk->lDelta;

        if(lSrcDelta != 4)
        {

            // The cursor bitmap is neither TOP-DOWN nor Contiguous.

            pbDst = ajBuf;

            // XOR MASK, AND MASK
            for (i = 2*cy; i != 0; i--)
            {
                pbSrc = pjSrcScan;

                for (j = 4; j != 0; j--)
                {
                    *pbDst = *pbSrc;
                    pbDst++;
                    pbSrc++;
                }

                pjSrcScan += lSrcDelta;
            }
            pjSrcScan = ajBuf;

        }

        pjDstScan    = &ppdev->NV_CursorImageData[0]; // Start with first AND plane
        lDstDelta    = 4;                             // Each scan is 4 bytes (see above)


        //**********************************************************************
        // If TV is enabled AND filtering is NOT enabled (i.e 8bpp), we need to 'clip'
        // the cursor in cursor memory. Otherwise, smearing will occur.
        // In order to do this, we won't implement cursor caching for this case.
        //
        // Also special case 16x16 monochrome cursors!  
        // (Occurs when running SoftImage with OGL).
        // If it's a 16x16 cursor, then don't bother doing cached cursor.
        //**********************************************************************

        if (   ( (ppdev->MonitorType) && (!(ppdev->FilterEnable)))   ||
                 (cx == 16)     )
            {              
            //******************************************************************
            // First take the cursor from GDI and format it for our hardware
            // We'll store it in a temporary buffer first
            //******************************************************************

            pjDstScan = &ppdev->NV_CursorImageData[0];

            //******************************************************************
            // Cursor load is same for NV3 and NV4
            //******************************************************************

            if (cx == 16)
                NV3Load16x16CursorData(pjSrcScan, pjDstScan);
            else
                NV3LoadCursorData(pjSrcScan, pjDstScan);

            //******************************************************************
            // Don't forget to tell hardware where the cursor resides in PRAMIN memory
            //******************************************************************

            PRMCIO_CRX_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CRX_COLOR_Reg += OFFSET_PRMCIO_CRX__COLOR_REG;

            PRMCIO_CR_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CR_COLOR_Reg += OFFSET_PRMCIO_CR__COLOR_REG;

            //******************************************************************
            // Each cursor color bitmap takes up 2k.  However, to fix up noise which
            // occurs when we load cursor shapes, we also allocate the subsequent 2k
            // in cursor memory, and clear it.  So essentially, each cursor
            // image takes up 4k. The 1st 2k is the cursor image, and the
            // following 2k is zero'd out (made transparent)
            //******************************************************************

            CursorAddress = ppdev->ulCursorMemOffset + NUM_CACHED_CURSORS * 2048 * 2;

            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR0_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)(CursorAddress >> 16);

            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR1_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)( (((CursorAddress >> 11) & 0x1f) << 3) | 1);

            //******************************************************************
            // Blast the cursor image data into hardware
            //******************************************************************

            NV3_SetHwCursor(ppdev, x,y, xHot, yHot, pjDstScan);

            return(SPS_ACCEPT_NOEXCLUDE);

            }


#ifdef _X86_
        //**********************************************************************
        // Use assembly to load cursor data as quickly as possible
        // To improve performance, we'll store the data DIRECTLY
        // to PRAMIN memory, update the position, and return as quickly
        // as possible. (We store cursor data at PRAMIN + 14k
        //**********************************************************************

        pjDstScan = (BYTE *) ((ULONG)(ppdev->PRAMINRegs));

        //**********************************************************************
        // Quickly scan the image and get a unique checksum
        // Cursor checksum is same for NV3 and NV4
        //**********************************************************************

        Checksum = NV3CursorChecksum(pjSrcScan);

        //**********************************************************************
        // Check and see if we've already loaded the image in a buffer
        //**********************************************************************

        CachedCursorIndex = -1;

        //**********************************************************************
        // But first check and see if we couldn't distinguish the image
        // If we encounter the flag value, then reload the image.
        // Otherwise, look for the checksum in the cache.
        //**********************************************************************

        if (Checksum != 0x12345678)
            {
            for (i=0;i<NUM_CACHED_CURSORS;i++)
                if (ppdev->SavedCursorCheckSums[i] == Checksum)
                    CachedCursorIndex=i;
            }


        if (CachedCursorIndex !=-1)

            {
            //******************************************************************
            // Load the cached image directly
            //******************************************************************

            //******************************************************************
            // The 2k bitmap of the cursor (32x32 16bpp) must be located in
            // INSTANCE memory on a 2k boundary.  That is, it must lie on a 2k
            // boundary, so only 12 bits are programmable
            //
            // NV3: ------------------------------------------------------------
            //
            //      NV_CIO_CRE_HCUR_ADDR0_INDEX
            //
            //          Bits [6:0]  -> Bits [22:16] of cursor address in instance memory
            //
            //      NV_CIO_CRE_HCUR_ADDR1_INDEX
            //
            //          Bits [7:3]  -> Bits [15:11] of cursor image address in instance memory
            //          Bit  [2]    -> Undefined?
            //          Bit  [1]    -> Turns on scan doubling
            //          Bit  [0]    -> Enable/Disables cursor
            //
            //      Cursor address [10:0] is zero, due to the 2k alignment requirement
            //
            //******************************************************************

            PRMCIO_CRX_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CRX_COLOR_Reg += OFFSET_PRMCIO_CRX__COLOR_REG;

            PRMCIO_CR_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CR_COLOR_Reg += OFFSET_PRMCIO_CR__COLOR_REG;

            //******************************************************************
            // Bit definitions for cursor address are different for NV3 vs NV4
            //******************************************************************
            
            //**************************************************************
            // Need to ALWAYS unlock the CRTC registers here, because
            // on NEC systems, when Power Saving is enabled and the
            // system goes into power down mode, the CRTC registers
            // are locked when power comes back on.  Need to do this 
            // otherwise the cursor caching functionality won't work 
            // (hardware addressing won't work)
            //**************************************************************

            PRMVIO_SRX_Reg = (volatile CHAR *)ppdev->PRMVIORegs;
            PRMVIO_SRX_Reg += OFFSET_PRMVIO_SRX_REG;

            PRMVIO_SR_LOCK_Reg = (volatile CHAR *)ppdev->PRMVIORegs;
            PRMVIO_SR_LOCK_Reg += OFFSET_PRMVIO_SR_LOCK_REG;

            *PRMVIO_SRX_Reg = 6;    // Unlock register
            *PRMVIO_SR_LOCK_Reg = NV_SR_UNLOCK_VALUE;

            //**************************************************************
            // Each cursor color bitmap takes up 2k.  However, to fix up noise which 
            // occurs when we load cursor shapes, we also allocate the subsequent 2k
            // in cursor memory, and clear it.  So essentially, each cursor
            // image takes up 4k. The 1st 2k is the cursor image, and the
            // following 2k is zero'd out (made transparent)
            //**************************************************************

            CursorAddress =  ppdev->ulCursorMemOffset + CachedCursorIndex * 2048 * 2;

            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR0_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)(CursorAddress >> 16);

            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR1_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)( (((CursorAddress >> 11) & 0x1f) << 3) | 1);
                
            }

        else

            {
            //******************************************************************
            // Save the calculated checksum value for the cursor bitmap
            //******************************************************************

            ppdev->SavedCursorCheckSums[ppdev->NextCachedCursor] = Checksum;

            //******************************************************************
            // Each cursor color bitmap takes up 2k.  However, to fix up noise which 
            // occurs when we load cursor shapes, we also allocate the subsequent 2k
            // in cursor memory, and clear it.  So essentially, each cursor
            // image takes up 4k. The 1st 2k is the cursor image, and the
            // following 2k is zero'd out (made transparent)
            //******************************************************************

            CursorAddress = ppdev->NextCachedCursor * 2048 * 2;

            pjDstScan = (BYTE *) ((ULONG)(ppdev->PRAMINRegs));

            NV3LoadCursorData(pjSrcScan,(BYTE *)((ULONG)pjDstScan+(ULONG)(CursorAddress)));
            NV3LoadCursorData(pjSrcScan,(BYTE *) (&(ppdev->SavedCursorBitmaps[ppdev->NextCachedCursor][0])));

            //******************************************************************
            // Tell hardware where the cursor resides in PRAMIN memory
            //******************************************************************

            PRMCIO_CRX_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CRX_COLOR_Reg += OFFSET_PRMCIO_CRX__COLOR_REG;

            PRMCIO_CR_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CR_COLOR_Reg += OFFSET_PRMCIO_CR__COLOR_REG;

            //******************************************************************
            // Bit definitions (for cursor address)
            //******************************************************************

            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR0_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)((CursorAddress + ppdev->ulCursorMemOffset) >> 16);

            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR1_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)( ((((CursorAddress + ppdev->ulCursorMemOffset) >> 11) & 0x1f) << 3) | 1);
            
            //******************************************************************
            // Reset the next cached cursor entry (round robin)
            //******************************************************************

            ppdev->NextCachedCursor++;
            if (ppdev->NextCachedCursor >= NUM_CACHED_CURSORS)
                ppdev->NextCachedCursor=0;
            }


        //**********************************************************************
        // Get HotSpot
        //**********************************************************************
        ppdev->xPointerHot = xHot;
        ppdev->yPointerHot = yHot;

        ppdev->pfnMovePointer(ppdev, x, y);
        return(SPS_ACCEPT_NOEXCLUDE);

#else

        for (i = 0; i < 32; i++)                      // 32 pixels vertically
            for (j = 0; j < 4; j++)                   // 4 bytes accross (32 pixels)
                {
                //**************************************************************
                // Read 1 byte (8 pixels) and write 8 words
                // XOR mask is 128 bytes from the beginning of source buffer
                //**************************************************************
#define COLOR_OFFSET    (32*4)

                mask = pjSrcScan[i*4 + j];                  // Get a Mask byte (8 pixels)
                mono = pjSrcScan[COLOR_OFFSET+ i*4 + j];    // Get a Mono byte (8 pixels)

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

                test = 0x80;
                for (k = 0; k < 8; k++, test >>= 1)
                    {
                    if (test & mono)        // Test this bit
                        color = 0x7fff;     // White
                    else
                        color = 0;          // Black

                    if (!(test & mask))
                        color |= 0x8000;    // Not transparent

                    //**********************************************************
                    // Store 16 bits for NV cursor image data
                    //**********************************************************

                    *((WORD *)(pjDstScan+ i*32*2 + j*8*2 + k*2)) = (USHORT) color;
                    }
                }


#endif


        }   // Draw monochrome cursor (psoColor == NULL)

    else

        {

        WORD    red;
        WORD    green;
        WORD    blue;
        WORD    alpha;
        ULONG   iSrcBitDepth;
        LONG    lPitch;
        BYTE*   pjSrcColor;
        BYTE*   pjSrcColorMask;
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

        if (psoColor->dhsurf != NULL)
            {
            //******************************************************************
            // Device managed bitmap
            //******************************************************************
            if (((DSURF *) psoColor->dhsurf)->dt == DT_SCREEN)
                {
                //**************************************************************
                // Ptr surface is in offscreen device memory, need to wait for
                // fifo empty and graphics engine idle prior to copying the
                // pointer bitmap.
                //**************************************************************
                pjSrcColor      = (PBYTE) (((DSURF *) psoColor->dhsurf)->LinearPtr);
                lPitch          = (((DSURF *) psoColor->dhsurf))->LinearStride;
                iSrcBitDepth =  (((DSURF *) psoColor->dhsurf)->ppdev)->iBitmapFormat;
                ppdev->pfnWaitEngineBusy(ppdev);
                }
            else // (pdsurfSrc->dt == DT_DIB)
                {
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

            }
        else if (psoColor->pvScan0 != NULL)
            {
            //******************************************************************
            // GDI managed host memory bitmap
            //******************************************************************
            pjSrcColor      = (PBYTE) psoColor->pvScan0;
            lPitch          = psoColor->lDelta;
            iSrcBitDepth =  psoColor->iBitmapFormat;
            }
        else
            {
            //******************************************************************
            // If ptr bmp is neither DFB nor DIB, it's likely an error occurred.
            //******************************************************************
            return(SPS_DECLINE);
            }

        //**********************************************************************
        // Can't use ptr without address of the bitmap
        //**********************************************************************

        if (pjSrcColor == NULL)
            {
            return(SPS_DECLINE);
            }

        //**********************************************************************
        // psoMsk is NULL when using an Alpha Blended cursor
        //**********************************************************************

        if (!(AlphaBlendedCursor)) 
            {
            pjSrcColorMask  = psoMsk->pvScan0;
            lSrcDelta       = psoMsk->lDelta;
            }
            
        pjDstScan       = &ppdev->NV_CursorImageData[0];

        switch (iSrcBitDepth)
            {
            //******************************************************************
            // We gotta look up the color from the palette to convert to RGB
            //******************************************************************

            case BMF_8BPP:
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
                for (i = 0; i < 32; i++)
                    {
                    mask = *(PULONG) (pjSrcColorMask + i*lSrcDelta);     // Get 8 AND bits
                    test = 0x00000080;
                    for (k = 0; k < 32; k++, test >>=1)
                        {
                        if (!(k & 7))
                            test = 0x00000080 << k;
                        color = *((PBYTE) (pjSrcColor + i*lPitch + k)); // Get 15 bits of color
                        blue =  pPal[color].peBlue;
                        green = pPal[color].peGreen;
                        red =  pPal[color].peRed;
                        color = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);
                        if (!(test & mask))
                            color |= 0x8000;              // Not transparent

                        //******************************************************
                        // Store 16 bits for NV cursor image data
                        //******************************************************
                        *((PUSHORT)(pjDstScan+ i*32*2 + k*2)) = (USHORT) color;
                        }
                    }
                break;

            //******************************************************************
            // Screen is currently in 16bpp mode
            //******************************************************************

            case BMF_16BPP:
                for (i = 0; i < 32; i++)
                   {
                   mask = *(PULONG) (pjSrcColorMask + i*lSrcDelta);     // Get 8 AND bits
                   test = 0x00000080;
                   for (k = 0; k < 32; k++, test >>=1)
                       {
                       if (!(k & 7))
                           test = 0x00000080 << k;

                       if (ppdev->flGreen == 0x03e0)
                            {
                            //**************************************************
                            // For 555: We can use the source color as is
                            //          since the color cursor is always 555
                            //**************************************************
                            color = *((PUSHORT) (pjSrcColor + i*lPitch + k*2)); // Get 15 bits of color
                            }
                       else
                            {
                            //**************************************************
                            // For 565: Convert from 565 bitmap format to 555
                            //          since the color cursor is always 555
                            //**************************************************
                            color = *((PUSHORT) (pjSrcColor + i*lPitch + k*2)); // Get 16 bits of color
                            red   = (WORD)(color & 0xf800); // 5 bits
                            green = (WORD)(color & 0x07c0); // Just take top 5 of 6 bits !
                            blue  = (WORD)(color & 0x001f); // 5 bits
                            color = (  (red >> 1 ) | (green >> 1 ) | (blue) );
                                                            
                            }

                       if (!(test & mask))
                             color |= 0x8000;                // Not transparent

                       //******************************************************
                       // Store 16 bits for NV cursor image data
                       //******************************************************
                       *((WORD *)(pjDstScan+ i*32*2 + k*2)) = (USHORT) color;
                       }
                   }

                break;

            //******************************************************************
            // Screen is currently in 32bpp mode
            //******************************************************************

            case BMF_32BPP:
                if (AlphaBlendedCursor)
                    {
                    // 32bpp color cursor                     
                    for (i = 0; i < cy; i++)
                        {
                        for (k = 0; k < (LONG)cx; k++)
                            {
                            blue =  *(pjSrcColor + i*lPitch + k*4 + 0);
                            green = *(pjSrcColor + i*lPitch + k*4 + 1);
                            red =   *(pjSrcColor + i*lPitch + k*4 + 2);
                            alpha = *(pjSrcColor + i*lPitch + k*4 + 3);
                            color = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);

                            if (alpha == 0xff)
                                color |= 0x8000;                // Not transparent

                            //**************************************************
                            // Store 16 bits for NV cursor image data
                            //**************************************************
                            *((PUSHORT)(pjDstScan+ i*32*2 + k*2)) = (USHORT) color;
                            }


                        //******************************************************
                        // If alpha blended cursor width < 32, then fill out
                        // rest of pixels with transparent color
                        //******************************************************

                        for (;k < 32; k++)
                            {
                            blue =  0x0;
                            green = 0x0;
                            red =   0x0;
                            alpha = 0x0;
                            color = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);

                            if (alpha == 0xff)
                                color |= 0x8000;                // Not transparent

                            //**************************************************
                            // Store 16 bits for NV cursor image data
                            //**************************************************
                            *((PUSHORT)(pjDstScan+ i*32*2 + k*2)) = (USHORT) color;
                            }


                        }


                    //**********************************************************
                    // If alpha blended cursor height < 32, then fill out
                    // remaining scanlines with transparent color
                    //**********************************************************

                    for (; i < 32; i++)
                        {
                        for (k = 0; k < 32; k++)
                            {
                            blue =  0x0;
                            green = 0x0;
                            red =   0x0;
                            alpha = 0x0;
                            color = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);

                            if (alpha == 0xff)
                                color |= 0x8000;                // Not transparent

                            //******************************************************
                            // Store 16 bits for NV cursor image data
                            //******************************************************
                            *((PUSHORT)(pjDstScan+ i*32*2 + k*2)) = (USHORT) color;
                            }

                        }

                    }
                    
                else
                
                    {
                    // 32bpp color cursor                     
                    for (i = 0; i < 32; i++)
                        {
                        mask = *(PULONG) (pjSrcColorMask + i*lSrcDelta);     // Get 8 AND bitsif (!(k & 7))
                        test = 0x00000080;
                        for (k = 0; k < 32; k++, test >>=1)
                            {
                            if (!(k & 7))
                                test = 0x00000080 << k;
    
                            blue =  *(pjSrcColor + i*lPitch + k*4 + 0);
                            green = *(pjSrcColor + i*lPitch + k*4 + 1);
                            red =   *(pjSrcColor + i*lPitch + k*4 + 2);
                            color = ((red & 0xf8) << 7) | ((green & 0xf8) << 2) | ((blue & 0xf8) >> 3);
                            if (!(test & mask))
                                color |= 0x8000;                // Not transparent
    
                            //******************************************************
                            // Store 16 bits for NV cursor image data
                            //******************************************************
                            *((PUSHORT)(pjDstScan+ i*32*2 + k*2)) = (USHORT) color;
                            }
                        }
                    }
                    

                break;

             default:
                //**************************************************************
                // Can't handle any other bit depths
                //**************************************************************
                return(SPS_DECLINE);

            }   // Switch
            //******************************************************************
            // Tell hardware where the cursor resides in PRAMIN memory
            //******************************************************************

            PRMCIO_CRX_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CRX_COLOR_Reg += OFFSET_PRMCIO_CRX__COLOR_REG;

            PRMCIO_CR_COLOR_Reg = (volatile CHAR *)ppdev->PRMCIORegs;
            PRMCIO_CR_COLOR_Reg += OFFSET_PRMCIO_CR__COLOR_REG;

            //******************************************************************
            // Each cursor color bitmap takes up 2k.  However, to fix up noise which
            // occurs when we load cursor shapes, we also allocate the subsequent 2k
            // in cursor memory, and clear it.  So essentially, each cursor
            // image takes up 4k. The 1st 2k is the cursor image, and the
            // following 2k is zero'd out (made transparent)
            //******************************************************************

            CursorAddress = ppdev->ulCursorMemOffset + NUM_CACHED_CURSORS * 2048 * 2;

            //******************************************************************
            // Bit definitions (for cursor address)
            //******************************************************************
            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR0_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)(CursorAddress >> 16);

            *PRMCIO_CRX_COLOR_Reg = (NV_CIO_CRE_HCUR_ADDR1_INDEX & 0x3f);
            *PRMCIO_CR_COLOR_Reg  = (volatile UCHAR)( (((CursorAddress >> 11) & 0x1f) << 3) | 1);

        } // psoColor != NULL

    //**************************************************************************
    // Set the buffer for NV1 monochrome hardware or NV3 hardware color cursor
    //**************************************************************************

    pjDstScan = &ppdev->NV_CursorImageData[0];

    //**************************************************************************
    // Blast the cursor image data into hardware
    //**************************************************************************
    NV3_SetHwCursor(ppdev, x,y, xHot, yHot, pjDstScan);

    //**************************************************************************
    // Since it's a hardware pointer, GDI doesn't have to worry about
    // overwriting the pointer on drawing operations (meaning that it
    // doesn't have to exclude the pointer), so we return 'NOEXCLUDE'.
    // Since we're returning 'NOEXCLUDE', we also don't have to update
    // the 'prcl' that GDI passed us.
    //**************************************************************************

    return(SPS_ACCEPT_NOEXCLUDE);

    }

#ifdef _WIN64
//*****************************************************************************
//
// Function: NV3LoadCursorData()
//
// Routine Description:
//
//   C version for IA64 only.
//
// Arguments:
//
// Return Value:
//
//     None.
//
//*****************************************************************************

DWORD MonoTable[4] = { 0x00000000, 0x7fff0000, 0x00007fff, 0x7fff7fff };
DWORD AndTable[4]  = { 0x80008000, 0x00008000, 0x80000000, 0x00000000 };

VOID NV3LoadCursorData(
BYTE*   pjSrcScan,
BYTE*   pjDstScan
)
    {
    UCHAR*    AndMaskPtr;
    UCHAR*     MonoMaskPtr;
    ULONG*     DstPtr;
    UCHAR      AndMask;
    UCHAR      MonoMask;
    LONG       i, j, k;

    AndMaskPtr = (UCHAR *)pjSrcScan;
    MonoMaskPtr = (UCHAR *)(pjSrcScan + 128);
    DstPtr = (ULONG *)pjDstScan;

   //***************************************************************************
   // Iterate through 32 rows of data
   //***************************************************************************
    for (i = 0; i < 32; i++)
        {
        //**********************************************************************
        // Load 16bpp pels - uses tables to convert ones to 0x7fff and zeros to
        // 0x0000.  Also uses table to add transparency bit from AND mask.
        // Zero in AND mask converts to 0x8000, ones to 0x0000.
        //
        // Create two pels at a time with 4 entry tables, above.  If pjDstScan
        // will also be quad word aligned, a future optimization might be
        // creating 4 pels at a time with 16 entry tables.
        //**********************************************************************
        

        for (j = 0; j < 4; j++)
            {
            AndMask = *AndMaskPtr++;
            MonoMask = *MonoMaskPtr++;
            for (k = 0; k < 4; k++)
                {
                *DstPtr++ = AndTable[(AndMask >> 6) & 0x3] |
                            MonoTable[(MonoMask >> 6) & 0x3];
                AndMask <<= 2;
                MonoMask <<= 2;
                }
            }
        }
    }

//*****************************************************************************
//
// Function: NV3Load16x16CursorData()
//
// Routine Description:
//
//   C version for IA64 only.
//
// Arguments:
//
// Return Value:
//
//     None.
//
//*****************************************************************************

VOID NV3Load16x16CursorData(
BYTE*   pjSrcScan,
BYTE*   pjDstScan
)
    {
    UCHAR*    AndMaskPtr;
    UCHAR*     MonoMaskPtr;
    ULONG*     DstPtr;
    UCHAR      AndMask;
    UCHAR      MonoMask;
    LONG       i, j, k;

    AndMaskPtr = (UCHAR *)pjSrcScan;
    MonoMaskPtr = (UCHAR *)(pjSrcScan + 64);
    DstPtr = (ULONG *)pjDstScan;

   //***************************************************************************
   // Iterate through 16 rows of data
   //***************************************************************************
    for (i = 0; i < 16; i++)
        {
        //**********************************************************************
        // Load 16bpp pels - uses tables to convert ones to 0x7fff and zeros to
        // 0x0000.  Also uses table to add transparency bit from AND mask.
        // Zero in AND mask converts to 0x8000, ones to 0x0000.
        //**********************************************************************
        

        for (j = 0; j < 2; j++)
            {
            AndMask = *AndMaskPtr++;
            MonoMask = *MonoMaskPtr++;
            for (k = 0; k < 4; k++)
                {
                *DstPtr++ = AndTable[(AndMask >> 6) & 0x3] |
                            MonoTable[(MonoMask >> 6) & 0x3];
                AndMask <<= 2;
                MonoMask <<= 2;
                }
            }
            AndMaskPtr += 2;
            MonoMaskPtr += 2;

        //**********************************************************************
        // Blank out remaining 'width' portion of 32x32 cursor (clear 16 pels)
        //**********************************************************************
        for (j = 0; j < 8; j++)
            *DstPtr++ = 0x0;
            }

    //**************************************************************************
    // Blank out remaining 'height' portion of 32x32 cursor
    // (clear out 16 lines * 32 pels = 512 pels = 256 dwords)
    //**************************************************************************
    for (i = 0; i < 256; i++)
        *DstPtr++ = 0x0;
    }
#endif // _WIN64
#endif // NV3
