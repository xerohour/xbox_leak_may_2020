/************************** Module Header *************************************
//                                                                             *
//  Module Name: nv3enabl.c                                                    *
//                                                                             *
//  This module contains the functions that enable and disable the             *
//  driver, the pdev, and the surface.                                         *
//                                                                             *
//  Copyright (c) 1992-1996 Microsoft Corporation                              *
//                                                                             *
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"

#ifdef NV3
#include "excpt.h"
#include "oldnv332.h"
#define NV32_INCLUDED // Prevents re-inclusion of real nv32 header
#include "driver.h"
#include "nv1c_ref.h"
#include "nvalpha.h"
#include "nvsubch.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"

#include "ddminint.h"
#include "dmamacro.h"
#include "nvcm.h"

//******************************************************************************
// Local prototypes
//******************************************************************************

BOOL bNV3CreateStdPatches(PDEV *ppdev);
VOID vNV3DestroyStdPatches(PDEV *ppdev, BOOL CreateStdPatchFailFlag);


//******************************************************************************
//
//  Function: bNV3CreateStdPatches
//
//  Routine Description:
//      Program the following patch:
//
//
//              ROP 5 SOLID   PATTERN     BLACK RECTANGLE     IMAGE SOLID
//                  |           |           |                   |
//                  \--|  |-----/           |                   |
//                     |  |                 |                   |
//                     V  V                 V                   V
//                   -----------       -------------       ----------
//      IMAGE       |           |     |             |     |          |
//      FROM   ---->|   IMAGE   |---->|    IMAGE    |---->|  IMAGE   |----\       (patch for subchannel 0)
//      CPU         |  ROP AND  |     |   STENCIL   |     | COLORKEY |    |
//                  |           |     |             |     |          |    |
//                   -----------       -------------       ----------     |
//                     |  |  ^              |                   |         |
//                     V  V  |              V                   V         |
//                   -----------       -------------       ----------     |
//                  |           |     |             |     |          |    |
//      IMAGE  ---->|   IMAGE   |---->|    IMAGE    |---->|  IMAGE   |--\ |       (patch for subchannel 1)
//      BLIT        |  ROP AND  |     |   STENCIL   |     | COLORKEY |  | |
//       ^          |           |     |             |     |          |  | |
//       |           -----------       -------------       ----------   | \-->
//   /---/             |  |  ^              |                   |       \---->IMAGE TO----> VIDEO
//   |                 V  V  |              V                   V       /----> VIDEO        SINK
//   |               -----------       -------------       ----------   | /--> |
//   |              |           |     |             |     |          |  | |    |
//   |  IMAGE  ---->|   IMAGE   |---->|    IMAGE    |---->|  IMAGE   |--/ |    |  (patch for subchannel 2)
//   |  MONO        |  ROP AND  |     |   STENCIL   |     | COLORKEY |    |    |
//   |              |           |     |             |     |          |    |    |
//   |               -----------       -------------       ----------     |    |
//   |                 |  |  ^              |                   |         |    |
//   |                 V  V  |              V                   V         |    |
//   |               -----------       -------------       ----------     |    |
//   |  RENDER      |           |     |             |     |          |    |    |
//   |  SOLID  ---->|   IMAGE   |---->|    IMAGE    |---->|  IMAGE   |----/    |  (patch for subchannel 3)
//   |  RECTANGLE   |  ROP AND  |     |   STENCIL   |     | COLORKEY |         |
//   |              |           |     |             |     |          |         |
//   |               -----------       -------------       ----------          |
//   |                       ^                                                 |
//   |                       |                                                 |
//   |                       |                                                 |
//   \-------------------------------------------------------------------------/
//
//
//  Arguments:
//      A pointer to the physical device
//
//  Return Value:
//      TRUE if successful
//      FALSE if unseccessful
//
//
//******************************************************************************

BOOL bNV3CreateStdPatches(
    PDEV *ppdev
)
{
    V032 colorFmtUnpack, colorFmtAlpha, colorFmtPack, videoFmt;
    ULONG color0, color1;
    Nv3ChannelPio *nv;
    U032 class;
    NV_CREATE_OBJECT_SETUP();

    if(!NvGetSupportedClasses(ppdev)){
        return FALSE;
    }


    //**************************************************************************
    // Get a channel from the NV device
    //**************************************************************************

    ppdev->hPioChannel = DD_PIO_CHANNEL_OBJECT_HANDLE;
    if (NvAllocChannelPio(  ppdev->hDriver,
                            ppdev->hClient,
                            ppdev->hDevice,
                            ppdev->hPioChannel,
                            NV03_CHANNEL_PIO,
                            0,
                            (PVOID)&nv,
                            0
                            ) != NVOS04_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot get NV PIO channel"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }

    //**************************************************************************
    // Save channel ptr in state struct.
    //**************************************************************************
    ppdev->pjMmBase = (ULONG *)nv;

    //**************************************************************************
    // For NV3, ddraw and 2d driver use the same channel.
    //**************************************************************************
    (Nv3ChannelPio*) ppdev->ddChannelPtr = (Nv3ChannelPio *) ppdev->pjMmBase;

    //**************************************************************************
    // Invalidate cached freecount in case this routine exits with an error
    //**************************************************************************
    ppdev->NVFreeCount = 0;

    DISPDBG((1, "DD setting up standard patches..."));

    // determine default packed and unpacked color formats from frame buffer depth
    switch (ppdev->iBitmapFormat)
    {
        case BMF_32BPP:
            colorFmtUnpack = NV_COLOR_FORMAT_LE_X8R8G8B8;
            colorFmtAlpha = NV_COLOR_FORMAT_LE_A8R8G8B8;
            colorFmtPack = colorFmtUnpack;
            videoFmt = NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;
            color0 =    NV_ALPHA_1_32 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_32 | 0x00ffffff;         // WHITE
            break;
        case BMF_16BPP:
            colorFmtUnpack = NV_COLOR_FORMAT_LE_X17R5G5B5;
            colorFmtAlpha = NV_COLOR_FORMAT_LE_X16A1R5G5B5;
            colorFmtPack = NV_COLOR_FORMAT_LE_X1R5G5B5_P2;
            videoFmt = NV_VFM_FORMAT_COLOR_LE_X1R5G5B5_P2;
            color0 =    NV_ALPHA_1_16 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_16 | 0x00007fff;         // WHITE
            break;
        case BMF_8BPP:
            colorFmtUnpack = NV_COLOR_FORMAT_LE_X24Y8;
            colorFmtAlpha = NV_COLOR_FORMAT_LE_X16A8Y8;
            colorFmtPack = NV_COLOR_FORMAT_LE_Y8_P4;
            videoFmt = NV_VFM_FORMAT_COLOR_LE_Y8_P4;
            color0 =    NV_ALPHA_1_08 | 0x00000000;         // BLACK
            color1 =    NV_ALPHA_1_08 | 0x000000ff;         // WHITE
            break;

        default:
            return(FALSE);
    }

    //**************************************************************************
    // Allocate a DMA context which points to all of video memory. The limit
    // must be page aligned: i.e. limit = (size in bytes of video mem rounded to the
    // closest page boundary) - 1.
    //**************************************************************************
    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_READ_WRITE,
                            (PVOID)(ppdev->pjFrameBufbase),
                            ppdev->cbFrameBuf
                            ) != NVOS03_STATUS_SUCCESS )
        {
        DISPDBG((1, "NVDD: Cannot allocate dma in memory context"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }

    //**************************************************************************
    // Allocate a DMA context to "sync" the fifo on ddraw lock calls.
    //**************************************************************************
    ((NvNotification *) ppdev->Notifiers->Sync)->status = 0;
    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            DD_FIFO_SYNC_NOTIFIER,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID)(ppdev->Notifiers->Sync),
                            sizeof(NvNotification) - 1
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot allocate fifo sync notifer"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }
    //*************************************************************************
    // allocate the dma notifier context for flip surface
    //*************************************************************************
    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID)(ppdev->Notifiers->FlipPrimary),
                            sizeof(NvNotification) - 1
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot allocate context dma notifier for flip surface"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }

    ((NvNotification *) (ppdev->Notifiers->FlipPrimary))->status = 0;

    //*************************************************************************
    // allocate the dma notifier context for video overlay flip
    //*************************************************************************
    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID)(ppdev->Notifiers->FlipOverlay),
                            sizeof(NvNotification) - 1
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot allocate notifier context"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }

    ((NvNotification *) (ppdev->Notifiers->FlipOverlay))->status = 0;
    //*************************************************************************
    // Setup palette for indexed color mode.
    // allocate the colormap context (from system memory)
    //*************************************************************************
    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            NV_WIN_COLORMAP_CONTEXT,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_READ_ONLY,
                            (PVOID)(ppdev->ajClutData),
                            (256 * sizeof(VIDEO_CLUTDATA)) - 1
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot allocate buffer context"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }

    //*************************************************************************
    // allocate the dma notifier context (from system memory)
    //*************************************************************************
    ((NvNotification *) (ppdev->Notifiers->DmaFromMem))->status = 0;

    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID) ppdev->Notifiers->DmaFromMem,
                            sizeof(NvNotification) - 1
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot allocate notifier context"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }

    //***********************************************************************************
    // Allocate a buffer for scanlines.
    //***********************************************************************************
    if ((ppdev->NvDmaBufferFlat = EngAllocMem(0, 0x8000, ALLOC_TAG)) != NULL)
        {
        ppdev->NvScanlineBufferFlat = ppdev->NvDmaBufferFlat;
        if (NvAllocContextDma(  ppdev->hDriver,
                                ppdev->hClient,
                                NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                                NV01_CONTEXT_DMA,
                                NVOS03_FLAGS_ACCESS_READ_WRITE,
                                (PVOID) ppdev->NvDmaBufferFlat,
                                0x8000 - 1
                                ) != NVOS03_STATUS_SUCCESS)

        goto bNV3CreateStdPatches_ReturnFalse;
        }

    //************************************************************************
    // Allocate Notifier contexts.
    //************************************************************************
    ((NvNotification *) ppdev->Notifiers->DmaToMem)->status = 0;

    if (NvAllocContextDma(  ppdev->hDriver,
                            ppdev->hClient,
                            NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY,
                            NV01_CONTEXT_DMA,
                            NVOS03_FLAGS_ACCESS_WRITE_ONLY,
                            (PVOID)(ppdev->Notifiers->DmaToMem),
                            sizeof(NvNotification) - 1
                            ) != NVOS03_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot allocate notifier context"));
        goto bNV3CreateStdPatches_ReturnFalse;
        }

    NV_CREATE_OBJECT_INIT();

    NV_CREATE_OBJECT_PIO1(NV_VIDEO_COLORMAP, DD_COLORMAP);

    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_VIDEO, DD_PATCHCORD_COLORMAP);

    // create ROP ANDs
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_ROP_AND, DD_IMAGE_ROP_AND0);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_ROP_AND, DD_IMAGE_ROP_AND1);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_ROP_AND, DD_IMAGE_ROP_AND2);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_ROP_AND, DD_IMAGE_ROP_AND3);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_ROP_AND, DD_IMAGE_ROP_AND4);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_ROP_AND, DD_IMAGE_ROP_AND5);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_ROP_AND, DD_IMAGE_ROP_AND6);

    // create Image Stencils
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_STENCIL, DD_IMAGE_STENCIL0);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_STENCIL, DD_IMAGE_STENCIL1);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_STENCIL, DD_IMAGE_STENCIL2);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_STENCIL, DD_IMAGE_STENCIL3);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_STENCIL, DD_IMAGE_STENCIL4);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_STENCIL, DD_IMAGE_STENCIL5);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_STENCIL, DD_IMAGE_STENCIL6);

    // create Image Color Keys
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_COLOR_KEY, DD_IMAGE_COLOR_KEY0);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_COLOR_KEY, DD_IMAGE_COLOR_KEY1);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_COLOR_KEY, DD_IMAGE_COLOR_KEY2);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_COLOR_KEY, DD_IMAGE_COLOR_KEY3);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_COLOR_KEY, DD_IMAGE_COLOR_KEY4);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_COLOR_KEY, DD_IMAGE_COLOR_KEY5);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_COLOR_KEY, DD_IMAGE_COLOR_KEY6);
    if (NV_CREATE_OBJECT_FAIL())
        return FALSE;


    NV_CREATE_OBJECT_INIT();
    // create Video Color Keys
    NV_CREATE_OBJECT_PIO1(NV_VIDEO_COLOR_KEY, NV_DD_VIDEO_COLOR_KEY);

    // create Image In Memory
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_IN_MEMORY, DD_PRIMARY_IMAGE_IN_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_IN_MEMORY, DD_SRC_IMAGE_IN_MEMORY);

    // create Video from Memory
    NV_CREATE_OBJECT_PIO1(NV_VIDEO_FROM_MEMORY, DD_PRIMARY_VIDEO_FROM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_VIDEO_FROM_MEMORY, NV_DD_YUV422_VIDEO_FROM_MEMORY);

    // create Video Sink
    NV_CREATE_OBJECT_PIO1(NV_VIDEO_SINK, DD_VIDEO_SINK);
    if (NV_CREATE_OBJECT_FAIL())
        return FALSE;

    NV_CREATE_OBJECT_INIT();
    // create Image Patchcords
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_FROM_CPU);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_BLIT);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_RECT_AND_TEXT);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_SOLID_RECT);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_PATTERN);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_BLACK_RECT);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_SOLID);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_ROP_AND0);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_ROP_AND1);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_ROP_AND2);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_ROP_AND3);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_ROP_AND4);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_ROP_AND5);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_ROP_AND6);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_STENCIL0);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_STENCIL1);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_STENCIL2);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_STENCIL3);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_STENCIL4);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_STENCIL5);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_STENCIL6);
    if (NV_CREATE_OBJECT_FAIL())
        return FALSE;

    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE0);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE1);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE2);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE3);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE4);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE5);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE6);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_IMAGE_FEEDBACK);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, DD_PATCHCORD_SRC_FEEDBACK);

    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_STRETCHED_IMAGE_FROM_CPU);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_STRETCHED_UV_IMAGE_FROM_CPU);
    if (NV_CREATE_OBJECT_FAIL())
        return FALSE;

    NV_CREATE_OBJECT_INIT();
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_IMAGE, NV_DD_P_I_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY);

    NV_CREATE_OBJECT_PIO1(NV_SCALED_YUV420_FROM_MEMORY, NV_DD_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_SCALED_YUV420_FROM_MEMORY, NV_DD_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY);


    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_VIDEO, DD_PATCHCORD_VIDEO);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_VIDEO, NV_DD_P_V_VIDEO_COLOR_KEY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_VIDEO, NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY);
    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_VIDEO, NV_DD_P_V_VIDEO_SCALER);

    NV_CREATE_OBJECT_PIO1(NV_PATCHCORD_ROP, DD_PATCHCORD_ROP5_SOLID);
    if (NV_CREATE_OBJECT_FAIL())
        return FALSE;

    NV_CREATE_OBJECT_INIT();
    // create video scaler
    NV_CREATE_OBJECT_PIO1(NV_VIDEO_SCALER, NV_DD_VIDEO_SCALER);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_FROM_CPU, DD_IMAGE_FROM_CPU);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_BLIT, DD_IMAGE_BLIT);
    NV_CREATE_OBJECT_PIO1(NV_RENDER_GDI0_RECTANGLE_AND_TEXT, DD_RENDER_RECT_AND_TEXT);
    NV_CREATE_OBJECT_PIO1(NV_RENDER_SOLID_RECTANGLE, DD_RENDER_SOLID_RECTANGLE);
    NV_CREATE_OBJECT_PIO1(NV_ROP5_SOLID, DD_ROP5_SOLID);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_PATTERN, DD_IMAGE_PATTERN);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_BLACK_RECTANGLE, DD_IMAGE_BLACK_RECTANGLE);
    NV_CREATE_OBJECT_PIO1(NV_IMAGE_SOLID, DD_IMAGE_SOLID);
    NV_CREATE_OBJECT_PIO1(NV_MEMORY_TO_MEMORY_FORMAT, NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT);
    NV_CREATE_OBJECT_PIO1(NV_MEMORY_TO_MEMORY_FORMAT, NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT);
    NV_CREATE_OBJECT_PIO1(NV_STRETCHED_IMAGE_FROM_CPU, NV_DD_STRETCHED_UV_IMAGE_FROM_CPU);
    NV_CREATE_OBJECT_PIO1(NV_STRETCHED_IMAGE_FROM_CPU, NV_DD_STRETCHED_IMAGE_FROM_CPU);
    if (ppdev->cBitsPerPel > 8)
        {
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
        NV_CREATE_OBJECT_PIO1(NV_SCALED_IMAGE_FROM_MEMORY, NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY);
        }
    if (NV_CREATE_OBJECT_FAIL())
        return FALSE;

    // setup patch 0
    while (NvGetFreeCount(nv, 0) < 14*4);
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_ROP_AND0;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_FROM_CPU;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[2] =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetRopInput = DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageOutput
        = DD_PATCHCORD_IMAGE_ROP_AND0;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_STENCIL0;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_ROP_AND0;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_BLACK_RECT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageOutput =
        DD_PATCHCORD_IMAGE_STENCIL0;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject =
        DD_IMAGE_COLOR_KEY0;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_STENCIL0;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageOutput =
        DD_PATCHCORD_IMAGE0;

    // setup patch 1
    while (NvGetFreeCount(nv, 0) < 14*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject =
        DD_IMAGE_ROP_AND1;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_BLIT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[2] =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetRopInput =
        DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageOutput =
        DD_PATCHCORD_IMAGE_ROP_AND1;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject =
        DD_IMAGE_STENCIL1;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_ROP_AND1;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_BLACK_RECT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageOutput =
        DD_PATCHCORD_IMAGE_STENCIL1;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject =
        DD_IMAGE_COLOR_KEY1;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_STENCIL1;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageOutput =
        DD_PATCHCORD_IMAGE1;

    // setup patch 2
    while (NvGetFreeCount(nv, 0) < 14*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject =
        DD_IMAGE_ROP_AND2;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[1] =
            DD_PATCHCORD_IMAGE_RECT_AND_TEXT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[2] =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetRopInput =
        DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageOutput =
        DD_PATCHCORD_IMAGE_ROP_AND2;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_STENCIL2;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_ROP_AND2;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_BLACK_RECT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageOutput =
        DD_PATCHCORD_IMAGE_STENCIL2;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_COLOR_KEY2;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_STENCIL2;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageOutput =
        DD_PATCHCORD_IMAGE2;

    // setup patch 3
    while (NvGetFreeCount(nv, 0) < 14*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_ROP_AND3;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_SOLID_RECT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[2] =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetRopInput =
        DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageOutput =
        DD_PATCHCORD_IMAGE_ROP_AND3;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_STENCIL3;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_ROP_AND3;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_BLACK_RECT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageOutput =
        DD_PATCHCORD_IMAGE_STENCIL3;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_COLOR_KEY3;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_STENCIL3;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageOutput =
        DD_PATCHCORD_IMAGE3;

    // setup patch 4
    while (NvGetFreeCount(nv, 0) < 14*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_ROP_AND4;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[1] =
        NV_DD_P_I_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[2] =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetRopInput =
        DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageOutput =
        DD_PATCHCORD_IMAGE_ROP_AND4;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_STENCIL4;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_ROP_AND4;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_BLACK_RECT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageOutput =
        DD_PATCHCORD_IMAGE_STENCIL4;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_COLOR_KEY4;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_STENCIL4;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageOutput =
        DD_PATCHCORD_IMAGE4;

    // setup patch 5
    while (NvGetFreeCount(nv, 0) < 14*4);
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_ROP_AND5;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[1] =
        NV_DD_P_I_STRETCHED_IMAGE_FROM_CPU;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[2] =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetRopInput = DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageOutput
        = DD_PATCHCORD_IMAGE_ROP_AND5;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_STENCIL5;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_ROP_AND5;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_BLACK_RECT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageStencil.SetImageOutput =
        DD_PATCHCORD_IMAGE_STENCIL5;
    nv->subchannel[BLIT_SUBCHANNEL].SetObject =
        DD_IMAGE_COLOR_KEY5;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageInput[1] =
        DD_PATCHCORD_IMAGE_STENCIL5;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageColorKey.SetImageOutput =
        DD_PATCHCORD_IMAGE5;

    // setup patch 6
    while (NvGetFreeCount(nv, 0) < 14*4);
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_ROP_AND6;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[0] =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[1] =
        NV_DD_P_I_STRETCHED_UV_IMAGE_FROM_CPU;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageInput[2] =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetRopInput = DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageRopAnd.SetImageOutput
        = DD_PATCHCORD_IMAGE_ROP_AND6;

    // setup patch back end
    while (NvGetFreeCount(nv, 0) < 25*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_PRIMARY_IMAGE_IN_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[0] =
        DD_PATCHCORD_IMAGE0;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[1] =
        DD_PATCHCORD_IMAGE1;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[2] =
        DD_PATCHCORD_IMAGE2;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[3] =
        DD_PATCHCORD_IMAGE3;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[4] =
        DD_PATCHCORD_IMAGE4;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[5] =
        DD_PATCHCORD_IMAGE5;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[6] =
        DD_PATCHCORD_IMAGE6;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[7] =
        NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[8] =
        NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[9] =
        NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[10] =
        NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[11] =
        NV_DD_P_I_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[12] =
        NV_DD_P_I_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[13] =
        NV_DD_P_I_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[14] =
        NV_DD_P_I_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[15] =
        NV_DD_P_I_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[16] =
        NV_DD_P_I_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[17] =
        NV_DD_P_I_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageInput[18] =
        DD_PATCHCORD_IMAGE_ROP_AND6;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageOutput =
        DD_PATCHCORD_IMAGE_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetColorFormat =
            colorFmtPack;

    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImagePitch = ppdev->lDelta;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageOffset = ppdev->ulPrimarySurfaceOffset;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageCtxDma =
        DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

    //***************************************************************************
    // Setup src image in memory for device bitmaps.
    //***************************************************************************
    while (NvGetFreeCount(nv, 0) < 6*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_SRC_IMAGE_IN_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageOutput =
        DD_PATCHCORD_SRC_FEEDBACK;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetColorFormat =
            colorFmtPack;

    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImagePitch = ppdev->lDelta;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageOffset = ppdev->ulPrimarySurfaceOffset;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageInMemory.SetImageCtxDma =
        DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

    // setup Video from memory object
    while (NvGetFreeCount(nv, 0) < 14*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_PRIMARY_VIDEO_FROM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetVideoOutput =
        DD_PATCHCORD_VIDEO;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].offset = ppdev->ulPrimarySurfaceOffset;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].pitch =
        ppdev->lDelta;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].size =
        (ppdev->cyScreen << 16) | (ppdev->cxScreen & 0xffff);
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].format =
        videoFmt;

    //**************************************************************************
    //
    // POSSIBLE PROBLEM ??  Or just a typo that was missed?
    //
    //        -> The following line used to be  ....nv4VideoFromMemory.ImageScan[0].notify.
    //           Now that this code has been separated out from ENABLE.C,
    //           this line has been changed to  ....nvVideoFromMemory.ImageScan[0].notify.
    //           in order for it to compile.   Essentially, the old obsolete
    //           nv3 classes are still being used in this file.
    //
    //           I don't know why .nv4VideoFromMemory was being used here,
    //           instead of nvVideoFromMemory.  First of all, nv4VideoFromMemory
    //           doesn't exist in the OLDNV332.H file.  And second, the structure
    //           OFFSETS for 'nvVideoFromMemory' vs nv4VideoFromMemory are DIFFERENT!
    //
    //           Bottom line: This looks like a typo that's been here forever.
    //                        It's now been changed, and should now be 'correct'.
    //                        However, since there's never been a problem with this before,
    //                        it's good to be aware of this change.   
    //
    //**************************************************************************

// REMOVED...causes lockup..ask!
//    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].notify = 0;

    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].offset = ppdev->ulPrimarySurfaceOffset;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].pitch =
        ppdev->lDelta;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].size =
        (ppdev->cyScreen << 16) | (ppdev->cxScreen & 0xffff);
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].format =
        videoFmt;

    //**************************************************************************
    //
    // POSSIBLE PROBLEM ??  Or just a typo that was missed?
    //
    //        -> The following line used to be  ....nv4VideoFromMemory.ImageScan[0].notify.
    //           Now that this code has been separated out from ENABLE.C,
    //           this line has been changed to  ....nvVideoFromMemory.ImageScan[0].notify.
    //           in order for it to compile.   Essentially, the old obsolete
    //           nv3 classes are still being used in this file.
    //
    //           I don't know why .nv4VideoFromMemory was being used here,
    //           instead of nvVideoFromMemory.  First of all, nv4VideoFromMemory
    //           doesn't exist in the OLDNV332.H file.  And second, the structure
    //           OFFSETS for 'nvVideoFromMemory' vs nv4VideoFromMemory were DIFFERENT!
    //
    //           Bottom line: This looks like a typo that's been here forever.
    //                        It's now been changed, and should now be 'correct'.
    //                        However, since there's never been a problem with this before,
    //                        it's good to be aware of this change.   
    //
    //**************************************************************************


// REMOVED...causes lockup..ask!
//    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].notify = 0;

    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetImageNotifyCtxDma[0] =
        NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetImageNotifyCtxDma[1] =
        NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

    // setup Image From CPU in subchannel 0
    while (NvGetFreeCount(nv, 0) < 3*4);

    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].SetObject = DD_IMAGE_FROM_CPU;
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nvImageFromCpu.SetImageOutput =
        DD_PATCHCORD_IMAGE_FROM_CPU;
    nv->subchannel[IMAGE_FROM_CPU_SUBCHANNEL].nvImageFromCpu.SetColorFormat =
        colorFmtPack;

    // setup Image BLIT in subchannel 1
    while (NvGetFreeCount(nv, 0) < 3*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_BLIT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageBlit.SetImageOutput =
        DD_PATCHCORD_IMAGE_BLIT;
    nv->subchannel[BLIT_SUBCHANNEL].nvImageBlit.SetImageInput =
        DD_PATCHCORD_SRC_FEEDBACK;

    // setup RECT and TEXT in subchannel 2
    while (NvGetFreeCount(nv, 0) < 6*4);

    nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].SetObject =
        DD_RENDER_RECT_AND_TEXT;
    nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nvRenderGdi0RectangleAndText.SetImageOutput =
        DD_PATCHCORD_IMAGE_RECT_AND_TEXT;
    nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nvRenderGdi0RectangleAndText.SetColorFormat =
        colorFmtAlpha;
    nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nvRenderGdi0RectangleAndText.SetMonochromeFormat =
        NV_MONO_CGA6_M1_P32;
    nv->subchannel[RECT_AND_TEXT_SUBCHANNEL].nvRenderGdi0RectangleAndText.SetNotifyCtxDma =
        DD_FIFO_SYNC_NOTIFIER;



    // setup Render Solid Rectangle in subchannel 3
    while (NvGetFreeCount(nv, 0) < 3*4);

    nv->subchannel[RECTANGLE_SUBCHANNEL].SetObject = DD_RENDER_SOLID_RECTANGLE;
    nv->subchannel[RECTANGLE_SUBCHANNEL].nvRenderSolidRectangle.SetImageOutput = DD_PATCHCORD_IMAGE_SOLID_RECT;
    nv->subchannel[RECTANGLE_SUBCHANNEL].nvRenderSolidRectangle.SetColorFormat = colorFmtUnpack;

    // setup ROP5 Solid in subchannel 4
    while (NvGetFreeCount(nv, 0) < 3*4);

    nv->subchannel[ROP_SOLID_SUBCHANNEL].SetObject = DD_ROP5_SOLID;
    nv->subchannel[ROP_SOLID_SUBCHANNEL].nvRop5Solid.SetRopOutput =
        DD_PATCHCORD_ROP5_SOLID;
    nv->subchannel[ROP_SOLID_SUBCHANNEL].nvRop5Solid.SetRop5 = 0xcc;

    // setup Image Pattern in subchannel 5
    while (NvGetFreeCount(nv, 0) < 9*4);

    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].SetObject = DD_IMAGE_PATTERN;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetImageOutput =
        DD_PATCHCORD_IMAGE_PATTERN;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetColorFormat =
        colorFmtAlpha;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetMonochromeFormat =
        NV_MONO_CGA6_M1_P32;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetPatternShape =
        NV_PATTERN_SHAPE_64X1;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetColor0 =
        color0;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetColor1 =
        color1;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetPattern.monochrome[0] =
        0xffffffff;
    nv->subchannel[IMAGE_PATTERN_SUBCHANNEL].nvImagePattern.SetPattern.monochrome[1] =
        0x50505050;

    // setup Image Rectangle Black in subchannel 6
    while (NvGetFreeCount(nv, 0) < 4*4);

    nv->subchannel[CLIP_SUBCHANNEL].SetObject = DD_IMAGE_BLACK_RECTANGLE;
    nv->subchannel[CLIP_SUBCHANNEL].nvImageBlackRectangle.SetImageOutput =
        DD_PATCHCORD_IMAGE_BLACK_RECT;
    nv->subchannel[CLIP_SUBCHANNEL].nvImageBlackRectangle.SetRectangle.y_x = 0;
    nv->subchannel[CLIP_SUBCHANNEL].nvImageBlackRectangle.SetRectangle.height_width =
        ((0x7fff<<16) | 0x7fff);

    // setup Image Solid in subchannel 7
    while (NvGetFreeCount(nv, 0) < 4*4);

    nv->subchannel[DD_SPARE].SetObject = DD_IMAGE_SOLID;
    nv->subchannel[DD_SPARE].nvImageSolid.SetColorFormat = colorFmtAlpha;
    nv->subchannel[DD_SPARE].nvImageSolid.SetImageOutput =
        DD_PATCHCORD_IMAGE_SOLID;
    nv->subchannel[DD_SPARE].nvImageSolid.SetColor = 0L;  //Disable color keying
    ppdev->dDrawSpareSubchannelObject = DD_IMAGE_SOLID;

    while (NvGetFreeCount(nv, 0) < 9*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_COLORMAP;

    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetVideoInput =
        DD_PATCHCORD_VIDEO;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetVideoOutput =
        DD_PATCHCORD_COLORMAP;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetNotifyCtxDma =
        NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetColormapCtxDma =
        NV_WIN_COLORMAP_CONTEXT;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetColormapFormat =
        NV_COLORMAP_X8R8G8B8;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetColormapStart = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetNotify = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorMap.SetColormapLength =
        256 * sizeof(VIDEO_CLUTDATA);
    while (((NvNotification *) (ppdev->Notifiers->DmaToMem))->status == NV_STATUS_IN_PROGRESS);

    while (NvGetFreeCount(nv, 0) < 2*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_VIDEO_SINK;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoSink.SetVideoInput[0] =
        NV_DD_P_V_VIDEO_COLOR_KEY;

    //************************************************************************
    // Create the video memory to system memory format object.
    //************************************************************************
    while (NvGetFreeCount(nv, 0) < 5*4);
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetNotifyCtxDma =
        NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetBufferInCtxDma =
        DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetBufferOutCtxDma =
        NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetBufferNotifyCtxDma =
        NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

    //**************************************************************************
    // create memory to memory format object used to reformat overlay surfaces
    //**************************************************************************
    while (NvGetFreeCount(nv, 0) < 5*4);
    nv->subchannel[BLIT_SUBCHANNEL].SetObject =
       NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetNotifyCtxDma =
       NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetBufferInCtxDma =
       DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetBufferOutCtxDma =
       DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[BLIT_SUBCHANNEL].nvMemoryToMemoryFormat.SetBufferNotifyCtxDma =
       NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

    //*******************************************************************************
    // Create objects needed for video overlay.
    //*******************************************************************************
    while (NvGetFreeCount(nv, BLIT_SUBCHANNEL) < 15*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = NV_DD_VIDEO_COLOR_KEY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetVideoOutput =
        NV_DD_P_V_VIDEO_COLOR_KEY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetVideoInput[0] =
        DD_PATCHCORD_VIDEO;

    if (ppdev->cBitsPerPel == 8)
        nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetVideoInput[1] =
            DD_PATCHCORD_COLORMAP;
    else
        nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetVideoInput[1] =
            DD_PATCHCORD_VIDEO;

    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetVideoInput[2] =
        NV_DD_P_V_VIDEO_SCALER;

    if (ppdev->cBitsPerPel == 8)
        nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetColorFormat =
            NV_COLOR_FORMAT_LE_X16A8Y8;
    else if (ppdev->cBitsPerPel == 16)
        nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetColorFormat =
            NV_COLOR_FORMAT_LE_X16A1R5G5B5;
    else
        nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetColorFormat =
            NV_COLOR_FORMAT_LE_A8R8G8B8;


    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetColorKey = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetPoint = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoColorKey.SetSize = 0;

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = NV_DD_VIDEO_SCALER;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoScaler.SetVideoOutput =
       NV_DD_P_V_VIDEO_SCALER;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoScaler.SetVideoInput =
       NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoScaler.SetDeltaDuDx = 0x100000;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoScaler.SetDeltaDvDy = 0x100000;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoScaler.SetPoint = 0;


    while (NvGetFreeCount(nv, BLIT_SUBCHANNEL) < 17*4);

    nv->subchannel[BLIT_SUBCHANNEL].SetObject = NV_DD_YUV422_VIDEO_FROM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetVideoOutput =
        NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetImageCtxDma[0] =
        DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetImageCtxDma[1] =
        DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetImageNotifyCtxDma[0] =
        NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.SetImageNotifyCtxDma[1] =
        NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].offset = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].pitch = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].size = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].format =
        NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[0].notify = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].offset = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].pitch = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].size = 0;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].format =
        NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;
    nv->subchannel[BLIT_SUBCHANNEL].nvVideoFromMemory.ImageScan[1].notify = 0;

    if (ppdev->cBitsPerPel > 8)
        {
        //*****************************************************************************
        // Hook up scaled image from mem objects.
        //*****************************************************************************
        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
        if (ppdev->cBitsPerPel == 16)
            nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
                NV_COLOR_FORMAT_LE_X1R5G5B5_P2;
        else
            nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
                NV_COLOR_FORMAT_LE_X8R8G8B8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
        if (ppdev->cBitsPerPel == 16)
            nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
                NV_COLOR_FORMAT_LE_X1R5G5B5_P2;
        else
            nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
                NV_COLOR_FORMAT_LE_X8R8G8B8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_Y8V8Y8U8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_Y8V8Y8U8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_V8Y8U8Y8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_V8Y8U8Y8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_Y8V8Y8U8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_Y8V8Y8U8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_V8Y8U8Y8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 5*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetColorFormat =
            NV_COLOR_FORMAT_LE_V8Y8U8Y8;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledImageFromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 4*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledYuv420FromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledYuv420FromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledYuv420FromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        while (NvGetFreeCount(nv, 0) < 4*4);

        nv->subchannel[BLIT_SUBCHANNEL].SetObject =
            NV_DD_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledYuv420FromMemory.SetNotifyCtxDma =
            NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledYuv420FromMemory.SetImageOutput =
            NV_DD_P_I_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY;
        nv->subchannel[BLIT_SUBCHANNEL].nvScaledYuv420FromMemory.SetImageCtxDma =
            NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
        }

    while (NvGetFreeCount(nv, 0) < 3*4);
    nv->subchannel[DD_STRETCH].SetObject =
        NV_DD_STRETCHED_IMAGE_FROM_CPU;
    nv->subchannel[DD_STRETCH].nvStretchedImageFromCpu.SetImageOutput =
        NV_DD_P_I_STRETCHED_IMAGE_FROM_CPU;
    nv->subchannel[DD_STRETCH].nvStretchedImageFromCpu.SetColorFormat =
        colorFmtPack;
    ppdev->dDrawSpareSubchannelObject = NV_DD_STRETCHED_IMAGE_FROM_CPU;

    while (NvGetFreeCount(nv, 0) < 3*4);
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = NV_DD_STRETCHED_UV_IMAGE_FROM_CPU;
    nv->subchannel[BLIT_SUBCHANNEL].nvStretchedImageFromCpu.SetImageOutput =
        NV_DD_P_I_STRETCHED_UV_IMAGE_FROM_CPU;
    nv->subchannel[BLIT_SUBCHANNEL].nvStretchedImageFromCpu.SetColorFormat =
        NV_COLOR_FORMAT_LE_Y8_P4;

    while (NvGetFreeCount(nv, 0) < 4);
    nv->subchannel[BLIT_SUBCHANNEL].SetObject = DD_IMAGE_BLIT;

    //******************************************************************************
    // Cache the freecount
    //******************************************************************************
    ppdev->NVFreeCount = NvGetFreeCount(nv, 0);

    return TRUE;




bNV3CreateStdPatches_ReturnFalse:

    //**************************************************************************
    // Remove ALL DMA contexts that we previously allocated!
    //**************************************************************************

    vNV3DestroyStdPatches(ppdev,TRUE);
    return(FALSE);


}

VOID vNV3DestroyStdPatches(
    PDEV *ppdev,
    BOOL  CreateStdPatchFailedFlag
)
{
    // get the FIFO pointer from the physical device descriptor
    Nv3ChannelPio *nv = (Nv3ChannelPio *)ppdev->pjMmBase;
    ULONG                   status;

    DISPDBG((1, "DD destroying standard patches..."));

    //******************************************************************************
    // Ensure all hw processing has completed prior to destroying objects.
    // WaitEngineBusy function is NOT yet initialized if we just came from
    // a bNV3CreateStdPatches call, which failed.   So skip it.
    //******************************************************************************

    if (CreateStdPatchFailedFlag != TRUE)
        ppdev->pfnWaitEngineBusy(ppdev);

    //******************************************************************************
    // Invalidate cached freecount
    //******************************************************************************
    ppdev->NVFreeCount = 0;

    //**************************************************************************
    // Relinquish -> user channel
    //**************************************************************************
    if (ppdev->pjMmBase != NULL)
        {

        status = NvFree(ppdev->hDriver,
                        ppdev->hClient,
                        ppdev->hDevice,
                        ppdev->hPioChannel);

        if (status != NVOS00_STATUS_SUCCESS)
            {
            DISPDBG((1, "Failed bAssertModeHardware"));
            return;
            }

        ASSERTDD(status == NVOS00_STATUS_SUCCESS,
                 "bAssertModeHardware: Failed to free NV user channel.");

        ppdev->pjMmBase = NULL;
        ppdev->ddChannelPtr = NULL;
        }


    //**********************************************************************
    // Destroy the notifier context (from system memory)
    //**********************************************************************

    if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY) != NVOS00_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot free notifier context"));
        }

    //**********************************************************************
    // Destroy the colormap context (from system memory)
    //**********************************************************************

    if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, NV_WIN_COLORMAP_CONTEXT) != NVOS00_STATUS_SUCCESS)
        {
        DISPDBG((1, "NVDD: Cannot free buffer context"));
        }

    //*************************************************************************
    // Destroy video flip notifier.
    //*************************************************************************
        if (NvFree(ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDevice,
            NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY) != NVOS00_STATUS_SUCCESS)
            {
            DISPDBG((1, "NVDD: Cannot free buffer context"));
            }

    //*************************************************************************
    // Destroy overlay flip notifier.
    //*************************************************************************
        if (NvFree(ppdev->hDriver,
            ppdev->hClient,
            ppdev->hDevice,
            NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY) != NVOS00_STATUS_SUCCESS)
            {
            DISPDBG((1, "NVDD: Cannot overlay notifier"));
            }

    //*************************************************************************
    // Destroy CONTEXT_DMA_IN_MEMORY object used to reference video memory.
    //*************************************************************************
    if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM) != NVOS00_STATUS_SUCCESS)
            {
            DISPDBG((1, "NVDD: Cannot video mem context"));
            }

    //*************************************************************************
    // Destroy FIFO sync notifier.
    //*************************************************************************
    if (NvFree(ppdev->hDriver, ppdev->hClient, ppdev->hDevice, DD_FIFO_SYNC_NOTIFIER) != NVOS00_STATUS_SUCCESS)
            {
            DISPDBG((1, "NVDD: Cannot free fifo sync notifier"));
            }

    //**********************************************************************************
    // Destroy DMA context for 2D/DDraw DMA Buffer.
    //**********************************************************************************
    if (NvFree(ppdev->hDriver,
                ppdev->hClient,
                ppdev->hDevice,
                NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
        {
        DISPDBG((1, "DisableOGLPatch: Cannot free 2D DMA buffer context"));
        }

    //**********************************************************************************
    // Destroy DMA notifier context for 2D/DDraw DMA Buffer.
    //**********************************************************************************
    if (ppdev->NvDmaBufferFlat)
        {
        if (NvFree(ppdev->hDriver,
                    ppdev->hClient,
                    ppdev->hDevice,
                    NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY) != NVOS00_STATUS_SUCCESS)
            {
            DISPDBG((1, "DisableOGLPatch: Cannot free 2D DMA buffer context"));
            }
        EngFreeMem((PVOID) ppdev->NvDmaBufferFlat);
        }

    ppdev->NvDmaBufferFlat = ppdev->NvScanlineBufferFlat = 0;

    return;
}
#endif // NV3
