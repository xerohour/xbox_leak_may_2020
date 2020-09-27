
/*
 ***************************************************************************
 *                                                                         *
 *  Copyright (C) 1995, 1998 NVidia Corporation.  All Rights Reserved.     *
 *                                                                         *
 ***************************************************************************
 */

/* NvDDEnable32() */

#include "windows.h"
#include <conio.h>
#include "nvd3ddrv.h"
#include "nv32.h"
#include "nvddobj.h"
#include "ddrvmem.h"
#include "d3dinc.h"

#ifdef  MULTI_MON
#include "nvrmapi.h"
#include "nvrmarch.inc"
#include "nvos.h"
#else   /* MULTI_MON */
#define NV_ERROR_NOTIFIER          0x0FF5
#endif  /* MULTI_MON */

#ifdef NVHEAPMGR
#ifdef MULTI_MON
static NVIOControl_Param HeapParams;
#else   // !MULTI_MON
static struct
{
  U032 function;
  U032 device;
  U032 owner;
  U032 type;
  U032 depth;
  U032 width;
  U032 height;
  S032 pitch;
  U032 offset;
  U032 size;
  U032 address;
  U032 limit;
  U032 total;
  U032 free;
  V032 status;
} HeapParams;
#endif  // !MULTI_MON
#endif  // NVHEAPMGR

/*
 * NvDDEnable32()
 *
 */

int
NvDDEnable32()
{
   NvNotification *npPioDmaToMemNotifier;
   NvNotification *npPioFlipOverlayNotifier;
   NvNotification *npDmaDmaToMemNotifier;
   NvNotification *npDmaDmaFromMemNotifier;
   NvNotification *npDmaFlipPrimaryNotifier;
   NvNotification *npDmaPusherSyncNotifier;
   NvNotification *npDmaSyncNotifier;
   FAST Nv4ControlDma *npDev;
   Nv3ChannelPio *npDevVideo;
   unsigned long temp;
   FAST long videoFreeCount;
   int pxlDepth;
   unsigned long *dmaPusherPutAddress =
       (unsigned long *)pDriverData->NvDmaPusherPutAddress;
   unsigned long *dmaPusherBufferBase =
       (unsigned long *)pDriverData->NvDmaPusherBufferBase;
   long freeCount;
   DWORD tmpIndex;

   npDev = (Nv4ControlDma *)pDriverData->NvDevFlatDma;

   pxlDepth = pDriverData->bi.biBitCount;

   /*
    * Allocate DMA notifiers and pixel/texel buffers
    *
    * Request 8 pages (NV_DD_COMMON_DMA_BUFFER_SIZE bytes) of memory.
    * Allows 2 buffers where 1 buffer can use
    * (NV_DD_COMMON_DMA_BUFFER_SIZE - (sizeof(NvNotification) << 4))/2 bytes
    * with up to 16 separate notifiers at the beginning of the buffer.
    *
    * Actual memory allocation done it 16 bit portion of driver.
    */

   pDriverData->NvDmaDmaToMemNotifierFlat = pDriverData->NvCommonDmaBufferFlat;

   pDriverData->NvDmaBufferNotifierFlat = pDriverData->NvCommonDmaBufferFlat;

   pDriverData->NvDmaDmaFromMemNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
       (sizeof(NvNotification) << 1);

   pDriverData->NvDmaPusherSyncNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
       (sizeof(NvNotification) << 1) + sizeof(NvNotification);

   pDriverData->NvDmaSyncNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
       (sizeof(NvNotification) << 2);

   /* Notifier array offsets 5, 6, 7, 8, and 9 */
   pDriverData->NvDmaFlipPrimaryNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
       (sizeof(NvNotification) << 2) + sizeof(NvNotification);

//   /* Notifier array offsets A, B, C, D, and E */
//   pDriverData->NvPioFlipOverlayNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
//       (sizeof(NvNotification) << 3) + (sizeof(NvNotification) << 1);

   /* Notifier array offset (sizeof(NvNotification) << 3) + (sizeof(NvNotification) << 2) + (sizeof(NvNotification) << 1) + sizeof(NvNotification) currently unused */

   pDriverData->NvScanlineBufferFlat = pDriverData->NvCommonDmaBufferFlat +
       (sizeof(NvNotification) << 4);

   npDmaDmaToMemNotifier = (NvNotification *)pDriverData->NvDmaDmaToMemNotifierFlat;
   npDmaDmaFromMemNotifier = (NvNotification *)pDriverData->NvDmaDmaFromMemNotifierFlat;
   npDmaPusherSyncNotifier = (NvNotification *)pDriverData->NvDmaPusherSyncNotifierFlat;
   npDmaSyncNotifier = (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
   npDmaFlipPrimaryNotifier = (NvNotification *)pDriverData->NvDmaFlipPrimaryNotifierFlat;

   /* Deal with possible mode change during pending flip */
   if (npDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) {
       long countDown = 0x200000;
       while ((npDmaFlipPrimaryNotifier[1].status == NV_IN_PROGRESS) && (--countDown > 0));
   }
   /* Deal with possible mode change during pending flip */
   if (npDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS) {
       long countDown = 0x200000;
       while ((npDmaFlipPrimaryNotifier[2].status == NV_IN_PROGRESS) && (--countDown > 0));
   }

   /* MUST MAKE SURE STATUS FLAG IS CLEAR BEFORE INITIAL USE */

   npDmaDmaToMemNotifier->status = 0;

   npDmaDmaFromMemNotifier->status = 0;

   npDmaSyncNotifier->status = 0;

   npDmaFlipPrimaryNotifier[1].status = 0;
   npDmaFlipPrimaryNotifier[2].status = 0;


   /* First create some objects */

   freeCount = 0;

#ifndef MULTI_MON
   while (freeCount < 14)
       NvGetDmaBufferFreeCount(npDev, freeCount, 14, dmaPusherPutAddress);
   freeCount -= 14;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(0) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(0) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(0) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[9] = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[10] = dDrawSubchannelOffset(0) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[12] = dDrawSubchannelOffset(0) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[13] = NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY;

   dmaPusherPutAddress += 14;

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[1] = NV_CONTEXT_DMA_FROM_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[9] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[10] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

   dmaPusherPutAddress += 12;

   while (freeCount < 4)
       NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
   freeCount -= 4;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[1] = NV_CONTEXT_DMA_IN_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;

   dmaPusherPutAddress += 4;

#endif  /* MULTI_MON */
   /*
    * Create ddraw objects.
    */
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_CONTEXT_ROP, NV3_CONTEXT_ROP) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_CONTEXT_PATTERN, NV4_CONTEXT_PATTERN) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_IMAGE_BLACK_RECTANGLE, NV1_IMAGE_BLACK_RECTANGLE) != NVOS05_STATUS_SUCCESS)
    return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_CONTEXT_COLOR_KEY, NV4_CONTEXT_COLOR_KEY) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_BETA_SOLID, NV1_BETA_SOLID) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_CONTEXT_BETA4, NV4_CONTEXT_BETA) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_ROP_GDI_RECT_AND_TEXT, NV4_GDI_RECTANGLE_TEXT) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_ABLEND_RECT_AND_TEXT, NV4_GDI_RECTANGLE_TEXT) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_IMAGE_FROM_CPU, NV4_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_ALT_IMAGE_FROM_CPU, NV4_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SRCCOPY_IMAGE_FROM_CPU, NV4_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_ABLEND_NOSRCALPHA_IMAGE_FROM_CPU, NV4_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_ABLEND_IMAGE_FROM_CPU, NV4_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_STRETCHED_UV_IMAGE_FROM_CPU, NV4_STRETCHED_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_STRETCHED_IMAGE_FROM_CPU, NV4_STRETCHED_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SRCCOPY_STRETCHED_IMAGE_FROM_CPU, NV4_STRETCHED_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_ABLEND_NOSRCALPHA_STRETCHED_IMAGE_FROM_CPU, NV4_STRETCHED_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

  if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_ABLEND_STRETCHED_IMAGE_FROM_CPU, NV4_STRETCHED_IMAGE_FROM_CPU) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (pxlDepth > 8) {

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                return(0);

        if (pxlDepth > 16) {
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                    return(0);
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                    return(0);
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                    return(0);
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                    return(0);
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                    return(0);
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY, NV4_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
                    return(0);
       }
   }

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_IMAGE_BLIT, NV4_IMAGE_BLIT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SRCCOPY_IMAGE_BLIT, NV4_IMAGE_BLIT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SURFACES_2D, NV4_CONTEXT_SURFACES_2D) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_PRIMARY_VIDEO_FROM_MEMORY, NV_VIDEO_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY, NV_PATCHCORD_VIDEO) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (pxlDepth == 8) {
    if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_P_V_SHARED_VIDEO_COLORMAP, NV_PATCHCORD_VIDEO) != NVOS05_STATUS_SUCCESS)
            return(0);
    }

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT, NV3_MEMORY_TO_MEMORY_FORMAT) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_VIDEO_SINK, NV_VIDEO_SINK) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (pxlDepth == 8) {

      if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_SHARED_VIDEO_COLORMAP, NV_VIDEO_COLORMAP) != NVOS05_STATUS_SUCCESS)
            return(0);
   }
#ifdef WINNT
   /*
    * Create NV4 DVD subpicture object in both DMA Push and PIO
    * channels. Don't know why the creation of these objects was
    * not in the common code.
    */
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_DVD_SUBPICTURE, NV04_DVD_SUBPICTURE) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_DVD_SUBPICTURE, NV04_DVD_SUBPICTURE) != NVOS05_STATUS_SUCCESS)
        return(0);
#endif // WINNT

    /*
     * Create D3D objects.
     */

    /*
     * Create the DX5 Textured Triangle.
     */
    if (pDriverData->ppdev->flCaps & (CAPS_NV10_IS_PRESENT | CAPS_NV10ULTRA_IS_PRESENT | CAPS_NV10GL_IS_PRESENT))
        {
        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            D3D_DX5_TEXTURED_TRIANGLE, NV10_DX5_TEXTURED_TRIANGLE) != NVOS05_STATUS_SUCCESS)
                return(0);
        }
    else
        if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            D3D_DX5_TEXTURED_TRIANGLE, NV04_DX5_TEXTURED_TRIANGLE) != NVOS05_STATUS_SUCCESS)
                return(0);

    /*
     * Create the DX6 Textured Triangle.
     */
        if (pDriverData->ppdev->flCaps & (CAPS_NV10_IS_PRESENT | CAPS_NV10ULTRA_IS_PRESENT | CAPS_NV10GL_IS_PRESENT))
            {
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                D3D_DX6_MULTI_TEXTURE_TRIANGLE, NV10_DX6_MULTI_TEXTURE_TRIANGLE) != NVOS05_STATUS_SUCCESS)
                    return(0);
            }
        else
            if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                D3D_DX6_MULTI_TEXTURE_TRIANGLE, NV04_DX6_MULTI_TEXTURE_TRIANGLE) != NVOS05_STATUS_SUCCESS)
                    return(0);

    /*
     * Create the ARGB_ZS surface context.
     */
    if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_CONTEXT_SURFACES_ARGB_ZS, NV04_CONTEXT_SURFACES_ARGB_ZS) != NVOS05_STATUS_SUCCESS)
            return(0);

    /*
     * Create the swizzled surface context.
     */
    if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_CONTEXT_SURFACE_SWIZZLED, NV04_CONTEXT_SURFACE_SWIZZLED) != NVOS05_STATUS_SUCCESS)
            return(0);


    /*
     * Create Scaled Image From Memory object for texture swizzling.
     */
    if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_SCALED_IMAGE_FROM_MEMORY, NV04_SCALED_IMAGE_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
            return(0);

    /*
     * Create Render Solid Rectangle for clearing the z-buffer and rendering surfaces.
     */
    if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_RENDER_SOLID_RECTANGLE, NV4_RENDER_SOLID_RECTANGLE) != NVOS05_STATUS_SUCCESS)
            return(0);
    if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        D3D_RENDER_SOLID_RECTANGLE_2, NV4_RENDER_SOLID_RECTANGLE) != NVOS05_STATUS_SUCCESS)
            return(0);

    /*
     * Create Video objects.
     */

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_PRIMARY_VIDEO_FROM_MEMORY, NV_VIDEO_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_YUV422_VIDEO_FROM_MEMORY, NV_VIDEO_FROM_MEMORY) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_VIDEO_COLOR_KEY, NV_VIDEO_COLOR_KEY) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_VIDEO_SCALER, NV_VIDEO_SCALER) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY, NV_PATCHCORD_VIDEO) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY, NV_PATCHCORD_VIDEO) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_P_V_VIDEO_SCALER, NV_PATCHCORD_VIDEO) != NVOS05_STATUS_SUCCESS)
        return(0);
   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_P_V_VIDEO_COLOR_KEY, NV_PATCHCORD_VIDEO) != NVOS05_STATUS_SUCCESS)
        return(0);

   if (pxlDepth == 8) {
    if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
        NV_DD_P_V_SHARED_VIDEO_COLORMAP, NV_PATCHCORD_VIDEO) != NVOS05_STATUS_SUCCESS)
            return(0);

       if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
        NV_DD_SHARED_VIDEO_COLORMAP, NV_VIDEO_COLORMAP) != NVOS05_STATUS_SUCCESS)
            return(0);
   }


#ifdef MULTI_MON

   if (NvRmAllocObject(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
        NV_DD_VIDEO_SINK, NV_VIDEO_SINK) != NVOS05_STATUS_SUCCESS)
            return(0);




   /*
    * Start patching things together (from bottom to top of patch)
    *    and otherwise initializing objects
    */

#ifdef MULTI_MON

   /* Free any existing dynamic context dma objects */
   if (pDriverData->dwVidMemCtxDmaSize != 0) {

       NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
       NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY);
       NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY);

       pDriverData->dwVidMemCtxDmaSize = 0;
   }

   /* Allocate our video memory context dmas */

   pDriverData->dwVidMemCtxDmaSize =
       (unsigned long)(pDriverData->VideoHeapEnd - pDriverData->BaseAddress);
   pDriverData->dwVidMemCtxDmaSize += 4096; /* Make sure to include permanently allocated video memory page */
#ifndef WINNT
   if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                       NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,
                       NV01_CONTEXT_DMA,
                       (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR |
                        ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                       (PVOID)pDriverData->BaseAddress,
                       pDriverData->dwVidMemCtxDmaSize) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);

   if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                       NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY,
                       NV01_CONTEXT_DMA,
                       (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                        ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                       (PVOID)pDriverData->BaseAddress,
                       pDriverData->dwVidMemCtxDmaSize) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);

   if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                       NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY,
                       NV01_CONTEXT_DMA,
                       (ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY |
                        ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                       (PVOID)pDriverData->BaseAddress,
                       pDriverData->dwVidMemCtxDmaSize) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);
#else
   if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                       NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY,
                       NV01_CONTEXT_DMA,
                       (ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR |
                        ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                       (PVOID)pDriverData->ppdev->pjScreen,
                       pDriverData->dwVidMemCtxDmaSize) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);

   if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                       NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY,
                       NV01_CONTEXT_DMA,
                       (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                        ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                       (PVOID)pDriverData->ppdev->pjScreen,
                       pDriverData->dwVidMemCtxDmaSize) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);

   if (NvRmAllocContextDma(pDriverData->dwRootHandle,
                       NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY,
                       NV01_CONTEXT_DMA,
                       (ALLOC_CTX_DMA_FLAGS_ACCESS_WRITE_ONLY |
                        ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED),
                       (PVOID)pDriverData->ppdev->pjScreen,
                       pDriverData->dwVidMemCtxDmaSize) != ALLOC_CTX_DMA_STATUS_SUCCESS)
                            return(FALSE);
#endif // #ifdef WINNT

   if (pxlDepth == 8) {

       while (freeCount < 5)
           NvGetDmaBufferFreeCount(npDev, freeCount, 5, dmaPusherPutAddress);
       freeCount -= 5;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(3) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_SHARED_VIDEO_COLORMAP;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(3) + NVFF9_SET_VIDEO_OUTPUT | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_P_V_SHARED_VIDEO_COLORMAP;
       dmaPusherPutAddress[4] = NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;

       dmaPusherPutAddress += 5;
   }

   while (freeCount < 4)
       NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
   freeCount -= 4;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(5) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_VIDEO_SINK;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(5) + NVFFA_SET_VIDEO_INPUT | 0x40000;

   if (pxlDepth == 8)
       dmaPusherPutAddress[3] = NV_DD_P_V_SHARED_VIDEO_COLORMAP;
   else
       dmaPusherPutAddress[3] = NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;

   dmaPusherPutAddress += 4;

   while (freeCount < 16)
       NvGetDmaBufferFreeCount(npDev, freeCount, 16, dmaPusherPutAddress);
   freeCount -= 16;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(4) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(4) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress[4] = dDrawSubchannelOffset(5) | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(5) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress[8] = dDrawSubchannelOffset(6) | 0x40000;
   dmaPusherPutAddress[9] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[10] = dDrawSubchannelOffset(6) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress[12] = dDrawSubchannelOffset(7) | 0x40000;
   dmaPusherPutAddress[13] = NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[14] = dDrawSubchannelOffset(7) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[15] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress += 16;

#else   /* MULTI_MON */

   while (freeCount < 30)
       NvGetDmaBufferFreeCount(npDev, freeCount, 30, dmaPusherPutAddress);
   freeCount -= 30;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[3] = (unsigned long)npDmaDmaToMemNotifier;
   dmaPusherPutAddress[4] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[5] = (sizeof(NvNotification) - 1);

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(0) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[9] = (unsigned long)npDmaDmaToMemNotifier;
   dmaPusherPutAddress[10] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[11] = ((2 * sizeof(NvNotification)) - 1);

   dmaPusherPutAddress[12] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[13] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[14] = dDrawSubchannelOffset(0) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[15] =
       ((unsigned long)npDmaDmaToMemNotifier + (sizeof(NvNotification) << 1));
   dmaPusherPutAddress[16] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[17] = (sizeof(NvNotification) - 1);

   dmaPusherPutAddress[18] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[19] = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[20] = dDrawSubchannelOffset(0) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[21] =
       ((unsigned long)npDmaDmaToMemNotifier + (sizeof(NvNotification) << 2));
   dmaPusherPutAddress[22] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[23] = (sizeof(NvNotification) - 1);

   dmaPusherPutAddress[24] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[25] = NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[26] = dDrawSubchannelOffset(0) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[27] =
       ((unsigned long)npDmaDmaToMemNotifier + (sizeof(NvNotification) << 2) +
       sizeof(NvNotification));
   dmaPusherPutAddress[28] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[29] = ((5 * sizeof(NvNotification)) - 1);

   dmaPusherPutAddress += 30;

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[3] = (unsigned long)npDmaDmaToMemNotifier;
   dmaPusherPutAddress[4] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[5] = (NV_DD_COMMON_DMA_BUFFER_SIZE - 1);

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(0) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[9] = (unsigned long)npDmaDmaToMemNotifier;
   dmaPusherPutAddress[10] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[11] = (NV_DD_COMMON_DMA_BUFFER_SIZE - 1);

   dmaPusherPutAddress += 12;

   while (freeCount < 4)
       NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
   freeCount -= 4;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[1] = NV_VIDEO_SINK;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_VIDEO_SINK;

   dmaPusherPutAddress += 4;

   if (pxlDepth == 8) {

       while (freeCount < 9)
           NvGetDmaBufferFreeCount(npDev, freeCount, 9, dmaPusherPutAddress);
       freeCount -= 9;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
       dmaPusherPutAddress[1] = NV_VIDEO_COLORMAP;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) + NVFFF_CREATE_OFFSET | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_SHARED_VIDEO_COLORMAP;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(3) | 0x40000;
       dmaPusherPutAddress[5] = NV_DD_SHARED_VIDEO_COLORMAP;
       dmaPusherPutAddress[6] = dDrawSubchannelOffset(3) + NVFF9_SET_VIDEO_OUTPUT | 0x80000;
       dmaPusherPutAddress[7] = NV_DD_P_V_SHARED_VIDEO_COLORMAP;
       dmaPusherPutAddress[8] = NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;

       dmaPusherPutAddress += 9;
   }

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress[4] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[5] = NV_CONTEXT_DMA_FROM_MEMORY;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(1) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;

   dmaPusherPutAddress[8] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[9] = NV_CONTEXT_DMA_FROM_MEMORY;
   dmaPusherPutAddress[10] = dDrawSubchannelOffset(2) + NVFFF_CREATE_OFFSET | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

   dmaPusherPutAddress += 12;


   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(3) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(3) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[3] = (unsigned long)pDriverData->BaseAddress;
   dmaPusherPutAddress[4] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[5] =
       (unsigned long)(pDriverData->VideoHeapEnd - pDriverData->BaseAddress);

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(4) | 0x40000;
   dmaPusherPutAddress[7] = NV_ERROR_NOTIFIER;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(4) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[9] = (unsigned long)pDriverData->NvDmaBufferNotifierFlat;
   dmaPusherPutAddress[10] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[11] =
       (NV_DD_COMMON_DMA_BUFFER_SIZE - (sizeof(NvNotification) << 2) -
       (sizeof(NvNotification) << 1) - 1);

   dmaPusherPutAddress += 12;

   while (freeCount < 4)
       NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
   freeCount -= 4;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(5) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_VIDEO_SINK;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(5) + NVFFA_SET_VIDEO_INPUT | 0x40000;

   if (pxlDepth == 8)
       dmaPusherPutAddress[3] = NV_DD_P_V_SHARED_VIDEO_COLORMAP;
   else
       dmaPusherPutAddress[3] = NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;

   dmaPusherPutAddress += 4;

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(6) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(6) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[3] = (unsigned long)pDriverData->BaseAddress;
   dmaPusherPutAddress[4] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[5] =
       (unsigned long)(pDriverData->VideoHeapEnd - pDriverData->BaseAddress);

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(7) | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(7) + NVFFD_SET_DMA_SPECIFIER | 0xC0000;
   dmaPusherPutAddress[9] = (unsigned long)pDriverData->BaseAddress;
   dmaPusherPutAddress[10] = (unsigned long)pDriverData->flatSelector;
   dmaPusherPutAddress[11] =
       (unsigned long)pDriverData->VideoHeapEnd - pDriverData->BaseAddress;

   dmaPusherPutAddress += 12;

   while (freeCount < 20)
       NvGetDmaBufferFreeCount(npDev, freeCount, 20, dmaPusherPutAddress);
   freeCount -= 20;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) + NVFFD_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress[4] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(1) + NVFFD_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress[8] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[9] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[10] = dDrawSubchannelOffset(2) + NVFFD_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress[12] = dDrawSubchannelOffset(3) | 0x40000;
   dmaPusherPutAddress[13] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[14] = dDrawSubchannelOffset(3) + NVFFD_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[15] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress[16] = dDrawSubchannelOffset(4) | 0x40000;
   dmaPusherPutAddress[17] = NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[18] = dDrawSubchannelOffset(4) + NVFFD_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[19] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   dmaPusherPutAddress += 20;

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(4) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(4) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[3] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[4] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[5] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(5) | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(5) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[9] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[10] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[11] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress += 12;

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(6) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(6) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[3] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[4] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[5] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(7) | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(7) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[9] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[10] = NV_DD_DMA_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[11] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress += 12;

#endif  /* MULTI_MON */

   while (freeCount < 24)
       NvGetDmaBufferFreeCount(npDev, freeCount, 24, dmaPusherPutAddress);
   freeCount -= 24;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[3] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[5] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(1) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[9] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[10] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[11] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress[12] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[13] = NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[14] = dDrawSubchannelOffset(2) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[15] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[16] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[17] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress[18] = dDrawSubchannelOffset(3) | 0x40000;
   dmaPusherPutAddress[19] = NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
   dmaPusherPutAddress[20] = dDrawSubchannelOffset(3) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[21] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[22] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[23] = NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY;

   dmaPusherPutAddress += 24;

   while (freeCount < 19)
       NvGetDmaBufferFreeCount(npDev, freeCount, 19, dmaPusherPutAddress);
   freeCount -= 19;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(7) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_PRIMARY_VIDEO_FROM_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(7) +
       NVFF8_SET_VIDEO_OUTPUT | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(7) +
       NVFF8_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[5] = NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[6] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[7] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(7) +
       NVFF8_IMAGE_SCAN_OFFSET | 0x280000;
   dmaPusherPutAddress[9] = 0;
   dmaPusherPutAddress[10] = pDriverData->HALInfo.vmiData.lDisplayPitch;
   dmaPusherPutAddress[11] =
       asmMergeCoords(pDriverData->bi.biWidth, pDriverData->bi.biHeight);

   if (pxlDepth == 8)
       dmaPusherPutAddress[12] = NV_VFM_FORMAT_COLOR_LE_Y8_P4;
   else if (pxlDepth == 16)
       dmaPusherPutAddress[12] = NV_VFM_FORMAT_COLOR_LE_R5G6B5_P2;
   else
       dmaPusherPutAddress[12] = NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;

   dmaPusherPutAddress[13] = 0;

   dmaPusherPutAddress[14] = 0;
   dmaPusherPutAddress[15] = pDriverData->HALInfo.vmiData.lDisplayPitch;
   dmaPusherPutAddress[16] =
       asmMergeCoords(pDriverData->bi.biWidth, pDriverData->bi.biHeight);

   if (pxlDepth == 8)
       dmaPusherPutAddress[17] = NV_VFM_FORMAT_COLOR_LE_Y8_P4;
   else if (pxlDepth == 16)
       dmaPusherPutAddress[17] = NV_VFM_FORMAT_COLOR_LE_R5G6B5_P2;
   else
       dmaPusherPutAddress[17] = NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;

   dmaPusherPutAddress[18] = 0;

   dmaPusherPutAddress += 19;

   /*
    * Connect buffer patch objects
    */


   while (freeCount < 11)
       NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
   freeCount -= 11;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_SURFACES_2D;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
       NV042_SET_COLOR_FORMAT | 0x40000;

   if (pxlDepth == 8)
       dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_Y8;
   else if (pxlDepth == 16)
       dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_R5G6B5;
   else
       dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

   dmaPusherPutAddress[4] = dDrawSubchannelOffset(2) +
       NV042_SET_CONTEXT_DMA_IMAGE_SOURCE | 0x80000;
   dmaPusherPutAddress[5] = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
   dmaPusherPutAddress[6] = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;
   dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
       NV042_SET_PITCH | 0xC0000;
   dmaPusherPutAddress[8] =
       (pDriverData->HALInfo.vmiData.lDisplayPitch << 16) |
        pDriverData->HALInfo.vmiData.lDisplayPitch;
   dmaPusherPutAddress[9] = 0;
   dmaPusherPutAddress[10] = 0;

   dmaPusherPutAddress += 11;

   while (freeCount < 11)
       NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
   freeCount -= 11;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_IMAGE_BLIT;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
       NV05F_SET_CONTEXT_COLOR_KEY | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
//       NV_DD_IMAGE_BLACK_RECTANGLE;  // messes up D3D surface clip
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(0) +
       NV05F_SET_CONTEXT_PATTERN | 0x80000;
   dmaPusherPutAddress[5] = NV_DD_CONTEXT_PATTERN;
   dmaPusherPutAddress[6] = NV_DD_CONTEXT_ROP;
   dmaPusherPutAddress[7] = dDrawSubchannelOffset(0) +
       NV05F_SET_OPERATION | 0x40000;
   dmaPusherPutAddress[8] = NV05F_SET_OPERATION_ROP_AND;
   dmaPusherPutAddress[9] = dDrawSubchannelOffset(0) +
       NV05F_SET_CONTEXT_SURFACES | 0x40000;
   dmaPusherPutAddress[10] = NV_DD_SURFACES_2D;

   dmaPusherPutAddress += 11;

   while (freeCount < 6)
       NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
   freeCount -= 6;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_SRCCOPY_IMAGE_BLIT;
//   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
//       NV05F_SET_CONTEXT_COLOR_KEY | 0x40000;   // destination alpha not supported if context color key set
//   dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
//       NV_DD_IMAGE_BLACK_RECTANGLE;  // messes up D3D surface clip
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
       NV05F_SET_OPERATION | 0x40000;
   dmaPusherPutAddress[3] = NV05F_SET_OPERATION_SRCCOPY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(0) +
       NV05F_SET_CONTEXT_SURFACES | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_SURFACES_2D;

   dmaPusherPutAddress += 6;

   while (freeCount < 11)
       NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
   freeCount -= 11;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_STRETCHED_UV_IMAGE_FROM_CPU;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) +
       NV076_SET_CONTEXT_PATTERN | 0x80000;
   dmaPusherPutAddress[3] = NV_DD_CONTEXT_PATTERN;
   dmaPusherPutAddress[4] = NV_DD_CONTEXT_ROP;
   dmaPusherPutAddress[5] = dDrawSubchannelOffset(1) +
       NV076_SET_OPERATION | 0x40000;
   dmaPusherPutAddress[6] = NV076_SET_OPERATION_ROP_AND;
   dmaPusherPutAddress[7] = dDrawSubchannelOffset(1) +
       NV076_SET_CONTEXT_SURFACE | 0x40000;
   dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
   dmaPusherPutAddress[9] = dDrawSubchannelOffset(1) +
       NV076_SET_COLOR_FORMAT | 0x40000;
   /* A8Y8 color format assumed by hardware when destination surface in 8bpp mode */
   /* Must be set to a legal value but hardware ignores it otherwise */
   dmaPusherPutAddress[10] = NV076_SET_COLOR_FORMAT_LE_X8R8G8B8;

   dmaPusherPutAddress += 11;

   if (pxlDepth == 8) {

       while (freeCount < 13)
           NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
       freeCount -= 13;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_STRETCHED_IMAGE_FROM_CPU;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
           NV076_SET_CONTEXT_COLOR_KEY | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(0) +
           NV076_SET_CONTEXT_PATTERN | 0x80000;
       dmaPusherPutAddress[5] = NV_DD_CONTEXT_PATTERN;
       dmaPusherPutAddress[6] = NV_DD_CONTEXT_ROP;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(0) +
           NV076_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[8] = NV076_SET_OPERATION_ROP_AND;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(0) +
           NV076_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[10] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[11] = dDrawSubchannelOffset(0) +
           NV076_SET_COLOR_FORMAT | 0x40000;
       /* A8Y8 color format assumed by hardware when destination surface in 8bpp mode */
       /* Must be set to a legal value but hardware ignores it otherwise */
       dmaPusherPutAddress[12] = NV076_SET_COLOR_FORMAT_LE_X8R8G8B8;

       dmaPusherPutAddress += 13;

   } else {

       while (freeCount < 13)
           NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
       freeCount -= 13;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_STRETCHED_IMAGE_FROM_CPU;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
           NV076_SET_CONTEXT_COLOR_KEY | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(0) +
           NV076_SET_CONTEXT_PATTERN | 0x80000;
       dmaPusherPutAddress[5] = NV_DD_CONTEXT_PATTERN;
       dmaPusherPutAddress[6] = NV_DD_CONTEXT_ROP;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(0) +
           NV076_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[8] = NV076_SET_OPERATION_ROP_AND;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(0) +
           NV076_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[10] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[11] = dDrawSubchannelOffset(0) +
           NV076_SET_COLOR_FORMAT | 0x40000;
       if (pxlDepth == 16)
           dmaPusherPutAddress[12] = NV076_SET_COLOR_FORMAT_LE_R5G6B5;
       else
           dmaPusherPutAddress[12] = NV076_SET_COLOR_FORMAT_LE_X8R8G8B8;

       dmaPusherPutAddress += 13;


       if (pxlDepth > 16) {

           while (freeCount < 10)
               NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
           freeCount -= 10;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
           dmaPusherPutAddress[1] = NV_DD_SRCCOPY_STRETCHED_IMAGE_FROM_CPU;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) +
               NV076_SET_CONTEXT_COLOR_KEY | 0x40000;
           dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
           dmaPusherPutAddress[4] = dDrawSubchannelOffset(1) +
               NV076_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[5] = NV076_SET_OPERATION_SRCCOPY;
           dmaPusherPutAddress[6] = dDrawSubchannelOffset(1) +
               NV076_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[7] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[8] = dDrawSubchannelOffset(1) +
               NV076_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[9] = NV076_SET_COLOR_FORMAT_LE_A8R8G8B8;

           dmaPusherPutAddress += 10;

           while (freeCount < 12)
               NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
           freeCount -= 12;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
           dmaPusherPutAddress[1] = NV_DD_ABLEND_NOSRCALPHA_STRETCHED_IMAGE_FROM_CPU;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
               NV076_SET_CONTEXT_COLOR_KEY | 0x40000;
           dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
           dmaPusherPutAddress[4] = dDrawSubchannelOffset(2) +
               NV076_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[5] = NV076_SET_OPERATION_BLEND_PREMULT;
           dmaPusherPutAddress[6] = dDrawSubchannelOffset(2) +
               NV076_SET_CONTEXT_BETA4 | 0x40000;
           dmaPusherPutAddress[7] = NV_DD_CONTEXT_BETA4;
           dmaPusherPutAddress[8] = dDrawSubchannelOffset(2) +
               NV076_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[9] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[10] = dDrawSubchannelOffset(2) +
               NV076_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[11] = NV076_SET_COLOR_FORMAT_LE_X8R8G8B8;

           dmaPusherPutAddress += 12;

           while (freeCount < 12)
               NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
           freeCount -= 12;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(3) | 0x40000;
           dmaPusherPutAddress[1] = NV_DD_ABLEND_STRETCHED_IMAGE_FROM_CPU;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(3) +
               NV076_SET_CONTEXT_COLOR_KEY | 0x40000;
           dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
           dmaPusherPutAddress[4] = dDrawSubchannelOffset(3) +
               NV076_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[5] = NV076_SET_OPERATION_BLEND_PREMULT;
           dmaPusherPutAddress[6] = dDrawSubchannelOffset(3) +
               NV076_SET_CONTEXT_BETA4 | 0x40000;
           dmaPusherPutAddress[7] = NV_DD_CONTEXT_BETA4;
           dmaPusherPutAddress[8] = dDrawSubchannelOffset(3) +
               NV076_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[9] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[10] = dDrawSubchannelOffset(3) +
               NV076_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[11] = NV076_SET_COLOR_FORMAT_LE_A8R8G8B8;

           dmaPusherPutAddress += 12;
       }


       while (freeCount < 14)
           NvGetDmaBufferFreeCount(npDev, freeCount, 14, dmaPusherPutAddress);
       freeCount -= 14;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(0) +
           NV077_SET_CONTEXT_PATTERN | 0x80000;
       dmaPusherPutAddress[6] = NV_DD_CONTEXT_PATTERN;
       dmaPusherPutAddress[7] = NV_DD_CONTEXT_ROP;
       dmaPusherPutAddress[8] = dDrawSubchannelOffset(0) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[9] = NV077_SET_OPERATION_ROP_AND;
       dmaPusherPutAddress[10] = dDrawSubchannelOffset(0) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[11] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[12] = dDrawSubchannelOffset(0) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       if (pxlDepth == 16)
           dmaPusherPutAddress[13] = NV077_SET_COLOR_FORMAT_LE_R5G6B5;
       else
           dmaPusherPutAddress[13] = NV077_SET_COLOR_FORMAT_LE_X8R8G8B8;

       dmaPusherPutAddress += 14;


       if (pxlDepth > 16) {

           while (freeCount < 11)
               NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
           freeCount -= 11;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
           dmaPusherPutAddress[1] =
               NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) +
               NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
           dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
           dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(1) +
               NV077_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(1) +
               NV077_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[9] = dDrawSubchannelOffset(1) +
               NV077_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_A8R8G8B8;

           dmaPusherPutAddress += 11;

           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
           dmaPusherPutAddress[1] =
               NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
           dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
           dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(2) +
               NV077_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[6] = NV077_SET_OPERATION_BLEND_PREMULT;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_BETA4 | 0x40000;
           dmaPusherPutAddress[8] = NV_DD_CONTEXT_BETA4;
           dmaPusherPutAddress[9] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(2) +
               NV077_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[12] = NV077_SET_COLOR_FORMAT_LE_X8R8G8B8;

           dmaPusherPutAddress += 13;

           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
           dmaPusherPutAddress[1] =
               NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
           dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
           dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(2) +
               NV077_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[6] = NV077_SET_OPERATION_BLEND_PREMULT;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_BETA4 | 0x40000;
           dmaPusherPutAddress[8] = NV_DD_CONTEXT_BETA4;
           dmaPusherPutAddress[9] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(2) +
               NV077_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[12] = NV077_SET_COLOR_FORMAT_LE_A8R8G8B8;

           dmaPusherPutAddress += 13;
       }


       while (freeCount < 14)
           NvGetDmaBufferFreeCount(npDev, freeCount, 14, dmaPusherPutAddress);
       freeCount -= 14;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(0) +
           NV077_SET_CONTEXT_PATTERN | 0x80000;
       dmaPusherPutAddress[6] = NV_DD_CONTEXT_PATTERN;
       dmaPusherPutAddress[7] = NV_DD_CONTEXT_ROP;
       dmaPusherPutAddress[8] = dDrawSubchannelOffset(0) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[9] = NV077_SET_OPERATION_ROP_AND;
       dmaPusherPutAddress[10] = dDrawSubchannelOffset(0) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[11] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[12] = dDrawSubchannelOffset(0) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       if (pxlDepth == 16)
           dmaPusherPutAddress[13] = NV077_SET_COLOR_FORMAT_LE_R5G6B5;
       else
           dmaPusherPutAddress[13] = NV077_SET_COLOR_FORMAT_LE_X8R8G8B8;

       dmaPusherPutAddress += 14;


       if (pxlDepth > 16) {

           while (freeCount < 11)
               NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
           freeCount -= 11;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
           dmaPusherPutAddress[1] =
               NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) +
               NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
           dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
           dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(1) +
               NV077_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(1) +
               NV077_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[9] = dDrawSubchannelOffset(1) +
               NV077_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_A8R8G8B8;

           dmaPusherPutAddress += 11;

           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
           dmaPusherPutAddress[1] =
               NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
           dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
           dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(2) +
               NV077_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[6] = NV077_SET_OPERATION_BLEND_PREMULT;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_BETA4 | 0x40000;
           dmaPusherPutAddress[8] = NV_DD_CONTEXT_BETA4;
           dmaPusherPutAddress[9] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(2) +
               NV077_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[12] = NV077_SET_COLOR_FORMAT_LE_X8R8G8B8;

           dmaPusherPutAddress += 13;

           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
           dmaPusherPutAddress[1] =
               NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
           dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
           dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(2) +
               NV077_SET_OPERATION | 0x40000;
           dmaPusherPutAddress[6] = NV077_SET_OPERATION_BLEND_PREMULT;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_BETA4 | 0x40000;
           dmaPusherPutAddress[8] = NV_DD_CONTEXT_BETA4;
           dmaPusherPutAddress[9] = dDrawSubchannelOffset(2) +
               NV077_SET_CONTEXT_SURFACE | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_SURFACES_2D;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(2) +
               NV077_SET_COLOR_FORMAT | 0x40000;
           dmaPusherPutAddress[12] = NV077_SET_COLOR_FORMAT_LE_A8R8G8B8;

           dmaPusherPutAddress += 13;
       }


       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(4) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(4) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(4) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(4) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(4) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;

       dmaPusherPutAddress += 11;

       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(5) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(5) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(5) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(5) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(5) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;

       dmaPusherPutAddress += 11;

       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(6) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(6) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(6) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(6) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(6) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;

       dmaPusherPutAddress += 11;

       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(7) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(7) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(7) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(7) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(7) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;

       dmaPusherPutAddress += 11;

#ifdef  MULTI_MON

       while (freeCount < 10)
           NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
       freeCount -= 10;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(4) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(4) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(4) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[5] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[6] = dDrawSubchannelOffset(4) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[7] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[8] = dDrawSubchannelOffset(4) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[9] = NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;

       dmaPusherPutAddress += 10;

       while (freeCount < 10)
           NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
       freeCount -= 10;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(5) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(5) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(5) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[5] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[6] = dDrawSubchannelOffset(5) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[7] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[8] = dDrawSubchannelOffset(5) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[9] = NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;

       dmaPusherPutAddress += 10;

       while (freeCount < 10)
           NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
       freeCount -= 10;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(6) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(6) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(6) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[5] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[6] = dDrawSubchannelOffset(6) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[7] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[8] = dDrawSubchannelOffset(6) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[9] = NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;

       dmaPusherPutAddress += 10;

       while (freeCount < 10)
           NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
       freeCount -= 10;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(7) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(7) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(7) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[5] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[6] = dDrawSubchannelOffset(7) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[7] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[8] = dDrawSubchannelOffset(7) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[9] = NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;

       dmaPusherPutAddress += 10;

#else   /* MULTI_MON */

       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(4) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(4) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(4) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(4) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(4) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;

       dmaPusherPutAddress += 11;

       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(5) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(5) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(5) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(5) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(5) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_YB8V8YA8U8;

       dmaPusherPutAddress += 11;

       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(6) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(6) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(6) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(6) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(6) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;

       dmaPusherPutAddress += 11;

       while (freeCount < 11)
           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
       freeCount -= 11;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(7) | 0x40000;
       dmaPusherPutAddress[1] =
           NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(7) +
           NV077_SET_CONTEXT_DMA_NOTIFIES | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
       dmaPusherPutAddress[4] = NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(7) +
           NV077_SET_OPERATION | 0x40000;
       dmaPusherPutAddress[6] = NV077_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(7) +
           NV077_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[9] = dDrawSubchannelOffset(7) +
           NV077_SET_COLOR_FORMAT | 0x40000;
       dmaPusherPutAddress[10] = NV077_SET_COLOR_FORMAT_LE_V8YB8U8YA8;

       dmaPusherPutAddress += 11;

#endif  /* MULTI_MON */
   }


   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_ALT_IMAGE_FROM_CPU;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
       NV061_SET_CONTEXT_COLOR_KEY | 0x100000;
   dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
   dmaPusherPutAddress[4] = NV_DD_IMAGE_BLACK_RECTANGLE;
   dmaPusherPutAddress[5] = NV_DD_CONTEXT_PATTERN;
   dmaPusherPutAddress[6] = NV_DD_CONTEXT_ROP;
   dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
       NV061_SET_CONTEXT_SURFACE | 0x40000;
   dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
   dmaPusherPutAddress[9] = dDrawSubchannelOffset(2) +
       NV061_SET_OPERATION | 0x80000;
   dmaPusherPutAddress[10] = NV061_SET_OPERATION_ROP_AND;

   if (pxlDepth == 8)
       /* Y8 color format assumed by hardware when destination surface in 8bpp mode */
       /* Must be set to a legal value but hardware ignores it otherwise */
       dmaPusherPutAddress[11] = NV061_SET_COLOR_FORMAT_LE_X8R8G8B8;
   else if (pxlDepth == 16)
       dmaPusherPutAddress[11] = NV061_SET_COLOR_FORMAT_LE_X8R8G8B8;
   else if (pxlDepth == 32)
       dmaPusherPutAddress[11] = NV061_SET_COLOR_FORMAT_LE_R5G6B5;

   dmaPusherPutAddress += 12;

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_IMAGE_FROM_CPU;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
       NV061_SET_CONTEXT_COLOR_KEY | 0x100000;
   dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
   dmaPusherPutAddress[4] = NV_DD_IMAGE_BLACK_RECTANGLE;
   dmaPusherPutAddress[5] = NV_DD_CONTEXT_PATTERN;
   dmaPusherPutAddress[6] = NV_DD_CONTEXT_ROP;
   dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
       NV061_SET_CONTEXT_SURFACE | 0x40000;
   dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
   dmaPusherPutAddress[9] = dDrawSubchannelOffset(2) +
       NV061_SET_OPERATION | 0x80000;
   dmaPusherPutAddress[10] = NV061_SET_OPERATION_ROP_AND;

   if (pxlDepth == 8)
       /* Y8 color format assumed by hardware when destination surface in 8bpp mode */
       /* Must be set to a legal value but hardware ignores it otherwise */
       dmaPusherPutAddress[11] = NV061_SET_COLOR_FORMAT_LE_X8R8G8B8;
   else if (pxlDepth == 16)
       dmaPusherPutAddress[11] = NV061_SET_COLOR_FORMAT_LE_R5G6B5;
   else if (pxlDepth == 32)
       dmaPusherPutAddress[11] = NV061_SET_COLOR_FORMAT_LE_X8R8G8B8;

   dmaPusherPutAddress += 12;


   if (pxlDepth > 16) {

       while (freeCount < 9)
           NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
       freeCount -= 9;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(3) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_SRCCOPY_IMAGE_FROM_CPU;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(3) +
           NV061_SET_CONTEXT_COLOR_KEY | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(3) +
           NV061_SET_CONTEXT_SURFACE | 0x40000;
       dmaPusherPutAddress[5] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[6] = dDrawSubchannelOffset(3) +
           NV061_SET_OPERATION | 0x80000;
       dmaPusherPutAddress[7] = NV061_SET_OPERATION_SRCCOPY;
       dmaPusherPutAddress[8] = NV061_SET_COLOR_FORMAT_LE_A8R8G8B8;

       dmaPusherPutAddress += 9;

       while (freeCount < 10)
           NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
       freeCount -= 10;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(4) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_ABLEND_NOSRCALPHA_IMAGE_FROM_CPU;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(4) +
           NV061_SET_CONTEXT_COLOR_KEY | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(4) +
           NV061_SET_CONTEXT_BETA4 | 0x80000;
       dmaPusherPutAddress[5] = NV_DD_CONTEXT_BETA4;
       dmaPusherPutAddress[6] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(4) +
           NV061_SET_OPERATION | 0x80000;
       dmaPusherPutAddress[8] = NV061_SET_OPERATION_BLEND_PREMULT;
       dmaPusherPutAddress[9] = NV061_SET_COLOR_FORMAT_LE_X8R8G8B8;

       dmaPusherPutAddress += 10;

       while (freeCount < 10)
           NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
       freeCount -= 10;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(5) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_ABLEND_IMAGE_FROM_CPU;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(5) +
           NV061_SET_CONTEXT_COLOR_KEY | 0x40000;
       dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
       dmaPusherPutAddress[4] = dDrawSubchannelOffset(5) +
           NV061_SET_CONTEXT_BETA4 | 0x80000;
       dmaPusherPutAddress[5] = NV_DD_CONTEXT_BETA4;
       dmaPusherPutAddress[6] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[7] = dDrawSubchannelOffset(5) +
           NV061_SET_OPERATION | 0x80000;
       dmaPusherPutAddress[8] = NV061_SET_OPERATION_BLEND_PREMULT;
       dmaPusherPutAddress[9] = NV061_SET_COLOR_FORMAT_LE_A8R8G8B8;

       dmaPusherPutAddress += 10;
   }


   while (freeCount < 13)
       NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
   freeCount -= 13;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_ROP_GDI_RECT_AND_TEXT;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(2) +
       NV04A_SET_CONTEXT_DMA_NOTIFIES | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(2) +
       NV04A_SET_CONTEXT_PATTERN | 0x80000;
   dmaPusherPutAddress[5] = NV_DD_CONTEXT_PATTERN;
   dmaPusherPutAddress[6] = NV_DD_CONTEXT_ROP;
   dmaPusherPutAddress[7] = dDrawSubchannelOffset(2) +
       NV04A_SET_CONTEXT_SURFACE | 0x40000;
   dmaPusherPutAddress[8] = NV_DD_SURFACES_2D;
   dmaPusherPutAddress[9] = dDrawSubchannelOffset(2) +
       NV04A_SET_OPERATION | 0xC0000;
   dmaPusherPutAddress[10] = NV04A_SET_OPERATION_ROP_AND;

   if (pxlDepth == 8)
       /* Y8 color format assumed by hardware when destination surface in 8bpp mode */
       /* Must be set to a legal value but hardware ignores it otherwise */
       dmaPusherPutAddress[11] = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
   else if (pxlDepth == 16)
       dmaPusherPutAddress[11] = NV04A_SET_COLOR_FORMAT_LE_X16R5G6B5;
   else if (pxlDepth == 32)
       dmaPusherPutAddress[11] = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;

   dmaPusherPutAddress[12] = NV04A_SET_MONOCHROME_FORMAT_CGA6_M1;

   dmaPusherPutAddress += 13;


   if (pxlDepth > 16) {

       while (freeCount < 9)
           NvGetDmaBufferFreeCount(npDev, freeCount, 9, dmaPusherPutAddress);
       freeCount -= 9;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(3) | 0x40000;
       dmaPusherPutAddress[1] = NV_DD_ABLEND_RECT_AND_TEXT;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(3) +
           NV04A_SET_CONTEXT_BETA4 | 0x80000;
       dmaPusherPutAddress[3] = NV_DD_CONTEXT_BETA4;
       dmaPusherPutAddress[4] = NV_DD_SURFACES_2D;
       dmaPusherPutAddress[5] = dDrawSubchannelOffset(3) +
           NV04A_SET_OPERATION | 0xC0000;
       dmaPusherPutAddress[6] = NV04A_SET_OPERATION_BLEND_PREMULT;
       dmaPusherPutAddress[7] = NV04A_SET_COLOR_FORMAT_LE_X8R8G8B8;
       dmaPusherPutAddress[8] = NV04A_SET_MONOCHROME_FORMAT_CGA6_M1;

       dmaPusherPutAddress += 9;
   }


   while (freeCount < 6)
       NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
   freeCount -= 6;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(4) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(4) +
       NV039_SET_CONTEXT_DMA_NOTIFIES | 0xC0000;
   dmaPusherPutAddress[3] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[4] = NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   dmaPusherPutAddress[5] = NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY;

   dmaPusherPutAddress += 6;

   while (freeCount < 5)
       NvGetDmaBufferFreeCount(npDev, freeCount, 5, dmaPusherPutAddress);
   freeCount -= 5;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(5) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_CONTEXT_COLOR_KEY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(5) +
       NV057_SET_COLOR_FORMAT | 0x80000;

   if (pxlDepth == 8)
       /* A8Y8 color format assumed by hardware when destination surface in 8bpp mode */
       /* Must be set to a legal value but hardware ignores it otherwise */
       dmaPusherPutAddress[3] = NV057_SET_COLOR_FORMAT_LE_A8R8G8B8;
   else if (pxlDepth == 16)
       dmaPusherPutAddress[3] = NV057_SET_COLOR_FORMAT_LE_A16R5G6B5;
   else if (pxlDepth == 32)
       dmaPusherPutAddress[3] = NV057_SET_COLOR_FORMAT_LE_A8R8G8B8;

   dmaPusherPutAddress[4] = 0;

   dmaPusherPutAddress += 5;

   /*
    * Pattern is currently only used for overlay video UV masking so
    * always set it's color format to 8 bit indexed.
    */

   while (freeCount < 9)
       NvGetDmaBufferFreeCount(npDev, freeCount, 9, dmaPusherPutAddress);
   freeCount -= 9;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(6) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_CONTEXT_PATTERN;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(6) +
       NV044_SET_COLOR_FORMAT | 0x80000;
   /* A8Y8 color format assumed by hardware when destination surface in 8bpp mode */
   /* Must be set to a legal value but hardware ignores it otherwise */
   dmaPusherPutAddress[3] = NV044_SET_COLOR_FORMAT_LE_A8R8G8B8;
   dmaPusherPutAddress[4] = NV044_SET_MONOCHROME_FORMAT_CGA6_M1;
   dmaPusherPutAddress[5] = dDrawSubchannelOffset(6) +
       NV044_SET_PATTERN_SELECT | 0xC0000;
   dmaPusherPutAddress[6] = NV044_SET_PATTERN_SELECT_MONOCHROME;

   if (pxlDepth == 8) {
       dmaPusherPutAddress[7] = NV_ALPHA_1_008;
       dmaPusherPutAddress[8] = NV_ALPHA_1_008;
   } else if (pxlDepth == 16) {
       dmaPusherPutAddress[7] = NV_ALPHA_1_016;
       dmaPusherPutAddress[8] = NV_ALPHA_1_016;
   } else {
       dmaPusherPutAddress[7] = NV_ALPHA_1_032;
       dmaPusherPutAddress[8] = NV_ALPHA_1_032;
   }

   dmaPusherPutAddress += 9;

   /* Create the D3D objects */
   pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

   pDriverData->dwDmaPusherFreeCount = freeCount;

   nvD3DCreateObjects();

   dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;

#ifdef  CACHE_FREECOUNT
   freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
   freeCount = 0;
#endif  /* CACHE_FREECOUNT */

   /* Make final subchannel assignments */

   while (freeCount < 5)
       NvGetDmaBufferFreeCount(npDev, freeCount, 5, dmaPusherPutAddress);
   freeCount -= 5;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_IMAGE_BLACK_RECTANGLE;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) +
       NV019_SET_POINT | 0x80000;
   dmaPusherPutAddress[3] = 0;
   dmaPusherPutAddress[4] = asmMergeCoords(pDriverData->bi.biWidth,
                                           pDriverData->maxOffScreenY);

   dmaPusherPutAddress += 5;

   while (freeCount < 18)
       NvGetDmaBufferFreeCount(npDev, freeCount, 18, dmaPusherPutAddress);
   freeCount -= 18;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_DD_CONTEXT_ROP;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(6) | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_CONTEXT_COLOR_KEY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_SURFACES_2D;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(1) +
       NV042_SET_PITCH | 0xC0000;

   dmaPusherPutAddress[7] = (pDriverData->HALInfo.vmiData.lDisplayPitch << 16) |
    pDriverData->HALInfo.vmiData.lDisplayPitch;

   dmaPusherPutAddress[8] = 0;
   dmaPusherPutAddress[9] = 0;

   dmaPusherPutAddress[10] = dDrawSubchannelOffset(2) | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_ROP_GDI_RECT_AND_TEXT;
   dmaPusherPutAddress[12] = dDrawSubchannelOffset(3) | 0x40000;
   dmaPusherPutAddress[13] = NV_DD_IMAGE_BLIT;
   dmaPusherPutAddress[14] = dDrawSubchannelOffset(4) | 0x40000;
   dmaPusherPutAddress[15] = NV_DD_IMAGE_FROM_CPU;
   dmaPusherPutAddress[16] = dDrawSubchannelOffset(5) | 0x40000;

   if (pxlDepth == 8)
       dmaPusherPutAddress[17] = NV_DD_STRETCHED_IMAGE_FROM_CPU;
   else
       dmaPusherPutAddress[17] = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;

   dmaPusherPutAddress += 18;

   while (freeCount < 8)
       NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
   freeCount -= 8;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
       NV043_SET_ROP5 | 0x40000;
   dmaPusherPutAddress[1] = 0x00000000;

   /* Force rectangle portion of patch to be validated (assures no RM interrupts on Lock calls) */

   dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
       NV04A_COLOR1_A | 0xC0000;
   dmaPusherPutAddress[3] = 0;
   dmaPusherPutAddress[4] = 0;
   dmaPusherPutAddress[5] = 0;

   /* Force Blit to be instantiated */

   dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_BLIT) +
       NV05F_CONTROL_POINT_IN | 0x40000;
   dmaPusherPutAddress[7] = 0;

   dmaPusherPutAddress += 8;

   /* Force write combine buffer to flush */
   pDriverData->NvDmaPusherBufferEnd[0] = 0;
   /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
   _outp (0x3d0,0);

   npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

   /* Wait for initialization to complete */
   while (npDev->Get != (unsigned long)((dmaPusherPutAddress - dmaPusherBufferBase) << 2));

   pDriverData->dwDDDmaPusherChannelIndex = 1;
   pDriverData->dwDDDmaPusherChannelMask = 0;

   /* Make sure ALL NV DMA channels are idle */
   NV_DD_DMA_PUSHER_SYNC();

   tmpIndex = -1;

   while (tmpIndex != pDriverData->dwDDDmaPusherChannelIndex) {

       pDriverData->dwDDDmaPusherChannelIndex = tmpIndex;

       while (freeCount < 4)
           NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
       freeCount -= 4;

       npDmaSyncNotifier->status = NV_IN_PROGRESS;

       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
           RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
       dmaPusherPutAddress[1] = 0;
       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
           RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
       dmaPusherPutAddress[3] = 0;

       dmaPusherPutAddress += 4;

       /* Force write combine buffer to flush */
       pDriverData->NvDmaPusherBufferEnd[0] = 0;
       /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
       _outp (0x3d0,0);

       npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

       while ((volatile)npDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

       NV_DD_GET_DMA_CHANNEL_INDEX(tmpIndex);
   }

   pDriverData->dwDDDmaPusherChannelMask = 1 << pDriverData->dwDDDmaPusherChannelIndex;

   pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

   pDriverData->dwDmaPusherFreeCount = freeCount;

   /*
    * Perform any re-enabling of D3D.
    * After a mode switch, it is necessary to make sure that the proper
    * setup is done on the push buffer and texture contexts.
    */
   nvD3DReenable();

   pDriverData->dDrawSpareSubchannel6Object = 0;
   pDriverData->dDrawSpareSubchannelObject = 0; // subchannel 7


   /*
    * Now initialize video channel
    *
    * NOTE:
    *
    *  Video channel should contain NO graphics rendering objects.  This should
    *  prevent frequent context switches when switching between graphics and video channels.
    *
    */

   npDevVideo = (Nv3ChannelPio *)pDriverData->NvDevVideoFlatPio;

   videoFreeCount = 0;

   /* Initialize for PIO notifiers */

   pDriverData->NvPioDmaToMemNotifierFlat = pDriverData->NvCommonDmaBufferFlat;

   pDriverData->NvPioBufferNotifierFlat = pDriverData->NvCommonDmaBufferFlat;

//   pDriverData->NvPioDmaFromMemNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
//       (sizeof(NvNotification) << 1);

//   pDriverData->NvDmaPusherSyncNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
//       (sizeof(NvNotification) << 1) + sizeof(NvNotification);

//   pDriverData->NvPioSyncNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
//       (sizeof(NvNotification) << 2);

//   /* Notifier array offsets 5, 6, 7, 8, and 9 */
//   pDriverData->NvPioFlipPrimaryNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
//       (sizeof(NvNotification) << 2) + sizeof(NvNotification);

   /* Notifier array offsets A, B, C, D, and E */
   pDriverData->NvPioFlipOverlayNotifierFlat = pDriverData->NvCommonDmaBufferFlat +
       (sizeof(NvNotification) << 3) + (sizeof(NvNotification) << 1);

   /* Notifier array offset (sizeof(NvNotification) << 3) + (sizeof(NvNotification) << 2) + (sizeof(NvNotification) << 1) + sizeof(NvNotification) currently unused */

   npPioDmaToMemNotifier = (NvNotification *)pDriverData->NvPioDmaToMemNotifierFlat;
   npPioFlipOverlayNotifier = (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;

   /* Deal with possible mode change during pending flip */
   if (npDmaPusherSyncNotifier->status == NV_IN_PROGRESS) {
       long countDown = 0x200000;
       while ((npDmaPusherSyncNotifier->status == NV_IN_PROGRESS) && (--countDown > 0));
   }
   /* Deal with possible mode change during pending flip */
   if (npPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) {
       long countDown = 0x200000;
       while ((npPioFlipOverlayNotifier[1].status == NV_IN_PROGRESS) && (--countDown > 0));
   }
   /* Deal with possible mode change during pending flip */
   if (npPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS) {
       long countDown = 0x200000;
       while ((npPioFlipOverlayNotifier[2].status == NV_IN_PROGRESS) && (--countDown > 0));
   }

   /* MUST MAKE SURE STATUS FLAG IS CLEAR BEFORE INITIAL USE */

   npDmaPusherSyncNotifier->status = 0;

   npPioDmaToMemNotifier->status = 0;

   npPioFlipOverlayNotifier[1].status = 0;
   npPioFlipOverlayNotifier[2].status = 0;


#ifndef MULTI_MON
   while (videoFreeCount < 24)
       videoFreeCount = NvGetFreeCount(npDevVideo, 0);
   videoFreeCount -= 24;

   npDevVideo->subchannel[0].SetObject = NV_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[0].nvClass.Create = NV_DD_PIO_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[0].nvClass.Create = NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   npDevVideo->subchannel[1].SetObject = NV_CONTEXT_DMA_FROM_MEMORY;
   npDevVideo->subchannel[1].nvClass.Create = NV_DD_PIO_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   npDevVideo->subchannel[1].nvClass.Create = NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
#endif  /* MULTI_MON */

   if (pxlDepth == 8) {

       while (videoFreeCount < 12)
           videoFreeCount = NvGetFreeCount(npDevVideo, 2);
       videoFreeCount -= 12;

       npDevVideo->subchannel[3].SetObject = NV_DD_SHARED_VIDEO_COLORMAP;
       npDevVideo->subchannel[3].videoColormap.SetVideoOutput =
           NV_DD_P_V_SHARED_VIDEO_COLORMAP;
       npDevVideo->subchannel[3].videoColormap.SetVideoInput =
           NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;
   }

   while (videoFreeCount < 8)
       videoFreeCount = NvGetFreeCount(npDevVideo, 5);
   videoFreeCount -= 8;

   npDevVideo->subchannel[5].SetObject = NV_DD_VIDEO_SINK;
   npDevVideo->subchannel[5].videoSink.SetVideoInput[0] =
       NV_DD_P_V_VIDEO_COLOR_KEY;

#else   /* MULTI_MON */

   while (videoFreeCount < 40)
       videoFreeCount = NvGetFreeCount(npDevVideo, 0);
   videoFreeCount -= 40;

   npDevVideo->subchannel[0].SetObject =
       NV_DD_PIO_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[0].contextDmaToMemory.SetDmaSpecifier.address[0] =
       (unsigned long)npPioDmaToMemNotifier;
   npDevVideo->subchannel[0].contextDmaToMemory.SetDmaSpecifier.address[1] =
       (unsigned long)pDriverData->flatSelector;
   npDevVideo->subchannel[0].contextDmaToMemory.SetDmaSpecifier.limit =
       (sizeof(NvNotification) - 1);

   npDevVideo->subchannel[0].SetObject =
       NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[0].contextDmaToMemory.SetDmaSpecifier.address[0] =
       ((unsigned long)npPioDmaToMemNotifier + (sizeof(NvNotification) << 3) +
        (sizeof(NvNotification) << 1));
   npDevVideo->subchannel[0].contextDmaToMemory.SetDmaSpecifier.address[1] =
       (unsigned long)pDriverData->flatSelector;
   npDevVideo->subchannel[0].contextDmaToMemory.SetDmaSpecifier.limit =
       ((5 * sizeof(NvNotification)) - 1);

   npDevVideo->subchannel[1].SetObject = NV_VIDEO_SINK;
   npDevVideo->subchannel[1].nvClass.Create = NV_DD_VIDEO_SINK;

   if (pxlDepth == 8) {

       while (videoFreeCount < 20)
           videoFreeCount = NvGetFreeCount(npDevVideo, 2);
       videoFreeCount -= 20;

       npDevVideo->subchannel[2].SetObject = NV_VIDEO_COLORMAP;
       npDevVideo->subchannel[2].nvClass.Create = NV_DD_SHARED_VIDEO_COLORMAP;

       npDevVideo->subchannel[3].SetObject = NV_DD_SHARED_VIDEO_COLORMAP;
       npDevVideo->subchannel[3].videoColormap.SetVideoOutput =
           NV_DD_P_V_SHARED_VIDEO_COLORMAP;
       npDevVideo->subchannel[3].videoColormap.SetVideoInput =
           NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;
   }

   while (videoFreeCount < 40)
       videoFreeCount = NvGetFreeCount(npDevVideo, 4);
   videoFreeCount -= 40;

   npDevVideo->subchannel[4].SetObject = NV_ERROR_NOTIFIER;
   npDevVideo->subchannel[4].contextDmaToMemory.SetDmaSpecifier.address[0] =
       (unsigned long)pDriverData->NvPioBufferNotifierFlat;
   npDevVideo->subchannel[4].contextDmaToMemory.SetDmaSpecifier.address[1] =
       (unsigned long)pDriverData->flatSelector;
   npDevVideo->subchannel[4].contextDmaToMemory.SetDmaSpecifier.limit =
       (NV_DD_COMMON_DMA_BUFFER_SIZE - (sizeof(NvNotification) << 2) -
       (sizeof(NvNotification) << 1) - 1);

   npDevVideo->subchannel[5].SetObject = NV_DD_VIDEO_SINK;
   npDevVideo->subchannel[5].videoSink.SetVideoInput[0] =
       NV_DD_P_V_VIDEO_COLOR_KEY;

   npDevVideo->subchannel[6].SetObject = NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   npDevVideo->subchannel[6].contextDmaFromMemory.SetDmaSpecifier.address[0] =
       (unsigned long)pDriverData->BaseAddress;
   npDevVideo->subchannel[6].contextDmaFromMemory.SetDmaSpecifier.address[1] =
       (unsigned long)pDriverData->flatSelector;
   npDevVideo->subchannel[6].contextDmaFromMemory.SetDmaSpecifier.limit =
       (unsigned long)pDriverData->VideoHeapEnd - pDriverData->BaseAddress;

#endif  /* MULTI_MON */

   while (videoFreeCount < 60)
       videoFreeCount = NvGetFreeCount(npDevVideo, 2);
   videoFreeCount -= 60;

   npDevVideo->subchannel[2].SetObject = NV_DD_VIDEO_COLOR_KEY;
   npDevVideo->subchannel[2].videoColorKey.SetVideoOutput =
       NV_DD_P_V_VIDEO_COLOR_KEY;
   npDevVideo->subchannel[2].videoColorKey.SetVideoInput[0] =
       NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;

   if (pxlDepth == 8)
       npDevVideo->subchannel[2].videoColorKey.SetVideoInput[1] =
           NV_DD_P_V_SHARED_VIDEO_COLORMAP;
   else
       npDevVideo->subchannel[2].videoColorKey.SetVideoInput[1] =
           NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;

   npDevVideo->subchannel[2].videoColorKey.SetVideoInput[2] =
       NV_DD_P_V_VIDEO_SCALER;

   if (pxlDepth == 8)
       npDevVideo->subchannel[2].videoColorKey.SetColorFormat =
           NVFF6_SET_COLOR_FORMAT_LE_X16A8Y8;
   else if (pxlDepth == 16)
       npDevVideo->subchannel[2].videoColorKey.SetColorFormat =
           NVFF6_SET_COLOR_FORMAT_LE_A16R5G6B5;
   else
       npDevVideo->subchannel[2].videoColorKey.SetColorFormat =
           NVFF6_SET_COLOR_FORMAT_LE_A8R8G8B8;

   npDevVideo->subchannel[2].videoColorKey.SetColorKey = 0;
   npDevVideo->subchannel[2].videoColorKey.SetPoint = 0;
   npDevVideo->subchannel[2].videoColorKey.SetSize = 0;

   npDevVideo->subchannel[3].SetObject = NV_DD_VIDEO_SCALER;
   npDevVideo->subchannel[3].videoScaler.SetVideoOutput =
       NV_DD_P_V_VIDEO_SCALER;
   npDevVideo->subchannel[3].videoScaler.SetVideoInput =
       NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY;
   npDevVideo->subchannel[3].videoScaler.SetDeltaDuDx = 0x100000;
   npDevVideo->subchannel[3].videoScaler.SetDeltaDvDy = 0x100000;
   npDevVideo->subchannel[3].videoScaler.SetPoint = 0;

   while (videoFreeCount < 60)
       videoFreeCount = NvGetFreeCount(npDevVideo, 3);
   videoFreeCount -= 60;

   npDevVideo->subchannel[3].SetObject = NV_DD_YUV422_VIDEO_FROM_MEMORY;
   npDevVideo->subchannel[3].videoFromMemory.SetVideoOutput =
       NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY;
   npDevVideo->subchannel[3].videoFromMemory.SetImageCtxDma[0] =
       NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   npDevVideo->subchannel[3].videoFromMemory.SetImageCtxDma[1] =
       NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   npDevVideo->subchannel[3].videoFromMemory.SetImageNotifyCtxDma =
       NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[0].offset = 0;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[0].pitch = 0;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[0].size = 0;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[0].format =
       NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[0].notify = 0;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[1].offset = 0;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[1].pitch = 0;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[1].size = 0;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[1].format =
       NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;
   npDevVideo->subchannel[3].videoFromMemory.ImageScan[1].notify = 0;


   while (videoFreeCount < 24)
       videoFreeCount = NvGetFreeCount(npDevVideo, 7);
   videoFreeCount -= 24;

   npDevVideo->subchannel[7].SetObject = NV_DD_PRIMARY_VIDEO_FROM_MEMORY;
   npDevVideo->subchannel[7].videoFromMemory.SetVideoOutput =
       NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY;
   npDevVideo->subchannel[7].videoFromMemory.SetImageCtxDma[0] =
       NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   npDevVideo->subchannel[7].videoFromMemory.SetImageCtxDma[1] =
       NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
//   npDevVideo->subchannel[7].videoFromMemory.SetImageNotifyCtxDma =
//       NV_DD_PIO_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[0].offset = 0;
//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[0].pitch =
//       pDriverData->HALInfo.vmiData.lDisplayPitch;
//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[0].size =
//       asmMergeCoords(pDriverData->bi.biWidth, pDriverData->bi.biHeight);
   if (pxlDepth == 8)
       npDevVideo->subchannel[7].videoFromMemory.ImageScan[0].format =
           NV_VFM_FORMAT_COLOR_LE_Y8_P4;
   else if (pxlDepth == 16)
       npDevVideo->subchannel[7].videoFromMemory.ImageScan[0].format =
           NV_VFM_FORMAT_COLOR_LE_R5G6B5_P2;
   else
       npDevVideo->subchannel[7].videoFromMemory.ImageScan[0].format =
           NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;
//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[0].notify = 0;

//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[1].offset = 0;
//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[1].pitch =
//       pDriverData->HALInfo.vmiData.lDisplayPitch;
//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[1].size =
//       asmMergeCoords(pDriverData->bi.biWidth, pDriverData->bi.biHeight);
   if (pxlDepth == 8)
       npDevVideo->subchannel[7].videoFromMemory.ImageScan[1].format =
           NV_VFM_FORMAT_COLOR_LE_Y8_P4;
   else if (pxlDepth == 16)
       npDevVideo->subchannel[7].videoFromMemory.ImageScan[1].format =
           NV_VFM_FORMAT_COLOR_LE_R5G6B5_P2;
   else
       npDevVideo->subchannel[7].videoFromMemory.ImageScan[1].format =
           NV_VFM_FORMAT_COLOR_LE_X8R8G8B8;
//   npDevVideo->subchannel[7].videoFromMemory.ImageScan[1].notify = 0;

   while (videoFreeCount < 20)
       videoFreeCount = NvGetFreeCount(npDevVideo, 0);
   videoFreeCount -= 20;

   npDevVideo->subchannel[0].SetObject = NV_DD_DVD_SUBPICTURE;
   npDevVideo->subchannel[0].nv4DvdSubpicture.SetContextDmaNotifies =
       NV_DD_PIO_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[0].nv4DvdSubpicture.SetContextDmaOverlay =
       NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   npDevVideo->subchannel[0].nv4DvdSubpicture.SetContextDmaImageIn =
       NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
   npDevVideo->subchannel[0].nv4DvdSubpicture.SetContextDmaImageOut =
       NV_DD_PIO_CONTEXT_DMA_TO_VIDEO_MEMORY;


   pDriverData->NvVideoFreeCount = (unsigned short)videoFreeCount;


    /*
     * Allocate psuedo-notifier memory
     *  we use <size> bytes starting at the highest 512 byte boundary inside of
     *  the 4k of persistent memory of the disp driver. This is so that we
     *  can set up a proper surface because of alignment restrictions.
     *  the caluclation is done in <addr>
     * Modify <size> only if you modify it in nv4ctxt.c also (I need some #defines)
     */
    {
        DWORD size =  16;
        DWORD addr = (pDriverData->VideoHeapEnd + 4096) & ~511;

#ifndef WINNT
        pDriverData->dwTMPseudoNotifierMem    = addr;
        pDriverData->dwTMPseudoNotifierOffset = pDriverData->dwTMPseudoNotifierMem - pDriverData->BaseAddress;
        memset ((void*)(pDriverData->dwTMPseudoNotifierOffset + pDriverData->BaseAddress),0,size);
#else
        pDriverData->dwTMPseudoNotifierOffset    = addr;
        pDriverData->dwTMPseudoNotifierMem = pDriverData->ppdev->pjScreen + pDriverData->dwTMPseudoNotifierOffset;
        memset ((void*)(pDriverData->dwTMPseudoNotifierMem),0,size);
#endif

        //DPF_LEVEL(NVDBG_LEVEL_INFO, "TM: Initial notifier = %08x",dwTMStageAddr);
        pDriverData->dwTMHead = pDriverData->dwTMStageAddr;
        pDriverData->dwTMTail = pDriverData->dwTMStageAddr;
        pDriverData->dwRenderedFrame = pDriverData->dwCurrentFrame = 0;
    }

   return(1);
}
