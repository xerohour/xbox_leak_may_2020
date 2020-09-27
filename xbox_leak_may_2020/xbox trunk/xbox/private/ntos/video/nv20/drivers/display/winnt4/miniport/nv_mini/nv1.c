//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     NV1.C
//
// Abstract:
//
//     This is code specific to NV1
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************


#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"

#include "nv.h"
#include "nv1c_ref.h"
#include "nv1_32.h"
#include "nvsubch.h"
#include "nvalpha.h"

#include "sgs_dac.h"

#include "cmdcnst.h"

//******************************************************************************
//
// Function Declarations
//
//******************************************************************************

VOID SetRop(PHW_DEVICE_EXTENSION,ULONG);
VOID SetPattern(PHW_DEVICE_EXTENSION);
VOID SetColorKey(PHW_DEVICE_EXTENSION,ULONG);
VOID SetClip(PHW_DEVICE_EXTENSION, ULONG, ULONG, ULONG, ULONG);
VOID ImageMonoBlit(PHW_DEVICE_EXTENSION,LONG,LONG,ULONG,ULONG);
VOID MemoryToScreenBlit(PHW_DEVICE_EXTENSION,ULONG,ULONG,ULONG,ULONG);
VOID ScreenToScreenBlit(PHW_DEVICE_EXTENSION,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG);
VOID RenderTriangle(PHW_DEVICE_EXTENSION,ULONG,ULONG,ULONG);
VOID RenderRectangle(PHW_DEVICE_EXTENSION,ULONG,ULONG,ULONG);
VOID UpdateRenderObjectContext(PHW_DEVICE_EXTENSION,ULONG,ULONG,ULONG);
VOID CalculateRenderObjectContext(PHW_DEVICE_EXTENSION,PRENDEROBJECT,ULONG);
VOID SetContextRopSolid(PHW_DEVICE_EXTENSION,ULONG);
VOID SetContextPattern(PHW_DEVICE_EXTENSION,ULONG);
VOID SetContextBlackRectangle(PHW_DEVICE_EXTENSION,ULONG);
VOID SetContextImageSolid(PHW_DEVICE_EXTENSION,ULONG);
VOID SetPatchContext(PHW_DEVICE_EXTENSION,ULONG);
VOID InitColorFormatTable(PHW_DEVICE_EXTENSION);
VOID SetObjectColorFormat(PHW_DEVICE_EXTENSION,PCOMMONOBJECT,ULONG,ULONG);
VOID ClearPatchContext(PHW_DEVICE_EXTENSION,PPATCHCONTEXT);
VOID NV1SetupPatches(PHW_DEVICE_EXTENSION);
VOID SetupRenderObject(PHW_DEVICE_EXTENSION,ULONG,ULONG,ULONG,ULONG,ULONG,ULONG);
VOID ClearOutRenderObjects(PHW_DEVICE_EXTENSION);
BOOLEAN NV1GetNVInfo(PHW_DEVICE_EXTENSION);
VOID NV1SetupGraphicsEngine(PHW_DEVICE_EXTENSION);
VOID NV1SetMode(PHW_DEVICE_EXTENSION,PMODE_ENTRY);
VOID SetObjectCachedCtxRegs(PHW_DEVICE_EXTENSION,ULONG,ULONG,ULONG);
VOID SetObjectHash(PHW_DEVICE_EXTENSION,ULONG,ULONG);
VOID FifoResend(PHW_DEVICE_EXTENSION);
VOID ClearOutGrTable(PHW_DEVICE_EXTENSION);
VOID ClearOutFifoTable(PHW_DEVICE_EXTENSION);
VOID ClearOutNVInfo(PHW_DEVICE_EXTENSION);
VOID ClearOutHashTables(PHW_DEVICE_EXTENSION);
VOID ClearOutContextMemoryAndRegisters(PHW_DEVICE_EXTENSION);
VOID LoadChannelContext(PHW_DEVICE_EXTENSION);
VOID NV1_SetColorLookup(PHW_DEVICE_EXTENSION,PVIDEO_CLUT,ULONG);
VOID NV1_InitPalette(PHW_DEVICE_EXTENSION);
ULONG NV1_MapMemoryRanges(PVOID HwDeviceExtension);
BOOLEAN NV1_IsPresent(PHW_DEVICE_EXTENSION,PVIDEO_ACCESS_RANGE,PULONG);
BOOLEAN NV1Interrupt(PHW_DEVICE_EXTENSION);
VOID dacCalcPClkVClkRatio(PHW_DEVICE_EXTENSION);
VOID dacCalcPLL(PHW_DEVICE_EXTENSION);
VOID NV1GraphicsTest(PHW_DEVICE_EXTENSION);
VOID NV1ClearScreen(PHW_DEVICE_EXTENSION);
VOID NV1WaitUntilFinished(PHW_DEVICE_EXTENSION);
VOID ImageMonoLetterY(PHW_DEVICE_EXTENSION,LONG,LONG,ULONG,ULONG);
VOID NV1SetOpenGLPatch(PHW_DEVICE_EXTENSION);
VOID NV1RestoreDisplayDriverPatch(PHW_DEVICE_EXTENSION);



VOID NV1ObjectSwappingTest(PHW_DEVICE_EXTENSION);
VOID NV1ObjectSwappingTryIt(PHW_DEVICE_EXTENSION);
VOID NV1ObjectSwappingRestore(PHW_DEVICE_EXTENSION);

// Make all other functions pageable???
#if defined(ALLOC_PRAGMA)
#pragma alloc_text(PAGE,NV1SetMode)
#pragma alloc_text(PAGE,NV1SetupGraphicsEngine)
#endif


//******************************************************************************
//
//  Function:   SetRop()
//
//  Routine Description:
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

VOID SetRop(PHW_DEVICE_EXTENSION HwDeviceExtension,ULONG Rop)

    {

    NvChannel *nv;
    volatile ULONG FreeCount;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait for FIFO
    //**************************************************************************

    FreeCount = nv->subchannel[ROP_SOLID_SUBCHANNEL].control.free;

    while (FreeCount < 1*4)
        FreeCount = nv->subchannel[ROP_SOLID_SUBCHANNEL].control.free;
    FreeCount -= 1*4;

    //**************************************************************************
    // Set ROP
    //**************************************************************************

    nv->subchannel[ROP_SOLID_SUBCHANNEL].ropSolid.SetRop = Rop;


    }


//******************************************************************************
//
//  Function:   SetPattern()
//
//  Routine Description:
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

VOID SetPattern(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {

    NvChannel *nv;
    ULONG color0;
    ULONG color1;
    PHWINFO NVInfo;

    //**************************************************************************
    // Get pointer to NVInfo structure, then init colors
    // according to what the current pixel depth is
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            color0 =    NV_ALPHA_1_32 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_32 | 0x00ffffff;         // WHITE
            break;
        case 16:
            color0 =    NV_ALPHA_1_16 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_16 | 0x00007fff;         // WHITE
            break;
        case 8:
            color0 =    NV_ALPHA_1_08 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_08 | 0x000000ff;         // WHITE
            break;
        }

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait for FIFO
    //**************************************************************************

    while (nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].control.free < 5*4);

    //*************************************************************************
    //
    //  nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
    //  nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetMonochromeFormat = NV_MONO_CGA6_M1_P32;
    //
    // Do NOT set ColorFormat and Monochrome format here, because
    // they require a Resource Manager to service them!  Doing so will cause
    // a lockup/interrupt. The remaining methods are hardware methods and go
    // straight to hardware. They will NOT generate an interrupt/lockup.
    //*************************************************************************

    //*************************************************************************
    // Regardless of what the pixel depth is, we should see a jailbar pattern.
    //*************************************************************************

    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetPatternShape = NV_PATTERN_SHAPE_64X1;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetColor0 = color0;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetColor1 = color1;
//    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetPattern.monochrome[0] = 0x50505050;
//    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetPattern.monochrome[1] = 0x12345678;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetPattern.monochrome[0] = 0xffffffff;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].imagePattern.SetPattern.monochrome[1] = 0x50505050;

    }


//******************************************************************************
//
//  Function:   SetColorKey()
//
//  Routine Description:
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

VOID SetColorKey(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG Color)

    {

    NvChannel *nv;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait for FIFO
    //**************************************************************************

    while (nv->subchannel[7].control.free < 1*4);

    //**************************************************************************
    // Set color key value
    //**************************************************************************

    nv->subchannel[7].imageSolid.SetColor = Color; // 0 will disable color keying

    }


//******************************************************************************
//
//  Function:   SetClip()
//
//  Routine Description:
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

VOID SetClip(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG x, ULONG y, ULONG width, ULONG height)

    {
    ULONG i;

    NvChannel *nv;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait for FIFO
    //**************************************************************************

    while (nv->subchannel[CLIP_SUBCHANNEL].control.free < 2*4);

    //**************************************************************************
    // Set the clipping rectangle
    //**************************************************************************

    nv->subchannel[CLIP_SUBCHANNEL].imageBlackRectangle.SetRectangle.y_x = ((y << 16) | x );
    nv->subchannel[CLIP_SUBCHANNEL].imageBlackRectangle.SetRectangle.height_width = ((height<<16) | width);

    }


//******************************************************************************
//
//  Function:   ImageMonoLetterY()
//
//  Routine Description:
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

VOID ImageMonoLetterY(PHW_DEVICE_EXTENSION HwDeviceExtension,
                       LONG x,LONG y, ULONG Color0, ULONG Color1)
    {

    NvChannel *nv;
    ULONG xscan, yscan;
    ULONG pixel;
    ULONG width,height;
    ULONG NumDwords;
    ULONG DwordCount;
    ULONG SizeInWidth;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // IMAGE MONO FROM CPU classes requires an UNpacked format !!!!
    //
    //          |<------------->| SizeIn = 8    (Data Width sent)
    //
    //          |<------->|  Size = 5           (Clipped size)
    //                    |
    //   ___     -------|-------
    //    ^     | | | | | | | | |
    //    |     | | | | | | | | |  1 dword = 0x00000000
    //    |     | | | | | | | | |
    //    |     | | | | | | | | |_________
    //    |     |x| | |x| | | | |
    //    |     |x| | |x| | | | |
    //Height=12 |x| | |x| | | | |  1 dword = 0x90909090
    //    |     |x| | |x| | | | |_________
    //    |     |x| | |x| | | | |
    //    |     | |x|x|x| | | | |
    //    |     | | | |x| | | | |  1 dword = 0xE0107090
    //    |     |x|x|x| | | | | |
    //   ---     ---------------
    //
    //**************************************************************************

    width = 5;
    height= 12;
    SizeInWidth = 8;

    //**************************************************************************
    // We'll set the alpha bits to test color 0 = transparent
    //                                  color 1 = visible
    //**************************************************************************

    while (nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].control.free < 5*4);
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Color0 =  Color0 ;
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Color1 =  Color1 ;
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Point =  ( (y <<16) | (x & 0xffff) );
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Size = ( (height <<16) | width );
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.SizeIn = ( (height <<16) | SizeInWidth );

    while (nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].control.free < 12);
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Monochrome[0] = 0x0;
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Monochrome[1] = 0x090909090;
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Monochrome[2] = 0x0E0107090;


    }



//******************************************************************************
//
//  Function:   ImageMonoBlit()
//
//  Routine Description:
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

VOID ImageMonoBlit(PHW_DEVICE_EXTENSION HwDeviceExtension,
                       LONG x,LONG y, ULONG Color0, ULONG Color1)
    {

    NvChannel *nv;
    ULONG xscan, yscan;
    ULONG pixel;
    ULONG width,height;
    ULONG NumDwords;
    ULONG DwordCount;
    ULONG SizeInWidth;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // IMAGE MONO FROM CPU classes requires an UNpacked format !!!!
    //**************************************************************************

    width = 50;
    height= 20;
    SizeInWidth = (width + 31) & 0xffe0;    // Specify pixels as multiple of 32 since
                                            // we always output DWORDS at a time

    NumDwords = (width + 31)/32;            // Dwords at a time (32 pixels per dword)

    //**************************************************************************
    // We'll set the alpha bits to test color 0 = transparent
    //                                  color 1 = visible
    //**************************************************************************

    while (nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].control.free < 5*4);
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Color0 =  Color0 ;
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Color1 =  Color1 ;
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Point =  ( (y <<16) | (x & 0xffff) );
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Size = ( (height <<16) | width );
    nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.SizeIn = ( (height <<16) | SizeInWidth );

    //**************************************************************************
    // The following method is EXTREMELY inefficient, but simple and works.
    // To get much better performance, do NOT constantly read the
    // freecount inside the loop.  Instead, CACHE it in local memory.
    // Also, don't constantly write to the same monochrome[0] location.
    // Use increasing locations to allow for PCI burst mode, and better performance.
    // See Text accleration code in the display driver for better examples.
    //**************************************************************************

    for (yscan=0; yscan < height; yscan ++)
        for (DwordCount = 0; DwordCount < NumDwords; DwordCount++)
            {
            while (nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].control.free < 4);
            nv->subchannel[IMAGE_MONO_FROM_CPU_SUBCHANNEL].imageMonochromeFromCpu.Monochrome[0] = 0x0f0f0f0f;
            }


    }


//******************************************************************************
//
//  Function:   MemoryToScreenBlit()
//
//  Routine Description:
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

VOID MemoryToScreenBlit(PHW_DEVICE_EXTENSION HwDeviceExtension,
                        ULONG width,ULONG height, ULONG x,ULONG y)
    {

    NvChannel *nv;
    ULONG xscan, yscan;
    ULONG pixeldata;
    PHWINFO NVInfo;

    //**************************************************************************
    // Get pointer to NVInfo structure, then init colors
    // according to what the current pixel depth is
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            pixeldata = NV_ALPHA_1_32 | 0x00ff0000;         // RED
            break;
        case 16:
            pixeldata = (ULONG) ((NV_ALPHA_1_16 | 0x7c00)<<16) |
                         (NV_ALPHA_1_16 | 0x7c00);          // RED
            break;
        case 8:
            pixeldata =   (ULONG) ( 0xff<<24) |
                                  ( 0xff<<16) |
                                  ( 0xff<<8)  |
                                  ( 0xff)     ;                     // WHITE
            break;
        }

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait for FIFO
    // IMAGE FROM CPU classes require a PACKED format !!!!
    //**************************************************************************

    while (nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.free < 3*4);
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].imageFromCpu.Point =  ( (y <<16) | x );
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].imageFromCpu.Size = ( (height <<16) | width );
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].imageFromCpu.SizeIn = ( (height <<16) | width );

    for (yscan=0;yscan < height; yscan++)
        for (xscan=0; xscan < width; xscan++)
            {
            while (nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.free < 4);
            nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].imageFromCpu.Color[0] =  pixeldata;
            }


    }

//******************************************************************************
//
//  Function:   ScreenToScreenBlit()
//
//  Routine Description:
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

VOID ScreenToScreenBlit(PHW_DEVICE_EXTENSION HwDeviceExtension,ULONG width,ULONG height,
                        ULONG xsrc,ULONG ysrc, ULONG xdst,ULONG ydst)

    {

    NvChannel *nv;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait for FIFO
    //**************************************************************************

    while (nv->subchannel[BLIT_SUBCHANNEL].control.free < 3*4);


    nv->subchannel[BLIT_SUBCHANNEL].imageBlit.ControlPointIn =  ( (ysrc <<16) | xsrc );
    nv->subchannel[BLIT_SUBCHANNEL].imageBlit.ControlPointOut = ( (ydst <<16) | xdst );
    nv->subchannel[BLIT_SUBCHANNEL].imageBlit.Size = ( (height <<16) | width );


    }


//******************************************************************************
//
//  Function:   RenderTriangle()
//
//  Routine Description:
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

VOID RenderTriangle(PHW_DEVICE_EXTENSION HwDeviceExtension,ULONG x,ULONG y,ULONG Color)

    {
    ULONG i;

    NvChannel *nv;

    //**************************************************************************
    // Since we've only got 8 subchannels, we don't have enough room for the
    // TRIANGLE object.  If we need it use it later (i.e. Direct 3D/OpenGL), we'll need
    // to manually swap out one of the current objects and load the TRIANGLE object
    //**************************************************************************

//    //**************************************************************************
//    // Get pointer to NV channel area
//    //**************************************************************************
//
//    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);
//
//    //**************************************************************************
//    // Wait for FIFO
//    //**************************************************************************
//
//    while (nv->subchannel[TRIANGLE_SUBCHANNEL].control.free < 4*4);
//
//    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidTriangle.Color = 0x00000000 | Color;
//    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y0_x0 = (((y+10) << 16) | (x-10) );
//    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y1_x1 = (((y+10) << 16) | (x+10) );
//    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y2_x2 = (((y-10) << 16) | x);

    }


//******************************************************************************
//
//  Function:   RenderRectangle()
//
//  Routine Description:
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

VOID RenderRectangle(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG x,ULONG y,ULONG Color)

    {
    ULONG i;

    NvChannel *nv;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait for FIFO
    //**************************************************************************

    while (nv->subchannel[RECTANGLE_SUBCHANNEL].control.free < 3*4);

    nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Color = Color;
    nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ((y << 16) | x );
    nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = 0x00400040;

    }

//******************************************************************************
//
//  Function:   UpdateRenderObjectContext()
//
//  Routine Description:
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

VOID UpdateRenderObjectContext(PHW_DEVICE_EXTENSION HwDeviceExtension,
                      ULONG ObjectName, ULONG SubChannel, ULONG Context)


    {
    ULONG caches;
    ULONG cache1push0;
    ULONG cache1pull0;

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Cache1 access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // Add object to hash table
    //**************************************************************************

    SetObjectHash(HwDeviceExtension, ObjectName, Context);

    //**************************************************************************
    // Update the cached context register
    //**************************************************************************

    SetObjectCachedCtxRegs(HwDeviceExtension, SubChannel, ObjectName, Context);

    //**************************************************************************
    // Restore Cache1 state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);


    }

//******************************************************************************
//
//  Function:   CalculateRenderObjectContext()
//
//  Routine Description:
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

VOID CalculateRenderObjectContext(PHW_DEVICE_EXTENSION HwDeviceExtension,
                                   PRENDEROBJECT PObject,ULONG RenderObjectClass)

    {
    ULONG Context;
    ULONG Device;
    ULONG Buffer;

    switch (RenderObjectClass)
        {
        case NV_RENDER_SOLID_POINT:
            Device = DEVICE_BASE(NV_UPOINT);
            break;
        case NV_RENDER_SOLID_LINE:
            Device = DEVICE_BASE(NV_ULINE);
            break;
        case NV_RENDER_SOLID_LIN:
            Device = DEVICE_BASE(NV_ULIN);
            break;
        case NV_RENDER_SOLID_TRIANGLE:
            Device = DEVICE_BASE(NV_UTRI);
            break;
        case NV_RENDER_SOLID_RECTANGLE:
            Device = DEVICE_BASE(NV_URECT);
            break;
        case NV_RENDER_TEXTURE_BILINEAR:
            Device = DEVICE_BASE(NV_UBTM);
            break;
        case NV_RENDER_TEXTURE_QUADRATIC:
            Device = DEVICE_BASE(NV_UQTM);
            break;
        case NV_IMAGE_FROM_CPU:
            Device = DEVICE_BASE(NV_UIMAGE);
            break;
        case NV_IMAGE_MONOCHROME_FROM_CPU:
            Device = DEVICE_BASE(NV_UBITMAP);
            break;
//        case NV_IMAGE_FROM_MEM:
//            Device = DEVICE_BASE(NV_UFROMEM);
//            break;
//        case NV_IMAGE_TO_MEM:
//            Device = DEVICE_BASE(NV_UTOMEM);
//          break;
        case NV_IMAGE_BLIT:
            Device = DEVICE_BASE(NV_UBLIT);
            break;
        }

    //**************************************************************************
    // Generate the CONTEXT for this device...
    // It consists of 2 parts...the physical DEVICE and phyiscal INSTANCE
    //
    //    ----------------------------------------------------
    //   |       DEVICE           |         Instance          |
    //    ----------------------------------------------------
    //
    //
    //  Label   Bits    Name        Description
    //  -----   ----    ----        -----------
    //  a       (0-4)   Patch Cfg   Encoded configuration value for data path units
    //                                      Value   Meaning
    //                                      -----   -------
    //                                      0 thru 1f   Many different cfgs
    //
    //  b       (5)     ChromaKey   Enables Chroma Key or Beta Maximum
    //  c       (6)     PlaneMask   Enables Plane Mask
    //  d       (7)     UserClip    Enables User Clip
    //  e       (8)     Notify      Notify valid or invalid for object
    //  f       (9-12)  Color Buffer Specifies color format of source pixel data
    //                                      Value   Meaning (Single Buffer 0)
    //                                      -----   -------
    //                                      0x0     R5G5B5
    //                                      0x1     R8G8B8
    //                                      0x2     R10G10B10
    //                                      0x3     Y8
    //                                      0x4     Y16
    //
    //  g       (13)    Alpha       Enables Alpha
    //  h       (14)    Mono        Specifies little endian or CGA6 format
    //  i       (15)    Patch ID    Resource Manager is needed to do heavy weigh context switch
    //
    //**************************************************************************

    Buffer = 0;         // Is this the correct value ???
                        // 1 for R8G8B8 ???
    Context = Device                                      |
              PObject->RCPatchContext.ImageConfig         |
              PObject->CColorFormat                       |
              PObject->CMonoFormat +
              DRF_NUM(_PGRAPH,_CTX_SWITCH,_COLOR_BUFFER, Buffer);

    //**************************************************************************
    // .. and return it.
    //**************************************************************************

    PObject->CContext = Context;
    }


//******************************************************************************
//
//  Function:   SetContextRopSolid()
//
//  Routine Description:
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

VOID SetContextRopSolid(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG PatchID)

    {
    ULONG i;
    ULONG caches;
    ULONG cache1push0;
    ULONG cache1pull0;
    PCOMMONOBJECT StaticObjectPtr;
    PPATCHCONTEXT PatchContextPtr;
    ULONG Context;
    PHWINFO NVInfo;

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Fifo access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // If a true resource manager were implemented, then there would probably
    // be several different patches setup (conceptually).  But since we don't
    // have a resource manager, we're programming at the hardware/physical level.
    // So basically, all rendering objects will share the ROP SOLID , PATTERN,
    // CLIP, and IMAGE SOLID devices.  Another way to look at it at this level,
    // is that we only implement ONE patch and all the rendering objects
    // will share it.
    //
    //
    //          ROP SOLID   PATTERN     BLACK RECTANGLE     IMAGE SOLID
    //              |           |           |                   |
    //  RENDER--     ---    ----            |                   |
    //  OBJECT  |       |  |                |                   |
    //          |    -----------         -------------       ----------
    //           ---|           |       |             |     |          |
    //  RENDER _____|   IMAGE   |_______|    IMAGE    |_____|  IMAGE   |__IMAGE TO__VIDEO
    //  OBJECT      |  ROP AND  |       |   STENCIL   |     | COLORKEY |   VIDEO    SINK
    //           ---|           |       |             |     |          |
    //  RENDER _|  - -----------         -------------       ----------
    //  OBJECT    |
    //            |
    //  RENDER ---
    //  OBJECT
    //
    //  etc...
    //
    //  Touching any of the ROP SOLID,PATTERN, CLIP, or IMAGE SOLID devices
    //  will affect ALL rendering objects.  In essence, we're programming
    //  this card without the assistance of context switching from a resource
    //  manager.  We have to do it ourselves.
    //
    // See function MthdStaticImage() in the Win 95 Resource manager
    // for more information on how the following contexts are initialized
    //
    //**************************************************************************

    //**************************************************************************
    // Update the context for the STATIC OBJECT (IMAGE_ROP_SOLID).
    // Enable COLORKEY'ing
    //**************************************************************************

    StaticObjectPtr = (PCOMMONOBJECT)&(HwDeviceExtension->CommonObject[IMAGE_ROP_ID]);

    //**************************************************************************
    // Set the color format according to the current frame buffer depth
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    if (NVInfo->Framebuffer.Depth == 8)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A8Y8;
    else if (NVInfo->Framebuffer.Depth == 16)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A1R5G5B5;
    else
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_A8R8G8B8;

    StaticObjectPtr->MonoFormat = 0;

    StaticObjectPtr->Context = DEVICE_BASE(NV_UROP) | StaticObjectPtr->ColorFormat;

    //**************************************************************************
    // Update the context for the associated RENDER OBJECT.
    // Enable COLORKEY'ing
    //**************************************************************************

    PatchContextPtr = (PPATCHCONTEXT)&(HwDeviceExtension->RenderObject[PatchID].RCPatchContext);
    ;   // No updates necessary

    //**************************************************************************
    // Specify the actual ROP value.
    //**************************************************************************

    PatchContextPtr->Rop.Rop3 = 0xcc;   // Source Copy for now

    //**************************************************************************
    // Save the name of this STATIC object
    // Also mark which object is currently using the specified subchannel
    //**************************************************************************

    StaticObjectPtr->Base.Name = MY_ROP_SOLID;
    HwDeviceExtension->FifoTable.ObjectStack[ROP_SOLID_SUBCHANNEL] = MY_ROP_SOLID;

    //**************************************************************************
    // Add STATIC object to hash table and update the cached context register
    //**************************************************************************

    Context = StaticObjectPtr->Context;
    SetObjectHash(HwDeviceExtension, MY_ROP_SOLID, Context);
    SetObjectCachedCtxRegs(HwDeviceExtension, ROP_SOLID_SUBCHANNEL, MY_ROP_SOLID, Context);

    //**************************************************************************
    // Restore saved FIFO state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);

    }


//******************************************************************************
//
//  Function:   SetContextPattern()
//
//  Routine Description:
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

VOID SetContextPattern(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG PatchID)

    {
    ULONG i;
    ULONG caches;
    ULONG cache1push0;
    ULONG cache1pull0;
    PCOMMONOBJECT StaticObjectPtr;
    PPATCHCONTEXT PatchContextPtr;
    ULONG Context;
    PHWINFO NVInfo;

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Fifo access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // Get pointer to NVInfo structure
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    //**************************************************************************
    // If a true resource manager were implemented, then there would probably
    // be several different patches setup (conceptually).  But since we don't
    // have a resource manager, we're programming at the hardware/physical level.
    // So basically, all rendering objects will share the ROP SOLID , PATTERN,
    // CLIP, and IMAGE SOLID devices.  Another way to look at it at this level,
    // is that we only implement ONE patch and all the rendering objects
    // will share it.
    //
    //
    //          ROP SOLID   PATTERN     BLACK RECTANGLE     IMAGE SOLID
    //              |           |           |                   |
    //  RENDER--     ---    ----            |                   |
    //  OBJECT  |       |  |                |                   |
    //          |    -----------         -------------       ----------
    //           ---|           |       |             |     |          |
    //  RENDER _____|   IMAGE   |_______|    IMAGE    |_____|  IMAGE   |__IMAGE TO__VIDEO
    //  OBJECT      |  ROP AND  |       |   STENCIL   |     | COLORKEY |   VIDEO    SINK
    //           ---|           |       |             |     |          |
    //  RENDER _|  - -----------         -------------       ----------
    //  OBJECT    |
    //            |
    //  RENDER ---
    //  OBJECT
    //
    //  etc...
    //
    //  Touching any of the ROP SOLID,PATTERN, CLIP, or IMAGE SOLID devices
    //  will affect ALL rendering objects.  In essence, we're programming
    //  this card without the assistance of context switching from a resource
    //  manager.  We have to do it ourselves.
    //
    // See function MthdStaticImage() in the Win 95 Resource manager
    // for more information on how the following contexts are initialized
    //
    //**************************************************************************

    //**************************************************************************
    // Update the context for the STATIC OBJECT (IMAGE_PATTERN).
    //**************************************************************************

    StaticObjectPtr = (PCOMMONOBJECT)&(HwDeviceExtension->CommonObject[IMAGE_PATTERN_ID]);

    //**************************************************************************
    // Set the color format according to the current frame buffer depth
    //**************************************************************************

//    if (NVInfo->Framebuffer.Depth == 8)
//        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A8Y8;
//    else if (NVInfo->Framebuffer.Depth == 16)
//        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A1R5G5B5;
//    else
//        StaticObjectPtr->ColorFormat = COLOR_FORMAT_A8R8G8B8;

    if (NVInfo->Framebuffer.Depth == 8)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X24Y8;
    else if (NVInfo->Framebuffer.Depth == 16)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X17R5G5B5;
    else
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X8R8G8B8;

    StaticObjectPtr->MonoFormat = MONO_FORMAT_CGA6;
    StaticObjectPtr->Context = DEVICE_BASE(NV_UPATT)                    |
                              StaticObjectPtr->ColorFormat              |
                              StaticObjectPtr->MonoFormat;

    //**************************************************************************
    // Update the context for the associated RENDER OBJECT.
    // Here we just specify the patch configuration.
    //
    // - PatchConfig - See the Patch Configuration table (7-1) in the
    //                 graphics reference to determine how to set this value.
    //                 It is determined by where src, pattern, and destination
    //                 are hooked up to the ROP_AND object.  For the DISPLAY
    //                 driver, we will be using the following conceptual hookup patch.
    //
    //                                                     ---
    //                  IMAGE_TO_VIDEO (DST)---> input 0  | R |
    //                  RENDER OBJECT (SRC) ---> input 1  | O |
    //                  PATTERN OBJECT (PAT)---> input 2  | P |
    //                                                     ---
    //                  This corresponds to patch configuration 0x10
    //                  for the CTX_SWITCH register bits 0-4.
    //
    //**************************************************************************

    PatchContextPtr = (PPATCHCONTEXT)&(HwDeviceExtension->RenderObject[PatchID].RCPatchContext);
    PatchContextPtr->ImageConfig &= 0xffffffe0;
    PatchContextPtr->ImageConfig |= DRF_NUM(_PGRAPH,_CTX_SWITCH, _PATCH_CONFIG, 0x10);

    //**************************************************************************
    // Specify the actual PATTERN values.  These will actually get written
    // to hardware later.
    //**************************************************************************

    PatchContextPtr->RopPat.PattColor0      = 0xff00ff00;
    PatchContextPtr->RopPat.PattColor0Alpha = 0xff00ff00;
    PatchContextPtr->RopPat.PattColor1      = 0xffffff00;
    PatchContextPtr->RopPat.Pattern0        = 0x50505050;
    PatchContextPtr->RopPat.Pattern1        = 0x12345678;
    PatchContextPtr->RopPat.PatternShape = NV_PGRAPH_PATTERN_SHAPE_VALUE_64X1;

    //**************************************************************************
    // Save the name of this STATIC object
    // Also mark which object is currently using the specified subchannel
    //**************************************************************************

    StaticObjectPtr->Base.Name = MY_IMAGE_PATTERN;
    HwDeviceExtension->FifoTable.ObjectStack[IMAGE_PATTERN_SUBCHANNEL] = MY_IMAGE_PATTERN;

    //**************************************************************************
    // Add STATIC object to hash table and update the cached context register
    //**************************************************************************

    Context = StaticObjectPtr->Context;
    SetObjectHash(HwDeviceExtension, MY_IMAGE_PATTERN, Context);
    SetObjectCachedCtxRegs(HwDeviceExtension, IMAGE_PATTERN_SUBCHANNEL, MY_IMAGE_PATTERN, Context);

    //**************************************************************************
    // Restore saved FIFO state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);

    }


//******************************************************************************
//
//  Function:   SetContextBlackRectangle()
//
//  Routine Description:
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

VOID SetContextBlackRectangle(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG PatchID)

    {
    ULONG i;
    ULONG caches;
    ULONG cache1push0;
    ULONG cache1pull0;
    PCOMMONOBJECT StaticObjectPtr;
    PPATCHCONTEXT PatchContextPtr;
    ULONG Context;
    PHWINFO NVInfo;

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Fifo access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // If a true resource manager were implemented, then there would probably
    // be several different patches setup (conceptually).  But since we don't
    // have a resource manager, we're programming at the hardware/physical level.
    // So basically, all rendering objects will share the ROP SOLID , PATTERN,
    // CLIP, and IMAGE SOLID devices.  Another way to look at it at this level,
    // is that we only implement ONE patch and all the rendering objects
    // will share it.
    //
    //
    //          ROP SOLID   PATTERN     BLACK RECTANGLE     IMAGE SOLID
    //              |           |           |                   |
    //  RENDER--     ---    ----            |                   |
    //  OBJECT  |       |  |                |                   |
    //          |    -----------         -------------       ----------
    //           ---|           |       |             |     |          |
    //  RENDER _____|   IMAGE   |_______|    IMAGE    |_____|  IMAGE   |__IMAGE TO__VIDEO
    //  OBJECT      |  ROP AND  |       |   STENCIL   |     | COLORKEY |   VIDEO    SINK
    //           ---|           |       |             |     |          |
    //  RENDER _|  - -----------         -------------       ----------
    //  OBJECT    |
    //            |
    //  RENDER ---
    //  OBJECT
    //
    //  etc...
    //
    //  Touching any of the ROP SOLID,PATTERN, CLIP, or IMAGE SOLID devices
    //  will affect ALL rendering objects.  In essence, we're programming
    //  this card without the assistance of context switching from a resource
    //  manager.  We have to do it ourselves.
    //
    // See function MthdStaticImage() in the Win 95 Resource manager
    // for more information on how the following contexts are initialized
    //
    //**************************************************************************


    //**************************************************************************
    // Update the context for the STATIC OBJECT (IMAGE_BLACK_RECTANGLE).
    //**************************************************************************

    StaticObjectPtr = (PCOMMONOBJECT)&(HwDeviceExtension->CommonObject[IMAGE_BLACK_RECTANGLE_ID]);

    //**************************************************************************
    // Set the color format according to the current frame buffer depth
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    if (NVInfo->Framebuffer.Depth == 8)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A8Y8;
    else if (NVInfo->Framebuffer.Depth == 16)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A1R5G5B5;
    else
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_A8R8G8B8;

    StaticObjectPtr->MonoFormat = 0;


    StaticObjectPtr->Context = DEVICE_BASE(NV_UCLIP) | StaticObjectPtr->ColorFormat;

    //**************************************************************************
    // Update the context for the associated RENDER OBJECT.
    // Enable CLIPPING
    //**************************************************************************

    PatchContextPtr = (PPATCHCONTEXT)&(HwDeviceExtension->RenderObject[PatchID].RCPatchContext);
    PatchContextPtr->ImageConfig |= DRF_DEF(_PGRAPH,_CTX_SWITCH, _USER_CLIP, _ENABLED);

    //**************************************************************************
    // Specify the actual CLIP values.  These will actually get written
    // to hardware later.
    //**************************************************************************

    PatchContextPtr->Clip.xClipMin = 100;
    PatchContextPtr->Clip.xClipMax = 320;  // TO DO :Get values from NVINFO !!!
    PatchContextPtr->Clip.yClipMin = 100;
    PatchContextPtr->Clip.yClipMax = 240;  // TO DO: Get values from NVINFO

    //**************************************************************************
    // Save the name of this STATIC object
    // Also mark which object is currently using the specified subchannel
    //**************************************************************************

    StaticObjectPtr->Base.Name = MY_BLACK_RECTANGLE;
    HwDeviceExtension->FifoTable.ObjectStack[CLIP_SUBCHANNEL] = MY_BLACK_RECTANGLE;

    //**************************************************************************
    // Add STATIC object to hash table and update the cached context register
    //**************************************************************************

    Context = StaticObjectPtr->Context;
    SetObjectHash(HwDeviceExtension, MY_BLACK_RECTANGLE, Context);
    SetObjectCachedCtxRegs(HwDeviceExtension, CLIP_SUBCHANNEL, MY_BLACK_RECTANGLE, Context);

    //**************************************************************************
    // Restore saved FIFO state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);

    }



//******************************************************************************
//
//  Function:   SetContextImageSolid()
//
//  Routine Description:
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

VOID SetContextImageSolid(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG PatchID)

    {
    ULONG i;
    ULONG caches;
    ULONG cache1push0;
    ULONG cache1pull0;
    PCOMMONOBJECT StaticObjectPtr;
    PPATCHCONTEXT PatchContextPtr;
    ULONG Context;
    PHWINFO NVInfo;

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Fifo access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // Get pointer to NVInfo structure
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    //**************************************************************************
    // If a true resource manager were implemented, then there would probably
    // be several different patches setup (conceptually).  But since we don't
    // have a resource manager, we're programming at the hardware/physical level.
    // So basically, all rendering objects will share the ROP SOLID , PATTERN,
    // CLIP, and IMAGE SOLID devices.  Another way to look at it at this level,
    // is that we only implement ONE patch and all the rendering objects
    // will share it.
    //
    //
    //          ROP SOLID   PATTERN     BLACK RECTANGLE     IMAGE SOLID
    //              |           |           |                   |
    //  RENDER--     ---    ----            |                   |
    //  OBJECT  |       |  |                |                   |
    //          |    -----------         -------------       ----------
    //           ---|           |       |             |     |          |
    //  RENDER _____|   IMAGE   |_______|    IMAGE    |_____|  IMAGE   |__IMAGE TO__VIDEO
    //  OBJECT      |  ROP AND  |       |   STENCIL   |     | COLORKEY |   VIDEO    SINK
    //           ---|           |       |             |     |          |
    //  RENDER _|  - -----------         -------------       ----------
    //  OBJECT    |
    //            |
    //  RENDER ---
    //  OBJECT
    //
    //  etc...
    //
    //  Touching any of the ROP SOLID,PATTERN, CLIP, or IMAGE SOLID devices
    //  will affect ALL rendering objects.  In essence, we're programming
    //  this card without the assistance of context switching from a resource
    //  manager.  We have to do it ourselves.
    //
    // See function MthdStaticImage() in the Win 95 Resource manager
    // for more information on how the following contexts are initialized
    //
    //**************************************************************************

    //**************************************************************************
    // Update the context for the STATIC OBJECT (IMAGE_SOLID).
    // Enable COLORKEY'ing
    //
    // Color Format information (Assumes only Destination Buffer 0 Enabled):
    // See Table 7-2 in the NV1 Graphics Reference Manual for more information
    // on Color and Buffer Configuration Bits 9-12 in CTX_SWITCH register
    //
    //              DEFINE  corresponds to  COLOR FORMAT
    //              --------------          ------------
    //              X17R5G5B5               0x00000000
    //              X16A1R5G5B5             0x00002000
    //              X8R8G8B8                0x00000200
    //              A8RG8B8                 0x00002200
    //              X2R10G10B10             0x00000400
    //              A2R10G10B10             0x00002400
    //              etc..
    //
    //**************************************************************************

    StaticObjectPtr = (PCOMMONOBJECT)&(HwDeviceExtension->CommonObject[IMAGE_SOLID_ID]);

    //**************************************************************************
    // Set the color format according to the current frame buffer depth
    //**************************************************************************

    if (NVInfo->Framebuffer.Depth == 8)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A8Y8;
    else if (NVInfo->Framebuffer.Depth == 16)
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_X16A1R5G5B5;
    else
        StaticObjectPtr->ColorFormat = COLOR_FORMAT_A8R8G8B8;

    StaticObjectPtr->MonoFormat = 0;
    StaticObjectPtr->Context = DEVICE_BASE(NV_UCHROMA)                    |
                              StaticObjectPtr->ColorFormat;

    //**************************************************************************
    // Update the context for the associated RENDER OBJECT.
    // Enable COLORKEY'ing
    //**************************************************************************

    PatchContextPtr = (PPATCHCONTEXT)&(HwDeviceExtension->RenderObject[PatchID].RCPatchContext);
    PatchContextPtr->ImageConfig |= DRF_DEF(_PGRAPH,_CTX_SWITCH,_CHROMA_KEY,_ENABLED);

    //**************************************************************************
    // Specify the actual ROP value.  This will actually get written
    // to hardware later.
    //**************************************************************************

    PatchContextPtr->ColorKey.Color = 0;  //Disable color keying

    //**************************************************************************
    // Save the name of this STATIC object
    // Also mark which object is currently using the specified subchannel
    //**************************************************************************

    StaticObjectPtr->Base.Name = MY_IMAGE_SOLID;
    HwDeviceExtension->FifoTable.ObjectStack[7] = MY_IMAGE_SOLID;

    //**************************************************************************
    // Add STATIC object to hash table and update the cached context register
    //**************************************************************************

    Context = StaticObjectPtr->Context;
    SetObjectHash(HwDeviceExtension, MY_IMAGE_SOLID, Context);
    SetObjectCachedCtxRegs(HwDeviceExtension, 7, MY_IMAGE_SOLID, Context);

    //**************************************************************************
    // Restore saved FIFO state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);

    }


//******************************************************************************
//
//  Function:   SetPatchContext()
//
//  Routine Description:
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

VOID SetPatchContext(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG PatchID)

    {
    ULONG i;
    ULONG caches;
    ULONG cache1push0;
    ULONG cache1pull0;
    PPATCHCONTEXT PatchContext;
    PGRAPHICSCHANNEL GrTablePtr;
    PCOMMONOBJECT StaticObjectPtr;
    ULONG Data;
    ULONG XYLogicMisc1;

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Fifo access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    //
    // grLoadPatch vs. grLoadChannelContext in Win 95RM
    // ------------------------------------------------
    // In the Win95 Resource Manager (for NV1), these functions are very similar.
    // The difference between these two functions is that grLoadChannelContext
    // is used mainly during Context Switching, whereas grLoadPatch is used
    // update the contexts of the NON-RENDERING objects, usually the first time.
    // grLoadChannelContext just manually goes in an modifies the global registers
    // and restores the global state of the graphics engine. In other words,
    // grLoadPatch is a SuperSet/One Time init of grLoadChannelContext.
    // Not only does it set the registers, it also sets up an entry in the
    // HASH TABLE for the object, and updates the object's context in the
    // appropriate cached context register.
    //
    // Here, in this miniport, the SetPatchContextXXXXX functions will be
    // analogous to grLoadPatch.
    //
    // This function will:
    //
    //         1) Set up the static object states (ROP,CLIP,PATTERN,COLOR KEY)
    //            This includes
    //                  - Adding an entry into the HASH table for each static object
    //                  - Setting up the context of static object in
    //                    the cached context registers
    //                  - Updating the associated RENDERING object's context.
    //                    It is affected by whatever values we specify in the
    //                    static objects (i.e.  enables clipping )
    //         2) Set the actual hardware registers for ROP, CLIP, PATTERN, and COLOR KEY
    //
    //**************************************************************************

    //**************************************************************************
    // The following functions will:
    //
    //      1) Calculate the context for the specific static object
    //         according to what attributes we specify
    //      2) Update the Hash Table and store the Context in the
    //         corresponding cached context register
    //      3) Update the context for the corresponding RENDER object
    //         associated with this patch
    //
    // Static Objects: ROP, PATTERN, BLACK RECTANGLE(CLIP), and IMAGE SOLID (COLOR KEY)
    //**************************************************************************

    //**************************************************************************
    // Color Keying is currently DISABLED !!! (Because we only have 8 subchannels
    // and color keying is not that important for a display driver.
    //
    // Update: Added SetContextImageSolid back in because DDRAW requires it!!
    //**************************************************************************

    SetContextImageSolid(HwDeviceExtension,PatchID);

    SetContextBlackRectangle(HwDeviceExtension,PatchID);

    SetContextRopSolid(HwDeviceExtension,PatchID);

    SetContextPattern(HwDeviceExtension,PatchID);

    //**************************************************************************
    // Get pointers to each of the static state objects so that
    // we can setup the hardware register values.
    //**************************************************************************

    PatchContext = (PPATCHCONTEXT)&(HwDeviceExtension->RenderObject[PatchID].RCPatchContext);
    GrTablePtr   = &(HwDeviceExtension->GrTable);

    GrTablePtr->CurrentColorKey  = &(PatchContext->ColorKey);
    GrTablePtr->CurrentRop       = &(PatchContext->Rop);
    GrTablePtr->CurrentClip      = &(PatchContext->Clip);
    GrTablePtr->CurrentPattern   = &(PatchContext->RopPat);

    //**************************************************************************
    // Load STATIC STATE into hardware registers:
    //**************************************************************************

    // Color Key/Image Solid
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CHROMA, GrTablePtr->CurrentColorKey->Color);

    // Clip/Black Rectangle (ALWAYS write MIN values before MAX values!!)
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_XMIN, GrTablePtr->CurrentClip->xClipMin);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_YMIN, GrTablePtr->CurrentClip->yClipMin);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_XMAX, GrTablePtr->CurrentClip->xClipMax);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_YMAX, GrTablePtr->CurrentClip->yClipMax);

    // Always use the USER clip rects
    XYLogicMisc1=0;
    XYLogicMisc1 |= (DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_XCMIN, _USERMIN)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_XCMAX, _USERMAX)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_YCMIN, _USERMIN)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_YCMAX, _USERMAX)) ;
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_XY_LOGIC_MISC1,XYLogicMisc1);

    // Pattern
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR0_0, GrTablePtr->CurrentPattern->PattColor0);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR0_1, GrTablePtr->CurrentPattern->PattColor0Alpha);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR1_0, GrTablePtr->CurrentPattern->PattColor1);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR1_1, GrTablePtr->CurrentPattern->PattColor1Alpha);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATTERN(0), GrTablePtr->CurrentPattern->Pattern0);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATTERN(1), GrTablePtr->CurrentPattern->Pattern1);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATTERN_SHAPE, GrTablePtr->CurrentPattern->PatternShape);

    // Rop
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ROP3, GrTablePtr->CurrentRop->Rop3);

    // Beta and Plane Mask
////    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_BETA, GrTablePtr->CurrentBeta->Beta);
////    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PLANE_MASK, GrTablePtr->CurrentPlaneMask->Color);

    //**************************************************************************
    // Reset context switch bits
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_EXCEPTIONS,0x00000000);

    //**************************************************************************
    // Restore saved FIFO state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);


    }


//******************************************************************************
//
//  Function:   InitColorFormatTable()
//
//  Routine Description:
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

VOID InitColorFormatTable(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    HwDeviceExtension->ColorFormatTable[0] = 0x00000000;
    HwDeviceExtension->ColorFormatTable[1] = 0x00000001;
    HwDeviceExtension->ColorFormatTable[2] = 0x00000002;
    HwDeviceExtension->ColorFormatTable[3] = 0x00010000;
    HwDeviceExtension->ColorFormatTable[4] = 0x00010001;
    }



//******************************************************************************
//
//  Function:   SetObjectColorFormat()
//
//  Routine Description:
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

VOID SetObjectColorFormat(PHW_DEVICE_EXTENSION HwDeviceExtension,
                          PCOMMONOBJECT Pobject, ULONG UserColorFormat,
                          ULONG MonoFormat)

    {
    ULONG AdjustedColor;
    ULONG i;

    //**************************************************************************
    // This function is used to convert the user specified color format
    // into the hardware context field
    //**************************************************************************

    //**************************************************************************
    // Mask off 2 bits. (Don't accept monochrome formats)
    //**************************************************************************

    AdjustedColor = (UserColorFormat & ~0x01100000);

    for (i=0; i < 5; i++)
        {
        if (AdjustedColor == HwDeviceExtension->ColorFormatTable[i])
            {
            Pobject->ColorFormat = DRF_NUM(_PGRAPH, _CTX_SWITCH,_COLOR_BUFFER,i);

            //******************************************************************
            // Determine if we need to set the ALPHA bit for this color format
            //******************************************************************

            if (UserColorFormat & 0x00100000)
                Pobject->ColorFormat |= DRF_DEF(_PGRAPH,_CTX_SWITCH,_ALPHA,_ENABLED);
            }
        }

    //**************************************************************************
    // Set the MonoFormat bit (either 0 or 1)
    //**************************************************************************

    Pobject->MonoFormat = MonoFormat;

    }


//******************************************************************************
//
//  Function:   ClearPatchContext()
//
//  Routine Description:
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

VOID ClearPatchContext(PHW_DEVICE_EXTENSION HwDeviceExtension,
                        PPATCHCONTEXT PatchContext)

    {
    ULONG i;

    //**************************************************************************
    // Initialize context configuration.
    //**************************************************************************

    PatchContext->ImageConfig  = DRF_NUM(_PGRAPH, _CTX_SWITCH, _PATCH_CONFIG, 0x17);
    PatchContext->VideoConfig  = (ULONG)NULL;
//    PatchContext->ImageVideo   = NULL;
//    PatchContext->Canvas       = NULL;
//    PatchContext->Rop          = NULL;
//    PatchContext->RopPat       = NULL;
//    PatchContext->RopDst       = NULL;
//    PatchContext->Beta         = NULL;
//    PatchContext->BetaSrc      = NULL;
//    PatchContext->BetaPat      = NULL;
//    PatchContext->BetaDst      = NULL;
//    PatchContext->ColorKey     = NULL;
//    PatchContext->PlaneMask    = NULL;
//    PatchContext->PlaneMaskDst = NULL;
//    PatchContext->Clip         = NULL;
//    PatchContext->Pattern      = NULL;
//    PatchContext->ColorMap0    = NULL;
//    PatchContext->ColorMap1    = NULL;
//    PatchContext->VideoSwitch  = NULL;

    }

//******************************************************************************
//
//  Function:   NV1SetupPatches()
//
//  Routine Description:
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

VOID NV1SetupPatches(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    PRENDEROBJECT PObject;
    ULONG Data;
    ULONG PatchCount;
    PHWINFO NVInfo;
    ULONG ColorFormat;
    ULONG MonoFormat;

    //**************************************************************************
    //
    // Enable FLOWTHRU
    //
    //      In order to access many of the registers in the Graphics Engine,
    //      we need to enable FLOWTHRU and disable the FIFO engine.
    //      FLOWTHRU = ENABLED,  FIFO = DISABLED, DMA = DISABLED
    //      CLASS = 0 to specify graphics registers.
    //
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED)          |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_DISABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
                            DRF_NUM(_PGRAPH,_MISC,_CLASS,0));

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    //**************************************************************************

    GR_DONE;

    //**************************************************************************
    // Checks for any interrupt requests.  For this miniport,
    // we shouldn't be getting any
    //**************************************************************************

    GR_IDLE;

    //**************************************************************************
    // Reset interrupt registers
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_0, 0x11111111);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_1, 0x11111111);

    //**************************************************************************
    // Init the PatchContexts to NULL for all the patches that we'll be using
    //**************************************************************************

    for (PatchCount=0; PatchCount < NUM_PATCHES; PatchCount++)
       {

       //***********************************************************************
       // Get the RENDER object structure that will be used for this patch
       //***********************************************************************

       PObject = &(HwDeviceExtension->RenderObject[PatchCount]);

       //***********************************************************************
       // Initialize the patch context values
       //***********************************************************************

       ClearPatchContext(HwDeviceExtension, &(PObject->RCPatchContext));

       }



    //**************************************************************************
    // Set the color format according to what the current mode depth is
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        case 16:
            ColorFormat = NV_COLOR_FORMAT_LE_X17R5G5B5;
            break;
        case 8:
            ColorFormat = NV_COLOR_FORMAT_LE_X24Y8;
            break;
        default:
            ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        }

    //**************************************************************************
    // Default mono format is 0 and ALPHA NOT enabled
    //**************************************************************************

    MonoFormat = MONO_FORMAT_LE;

    //**************************************************************************
    // Now setup the individual patches that we'll be using for
    // the NT Display driver
    //**************************************************************************

RECTANGLE:

    //**************************************************************************
    // Set Patch for RECTANGLE
    //**************************************************************************

    SetPatchContext(HwDeviceExtension, RECTANGLE_PATCH);

    //**************************************************************************
    // Setup the Render object (MY_RENDER_SOLID_RECTANGLE)
    //**************************************************************************

    MonoFormat = MONO_FORMAT_LE;
    SetupRenderObject(HwDeviceExtension, RECTANGLE_SUBCHANNEL, RECTANGLE_PATCH,
                      NV_RENDER_SOLID_RECTANGLE, MY_RENDER_SOLID_RECTANGLE,
                      ColorFormat,MonoFormat);

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
    //**************************************************************************

    GR_DONE;
    GR_IDLE;


TRIANGLE:

    //**************************************************************************
    // Since we've only got 8 subchannels, we don't have enough room for the
    // TRIANGLE object.  If we need it use it later (i.e. Direct 3D/OpenGL), we'll need
    // to manually swap out one of the current objects and load the TRIANGLE object
    //**************************************************************************


//    //**************************************************************************
//    // Set Patch for TRIANGLE
//    //**************************************************************************
//
//    SetPatchContext(HwDeviceExtension, TRIANGLE_PATCH);
//
//    //**************************************************************************
//    // Setup the Render object (MY_RENDER_SOLID_TRIANGLE)
//    //**************************************************************************
//
//    MonoFormat = MONO_FORMAT_LE;
//    SetupRenderObject(HwDeviceExtension,TRIANGLE_SUBCHANNEL,TRIANGLE_PATCH ,
//                      NV_RENDER_SOLID_TRIANGLE, MY_RENDER_SOLID_TRIANGLE,
//                      ColorFormat,MonoFormat);
//
//    //**************************************************************************
//    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
//    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
//    //**************************************************************************
//
//    GR_DONE;
//    GR_IDLE;


BLIT:

    //**************************************************************************
    // Set Patch for BLIT
    //**************************************************************************

    SetPatchContext(HwDeviceExtension, BLIT_PATCH);

    //**************************************************************************
    // Setup the BLIT object
    //**************************************************************************

    MonoFormat = MONO_FORMAT_LE;
    SetupRenderObject(HwDeviceExtension,BLIT_SUBCHANNEL,BLIT_PATCH ,
                      NV_IMAGE_BLIT, MY_RENDER_SCRN_TO_SCRN_BLIT,
                      ColorFormat, MonoFormat);

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
    //**************************************************************************

    GR_DONE;
    GR_IDLE;


CPU_TO_SCREEN_BLIT:

    //**************************************************************************
    // Set Patch for IMAGE FROM CPU
    //**************************************************************************

    SetPatchContext(HwDeviceExtension, IMAGE_FROM_CPU_PATCH);

    //**************************************************************************
    // Setup the IMAGE_FROM_CPU object
    //**************************************************************************

    MonoFormat = MONO_FORMAT_LE;
    SetupRenderObject(HwDeviceExtension,IMAGE_FROM_CPU_SUBCHANNEL,IMAGE_FROM_CPU_PATCH,
                      NV_IMAGE_FROM_CPU, MY_IMAGE_FROM_CPU_BLIT,
                      ColorFormat, MonoFormat);

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
    //**************************************************************************

    GR_DONE;
    GR_IDLE;



IMAGE_MONO_BLIT:

    //**************************************************************************
    // For text, we want color 0 (background) to be transparent
    // So set the color format appropriately with ALPHA channel enabled
    //**************************************************************************

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            ColorFormat = NV_COLOR_FORMAT_LE_A8R8G8B8;
            break;
        case 16:
            ColorFormat = NV_COLOR_FORMAT_LE_X16A1R5G5B5;
            break;
        case 8:
            ColorFormat = NV_COLOR_FORMAT_LE_X16A8Y8;
            break;
        default:
            ColorFormat = NV_COLOR_FORMAT_LE_A8R8G8B8;
            break;
        }

    //**************************************************************************
    // Set Patch for IMAGE MONO FROM CPU
    //**************************************************************************

    SetPatchContext(HwDeviceExtension, IMAGE_MONO_FROM_CPU_PATCH);

    //**************************************************************************
    // Setup the IMAGE_MONO_FROM_CPU object
    //**************************************************************************

    MonoFormat = MONO_FORMAT_CGA6;
    SetupRenderObject(HwDeviceExtension,IMAGE_MONO_FROM_CPU_SUBCHANNEL,IMAGE_MONO_FROM_CPU_PATCH,
                      NV_IMAGE_MONOCHROME_FROM_CPU, MY_IMAGE_MONO_FROM_CPU_BLIT,
                      ColorFormat, MonoFormat);

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
    //**************************************************************************

    GR_DONE;
    GR_IDLE;



    //**************************************************************************
    // We're done with the Graphics registers for now.
    // Make sure the FIFO and DMA engines are enabled.
    //
    //      FLOWTHRU = ENABLED,  FIFO = ENABLED, DMA = ENABLED
    //
    // DON'T TOUCH THE CLASS bits!!
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_ENABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_ENABLED));

    }




//******************************************************************************
//
//  Function:   SetupRenderObject()
//
//  Routine Description:
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

VOID SetupRenderObject(PHW_DEVICE_EXTENSION HwDeviceExtension,
                       ULONG SubChannel, ULONG PatchID,
                       ULONG RenderObjectClass, ULONG UserObjectName,
                       ULONG ColorFormat, ULONG MonoFormat)

    {
    PCOMMONOBJECT PObject;
    ULONG Class;
    ULONG Instance;
    ULONG Data;

    //**************************************************************************
    //
    // Enable FLOWTHRU
    //
    //      In order to access many of the registers in the Graphics Engine,
    //      we need to enable FLOWTHRU and disable the FIFO engine.
    //
    //      FLOWTHRU = ENABLED,  FIFO = DISABLED, DMA = DISABLED
    //      CLASS = 0 to access graphics registers.
    //
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED)          |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_DISABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
                            DRF_NUM(_PGRAPH,_MISC,_CLASS,0));

    //**************************************************************************
    // Get a ptr to the RENDER object structure (Source Object)
    //**************************************************************************

    PObject = (PCOMMONOBJECT) &(HwDeviceExtension->RenderObject[PatchID]);

    //**************************************************************************
    // Save the user specified name of the object
    // Also mark which object is currently using the specified subchannel
    //**************************************************************************

    PObject->Base.Name = UserObjectName;
    HwDeviceExtension->FifoTable.ObjectStack[SubChannel] = UserObjectName;

    //**************************************************************************
    // Set Color format of the RENDER object
    //**************************************************************************

    SetObjectColorFormat(HwDeviceExtension,PObject,ColorFormat,MonoFormat);

    //**************************************************************************
    // After updating non-rendering objects and their contexts,
    // we now need to calculate the context for the the RENDER object.
    //
    // A Simple Patch:
    //
    //      RENDER SOLID  ----->  IMAGE TO  ------>  VIDEO SINK
    //        RECTANGLE             VIDEO
    //
    //**************************************************************************

    CalculateRenderObjectContext(HwDeviceExtension,(PRENDEROBJECT)PObject,RenderObjectClass);

    //**************************************************************************
    // CAREFUL!
    //
    // 1) When setting Device specific CTX_SWITCH registers,
    //    an INTERRUPT is generated.  So to avoid this,
    //    DISABLE interrupts when setting these registers !!
    //
    //     - Update-> The reason why interrupts were being generated
    //                is because the PATCH_ID switch was getting SET
    //                in the CTX_SWITCH register.  This is ok for Resource
    //                Manager code which handles interrupts, but here in this
    //                Miniport, we're doing a ONE TIME init of all the objects
    //                and don't want interrupts.  Disabling/Enabling
    //                interrupts around the USER_xxx_CTX_SWITCH registers
    //                is no longer necessary.  Oh Yeah!
    //
    // 2) If the object changed or is NEW, we need to set the device's
    //    OWN context switch register (as opposed to PGRAPH_CTX_SWITCH)
    //    First determine the device, then load the instance in the specific
    //    device's CTX_SWITCH register.  However, be aware that this WILL CLEAR
    //    FLOWTHRU !!! We need to make sure and re-enable it. Otherwise,
    //    subsequent functions which touch graphics registers may not work correctly.
    //
    // 2)  Also, the CTX_SWITCH register MUST be set BEFORE we call
    //     UpdateRenderObjectContext().  Otherwise, the graphics engine
    //     will stop when rendering the object.  I'm not sure I understand
    //     why this is, but for now, just make sure it occurs BEFORE, not AFTER.
    //
    //**************************************************************************

//    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_EN_0, 0x00000000);

    //**************************************************************************
    // Get the physical instance from the context (Bottom 16 bits)
    // We will always be using Channel 0 (Specified in the Upper bits)
    //**************************************************************************

    Instance = (PObject->Context) & 0x0000ffff;

    switch (RenderObjectClass)
        {
        case NV_RENDER_SOLID_POINT:
            break;
        case NV_RENDER_SOLID_LINE:
            break;
        case NV_RENDER_SOLID_LIN:
            break;
        case NV_RENDER_SOLID_TRIANGLE:
            UTRI_REG_WR32(UTRI_Base, NV_UTRI_CTX_SWITCH, (0 | Instance) );
            break;
        case NV_RENDER_SOLID_RECTANGLE:
            URECT_REG_WR32(URECT_Base, NV_URECT_CTX_SWITCH, (0 | Instance) );
            break;
        case NV_RENDER_TEXTURE_BILINEAR:
            break;
        case NV_RENDER_TEXTURE_QUADRATIC:
            break;
        case NV_IMAGE_BLIT:
            UBLIT_REG_WR32(UBLIT_Base, NV_UBLIT_CTX_SWITCH, (0 | Instance) );
            break;
        case NV_IMAGE_FROM_CPU:
            UIMAGE_REG_WR32(UIMAGE_Base, NV_UIMAGE_CTX_SWITCH, (0 | Instance) );
            break;
        case NV_IMAGE_MONOCHROME_FROM_CPU:
            UBITMAP_REG_WR32(UIMAGE_Base, NV_UBITMAP_CTX_SWITCH, (0 | Instance) );
            break;
        default:
            break;
        }

//    //*************************************************************************
//    // Now that we've set the device specific CTX_SWITCH register,
//    // re-enable interrupts
//    //*************************************************************************
//
//    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_EN_0, 0x11111111);

    //**************************************************************************
    // FLOWTHRU was cleared up above! Make sure and RE-enable it,
    // Otherwise, UpdateRenderObjectContext may not work correctly.
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED));

    //**************************************************************************
    // Now store the context for the RENDER object
    //**************************************************************************

    UpdateRenderObjectContext(HwDeviceExtension, UserObjectName, SubChannel,
                              PObject->Context);

    //**************************************************************************
    // FifoResend may be useful for debugging later on (not yet tested)
    //**************************************************************************

//    FifoResend(HwDeviceExtension);

    //**************************************************************************
    // Determine the CLASS value to store in the PGRAPH_MISC register
    // See Appendix E of the Graphics Register reference
    //**************************************************************************

    switch (RenderObjectClass)
        {
        case NV_RENDER_SOLID_POINT:
            Class = 0x8;
            break;
        case NV_RENDER_SOLID_LINE:
            Class = 0x9;
            break;
        case NV_RENDER_SOLID_LIN:
            Class = 0xA;
            break;
        case NV_RENDER_SOLID_TRIANGLE:
            Class = 0xB;
            break;
        case NV_RENDER_SOLID_RECTANGLE:
            Class = 0xC;
            break;
        case NV_RENDER_TEXTURE_BILINEAR:
            Class = 0xD;
            break;
        case NV_RENDER_TEXTURE_QUADRATIC:
            Class = 0xE;
            break;
        case NV_IMAGE_BLIT:
            Class = 0x10;       // UBLIT
            break;
        case NV_IMAGE_FROM_CPU:
            Class = 0x11;       // UIMAGE
            break;
        case NV_IMAGE_MONOCHROME_FROM_CPU:
            Class = 0x12;       // UBITMAP
            break;
        default:
            Class = 8;      // Default to a Point
            break;
        }

    //**************************************************************************
    // Prepare to put this value in the GRAPHIC_MISC register, Bit position 12.
    //**************************************************************************

    Class = Class << 12;

    //**************************************************************************
    // We're done modifying the Graphics registers.
    // ReEnable the FIFO and DMA engines as well as
    // specify the CLASS of the object.
    //
    //      FLOWTHRU = ENABLED,  FIFO = ENABLED, DMA = ENABLED
    //
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_ENABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_ENABLED)            |
                            DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
                            Class);

    //**************************************************************************
    // Enable CACHE1.  Do NOT enable CACHE0 (Why? Runout data present?).
    // Otherwise, the rendering functions will NOT work.
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_ENABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_ENABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_ENABLED);

    }


//******************************************************************************
//
//  Function:   ClearOutRenderObjects()
//
//  Routine Description:
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

VOID ClearOutRenderObjects(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG i;
    ULONG Length;
    ULONG ObjectCount;
    PULONG Ptr;

    //**************************************************************************
    // Get length of RENDER object structure (in Dwords)
    //**************************************************************************

    Length = sizeof(RENDEROBJECT)/4;

    //**************************************************************************
    // Fill render object memory with zeros
    //**************************************************************************

    for (ObjectCount=0; ObjectCount < 10; ObjectCount++)
        {
        Ptr = (PULONG)&(HwDeviceExtension->RenderObject[ObjectCount]);

        //**********************************************************************
        // Clear out one object
        //**********************************************************************

        for (i=0; i<Length; i++)
            {
            *Ptr = 0;
            Ptr++;
            }

        }

    }


//******************************************************************************
//
//  Function:   NV1GetNVInfo()
//
//  Routine Description:
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

BOOLEAN NV1GetNVInfo(PHW_DEVICE_EXTENSION HwDeviceExtension)


    {
    PHWINFO NVInfo;
    ULONG refresh;
    ULONG DepthIndex;
    ULONG i,j;
    BOOLEAN status=TRUE;

    //**************************************************************************
    // Get ptr to NVInfo structure
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    //**************************************************************************
    // Get Chip revision and implementation
    //**************************************************************************

    NVInfo->Chip.Implementation = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_IMPLEMENTATION);
    NVInfo->Chip.Revision = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_REVISION);
    NVInfo->Chip.Architecture = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_ARCHITECTURE);
    NVInfo->Chip.Manufacturer = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_MANUFACTURER);

    //**************************************************************************
    // Get memory type
    //**************************************************************************

    NVInfo->Framebuffer.RamType = PEXTDEV_REG_RD_DRF(PEXTDEV_Base,_PEXTDEV,_BOOT_0,_STRAP_RAM_TYPE) ==
                            NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_VRAM ? BUFFER_VRAM : BUFFER_DRAM;

    //**************************************************************************
    // Get amount of VRAM
    //**************************************************************************

    switch (PFB_REG_RD_DRF(PFB_Base,_PFB,_BOOT_0, _RAM_AMOUNT))
        {
        case NV_PFB_BOOT_0_RAM_AMOUNT_4MB:
            NVInfo->Framebuffer.RamSizeMb = 4;
            NVInfo->Framebuffer.RamSize   = 0x00400000;
            break;
        case NV_PFB_BOOT_0_RAM_AMOUNT_2MB:
            NVInfo->Framebuffer.RamSizeMb = 2;
            NVInfo->Framebuffer.RamSize   = 0x00200000;
            break;
        case NV_PFB_BOOT_0_RAM_AMOUNT_1MB:
            NVInfo->Framebuffer.RamSizeMb = 1;
            NVInfo->Framebuffer.RamSize   = 0x00100000;
            break;
        default:
            //******************************************************************
            // Invalid memory configuration
            //******************************************************************
            status=FALSE;
            return(status);
        }

    //**************************************************************************
    // Don't forget to fixup the RangeOffset array.
    // NVIsPresent() put a default value (4Mb) for the length of the Dumb Frame Buffer..
    // Now that we know what the correct memory size is, let's make sure to
    // update the correct value.
    //**************************************************************************

    RangeOffsets[NV_PDFB_INDEX].ulLength =  NVInfo->Framebuffer.RamSize;

    //**************************************************************************
    // Default to 640x480x16x1 framebuffer with VGA timing
    //**************************************************************************

    NVInfo->Framebuffer.DpmLevel          = 0;
    NVInfo->Framebuffer.Resolution        = RESOLUTION_640X480;
    NVInfo->Framebuffer.Depth             = 16;
    NVInfo->Framebuffer.RefreshRate       = 60;
    NVInfo->Framebuffer.Count             = 1;
    NVInfo->Framebuffer.ActiveCount       = 1;
    NVInfo->Framebuffer.ActiveMask        = 1;
    NVInfo->Framebuffer.Current           = 0;
    NVInfo->Framebuffer.FlipUsageCount    = 0;
    NVInfo->Framebuffer.FlipTo            = 0;
    NVInfo->Framebuffer.FlipFrom          = 0;
    NVInfo->Framebuffer.UpdateFlags       = 0;
    NVInfo->Framebuffer.HorizFrontPorch   = 0;
    NVInfo->Framebuffer.HorizSyncWidth    = 0;
    NVInfo->Framebuffer.HorizBackPorch    = 0;
    NVInfo->Framebuffer.HorizDisplayWidth = 0;
    NVInfo->Framebuffer.VertFrontPorch    = 0;
    NVInfo->Framebuffer.VertSyncWidth     = 0;
    NVInfo->Framebuffer.VertBackPorch     = 0;
    NVInfo->Framebuffer.VertDisplayWidth  = 0;
    NVInfo->Framebuffer.HSyncPolarity     = 0;
    NVInfo->Framebuffer.VSyncPolarity     = 0;
    NVInfo->Framebuffer.CSync             = 0;
    NVInfo->Framebuffer.ConfigPageHeight  = 0;
    NVInfo->Pram.CurrentSize              = 0;
//    NVInfo->Pram.AvailableSize[0]         = NV_PRAM_DEVICE_SIZE_12_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;
//    NVInfo->Pram.AvailableSize[1]         = NV_PRAM_DEVICE_SIZE_20_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;
//    NVInfo->Pram.AvailableSize[2]         = NV_PRAM_DEVICE_SIZE_36_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;
//    NVInfo->Pram.AvailableSize[3]         = NV_PRAM_DEVICE_SIZE_68_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;

    //**************************************************************************
    // Default Refresh Rate
    //**************************************************************************

    NVInfo->Framebuffer.RefreshRate = 60;
    NVInfo->Framebuffer.HSyncPolarity = BUFFER_HSYNC_NEGATIVE;
    NVInfo->Framebuffer.VSyncPolarity = BUFFER_HSYNC_NEGATIVE;

    //**************************************************************************
    // Default resolution
    //**************************************************************************

    NVInfo->Framebuffer.Resolution        = RESOLUTION_640X480;
    NVInfo->Framebuffer.HorizDisplayWidth = 640;
    NVInfo->Framebuffer.VertDisplayWidth  = 480;
    NVInfo->Framebuffer.Depth             = 16;

    //**************************************************************************
    // Determine refresh rate
    //**************************************************************************

    if (NVInfo->Framebuffer.RefreshRate < 72)
        refresh = 0;
    else if (NVInfo->Framebuffer.RefreshRate < 75)
        refresh = 1;
    else if (NVInfo->Framebuffer.RefreshRate < 85)
        refresh = 2;
    else if (NVInfo->Framebuffer.RefreshRate < 100)
        refresh = 3;
    else if (NVInfo->Framebuffer.RefreshRate < 120)
        refresh = 4;
    else
        refresh = 5;

    //**************************************************************************
    // Get depth index into fbTimingTable
    //**************************************************************************

    switch (NVInfo->Framebuffer.Depth)
      {
      case 8:
         DepthIndex = 0;
         break;
      case 16:
         DepthIndex = 1;
         break;
      case 32:
         DepthIndex = 2;
         break;
      default:
         DepthIndex = 0;
         break;
     }

    //**************************************************************************
    // Init Timing Table
    //**************************************************************************

    NVInfo->Framebuffer.HorizFrontPorch = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][4];
    NVInfo->Framebuffer.HorizSyncWidth  = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][5];
    NVInfo->Framebuffer.HorizBackPorch  = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][6];
    NVInfo->Framebuffer.VertFrontPorch  = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][7];
    NVInfo->Framebuffer.VertSyncWidth   = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][8];
    NVInfo->Framebuffer.VertBackPorch   = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][9];
    NVInfo->Framebuffer.HSyncPolarity   = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][10];
    NVInfo->Framebuffer.VSyncPolarity   = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][11];
    NVInfo->Framebuffer.CSync           = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][12];
    NVInfo->Dac.VClk                    = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][13];
    NVInfo->Framebuffer.RefreshRate     = fbTimingTable[NVInfo->Framebuffer.Resolution + DepthIndex][refresh][14];

    //**************************************************************************
    // TODO: Need to add code to check amount of memory required for
    //       HASH TABLE, PRAM, and INSTANCES.  If not enough, then need to
    //       downgrade resolution until there is enough.
    //**************************************************************************

    //**************************************************************************
    // Determine the DAC type
    //**************************************************************************

    switch (PFB_REG_RD_DRF(PFB_Base,_PFB,_BOOT_0,_DAC_WIDTH))
        {
        case NV_PFB_BOOT_0_DAC_WIDTH_16_BIT:
            NVInfo->Dac.InputWidth = 16;
        case NV_PFB_BOOT_0_DAC_WIDTH_32_BIT:
            NVInfo->Dac.InputWidth = 32;
        case NV_PFB_BOOT_0_DAC_WIDTH_64_BIT:
            NVInfo->Dac.InputWidth = 64;
        }


    //**************************************************************************
    // Update the DAC flags and types
    //**************************************************************************

    NVInfo->Dac.UpdateFlags = 0;
    NVInfo->Dac.FinishFlags = 0;

    //**************************************************************************
    // Initialize the cursor data (transparent)
    // The cursor registers actually get set in NVSetMode
    //**************************************************************************

    NVInfo->Dac.CursorType = NV1_DAC_CURSOR_TWO_COLOR_XOR;
    NVInfo->Dac.CursorEmulation = FALSE;
    NVInfo->Dac.CursorExclude= FALSE;
    NVInfo->Dac.CursorColor1 = 0x00000000;
    NVInfo->Dac.CursorColor2 = 0x00ffffff;
    NVInfo->Dac.CursorColor3 = 0x00000000;

    for (i=0;i<128;i++)
        NVInfo->Dac.CursorImagePlane[0][i] = 0xff;

    for (i=0;i<128;i++)
        NVInfo->Dac.CursorImagePlane[1][i] = 0xff;

    for (i=0;i<32;i++)
        for (j=0;j<32;j++)
            NVInfo->Dac.CursorColorImage[i][j]= 0x00000000;

    //**************************************************************************
    // Initialize palette to gray scale
    //**************************************************************************

    for (i=0;i<256;i++)
        NVInfo->Dac.Palette[i] = (i<<16) | (i<<8) | i;

    //**************************************************************************
    // Initialize Power Management state
    //**************************************************************************

    NVInfo->Dac.DpmLevel = 0;

    //**************************************************************************
    // Initialize Video Clock
    //**************************************************************************

    NVInfo->Dac.VClk= 0;

    //**************************************************************************
    // Return success
    //**************************************************************************

    return(status);
    }


//******************************************************************************
//
//  Function:   NV1SetupGraphicsEngine()
//
//  Routine Description:
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

VOID NV1SetupGraphicsEngine(PHW_DEVICE_EXTENSION HwDeviceExtension)


    {
    ULONG Data;
    PHWINFO NVInfo;


    //**************************************************************************
    // Wait Graphics engine to be IDLE
    //**************************************************************************

    GR_IDLE;

    //**************************************************************************
    // Get ptr to NVInfo structure
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    //**************************************************************************
    // Master Control Registers
    //
    //      - Enable all ENGINES
    //      - HARDWARE interrupts on PIN A were already init in ModeSet code
    //        (PMC_INTR_EN_0_INTA set to HARDWARE)
    //
    //**************************************************************************

    PMC_REG_WR32(PMC_Base, NV_PMC_ENABLE, 0xfff000ff);  // Reset Text, Graphics
    PMC_REG_WR32(PMC_Base, NV_PMC_ENABLE, 0xffffffff);  //   and Fifo Engines

    //**************************************************************************
    // We MUST make sure to initialize these register addresses to 0.
    // If we don't, the Graphics Engine may not work !
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_RUNOUT_PUT,_ADDRESS,0);
    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_RUNOUT_GET,_ADDRESS,0);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_ENABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_ENABLED);

    //**************************************************************************
    // Enable FLOWTHRU
    //
    //      In order to access many of the registers in the Graphics Engine,
    //      we need to enable FLOWTHRU and disable the FIFO engine.
    //      FLOWTHRU = ENABLED,  FIFO = DISABLED, DMA = DISABLED
    //      Class = 0 to access graphics registers
    //
    // NOTE:
    //      Just about every GRAPHICS register requires FLOWTHRU to be enabled.
    //      (except for PGRAPH_INTR_0 and PGRAPH_MISC). So we need to be careful
    //      that it is enabled when we need it.  When setting CTX_SWITCH registers,
    //      this bit can become disabled.  So be careful!  WE don't currently
    //      implement a Resource Manager/ Interrupt handling. If these registers
    //      are accessed when FLOWTHRU is disabled, the graphics engine will probably
    //      STOP , waiting for someone (the Resource Manager) to fix the error
    //      condition. This may occur in the user's 'While control.free < N' loop.
    //      Also, make sure to init CLASS field to 0.  Class = 0 should be
    //      used to denote writes to the graphics registers.
    //
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED)          |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_DISABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
                            DRF_NUM(_PGRAPH,_MISC,_CLASS,0));

    //**************************************************************************
    // Reset state machine in graphics engine
    //**************************************************************************

    PGRAPH_REG_WR_DRF_DEF(PGRAPH_Base,_PGRAPH, _DEBUG_1, _VOLATILE_RESET, _LAST);
    PGRAPH_REG_WR_DRF_DEF(PGRAPH_Base,_PGRAPH, _DEBUG_0, _STATE, _RESET);

    //**************************************************************************
    // Deal with bug in chip.
    // Block write is only enabled under certain conditions
    // For now, we always enable it
    //**************************************************************************

//******************************************************************************
//    if ((pDev.Framebuffer.RamType == BUFFER_VRAM)
//      && ((((pDev.Framebuffer.RamSizeMb != 4) && (pDev.Framebuffer.Depth == 32))
//       || ((pDev.Framebuffer.RamSizeMb == 1) && (pDev.Framebuffer.Depth == 16)))))
//         pDev.Graphics.Debug0 &= ~DRF_DEF(_PGRAPH, _DEBUG_0, _BLOCK, _ENABLED);
//    else
//         pDev.Graphics.Debug0 |=  DRF_DEF(_PGRAPH, _DEBUG_0, _BLOCK, _ENABLED);
//******************************************************************************

    Data = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_DEBUG_0);
    Data |= DRF_DEF(_PGRAPH,_DEBUG_0,_BLOCK, _ENABLED);
    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_0,Data);

//******************************************************************************
// Need to load these DEBUG registers appropriately.
// Otherwise, performance may be dog slow !!!!
// Currently, we assume REV_B !!!!!  (Not yet tested on other REVS)
//******************************************************************************


    NVInfo->Graphics.Debug0 = DRF_DEF(_PGRAPH, _DEBUG_0, _ALPHA_ABORT,     _ENABLED) |
                             DRF_DEF(_PGRAPH, _DEBUG_0, _EDGE_FILLING,    _ENABLED) |
                             DRF_DEF(_PGRAPH, _DEBUG_0, _WRITE_ONLY_ROPS, _ENABLED) |
                             DRF_DEF(_PGRAPH, _DEBUG_0, _NONBLOCK_BROAD,  _ENABLED) |
                             DRF_DEF(_PGRAPH, _DEBUG_0, _BLOCK_BROAD,     _ENABLED) |
                             DRF_DEF(_PGRAPH,_DEBUG_0, _BLOCK, _ENABLED)            |
                             DRF_DEF(_PGRAPH,_DEBUG_0, _BULK_READS, _ENABLED);

//******************************************************************************
// Revision was already obtained in GetNVInfo during FindAdapter call
//******************************************************************************

    switch (NVInfo->Chip.Revision)
        {
        case NV1_REV_B_02:
        case NV1_REV_B_03:
            NVInfo->Graphics.Debug1 = DRF_DEF(_PGRAPH, _DEBUG_1, _HIRES_TM,        _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _FAST_BUS,        _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _TM_QUAD_HANDOFF, _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _FAST_RMW_BLITS,  _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _PATT_BLOCK,      _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _TRI_OPTS,        _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _BI_RECTS,        _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _DMA_ACTIVITY,    _IGNORE)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _VOLATILE_RESET,  _NOT_LAST);
            NVInfo->Graphics.Debug2 = DRF_DEF(_PGRAPH, _DEBUG_2, _VOLATILE_RESET,  _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _TM_FASTINPUT,    _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _BUSY_PATIENCE,   _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _TRAPEZOID_TEXEL, _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _MONO_ABORT,      _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _BETA_ABORT,      _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _ALPHA_ABORT,     _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _AVOID_RMW_BLEND, _DISABLED);
            break;
        case NV1_REV_C_01:
        case NV1_REV_C_02:
            NVInfo->Graphics.Debug1 = DRF_DEF(_PGRAPH, _DEBUG_1, _HIRES_TM,           _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _FAST_BUS,           _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _TM_QUAD_HANDOFF,    _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _FAST_RMW_BLITS,     _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _PATT_BLOCK,         _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _TRI_OPTS,           _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _BI_RECTS,           _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _DMA_ACTIVITY,       _IGNORE)
                                   | DRF_DEF(_PGRAPH, _DEBUG_1, _VOLATILE_RESET,     _NOT_LAST);
            NVInfo->Graphics.Debug2 = DRF_DEF(_PGRAPH, _DEBUG_2, _VOLATILE_RESET,     _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _TM_FASTINPUT,       _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _BUSY_PATIENCE,      _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _TRAPEZOID_TEXEL,    _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _MONO_ABORT,         _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _BETA_ABORT,         _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _ALPHA_ABORT,        _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_2, _AVOID_RMW_BLEND,    _ENABLED);
            NVInfo->Graphics.Debug3 = DRF_DEF(_PGRAPH, _DEBUG_3, _TM_RANGE_INTERRUPT, _DISABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_3, _MONO_BLOCK,         _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_3, _SPARE1,             _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_3, _SPARE2,             _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_3, _SPARE3,             _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_3, _SPARE4,             _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_3, _SPARE5,             _ENABLED)
                                   | DRF_DEF(_PGRAPH, _DEBUG_3, _SPARE6,             _ENABLED);
            break;
        }


    //**************************************************************************
    // Set the DEBUG register appropriately
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_0,NVInfo->Graphics.Debug0);
    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_1,NVInfo->Graphics.Debug1);
    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_2,NVInfo->Graphics.Debug2);

//******************************************************************************
//    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_0, pDev.Graphics.Debug0);
//    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_1, pDev.Graphics.Debug1);
//    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_2, pDev.Graphics.Debug2);
//
//    Need to check revision of chip if need to write out this register
//    if ((pDev.Chip.Revision == NV1_REV_C_01) || (pDev.Chip.Revision == NV1_REV_C_02))
//       REG_WR32(NV_PGRAPH_DEBUG_3, pDev.Graphics.Debug3);
//
//    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_2, pDev.Graphics.Debug2);
//******************************************************************************


    //**************************************************************************
    // Initialize DMA related instance registers to zero
    //**************************************************************************

    PDMA_REG_WR32(PDMA_Base,NV_PDMA_GR_CHANNEL, NV_PDMA_GR_CHANNEL_ACCESS_DISABLED);
    PDMA_REG_WR32(PDMA_Base,NV_PDMA_GR_INSTANCE, 0x00000000);
    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DMA,0x00000000);
    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_NOTIFY,0x00000000);
    PDMA_REG_WR32(PDMA_Base,NV_PDMA_GR_CHANNEL,  NV_PDMA_GR_CHANNEL_ACCESS_ENABLED);

    //**************************************************************************
    // Init Context register for the first time
    // Instance is zero, and Channel is currently invalid
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_CTX_SWITCH,  0x00000000);
    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_CTX_CONTROL, DRF_DEF (_PGRAPH,_CTX_CONTROL,_MINIMUM_TIME,_33US)
                                                 | DRF_DEF (_PGRAPH,_CTX_CONTROL,_TIME,_EXPIRED)
                                                 | DRF_DEF (_PGRAPH,_CTX_CONTROL,_CHID,_INVALID)
                                                 | DRF_DEF (_PGRAPH,_CTX_CONTROL,_SWITCHING,_IDLE)
                                                 | DRF_DEF (_PGRAPH,_CTX_CONTROL,_DEVICE,_ENABLED));

//******************************************************************************
// Update system canvas to current settings.  In order to include offscreen access
// system clients, calculate the canvas height based on the limit of buffer[0].
//******************************************************************************

//******************************************************************************
//    CanvasHeight = pDev.Framebuffer.Limit[0]
//                             / (pDev.Framebuffer.Depth / 8)
//                             / pDev.Framebuffer.HorizDisplayWidth;
//    if (CanvasHeight > 0x0FFF)
//        CanvasHeight = 0x0FFF;
//******************************************************************************

//******************************************************************************
// On NV1, 320x200, 320x240 and 512x384 modes aren't really accelerated by the
// hardware directly.  The canvas and clip width has to be set up for 640 pixels
// like the hardware.
//******************************************************************************

//******************************************************************************
//      switch (pDev.Framebuffer.Resolution)
//        {
//        case RESOLUTION_320X200:
//        case RESOLUTION_320X240:
//        case RESOLUTION_512X384:
//            CanvasWidth = pDev.Framebuffer.HorizDisplayWidth * 2;
//            break;
//        default:
//            CanvasWidth = pDev.Framebuffer.HorizDisplayWidth;
//            break;
//        }
//
//    grSysCanvas->xyMin    = PACK_XY(0, 0);
//    grSysCanvas->xyMax    = PACK_XY(CanvasWidth, CanvasHeight);
//
//
//    grSysCanvas->ClipMisc = 0;
//    for (i = 0; i <MAX_CLIP_REGIONS; i++)
//        {
//        grSysCanvas->xyClipMin[i] = PACK_XY(0, 0);
//        grSysCanvas->xyClipMax[i] = PACK_XY(CanvasWidth, CanvasHeight);
//        grSysCanvas->InOutClip[i] = 0;
//        }
//******************************************************************************

    //**************************************************************************
    // Init color format table
    //**************************************************************************

    InitColorFormatTable(HwDeviceExtension);

    //**************************************************************************
    // Clear out local Graphics Channel (GrTable) memory (We only use 1 channel)
    //**************************************************************************

    ClearOutGrTable(HwDeviceExtension);

    //**************************************************************************
    // Clear out local FIFO table structure (We only use 1 channel)
    //**************************************************************************

    ClearOutFifoTable(HwDeviceExtension);

    //**************************************************************************
    // Clear out local RENDEROBJECT structures (Currently allow up to 10 objects)
    //**************************************************************************

    ClearOutRenderObjects(HwDeviceExtension);

//    //*************************************************************************
//    // Clear out local local NVInfo structure
//    //*************************************************************************
//
//    ClearOutNVInfo(HwDeviceExtension);

    //**************************************************************************
    // Clear out hardware Cached Context registers and FIFO Context memory
    //**************************************************************************

    ClearOutContextMemoryAndRegisters(HwDeviceExtension);

    //**************************************************************************
    // Clear out hardware HASH TABLE memory and local hash table)
    //**************************************************************************

    ClearOutHashTables(HwDeviceExtension);

    //**************************************************************************
    // Init current channel 0 context.
    // Since we cleared out the Fifo table up above (FifoTable.InUse=INVALID),
    // an initial 'default' context will be loaded
    //**************************************************************************

    LoadChannelContext(HwDeviceExtension);

    //**************************************************************************
    // Reset ALL the interrupts that the Graphics Engine can generate.
    // Then enable all of them.
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_0,    0x11111111);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_EN_0, 0x11111111);

    //**************************************************************************
    // The second graphics interrupt register is more information for the
    // first interrupt register.  Don't bother enabling it.
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_1,    0x11111111);
    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_EN_1, 0x00000000);

    //**************************************************************************
    // We're done with the Graphics registers for now.
    // ReEnable the FIFO and DMA engines.
    //
    //      FLOWTHRU = ENABLED,  FIFO = ENABLED, DMA = ENABLED
    //      Specify Class = 0 to access graphics registers
    //
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_ENABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_ENABLED)            |
                            DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
                            DRF_NUM(_PGRAPH,_MISC,_CLASS,0));

    //**************************************************************************
    // The Graphics engine is now ready to be used!
    //**************************************************************************

    HwDeviceExtension->NvInfo.Graphics.Enabled = TRUE;


    }



//******************************************************************************
//
//  Function:   dacCalcPLL()
//
//  Routine Description:
//
//       Find closest DAC PLL parameters for a given frequency.
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//      pusCmdStream - array of commands to be interpreted.
//
//  Return Value:
//
//      The status of the operation (can only fail on a bad command); TRUE for
//      success, FALSE for failure.
//
//******************************************************************************

VOID dacCalcPClkVClkRatio(PHW_DEVICE_EXTENSION HwDeviceExtension )

    {
    PHWINFO pDev;

    pDev = &(HwDeviceExtension->NvInfo);

    //
    // changed calculation - 28 Nov 95 ibb
    // this is so that the p/v ratio is minimised especially for
    // certain customers who want to drive NTSC and thus want 25 MHz
    // out from the DAC, and not 12.5 MHz as is the case with p/v of 2.
    //
    if (pDev->Dac.VClk < 50000000)
      pDev->Dac.PClkVClkRatio = 1;
    else if (pDev->Dac.VClk < 100000000)
      pDev->Dac.PClkVClkRatio = 2;
    else if (pDev->Dac.VClk < 200000000)
      pDev->Dac.PClkVClkRatio = 4;
    else if (pDev->Dac.VClk < 400000000)
      pDev->Dac.PClkVClkRatio = 8;
    else // if (pDev->Dac.VClk < 800000000)
      pDev->Dac.PClkVClkRatio = 16;
    }

//******************************************************************************
//
//  Function:   dacCalcPLL()
//
//  Routine Description:
//
//       Find closest DAC PLL parameters for a given frequency.
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//      pusCmdStream - array of commands to be interpreted.
//
//  Return Value:
//
//      The status of the operation (can only fail on a bad command); TRUE for
//      success, FALSE for failure.
//
//******************************************************************************

VOID dacCalcPLL(PHW_DEVICE_EXTENSION HwDeviceExtension )


    {
    U032 DeltaNew;
    U032 DeltaOld;
    U032 VClk;
    U032 Freq;
    U032 M;
    U032 N;
    U032 O;
    U032 P;
    U032 MinN;
    U032 MaxN;

    PHWINFO pDev;

    //**************************************************************************
    // Get pointer to hardware information
    //**************************************************************************

    pDev = &(HwDeviceExtension->NvInfo);

    //
    // Calc VPLL.
    //
    DeltaOld = 0xFFFFFFFF;
    VClk     = pDev->Dac.VClk / 1000;
    //
    // Calculate frequencies using KHz to keep the math precision inside 32 bits.
    //
    for (P = 1; P <= 8; P <<= 1)
    {
        Freq = VClk * P;
        //
        // Bound the parameters to the internal frequencies of the DAC.
        //
        if ((Freq >= 64000) && (Freq <= 170000))
        {
            for (M = 7; M <= 12; M++)
            {
                N    = VClk * P * M / 12096;
                Freq = N * 12096 / P / M;
                if (Freq > VClk)
                    DeltaNew = Freq - VClk;
                else
                    DeltaNew = VClk - Freq;
                if (DeltaNew < DeltaOld)
                {
                    //
                    // Closer match.
                    //
                    pDev->Dac.VPllM = M;
                    pDev->Dac.VPllN = N;
                    pDev->Dac.VPllO = 1;
                    pDev->Dac.VPllP = P;
                    DeltaOld = DeltaNew;
                }
            }
        }
    }

    }




//******************************************************************************
//
//  Function:   NV1SetMode()
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//      pusCmdStream - array of commands to be interpreted.
//
//  Return Value:
//
//      The status of the operation (can only fail on a bad command); TRUE for
//      success, FALSE for failure.
//
//******************************************************************************

VOID NV1SetMode(PHW_DEVICE_EXTENSION HwDeviceExtension,
                        PMODE_ENTRY RequestedMode  )


    {
    ULONG ulCmd;
    ULONG ulPort;
    UCHAR jValue;
    USHORT usValue;
    ULONG culCount;
    ULONG ulIndex;
    ULONG ulBase;

    ULONG OutData;
    UCHAR SaveMCLK[4];
    ULONG index;
    PHWINFO NVInfo;
    ULONG DepthIndex;
    ULONG Refresh;
    ULONG DACWidth;
    ULONG i;
    ULONG m,n,o,p;
    ULONG clock;

    //**************************************************************************
    //
    // NOTE: Currently, this setmode code does NOT contain all the smart detection
    //       that the Resource Manager contains (due to time constraints), as follows:
    //
    //          - DAC Pclk/VClk ratio calculations
    //          - PRAM size (currently set to 20kb)
    //          - Framebuffer count = 0 (no double buffering)
    //          - DRAM detection (only VRAM functionality is implemented)
    //
    //       When the rest of the display driver is completed, we may want to
    //       pull in the rest of this functionality from the Resource Manager code.
    //
    //**************************************************************************

    //**************************************************************************
    // Get pointer to NVInfo structure
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    //**************************************************************************
    // Fill frame buffer with data (testing purposes)
    //**************************************************************************

//    OutData=0xffffffff;
//    for (index=0;index < ((640*480*2)/4); index++)
//        {
//        VideoPortWriteRegisterBufferUlong(
//                ((PULONG)(HwDeviceExtension->VideoMemoryAddress)) + index,
//                &OutData,1);
//        }

    //**************************************************************************
    // Enable Hardware Interrupts, Disable Software interrupts
    //**************************************************************************

    PMC_REG_WR32( HwDeviceExtension->NV1_Lin_PMC_Registers, NV_PMC_INTR_EN_0, 0x1);

    //**************************************************************************
    // Turn off BIOS Fixups (code taken from RM)
    // BIOS Fixups were never applied anyway.
    //**************************************************************************

    PRM_REG_WR32( PRM_Base, NV_PRM_INTR_EN_0, 0x00000000);
    PRM_REG_WR32( PRM_Base, NV_PRM_TRACE, 0x00000000);
    PRM_REG_WR32( PRM_Base, NV_PRM_CONFIG_0, 0x00000000);
    PRM_REG_WR32( PRM_Base, NV_PRM_IGNORE_0, 0x33333333);
    PRM_REG_WR32( PRM_Base, NV_PRM_IGNORE_1, 0x33333333);

    //**************************************************************************
    // Restore MCLK to full speed for Windows
    // This involves restoring DAC registers.
    // Here, we just save them because so far, we have not turned off VGA mode
    // Even though we only write BYTES at a time to the DAC,
    // we must write DWORDS at a time to the NV interface.
    //**************************************************************************

    //**************************************************************************
    //  Set the clock speed
    //
    // The following switch statement let's us experiment with increasing
    // the clock speed of an NV1 card.  I've only been able to increase
    // the speed of a 2Mb VRAM card to 65Mhz, and the speed of a 4Mb VRAM card
    // to 60Mhz.  As the clock is increased beyond the spec'd speed of NV1 (50Mhz),
    // you'll notice more and more graphics being drawn incorrectly with garbage.
    // At a certain point, the card just won't work.  This is somewhat useful
    // for predicting how much performance will increase when increasing the
    // clock speed.  For now, just default to the standard 50Mhz.
    //**************************************************************************

    clock = 50;

    switch (clock)
        {
        case 30:
            m=7;
            n=139;
            o=1;
            p=4;
            break;
        case 40:
            m=12;
            n=159;
            o=1;
            p=2;
            break;
        case 50:
            m=11;
            n=91;
            o=1;
            p=1;
            break;
        case 60:
            m=7;
            n=139;
            o=1;
            p=2;
            break;
        case 65:
            m=12;
            n=129;
            o=1;
            p=1;
            break;
        case 70:
            m=12;
            n=139;
            o=1;
            p=1;
            break;
        default:
            m=11;
            n=91;
            o=1;
            p=1;
            break;
        }


    OutData=(SGS_DAC_MPLL_M & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_M>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, m);

    //**************************************************************************
    // Add Delay for DAC ??
    //**************************************************************************

    OutData=(SGS_DAC_MPLL_N & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_N>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, n);

    //**************************************************************************
    // Add Delay for DAC ??
    //**************************************************************************

    OutData=(SGS_DAC_MPLL_O & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_O>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, o);

    //**************************************************************************
    // Add Delay for DAC ??
    //**************************************************************************

    OutData=(SGS_DAC_MPLL_P & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_P>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, p);



    OutData=(SGS_DAC_MPLL_M & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_M>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData = PDAC_REG_RD32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA);
    SaveMCLK[0] = (UCHAR) (OutData & 0xff);

    //**************************************************************************
    // Add Delay for DAC ??
    //**************************************************************************

    OutData=(SGS_DAC_MPLL_N & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_N>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData = PDAC_REG_RD32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA);
    SaveMCLK[1] = (UCHAR) (OutData & 0xff);

    //**************************************************************************
    // Add Delay for DAC ??
    //**************************************************************************

    OutData=(SGS_DAC_MPLL_O & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_O>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData = PDAC_REG_RD32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA);
    SaveMCLK[2] = (UCHAR) (OutData & 0xff);

    //**************************************************************************
    // Add Delay for DAC ??
    //**************************************************************************

    OutData=(SGS_DAC_MPLL_P & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_MPLL_P>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData = PDAC_REG_RD32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA);
    SaveMCLK[3] = (UCHAR) (OutData & 0xff);

    //**************************************************************************
    // Output the config register for RAM
    //
    // TO DO: Need to calculate this value in realtime!!
    //        For now, specify 20Kb
    //**************************************************************************

    NVInfo->Pram.ConfigReg = DRF_DEF(_PRAM,_CONFIG_0,_SIZE,_20KB);
    NVInfo->Pram.HashDepth = 4;
    NVInfo->Pram.RunOutMask = 0x0fff;
    NVInfo->Pram.CurrentSize = NV_PRAM_DEVICE_SIZE_20_KBYTES;

    //**************************************************************************
    // Setup the NV1 memory map
    //**************************************************************************

    NVInfo->Pram.HashTableAddr = DEVICE_BASE(NV_PRAMHT);
    NVInfo->Pram.FifoRunoutAddr = DEVICE_BASE(NV_PRAMRO);
    NVInfo->Pram.FifoContextAddr = DEVICE_BASE(NV_PRAMFC);
    NVInfo->Pram.AudioScratchAddr = DEVICE_BASE(NV_PRAMAU);
    NVInfo->Pram.AuthScratchAddr = DEVICE_BASE(NV_PRAMPW);

    //**************************************************************************
    // For now, just force VClk = 0, which will force PClkVClkRatio to be 1.
    // (There's a lot of code associated with calculating this ratio.  So for
    // now, we just default to 1.  May want to pull this code in later)
    //**************************************************************************

//    NVInfo->Dac.VClk = 0;
//    NVInfo->Dac.PClkVClkRatio = 1;

    //**************************************************************************
    //  For now, set count to zero (no double buffering for now)
    //
    //  For NV1, we want a completely flat and available dumb frame buffer.
    //  Enabling double buffering causes PRAMIN Instance memory to be
    //  split in two places (i.e. on a 2 Mb system, instance memory would occur
    //  at end of 1 Mb and at end of 2Mb)  This is fine for apps that use
    //  the VIDEO_SWITCH class and an RM, which alternates between two buffers.
    //  But the problem is that the Instance memory gets in the way when
    //  giving offscreen memory to Direct X.  So by DISABLING double buffering,
    //  we avoid this issue entirely. We can do this because 1) we're bypassing
    //  the NVidia patch architecture (for the most part) in this display driver
    //  that is...we're not using the VIDEO_SWITCH_CLASS to flip buffers, we flip
    //  buffers directly by going straight to the PFB_START_ADDRESS register. and
    //  2) We're not using Resource Manager.  This miniport does all the initialization
    //  that a Resource Manager normally would do.
    //
    //**************************************************************************

    NVInfo->Framebuffer.Count = 0;

    //**************************************************************************
    // Now get the appropriate values from the requested mode
    // and set them in the NVInfo structure
    //**************************************************************************

    switch (RequestedMode->Width)
       {
       case 1600:
          NVInfo->Framebuffer.Resolution = RESOLUTION_1600X1200;
          break;
       case 1280:
          NVInfo->Framebuffer.Resolution = RESOLUTION_1280X1024;
          break;
       case 1152:
          NVInfo->Framebuffer.Resolution = RESOLUTION_1152X864;
          break;
       case 1024:
          NVInfo->Framebuffer.Resolution = RESOLUTION_1024X768;
          break;
       case 800:
          NVInfo->Framebuffer.Resolution = RESOLUTION_800X600;
          break;
       case 640:
          NVInfo->Framebuffer.Resolution = RESOLUTION_640X480;
          break;
       default:
          NVInfo->Framebuffer.Resolution = RESOLUTION_640X480;
          break;
       }

    //**************************************************************************
    // Set the requested depth
    //**************************************************************************

    NVInfo->Framebuffer.Depth = RequestedMode->Depth;

    if (RequestedMode->Depth == 32)
       DepthIndex = 2;
    else if (RequestedMode->Depth == 16)
       DepthIndex = 1;
    else
       DepthIndex = 0;

    //**************************************************************************
    // Determine refresh rate
    //**************************************************************************

    if (RequestedMode->RefreshRate < 72)
        Refresh = 0;
    else if (RequestedMode->RefreshRate < 75)
        Refresh = 1;
    else if (RequestedMode->RefreshRate < 85)
        Refresh = 2;
    else if (RequestedMode->RefreshRate < 100)
        Refresh = 3;
    else if (RequestedMode->RefreshRate < 120)
        Refresh = 4;
    else
        Refresh = 5;

    //**************************************************************************
    // Set the display resolution and monitor timings
    //**************************************************************************

    NVInfo->Framebuffer.HorizDisplayWidth = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][1];
    NVInfo->Framebuffer.VertDisplayWidth  = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][2];

    NVInfo->Framebuffer.HorizFrontPorch = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][4];
    NVInfo->Framebuffer.HorizSyncWidth  = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][5];
    NVInfo->Framebuffer.HorizBackPorch  = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][6];
    NVInfo->Framebuffer.VertFrontPorch  = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][7];
    NVInfo->Framebuffer.VertSyncWidth   = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][8];
    NVInfo->Framebuffer.VertBackPorch   = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][9];
    NVInfo->Framebuffer.HSyncPolarity   = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][10];
    NVInfo->Framebuffer.VSyncPolarity   = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][11];
    NVInfo->Framebuffer.CSync           = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][12];
    NVInfo->Dac.VClk                    = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][13];
    NVInfo->Framebuffer.RefreshRate     = fbTimingTable[NVInfo->Framebuffer.Resolution+DepthIndex][Refresh][14];

    //**************************************************************************
    // Now that NVInfo->Dac.Vclk is initialized, calculate
    // the PClkVclk ratio and PLL values as well
    //**************************************************************************

    dacCalcPLL(HwDeviceExtension );
    dacCalcPClkVClkRatio(HwDeviceExtension );

    //**************************************************************************
    // Set 20Kb always...  (20Kb set up above)
    //**************************************************************************

    PRAM_REG_WR32( PRAM_Base, NV_PRAM_CONFIG_0, NVInfo->Pram.ConfigReg);

    //**************************************************************************
    // Output the Frame Buffer control register
    // Start at address 0
    //**************************************************************************

    OutData = 0x00000000;
    PFB_REG_WR32( PFB_Base, NV_PFB_START, OutData);

    //**************************************************************************
    // Output the Frame Buffer config0 register
    //**************************************************************************

    switch (NVInfo->Framebuffer.Resolution)
    {
        case RESOLUTION_1600X1200:
            NVInfo->Framebuffer.ConfigReg = DRF_DEF(_PFB, _CONFIG_0, _RESOLUTION, _1600_PIXELS)
                                         | DRF_DEF(_PFB, _CONFIG_0, _SCANLINE,   _NO_DUPLICATE);
            break;
        case RESOLUTION_1280X1024:
            NVInfo->Framebuffer.ConfigReg = DRF_DEF(_PFB, _CONFIG_0, _RESOLUTION, _1280_PIXELS)
                                         | DRF_DEF(_PFB, _CONFIG_0, _SCANLINE,   _NO_DUPLICATE);
            break;
        case RESOLUTION_1152X864:
            NVInfo->Framebuffer.ConfigReg = DRF_DEF(_PFB, _CONFIG_0, _RESOLUTION, _1152_PIXELS)
                                         | DRF_DEF(_PFB, _CONFIG_0, _SCANLINE,   _NO_DUPLICATE);
            break;
        case RESOLUTION_1024X768:
            NVInfo->Framebuffer.ConfigReg = DRF_DEF(_PFB, _CONFIG_0, _RESOLUTION, _1024_PIXELS)
                                         | DRF_DEF(_PFB, _CONFIG_0, _SCANLINE,   _NO_DUPLICATE);
            break;
        case RESOLUTION_800X600:
            NVInfo->Framebuffer.ConfigReg = DRF_DEF(_PFB, _CONFIG_0, _RESOLUTION, _800_PIXELS)
                                         | DRF_DEF(_PFB, _CONFIG_0, _SCANLINE,   _NO_DUPLICATE);
            break;
        case RESOLUTION_640X480:
            NVInfo->Framebuffer.ConfigReg = DRF_DEF(_PFB, _CONFIG_0, _RESOLUTION, _640_PIXELS)
                                         | DRF_DEF(_PFB, _CONFIG_0, _SCANLINE,   _NO_DUPLICATE);
            break;
    }

    //**************************************************************************
    //  For now, we don't enable a second buffer (Count = 0 always, set up above)
    //
    //  For NV1, we want a completely flat and available dumb frame buffer.
    //  Enabling double buffering causes PRAMIN Instance memory to be
    //  split in two places (i.e. on a 2 Mb system, instance memory would occur
    //  at end of 1 Mb and at end of 2Mb)  This is fine for apps that use
    //  the VIDEO_SWITCH class and an RM, which alternates between two buffers.
    //  But the problem is that the Instance memory gets in the way when
    //  giving offscreen memory to Direct X.  So by DISABLING double buffering,
    //  we avoid this issue entirely. We can do this because 1) we're bypassing
    //  the NVidia patch architecture (for the most part) in this display driver
    //  that is...we're not using the VIDEO_SWITCH_CLASS to flip buffers, we flip
    //  buffers directly by going straight to the PFB_START_ADDRESS register. and
    //  2) We're not using Resource Manager.  This miniport does all the initialization
    //  that a Resource Manager normally would do.
    //
    //**************************************************************************

    if (NVInfo->Framebuffer.Count == 2)
        NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _SECOND_BUFFER, _ENABLED);
    else
        NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _SECOND_BUFFER, _DISABLED);

    //**************************************************************************
    // PClkVClk Ratio is hard coded up above for now....
    //**************************************************************************

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PIXEL_DEPTH, _32_BITS);
            PFB_FLD_WR_DRF_NUM(PFB_Base,_PFB, _DELAY_0, _SYNC, 16 / NVInfo->Dac.PClkVClkRatio + 3);
            break;
        case 16:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PIXEL_DEPTH, _16_BITS);
            PFB_FLD_WR_DRF_NUM(PFB_Base,_PFB, _DELAY_0, _SYNC, 16 / NVInfo->Dac.PClkVClkRatio + 3);
            break;
        case 8:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PIXEL_DEPTH, _8_BITS);
            PFB_FLD_WR_DRF_NUM(PFB_Base,_PFB, _DELAY_0, _SYNC, 15 / NVInfo->Dac.PClkVClkRatio + 3);
        }

    //**************************************************************************
    // Page height was set to zero in GetNVInfo() - disabled
    //**************************************************************************

    NVInfo->Framebuffer.ConfigReg |= DRF_NUM(_PFB, _CONFIG_0, _PAGE_HEIGHT, NVInfo->Framebuffer.ConfigPageHeight);

    //**************************************************************************
    // Currently , PClkVClk Ratio is set to 1 up above...
    //**************************************************************************

    switch (NVInfo->Dac.PClkVClkRatio)
    {
        case 1:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PCLK_VCLK_RATIO, _1_TO_1);
            break;
        case 2:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PCLK_VCLK_RATIO, _2_TO_1);
            break;
        case 4:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PCLK_VCLK_RATIO, _4_TO_1);
            break;
        case 8:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PCLK_VCLK_RATIO, _8_TO_1);
            break;
        case 16:
            NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _PCLK_VCLK_RATIO, _16_TO_1);
            break;
    }

    //**************************************************************************
    // No stereoscopic stuff..
    //**************************************************************************

    NVInfo->Framebuffer.ConfigReg |= DRF_DEF(_PFB, _CONFIG_0, _STEREOSCOPIC, _DISABLED);

    //**************************************************************************
    // Finally output the ConfigReg value
    //**************************************************************************

    PFB_REG_WR32( PFB_Base, NV_PFB_CONFIG_0,NVInfo->Framebuffer.ConfigReg );


    //**************************************************************************
    // Set LOW_MARK for VRAM type adapters only
    //...May want to pull in the DRAM code later!!!!
    //**************************************************************************

    OutData = (NV_PFB_CONFIG_1_FIFO_LOW_MARK_512_BITS);
    PFB_REG_WR32( HwDeviceExtension->NV1_Lin_PFB_Registers, NV_PFB_CONFIG_1, OutData);

    //**************************************************************************
    // Output for Monitor timings/Framebuffer display registers
    //**************************************************************************

    PFB_REG_WR32(PFB_Base,NV_PFB_HOR_FRNT_PORCH,NVInfo->Framebuffer.HorizFrontPorch);
    PFB_REG_WR32(PFB_Base,NV_PFB_HOR_SYNC_WIDTH,NVInfo->Framebuffer.HorizSyncWidth);
    PFB_REG_WR32(PFB_Base,NV_PFB_HOR_BACK_PORCH,NVInfo->Framebuffer.HorizBackPorch);
    PFB_REG_WR32(PFB_Base,NV_PFB_HOR_DISP_WIDTH,NVInfo->Framebuffer.HorizDisplayWidth);
    PFB_REG_WR32(PFB_Base,NV_PFB_VER_FRNT_PORCH,NVInfo->Framebuffer.VertFrontPorch);
    PFB_REG_WR32(PFB_Base,NV_PFB_VER_SYNC_WIDTH,NVInfo->Framebuffer.VertSyncWidth);
    PFB_REG_WR32(PFB_Base,NV_PFB_VER_BACK_PORCH,NVInfo->Framebuffer.VertBackPorch);
    PFB_REG_WR32(PFB_Base,NV_PFB_VER_DISP_WIDTH,NVInfo->Framebuffer.VertDisplayWidth);

    //**************************************************************************
    // DAC Load Width/Depth
    //**************************************************************************

    OutData=(SGS_DAC_CONFIG_0 & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);


    OutData=((SGS_DAC_CONFIG_0>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    //**************************************************************************
    // Determine the DAC Width first...
    //**************************************************************************

    DACWidth = PFB_REG_RD_DRF(PFB_Base,_PFB,_BOOT_0,_DAC_WIDTH);

    switch (DACWidth)
        {
        case NV_PFB_BOOT_0_DAC_WIDTH_16_BIT:
            NVInfo->Dac.InputWidth =16;
            OutData = (SGS_DAC_CONFIG_0_PORT_WIDTH_16BIT << 2) ;
            break;
        case NV_PFB_BOOT_0_DAC_WIDTH_32_BIT:
            NVInfo->Dac.InputWidth =32;
            OutData = (SGS_DAC_CONFIG_0_PORT_WIDTH_32BIT << 2) ;
            break;
        case NV_PFB_BOOT_0_DAC_WIDTH_64_BIT:
            NVInfo->Dac.InputWidth =64;
            OutData = (SGS_DAC_CONFIG_0_PORT_WIDTH_64BIT << 2) ;
            break;
        }

    //**************************************************************************
    // Then set the pixel depth appropriately
    //**************************************************************************

    switch (NVInfo->Framebuffer.Depth)
        {
        case 8:
            OutData |= (SGS_DAC_CONFIG_0_PIXEL_DEPTH_8BIT) |
                       (SGS_DAC_CONFIG_0_IDC_MODE_INDEX << 6);
            break;
        case 16:
            OutData |= (SGS_DAC_CONFIG_0_PIXEL_DEPTH_16BIT) |
                       (SGS_DAC_CONFIG_0_IDC_MODE_DIRECT_COLOR << 6);
            break;
        case 32:
            OutData |= (SGS_DAC_CONFIG_0_PIXEL_DEPTH_32BIT) |
                       (SGS_DAC_CONFIG_0_IDC_MODE_DIRECT_COLOR <<6);
            break;
        }

    //**************************************************************************
    // Set DAC_CONFIG_0 register
    //**************************************************************************

    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // DAC Load PCLK/VCLK ratio
    //**************************************************************************

    OutData=(SGS_DAC_CONFIG_1 & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_CONFIG_1>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    //**************************************************************************
    // Currently , PClkVClk Ratio is set to 1 up above...
    //**************************************************************************

    switch (NVInfo->Dac.PClkVClkRatio)
    {
        case 1:
            OutData=( SGS_DAC_CONFIG_1_VCLK_DIVIDE_BY_1 << 0) | (SGS_DAC_CONFIG_1_VCLK_IMPEDENCE_LOW << 3);
            break;
        case 2:
            OutData=( SGS_DAC_CONFIG_1_VCLK_DIVIDE_BY_2 << 0) | (SGS_DAC_CONFIG_1_VCLK_IMPEDENCE_LOW << 3);
            break;
        case 4:
            OutData=( SGS_DAC_CONFIG_1_VCLK_DIVIDE_BY_4 << 0) | (SGS_DAC_CONFIG_1_VCLK_IMPEDENCE_LOW << 3);
            break;
        case 8:
            OutData=( SGS_DAC_CONFIG_1_VCLK_DIVIDE_BY_8 << 0) | (SGS_DAC_CONFIG_1_VCLK_IMPEDENCE_LOW << 3);
            break;
        case 16:
            OutData=( SGS_DAC_CONFIG_1_VCLK_DIVIDE_BY_16 << 0) | (SGS_DAC_CONFIG_1_VCLK_IMPEDENCE_LOW << 3);
            break;
    }

    PDAC_REG_WR32(PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // Prepare to output green power down level register
    //**************************************************************************

    if (NVInfo->Framebuffer.HSyncPolarity)
        OutData = DRF_DEF(_PFB,_GREEN_0,_POLAR_HSYNC,_POSITIVE);
    else
        OutData = DRF_DEF(_PFB,_GREEN_0,_POLAR_HSYNC,_NEGATIVE);

    if (NVInfo->Framebuffer.VSyncPolarity)
        OutData |= DRF_DEF(_PFB,_GREEN_0,_POLAR_VSYNC,_POSITIVE);
    else
        OutData |= DRF_DEF(_PFB,_GREEN_0,_POLAR_VSYNC,_NEGATIVE);

    if (NVInfo->Framebuffer.CSync)
        OutData |= DRF_DEF(_PFB,_GREEN_0,_CSYNC,_ENABLED);
    else
        OutData |= DRF_DEF(_PFB,_GREEN_0,_CSYNC,_DISABLED);

    //**************************************************************************
    // Make sure video is re-enabled
    //**************************************************************************

    OutData |= DRF_DEF(_PFB,_GREEN_0,_LEVEL,_VIDEO_ENABLED);

    //**************************************************************************
    // Green register
    //**************************************************************************

    PFB_REG_WR32( PFB_Base, NV_PFB_GREEN_0, OutData);

    //**************************************************************************
    // Pixel Mask Register
    //**************************************************************************

    OutData= 0xff;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_PIXEL_MASK, OutData);


    //**************************************************************************
    // DAC PLL values
    //**************************************************************************

    OutData=(SGS_DAC_VPLL_M & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_VPLL_M>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= NVInfo->Dac.VPllM;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);



    OutData=(SGS_DAC_VPLL_N & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_VPLL_N>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= NVInfo->Dac.VPllN;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);



    OutData=(SGS_DAC_VPLL_O & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_VPLL_O>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= NVInfo->Dac.VPllO;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);



    OutData=(SGS_DAC_VPLL_P & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_VPLL_P>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= NVInfo->Dac.VPllP;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // Init palette registers (for 8bpp modes only)
    //**************************************************************************

    if (NVInfo->Framebuffer.Depth == 8)
        NV1_InitPalette(HwDeviceExtension);


    //**************************************************************************
    // Init hardware cursor
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_CTRL & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_CURSOR_CTRL>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= SGS_DAC_CURSOR_CTRL_DISPLAY_TWO_COLOR_XOR;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // Set cursor color 1
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_COLOR_1_RED & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_1_RED>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData=(SGS_DAC_CURSOR_COLOR_1_GREEN & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_1_GREEN>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData=(SGS_DAC_CURSOR_COLOR_1_BLUE & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_1_BLUE>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // Set cursor color 2
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_COLOR_2_RED & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_2_RED>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0xff;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData=(SGS_DAC_CURSOR_COLOR_2_GREEN & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_2_GREEN>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0xff;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData=(SGS_DAC_CURSOR_COLOR_2_BLUE & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_2_BLUE>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0xff;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // Set cursor color 3
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_COLOR_3_RED & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_3_RED>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData=(SGS_DAC_CURSOR_COLOR_3_GREEN & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_3_GREEN>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData=(SGS_DAC_CURSOR_COLOR_3_BLUE & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
    OutData=((SGS_DAC_CURSOR_COLOR_3_BLUE>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
    OutData= 0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // Set cursor image plane 0
    //**************************************************************************

// Someone already loaded a cursor image?? BIOS?
//    OutData=(SGS_DAC_CURSOR_PLANE_0_WRITE_0 & 0xff);
//    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
//
//    OutData=((SGS_DAC_CURSOR_PLANE_0_WRITE_0>>8) & 0xff);
//    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
//
//    for (i=0;i<128;i++)
//        {
//        OutData = NVInfo->Dac.CursorImagePlane[0][i];
//        PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);
//        }
//
//    //**************************************************************************
//    // Set cursor image plane 1
//    //**************************************************************************
//
//    OutData=(SGS_DAC_CURSOR_PLANE_1_WRITE_0 & 0xff);
//    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);
//
//    OutData=((SGS_DAC_CURSOR_PLANE_1_WRITE_0>>8) & 0xff);
//    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);
//
//    for (i=0;i<128;i++)
//        {
//        OutData = NVInfo->Dac.CursorImagePlane[1][i];
//        PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);
//        }

    //**************************************************************************
    // Set cursor position
    //**************************************************************************

    OutData=(SGS_DAC_CURSOR_POS_X_LO & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_CURSOR_POS_X_LO>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData=512;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData>>8);


    OutData=(SGS_DAC_CURSOR_POS_Y_LO & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_CURSOR_POS_Y_LO>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData=384;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData>>8);


    //**************************************************************************
    // Set palette register (DAC State Writable)?
    //**************************************************************************

    OutData=(SGS_DAC_RGB_EXT_PAL_CTRL & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_RGB_EXT_PAL_CTRL>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= 0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);


    //**************************************************************************
    // DAC Power Management values
    //**************************************************************************

    OutData=(SGS_DAC_POWER_MGMNT_A & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_POWER_MGMNT_A>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= 0x88;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);



    OutData=(SGS_DAC_POWER_MGMNT_B & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_POWER_MGMNT_B>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= 0x0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);



    OutData=(SGS_DAC_POWER_MGMNT_C & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_POWER_MGMNT_C>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= 0x0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);



    OutData=(SGS_DAC_VBLANK_CTRL & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_VBLANK_CTRL>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= 0x0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    //**************************************************************************
    // Disable SERR
    //**************************************************************************

    OutData = PBUS_REG_RD32( HwDeviceExtension->NV1_Lin_PBUS_Registers, NV_PBUS_PCI_NV_1);
    OutData = (OutData & (~0x00000100));    // SERR DISABLED ???  Why?
    PBUS_REG_WR32( HwDeviceExtension->NV1_Lin_PBUS_Registers, NV_PBUS_PCI_NV_1, OutData);

    //**************************************************************************
    // Enable HARDWARE interrupts throught Pin 1 (A)
    // Enable INTA_Hardware interrupts, NO SOFTWARE Interrupts allowed
    //**************************************************************************

    OutData = 0x1;
    PMC_REG_WR32( HwDeviceExtension->NV1_Lin_PMC_Registers, NV_PMC_INTR_EN_0, OutData);

    } // end NV1SetMode()


//******************************************************************************
//
//  Function:   SetObjectCachedCtxRegs()
//
//  Routine Description:
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

VOID SetObjectCachedCtxRegs(PHW_DEVICE_EXTENSION HwDeviceExtension,
                            ULONG CurrentSubChannel, ULONG ObjectName, ULONG NewContext)


    {
    ULONG SubChannelCount;

    //**************************************************************************
    // Update object's context in FIFO's cached context registers
    //**************************************************************************

    for (SubChannelCount = 0; SubChannelCount < 8; SubChannelCount++)
        {
        if (HwDeviceExtension->FifoTable.ObjectStack[SubChannelCount] == ObjectName )
            {
            PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(SubChannelCount),NewContext);

            //******************************************************************
            // If this is the current subchannel, tell engine the context
            // has been dirtied and needs to be updated
            // We shouldn't have to specify the SubChannel here because
            // that's done by hardware automatically, when we write out
            // the method???(Verify)
            //******************************************************************

            if (SubChannelCount == CurrentSubChannel)
                {
                PFIFO_FLD_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL1,_CTX,_DIRTY);

                //**************************************************************
                // This should be CHANGED so that object swapping works !!!!!
                //**************************************************************

                PFIFO_FLD_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL1,_OBJECT,_CHANGED);

                }

            }

        }

    }



//******************************************************************************
//
//  Function:   SetObjectHash()
//
//  Routine Description:
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

VOID SetObjectHash(PHW_DEVICE_EXTENSION HwDeviceExtension,
                    ULONG ObjectName, ULONG Context)


    {
    ULONG   HashValue;
    ULONG   HashEntry;
    ULONG   i;
    ULONG  *HashTablePtr;


    //**************************************************************************
    // Get ptr to array of hashed objects (object names)
    //**************************************************************************

    HashTablePtr = &(HwDeviceExtension->HashTable[0]);

    //**************************************************************************
    // Get hash value for this object in channel 0
    //**************************************************************************

    HashValue = FIFO_HASH(ObjectName,0);

    //**************************************************************************
    // Find first empty entry (FOR NOW WE ASSUME HASH_DEPTH = 4) UPDATE !!!!!!!
    //**************************************************************************

    for (i=0; i<HASH_DEPTH; i++)
        {
        HashEntry = HASH_ENTRY(HashValue,i);

        //**********************************************************************
        // Need to multiply HashEntry by 2 because HashEntry is only an index
        // from 0 - 255.  That is, when searching through the hash table, each
        // object context takes up 8 bytes.  However, C compiler will already
        // convert address into a ULONG address.  So end result is:
        //  HashEntry (aligned on 8 byte bndry) =  HashTablePtr + (HashEntry*4) *2.
        //**********************************************************************

        if ( *(HashTablePtr+HashEntry*2) == (ULONG)NULL)
            break;

        //**********************************************************************
        // If Object already exists, then use this SAME entry.
        //**********************************************************************

        if ( *(HashTablePtr+HashEntry*2) == ObjectName)
            break;

        }

    //**************************************************************************
    // If we did not find an empty entry, choose one randomly (use 2 for now)
    //**************************************************************************

    if (i == HASH_DEPTH)
        HashEntry = HASH_ENTRY(HashValue,2);

    //**************************************************************************
    // Add object to our local hash table array
    //**************************************************************************

    *(HashTablePtr+HashEntry*2) = ObjectName;

    //**************************************************************************
    // Set object name in hardware HASH table memory
    //**************************************************************************

    PRAMHT_REG_WR32(PRAMHT_Base,DEVICE_BASE(NV_PRAMHT) + HashEntry*8, ObjectName);

    //**************************************************************************
    // Set object context in hardware HASH table memory
    // No lie, using channel 0
    //**************************************************************************

    PRAMHT_REG_WR32(PRAMHT_Base,DEVICE_BASE(NV_PRAMHT) + HashEntry*8+4,Context & 0x00ffffff);
    }


//******************************************************************************
//
//  Function:   FifoResend()
//
//  Routine Description:
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

VOID FifoResend(PHW_DEVICE_EXTENSION HwDeviceExtension)


    {
    ULONG Data;
    ULONG Method;
    ULONG caches, push0, pull0;

    //**************************************************************************
    // THIS FUNCTION HAS NOT YET BEEN TESTED
    //**************************************************************************

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable CACHE first
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // Resend Data
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE0_GET,0x00000000);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE0_PUT,0x00000004);

    //**************************************************************************
    // Channel 0
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE0_PUSH1, 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE0_CTX(0), 0);

    //**************************************************************************
    // Wait for it to drain
    //**************************************************************************

    while (PFIFO_REG_RD_DRF(PFIFO_Base,_PFIFO,_CACHE0_STATUS,_LOW_MARK) ==
                                                NV_PFIFO_CACHE0_STATUS_LOW_MARK_NOT_EMPTY)
        {
        if (PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_INTR_0) &  0x11111011)
            VideoDebugPrint((1, "Error - FifoResend."));  /// oooopppssss
        }

    //**************************************************************************
    // Disable CACHE0
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE0_PULL0,_ACCESS, _DISABLED);

    //**************************************************************************
    // Re-Enable CACHE1
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_ENABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_ENABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_ENABLED);


    }

//******************************************************************************
//
//  Function:   ClearOutGrTable()
//
//  Routine Description:
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

VOID ClearOutGrTable(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    PGRAPHICSCHANNEL Ptr;

    //**************************************************************************
    // Get ptr to our Graphics Channel
    //**************************************************************************

    Ptr = (PGRAPHICSCHANNEL) &(HwDeviceExtension->GrTable);

    Ptr->NotifyObject       = NULL;
    Ptr->Notify             = (ULONG)NULL;
    Ptr->CurrentRop         = NULL;
    Ptr->CurrentBeta        = NULL;
    Ptr->CurrentColorKey    = NULL;
    Ptr->CurrentPlaneMask   = NULL;
    Ptr->CurrentClip        = NULL;
    Ptr->CurrentPattern     = NULL;
    Ptr->CurrentPatch       = NULL;
    Ptr->PatchList          = NULL;
    Ptr->PatchCount         = (ULONG)NULL;
    Ptr->Exceptions         = (ULONG)NULL;
    }

//******************************************************************************
//
//  Function:   ClearOutFifoTable()
//
//  Routine Description:
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

VOID ClearOutFifoTable(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG i;
    PFIFO Ptr;

    //**************************************************************************
    // Get ptr to our FIFO structure
    //**************************************************************************

    Ptr = (PFIFO) &(HwDeviceExtension->FifoTable);

    //**************************************************************************
    // Initialize FIFO table structure
    //**************************************************************************

    Ptr->InUse = INVALID;
    Ptr->ChID = INVALID;                    // We will always use channel 0 (for now)
    Ptr->ObjectCount = 0;                   // No objects loaded yet

    //**************************************************************************
    // Clear out the objects
    //**************************************************************************

    for (i=0; i<NUM_SUBCHANNELS; i++)
        Ptr->ObjectStack[i] = (ULONG)NULL;  // 0 means not used

    }



//******************************************************************************
//
//  Function:   ClearOutNVInfo()
//
//  Routine Description:
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

VOID ClearOutNVInfo(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG i;
    ULONG Length;
    PULONG Ptr;

    //**************************************************************************
    // Get length of HWINFO object structure (in Dwords)
    //**************************************************************************

    Length = sizeof(HWINFO)/4;

    //**************************************************************************
    // Get Ptr to NVInfo structure and fill it with zeros
    //**************************************************************************

    Ptr = (PULONG)&(HwDeviceExtension->NvInfo);

    for (i=0; i<Length; i++)
            {
            *Ptr = 0;
            Ptr++;
            }


    }


//******************************************************************************
//
//  Function:   ClearOutHashTables()
//
//  Routine Description:
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

VOID ClearOutHashTables(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG i;
    ULONG caches, push0, pull0;

    //**************************************************************************
    // Save FIFO cache1 state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable CACHE1 first
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

   //***************************************************************************
    // Clear out local hash table with zeros (currently 1k worth)
    //**************************************************************************

    for (i=0; i < 256 * HASH_DEPTH; i++)
        HwDeviceExtension->HashTable[i] = 0;

    //**************************************************************************
    // Clear out hash virtual registers ( 128 of these , one per channel ?)
    // When the Hash Engine looks thru Hash Table memory and it finds
    // an entry that matches the one in the virtual register, then
    // the physical information about the object will be written to
    // the corresponding hash physical register.  Otherwise, zero will be written
    //      -------------------------------------------------------------
    //  n  |                   Object Handle                             |
    //      -------------------------------------------------------------
    //**************************************************************************

    for (i=0; i < 128  ; i++)
        PRAM_REG_WR32(PRAM_Base,NV_PRAM_HASH_VIRTUAL(i),0 );


    //**************************************************************************
    // Clear out hardware hash memory (current 8K worth)
    // There are 256 structures, each containing 4 entries which are each 8 bytes
    // For NV1 (currently), memory_size = 256 * HASH_DEPTH * 8 = 8k
    //      --------------------------------------------------------------
    //     |                    Object Handle                             |
    //  n   --------------------------------------------------------------
    //     | Channel ID       Phys Device Address   Phys Instance Address |
    //      --------------------------------------------------------------
    //**************************************************************************

    for (i=0; i < (256 * HASH_DEPTH * 8) ; i+=8) // Advance 8 bytes each time
        {
        PRAMHT_REG_WR32(PRAMHT_Base,DEVICE_BASE(NV_PRAMHT)+i,0 );
        PRAMHT_REG_WR32(PRAMHT_Base,DEVICE_BASE(NV_PRAMHT)+i+4,0 );
        }

    //**************************************************************************
    // Restore saved FIFO cache1 state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);


    }

//******************************************************************************
//
//  Function:   ClearOutContextMemoryAndRegisters()
//
//  Routine Description:
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

VOID ClearOutContextMemoryAndRegisters(PHW_DEVICE_EXTENSION HwDeviceExtension)


    {
    ULONG CurrentChID;
    ULONG Data;
    ULONG CurrentInstance;
    ULONG InitContextPtr;
    ULONG ContextPtr;
    ULONG caches;
    ULONG cache1push0;
    ULONG cache1pull0;

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Fifo access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // Tell Cache1 to use Channel 0
    //**************************************************************************

    CurrentChID=0;
    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_CACHE1_PUSH1,_CHID, CurrentChID);

    //**************************************************************************
    // Clean out FIFO CONTEXT memory
    // Currently, There are a 128 total possible channels, each channel
    // contains 8 contexts, each 4 bytes.  Memory_size = 128 * 8 * 4;
    // However, since we are only using channel 0, just clear out channel 0.
    //**************************************************************************

    CurrentChID=0;
    InitContextPtr = DEVICE_BASE(NV_PRAMFC) + (CurrentChID*32) ;
    for (ContextPtr=InitContextPtr; ContextPtr < (InitContextPtr + 32); ContextPtr+=4)
        PRAMFC_REG_WR32(PRAMFC_Base, ContextPtr , 0);

    //**************************************************************************
    // Initialize Cache1 to use subchannel zero
    //**************************************************************************

    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_CACHE1_PULL1,_SUBCHANNEL,0);

    //**************************************************************************
    // Set PUT and GET pointers to address 0
    //**************************************************************************

    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_CACHE1_PUT,_ADDRESS,0);
    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_CACHE1_GET,_ADDRESS,0);

    //**************************************************************************
    // Clear out CACHED CONTEXT registers
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE0_CTX(0), 0);

    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(0), 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(1), 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(2), 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(3), 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(4), 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(5), 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(6), 0);
    PFIFO_REG_WR32(PFIFO_Base,NV_PFIFO_CACHE1_CTX(7), 0);

    //**************************************************************************
    // Check if Context has changed.
    // (Make sure that Chroma, Beta, Plane_Mask, UserClip, Alpha,
    // and Mono have not changed).
    // For now, we FORCE context to be loaded up above, so it should be the same..
    //**************************************************************************

//    Data = PGRAPH_REG_RD32(PGRAPH_Base, NV_PGRAPH_CTX_SWITCH);
//    CurrentInstance = DRF_VAL(_PGRAPH,_CTX_SWITCH,_INSTANCE,Data);
//
//    if ( (CurrentContext & 0xffff) != CurrentInstance)
//            ASSERTDD(FALSE, "ClearOutContextMemory.");  /// oooopppssss

    //**************************************************************************
    // Make sure that channel is the same
    //**************************************************************************

//    CurrentChID = PGRAPH_REG_RD_DRF(PGRAPH_Base,_PGRAPH,_MISC,_CLASS);
//    if (CurrentChID != ((CurrentContext >> 16) & 0x7f))
//        {
//          ASSERTDD(FALSE, "ClearOutContextMemory.");  /// oooopppssss
//        PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE0_PULL1,_OBJECT,_CHANGED);
//        }

    //**************************************************************************
    // Check object_changed bit for CACHE1
    // Set CACHE0 bit to have same value.
    //**************************************************************************

//    Data = PFIFO_REG_RD32(PFIFO_Base,NV_PFIFO_CACHE1_PULL1);
//
//    if (Data)
//        PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE0_PULL1,_OBJECT,_CHANGED);
//    else
//        PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE0_PULL1,_OBJECT,_UNCHANGED);

    //**************************************************************************
    // Restore saved FIFO state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);


    }


//******************************************************************************
//
//  Function:   LoadChannelContext()
//
//  Routine Description:
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

VOID LoadChannelContext(PHW_DEVICE_EXTENSION HwDeviceExtension)


    {
    ULONG Data;
    ULONG Result;
    ULONG CurrentInstance;
    ULONG SavedGraphicsState;
    ULONG CurrentChID;
    ULONG caches;
    ULONG cache0push0;
    ULONG cache0pull0;
    ULONG cache1push0;
    ULONG cache1pull0;
    ULONG XYLogicMisc1;
    PGRAPHICSCHANNEL GrTablePtr;
    ULONG i;

    //**************************************************************************
    // Get pointer to graphics channel
    //**************************************************************************

    GrTablePtr = &(HwDeviceExtension->GrTable);

    //**************************************************************************
    // Saved FIFO state
    //**************************************************************************

    caches = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHES);
    cache0push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE0_PUSH0);
    cache0pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE0_PULL0);
    cache1push0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0);
    cache1pull0 = PFIFO_REG_RD32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0);

    //**************************************************************************
    // Disable Fifo access
    //**************************************************************************

    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHES,_REASSIGN,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE0_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE0_PULL0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PUSH0,_ACCESS,_DISABLED);
    PFIFO_REG_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL0,_ACCESS,_DISABLED);

    //**************************************************************************
    // Force Cache 0 and Cache 1 to be set for channel 0
    //**************************************************************************

    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_CACHE0_PUSH1,_CHID, 0);
    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_CACHE1_PUSH1,_CHID, 0);

    //**************************************************************************
    // Default Cache 1 to be set for subchannel 0 also.
    // Signal that the object is DIRTY and the object has CHANGED
    // since we haven't loaded it yet.  We don't bother setting
    // the subchannel for Cache0 because that's mainly used by the Resource Mgr.
    // Make sure the subsequent macros are the FLD type, to preserve what's
    // currently in the register.
    //**************************************************************************

    PFIFO_REG_WR_DRF_NUM(PFIFO_Base,_PFIFO,_CACHE1_PULL1,_SUBCHANNEL, 0);
    PFIFO_FLD_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL1,_CTX,_DIRTY);
    PFIFO_FLD_WR_DRF_DEF(PFIFO_Base,_PFIFO,_CACHE1_PULL1,_OBJECT,_CHANGED);

    //**************************************************************************
    // Check for any outstanding graphics exceptions
    //**************************************************************************
//
//    while (PGRAPH_REG_RD32(PGRAPH_Base, NV_PGRAPH_INTR_0) & 0x11111011)
//        ASSERTDD(FALSE, "LoadChannelContext.");      // Uh oh....shouldn't happen

    //**************************************************************************
    // Reset graphics state engine.  I'm not sure if this is necessary?
    // but I'm including it because the Resource Manager uses it.
    //**************************************************************************

    if (PGRAPH_REG_RD32(PGRAPH_Base, NV_PGRAPH_DEBUG_1) & 0x01)
        PGRAPH_FLD_WR_DRF_DEF(PGRAPH_Base,_PGRAPH,_DEBUG_0,_STATE,_NORMAL);


    //**************************************************************************
    // Init Context Control register for the first time
    // Instance is zero, and Channel is currently invalid
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CTX_CONTROL,
                               DRF_DEF(_PGRAPH, _CTX_CONTROL, _MINIMUM_TIME, _2MS)
                             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _TIME,         _EXPIRED)
                             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _CHID,         _INVALID)
                             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _SWITCHING,    _IDLE)
                             | DRF_DEF(_PGRAPH, _CTX_CONTROL, _DEVICE,       _ENABLED));


    //**************************************************************************
    // Note: I'm skipping the grTextureFixup() call because
    //       we won't be using textures at this point.
    //**************************************************************************

    //**************************************************************************
    // Init Static State to Default value first, just to be safe. Necessary?
    // ROP, Beta, ColorKey, PlaneMask, Clip, and Pattern
    //**************************************************************************

//    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ROP3, 0xcc);     // Source Copy
//    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_BETA,0x07f8 );
//    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PLANE_MASK, 0);
//    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CHROMA, 0);

    //**************************************************************************
    // Load STATIC STATE as follows:
    // ROP, Beta, ColorKey, PlaneMask, Clip, and Pattern
    //**************************************************************************

    if (GrTablePtr->CurrentRop)
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ROP3, GrTablePtr->CurrentRop->Rop3);

    if (GrTablePtr->CurrentBeta)
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_BETA, GrTablePtr->CurrentBeta->Beta);

    if (GrTablePtr->CurrentClip)
        {
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_XMIN, GrTablePtr->CurrentClip->xClipMin);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_YMIN, GrTablePtr->CurrentClip->yClipMin);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_XMAX, GrTablePtr->CurrentClip->xClipMax);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_YMAX, GrTablePtr->CurrentClip->yClipMax);
        XYLogicMisc1 = (DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_XCMIN, _USERMIN)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_XCMAX, _USERMAX)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_YCMIN, _USERMIN)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_YCMAX, _USERMAX)) ;
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_XY_LOGIC_MISC1,XYLogicMisc1);


        }

    if (GrTablePtr->CurrentPlaneMask)
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PLANE_MASK, GrTablePtr->CurrentPlaneMask->Color);

    if (GrTablePtr->CurrentColorKey)
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CHROMA, GrTablePtr->CurrentColorKey->Color);

    if (GrTablePtr->CurrentPattern)
        {
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR0_0, GrTablePtr->CurrentPattern->PattColor0);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR0_1, GrTablePtr->CurrentPattern->PattColor0Alpha);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR1_0, GrTablePtr->CurrentPattern->PattColor1);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATT_COLOR1_1, GrTablePtr->CurrentPattern->PattColor1Alpha);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATTERN(0), GrTablePtr->CurrentPattern->Pattern0);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATTERN(1), GrTablePtr->CurrentPattern->Pattern1);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_PATTERN_SHAPE, GrTablePtr->CurrentPattern->PatternShape);
        }

    //**************************************************************************
    // Load RENDERING STATE as follows:
    //**************************************************************************

    if (GrTablePtr->CurrentPatch)
        {

        //**********************************************************************
        // STATIC STATE vs. RENDERING STATE
        //
        // STATIC STATE values are those attributes associated with the patch
        // and are specified when creating a patch.
        //
        // RENDERING STATE refers to the current attributes that are present
        // when an object is currently rendering.  That is, if a blit is
        // is interrupted, additional registers need to be saved and restored
        // such as the current source color or BETA ram values, so that the
        // rendering state can be restored EXACTLY the way it was prior to
        // being context switched.  Since this Miniport driver will not handle
        // context switching (We are in full control of context switching here),
        // we do not have to worry about saving/restoring them.(Need to init, though?)
        //
        // So for now, we do nothing.  As long as we keep track of the
        // static state objects, we should be fine.
        //**********************************************************************

        }

    else

        {
        //**********************************************************************
        // Init default values for the rendering state, since no patch was present
        //**********************************************************************

        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CANVAS_MIN, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CANVAS_MAX, 0x1fff1fff);

        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CLIP_MISC, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CLIP0_MIN, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CLIP1_MIN, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CLIP0_MAX, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CLIP1_MAX, 0x00000000);

        //**********************************************************************
        // TO DO!!
        //
        // NOTE: We need to determine what mode we are in so we can ENABLE/DISABLE
        //       DAC bypass appropriately !!!
        //
        // Currently set as follows:  DITHERING enabled and DACBYPASS enabled
        //
        //**********************************************************************

        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CANVAS_MISC, 0x00110001);

        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_SOURCE_COLOR, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MONO_COLOR0, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MONO_COLOR1, 0x00000000);

        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_XMIN, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_YMIN, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_XMAX, 0x00007fff);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_UCLIP_YMAX, 0x00007fff);

        for (i=0;i < 14;i++)
            PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_BETA_RAM(i), 0x00000000);

        for (i=0;i < 18;i++)
            {
            PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_X_RAM(i), 0x00000000);
            PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_Y_RAM(i), 0x00000000);
            }

        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_ICLIP_XMAX, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_ABS_ICLIP_YMAX, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_XY_LOGIC_MISC0, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_XY_LOGIC_MISC1, 0x00000000);

        //**********************************************************************
        // Always use the USER clip rects
        //**********************************************************************

        XYLogicMisc1 |= (DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_XCMIN, _USERMIN)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_XCMAX, _USERMAX)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_YCMIN, _USERMIN)  |
                     DRF_DEF(_PGRAPH, _XY_LOGIC_MISC1, _SEL_YCMAX, _USERMAX)) ;
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_XY_LOGIC_MISC1,XYLogicMisc1);


        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_X_MISC, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_Y_MISC, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_SUBDIVIDE, 0x00000000);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_EDGEFILL, 0x00000000);


        //**********************************************************************
        // Now that we've initialized the registers, go ahead and mark
        // this channel as valid in the FIFO table structure
        //**********************************************************************

        HwDeviceExtension->FifoTable.InUse = 1;             // 1 = Valid
        HwDeviceExtension->FifoTable.ChID = 0;              // Channel 0 always
        }


    //**************************************************************************
    // TO DO:
    // Notify object --> Need to update??
    //**************************************************************************


    //**************************************************************************
    // Reset Context Switch bits
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_EXCEPTIONS,0);


    //**************************************************************************
    // Update and validate the channel (0)
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_CTX_CONTROL,
                                DRF_DEF(_PGRAPH,_CTX_CONTROL,_MINIMUM_TIME, _2MS) |
                                DRF_DEF(_PGRAPH,_CTX_CONTROL,_TIME, _NOT_EXPIRED) |
                                DRF_DEF(_PGRAPH,_CTX_CONTROL,_CHID, _VALID)       |
                                DRF_DEF(_PGRAPH,_CTX_CONTROL,_SWITCHING,_IDLE)    |
                                DRF_DEF(_PGRAPH,_CTX_CONTROL,_DEVICE,_ENABLED));


    //**************************************************************************
    // Restore saved FIFO state
    //**************************************************************************

    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE0_PULL0,cache0pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE0_PUSH0,cache0push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PULL0,cache1pull0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHE1_PUSH0,cache1push0);
    PFIFO_REG_WR32(PFIFO_Base, NV_PFIFO_CACHES,caches);

    }


//******************************************************************************
//
// Function: NV1_SetColorLookup()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV1_SetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    )


    {
    ULONG OutData;
    USHORT i;

    //**************************************************************************
    // Set the first CLUT index
    //**************************************************************************

    OutData=(SGS_DAC_RGB_EXT_PAL_CTRL & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_RGB_EXT_PAL_CTRL>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= 0x0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData= (UCHAR) ClutBuffer->FirstEntry;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_WRITE_PAL_ADDR,OutData);

    //**************************************************************************
    //  Set CLUT registers directly on the hardware
    //**************************************************************************

    for (i = 0; i < ClutBuffer->NumEntries; i++)
        {
        OutData= (ULONG) ((UCHAR)(ClutBuffer->LookupTable[i].RgbArray.Red));
        PDAC_REG_WR32 (PDAC_Base,SGS_DAC_UPORT_COLOR,OutData);

        OutData= (ULONG) ((UCHAR)(ClutBuffer->LookupTable[i].RgbArray.Green));
        PDAC_REG_WR32 (PDAC_Base,SGS_DAC_UPORT_COLOR,OutData);

        OutData= (ULONG) ((UCHAR)(ClutBuffer->LookupTable[i].RgbArray.Blue));
        PDAC_REG_WR32 (PDAC_Base,SGS_DAC_UPORT_COLOR,OutData);
        }

    }


//******************************************************************************
//
// Function: NV1_InitPalette()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV1_InitPalette(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )

    {
    ULONG OutData;
    USHORT i;

    //**************************************************************************
    // Set the first CLUT index
    //**************************************************************************

    OutData=(SGS_DAC_RGB_EXT_PAL_CTRL & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_LO, OutData);

    OutData=((SGS_DAC_RGB_EXT_PAL_CTRL>>8) & 0xff);
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_HI, OutData);

    OutData= 0x0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_INDEX_DATA, OutData);

    OutData= 0x0;
    PDAC_REG_WR32( PDAC_Base, SGS_DAC_UPORT_WRITE_PAL_ADDR,OutData);

    //**************************************************************************
    //  Set CLUT registers directly on the hardware
    // Init to a gray scale so we'll something appear instead of just black
    //**************************************************************************

    for (i = 0; i < 256 ; i++)
        {
        OutData= (ULONG) ((UCHAR)(i));
        PDAC_REG_WR32 (PDAC_Base,SGS_DAC_UPORT_COLOR,OutData);

        OutData= (ULONG) ((UCHAR)(i));
        PDAC_REG_WR32 (PDAC_Base,SGS_DAC_UPORT_COLOR,OutData);

        OutData= (ULONG) ((UCHAR)(i));
        PDAC_REG_WR32 (PDAC_Base,SGS_DAC_UPORT_COLOR,OutData);
        }

    }


//******************************************************************************
//
// Function:    NV1_MapMemoryRanges()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

ULONG NV1_MapMemoryRanges(PVOID HwDeviceExtension)

    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    PHWREG LinearMasterControlAddress;
    PHWREG LinearPFBControlAddress;
    ULONG SavedPMC;
    ULONG SavedIntrEn0;
    ULONG SavedConfig0;
    ULONG SavedDelay0;
    ULONG SavedGreen0;

    //**************************************************************************
    // Map memory for the NV hardware registers.  Not all of these ranges
    // are necessary, but we may need to access some of them later.
    // So we'll map a good majority of them for now.
    //**************************************************************************

    //**************************************************************************
    // Map memory for the NV1 MASTER CONTROL registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PMC_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PMC_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PMC_Registers.LowPart += RangeOffsets[NV_PMC_INDEX].ulOffset;
    hwDeviceExtension->NV1_PMC_Length = RangeOffsets[NV_PMC_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PMC_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PMC_Registers,
              hwDeviceExtension->NV1_PMC_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map Master Control Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }

    LinearMasterControlAddress = hwDeviceExtension->NV1_Lin_PMC_Registers;

    //**************************************************************************
    // Enable ALL devices and interrupts.  Memory registers will NOT be accessible
    // until we do this final step. (Trying to read an NV Memory location before
    // setting this register will cause the debugger to hang the bus).
    //
    // NOTE:   Memory locations which are UNDEFINED for the NV architecture
    //         could hang the BUS if read.  So be careful about which
    //         memory locations you look at.  Some debuggers don't handle
    //         this too well.  So you might not be able to manually
    //         examine the registers if your debugger tries to read more
    //         than one location at once (possibly reading some undefined regs)
    //         Also, the NV architecture is a 32-bit environment and
    //         does not like reading/writing bytes at a time.
    //**************************************************************************

    PMC_REG_WR32(LinearMasterControlAddress,NV_PMC_ENABLE, 0xffffffff);

    //**************************************************************************
    // Map memory for the NV1 FRAME BUFFER CONTROL registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PFB_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PFB_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PFB_Registers.LowPart += RangeOffsets[NV_PFB_INDEX].ulOffset;
    hwDeviceExtension->NV1_PFB_Length = RangeOffsets[NV_PFB_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PFB_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PFB_Registers,
              hwDeviceExtension->NV1_PFB_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map Frame Buffer Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }

    LinearPFBControlAddress = hwDeviceExtension->NV1_Lin_PFB_Registers;


    //**************************************************************************
    // Map memory for the NV1 REAL MODE CONTROL registers (32k worth )
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PRM_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PRM_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PRM_Registers.LowPart += RangeOffsets[NV_PRM_INDEX].ulOffset;
    hwDeviceExtension->NV1_PRM_Length = RangeOffsets[NV_PRM_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PRM_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PRM_Registers,
              hwDeviceExtension->NV1_PRM_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map Real Mode Control Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 DAC registers (4k worth??)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PDAC_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PDAC_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PDAC_Registers.LowPart += RangeOffsets[NV_PDAC_INDEX].ulOffset;
    hwDeviceExtension->NV1_PDAC_Length = RangeOffsets[NV_PDAC_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PDAC_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PDAC_Registers,
              hwDeviceExtension->NV1_PDAC_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map DAC Registers\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 RAM registers (4k worth??)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PRAM_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PRAM_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PRAM_Registers.LowPart += RangeOffsets[NV_PRAM_INDEX].ulOffset;
    hwDeviceExtension->NV1_PRAM_Length = RangeOffsets[NV_PRAM_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PRAM_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PRAM_Registers,
              hwDeviceExtension->NV1_PRAM_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map RAM Registers\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 RAMFC registers
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PRAMFC_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PRAMFC_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PRAMFC_Registers.LowPart += RangeOffsets[NV_PRAMFC_INDEX].ulOffset;
    hwDeviceExtension->NV1_PRAMFC_Length = RangeOffsets[NV_PRAMFC_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PRAMFC_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PRAMFC_Registers,
              hwDeviceExtension->NV1_PRAMFC_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map RAMFC Registers\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 RAMHT registers
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PRAMHT_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PRAMHT_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PRAMHT_Registers.LowPart += RangeOffsets[NV_PRAMHT_INDEX].ulOffset;
    hwDeviceExtension->NV1_PRAMHT_Length = RangeOffsets[NV_PRAMHT_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PRAMHT_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PRAMHT_Registers,
              hwDeviceExtension->NV1_PRAMHT_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map RAMHT Registers\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 FIFO registers (8k worth??)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PFIFO_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PFIFO_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PFIFO_Registers.LowPart += RangeOffsets[NV_PFIFO_INDEX].ulOffset;
    hwDeviceExtension->NV1_PFIFO_Length = RangeOffsets[NV_PFIFO_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PFIFO_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PFIFO_Registers,
              hwDeviceExtension->NV1_PFIFO_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map FIFO Registers\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PBUS registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PBUS_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PBUS_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PBUS_Registers.LowPart += RangeOffsets[NV_PBUS_INDEX].ulOffset;
    hwDeviceExtension->NV1_PBUS_Length = RangeOffsets[NV_PBUS_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PBUS_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PBUS_Registers,
              hwDeviceExtension->NV1_PBUS_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PBUS Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PGRAPH registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PGRAPH_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PGRAPH_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PGRAPH_Registers.LowPart += RangeOffsets[NV_PGRAPH_INDEX].ulOffset;
    hwDeviceExtension->NV1_PGRAPH_Length = RangeOffsets[NV_PGRAPH_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PGRAPH_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PGRAPH_Registers,
              hwDeviceExtension->NV1_PGRAPH_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PGRAPH Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 Rectangle USER registers (8k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_URECT_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_URECT_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_URECT_Registers.LowPart += RangeOffsets[NV_URECT_INDEX].ulOffset;
    hwDeviceExtension->NV1_URECT_Length = RangeOffsets[NV_URECT_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_URECT_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_URECT_Registers,
              hwDeviceExtension->NV1_URECT_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map URECT Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 Triangle registers (8k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_UTRI_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_UTRI_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_UTRI_Registers.LowPart += RangeOffsets[NV_UTRI_INDEX].ulOffset;
    hwDeviceExtension->NV1_UTRI_Length = RangeOffsets[NV_UTRI_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_UTRI_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_UTRI_Registers,
              hwDeviceExtension->NV1_UTRI_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map UTRI Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 BLIT registers (8k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_UBLIT_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_UBLIT_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_UBLIT_Registers.LowPart += RangeOffsets[NV_UBLIT_INDEX].ulOffset;
    hwDeviceExtension->NV1_UBLIT_Length = RangeOffsets[NV_UBLIT_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_UBLIT_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_UBLIT_Registers,
              hwDeviceExtension->NV1_UBLIT_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map UBLIT Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 IMAGE From CPU registers (8k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_UIMAGE_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_UIMAGE_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_UIMAGE_Registers.LowPart += RangeOffsets[NV_UIMAGE_INDEX].ulOffset;
    hwDeviceExtension->NV1_UIMAGE_Length = RangeOffsets[NV_UIMAGE_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_UIMAGE_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_UIMAGE_Registers,
              hwDeviceExtension->NV1_UIMAGE_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map UIMAGE Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 IMAGE MONO From CPU registers (8k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_UBITMAP_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_UBITMAP_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_UBITMAP_Registers.LowPart += RangeOffsets[NV_UBITMAP_INDEX].ulOffset;
    hwDeviceExtension->NV1_UBITMAP_Length = RangeOffsets[NV_UBITMAP_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_UBITMAP_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_UBITMAP_Registers,
              hwDeviceExtension->NV1_UBITMAP_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map UBITMAP Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }

    //**************************************************************************
    // Map memory for the NV1 PCI CONFIG registers (512 bytes worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_CONFIG_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_CONFIG_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_CONFIG_Registers.LowPart += RangeOffsets[NV_CONFIG_INDEX].ulOffset;
    hwDeviceExtension->NV1_CONFIG_Length = RangeOffsets[NV_CONFIG_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_CONFIG_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_CONFIG_Registers,
              hwDeviceExtension->NV1_CONFIG_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map CONFIG Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 DMA registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PDMA_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PDMA_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PDMA_Registers.LowPart += RangeOffsets[NV_PDMA_INDEX].ulOffset;
    hwDeviceExtension->NV1_PDMA_Length = RangeOffsets[NV_PDMA_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PDMA_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PDMA_Registers,
              hwDeviceExtension->NV1_PDMA_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PDMA Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PTIMER registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PTIMER_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PTIMER_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PTIMER_Registers.LowPart += RangeOffsets[NV_PTIMER_INDEX].ulOffset;
    hwDeviceExtension->NV1_PTIMER_Length = RangeOffsets[NV_PTIMER_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PTIMER_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PTIMER_Registers,
              hwDeviceExtension->NV1_PTIMER_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PTIMER Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PAUDIO registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PAUDIO_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PAUDIO_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PAUDIO_Registers.LowPart += RangeOffsets[NV_PAUDIO_INDEX].ulOffset;
    hwDeviceExtension->NV1_PAUDIO_Length = RangeOffsets[NV_PAUDIO_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PAUDIO_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PAUDIO_Registers,
              hwDeviceExtension->NV1_PAUDIO_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PAUDIO Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PAUTH registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PAUTH_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PAUTH_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PAUTH_Registers.LowPart += RangeOffsets[NV_PAUTH_INDEX].ulOffset;
    hwDeviceExtension->NV1_PAUTH_Length = RangeOffsets[NV_PAUTH_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PAUTH_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PAUTH_Registers,
              hwDeviceExtension->NV1_PAUTH_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PAUTH Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PEXTDEV registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PEXTDEV_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PEXTDEV_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PEXTDEV_Registers.LowPart += RangeOffsets[NV_PEXTDEV_INDEX].ulOffset;
    hwDeviceExtension->NV1_PEXTDEV_Length = RangeOffsets[NV_PEXTDEV_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PEXTDEV_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PEXTDEV_Registers,
              hwDeviceExtension->NV1_PEXTDEV_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PEXTDEV Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PEEPROM registers (4k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PEEPROM_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PEEPROM_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PEEPROM_Registers.LowPart += RangeOffsets[NV_PEEPROM_INDEX].ulOffset;
    hwDeviceExtension->NV1_PEEPROM_Length = RangeOffsets[NV_PEEPROM_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PEEPROM_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PEEPROM_Registers,
              hwDeviceExtension->NV1_PEEPROM_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PEEPROM Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PROM registers (32k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PROM_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PROM_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PROM_Registers.LowPart += RangeOffsets[NV_PROM_INDEX].ulOffset;
    hwDeviceExtension->NV1_PROM_Length = RangeOffsets[NV_PROM_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PROM_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PROM_Registers,
              hwDeviceExtension->NV1_PROM_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PROM Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }


    //**************************************************************************
    // Map memory for the NV1 PALT registers (32k worth)
    // Here, we obtain a virtual address from the physical address
    //**************************************************************************

    hwDeviceExtension->NV1_Phys_PALT_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_PALT_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_PALT_Registers.LowPart += RangeOffsets[NV_PALT_INDEX].ulOffset;
    hwDeviceExtension->NV1_PALT_Length = RangeOffsets[NV_PALT_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_PALT_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_PALT_Registers,
              hwDeviceExtension->NV1_PALT_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PALT Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }

    //******************************************************************
    // Store the USER channel address in our HwDeviceExtension
    // structure, so that we can verify that the graphics engine is
    // working HERE in the miniport, before the display driver takes over.
    //******************************************************************
    hwDeviceExtension->NV1_Phys_USER_Registers.HighPart = 0x00000000;
    hwDeviceExtension->NV1_Phys_USER_Registers.LowPart = NVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    hwDeviceExtension->NV1_Phys_USER_Registers.LowPart += RangeOffsets[NV_USER_INDEX].ulOffset;
    hwDeviceExtension->NV1_USER_Length = RangeOffsets[NV_USER_INDEX].ulLength;

    if ( ( hwDeviceExtension->NV1_Lin_USER_Registers =
              VideoPortGetDeviceBase(hwDeviceExtension,
                      hwDeviceExtension->NV1_Phys_USER_Registers,
              hwDeviceExtension->NV1_USER_Length, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map USER Channel\n"));
        return(ERROR_INVALID_PARAMETER);
        }



    //**************************************************************************
    // Init the Master Control Register
    //**************************************************************************

    SavedPMC = PMC_REG_RD32(LinearMasterControlAddress, NV_PMC_ENABLE);
    SavedIntrEn0 = PMC_REG_RD32(LinearMasterControlAddress, NV_PMC_INTR_EN_0);

    PMC_REG_WR32(LinearMasterControlAddress, NV_PMC_ENABLE, 0xffffffff);

    //**************************************************************************
    // Prepare to write to the frame buffer control registers
    //**************************************************************************

    SavedConfig0 = PFB_REG_RD32(LinearPFBControlAddress, NV_PFB_CONFIG_0);
    SavedDelay0 = PFB_REG_RD32(LinearPFBControlAddress, NV_PFB_DELAY_0);
    SavedGreen0 = PFB_REG_RD32(LinearPFBControlAddress, NV_PFB_GREEN_0);

    //**************************************************************************
    // TEST CODE:
    // Blank the screen, then turn it back on just to verify that we've
    // correctly mapped the NV registers and that they are enabled.
    //**************************************************************************

    PFB_REG_WR32(LinearPFBControlAddress, NV_PFB_GREEN_0, 0x00000001);
    PFB_REG_WR32(LinearPFBControlAddress, NV_PFB_GREEN_0, SavedGreen0);

    //**************************************************************************
    // Turn off all NV interrupts
    //**************************************************************************

    PMC_REG_WR32(LinearMasterControlAddress, NV_PMC_INTR_EN_0, 0x0);

    //**************************************************************************
    // Return successful
    //**************************************************************************

    return(NO_ERROR);
    } // NV1_MapMemoryRanges()



//******************************************************************************
//
// Function: NV1_IsPresent()
//
// Routine Description:
//
// Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
// Return Value:
//
//******************************************************************************


BOOLEAN NV1_IsPresent(PHW_DEVICE_EXTENSION HwDeviceExtension,
                      PVIDEO_ACCESS_RANGE pNVAccessRange,
                      PULONG NV1Slot)


    {
    VIDEO_ACCESS_RANGE TempAccessRange;
    USHORT  usVendorId,stVendorId;
    USHORT  usDeviceId;
    ULONG   ulSlot     = 0;
    ULONG   Address;
    ULONG   i;

    //**************************************************************************
    // VideoPortGetAccessRanges calls the PCI Bios to determine
    //    if the specified device is present or not.  This appears
    //    to be the equivalent call to the PCI Bios FIND_PCI_DEVICE call.
    //
    // Specify NumRequestedResources = 0
    //         RequestedResources = NULL
    //         NumAccessRanges = 1
    //         TempAccessRanges = Bus relative memory ranges for device (returned)
    //         VendorID = ID which identifies manufacturer
    //         DeviceID = ID which identifies device
    //         Slot = Specifies the Nth device of this type.
    //                Specify 0 to find the first (zero'th) occurence of device)
    //**************************************************************************

    stVendorId = ST1_VENDOR_ID;
    usVendorId = NV1_VENDOR_ID;
    usDeviceId = NV1_DEVICE_NV1_ID;

    //**************************************************************************
    // First, check and see if NV1 is present
    //**************************************************************************

    if (VideoPortGetAccessRanges(HwDeviceExtension,
                                 0,
                                 NULL,
                                 1,
                                 &TempAccessRange,
                                 &usVendorId,
                                 &usDeviceId,
                                 &ulSlot) == NO_ERROR)
        {

Treat_SGS_Same_As_NV:

        VideoDebugPrint((1,"Found an NV1 in pci slot %d\n", ulSlot));
        HwDeviceExtension->ulSlot = ulSlot;

        //**********************************************************************
        //  At this point we have identified an NV1 on the PCi bus by using the
        //  vendor and device id.
        //**********************************************************************

        HwDeviceExtension->ulChipID   = NV1_DEVICE_NV1_ID;
        HwDeviceExtension->ulRevLevel = REV_UNDEF;
        HwDeviceExtension->BoardID = NV1_VENDOR_ID;

        //**********************************************************************
        // Save the slot number of later (for VideoPortSetBusData)
        //**********************************************************************

        *NV1Slot = ulSlot;

        //**********************************************************************
        //  TO DO:  The VideoPortGetAccessRanges call seems to work
        //          by calling the PCI Bios.  However, to be safe, we should
        //          add additional code to manually search for an NV1 adapter
        //          (ala the ScanNVMechanism1/2 routines in the Resource Manager)
        //          just in case some system BIOS's don't work correctly, or
        //          we're using a system with an older/buggy PCI Bios
        //**********************************************************************

        //**********************************************************************
        // Initialize the BASE PHYSICAL ADDRESS of the NV adapter in the Access Ranges
        //**********************************************************************

        pNVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeStart = TempAccessRange.RangeStart;
        pNVAccessRange[NV_PHYSICAL_BASE_ADDRESS].RangeLength = TempAccessRange.RangeLength;

        //**********************************************************************
        // QUERY_PUBLIC_ACCESS_RANGES will use the following values to properly map
        // in the public access ranges.  For our display driver, we're going
        // to declare 4 public access ranges:   1) NV User Channel
        //                                      2) Graphics Status Register
        //                                      3) Frame Buffer VBLANK register
        //                                      4) Frame Buffer START register
        // Only the miniport will have access to all of the other privileged
        // graphics registers.
        //**********************************************************************

        //**********************************************************************
        // Init NV memory range offset values to zero
        //**********************************************************************

        for (i=0 ; i<NV_NUM_RANGES ;i++)
          {
          RangeOffsets[i].ulOffset = 0;
          RangeOffsets[i].ulLength = 0;
          }

        //**********************************************************************
        // Initialize the actual offset values
        //**********************************************************************

        RangeOffsets[NV_PMC_INDEX].ulOffset = DEVICE_BASE(NV_PMC);
        RangeOffsets[NV_PMC_INDEX].ulLength = DEVICE_EXTENT(NV_PMC);

        RangeOffsets[NV_PFB_INDEX].ulOffset = DEVICE_BASE(NV_PFB);
        RangeOffsets[NV_PFB_INDEX].ulLength = DEVICE_EXTENT(NV_PFB);

        RangeOffsets[NV_PRM_INDEX].ulOffset = DEVICE_BASE(NV_PRM);
        RangeOffsets[NV_PRM_INDEX].ulLength = DEVICE_EXTENT(NV_PRM);

        RangeOffsets[NV_PDAC_INDEX].ulOffset = DEVICE_BASE(NV_PDAC);
        RangeOffsets[NV_PDAC_INDEX].ulLength = DEVICE_EXTENT(NV_PDAC);

        RangeOffsets[NV_PRAM_INDEX].ulOffset = DEVICE_BASE(NV_PRAM);
        RangeOffsets[NV_PRAM_INDEX].ulLength = DEVICE_EXTENT(NV_PRAM);

        RangeOffsets[NV_PRAMFC_INDEX].ulOffset = DEVICE_BASE(NV_PRAMFC);
        RangeOffsets[NV_PRAMFC_INDEX].ulLength = DEVICE_EXTENT(NV_PRAMFC);

        RangeOffsets[NV_PRAMHT_INDEX].ulOffset = DEVICE_BASE(NV_PRAMHT);
        RangeOffsets[NV_PRAMHT_INDEX].ulLength = DEVICE_EXTENT(NV_PRAMHT);

        RangeOffsets[NV_PFIFO_INDEX].ulOffset = DEVICE_BASE(NV_PFIFO);
        RangeOffsets[NV_PFIFO_INDEX].ulLength = DEVICE_EXTENT(NV_PFIFO);

        RangeOffsets[NV_PBUS_INDEX].ulOffset = DEVICE_BASE(NV_PBUS);
        RangeOffsets[NV_PBUS_INDEX].ulLength = DEVICE_EXTENT(NV_PBUS);

        RangeOffsets[NV_PGRAPH_INDEX].ulOffset = DEVICE_BASE(NV_PGRAPH);
        RangeOffsets[NV_PGRAPH_INDEX].ulLength = DEVICE_EXTENT(NV_PGRAPH);

        RangeOffsets[NV_URECT_INDEX].ulOffset = DEVICE_BASE(NV_URECT);
        RangeOffsets[NV_URECT_INDEX].ulLength = DEVICE_EXTENT(NV_URECT);

        RangeOffsets[NV_UTRI_INDEX].ulOffset = DEVICE_BASE(NV_UTRI);
        RangeOffsets[NV_UTRI_INDEX].ulLength = DEVICE_EXTENT(NV_UTRI);

        RangeOffsets[NV_UBLIT_INDEX].ulOffset = DEVICE_BASE(NV_UBLIT);
        RangeOffsets[NV_UBLIT_INDEX].ulLength = DEVICE_EXTENT(NV_UBLIT);

        RangeOffsets[NV_UIMAGE_INDEX].ulOffset = DEVICE_BASE(NV_UIMAGE);
        RangeOffsets[NV_UIMAGE_INDEX].ulLength = DEVICE_EXTENT(NV_UIMAGE);

        RangeOffsets[NV_UBITMAP_INDEX].ulOffset = DEVICE_BASE(NV_UBITMAP);
        RangeOffsets[NV_UBITMAP_INDEX].ulLength = DEVICE_EXTENT(NV_UBITMAP);

        RangeOffsets[NV_CONFIG_INDEX].ulOffset = DEVICE_BASE(NV_CONFIG);
        RangeOffsets[NV_CONFIG_INDEX].ulLength = DEVICE_EXTENT(NV_CONFIG);

        RangeOffsets[NV_PDMA_INDEX].ulOffset = DEVICE_BASE(NV_PDMA);
        RangeOffsets[NV_PDMA_INDEX].ulLength = DEVICE_EXTENT(NV_PDMA);

        RangeOffsets[NV_PTIMER_INDEX].ulOffset = DEVICE_BASE(NV_PTIMER);
        RangeOffsets[NV_PTIMER_INDEX].ulLength = DEVICE_EXTENT(NV_PTIMER);

        RangeOffsets[NV_PAUDIO_INDEX].ulOffset = DEVICE_BASE(NV_PAUDIO);
        RangeOffsets[NV_PAUDIO_INDEX].ulLength = DEVICE_EXTENT(NV_PAUDIO);

        RangeOffsets[NV_PAUTH_INDEX].ulOffset = DEVICE_BASE(NV_PAUTH);
        RangeOffsets[NV_PAUTH_INDEX].ulLength = DEVICE_EXTENT(NV_PAUTH);

        RangeOffsets[NV_PEXTDEV_INDEX].ulOffset = DEVICE_BASE(NV_PEXTDEV);
        RangeOffsets[NV_PEXTDEV_INDEX].ulLength = DEVICE_EXTENT(NV_PEXTDEV);

        RangeOffsets[NV_PEEPROM_INDEX].ulOffset = DEVICE_BASE(NV_PEEPROM);
        RangeOffsets[NV_PEEPROM_INDEX].ulLength = DEVICE_EXTENT(NV_PEEPROM);

        RangeOffsets[NV_PROM_INDEX].ulOffset = DEVICE_BASE(NV_PROM);
        RangeOffsets[NV_PROM_INDEX].ulLength = DEVICE_EXTENT(NV_PROM);

        RangeOffsets[NV_PALT_INDEX].ulOffset = DEVICE_BASE(NV_PALT);
        RangeOffsets[NV_PALT_INDEX].ulLength = DEVICE_EXTENT(NV_PALT);

        //**********************************************************************
        // The following 5 ranges (in addition to the frame buffer range)
        // are the only ones that we'll pass to the display driver.
        // The previous ranges will only be used within this miniport driver.
        //
        // First, init the USER channel
        // NOTE: We will ONLY be using 1 Channel (for the display driver only)
        //       So we only need to allocate 64k for 1 channel.
        // Second, init the Graphics Status register
        // Third, init access to Vertical Blanking register (PFB_CONFIG_0)
        // Fourth, init access to Frame Buffer Start Address register (PFB_START_ADDRESS)
        // Fifth, init access to SGS DAC register (SGS_DAC)
        // Finally, init access to the dumb frame buffer
        //**********************************************************************

        RangeOffsets[NV_USER_INDEX].ulOffset = DEVICE_BASE(NV_USER);
//      RangeOffsets[NV_USER_INDEX].ulLength = DEVICE_EXTENT(NV_USER);
        RangeOffsets[NV_USER_INDEX].ulLength = 0x10000;

        //**********************************************************************
        // Just allocate memory for the GRAPHICS STATUS register
        //**********************************************************************

        RangeOffsets[NV_PGRAPH_STATUS_INDEX].ulOffset = NV_PGRAPH_STATUS;
        RangeOffsets[NV_PGRAPH_STATUS_INDEX].ulLength = 16;

        //**********************************************************************
        // Just allocate memory for the PFB_CONFIG_0 register
        // so we can check if we're in vertical blanking or not
        //**********************************************************************

        RangeOffsets[NV_PFB_CONFIG_0_INDEX].ulOffset = NV_PFB_CONFIG_0;
        RangeOffsets[NV_PFB_CONFIG_0_INDEX].ulLength = 16;


        //**********************************************************************
        // Just allocate memory for the PFB_START_ADDRESS register
        // so we can flip the current buffer
        //**********************************************************************

        RangeOffsets[NV_PFB_START_ADDRESS_INDEX].ulOffset = NV_PFB_START;
        RangeOffsets[NV_PFB_START_ADDRESS_INDEX].ulLength = 16;

        //**********************************************************************
        // Allocate memory for the SGS DAC memory range
        // so we can set the cursor registers.  Just allocate 64 bytes.
        // That will cover the entire needed range.
        //**********************************************************************

        RangeOffsets[NV_SGS_DAC_INDEX].ulOffset = DEVICE_BASE(NV_PDAC);
        RangeOffsets[NV_SGS_DAC_INDEX].ulLength = 64;


        //**********************************************************************
        // DUMB FRAME BUFFER location
        // The actual amount of memory is determined later in NV_GetNVInfo,
        // but for now, just default to 4Mb.  This value will be fixed up
        // in NV_GetNVInfo so that IOCTL_VIDEO_MAP_MEMORY will subsequently
        // map in the correct number of bytes.
        //**********************************************************************

        RangeOffsets[NV_PDFB_INDEX].ulOffset = DEVICE_BASE(NV_PDFB);
        RangeOffsets[NV_PDFB_INDEX].ulLength =  0x400000;



        //**********************************************************************
        //  Make sure the base Physical Address of NV device access range is
        //  stored in the PCI configuration space. Here, we get the the base
        //  address currently stored in PCI configuration space.
        //      PCIConfiguration is an ENUMERATED type
        //      Address will hold the PCI Common Config information
        //**********************************************************************

        VideoPortGetBusData(HwDeviceExtension,
                            PCIConfiguration,
                            ulSlot,
                            (PVOID) &Address,
                            FIELD_OFFSET(PCI_COMMON_CONFIG, u.type0.BaseAddresses),
                            sizeof(ULONG));

        //**********************************************************************
        //  Make sure the base Physical Address access range returned by
        //  GetAccessRanges is the same as the one returned by GetBusData.
        //  If not, then use SetBusData to make them agree.
        //**********************************************************************

        if (Address != TempAccessRange.RangeStart.LowPart)
            {
            VideoDebugPrint((0, "I really wish they we're equal, but\n"
                                "they're not, so make them equal!\n"));

            VideoPortSetBusData(HwDeviceExtension,
                                PCIConfiguration,
                                ulSlot,
                                (PVOID) &TempAccessRange.RangeStart.LowPart,
                                FIELD_OFFSET(PCI_COMMON_CONFIG, u.type0.BaseAddresses),
                                sizeof(ULONG));
            }


        return(TRUE);
        }


    //**************************************************************************
    // Also, check if an ST equivalent of the chip is present
    //**************************************************************************

    else if (VideoPortGetAccessRanges(HwDeviceExtension,
                                 0,
                                 NULL,
                                 1,
                                 &TempAccessRange,
                                 &stVendorId,
                                 &usDeviceId,
                                 &ulSlot) == NO_ERROR)
        {
        goto Treat_SGS_Same_As_NV;
        }

    //**************************************************************************
    // Didn't recognize NV1 or ST
    //**************************************************************************

    else

        return(FALSE);

    }


//******************************************************************************
//
// Function: NV1Interrupt()
//
// Routine Description:
//
//      Debug Interrupt routine to catch any inadvertent Interrupts that
//      the NV engine may generate (since we don't implement a resource manager)
//
// Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
// Return Value:
//
//******************************************************************************


BOOLEAN NV1Interrupt(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG OutData;
    ULONG SavedEnable;
    ULONG SavedWho;
    ULONG SavedPgraph;
    ULONG SavedAddress;
    ULONG SavedData;
    ULONG SavedCtxSwitch;
    ULONG SavedCtxControl;

    //**************************************************************************
    // Get the current state of some important registers
    // to help us figure out why an interrupt was generated.
    //**************************************************************************

    SavedEnable = PMC_REG_RD32(PMC_Base, NV_PMC_INTR_EN_0);
    SavedWho    = PMC_REG_RD32(PMC_Base, NV_PMC_INTR_0);
    SavedWho    = SavedWho & (~0x10000000);
    SavedPgraph = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_INTR_0);
    SavedAddress = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_TRAPPED_ADDR);
    SavedData = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_TRAPPED_DATA);
    SavedCtxSwitch = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_CTX_SWITCH);
    SavedCtxControl = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_CTX_CONTROL);

    //**************************************************************************
    // Check the interrupt that occurred
    //**************************************************************************

    if (SavedWho & 0x01000000)

        {
        OutData = DRF_DEF(_PGRAPH,_INTR_0,_VBLANK,_RESET);
        PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_INTR_0, OutData);
        return(TRUE);                   // This is probably 0x01000000 (VBLANK)
        }

    else if (SavedWho & ~0x11000000)    // Check all except vblank

        {
        //**********************************************************************
        // ARGHHHHH......ACCCKKKKK!!!!
        // We shouldn't be causing any interrupts....!!!!!
        // We don't have a Resource Manager yet.....
        //**********************************************************************

        OutData = DRF_DEF(_PFIFO,_INTR_0,_RUNOUT,_RESET) ;
        return(TRUE);       // This is PFIFO pending
        }


    return(FALSE);          // Someone else's interrupt, let them handle it

    }





//******************************************************************************
//
//  Function:   NV1ObjectSwappingTest()
//
//  Routine Description:
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

VOID NV1ObjectSwappingTest(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
//      PRENDEROBJECT PObject;
//      ULONG Data;
//      ULONG PatchCount;
//      PHWINFO NVInfo;
//      ULONG ColorFormat;
//      ULONG MonoFormat;
//      ULONG FreeCount;
//      NvChannel *nv;
//
//      //**************************************************************************
//      // NOTE: This function currently just tests the SWAPPING of objects only.
//      //       If we need to load in a brand NEW object, then we'll
//      //       need to completely initialize that object, as in NVSetupPatches.
//      //       Since we're just swapping out the triangle and rectangle object
//      //       here, I've skipped the calls to ClearPatchContext() and
//      //       SetPatchContext().  Remember, the context of a RENDER object
//      //       depends on the 'static' objects that make up its patch.
//      //       That is, SetPatchContext() generates the context of an object
//      //       depending on the IMAGE_SOLID,BLACK_RECTANGLE, ROP_SOLID, and
//      //       PATTERN objects that make up the patch associated with the
//      //       RENDER object.
//      //**************************************************************************
//
//      //**************************************************************************
//      // Wait for the Graphics Engine to be idle AS WELL AS the FIFO !!
//      // We MUST make sure that the NV engines (the Graphics Engine and FIFO) are both
//      // idle before we swap out OBJECTS. Otherwise, swapping out objects
//      // while something is still being drawn will probably generate interrupts.
//      //**************************************************************************
//
//      nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);
//
//      FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free;
//      while (FreeCount < NV_GUARANTEED_FIFO_SIZE)
//          FreeCount = nv->subchannel[RECTANGLE_SUBCHANNEL].control.free;
//
//      GR_DONE;
//
//
//      //**************************************************************************
//      //
//      // Enable FLOWTHRU
//      //
//      //      In order to access many of the registers in the Graphics Engine,
//      //      we need to enable FLOWTHRU and disable the FIFO engine.
//      //      FLOWTHRU = ENABLED,  FIFO = DISABLED, DMA = DISABLED
//      //      CLASS = 0 to specify graphics registers.
//      //
//      //**************************************************************************
//
//      PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED)          |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA,_DISABLED)           |
//                              DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
//                              DRF_NUM(_PGRAPH,_MISC,_CLASS,0));
//
//      //**************************************************************************
//      // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
//      //**************************************************************************
//
//      GR_DONE;
//
//      //**************************************************************************
//      // Checks for any interrupt requests.  For this miniport,
//      // we shouldn't be getting any
//      //**************************************************************************
//
//      GR_IDLE;
//
//
//      //**************************************************************************
//      // Set the color format according to what the current mode depth is
//      //**************************************************************************
//
//      NVInfo = &(HwDeviceExtension->NvInfo);
//
//      switch (NVInfo->Framebuffer.Depth)
//          {
//          case 32:
//              ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
//              break;
//          case 16:
//              ColorFormat = NV_COLOR_FORMAT_LE_X17R5G5B5;
//              break;
//          case 8:
//              ColorFormat = NV_COLOR_FORMAT_LE_X24Y8;
//              break;
//          default:
//              ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
//              break;
//          }
//
//      //**************************************************************************
//      // Default mono format is 0 and ALPHA NOT enabled
//      //**************************************************************************
//
//      MonoFormat = MONO_FORMAT_LE;
//
//      //**************************************************************************
//      // Now setup the individual patches that we'll be using for
//      // the NT Display driver
//      //**************************************************************************
//
//  RECTANGLE_SWAP:
//
//      //**************************************************************************
//      // Replace RECTANGLE with TRIANGLE
//      //**************************************************************************
//
//      MonoFormat = MONO_FORMAT_LE;
//      SetupRenderObject(HwDeviceExtension,RECTANGLE_SUBCHANNEL,TRIANGLE_PATCH ,
//                        NV_RENDER_SOLID_TRIANGLE, MY_RENDER_SOLID_TRIANGLE,
//                        ColorFormat,MonoFormat);
//
//      //**************************************************************************
//      // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
//      // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
//      //**************************************************************************
//
//      GR_DONE;
//      GR_IDLE;
//
//
//  //TRIANGLE_SWAP:
//  //
//  //    //**************************************************************************
//  //    // Replace the TRIANGLE object with RECTANGLE
//  //    //**************************************************************************
//  //
//  //    MonoFormat = MONO_FORMAT_LE;
//  //    SetupRenderObject(HwDeviceExtension,TRIANGLE_SUBCHANNEL,RECTANGLE_PATCH ,
//  //                      NV_RENDER_SOLID_RECTANGLE, MY_RENDER_SOLID_RECTANGLE,
//  //                      ColorFormat,MonoFormat);
//  //
//  //    //**************************************************************************
//  //    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
//  //    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
//  //    //**************************************************************************
//  //
//  //    GR_DONE;
//  //    GR_IDLE;
//  //
//  //
//      //**************************************************************************
//      // We're done with the Graphics registers for now.
//      // Make sure the FIFO and DMA engines are enabled.
//      //
//      //      FLOWTHRU = ENABLED,  FIFO = ENABLED, DMA = ENABLED
//      //
//      // DON'T TOUCH THE CLASS bits!!
//      //**************************************************************************
//
//      PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO,_ENABLED)           |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA,_ENABLED));
//
    }



//******************************************************************************
//
//  Function:   NV1ObjectSwappingTryIt()
//
//  Routine Description:
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

VOID NV1ObjectSwappingTryIt(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
//
//      ULONG i;
//      ULONG x,y,Color;
//      NvChannel *nv;
//
//      //**************************************************************************
//      // Get pointer to NV channel area
//      //**************************************************************************
//
//      nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);
//
//      //**************************************************************************
//      // Rectangle subchannel is NOW the Triangle subchannel.
//      // See if we successfully swapped it.  Note the RECTANGLE_SUBCHANNEL
//      // is being used, but we're actually writing out TRIANGLE methods.
//      //**************************************************************************
//
//      Color = 0x000000ff;
//      x = 125;
//      y = 125;
//
//      while (nv->subchannel[RECTANGLE_SUBCHANNEL].control.free < 4*4);
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Color = 0xff000000 | Color;
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y0_x0 = (((y+10) << 16) | (x-10) );
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y1_x1 = (((y+10) << 16) | (x+10) );
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y2_x2 = (((y-10) << 16) | x);
//
//      Color = 0x0000ff00;
//      x = 225;
//      y = 225;
//
//      while (nv->subchannel[RECTANGLE_SUBCHANNEL].control.free < 4*4);
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Color = 0xff000000 | Color;
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y0_x0 = (((y+10) << 16) | (x-10) );
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y1_x1 = (((y+10) << 16) | (x+10) );
//      nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidTriangle.Triangle.y2_x2 = (((y-10) << 16) | x);
//
//
//  //    //**************************************************************************
//  //    // Triangle subchannel is NOW the Rectangle subchannel.
//  //    // Note the TRIANGLE_SUBCHANNEL is being used, but we're actually
//  //    // writing out RECTANGLE methods.
//  //    //**************************************************************************
//  //
//  //    Color = 0x00ff0000;
//  //    x = 325;
//  //    y = 325;
//  //
//  //
//  //    while (nv->subchannel[TRIANGLE_SUBCHANNEL].control.free < 3*4);
//  //    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidRectangle.Color = 0xff000000 | Color;
//  //    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ((y << 16) | x );
//  //    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = 0x00500040;
//  //
//  //
//  //
//  //    Color = 0x00ff00ff;
//  //    x = 425;
//  //    y = 425;
//  //
//  //    while (nv->subchannel[TRIANGLE_SUBCHANNEL].control.free < 3*4);
//  //    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidRectangle.Color = 0xff000000 | Color;
//  //    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ((y << 16) | x );
//  //    nv->subchannel[TRIANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = 0x00600040;
//  //
//
    }


//******************************************************************************
//
//  Function:   NV1ObjectSwappingRestore()
//
//  Routine Description:
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

VOID NV1ObjectSwappingRestore(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
//      PRENDEROBJECT PObject;
//      ULONG Data;
//      ULONG PatchCount;
//      PHWINFO NVInfo;
//      ULONG ColorFormat;
//      ULONG MonoFormat;
//
//      ULONG FreeCount;
//      NvChannel *nv;
//
//
//      //**************************************************************************
//      // NOTE: This function currently just tests the SWAPPING of objects only.
//      //       If we need to load in a brand NEW object, then we'll
//      //       need to completely initialize that object, as in NVSetupPatches.
//      //       Since we're just swapping out the triangle and rectangle object
//      //       here, I've skipped the calls to ClearPatchContext() and
//      //       SetPatchContext().  Remember, the context of a RENDER object
//      //       depends on the 'static' objects that make up its patch.
//      //       That is, SetPatchContext() generates the context of an object
//      //       depending on the IMAGE_SOLID,BLACK_RECTANGLE, ROP_SOLID, and
//      //       PATTERN objects that make up the patch associated with the
//      //       RENDER object.
//      //**************************************************************************
//
//      //**************************************************************************
//      // Wait for the Graphics Engine to be idle AS WELL AS the FIFO !!
//      // We MUST make sure that the NV engines (the Graphics Engine and FIFO) are both
//      // idle before we swap out OBJECTS. Otherwise, swapping out objects
//      // while something is still being drawn will probably generate interrupts.
//      //**************************************************************************
//
//      nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);
//
//      FreeCount = nv->subchannel[TRIANGLE_SUBCHANNEL].control.free;
//      while (FreeCount < NV_GUARANTEED_FIFO_SIZE)
//          FreeCount = nv->subchannel[TRIANGLE_SUBCHANNEL].control.free;
//
//      GR_DONE;
//
//
//      //**************************************************************************
//      //
//      // Enable FLOWTHRU
//      //
//      //      In order to access many of the registers in the Graphics Engine,
//      //      we need to enable FLOWTHRU and disable the FIFO engine.
//      //      FLOWTHRU = ENABLED,  FIFO = DISABLED, DMA = DISABLED
//      //      CLASS = 0 to specify graphics registers.
//      //
//      //**************************************************************************
//
//      PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED)          |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA,_DISABLED)           |
//                              DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
//                              DRF_NUM(_PGRAPH,_MISC,_CLASS,0));
//
//      //**************************************************************************
//      // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
//      //**************************************************************************
//
//      GR_DONE;
//
//      //**************************************************************************
//      // Checks for any interrupt requests.  For this miniport,
//      // we shouldn't be getting any
//      //**************************************************************************
//
//      GR_IDLE;
//
//      //**************************************************************************
//      // Set the color format according to what the current mode depth is
//      //**************************************************************************
//
//      NVInfo = &(HwDeviceExtension->NvInfo);
//
//      switch (NVInfo->Framebuffer.Depth)
//          {
//          case 32:
//              ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
//              break;
//          case 16:
//              ColorFormat = NV_COLOR_FORMAT_LE_X17R5G5B5;
//              break;
//          case 8:
//              ColorFormat = NV_COLOR_FORMAT_LE_X24Y8;
//              break;
//          default:
//              ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
//              break;
//          }
//
//      //**************************************************************************
//      // Default mono format is 0 and ALPHA NOT enabled
//      //**************************************************************************
//
//      MonoFormat = MONO_FORMAT_LE;
//
//      //**************************************************************************
//      // Now setup the individual patches that we'll be using for
//      // the NT Display driver
//      //**************************************************************************
//
//  RECTANGLE_RESTORE:
//
//      //**************************************************************************
//      // Restore RECTANGLE object in TRIANGLE subchannel
//      //**************************************************************************
//
//      MonoFormat = MONO_FORMAT_LE;
//      SetupRenderObject(HwDeviceExtension,RECTANGLE_SUBCHANNEL,RECTANGLE_PATCH ,
//                        NV_RENDER_SOLID_RECTANGLE, MY_RENDER_SOLID_RECTANGLE,
//                        ColorFormat,MonoFormat);
//
//      //**************************************************************************
//      // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
//      // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
//      //**************************************************************************
//
//      GR_DONE;
//      GR_IDLE;
//
//
//  //TRIANGLE_RESTORE:
//  //
//  //    //**************************************************************************
//  //    // Restore the TRIANGLE object
//  //    //**************************************************************************
//  //
//  //    MonoFormat = MONO_FORMAT_LE;
//  //    SetupRenderObject(HwDeviceExtension,TRIANGLE_SUBCHANNEL,TRIANGLE_PATCH ,
//  //                      NV_RENDER_SOLID_TRIANGLE, MY_RENDER_SOLID_TRIANGLE,
//  //                      ColorFormat,MonoFormat);
//  //
//  //    //**************************************************************************
//  //    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
//  //    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
//  //    //**************************************************************************
//  //
//  //    GR_DONE;
//  //    GR_IDLE;
//  //
//  //
//      //**************************************************************************
//      // We're done with the Graphics registers for now.
//      // Make sure the FIFO and DMA engines are enabled.
//      //
//      //      FLOWTHRU = ENABLED,  FIFO = ENABLED, DMA = ENABLED
//      //
//      // DON'T TOUCH THE CLASS bits!!
//      //**************************************************************************
//
//      PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
//                              DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
//                              DRF_DEF(_PGRAPH,_MISC,_FIFO,_ENABLED)           |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
//                              DRF_DEF(_PGRAPH,_MISC,_DMA,_ENABLED));
//
    }


//******************************************************************************
//
// Function:    NV1WaitUntilFinished()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV1WaitUntilFinished(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    PHWINFO NVInfo;
    ULONG Width;
    ULONG Height;
    NvChannel *nv;

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Wait until there's nothing left in the FIFO to be done
    //**************************************************************************

    while (nv->subchannel[RECTANGLE_SUBCHANNEL].control.free < NV_GUARANTEED_FIFO_SIZE);

    //**************************************************************************
    // Make ABSOLUTELY sure the graphics engine is not busy anymore
    // by checking the Graphics status register.
    // This is a MUST.  If we don't do this, mode switches may hang
    // on fast systems (p6-200's or better).
    //**************************************************************************

    GR_DONE;

    }

//******************************************************************************
//
// Function:    NV1SetOpenGLPatch()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV1SetOpenGLPatch(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    PRENDEROBJECT PObject;
    ULONG Data;
    ULONG PatchCount;
    PHWINFO NVInfo;
    ULONG ColorFormat;
    ULONG MonoFormat;
    ULONG FreeCount;
    NvChannel *nv;

    //**************************************************************************
    // NOTE: This function currently just tests the SWAPPING of objects only.
    //       If we need to load in a brand NEW object, then we'll
    //       need to completely initialize that object, as in NVSetupPatches.
    //       Since we're just swapping out the triangle and rectangle object
    //       here, I've skipped the calls to ClearPatchContext() and
    //       SetPatchContext().  Remember, the context of a RENDER object
    //       depends on the 'static' objects that make up its patch.
    //       That is, SetPatchContext() generates the context of an object
    //       depending on the IMAGE_SOLID,BLACK_RECTANGLE, ROP_SOLID, and
    //       PATTERN objects that make up the patch associated with the
    //       RENDER object.
    //**************************************************************************

    //**************************************************************************
    // Wait for the Graphics Engine to be idle AS WELL AS the FIFO !!
    // We MUST make sure that the NV engines (the Graphics Engine and FIFO) are both
    // idle before we swap out OBJECTS. Otherwise, swapping out objects
    // while something is still being drawn will probably generate interrupts.
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.free;
    while (FreeCount < NV_GUARANTEED_FIFO_SIZE)
        FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.free;

    GR_DONE;


    //**************************************************************************
    //
    // Enable FLOWTHRU
    //
    //      In order to access many of the registers in the Graphics Engine,
    //      we need to enable FLOWTHRU and disable the FIFO engine.
    //      FLOWTHRU = ENABLED,  FIFO = DISABLED, DMA = DISABLED
    //      CLASS = 0 to specify graphics registers.
    //
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED)          |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_DISABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
                            DRF_NUM(_PGRAPH,_MISC,_CLASS,0));

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    //**************************************************************************

    GR_DONE;

    //**************************************************************************
    // Checks for any interrupt requests.  For this miniport,
    // we shouldn't be getting any
    //**************************************************************************

    GR_IDLE;


    //**************************************************************************
    // Set the color format according to what the current mode depth is
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        case 16:
            ColorFormat = NV_COLOR_FORMAT_LE_X17R5G5B5;
            break;
        case 8:
            ColorFormat = NV_COLOR_FORMAT_LE_X24Y8;
            break;
        default:
            ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        }

    //**************************************************************************
    // Default mono format is 0 and ALPHA NOT enabled
    //**************************************************************************


SWAP_IMAGE_FROM_CPU_WITH_TRIANGLE:

    //**************************************************************************
    // Replace IMAGE_FROM_CPU with TRIANGLE
    //**************************************************************************

    MonoFormat = MONO_FORMAT_LE;
    SetupRenderObject(HwDeviceExtension,TRIANGLE_SUBCHANNEL,TRIANGLE_PATCH ,
                      NV_RENDER_SOLID_TRIANGLE, MY_RENDER_SOLID_TRIANGLE,
                      ColorFormat,MonoFormat);

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
    //**************************************************************************

    GR_DONE;
    GR_IDLE;

    //**************************************************************************
    // We're done with the Graphics registers for now.
    // Make sure the FIFO and DMA engines are enabled.
    //
    //      FLOWTHRU = ENABLED,  FIFO = ENABLED, DMA = ENABLED
    //
    // DON'T TOUCH THE CLASS bits!!
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_ENABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_ENABLED));


    }


//******************************************************************************
//
// Function:    NV1RestoreDisplayDriverPatch()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV1RestoreDisplayDriverPatch(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    PRENDEROBJECT PObject;
    ULONG Data;
    ULONG PatchCount;
    PHWINFO NVInfo;
    ULONG ColorFormat;
    ULONG MonoFormat;
    ULONG FreeCount;
    NvChannel *nv;

    //**************************************************************************
    // NOTE: This function currently just tests the SWAPPING of objects only.
    //       If we need to load in a brand NEW object, then we'll
    //       need to completely initialize that object, as in NVSetupPatches.
    //       Since we're just swapping out the triangle and rectangle object
    //       here, I've skipped the calls to ClearPatchContext() and
    //       SetPatchContext().  Remember, the context of a RENDER object
    //       depends on the 'static' objects that make up its patch.
    //       That is, SetPatchContext() generates the context of an object
    //       depending on the IMAGE_SOLID,BLACK_RECTANGLE, ROP_SOLID, and
    //       PATTERN objects that make up the patch associated with the
    //       RENDER object.
    //**************************************************************************

    //**************************************************************************
    // Wait for the Graphics Engine to be idle AS WELL AS the FIFO !!
    // We MUST make sure that the NV engines (the Graphics Engine and FIFO) are both
    // idle before we swap out OBJECTS. Otherwise, swapping out objects
    // while something is still being drawn will probably generate interrupts.
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.free;
    while (FreeCount < NV_GUARANTEED_FIFO_SIZE)
        FreeCount = nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].control.free;

    GR_DONE;


    //**************************************************************************
    //
    // Enable FLOWTHRU
    //
    //      In order to access many of the registers in the Graphics Engine,
    //      we need to enable FLOWTHRU and disable the FIFO engine.
    //      FLOWTHRU = ENABLED,  FIFO = DISABLED, DMA = DISABLED
    //      CLASS = 0 to specify graphics registers.
    //
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_DISABLED)          |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_DISABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_CLASS_WRITE,_ENABLED)    |
                            DRF_NUM(_PGRAPH,_MISC,_CLASS,0));

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    //**************************************************************************

    GR_DONE;

    //**************************************************************************
    // Checks for any interrupt requests.  For this miniport,
    // we shouldn't be getting any
    //**************************************************************************

    GR_IDLE;

    //**************************************************************************
    // Now restore the individual patches that we'll be using for
    // the NT Display driver
    //**************************************************************************

SWAP_TRIANGLE_WITH_IMAGE_FROM_CPU:

    //**************************************************************************
    // For text, we want color 0 (background) to be transparent
    // So set the color format appropriately with ALPHA channel enabled
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        case 16:
            ColorFormat = NV_COLOR_FORMAT_LE_X17R5G5B5;
            break;
        case 8:
            ColorFormat = NV_COLOR_FORMAT_LE_X24Y8;
            break;
        default:
            ColorFormat = NV_COLOR_FORMAT_LE_X8R8G8B8;
            break;
        }

    //**************************************************************************
    // Setup the IMAGE_FROM_CPU object
    //**************************************************************************

    MonoFormat = MONO_FORMAT_LE;
    SetupRenderObject(HwDeviceExtension,IMAGE_FROM_CPU_SUBCHANNEL,IMAGE_FROM_CPU_PATCH,
                      NV_IMAGE_FROM_CPU, MY_IMAGE_FROM_CPU_BLIT,
                      ColorFormat, MonoFormat);

    //**************************************************************************
    // Wait for Engine to to be idle  (Reads PGRAPH_STATUS register)
    // And check for any interrupt requests.  For this miniport, shouldn't getting any !!
    //**************************************************************************

    GR_DONE;
    GR_IDLE;

    //**************************************************************************
    // We're done with the Graphics registers for now.
    // Make sure the FIFO and DMA engines are enabled.
    //
    //      FLOWTHRU = ENABLED,  FIFO = ENABLED, DMA = ENABLED
    //
    // DON'T TOUCH THE CLASS bits!!
    //**************************************************************************

    PGRAPH_REG_WR32(PGRAPH_Base, NV_PGRAPH_MISC,
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU_WRITE,_ENABLED) |
                            DRF_DEF(_PGRAPH,_MISC,_FLOWTHRU,_ENABLED)       |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO_WRITE,_ENABLED)     |
                            DRF_DEF(_PGRAPH,_MISC,_FIFO,_ENABLED)           |
                            DRF_DEF(_PGRAPH,_MISC,_DMA_WRITE,_ENABLED)      |
                            DRF_DEF(_PGRAPH,_MISC,_DMA,_ENABLED));


    }



//******************************************************************************
//
// Function:    NV1ClearScreen()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV1ClearScreen(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    PHWINFO NVInfo;
    ULONG Width;
    ULONG Height;
    NvChannel *nv;

    //**************************************************************************
    // Get pointer to NVInfo structure
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    //**************************************************************************
    // Get pointer to NV channel area
    //**************************************************************************

    nv = (NvChannel *)(HwDeviceExtension->NV1_Lin_USER_Registers);

    //**************************************************************************
    // Get width and height of current mode
    //**************************************************************************

    Width = NVInfo->Framebuffer.HorizDisplayWidth;
    Height = NVInfo->Framebuffer.VertDisplayWidth;

    //**************************************************************************
    // First set ROP, CLIP, and PATTERN objects.  Disable color keying
    //**************************************************************************

    SetRop(HwDeviceExtension,SRCCOPY);
    SetClip(HwDeviceExtension, 0, 0, 0x7fff, 0x7fff);
    SetPattern(HwDeviceExtension);
    SetColorKey(HwDeviceExtension, 0x00000000);

    //**************************************************************************
    // Fill Screen with black rectangle (color 0 will always be black
    // regardless of mode at this point)
    //**************************************************************************

    //**************************************************************************
    // Wait for FIFO to be available first
    //**************************************************************************

    while (nv->subchannel[RECTANGLE_SUBCHANNEL].control.free < 3*4);

    nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Color = 0x00000000;
    nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].y_x = ((0 << 16) | 0 );
    nv->subchannel[RECTANGLE_SUBCHANNEL].renderSolidRectangle.Rectangle[0].height_width = ((Height << 16) | (Width));


    }


//******************************************************************************
//
// Function:    NV1GraphicsTest()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV1GraphicsTest(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    PHWINFO NVInfo;
    LONG x_inc,y_inc;
    ULONG x,y;
    ULONG i;

    //**************************************************************************
    // Get pointer to NVInfo structure
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);

    //**************************************************************************
    // Give it a try! First set ROP, CLIP, and PATTERN objects
    //**************************************************************************

    SetRop(HwDeviceExtension,SRCCOPY);
    SetClip(HwDeviceExtension, 0, 0, 0x40, 0x40);
    SetPattern(HwDeviceExtension);

    //**************************************************************************
    // Disable Color keying for now, because it's not currently used by
    // the display driver, and we can use it's subchannel with another object
    //
    // Update: Initialize the ColorKey! We need to use the COLOR KEY object for
    //         Direct Draw.  (Currently, the only place that uses it is Direct Draw)
    //         Specify color = 0 to make it inactive.  Specify (ALPHA_1_32/
    //         ALPHA_1_16/ALPHA_1_08 | Color) to activate the color key.
    //**************************************************************************

    SetColorKey(HwDeviceExtension, 0x00000000);

    //**************************************************************************
    // Draw some rectangles at top left.
    //**************************************************************************

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            //******************************************************************
            // Color Format in 32bpp mode is X8R8G8B8 (See SetupPatches)
            //******************************************************************

            RenderRectangle(HwDeviceExtension,0,0,  0x0000ffff);    // G and B
            RenderRectangle(HwDeviceExtension,10,10,0x00ff00ff);    // R and B
            RenderRectangle(HwDeviceExtension,20,20,0x00ffff00);    // R and G

            //******************************************************************
            // Verify ColorKey is working with color 0
            //******************************************************************

            SetColorKey(HwDeviceExtension, NV_ALPHA_1_32 | 0);      // Enable Color Key
            RenderRectangle(HwDeviceExtension,0,0,  0x0000000);     // Not drawn
            SetColorKey(HwDeviceExtension, 0);                      // Disable Color Key
            RenderRectangle(HwDeviceExtension,0,0,  0x0000000);     // Drawn

            break;

        case 16:
            //******************************************************************
            // Color Format in 16bpp mode is X17R5G5B5 (See SetupPatches)
            //******************************************************************

            RenderRectangle(HwDeviceExtension,0,0,  0x000003ff);    // G and B
            RenderRectangle(HwDeviceExtension,10,10,0x00007c1f);    // R and B
            RenderRectangle(HwDeviceExtension,20,20,0x00007fe0);    // R and G

            //******************************************************************
            // Verify ColorKey is working with color 0
            //******************************************************************

            SetColorKey(HwDeviceExtension, NV_ALPHA_1_16 | 0);      // Enable Color Key
            RenderRectangle(HwDeviceExtension,0,0,  0x0000000);     // Not drawn
            SetColorKey(HwDeviceExtension, 0);                      // Disable Color Key
            RenderRectangle(HwDeviceExtension,0,0,  0x0000000);     // Drawn

            break;

        case 8:
            //******************************************************************
            // Color Format in 8bpp mode is X24Y8 (See SetupPatches)
            //******************************************************************

            RenderRectangle(HwDeviceExtension,0,0,  0x00000030);    // Dark Grey
            RenderRectangle(HwDeviceExtension,10,10,0x00000080);    // Medium Grey
            RenderRectangle(HwDeviceExtension,20,20,0x000000c0);    // Light Grey

            //******************************************************************
            // Verify ColorKey is working with color 0
            //******************************************************************

            SetColorKey(HwDeviceExtension, NV_ALPHA_1_08 | 0);      // Enable Color Key
            RenderRectangle(HwDeviceExtension,0,0,0x0000000);       // Not Drawn
            SetColorKey(HwDeviceExtension, 0);                      // Disable Color Key
            RenderRectangle(HwDeviceExtension,0,0,0x0000000);       // Drawn

            break;
        }

//    //**************************************************************************
//    // Init clip to a large rectangle
//    //**************************************************************************
//
//    SetClip(HwDeviceExtension, 0, 0, 640, 0x480);
//
//    switch (NVInfo->Framebuffer.Depth)
//        {
//        case 32:
//            RenderTriangle(HwDeviceExtension,320,240,0x00ffffff);
//            RenderTriangle(HwDeviceExtension,400,400,0x0000ffff);
//            break;
//
//        case 16:
//            RenderTriangle(HwDeviceExtension,320,240,0x0000ffff);
//            RenderTriangle(HwDeviceExtension,400,400,0x000003ff);
//            break;
//
//        case 8:
//            RenderTriangle(HwDeviceExtension,320,240,0x000000ff);
//            RenderTriangle(HwDeviceExtension,400,400,0x00000080);
//            break;
//        }

    //**************************************************************************
    // Draw a bunch of rectangles just to prove that the NV engine is working
    //
    // NOTE: The following example will show up better in 16bpp modes
    //       with 5:5:5 format (We're just generating random colors,
    //       and they probably won't look good since we're not paying
    //       attention to the color format)
    //**************************************************************************

    SetClip(HwDeviceExtension, 100, 100, 200, 200);

    x=0;y=0;
    x_inc = 1; y_inc = 1;
    i = 0;

    while (i < 10000)
        {
        if ( (x > 640-64) || (x < 0))
            x_inc*=-1;

        if ( (y > 480-64) || (y < 0))
            y_inc*=-1;

        x += x_inc;
        y += y_inc;

        RenderRectangle(HwDeviceExtension,x,y,i);
//        RenderTriangle(HwDeviceExtension,x,y,i<<5);

        i++;
        }

    //**************************************************************************
    // Now test out ROP functionality
    //**************************************************************************

    SetClip(HwDeviceExtension, 200, 200, 300, 300);
    SetRop(HwDeviceExtension, SRCAND );           // Src AND ROP

    x=0;y=0;
    x_inc = 1; y_inc = 1;
    i = 0;

    while (i < 10000)
        {
        if ( (x > 640-64) || (x < 0))
            x_inc*=-1;

        if ( (y > 480-64) || (y < 0))
            y_inc*=-1;

        x += x_inc;
        y += y_inc;

        RenderRectangle(HwDeviceExtension,x,y,i);
//        RenderTriangle(HwDeviceExtension,x,y,i<<5);


        i++;
        }

    //**************************************************************************
    // Now test out PATTERN functionality
    //**************************************************************************

    SetClip(HwDeviceExtension, 200, 200, 300, 300);
    SetRop(HwDeviceExtension, PATCOPY );

    x=0;y=0;
    x_inc = 1; y_inc = 1;
    i = 0;

    while (i < 10000)
        {
        if ( (x > 640-64) || (x < 0))
            x_inc*=-1;

        if ( (y > 480-64) || (y < 0))
            y_inc*=-1;

        x += x_inc;
        y += y_inc;

        RenderRectangle(HwDeviceExtension,x,y,i);
//        RenderTriangle(HwDeviceExtension,x,y,i<<5);


        i++;
        }

    //**************************************************************************
    // Test out Screen to Screen Blits (IMAGE_BLIT class)
    //**************************************************************************

    SetClip(HwDeviceExtension, 0,0,640,480);
    SetRop(HwDeviceExtension, SRCCOPY );

    ScreenToScreenBlit(HwDeviceExtension,320,240,  0,0,  20,20);
    ScreenToScreenBlit(HwDeviceExtension,320,240,  50,50,  100,100);
    ScreenToScreenBlit(HwDeviceExtension,320,480,  0,0,  300,0);

    //**************************************************************************
    // Test out Memory to Screen Blits (IMAGE_FROM_CPU class)
    //**************************************************************************

    MemoryToScreenBlit(HwDeviceExtension,100,100,  200,200);

    //**************************************************************************
    // Test out Memory to Screen Blits (IMAGE_MONO_FROM_CPU class)
    // Used for text acceleration.  We want color 0 to be transparent
    // and color 1 to be visible (Make sure ALPHA channel is working)
    //**************************************************************************

    switch (NVInfo->Framebuffer.Depth)
        {
        case 32:
            //******************************************************************
            // Color Format in 32bpp mode is X8R8G8B8 (See SetupPatches)
            //******************************************************************

            ImageMonoBlit(HwDeviceExtension,100,0,(~NV_ALPHA_1_32) & 0x00ff0000, (NV_ALPHA_1_32) | 0x0000ff00);
            ImageMonoBlit(HwDeviceExtension,200,0,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);
            ImageMonoBlit(HwDeviceExtension,300,0,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);
            ImageMonoBlit(HwDeviceExtension,400,-1,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);
            ImageMonoBlit(HwDeviceExtension,500,-10,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);

            ImageMonoBlit(HwDeviceExtension,0,100,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);
            ImageMonoBlit(HwDeviceExtension,0,200,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);
            ImageMonoBlit(HwDeviceExtension,-1,300,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);
            ImageMonoBlit(HwDeviceExtension,-10,400,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);

            //******************************************************************
            // Test alignment
            //******************************************************************

            for (i=0; i<20;i++)
                ImageMonoBlit(HwDeviceExtension,300+i,i*20,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x0000ff00);

            //******************************************************************
            // Print out character data
            //******************************************************************

            for (i=0;i <20;i++)
                ImageMonoLetterY(HwDeviceExtension, 400+i,i*20,(~NV_ALPHA_1_32) & 0x00ff0000,(NV_ALPHA_1_32) | 0x00eeeeee);

            break;

        case 16:
            //******************************************************************
            // Color Format in 16bpp mode is X17R5G5B5 (See SetupPatches)
            //******************************************************************

            ImageMonoBlit(HwDeviceExtension,100,0,(~NV_ALPHA_1_16) & 0x00007C00, (NV_ALPHA_1_16) | 0x000003E0);
            ImageMonoBlit(HwDeviceExtension,200,0,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);
            ImageMonoBlit(HwDeviceExtension,300,0,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);
            ImageMonoBlit(HwDeviceExtension,400,-1,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);
            ImageMonoBlit(HwDeviceExtension,500,-10,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);

            ImageMonoBlit(HwDeviceExtension,0,100,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);
            ImageMonoBlit(HwDeviceExtension,0,200,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);
            ImageMonoBlit(HwDeviceExtension,-1,300,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);
            ImageMonoBlit(HwDeviceExtension,-10,400,(~NV_ALPHA_1_16) & 0x00007C00,(NV_ALPHA_1_16) | 0x000003E0);

            //******************************************************************
            // Test alignment
            //******************************************************************

            for (i=0; i<20;i++)
                ImageMonoBlit(HwDeviceExtension,300+i,i*20,(~NV_ALPHA_1_16) & 0x00007c00,(NV_ALPHA_1_16) | 0x000003e0);

            //******************************************************************
            // Print out character data
            //******************************************************************

            for (i=0;i <20;i++)
                ImageMonoLetterY(HwDeviceExtension, 400+i,i*20,(~NV_ALPHA_1_16) & 0x00007c00,(NV_ALPHA_1_16) | 0x00007fff);


            break;

        case 8:
            //******************************************************************
            // Color Format in 8bpp mode is X24Y8 (See SetupPatches)
            //******************************************************************

            ImageMonoBlit(HwDeviceExtension,100,0,(~NV_ALPHA_1_08) & 0x0, (NV_ALPHA_1_08) | 0xff);
            ImageMonoBlit(HwDeviceExtension,200,0,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);
            ImageMonoBlit(HwDeviceExtension,300,0,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);
            ImageMonoBlit(HwDeviceExtension,400,-1,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);
            ImageMonoBlit(HwDeviceExtension,500,-10,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);

            ImageMonoBlit(HwDeviceExtension,0,100,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);
            ImageMonoBlit(HwDeviceExtension,0,200,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);
            ImageMonoBlit(HwDeviceExtension,-1,300,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);
            ImageMonoBlit(HwDeviceExtension,-10,400,(~NV_ALPHA_1_08) & 0x0,(NV_ALPHA_1_08) | 0xff);

            //******************************************************************
            // Test alignment
            //******************************************************************

            for (i=0; i<20;i++)
                ImageMonoBlit(HwDeviceExtension,300+i,i*20,(~NV_ALPHA_1_08) & 0x10,(NV_ALPHA_1_08) | 0xff);

            //******************************************************************
            // Print out character data
            //******************************************************************

            for (i=0;i <20;i++)
                ImageMonoLetterY(HwDeviceExtension, 400+i,i*20,(~NV_ALPHA_1_08) & 0x10,(NV_ALPHA_1_08) | 0xff);


            break;
        }

    //**************************************************************************
    // Default back to a full screen clip and SRCCOPY Rop
    // May need to modify this later for drawing to OFFSCREEN
    //**************************************************************************

    SetRop(HwDeviceExtension,SRCCOPY);
    SetClip(HwDeviceExtension, 0, 0, 0x7fff, 0x7fff);
    }

