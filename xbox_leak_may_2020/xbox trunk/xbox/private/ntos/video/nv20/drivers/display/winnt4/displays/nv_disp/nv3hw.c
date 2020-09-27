//******************************************************************************
//
// Module Name:
//
//     NV3HW.C
//
// Abstract:
//
//     Implements NV3 specific routines
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//     This driver was adapted from the S3 Display driver
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "precomp.h"

#ifdef NV3
#include "oldnv332.h"
#define NV32_INCLUDED // Prevents re-inclusion of real nv32 header
#include "driver.h"

#include "nv3a_ref.h"

#include "nvsubch.h"
#include "nvalpha.h"

//******************************************************************************
// Useful defines (taken from NV.H).  Need to cleanup NV.H so we can include
// it without compiler failing, but don't have time right now....
//******************************************************************************

#define DEVICE_BASE(d)          (0?d)


#define OFFSET_PGRAPH_BOFFSET0_REG      (NV_PGRAPH_BOFFSET0 - DEVICE_BASE(NV_PGRAPH))
#define OFFSET_PGRAPH_BOFFSET1_REG      (NV_PGRAPH_BOFFSET1 - DEVICE_BASE(NV_PGRAPH))
#define OFFSET_PGRAPH_BOFFSET3_REG      (NV_PGRAPH_BOFFSET3 - DEVICE_BASE(NV_PGRAPH))
#define OFFSET_PGRAPH_BPITCH0_REG       (NV_PGRAPH_BPITCH0 - DEVICE_BASE(NV_PGRAPH))
#define OFFSET_PGRAPH_BPITCH1_REG       (NV_PGRAPH_BPITCH1 - DEVICE_BASE(NV_PGRAPH))
#define OFFSET_PGRAPH_BPITCH3_REG       (NV_PGRAPH_BPITCH3 - DEVICE_BASE(NV_PGRAPH))
#define OFFSET_PGRAPH_FIFO_REG          (NV_PGRAPH_FIFO - DEVICE_BASE(NV_PGRAPH))
#define OFFSET_PGRAPH_DEBUG_3_REG       (NV_PGRAPH_DEBUG_3 - DEVICE_BASE(NV_PGRAPH))


#define OFFSET_PRMCIO_INP0_COLOR_REG                0x3da

#define OFFSET_PRMCIO_CRX__COLOR_REG                0x3d4
#define OFFSET_PRMCIO_CR__COLOR_REG                 0x3d5

#define OFFSET_PRMVIO_SRX_REG                       0x3c4
#define OFFSET_PRMVIO_SR_LOCK_REG                   0x3c5
#define OFFSET_PRMVIO_MISC_READ_REG                 0x3cc
#define OFFSET_PRAMDAC_CU_START_POS_REG             0x0

#define NV_SR_UNLOCK_VALUE                          0x00000057
#define NV_SR_LOCK_VALUE                            0x00000099

//******************************************************************************
// Forward Declarations
//******************************************************************************

VOID NV3_ClearZ(PDEV *ppdev ,ULONG x,ULONG y,ULONG Width, ULONG Height, ULONG zvalue);
VOID NV3_ClearSurface(PDEV *ppdev ,ULONG x,ULONG y,ULONG Width, ULONG Height, ULONG color);
VOID NV3_BlitSurface(PDEV *ppdev ,RECTL *Src, RECTL *Dst);

VOID NV3_SetSourceBase(PDEV*   ppdev, ULONG Offset, LONG Stride);
VOID NV3_SetDestBase(PDEV*   ppdev, ULONG Offset, LONG Stride);
VOID NV3_OGLSetSourceBase(PDEV*   ppdev, ULONG Offset, LONG Stride);
VOID NV3_OGLSetDestBase(PDEV*   ppdev, ULONG Offset, LONG Stride);
VOID NV3_SetZBufferBase(PDEV*   ppdev, ULONG MemoryOffset, LONG Stride);

VOID NV3_SetDisplayBase(PDEV*   ppdev, ULONG MemoryOffset);

VOID NV3_SetClipRect(PDEV *ppdev, RECTL *pClip);
VOID NV3_SetDitherMode(PDEV *ppdev, BOOL enableDither);
VOID NV3_SynchronizeHW(PDEV*   ppdev);
VOID NV3_SwapDisplaySurface(PDEV *ppdev, ULONG displaybase, ULONG renderbase, BOOL waitforVSync);

VOID NV3_VerifyFunctions(PDEV*   ppdev);
VOID NV3_RestoreState(PDEV*   ppdev, ULONG ZBufferOffset);
VOID NV3_VerifyTriangleWithZBuffer(PDEV *ppdev,ULONG,ULONG);


//******************************Public*Routine**********************************
//
// Function: NV3_ClearZ
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

VOID NV3_ClearZ(PDEV *ppdev ,ULONG x,ULONG y,ULONG Width, ULONG Height, ULONG zvalue)

    {
    ULONG SavedDestOffset;
    ULONG SavedDestPitch;
    Nv3ChannelPio *nv  = (Nv3ChannelPio *) ppdev->pjMmBase;
    USHORT FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Fill Z buffer with specified value
    //**************************************************************************

    while (FreeCount < 3*4)
       FreeCount = NvGetFreeCount(nv, CLEARZ_SUBCHANNEL);
    FreeCount -= 3*4;

    nv->subchannel[CLEARZ_SUBCHANNEL].nv1RenderSolidRectangle.Color = zvalue;
    nv->subchannel[CLEARZ_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].point = ((y << 16) | (x & 0xffff));
    nv->subchannel[CLEARZ_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].size = ((Height << 16) | (Width & 0xffff) );

    ppdev->NVFreeCount = FreeCount;

    }




//******************************Public*Routine**********************************
//
// Function: NV3_ClearSurface
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

VOID NV3_ClearSurface(PDEV *ppdev ,ULONG x,ULONG y,ULONG Width, ULONG Height, ULONG color)

    {
    ULONG Pitch;
    Nv3ChannelPio   *nv = (Nv3ChannelPio *) ppdev->pjMmBase;
    USHORT FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Fill Z buffer with specified value
    //**************************************************************************

    while (FreeCount < 5*4)
       FreeCount = NvGetFreeCount(nv, RECTANGLE_SUBCHANNEL);
    FreeCount -= 5*4;

    //**************************************************************************
    // We'll need to temporarily replace the ROP solid object,
    // because OpenGL currently uses this subchannel to contain
    // the D3DTriangle object with textures in System Memory.
    //**************************************************************************

    nv->subchannel[ROP_SOLID_SUBCHANNEL].SetObject             = DD_ROP5_SOLID; //MY_ROP_SOLID;

    nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = 0xcc ;
    nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Color = color;
    nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].point = ((y << 16) | (x & 0xffff));
    nv->subchannel[RECTANGLE_SUBCHANNEL].nv1RenderSolidRectangle.Rectangle[0].size = ((Height << 16) | (Width & 0xffff) );

    //**************************************************************************
    // We need to restore the D3D Triangle (SYSMEM) object
    // because OpenGL expects this subchannel to contain
    // the D3DTriangle object with textures in System Memory.
    //**************************************************************************

    // nv->subchannel[D3DTRIANGLE_SYSMEM_SUBCHANNEL].SetObject     = MY_RENDER_D3D_TRIANGLE_SYSMEM;

    ppdev->NVFreeCount = FreeCount;

    }



//******************************Public*Routine**********************************
//
// Function: NV3_BlitSurface
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************

VOID NV3_BlitSurface(PDEV *ppdev ,RECTL *Src, RECTL *Dst)

    {
    ULONG   srcX,srcY;
    ULONG   dstX,dstY;
    ULONG   dstWidth,dstHeight;

    Nv3ChannelPio   *nv = (Nv3ChannelPio *) ppdev->pjMmBase;
    USHORT FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Get source coordinates
    //**************************************************************************

    srcX = Src->left;
    srcY = Src->top;

    //**************************************************************************
    // Get destination coordinates
    //**************************************************************************

    dstX = Dst->left;
    dstY = Dst->top;

    //**************************************************************************
    // Get width and height
    //**************************************************************************

    dstWidth = Src->right - Src->left;
    dstHeight = Src->bottom - Src->top;

    //**************************************************************************
    // Wait for FIFO to be ready, then draw
    // We know that this is a straight SOLID FILL with no
    // pattern or source.  So just set ROP to SRCCOPY.
    //**************************************************************************

    while (FreeCount < 5*4)
       FreeCount = NvGetFreeCount(nv, BLIT_SUBCHANNEL);
    FreeCount -= 5*4;

    //**************************************************************************
    // We'll need to temporarily replace the ROP solid object,
    // because OpenGL currently uses this subchannel to contain
    // the D3DTriangle object with textures in System Memory.
    //**************************************************************************

    nv->subchannel[ROP_SOLID_SUBCHANNEL].SetObject             = DD_ROP5_SOLID; //MY_ROP_SOLID;


    nv->subchannel[ROP_SOLID_SUBCHANNEL].nv3ContextRop.SetRop5 = NV_SRCCOPY ;

    nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointIn = ( (srcY << 16) | ((srcX) & 0xffff) );
    nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.ControlPointOut = ( (dstY << 16) | ((dstX) & 0xffff) );
    nv->subchannel[BLIT_SUBCHANNEL].nv1ImageBlit.Size = ( (dstHeight << 16) | ((dstWidth) & 0xffff) );

    //**************************************************************************
    // We need to restore the D3D Triangle (SYSMEM) object
    // because OpenGL expects this subchannel to contain
    // the D3DTriangle object with textures in System Memory.
    //**************************************************************************

    // nv->subchannel[D3DTRIANGLE_SYSMEM_SUBCHANNEL].SetObject     = MY_RENDER_D3D_TRIANGLE_SYSMEM;

    ppdev->NVFreeCount = FreeCount;
    }



//******************************************************************************
//
// Function: NV3_SetSourceBase()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV3_SetSourceBase(PDEV*   ppdev, ULONG Offset, LONG Stride)

    {
    USHORT  FreeCount = ppdev->NVFreeCount;
    Nv3ChannelPio   *nv = (Nv3ChannelPio *) ppdev->pjMmBase;

    //**************************************************************************
    // Performance optimization, check if offset and pitch
    // are already up to date
    //**************************************************************************

    if ( (Offset == ppdev->CurrentSourceOffset)  &&  (Stride == (LONG)ppdev->CurrentSourcePitch))
        return;

    //**************************************************************************
    // Setup SRCIMAGE_IN_MEMORY object.
    //**************************************************************************
    while (FreeCount < 4*4)
        FreeCount = NvGetFreeCount(nv, SRCIMAGE_IN_MEMORY_SUBCHANNEL);
    FreeCount -= 4*4;


    nv->subchannel[SRCIMAGE_IN_MEMORY_SUBCHANNEL].SetObject =
         DD_SRC_IMAGE_IN_MEMORY;
    nv->subchannel[SRCIMAGE_IN_MEMORY_SUBCHANNEL].nv3ContextSurface1.SetImageOffset =
        Offset;
    nv->subchannel[SRCIMAGE_IN_MEMORY_SUBCHANNEL].nv3ContextSurface1.SetImagePitch =
        Stride;

    //**************************************************************************
    // Restore the original object in the subchannel
    //**************************************************************************
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].SetObject = DD_IMAGE_PATTERN;

    //**************************************************************************
    // Save the updated offset and pitch, update global freecount.
    //**************************************************************************
    ppdev->CurrentSourceOffset = Offset;
    ppdev->CurrentSourcePitch  = Stride;

    //**************************************************************************
    // NOTE: This function is currently used by the 2d display driver ONLY.
    //       So update the global free count for the DISPLAY driver
    //**************************************************************************
    ppdev->NVFreeCount = FreeCount;


    }


//******************************************************************************
//
// Function: NV3_SetDestBase()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV3_SetDestBase(PDEV*   ppdev, ULONG Offset, LONG Stride)

    {
    USHORT FreeCount = ppdev->NVFreeCount;
    Nv3ChannelPio   *nv = (Nv3ChannelPio *) ppdev->pjMmBase;

    //**************************************************************************
    // Performance optimization, check if offset and pitch
    // are already up to date
    //**************************************************************************

    if ( (Offset == ppdev->CurrentDestOffset)  &&  (Stride == (LONG)ppdev->CurrentDestPitch))
        return;

    //**************************************************************************
    // Setup DSTIMAGE_IN_MEMORY object.
    //**************************************************************************
    while (FreeCount < 4*4)
        FreeCount = NvGetFreeCount(nv, DD_SPARE);
    FreeCount -= 4*4;

    if (ppdev->dDrawSpareSubchannelObject != DD_PRIMARY_IMAGE_IN_MEMORY)
        {
        nv->subchannel[DD_SPARE].SetObject = DD_PRIMARY_IMAGE_IN_MEMORY;
        ppdev->dDrawSpareSubchannelObject     = DD_PRIMARY_IMAGE_IN_MEMORY;
        }

    nv->subchannel[DD_SPARE].nv3ContextSurface0.SetImageOffset =
        Offset;
    nv->subchannel[DD_SPARE].nv3ContextSurface0.SetImagePitch =
        Stride;

    //**************************************************************************
    // Save the updated offset and pitch, update global freecount.
    //**************************************************************************
    ppdev->CurrentDestOffset = Offset;
    ppdev->CurrentDestPitch  = Stride;

    //**************************************************************************
    // NOTE: This function is currently used by the 2d display driver ONLY.
    //       So update the global free count for the DISPLAY driver
    //**************************************************************************
    ppdev->NVFreeCount = FreeCount;


    }



//******************************************************************************
//
// Function: NV3_OGLSetSourceBase()
//
// Routine Description:
//
//          This version of SetSourceBase requires the IMAGE_IN_MEMORY objects
//          to be present in the NV user channel.  This occurs whenever
//          OpenGL is initialized, which means that this function is NOT AVAILABLE
//          for the 2D Display driver to use (because all 8 subchannels stay
//          constant when the 2D Display driver is running)
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV3_OGLSetSourceBase(PDEV*   ppdev, ULONG Offset, LONG Stride)

    {
    Nv3ChannelPio           *nv = (Nv3ChannelPio *) ppdev->pjMmBase;
    USHORT FreeCount = ppdev->NVFreeCount;

    //**************************************************************************
    // Set offset and stride
    //**************************************************************************

    while (FreeCount < 2*4)
       FreeCount = NvGetFreeCount(nv, SRCIMAGE_IN_MEMORY_SUBCHANNEL);
    FreeCount -= 2*4;

    nv->subchannel[SRCIMAGE_IN_MEMORY_SUBCHANNEL].nv3ContextSurface1.SetImagePitch = Stride;
    nv->subchannel[SRCIMAGE_IN_MEMORY_SUBCHANNEL].nv3ContextSurface1.SetImageOffset = Offset;

    ppdev->NVFreeCount = FreeCount;
    }


//******************************************************************************
//
// Function: NV3_OGLSetDestBase()
//
// Routine Description:
//
//          This version of SetDestBase requires the IMAGE_IN_MEMORY objects
//          to be present in the NV user channel.  This occurs whenever
//          OpenGL is initialized, which means that this function is NOT AVAILABLE
//          for the 2D Display driver to use (because all 8 subchannels stay
//          constant when the 2D Display driver is running)
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV3_OGLSetDestBase(PDEV*   ppdev, ULONG Offset, LONG Stride)

    {
    USHORT FreeCount = ppdev->NVFreeCount;
    Nv3ChannelPio   *nv = (Nv3ChannelPio*) ppdev->pjMmBase;

    //**************************************************************************
    //Set offset and stride
    //**************************************************************************

    while (FreeCount < 2*4)
       FreeCount = NvGetFreeCount(nv, DD_SPARE);
    FreeCount -= 2*4;

    nv->subchannel[DD_SPARE].nv3ContextSurface0.SetImagePitch = Stride;
    nv->subchannel[DD_SPARE].nv3ContextSurface0.SetImageOffset = Offset;

    ppdev->NVFreeCount = FreeCount;
    }


//******************************************************************************
//
// Function: NV3_SetDisplayBase()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV3_SetDisplayBase(PDEV*   ppdev, ULONG Offset)

    {
    NV3_FlipBuffer(ppdev,Offset);

    //**************************************************************************
    // Save the updated offset
    //**************************************************************************

    ppdev->CurrentDisplayOffset = Offset;

    }


//******************************************************************************
//
// Function: NV3_SetZBufferBase()
//
// Routine Description:
//
// Arguments:
//
//     ppdev - Pointer to the physical device structure
//
// Return Value:
//
//     None.
//
//******************************************************************************


VOID NV3_SetZBufferBase(PDEV*   ppdev, ULONG Offset, LONG Stride)

    {
    USHORT FreeCount = ppdev->NVFreeCount;
    Nv3ChannelPio   *nv = (Nv3ChannelPio *) ppdev->pjMmBase;

    //**************************************************************************
    // Setup ZETA_BUFFER_IN_MEMORY object.
    //**************************************************************************
    while (FreeCount < 7*4)
        FreeCount = NvGetFreeCount(nv, CLEARZ_SUBCHANNEL);
    FreeCount -= 7*4;

    nv->subchannel[CLEARZ_SUBCHANNEL].SetObject = DD_ZETA_BUFFER;
    nv->subchannel[CLEARZ_SUBCHANNEL].nv3ContextSurface3.SetImagePitch = Stride;
    nv->subchannel[CLEARZ_SUBCHANNEL].nv3ContextSurface3.SetImageOffset = Offset;

    nv->subchannel[CLEARZ_SUBCHANNEL].SetObject = DD_ZBUFFER_IMAGE_IN_MEMORY;
    nv->subchannel[CLEARZ_SUBCHANNEL].nv3ContextSurface3.SetImagePitch = Stride;
    nv->subchannel[CLEARZ_SUBCHANNEL].nv3ContextSurface3.SetImageOffset = Offset;
    nv->subchannel[CLEARZ_SUBCHANNEL].SetObject = MY_CLEARZ_RECTANGLE;

    //**************************************************************************
    // Save the updated offset and pitch, update global freecount.
    //**************************************************************************
    ppdev->CurrentZOffset = Offset;
    ppdev->CurrentZPitch  = Stride;
    ppdev->NVFreeCount = FreeCount;
    }




//******************************************************************************
//
//  Function:   NV3_SetClipRect
//
//  Routine Description:
//
//      Hardware-specific utility functions:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV3_SetClipRect(PDEV *ppdev, RECTL *pClip)

    {
    USHORT FreeCount = ppdev->NVFreeCount;
    Nv3ChannelPio  *nv  = (Nv3ChannelPio *)ppdev->pjMmBase;

    while (FreeCount < 2*4)
       FreeCount = NvGetFreeCount(nv, CLIP_SUBCHANNEL);
    FreeCount -= 2*4;

    nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetPoint = (( (pClip->top) << 16) | pClip->left );
    nv->subchannel[CLIP_SUBCHANNEL].nv1ImageBlackRectangle.SetSize = (( (pClip->bottom - pClip->top)<<16) | (pClip->right - pClip->left));

    ppdev->NVClipResetFlag=1;       // Notify rest of driver that next guy needs
                                    // to restore it
    ppdev->NVFreeCount = FreeCount;
    }


//******************************************************************************
//
//  Function:   NV3_SetDitherMode
//
//  Routine Description:
//
//      Hardware-specific utility functions:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID NV3_SetDitherMode(PDEV *ppdev, BOOL enableDither)

    {
    volatile DWORD *PGRAPH_DEBUG_3_Reg;
    volatile DWORD *PGRAPH_STATUS_Reg;
    volatile DWORD *PGRAPH_FIFO_Reg;
    ULONG value;

    //**************************************************************************
    // 1st make sure the engine is not busy
    //**************************************************************************

    NV3_SynchronizeHW(ppdev);

//    //**************************************************************************
//    // 2nd make sure the FIFO is disabled
//    // NV_PGRAPH_FIFO_ACCESS_DISABLED --> Clear bit 0
//    //**************************************************************************
//
//    PGRAPH_FIFO_Reg = (volatile DWORD *)ppdev->PGRAPHRegs;
//    PGRAPH_FIFO_Reg += OFFSET_PGRAPH_FIFO_REG/4;
//    *PGRAPH_FIFO_Reg &= 0xfffffffe;

    //**************************************************************************
    // Set the dither state
    //**************************************************************************

    PGRAPH_DEBUG_3_Reg = (volatile DWORD *)ppdev->PGRAPHRegs;
    PGRAPH_DEBUG_3_Reg += OFFSET_PGRAPH_DEBUG_3_REG/4;

    //**************************************************************************
    // WARNING: Do NOT 'or' values directly with HARDWARE registers because
    //          the compiler will optimize and do 'Byte' or's, which will
    //          cause the hardware to hang !!  Instead, read the hardware register
    //          first (read a dword), then modify the value, then output
    //          as a dword.
    //**************************************************************************

    value = *PGRAPH_DEBUG_3_Reg;                // Get current DWORD value

    if (enableDither)
        value |= (DWORD)0x00008000;             // Set Bit 15 to enable dithering
    else
        value &= (DWORD)0xffff7fff;             // Clear bit 15 to disable dithering

    *PGRAPH_DEBUG_3_Reg = value;                // Output new value


    //**************************************************************************
    // Save the updated dither state
    //**************************************************************************

    ppdev->CurrentDitherState = enableDither;

//    //**************************************************************************
//    // Re-enable the FIFO
//    //**************************************************************************
//
//    *PGRAPH_FIFO_Reg |= 0x00000001;


    }


//******************************************************************************
//
//  Function:   NV3_SynchronizeHW
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


void NV3_SynchronizeHW(PDEV*   ppdev)

    {

    //**************************************************************************
    // Loop until the engine is not busy
    //**************************************************************************

    while (NV3_GraphicsEngineBusy(ppdev))
        ;
    }



//******************************************************************************
//
//  Function:   NV3_SwapDisplaySurface
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


VOID NV3_SwapDisplaySurface(PDEV *ppdev, ULONG NewDisplayOffset, ULONG NewRenderOffset, BOOL waitforVSync)

    {
    if (waitforVSync)
        NV3_WaitWhileDisplayActive(ppdev);

    //**************************************************************************
    // Swap render(destination) and display base
    //**************************************************************************

    NV3_SetDisplayBase(ppdev,NewDisplayOffset);
    NV3_SetDestBase(ppdev,NewRenderOffset,ppdev->lDelta);


    }


//******************************************************************************
//
//  Function:   NV3_VerifyFunctions
//
//  Routine Description:
//
//      This functions tests the functionality of the above functions
//      It assumes the display driver is currently running at 640x480 16bpp
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************


VOID NV3_VerifyFunctions(PDEV*   ppdev)

    {
    RECTL SrcRect,DstRect,ClipRect;

    //**************************************************************************
    // !! SET DISPLAY MODE to 640x480 at 16bpp to view this test properly !!!
    //**************************************************************************

    //**************************************************************************
    // Source = 0
    // Destination = at 3Mb
    // pitch = 640 at 16bpp
    //**************************************************************************

    NV3_SetDisplayBase(ppdev,0x300000);
    NV3_SetSourceBase(ppdev,0,640*2 );
    NV3_SetDestBase(ppdev,0x300000,640*2);

    ClipRect.left=0;                            // Restore clip rect to full screen
    ClipRect.top=0;
    ClipRect.right=ppdev->cxMemory;
    ClipRect.bottom=ppdev->cyMemory;
    NV3_SetClipRect(ppdev,&ClipRect);

    //**************************************************************************
    // Verify clear surface works (Currently only 15 bit, 16th bit is don't care)
    //**************************************************************************

    NV3_ClearSurface(ppdev,0,0,640,480,0xffff);
    NV3_ClearSurface(ppdev,0,0,320,240,0x7c00);     // red
    NV3_ClearSurface(ppdev,0,240,320,480,0x03e0);   // green
    NV3_ClearSurface(ppdev,320,0,640,240,0x001f);   // blue
    NV3_ClearSurface(ppdev,320,240,640,480,0x0000); // black

    //**************************************************************************
    // Specify blit from buffer 0 (top left quadrant) to buffer 1 (bottom right)
    //**************************************************************************

    SrcRect.left=0;
    SrcRect.top=0;
    SrcRect.right=320;
    SrcRect.bottom=240;

    DstRect.left=320;
    DstRect.top=240;
    DstRect.right=640;
    DstRect.bottom=480;

    //**************************************************************************
    // Verify source and destination offsets were set correctly
    // Blit top left quadrant of windows desktop to bottom right of buffer 1
    //**************************************************************************

    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Verifies a different source offset for blit (same pitch)
    // Blit bottom left of windows desktop to bottom right of buffer 1
    //**************************************************************************

    NV3_SetSourceBase(ppdev,640*2*240,640*2);
    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

//    //**************************************************************************
//    // Verify FIFO versions of SetSourceBase and SetDestBase
//    // These are ONLY available if OpenGL is running
//    //**************************************************************************
//
//    NV3_OGLSetSourceBase(ppdev,0, 640*2);
//    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//
//    NV3_OGLSetSourceBase(ppdev,20, 640*2);
//    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//
//    NV3_OGLSetSourceBase(ppdev,20, 640);
//    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//
//    NV3_OGLSetSourceBase(ppdev,0, 640*2);
//    NV3_OGLSetDestBase(ppdev,0x300000, 640*2);
//    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//
//    NV3_OGLSetDestBase(ppdev,0x300010, 640*2);
//    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//
//    NV3_OGLSetDestBase(ppdev,0x300000, 640);
//    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//
//    NV3_OGLSetDestBase(ppdev,0x300000, 640*2);

    //**************************************************************************
    // Restore Source Offset to point to windows desktop
    // Blit top left of windows desktop to bottom right of buffer 1
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640*2);
    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Check that we can change the source pitch (Specify it twice as long
    // so that the destination blit should be 'scrunched')
    // Blit top left of windows desktop to bottom right of buffer 1
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640*2 * 2);
    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Check that we can change the source pitch (Specify it half as long
    // so that the destination blit should show an 'interleaved' image)
    // Blit top left of windows desktop to bottom right of buffer 1
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640);
    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Check that we can change the destination pitch (Specify it half as long
    // so that the destination blit should be 'scrunched')
    // Blit top left of windows desktop to bottom right of buffer 1
    // NOTE: Since we decreased the pitch by half, the blit will occur higher
    //       up in the visible screen, instead of where it would appear
    //       if the pitch had not changed.
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640*2);
    NV3_SetDestBase(ppdev,0x300000, 640);
    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Check that we can blit from a source to a destination pitch and back
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640*2);
    NV3_SetDestBase(ppdev,0x300000, 640);

    DstRect.left=0;
    DstRect.top=0;
    DstRect.right=320;
    DstRect.bottom=240;

    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);

    NV3_SetSourceBase(ppdev,0x300000, 640);
    NV3_SetDestBase(ppdev,0x300000, 640*2);

    DstRect.left=320;
    DstRect.top=240;
    DstRect.right=640;
    DstRect.bottom=480;

    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);

    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Check an unusual pitch value . (Specify default_pitch + 16 bytes)
    // The destination blit should be 'skewed'
    // Blit top left of windows desktop to bottom right of buffer 1
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640*2);
    NV3_SetDestBase(ppdev,0x300000, 640*2 + 16);
    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Check that clipping rectangle is working
    // Blit clipped portion of desktop to buffer 1
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640*2);
    NV3_SetDestBase(ppdev,0x300000, 640*2);

    ClipRect.left=100;
    ClipRect.top=50;
    ClipRect.right=640-100;
    ClipRect.bottom=480-50;
    NV3_SetClipRect(ppdev,&ClipRect);

    SrcRect.left=0;
    SrcRect.top=0;
    SrcRect.right=640;
    SrcRect.bottom=480;

    DstRect.left=0;
    DstRect.top=0;
    DstRect.right=640;
    DstRect.bottom=480;

    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    //**************************************************************************
    // Check that clipping rectangle is affected by the destination offset and pitch
    // Blit top left of windows desktop to bottom right of buffer 1
    //**************************************************************************

    NV3_SetSourceBase(ppdev,0, 640*2);
    NV3_SetDestBase(ppdev,0x300000, 640*2);

    ClipRect.left=320;                          // Blit entire quadrant
    ClipRect.top=240;
    ClipRect.right=640;
    ClipRect.bottom=480;
    NV3_SetClipRect(ppdev,&ClipRect);

    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    ClipRect.left=320;                          // Only top left of quadrant should appear
    ClipRect.top=240;
    ClipRect.right=320 + 160;
    ClipRect.bottom=240 + 120;
    NV3_SetClipRect(ppdev,&ClipRect);

    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);

    NV3_SetDestBase(ppdev,0x300000, 640);       // Change the destination pitch

    ClipRect.left=320;
    ClipRect.top=240;
    ClipRect.right=320+160;
    ClipRect.bottom=240+120;
    NV3_SetClipRect(ppdev,&ClipRect);


    NV3_BlitSurface(ppdev,&SrcRect,&DstRect);
    NV3_ClearSurface(ppdev,0,0,640,480,0x0);


//    //**************************************************************************
//    // Verify Z Buffer functions (using the D3D Triangle)
//    // (Z buffer should appear halfway down the screen (y=240)
//    //**************************************************************************
//
//    NV3_SetSourceBase(ppdev,0, 640*2);
//    NV3_SetDestBase(ppdev,0x300000, 640*2);
//    NV3_SetZBufferBase(ppdev,0x300000 + 640*2 *240, 640*2);
//
//    ClipRect.left=50;                           // Test specific clipping rectangle
//    ClipRect.top=50;
//    ClipRect.right=200;
//    ClipRect.bottom=200;
//    NV3_SetClipRect(ppdev,&ClipRect);
//
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    ClipRect.left=0;                            // Restore clip rect to full screen
//    ClipRect.top=0;
//    ClipRect.right=640;
//    ClipRect.bottom=480;
//    NV3_SetClipRect(ppdev,&ClipRect);
//
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    //**************************************************************************
//    // Verify dithering
//    //**************************************************************************
//
//    NV3_SetDitherMode(ppdev,FALSE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    NV3_SetDitherMode(ppdev,TRUE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    NV3_SetDitherMode(ppdev,FALSE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    NV3_SetDitherMode(ppdev,TRUE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    //**************************************************************************
//    // Verify Clear Z function
//    //**************************************************************************
//
//    NV3_ClearZ(ppdev,0,0,320,10, 0);            // Just clear a portion of Z buffer
//    NV3_ClearZ(ppdev,0,0,320,10,0xffff );       // Just clear a portion of Z buffer
//    NV3_ClearZ(ppdev,320,0,320,10,0x0000 );     // Just clear a portion of Z buffer
//    NV3_ClearZ(ppdev,320,0,320,10,0xffff );     // Just clear a portion of Z buffer
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    //**************************************************************************
//    // Verify dithering
//    //**************************************************************************
//
//    NV3_SetDitherMode(ppdev,FALSE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    NV3_SetDitherMode(ppdev,TRUE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    //**************************************************************************
//    // Verify Swap buffer function
//    // 1st setup display to be front buffer at offset = 0, and
//    // destination to be 'back' buffer at offset = 0x100000
//    //**************************************************************************
//
//    NV3_SwapDisplaySurface(ppdev,0x00000000,0x00300000, FALSE);
//    NV3_SwapDisplaySurface(ppdev,0x00300000,0x00000000, TRUE);
//
//    //**************************************************************************
//    // Verify Synchronize function
//    //**************************************************************************
//
//    NV3_SynchronizeHW(ppdev);
//
//    //**************************************************************************
//    // Verify dithering
//    //**************************************************************************
//
//    NV3_SetDestBase(ppdev,0x300000, 640*2);
//
//    NV3_SetDitherMode(ppdev,FALSE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    NV3_SetDitherMode(ppdev,TRUE);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    //**************************************************************************
//    // Verify D3D triangle Zeta values are stored according to the specified destination
//    //**************************************************************************
//
//    ClipRect.left=0;                            // Restore clip rect to full screen
//    ClipRect.top=0;
//    ClipRect.right=ppdev->cxMemory;
//    ClipRect.bottom=ppdev->cyMemory;
//    NV3_SetClipRect(ppdev,&ClipRect);
//
//    NV3_SetDisplayBase(ppdev,0x300000);
//    NV3_SetSourceBase(ppdev,0,640*2 );
//    NV3_SetDestBase(ppdev,0x300000,640*2);
//    NV3_SetZBufferBase(ppdev,0x300000 + 640*2 *240, 640*2);
//
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
//    NV3_ClearSurface(ppdev,0,0,640,480,0x0);
//    NV3_ClearZ(ppdev,0,0,640,480,0x0000 );      // Clear entire Z Buffer
//    NV3_SetZBufferBase(ppdev,0x300000 + 640*2 *240, 640);
//    NV3_VerifyTriangleWithZBuffer(ppdev);       // Draw a solid D3D triangle
//
    //**************************************************************************
    // Restore 640x480 16bpp display screen
    //**************************************************************************

    NV3_SetDisplayBase(ppdev,0x0);
    NV3_SetSourceBase(ppdev,0,640*2);
    NV3_SetDestBase(ppdev,0,640*2);

    ClipRect.left=0;                            // Restore clip rect to full screen
    ClipRect.top=0;
    ClipRect.right=ppdev->cxMemory;
    ClipRect.bottom=ppdev->cyMemory;
    NV3_SetClipRect(ppdev,&ClipRect);


    }





//******************************************************************************
//
//  Function:   NV3_RestoreState
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


VOID NV3_RestoreState(PDEV*   ppdev, ULONG ZBufferOffset)

    {
    RECTL ClipRect;

    //**************************************************************************
    // Restore display , source, destination, and Z Buffer bases
    //**************************************************************************

    NV3_SetDisplayBase(ppdev,0);
    NV3_SetSourceBase(ppdev,0,640*2);
    NV3_SetDestBase(ppdev,0,640*2);
    NV3_SetZBufferBase(ppdev, ZBufferOffset, 640*2);

    ClipRect.left=0;                            // Restore clip rect to full screen
    ClipRect.top=0;
    ClipRect.right=ppdev->cxMemory;
    ClipRect.bottom=ppdev->cyMemory;
    NV3_SetClipRect(ppdev,&ClipRect);

    }




//******************************************************************************
//
//  Function:   NV3_VerifyTriangleWithZBuffer
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


VOID NV3_VerifyTriangleWithZBuffer(PDEV *ppdev, ULONG TextureOffset, ULONG D3DSubChannel)

    {
    ULONG TextureFormat;
    ULONG ControlOut;
    ULONG red,green,blue,alpha;
    USHORT FreeCount = ppdev->NVFreeCount;
    Nv3ChannelPio *nv = (Nv3ChannelPio *) ppdev->pjMmBase;

    //**************************************************************************
    // Specify texture offset
    //**************************************************************************

    while (FreeCount < 1*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 1*4;


    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.TextureOffset = TextureOffset;

    //**************************************************************************
    // Texture format
    //      Color Mask                              Specify 0 for now
    //      Color Key Enable/Disable                Specify DISABLED for now
    //      Actual color format                     Specify X1R5G5B5 for now
    //      Min size of mip map levels (bits   .    Select 4x4 for now
    //      Max size of mip map levels (bits   .    Select 4x4 for now
    //**************************************************************************

    TextureFormat = NV_D3D0_TEXTURE_FORMAT_COLOR_KEY_DISABLED    |
                    NV_D3D0_TEXTURE_FORMAT_COLOR_FORMAT_X1R5G5B5 |
                    NV_D3D0_TEXTURE_FORMAT_SIZE_MIN_4X4          |
                    NV_D3D0_TEXTURE_FORMAT_SIZE_MAX_4X4          ;

    while (FreeCount < 1*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 1*4;

    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.TextureFormat = TextureFormat;

    //**************************************************************************
    // Specify FILTER consists of several components OR'ed together:
    //
    //      Filter_Spread_X
    //      Filter_Spread_Y
    //      Filter_MipMap
    //      Filter_Turbo
    //
    //**************************************************************************

    while (FreeCount < 1*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 1*4;

    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.TextureFilter = 0x00000000;

    //**************************************************************************
    // Specify FOG COLOR, in X8R8G8B8 format
    //**************************************************************************

    while (FreeCount < 1*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 1*4;

    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.FogColor = 0x00000000;

    //**************************************************************************
    // Specify CONTROL OUT OPTIONS
    //
    // See NV3 reference for description of this register.
    //
    //  INTERPOLATOR    (filter from discrete image to continuous image)
    //  WRAP_U          (cylindrical, wrap, mirror, clamp)
    //  WRAP_V          (cylindrical, wrap, mirror, clamp)
    //  COLOR_FORMAT    (X8R8G8B8 or A8R8G8B8)
    //  CULLING         (None, Clockwise, Counterclockwise);
    //  ZBUFFER         (SCREEN using controlpoint[].z values, or LINEAR using 1/m
    //  ZETA_COMPARE    (conditional types = LT,EQ,LE,GT,NE,GE,TRUE)
    //  ZETA_WRITE      (NEVER,ALPHA,ZETA,ALPHA_ZETA, ALWAYS)
    //  COLOR_WRITE     (NEVER,ALPHA,ALPHA_ZETA)
    //  ROP             (BLEND_AND or ADD_WITH_SATURATION)
    //
    //**************************************************************************

    //**************************************************************************
    // Send the ControlOut value to hardware
    //**************************************************************************

    ControlOut                  = NV_D3D0_CONTROL0_INTERPOLATOR_ZOH         ;
    ControlOut                 |= NV_D3D0_CONTROL0_WRAP_U_WRAP              ;
    ControlOut                 |= NV_D3D0_CONTROL0_WRAP_V_WRAP              ;
    ControlOut                 |= NV_D3D0_CONTROL0_COLOR_FORMAT_LE_X8R8G8B8 ;
    ControlOut                 |= NV_D3D0_CONTROL0_CULLING_NONE             ;
    ControlOut                 |= NV_D3D0_CONTROL0_ZETA_BUFFER_SCREEN       ;
    ControlOut                 |= NV_D3D0_CONTROL0_ZETA_COMPARE_TRUE        ;
    ControlOut                 |= NV_D3D0_CONTROL0_ZETA_WRITE_ALPHA_ZETA    ;
    ControlOut                 |= NV_D3D0_CONTROL0_COLOR_WRITE_ALPHA_ZETA   ;
    ControlOut                 |= NV_D3D0_CONTROL0_ROP_BLEND_AND            ;

    while (FreeCount < 1*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 1*4;

    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Control0 = ControlOut;

    //**************************************************************************
    // Vertex 1
    //**************************************************************************

    red     = 0xff;
    green   = 0x00;
    blue    = 0x00;
    alpha   = 0xff;

    alpha   <<=24;
    red     <<=16;
    green   <<=8 ;
    blue    <<=0 ;

    while (FreeCount < 8*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 8*4;

    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].specular = 0x00000000;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].color    = (alpha | red | green | blue);
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].sx       = (float)0.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].sy       = (float)0.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].sz       = (float)0.9999;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].rhw      = (float)0.1127;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].tu       = (float)0.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[0].tv       = (float)0.0;


    //**************************************************************************
    // Vertex 2
    //**************************************************************************

    red     = 0x00;
    green   = 0xff;
    blue    = 0x00;
    alpha   = 0xff;

    alpha   <<=24;
    red     <<=16;
    green   <<=8 ;
    blue    <<=0 ;

    while (FreeCount < 8*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 8*4;

    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].specular = 0x00000001;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].color    = (alpha | red | green | blue);
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].sx       = (float)320.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].sy       = (float)0.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].sz       = (float)0.9999;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].rhw      = (float)0.1127;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].tu       = (float)0.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[1].tv       = (float)0.0;


    //**************************************************************************
    // Vertex 3
    //**************************************************************************

    red     = 0x00;
    green   = 0x00;
    blue    = 0xff;
    alpha   = 0xff;

    alpha   <<=24;
    red     <<=16;
    green   <<=8 ;
    blue    <<=0 ;

    while (FreeCount < 8*4)
       FreeCount = nv->subchannel[D3DSubChannel].control.Free;
    FreeCount -= 8*4;

    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].specular = 0x00000102;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].color    = (alpha | red | green | blue);
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].sx       = (float)320.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].sy       = (float)240.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].sz       = (float)0.9999;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].rhw      = (float)0.1127;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].tu       = (float)0.0;
    nv->subchannel[D3DSubChannel].nv3Dx3TexturedTriangle.Tlvertex[2].tv       = (float)0.0;

    ppdev->NVFreeCount = FreeCount;

    }
#endif // NV3
