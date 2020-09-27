
/*
 ***************************************************************************
 *                                                                         *
 *  Copyright (C) 1995, 1998 NVidia Corporation.  All Rights Reserved.     *
 *                                                                         *
 ***************************************************************************
 */

/* NvDDDisable32() */

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
 * NvDDDisable32()
 *
 */

int
NvDDDisable32()
{
   FAST Nv4ControlDma *npDev = (Nv4ControlDma *)pDriverData->NvDevFlatDma;
   Nv3ChannelPio *npDevVideo;
   long videoFreeCount;
   unsigned long *dmaPusherPutAddress =
       (unsigned long *)pDriverData->NvDmaPusherPutAddress;
   unsigned long *dmaPusherBufferBase =
       (unsigned long *)pDriverData->NvDmaPusherBufferBase;
   NvNotification *npDmaSyncNotifier =
       (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;
#ifdef  CACHE_FREECOUNT
   long freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
   long freeCount = 0;
#endif  /* CACHE_FREECOUNT */

   /*
    * Ensure the push buffer has completed prior to deleting objects
    * which it might reference.
    */

   while ((volatile)npDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

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

#ifndef MULTI_MON
   while (freeCount < 14)
       NvGetDmaBufferFreeCount(npDev, freeCount, 14, dmaPusherPutAddress);
   freeCount -= 14;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(0) | 0x40000;
   dmaPusherPutAddress[1] = NV_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(0) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(0) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_DMA_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(0) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(0) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[9] = NV_DD_DMA_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[10] = dDrawSubchannelOffset(0) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_DMA_BUFFER_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   dmaPusherPutAddress[12] = dDrawSubchannelOffset(0) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[13] = NV_DD_DMA_CONTEXT_DMA_TO_SYSTEM_MEMORY;

   dmaPusherPutAddress += 14;

   while (freeCount < 12)
       NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
   freeCount -= 12;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[1] = NV_CONTEXT_DMA_FROM_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[4] = dDrawSubchannelOffset(1) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[5] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[6] = dDrawSubchannelOffset(1) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[7] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[8] = dDrawSubchannelOffset(1) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[9] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   dmaPusherPutAddress[10] = dDrawSubchannelOffset(1) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[11] = NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

   dmaPusherPutAddress += 12;

   while (freeCount < 4)
       NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
   freeCount -= 4;

   dmaPusherPutAddress[0] = dDrawSubchannelOffset(1) | 0x40000;
   dmaPusherPutAddress[1] = NV_CONTEXT_DMA_IN_MEMORY;
   dmaPusherPutAddress[2] = dDrawSubchannelOffset(1) + NVFFF_DESTROY_OFFSET | 0x40000;
   dmaPusherPutAddress[3] = NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY;

   dmaPusherPutAddress += 4;

#endif  /* MULTI_MON */

   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_CONTEXT_ROP) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_CONTEXT_PATTERN) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_IMAGE_BLACK_RECTANGLE) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_CONTEXT_COLOR_KEY) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_BETA_SOLID) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_CONTEXT_BETA4) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_ROP_GDI_RECT_AND_TEXT) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_ABLEND_RECT_AND_TEXT) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_ALT_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_SRCCOPY_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_ABLEND_NOSRCALPHA_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_ABLEND_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_STRETCHED_UV_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_STRETCHED_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_SRCCOPY_STRETCHED_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_ABLEND_NOSRCALPHA_STRETCHED_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_ABLEND_STRETCHED_IMAGE_FROM_CPU) != NVOS00_STATUS_SUCCESS)
            return(0);

   if (pDriverData->bi.biBitCount > 8) {
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
            NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                return(0);

       if (pDriverData->bi.biBitCount > 16) {

        if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(0);
        if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(0);
        if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(0);
        if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(0);
        if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_NOSRCALPHA_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(0);
        if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
                NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY) != NVOS00_STATUS_SUCCESS)
                    return(0);
       }
   }
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_IMAGE_BLIT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_SRCCOPY_IMAGE_BLIT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_SURFACES_2D) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_PRIMARY_VIDEO_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
               return(0);

   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
               return(0);

   if (pDriverData->bi.biBitCount == 8) {
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_P_V_SHARED_VIDEO_COLORMAP) != NVOS00_STATUS_SUCCESS)
            return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
        NV_DD_SHARED_VIDEO_COLORMAP) != NVOS00_STATUS_SUCCESS)
            return(0);
   }

   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);

   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
           NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT) != NVOS00_STATUS_SUCCESS)
               return(0);

   pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

   /* Force write combine buffer to flush */
   pDriverData->NvDmaPusherBufferEnd[0] = 0;
   /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
   _outp (0x3d0,0);

   pDriverData->dwDmaPusherFreeCount = freeCount;

   npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

   /* Wait for destruction to complete */
   while (npDev->Get != (unsigned long)((dmaPusherPutAddress - dmaPusherBufferBase) << 2));

   /* Destroy the D3D objects */
   nvD3DDestroyObjects();

   /*
    * Now destroy objects in video channel
    */

   npDevVideo = (Nv3ChannelPio *)pDriverData->NvDevVideoFlatPio;

   videoFreeCount = pDriverData->NvVideoFreeCount;

#ifndef MULTI_MON
   while (videoFreeCount < 24)
       videoFreeCount = NvGetFreeCount(npDevVideo, 0);
   videoFreeCount -= 24;

   npDevVideo->subchannel[0].SetObject = NV_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[0].nvClass.Destroy = NV_DD_PIO_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY;
   npDevVideo->subchannel[0].nvClass.Destroy = NV_DD_PIO_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY;

   npDevVideo->subchannel[1].SetObject = NV_CONTEXT_DMA_FROM_MEMORY;
   npDevVideo->subchannel[1].nvClass.Destroy = NV_DD_PIO_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
   npDevVideo->subchannel[1].nvClass.Destroy = NV_DD_PIO_CONTEXT_DMA_FROM_VIDEO_MEMORY;
#endif  /* MULTI_MON */

   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_PRIMARY_VIDEO_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_YUV422_VIDEO_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_VIDEO_SINK) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_VIDEO_COLOR_KEY) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_VIDEO_SCALER) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_P_V_VIDEO_SCALER) != NVOS00_STATUS_SUCCESS)
               return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
           NV_DD_P_V_VIDEO_COLOR_KEY) != NVOS00_STATUS_SUCCESS)
               return(0);

#ifdef WINNT
   /*
    * Destroy NV4 DVD subpicture object in both DMA Push and PIO
    * channels. Don't know why the destruction of these objects was
    * not in the common code.
    */
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_DMA,
    NV_DD_DVD_SUBPICTURE) != NVOS00_STATUS_SUCCESS)
        return(0);
   if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
    NV_DD_DVD_SUBPICTURE) != NVOS00_STATUS_SUCCESS)
        return(0);
#endif // WINNT

   if (pDriverData->bi.biBitCount == 8) {
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
            NV_DD_P_V_SHARED_VIDEO_COLORMAP) != NVOS00_STATUS_SUCCESS)
                    return(0);
    if (NvRmFree(pDriverData->dwRootHandle, NV_DD_DEV_VIDEO,
            NV_DD_SHARED_VIDEO_COLORMAP) != NVOS00_STATUS_SUCCESS)
                    return(0);
   }

   pDriverData->NvVideoFreeCount = (unsigned short)videoFreeCount;

   return(1);
}
