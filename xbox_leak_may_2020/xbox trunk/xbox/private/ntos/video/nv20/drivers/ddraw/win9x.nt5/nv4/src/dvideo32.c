/*==========================================================================;
 *
 *  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
 *  Portions Copyright (C) 1997, 1998 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       dvideo32.c
 *  Content:    NVidia Direct Video/Active Movie Direct Draw support
 *              32 bit DirectDraw part.
 *
 ***************************************************************************/
#include "windows.h"
#include <conio.h>
#include "nvd3ddrv.h"
#include "nvcm.h"
#include "nv32.h"
#include "nvddobj.h"
#include "wingdi.h"
#include "arb_fifo.h" /* video bandwidth related stuff */
#include "nvrmapi.h"
#include "nvrmarch.inc"

/*
 * Make fixups for NV3->NV4 style includes.
 */
#undef NvChannel
#define NvChannel Nv3ChannelPio
#define nvContextDmaToMemory contextDmaToMemory

#if 0
DWORD __stdcall HandleVideoPortOverlayUpdate(LPDDHAL_UPDATEOVERLAYDATA lpUOData);
#endif

#ifdef NVPE
///// H.AZAR: (03/01/1999): this function is defined in nvpkrnl.c (nvpe.lib)
extern DWORD NVPUpdateOverlay();
#endif

/*
 * ConvertOverlay32
 *
 */

DWORD __stdcall
ConvertOverlay32(LPDDRAWI_DDRAWSURFACE_LCL lpDDSurface, BOOL waitForCompletion )
{
    FAST Nv10ControlDma         *npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    DWORD                       caps;
    DWORD                       dwSrcYOffset;
    DWORD                       dwSrcVOffset;
    DWORD                       dwSrcUOffset;
    long                        dwSrcYPitch;
    long                        dwSrcUVPitch;
    long                        dwSrcUVHeight;
    DWORD                       dwDstYOffset;
    long                        dwDstYPitch;
    unsigned char               *fpSrcYMem;
    unsigned char               *fpSrcUMem;
    unsigned char               *fpSrcVMem;
    long                        dwSrcPitch;
    long                        dwSrcWidth;
    long                        dwSrcHeight;
    unsigned long               *dmaPusherPutAddress =
                                    (unsigned long *)pDriverData->NvDmaPusherPutAddress;
    unsigned long               *dmaPusherBufferBase =
                                    (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
    long                        freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
    long                        freeCount = 0;
#endif  /* CACHE_FREECOUNT */

    pSurf_gbl = lpDDSurface->lpGbl;

    caps = lpDDSurface->ddsCaps.dwCaps;

    NV_DD_DMA_PUSHER_SYNC();

    /* If surface is IF09 format then must reformat data before
       video scaler can be used to display it */
    if ((pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IF09) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YVU9) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IV32) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IV31)) {

        dwSrcYOffset = pSurf_gbl->fpVidMem - pDriverData->BaseAddress;
        dwSrcYPitch = (DWORD)pSurf_gbl->wWidth;
        dwSrcYPitch = (dwSrcYPitch + 3) & ~3L;
        dwDstYOffset = dwSrcYOffset + ((dwSrcYPitch * ((DWORD)pSurf_gbl->wHeight * 10L)) >> 3);
        dwDstYOffset = (dwDstYOffset + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        dwDstYPitch = ((pSurf_gbl->wWidth + 1) & 0xFFFE) << 1;
        dwDstYPitch = (dwDstYPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

        fpSrcYMem = (unsigned char *)pSurf_gbl->fpVidMem;
        dwSrcHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcHeight = (dwSrcHeight + 3) & ~3L;
        fpSrcVMem = fpSrcYMem + (dwSrcYPitch * dwSrcHeight);
        dwSrcHeight >>= 2;
        fpSrcUMem = fpSrcVMem + ((dwSrcYPitch >> 2) * dwSrcHeight);

        dwSrcUVHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcUVHeight = (dwSrcUVHeight + 3) & ~3L;
        dwSrcUVHeight >>= 2;

        dwSrcUVPitch = (DWORD)pSurf_gbl->wWidth;
        dwSrcUVPitch = (dwSrcUVPitch + 3) & ~3L;
        dwSrcUVPitch >>= 2;

        /* Let D3D code know that we have touched NV */
        pDriverData->TwoDRenderingOccurred = 1;

        while (freeCount < 25)
            NvGetDmaBufferFreeCount(npDev, freeCount, 25, dmaPusherPutAddress);
        freeCount -= 25;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
            SET_ROP_OFFSET | 0x40000;
        dmaPusherPutAddress[1] = 0x000000C0; /* MERGECOPY PSa */

        /* Temporarily change destination surface color format */
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x80000;
        dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_Y8;
        dmaPusherPutAddress[4] = (dwDstYPitch << 16) | dwDstYPitch;
        dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_DEST_OFFSET | 0x40000;
        dmaPusherPutAddress[6] = dwDstYOffset;

        dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        dmaPusherPutAddress[8] = NV_DD_CONTEXT_PATTERN;
        dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_SPARE) +
            NV044_SET_MONOCHROME_SHAPE | 0x40000;
        dmaPusherPutAddress[10] = NV044_SET_MONOCHROME_SHAPE_64X_1Y;
        dmaPusherPutAddress[11] = dDrawSubchannelOffset(NV_DD_SPARE) +
            NV044_SET_MONOCHORME_COLOR0 | 0x100000;
        dmaPusherPutAddress[12] = 0;
        dmaPusherPutAddress[13] = 0xFFFFFFFF;

        /* First enable only V values to be written */
        dmaPusherPutAddress[14] = 0x11111111;
        dmaPusherPutAddress[15] = 0x11111111;

        dmaPusherPutAddress[16] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;
        dmaPusherPutAddress[17] = NV_DD_STRETCHED_UV_IMAGE_FROM_CPU;
        dmaPusherPutAddress[18] = dDrawSubchannelOffset(NV_DD_STRETCH) +
            STRETCHED_IMAGE_SIZE_IN_OFFSET | 0x180000;

        dmaPusherPutAddress[19] = asmMergeCoords(dwSrcUVPitch, dwSrcUVHeight);
        dmaPusherPutAddress[20] = 8 << 20;
        dmaPusherPutAddress[21] = 4 << 20;
        dmaPusherPutAddress[22] = 0;
        dmaPusherPutAddress[23] =
            asmMergeCoords((dwSrcYPitch << 1), (DWORD)pSurf_gbl->wHeight);
        dmaPusherPutAddress[24] = 0;

        dmaPusherPutAddress += 25;

        dwSrcHeight = dwSrcUVHeight;

        while (--dwSrcHeight >= 0) {
            FAST DWORD vPxls0;
            FAST DWORD vPxls1;

            dwSrcPitch = dwSrcUVPitch;

            /* Source scanlines may not be exact multiples of 4 */
            dwSrcPitch = (dwSrcPitch + 3) & ~3;

            // Get enough room for this scanline
            while (freeCount < (1 + (dwSrcPitch >> 2)))
                NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (dwSrcPitch >> 2)), dmaPusherPutAddress);
            freeCount -= (1 + (dwSrcPitch >> 2));

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                STRETCHED_IMAGE_COLOR_OFFSET | (dwSrcPitch << 16);

            dmaPusherPutAddress += 1;

            while (dwSrcPitch >= 16) {
                dwSrcPitch -= 16;

                vPxls0 = *(unsigned long *)fpSrcVMem;
                vPxls1 = *(unsigned long *)&fpSrcVMem[4];

                dmaPusherPutAddress[0] = vPxls0;

                vPxls0 = *(unsigned long *)&fpSrcVMem[8];
                dmaPusherPutAddress[1] = vPxls1;
                vPxls1 = *(unsigned long *)&fpSrcVMem[12];
                dmaPusherPutAddress[2] = vPxls0;
                fpSrcVMem += 16;
                dmaPusherPutAddress[3] = vPxls1;

                dmaPusherPutAddress += 4;
            }

            while (dwSrcPitch >= 4) {
                dwSrcPitch -= 4;
                vPxls0 = *(unsigned long *)fpSrcVMem;
                fpSrcVMem += 4;
                dmaPusherPutAddress[0] = vPxls0;

                dmaPusherPutAddress += 1;
            }
        }

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        // Get NV started on reading the image data
        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

        while (freeCount < 14)
            NvGetDmaBufferFreeCount(npDev, freeCount, 14, dmaPusherPutAddress);
        freeCount -= 14;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) +
            NV044_SET_MONOCHROME_SHAPE | 0x40000;
        dmaPusherPutAddress[1] = NV044_SET_MONOCHROME_SHAPE_64X_1Y;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
            NV044_SET_MONOCHORME_COLOR0 | 0x100000;
        dmaPusherPutAddress[3] = 0;
        dmaPusherPutAddress[4] = 0xFFFFFFFF;

        /* Now enable only U values to be written */
        dmaPusherPutAddress[5] = 0x44444444;
        dmaPusherPutAddress[6] = 0x44444444;

        dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_STRETCH) +
            STRETCHED_IMAGE_SIZE_IN_OFFSET | 0x180000;

        dmaPusherPutAddress[8] = asmMergeCoords(dwSrcUVPitch, dwSrcUVHeight);
        dmaPusherPutAddress[9] = 8 << 20;
        dmaPusherPutAddress[10] = 4 << 20;
        dmaPusherPutAddress[11] = 0;
        dmaPusherPutAddress[12] =
            asmMergeCoords((dwSrcYPitch << 1), (DWORD)pSurf_gbl->wHeight);
        dmaPusherPutAddress[13] = 0;

        dmaPusherPutAddress += 14;

        dwSrcHeight = dwSrcUVHeight;

        while (--dwSrcHeight >= 0) {
            FAST DWORD uPxls0;
            FAST DWORD uPxls1;

            dwSrcPitch = dwSrcUVPitch;

            /* Source scanlines may not be exact multiples of 4 */
            dwSrcPitch = (dwSrcPitch + 3) & ~3;

            // Get enough room for this scanline
            while (freeCount < (1 + (dwSrcPitch >> 2)))
                NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (dwSrcPitch >> 2)), dmaPusherPutAddress);
            freeCount -= (1 + (dwSrcPitch >> 2));

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                STRETCHED_IMAGE_COLOR_OFFSET | (dwSrcPitch << 16);

            dmaPusherPutAddress += 1;

            while (dwSrcPitch >= 16) {
                dwSrcPitch -= 16;

                uPxls0 = *(unsigned long *)fpSrcUMem;
                uPxls1 = *(unsigned long *)&fpSrcUMem[4];

                dmaPusherPutAddress[0] = uPxls0;
                uPxls0 = *(unsigned long *)&fpSrcUMem[8];
                dmaPusherPutAddress[1] = uPxls1;
                uPxls1 = *(unsigned long *)&fpSrcUMem[12];
                dmaPusherPutAddress[2] = uPxls0;
                fpSrcUMem += 16;
                dmaPusherPutAddress[3] = uPxls1;

                dmaPusherPutAddress += 4;
            }

            while (dwSrcPitch >= 4) {
                dwSrcPitch -= 4;
                uPxls0 = *(unsigned long *)fpSrcUMem;
                fpSrcUMem += 4;
                dmaPusherPutAddress[0] = uPxls0;

                dmaPusherPutAddress += 1;
            }
        }

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        // Get NV started on reading the image data
        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

        /* restore default stretch subchannel object */
        while (freeCount < 6)
            NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
        freeCount -= 6;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;
        if (pDriverData->bi.biBitCount == 8)
            dmaPusherPutAddress[1] = NV_DD_STRETCHED_IMAGE_FROM_CPU;
        else
            dmaPusherPutAddress[1] = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;

        /* restore destination surface color format and reenable pattern object alpha */
        if (pDriverData->bi.biBitCount == 8) {

            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                NV042_SET_COLOR_FORMAT | 0x40000;
            dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_Y8;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SPARE) +
                NV044_SET_MONOCHORME_COLOR0 | 0x40000;
            dmaPusherPutAddress[5] = NV_ALPHA_1_008;

        } else if (pDriverData->bi.biBitCount == 16) {

            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                NV042_SET_COLOR_FORMAT | 0x40000;
            dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_R5G6B5;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SPARE) +
                NV044_SET_MONOCHORME_COLOR0 | 0x40000;
            dmaPusherPutAddress[5] = NV_ALPHA_1_016;

        } else {

            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                NV042_SET_COLOR_FORMAT | 0x40000;
            dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SPARE) +
                NV044_SET_MONOCHORME_COLOR0 | 0x40000;
            dmaPusherPutAddress[5] = NV_ALPHA_1_032;
        }

        dmaPusherPutAddress += 6;


        /* Update all Y values */
        while (freeCount < 11)
            NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
        freeCount -= 11;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
        dmaPusherPutAddress[3] = dwSrcYOffset;
        dmaPusherPutAddress[4] = dwDstYOffset;
        dmaPusherPutAddress[5] = dwSrcYPitch;
        dmaPusherPutAddress[6] = dwDstYPitch;
        dmaPusherPutAddress[7] = dwSrcYPitch;
        dmaPusherPutAddress[8] = (DWORD)pSurf_gbl->wHeight;
        dmaPusherPutAddress[9] = (2 << 8) | 1;
        dmaPusherPutAddress[10] = 0;

        dmaPusherPutAddress += 11;

        pDriverData->dDrawSpareSubchannelObject = 0;

        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        pDriverData->dwDmaPusherFreeCount = freeCount;

        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

     /* YV12/420i surfaces also need reformated before the video scaler can display them */
    } else if ((pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YV12) ||
               (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_420i)) {
        DWORD srcMemType = 0;
        DWORD interleaveUVFields = 0;

        /* First determine which source buffer is being used */
        dwSrcYOffset = pSurf_gbl->fpVidMem - pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0];

        dwSrcYPitch = (DWORD)pSurf_gbl->wWidth;
        dwSrcYPitch = (dwSrcYPitch + 3) & ~3L;

        dwSrcWidth = dwSrcYPitch;

        if (dwSrcYOffset == 0)
            dwDstYOffset = pDriverData->NvYUY2Surface0Mem -
                               pDriverData->BaseAddress;
        else if (pSurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1])
            dwDstYOffset = pDriverData->NvYUY2Surface1Mem -
                               pDriverData->BaseAddress;
        else if (pSurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2])
            dwDstYOffset = pDriverData->NvYUY2Surface2Mem -
                               pDriverData->BaseAddress;
        else
            dwDstYOffset = pDriverData->NvYUY2Surface3Mem -
                               pDriverData->BaseAddress;

        /* Now correct source offset if it is really in video memory */
        if ((pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] >= pDriverData->BaseAddress) && /* If video memory */
            (pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0] < pDriverData->BaseAddress + 0x1000000)) {
            dwSrcYOffset = pSurf_gbl->fpVidMem - pDriverData->BaseAddress;
            srcMemType = DDSCAPS_LOCALVIDMEM;
        }

        dwDstYPitch = ((pSurf_gbl->wWidth + 1) & 0xFFFE) << 1;
        dwDstYPitch = (dwDstYPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;

        dwSrcHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcHeight = (dwSrcHeight + 1) & ~1L;

        if (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YV12) {
            dwSrcVOffset = dwSrcYOffset + (dwSrcYPitch * dwSrcHeight);
            dwSrcUOffset = dwSrcVOffset + ((dwSrcYPitch * dwSrcHeight) >> 2);

            dwSrcUVPitch = (DWORD)pSurf_gbl->wWidth;
            dwSrcUVPitch = (dwSrcUVPitch + 3) & ~3L;
            dwSrcUVPitch >>= 1;

        } else { /* FOURCC_420i */
            dwSrcVOffset = dwSrcYOffset + dwSrcYPitch;
            dwSrcYPitch += (dwSrcYPitch >> 1);
            dwSrcUOffset = dwSrcVOffset + dwSrcYPitch;

            dwSrcUVPitch = dwSrcYPitch << 1;
        }

        dwSrcUVHeight = (DWORD)pSurf_gbl->wHeight;
        dwSrcUVHeight = (dwSrcUVHeight + 1) & ~1L;
        dwSrcUVHeight >>= 1;

        /*
         * NOTE:
         *
         * NV3 MemToMemFormat does not double buffer it's methods.
         * Therefore, if a MemToMemFormat operation is started within
         * a channel, sending an additional MemToMemFormat request down the
         * FIFO will corrupt the state of the first MemToMemFormat operation if
         * it had not yet finished.  However, loading a new object
         * into a subchannel will ALWAYS be blocked until the object
         * previously in that subchannel has completed it's operation.
         * Since we can't afford to wait around in this algorithm, we
         * take advantage of the fact that this object blocking will occur
         * by loading several different MemToMemFormat object instances
         * to accomplish our format conversion.
         */

        /* Let D3D code know that we have touched NV */
        pDriverData->TwoDRenderingOccurred = 1;

        while (freeCount < 11)
            NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
        freeCount -= 11;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        if (srcMemType == 0) /* If system memory */
            dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
        else
            dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
        dmaPusherPutAddress[3] = dwSrcVOffset;
        dmaPusherPutAddress[4] = (dwDstYOffset + 3);
        dmaPusherPutAddress[5] = dwSrcUVPitch;
        dmaPusherPutAddress[6] = (dwDstYPitch << 1);
        dmaPusherPutAddress[7] = (dwSrcWidth >> 1);
        dmaPusherPutAddress[8] = dwSrcUVHeight;
        dmaPusherPutAddress[9] = (4 << 8) | 1;
        dmaPusherPutAddress[10] = 0;

        dmaPusherPutAddress += 11;

        while (freeCount < 11)
            NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
        freeCount -= 11;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        if (srcMemType == 0) /* If system memory */
            dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
        else
            dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
        dmaPusherPutAddress[3] = dwSrcUOffset;
        dmaPusherPutAddress[4] = (dwDstYOffset + 1);
        dmaPusherPutAddress[5] = dwSrcUVPitch;
        dmaPusherPutAddress[6] = (dwDstYPitch << 1);
        dmaPusherPutAddress[7] = (dwSrcWidth >> 1);
        dmaPusherPutAddress[8] = dwSrcUVHeight;
        dmaPusherPutAddress[9] = (4 << 8) | 1;
        dmaPusherPutAddress[10] = 0;

        dmaPusherPutAddress += 11;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        // Get NV started on processing the image data
        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

        if (interleaveUVFields) { /* repeat for odd field */

            while (freeCount < 11)
                NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
            freeCount -= 11;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            if (srcMemType == 0) /* If system memory */
                dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
            else
                dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
            dmaPusherPutAddress[3] = (dwSrcVOffset + (dwSrcPitch >> 1));
            dmaPusherPutAddress[4] = (dwDstYOffset + 3);
            dmaPusherPutAddress[5] = dwSrcUVPitch;
            dmaPusherPutAddress[6] = (dwDstYPitch << 1);
            dmaPusherPutAddress[7] = (dwSrcWidth >> 1);
            dmaPusherPutAddress[8] = dwSrcUVHeight;
            dmaPusherPutAddress[9] = (4 << 8) | 1;
            dmaPusherPutAddress[10] = 0;

            dmaPusherPutAddress += 11;

            while (freeCount < 11)
                NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
            freeCount -= 11;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            if (srcMemType == 0) /* If system memory */
                dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
            else
                dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
            dmaPusherPutAddress[3] = (dwSrcUOffset + (dwSrcPitch >> 1));
            dmaPusherPutAddress[4] = (dwDstYOffset + 1);
            dmaPusherPutAddress[5] = dwSrcUVPitch;
            dmaPusherPutAddress[6] = (dwDstYPitch << 1);
            dmaPusherPutAddress[7] = (dwSrcWidth >> 1);
            dmaPusherPutAddress[8] = dwSrcUVHeight;
            dmaPusherPutAddress[9] = (4 << 8) | 1;
            dmaPusherPutAddress[10] = 0;

            dmaPusherPutAddress += 11;

            while (freeCount < 11)
                NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
            freeCount -= 11;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
            dmaPusherPutAddress[3] = (dwDstYOffset + 1 + (dwDstYPitch >> 1));
            dmaPusherPutAddress[4] = (dwDstYOffset + 1 + (dwDstYPitch >> 1) + dwDstYPitch);
            dmaPusherPutAddress[5] = (dwDstYPitch << 1);
            dmaPusherPutAddress[6] = (dwDstYPitch << 1);
            dmaPusherPutAddress[7] = (dwDstYPitch >> 2);
            dmaPusherPutAddress[8] = (DWORD)pSurf_gbl->wHeight >> 2;
            dmaPusherPutAddress[9] = (2 << 8) | 2;
            dmaPusherPutAddress[10] = 0;

            dmaPusherPutAddress += 11;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            // Get NV started on processing the image data
            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            /* restore Y pitch while processing interleaved Y values for full frame */
            dwDstYPitch >>= 1;
        }

        while (freeCount < 11)
            NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
        freeCount -= 11;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        if (srcMemType == 0) /* If system memory */
            dmaPusherPutAddress[1] = NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
        else
            dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
        dmaPusherPutAddress[3] = dwSrcYOffset;
        dmaPusherPutAddress[4] = dwDstYOffset;
        dmaPusherPutAddress[5] = dwSrcYPitch;
        dmaPusherPutAddress[6] = dwDstYPitch;
        dmaPusherPutAddress[7] = dwSrcWidth;
        dmaPusherPutAddress[8] = (DWORD)pSurf_gbl->wHeight;
        dmaPusherPutAddress[9] = (2 << 8) | 1;
        dmaPusherPutAddress[10] = 0;

        dmaPusherPutAddress += 11;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        // Get NV started on processing the image data
        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

        if (interleaveUVFields)
            dwDstYPitch <<= 1;

        while (freeCount < 11)
            NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
        freeCount -= 11;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;
        dmaPusherPutAddress[3] = (dwDstYOffset + 1);
        dmaPusherPutAddress[4] = (dwDstYOffset + 1 + dwDstYPitch);
        dmaPusherPutAddress[5] = (dwDstYPitch << 1);
        dmaPusherPutAddress[6] = (dwDstYPitch << 1);
        if (interleaveUVFields) {
            dmaPusherPutAddress[7] = (dwDstYPitch >> 2);
            dmaPusherPutAddress[8] = (DWORD)pSurf_gbl->wHeight >> 2;
        } else {
            dmaPusherPutAddress[7] = (dwDstYPitch >> 1);
            dmaPusherPutAddress[8] = (DWORD)pSurf_gbl->wHeight >> 1;
        }
        dmaPusherPutAddress[9] = (2 << 8) | 2;
        dmaPusherPutAddress[10] = 0;

        dmaPusherPutAddress += 11;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        // Get NV started on processing the image data
        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

        if (waitForCompletion) {
            NvNotification *pDmaSyncNotifier =
                (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;

            // MUST wait for any pending notification to prevent possible loss of notification serialization
            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

            while (freeCount < 4)
                NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
            freeCount -= 4;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

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

            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification
        }

        pDriverData->dDrawSpareSubchannelObject = 0;

        pDriverData->dwDmaPusherFreeCount = freeCount;

        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;
    }

    return DDHAL_DRIVER_HANDLED;

} /* ConvertOverlay32 */


/*
 * PreScaleOverlay32
 *
 * This function does not support interleaved overlay surfaces.
 *
 */

DWORD __stdcall
PreScaleOverlay32(LPDDRAWI_DDRAWSURFACE_LCL lpDDSurface )
{
    FAST Nv10ControlDma         *npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
    LPDDRAWI_DDRAWSURFACE_GBL   pSurf_gbl;
    DWORD                       caps;
    DWORD                       dwOffset;
    DWORD                       dwPitch;
    DWORD                       preScaleOverlay = 0;
    DWORD                       srcPreShrinkDeltaX;
    DWORD                       srcPreShrinkDeltaY;
    BOOL                        surfaceIs422 = FALSE;
    unsigned long               *dmaPusherPutAddress =
                                    (unsigned long *)pDriverData->NvDmaPusherPutAddress;
    unsigned long               *dmaPusherBufferBase =
                                    (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
    long                        freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
    long                        freeCount = 0;
#endif  /* CACHE_FREECOUNT */

    if (npDev == NULL) {
        return DDHAL_DRIVER_HANDLED;
    }

    pSurf_gbl = lpDDSurface->lpGbl;

    caps = lpDDSurface->ddsCaps.dwCaps;

    NV_DD_DMA_PUSHER_SYNC();

    /* Make sure we flip to the correct starting offset in new overlay surface */
    dwOffset = pSurf_gbl->fpVidMem - pDriverData->BaseAddress;

    if ((pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IF09) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YVU9) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IV32) ||
        (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_IV31)) {
        /* Skip over Indeo portion of surface */
        dwPitch = (DWORD)pSurf_gbl->wWidth;
        dwOffset += ((dwPitch * ((DWORD)pSurf_gbl->wHeight * 10L)) >> 3);
        /* Force block to be properly aligned */
        dwOffset = (dwOffset + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        dwPitch = ((pSurf_gbl->wWidth + 1) & 0xFFFE) << 1;
        dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
    } else if ((pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_YV12) ||
               (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_420i)) {
        /* Assumes a separate overlay YUY2 surface in video memory */
        if (pSurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0])
            dwOffset = pDriverData->NvYUY2Surface0Mem -
                           pDriverData->BaseAddress;
        else if (pSurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1])
            dwOffset = pDriverData->NvYUY2Surface1Mem -
                           pDriverData->BaseAddress;
        else if (pSurf_gbl->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2])
            dwOffset = pDriverData->NvYUY2Surface2Mem -
                           pDriverData->BaseAddress;
        else
            dwOffset = pDriverData->NvYUY2Surface3Mem -
                           pDriverData->BaseAddress;

        dwPitch = ((pSurf_gbl->wWidth + 1) & 0xFFFE) << 1;
        dwPitch = (dwPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
    } else {
        dwPitch = (DWORD)pSurf_gbl->lPitch;
        surfaceIs422 = TRUE;
    }

    srcPreShrinkDeltaX = 0x100000;
    srcPreShrinkDeltaY = 0x100000;

    if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

        if ((((pDriverData->dwOverlaySrcWidth + 7) >> 3) >
               pDriverData->dwOverlayDstWidth) && (pDriverData->dwOverlayDstWidth > 0)) {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
            srcPreShrinkDeltaX =
                (pDriverData->dwOverlaySrcWidth << 20) / pDriverData->dwOverlayDstWidth;
        }

        if ((((pDriverData->dwOverlaySrcHeight + 7) >> 3) >
               pDriverData->dwOverlayDstHeight) && (pDriverData->dwOverlayDstHeight > 0)) {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
            srcPreShrinkDeltaY =
                (pDriverData->dwOverlaySrcHeight << 20) / pDriverData->dwOverlayDstHeight;
        }
    } else {

        if ((pDriverData->dwOverlaySrcWidth >
               pDriverData->dwOverlayDstWidth) && (pDriverData->dwOverlayDstWidth > 0)) {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
            srcPreShrinkDeltaX =
                (pDriverData->dwOverlaySrcWidth << 20) / pDriverData->dwOverlayDstWidth;
        }

        if ((pDriverData->dwOverlaySrcHeight >
               pDriverData->dwOverlayDstHeight) && (pDriverData->dwOverlayDstHeight > 0)) {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
            srcPreShrinkDeltaY =
                (pDriverData->dwOverlaySrcHeight << 20) / pDriverData->dwOverlayDstHeight;
        }
    }

    if ((surfaceIs422) && (preScaleOverlay == 0) &&
        (pDriverData->dwOverlaySurfaces == 1) &&
        (pSurf_gbl->wHeight >= 480) &&
        (pDriverData->lockCount > 2))
        preScaleOverlay = NV_PRESCALE_OVERLAY_QUADRANT;

    /* Preshrink overlay surface if necessary */
    if ((pDriverData->TotalVRAM >> 20 > 4) && (preScaleOverlay)) {

        while (freeCount < 19)
            NvGetDmaBufferFreeCount(npDev, freeCount, 19, dmaPusherPutAddress);
        freeCount -= 19;

        /* Trash spare subchannel */
        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        dmaPusherPutAddress[1] = NV_DD_DVD_SUBPICTURE;
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
            NV038_IMAGE_OUT_POINT | 0x400000;
        dmaPusherPutAddress[3] = 0;

        /* Widths below are increased by 2 instead of 1 to remain hardware compatible */
        if (preScaleOverlay == NV_PRESCALE_OVERLAY_X) {
            dmaPusherPutAddress[4] =
                asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                               pSurf_gbl->wHeight);
        } else if (preScaleOverlay == NV_PRESCALE_OVERLAY_X) {
            dmaPusherPutAddress[4] =
                asmMergeCoords(((pSurf_gbl->wWidth + 2) & ~1),
                               pDriverData->dwOverlayDstHeight);
        } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_X + NV_PRESCALE_OVERLAY_Y)) {
            dmaPusherPutAddress[4] =
                asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                               pDriverData->dwOverlayDstHeight);
        } else {
            dmaPusherPutAddress[4] =
                asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 2) & ~1),
                               pDriverData->dwOverlaySrcHeight);
        }

        if (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY)
            dmaPusherPutAddress[5] =
                (NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | dwPitch;
        else
            dmaPusherPutAddress[5] =
                (NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | dwPitch;

        /* Output to extra overlay surface memory area */
        if ((surfaceIs422) && (pDriverData->extra422OverlayOffset[pDriverData->OverlayBufferIndex] != 0))
            dmaPusherPutAddress[6] =
                pDriverData->extra422OverlayOffset[pDriverData->OverlayBufferIndex];
        else
            dmaPusherPutAddress[6] = (dwOffset + pSurf_gbl->dwReserved1);

        dmaPusherPutAddress[7] = srcPreShrinkDeltaX;
        dmaPusherPutAddress[8] = srcPreShrinkDeltaY;

        dmaPusherPutAddress[9] = pDriverData->OverlaySrcSize;

        if (pSurf_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY)
            dmaPusherPutAddress[10] =
                (NV038_IMAGE_IN_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | dwPitch;
        else
            dmaPusherPutAddress[10] =
                (NV038_IMAGE_IN_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | dwPitch;

        /* Temporarily offset to the beginning of the visible portion of source surface */
        dwOffset += (pDriverData->OverlaySrcY * dwPitch);
        dwOffset += (pDriverData->OverlaySrcX << 1);

        dmaPusherPutAddress[11] = dwOffset;

        dmaPusherPutAddress[12] = 0;

        dmaPusherPutAddress[13] = srcPreShrinkDeltaX;
        dmaPusherPutAddress[14] = srcPreShrinkDeltaY;

        dmaPusherPutAddress[15] = pDriverData->OverlaySrcSize;

        dmaPusherPutAddress[16] =
            (NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | dwPitch;

        dmaPusherPutAddress[17] = dwOffset;

        dmaPusherPutAddress[18] = 0;

        dmaPusherPutAddress += 19;

        pDriverData->dDrawSpareSubchannelObject = 0;

        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        pDriverData->dwDmaPusherFreeCount = freeCount;

        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
    }

    pDriverData->dwDmaPusherFreeCount = freeCount;

    pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

    return DDHAL_DRIVER_HANDLED;
}



/*
 * UpdateOverlay32
 *
 *
 * NOTES:
 *
 * This callback is invoked to update an overlay surface.
 * This is where the src/destination is specified, any effects, and
 * it is shown or hidden
 *
 * For overlay surfaces NOT associated with a video port the surface will
 * be shown (DDOVER_SHOW) according to the following criteria.
 *
 *     DDOVER_INTERLEAVED   DDOVER_BOB      DISPLAYED
 *
 *             1                1           Even field
 *             1                0           Full surface (no special processing)
 *             0                X           Full surface (no special processing)
 *
 */

DWORD __stdcall
UpdateOverlay32( LPDDHAL_UPDATEOVERLAYDATA lpUOData )
{
    FAST Nv3ChannelPio          *npDevVideo;
    FAST long                   videoFreeCount;
    NvNotification              *pPioFlipOverlayNotifier;
    long                        srcDeltaX;
    long                        srcDeltaY;
    long                        srcPreShrinkDeltaX;
    long                        srcPreShrinkDeltaY;
    long                        dwSrcOffset;
    long                        dwSrcWidth;
    long                        dwSrcHeight;
    long                        dwDstWidth;
    long                        dwDstHeight;
    long                        dwSrcPitch;
    DWORD                       index = 0;
    DWORD                       match = 0;
    DWORD                       showInterleavedFields = 0;
    DWORD                       preScaleOverlay = 0;
    U032                        bMediaPortControl = FALSE;
    BOOL                        surfaceIs422 = FALSE;

    LPDDRAWI_DDRAWSURFACE_LCL   srcx = lpUOData->lpDDSrcSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   src = srcx->lpGbl;

    NvSetDriverDataPtr(lpUOData->lpDD);

    npDevVideo = (NvChannel *)pDriverData->NvDevVideoFlatPio;
    videoFreeCount = pDriverData->NvVideoFreeCount;
    pPioFlipOverlayNotifier = (NvNotification *)pDriverData->NvPioFlipOverlayNotifierFlat;

#if 0
        // This code will eventually be used to specially opt out of overlay stuff when video port is in use
    if( srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)  {

        lpUOData->ddRVal = HandleVideoPortOverlayUpdate( lpUOData);
    return DDHAL_DRIVER_HANDLED;

    }
#endif //
    if ((src->ddpfSurface.dwFourCC != FOURCC_UYVY) &&
        (src->ddpfSurface.dwFourCC != FOURCC_UYNV) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YUY2) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YUNV) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YV12) &&
        (src->ddpfSurface.dwFourCC != FOURCC_420i) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IF09) &&
        (src->ddpfSurface.dwFourCC != FOURCC_YVU9) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IV32) &&
        (src->ddpfSurface.dwFourCC != FOURCC_IV31)) {
            lpUOData->ddRVal = DDERR_INVALIDSURFACETYPE;
            return DDHAL_DRIVER_HANDLED;
    }

    if (!(srcx->ddsCaps.dwCaps & DDSCAPS_OVERLAY)) {
            lpUOData->ddRVal = DDERR_NOTAOVERLAYSURFACE;
            return DDHAL_DRIVER_HANDLED;
    }

    while ((index < NV_MAX_OVERLAY_SURFACES) && (!match))
        if ((DWORD)srcx == pDriverData->dwOverlaySurfaceLCL[index++])
            match = 1;

    if (!match) {
        lpUOData->ddRVal = DDERR_OUTOFCAPS;
        return DDHAL_DRIVER_HANDLED;
    }

    NV_DD_DMA_PUSHER_SYNC();

    if (npDevVideo == NULL) {  /* Mode switches might occur without warning */
        lpUOData->ddRVal = DDERR_SURFACELOST;
        pDriverData->fDDrawFullScreenDosOccurred = 0;    /* reset flag */
        pDriverData->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;
        pDriverData->dwOverlaySurfaces = 0;         /* should clean up AGP surface memory here if used */
        pDriverData->dwOverlayOwner = 0;
        pPioFlipOverlayNotifier[1].status = 0;      /* clear notifiers */
        pPioFlipOverlayNotifier[2].status = 0;
        return DDHAL_DRIVER_HANDLED;
    }
    pDriverData->dwRingZeroMutex = TRUE;

    if (pDriverData->fDDrawFullScreenDosOccurred) {
        pDriverData->OverlayRelaxOwnerCheck = 1;        /* relax owner check when we come back from DOS mode */
        pPioFlipOverlayNotifier[1].status = 0;      /* clear notifiers */
        pPioFlipOverlayNotifier[2].status = 0;
    }

    /* Alternate buffer indexes to keep the hardware happy */
    pDriverData->OverlayBufferIndex ^= 1;

    index = pDriverData->OverlayBufferIndex;
        
    if (lpUOData->dwFlags & DDOVER_HIDE ) {
        DPF("UPDATE OVERLAY - DDOVER_HIDE");

        pDriverData->dwOverlaySrcWidth = 0;
        pDriverData->dwOverlayDstWidth = 0;
        pDriverData->dwOverlaySrcHeight = 0;
        pDriverData->dwOverlayDstHeight = 0;

#ifdef NVPE
		// tell miniport to stop autoflip !
        if (srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)
			NVPUpdateOverlay();
#endif	// #ifdef NVPE

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

            while (videoFreeCount < 32)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 32;

            npDevVideo->dDrawSpare.SetObject = NV_VIDEO_OVERLAY;

       } else {

            while (videoFreeCount < 32)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 32;

            /* Trash spare subchannel */
            npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_COLOR_KEY;

            npDevVideo->dDrawSpare.videoColorKey.SetSize = 0;

            npDevVideo->dDrawSpare.SetObject = NV_DD_YUV422_VIDEO_FROM_MEMORY;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].offset = 0;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].pitch = 0;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].size = 0;

            if ((src->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
                (src->ddpfSurface.dwFourCC == FOURCC_UYNV))
                npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].format =
                    NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;
            else
                npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].format =
                    NV_VFM_FORMAT_COLOR_LE_V8Y8U8Y8;

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].notify = 0;
        }
        
          //pDriverData->dwRingZeroMutex = FALSE;

        pDriverData->dDrawSpareSubchannelObject = 0;
        pDriverData->OverlayFormat = 0;     // This should hide things, since we'll stop flipping
            
        /* Let DDraw know how many visible overlays are active */
        pDriverData->HALInfo.ddCaps.dwCurrVisibleOverlays = 0;

        if (pDriverData->fDDrawFullScreenDosOccurred) {
            if (pDriverData->fDDrawFullScreenDosOccurred != 2) {  /* still in DOS mode ? */
                pDriverData->fDDrawFullScreenDosOccurred = 0;    /* reset flag */
            }
        }

    } else {
        DPF("UPDATE OVERLAY - DDOVER_SHOW OR MOVE/RESIZE ");
        DPF( "Source Rect = (%ld,%ld), (%ld,%ld)",
             lpUOData->rSrc.left, lpUOData->rSrc.top,
             lpUOData->rSrc.right, lpUOData->rSrc.bottom );

        DPF( "Dest Rect = (%ld,%ld), (%ld,%ld)",
             lpUOData->rDest.left, lpUOData->rDest.top,
             lpUOData->rDest.right, lpUOData->rDest.bottom );

        if ((lpUOData->rDest.left < 0) || (lpUOData->rDest.top < 0)) {
            pDriverData->dwRingZeroMutex = FALSE;
            
                lpUOData->ddRVal = DDERR_INVALIDPOSITION;
            return DDHAL_DRIVER_HANDLED;
        }

        if (pDriverData->fDDrawFullScreenDosOccurred) {
            if (pDriverData->fDDrawFullScreenDosOccurred == 2) {  /* still in DOS mode ? */
                pDriverData->dwRingZeroMutex = FALSE;
                 lpUOData->ddRVal = DDERR_SURFACEBUSY;
                return DDHAL_DRIVER_HANDLED;
            } else {                                        /* back to hires mode */
                pDriverData->fDDrawFullScreenDosOccurred = 0;    /* reset flag */
            }
        }

        /* This surface may need converted */
        if (pDriverData->dwOverlaySurfaces > 2)
            ConvertOverlay32(srcx, TRUE);

        dwSrcWidth = lpUOData->rSrc.right - lpUOData->rSrc.left;
        dwSrcHeight = lpUOData->rSrc.bottom - lpUOData->rSrc.top;
        dwDstWidth = lpUOData->rDest.right - lpUOData->rDest.left;
        dwDstHeight = lpUOData->rDest.bottom - lpUOData->rDest.top;
            
        if (srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) {
            if (lpUOData->dwFlags & DDOVER_AUTOFLIP) {
                bMediaPortControl = TRUE;
            }
        }       
            
        /* Remember sizes so Flip32 knows what to do */
        pDriverData->dwOverlaySrcWidth = dwSrcWidth;
        pDriverData->dwOverlayDstWidth = dwDstWidth;
        pDriverData->dwOverlaySrcHeight = dwSrcHeight;
        pDriverData->dwOverlayDstHeight = dwDstHeight;

        /* Align to 2 pixel boundary + 1 for video scaler */
        dwDstWidth = (dwDstWidth + 2) & ~1L;
        dwDstHeight = (dwDstHeight + 1) & ~1L;

        srcDeltaX = (dwSrcWidth - 1) << 16;
        srcDeltaX /= dwDstWidth - 1;
        srcDeltaX <<= 4;

        srcDeltaY = (dwSrcHeight - 1) << 16;
        srcDeltaY /= dwDstHeight - 1;
        srcDeltaY <<= 4;

        pDriverData->dwOverlayDeltaY = (DWORD)srcDeltaY;

        if( !(srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) ) {
            if ((lpUOData->dwFlags & DDOVER_INTERLEAVED) &&
                (lpUOData->dwFlags & DDOVER_BOB)) {

                showInterleavedFields = 1;
                dwSrcHeight >>= 1;
#ifdef  NOPRESTRETCH
                srcDeltaY = (dwSrcHeight - 1) << 16;
                srcDeltaY /= dwDstHeight - 1;
                srcDeltaY <<= 4;
#endif  // NOPRESTRETCH
            }
        }

        srcPreShrinkDeltaX = 0x100000;
        srcPreShrinkDeltaY = 0x100000;

        if (pDriverData->NvDeviceVersion >= NV_DEVICE_VERSION_10) {

            if ((((pDriverData->dwOverlaySrcWidth + 7) >> 3) >
                   pDriverData->dwOverlayDstWidth) && (pDriverData->dwOverlayDstWidth > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
                srcPreShrinkDeltaX =
                    (pDriverData->dwOverlaySrcWidth << 20) / pDriverData->dwOverlayDstWidth;
                srcDeltaX = 0x100000;
            }

            if ((((pDriverData->dwOverlaySrcHeight + 7) >> 3) >
                   pDriverData->dwOverlayDstHeight) && (pDriverData->dwOverlayDstHeight > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
                srcPreShrinkDeltaY =
                    (pDriverData->dwOverlaySrcHeight << 20) / pDriverData->dwOverlayDstHeight;
#ifdef  NOPRESTRETCH
                if (showInterleavedFields) {
                    srcDeltaY = 0x80000;
                    pDriverData->dwOverlayDeltaY = 0x80000;
                } else {
#endif  // NOPRESTRETCH
                    srcDeltaY = 0x100000;
                    pDriverData->dwOverlayDeltaY = 0x100000;
#ifdef  NOPRESTRETCH
                }
#endif  // NOPRESTRETCH
            }
        } else {

            if ((pDriverData->dwOverlaySrcWidth >
                   pDriverData->dwOverlayDstWidth) && (pDriverData->dwOverlayDstWidth > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_X;
                srcPreShrinkDeltaX =
                    (pDriverData->dwOverlaySrcWidth << 20) / pDriverData->dwOverlayDstWidth;
                srcDeltaX = 0x100000;
            }

            if ((pDriverData->dwOverlaySrcHeight >
                   pDriverData->dwOverlayDstHeight) && (pDriverData->dwOverlayDstHeight > 0)) {
                preScaleOverlay |= NV_PRESCALE_OVERLAY_Y;
                srcPreShrinkDeltaY =
                    (pDriverData->dwOverlaySrcHeight << 20) / pDriverData->dwOverlayDstHeight;
#ifdef  NOPRESTRETCH
                if (showInterleavedFields) {
                    srcDeltaY = 0x80000;
                    pDriverData->dwOverlayDeltaY = 0x80000;
                } else {
#endif  // NOPRESTRETCH
                    srcDeltaY = 0x100000;
                    pDriverData->dwOverlayDeltaY = 0x100000;
#ifdef  NOPRESTRETCH
                }
#endif  // NOPRESTRETCH
            }
        }

#ifndef NOPRESTRETCH
        if ((showInterleavedFields) && (!(srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT))) {
            preScaleOverlay |= NV_PRESCALE_OVERLAY_FIELD;
            srcPreShrinkDeltaY >>= 1;
        }
#endif  // NOPRESTRETCH

        /* Check for Quadrant MCI DVD decoder */
        if ((preScaleOverlay == 0) && (surfaceIs422) &&
            (pDriverData->dwOverlaySurfaces == 1) &&
            (showInterleavedFields == 0) &&
            (src->wHeight >= 480) &&  // cannot make this larger due to widescreen problems
            (!(srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)))
            preScaleOverlay = NV_PRESCALE_OVERLAY_QUADRANT;


        /* Must wait for any pending overlay flip to finish */
        /* if it is an autoflip then do not wait */
        if (!(lpUOData->dwFlags & DDOVER_AUTOFLIP)) {
            while (((volatile)pPioFlipOverlayNotifier[index + 1].status == NV_IN_PROGRESS))
                NV_SLEEP;
        }

        pPioFlipOverlayNotifier[index + 1].status = NV_IN_PROGRESS;

        dwSrcOffset = src->fpVidMem - pDriverData->BaseAddress;

        if ((src->ddpfSurface.dwFourCC == FOURCC_IF09) ||
            (src->ddpfSurface.dwFourCC == FOURCC_YVU9) ||
            (src->ddpfSurface.dwFourCC == FOURCC_IV32) ||
            (src->ddpfSurface.dwFourCC == FOURCC_IV31)) {
            /* Skip over Indeo portion of surface */
            dwSrcPitch = (DWORD)src->wWidth;
            dwSrcOffset += ((dwSrcPitch * ((DWORD)src->wHeight * 10L)) >> 3);
            /* Force block to be properly aligned */
            dwSrcOffset = (dwSrcOffset + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
            dwSrcPitch = ((src->wWidth + 1) & 0xFFFE) << 1;
            dwSrcPitch = (dwSrcPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        } else if ((src->ddpfSurface.dwFourCC == FOURCC_YV12) ||
                   (src->ddpfSurface.dwFourCC == FOURCC_420i)) {
            /* Assumes a separate overlay YUY2 surface in video memory */
            if (src->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[0])
                dwSrcOffset = pDriverData->NvYUY2Surface0Mem -
                                  pDriverData->BaseAddress;
            else if (src->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[1])
                dwSrcOffset = pDriverData->NvYUY2Surface1Mem -
                                  pDriverData->BaseAddress;
            else if (src->fpVidMem == pDriverData->NvFloatingMem2MemFormatSurfaceAddress[2])
                dwSrcOffset = pDriverData->NvYUY2Surface2Mem -
                                  pDriverData->BaseAddress;
            else
                dwSrcOffset = pDriverData->NvYUY2Surface3Mem -
                                  pDriverData->BaseAddress;

            dwSrcPitch = ((src->wWidth + 1) & 0xFFFE) << 1;
            dwSrcPitch = (dwSrcPitch + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad;
        } else {
            dwSrcPitch = (DWORD)src->lPitch;
            surfaceIs422 = TRUE;
        }

        if (showInterleavedFields)
            dwSrcPitch <<= 1;

        /* Let the world know what the current overlay src offset is */
        pDriverData->OverlaySrcX = (short)(lpUOData->rSrc.left & ~1);
        pDriverData->OverlaySrcY = (short)lpUOData->rSrc.top;

        /* Preshrink overlay surface if necessary */
        if ((pDriverData->TotalVRAM >> 20 > 4) && (preScaleOverlay) &&
            (!(srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT))) {

            /* needed for Quadrant MCI DVD decoder fix only */
            pDriverData->lockCount = 0;

            while (videoFreeCount < 76)
                videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
            videoFreeCount -= 76;

            //pDriverData->dwRingZeroMutex = TRUE;
            
                /* Trash spare subchannel */
            npDevVideo->dDrawSpare.SetObject = NV_DD_DVD_SUBPICTURE;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutPoint = 0;

            /* Widths below are increased by 2 instead of 1 to remain hardware compatible */
            if (preScaleOverlay == NV_PRESCALE_OVERLAY_X) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                   dwSrcHeight);
            } else if (preScaleOverlay == NV_PRESCALE_OVERLAY_Y) {
                if (showInterleavedFields) {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((dwSrcWidth + 2) & ~1),
                                       (pDriverData->dwOverlayDstHeight >> 1));
                } else {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((dwSrcWidth + 2) & ~1),
                                       pDriverData->dwOverlayDstHeight);
                }
            } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_X + NV_PRESCALE_OVERLAY_Y)) {
                if (showInterleavedFields) {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                       (pDriverData->dwOverlayDstHeight >> 1));
                } else {
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                        asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                       pDriverData->dwOverlayDstHeight);
                }
            } else if (preScaleOverlay == NV_PRESCALE_OVERLAY_FIELD) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 2) & ~1),
                                   pDriverData->dwOverlaySrcHeight);
            } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_X + NV_PRESCALE_OVERLAY_FIELD)) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                   pDriverData->dwOverlaySrcHeight);
            } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_Y + NV_PRESCALE_OVERLAY_FIELD)) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((dwSrcWidth + 2) & ~1),
                                   pDriverData->dwOverlayDstHeight);
            } else if (preScaleOverlay == (NV_PRESCALE_OVERLAY_X + NV_PRESCALE_OVERLAY_Y + NV_PRESCALE_OVERLAY_FIELD)) {
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlayDstWidth + 2) & ~1),
                                   pDriverData->dwOverlayDstHeight);
            } else { // NV_PRESCALE_OVERLAY_QUADRANT
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutSize =
                    asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 2) & ~1),
                                   pDriverData->dwOverlaySrcHeight);
            }

            if (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD) {
                if (src->ddpfSurface.dwFourCC == FOURCC_UYVY)
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | (dwSrcPitch >> 1);
                else
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | (dwSrcPitch >> 1);
            } else {
                if (src->ddpfSurface.dwFourCC == FOURCC_UYVY)
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | dwSrcPitch;
                else
                    npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutFormat =
                        (NV038_IMAGE_OUT_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | dwSrcPitch;
            }

            /* Output to extra overlay surface memory area */
            if ((surfaceIs422) && (pDriverData->extra422OverlayOffset[index] != 0))
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutOffset =
                    pDriverData->extra422OverlayOffset[index];
            else
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageOutOffset =
                    (dwSrcOffset + src->dwReserved1);

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInDeltaDuDx = srcPreShrinkDeltaX;
            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInDeltaDvDy = srcPreShrinkDeltaY;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInSize =
                asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 1) & ~1), dwSrcHeight);

            if (src->ddpfSurface.dwFourCC == FOURCC_UYVY)
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInFormat =
                    (NV038_IMAGE_IN_FORMAT_COLOR_LE_YB8V8YA8U8 << 16) | dwSrcPitch;
            else
                npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInFormat =
                    (NV038_IMAGE_IN_FORMAT_COLOR_LE_V8YB8U8YA8 << 16) | dwSrcPitch;

            /* Temporarily offset to the beginning of the visible portion of source surface */
            dwSrcOffset += (lpUOData->rSrc.top * dwSrcPitch);
            dwSrcOffset += ((lpUOData->rSrc.left & ~1) << 1);

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInOffset =
                dwSrcOffset;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.ImageInPoint = 0;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayDeltaDuDx = srcPreShrinkDeltaX;
            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayDeltaDvDy = srcPreShrinkDeltaY;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlaySize =
                asmMergeCoords(((pDriverData->dwOverlaySrcWidth + 1) & ~1), dwSrcHeight);

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayFormat =
                (NV038_OVERLAY_FORMAT_COLOR_LE_TRANSPARENT << 16) | dwSrcPitch;

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayOffset =
                dwSrcOffset;

            /* Restore offset */
            dwSrcOffset -= (lpUOData->rSrc.top * dwSrcPitch);
            dwSrcOffset -= ((lpUOData->rSrc.left & ~1) << 1);

            npDevVideo->dDrawSpare.nv4DvdSubpicture.OverlayPoint = 0;

            pDriverData->dDrawSpareSubchannelObject = 0;

            /* Change offset to point to shrunken overlay surface memory */
            if ((surfaceIs422) && (pDriverData->extra422OverlayOffset[index] != 0))
                dwSrcOffset = pDriverData->extra422OverlayOffset[index];
            else
                dwSrcOffset += src->dwReserved1;

            /* Restore overlay source pitch and height to normal since we prescaled it */
            if (preScaleOverlay & NV_PRESCALE_OVERLAY_FIELD) {
                dwSrcPitch >>= 1;
                dwSrcHeight <<= 1;
            }

            /* Force fifo to stall until shrink operation finishes */
            npDevVideo->dDrawSpare.nv4DvdSubpicture.Notify = 0;
            npDevVideo->dDrawSpare.nv4DvdSubpicture.NoOperation = 0;
                
                //pDriverData->dwRingZeroMutex = FALSE;
            
        } else {
            /* Offset to visible portion of source surface */
            dwSrcOffset += (lpUOData->rSrc.top * dwSrcPitch);
            dwSrcOffset += ((lpUOData->rSrc.left & ~1) << 1);
        }

        while (videoFreeCount <56)
            videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
        videoFreeCount -= 56;

        //pDriverData->dwRingZeroMutex = TRUE;
            
        /* Trash spare subchannel */
        npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_COLOR_KEY;
        npDevVideo->dDrawSpare.videoColorKey.SetPoint =
            asmMergeCoords(lpUOData->rDest.left, lpUOData->rDest.top);
        npDevVideo->dDrawSpare.videoColorKey.SetSize =
            asmMergeCoords((dwDstWidth & ~1), (dwDstHeight & ~1));

        /*
         * color key?
         */
        if( lpUOData->dwFlags & (DDOVER_KEYDESTOVERRIDE) ) {
            DPF( "Dest ColorKey = %08lx,%08lx",
                lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue,
                lpUOData->overlayFX.dckDestColorkey.dwColorSpaceHighValue );

            if (pDriverData->bi.biBitCount == 8)
                npDevVideo->dDrawSpare.videoColorKey.SetColorKey =
                    lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue |
                        NV_ALPHA_1_008;
            else if (pDriverData->bi.biBitCount == 16)
                npDevVideo->dDrawSpare.videoColorKey.SetColorKey =
                    lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue |
                        NV_ALPHA_1_016;
            else
                npDevVideo->dDrawSpare.videoColorKey.SetColorKey =
                    lpUOData->overlayFX.dckDestColorkey.dwColorSpaceLowValue |
                        NV_ALPHA_1_032;

        } else // disable color key
            npDevVideo->dDrawSpare.videoColorKey.SetColorKey = 0;


        npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_SCALER;
        npDevVideo->dDrawSpare.videoScaler.SetPoint =
            asmMergeCoords(lpUOData->rDest.left, lpUOData->rDest.top);
        npDevVideo->dDrawSpare.videoScaler.SetDeltaDuDx = srcDeltaX;

        if( ! bMediaPortControl )
            npDevVideo->dDrawSpare.videoScaler.SetDeltaDvDy = srcDeltaY;

        npDevVideo->dDrawSpare.SetObject = NV_DD_YUV422_VIDEO_FROM_MEMORY;

// temp defines b4 becoming a class standard
#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_0 (0x04000000)
#define NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1 (0x02000000)
#define NV_VFM_FORMAT_BOBFROMINTERLEAVE           (0x08000000)

        if (lpUOData->dwFlags & DDOVER_AUTOFLIP)
            index = 0; // overide index for mediaport/overlay autoflip

        if ((src->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
            (src->ddpfSurface.dwFourCC == FOURCC_UYNV)) {
            U032 dwFormat = NV_VFM_FORMAT_COLOR_LE_Y8V8Y8U8;

            // we need to know if we're bobbing
            if((lpUOData->dwFlags & DDOVER_BOB) && (lpUOData->dwFlags & DDOVER_INTERLEAVED) ){
                dwFormat = dwFormat | NV_VFM_FORMAT_BOBFROMINTERLEAVE;
                DPF( "Video Port - Interleaved - BOB");
            }

            if (srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT) {

                if (lpUOData->dwFlags & DDOVER_AUTOFLIP) {
                    dwFormat |= (index ? NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_1 :
                                         NV_VFM_FORMAT_CONTROL_FLIP_BY_MEDIAPORT_0);
                }

                if ((lpUOData->dwFlags & DDOVER_BOB) && (index == 1)) {
                    // improves quality on BOB for low resolution images
                    // only do a BOB if srcY >= 120
                    if ((lpUOData->rSrc.bottom - lpUOData->rSrc.top) >=120)
                        dwSrcPitch |= 2; // tell overlay to do a halfline offset
                }
            }

            if( ! bMediaPortControl )
                npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].format =
                    dwFormat;
            /* Let the world know what the overlay src info is */
            pDriverData->OverlayFormat = dwFormat;
        } else
            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].format =
                NV_VFM_FORMAT_COLOR_LE_V8Y8U8Y8;

        /* Let the world know what the overlay src info is */
        pDriverData->OverlaySrcOffset = dwSrcOffset;

        pDriverData->OverlaySrcPitch = dwSrcPitch;

        pDriverData->OverlaySrcSize = asmMergeCoords(dwSrcWidth, dwSrcHeight);

        if( ! bMediaPortControl ) {
            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].offset =
                dwSrcOffset;
            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].pitch =
                dwSrcPitch;
            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].size =
                asmMergeCoords(dwSrcWidth, dwSrcHeight);

            npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].notify = 0;
        }
      //pDriverData->dwRingZeroMutex = FALSE;
            
        pDriverData->dDrawSpareSubchannelObject = 0;

        /* Quadrant DVD decoder hack should NOT go here because DVD subpicture slows things down enough */

        /* Let DDraw know how many visible overlays are active */
        if (pDriverData->HALInfo.ddCaps.dwCurrVisibleOverlays == 0) {
            pDriverData->HALInfo.ddCaps.dwCurrVisibleOverlays = 1;
        }
    }

    pDriverData->NvVideoFreeCount = (short)videoFreeCount;
    
    pDriverData->dwRingZeroMutex = FALSE;

#ifdef NVPE
	///// H.AZAR: (03/01/1999): notify miniport of overlay changes !
	if (bMediaPortControl)
		NVPUpdateOverlay();
#endif	// #ifdef NVPE

    lpUOData->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

} /* UpdateOverlay32 */


/*
 * SetOverlayPosition32
 */
DWORD __stdcall
SetOverlayPosition32( LPDDHAL_SETOVERLAYPOSITIONDATA lpSOPData )
{
    FAST Nv3ChannelPio          *npDevVideo;
    FAST long                   videoFreeCount;
    DWORD                       index = 0;
    DWORD                       match = 0;
    LPDDRAWI_DDRAWSURFACE_LCL   srcx = lpSOPData->lpDDSrcSurface;

    NvSetDriverDataPtr(lpSOPData->lpDD);

    npDevVideo = (NvChannel *)pDriverData->NvDevVideoFlatPio;
    videoFreeCount = pDriverData->NvVideoFreeCount;

    if (!(srcx->ddsCaps.dwCaps & DDSCAPS_OVERLAY)) {
            lpSOPData->ddRVal = DDERR_NOTAOVERLAYSURFACE;
            return DDHAL_DRIVER_HANDLED;
    }

    if ((lpSOPData->lXPos < 0) || (lpSOPData->lYPos < 0)) {
            lpSOPData->ddRVal = DDERR_INVALIDPOSITION;
            return DDHAL_DRIVER_HANDLED;
    }

    while ((index < NV_MAX_OVERLAY_SURFACES) && (!match))
        if ((DWORD)srcx == pDriverData->dwOverlaySurfaceLCL[index++])
            match = 1;

    if (!match) {
        lpSOPData->ddRVal = DDERR_OUTOFCAPS;
        return DDHAL_DRIVER_HANDLED;
    }

#if 0
        // This code will eventually be used to specially opt out of overlay stuff when video port is in use
    if( srcx->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT)  {

        lpUOData->ddRVal = HandleVideoPortOverlayPosition( lpSOPData);
    return DDHAL_DRIVER_HANDLED;

    }
#endif //

    NV_DD_DMA_PUSHER_SYNC();

    if (npDevVideo == NULL) {  /* Mode switches might occur without warning */
        lpSOPData->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
    }

    while (videoFreeCount < 28)
        videoFreeCount = NvGetFreeCount(npDevVideo, NV_DD_SPARE);
    videoFreeCount -= 28;

    /*
     * NOTES:
     *
     * This callback is invoked to set an overlay position
     */
    DPF( "SET OVERLAY POSITION" );
    DPF( "Overlay surface = %08lx", lpSOPData->lpDDSrcSurface );
    DPF( "(%ld,%ld)", lpSOPData->lXPos, lpSOPData->lYPos );

    pDriverData->dwRingZeroMutex = TRUE;
     /* Trash spare subchannel */
    npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_COLOR_KEY;
    npDevVideo->dDrawSpare.videoColorKey.SetPoint =
        asmMergeCoords(lpSOPData->lXPos, lpSOPData->lYPos);

    npDevVideo->dDrawSpare.SetObject = NV_DD_VIDEO_SCALER;
    npDevVideo->dDrawSpare.videoScaler.SetPoint =
        asmMergeCoords(lpSOPData->lXPos, lpSOPData->lYPos);

    /* Our new mechanism requires that we use the VFM notifier to make this happen */
    npDevVideo->dDrawSpare.SetObject = NV_DD_YUV422_VIDEO_FROM_MEMORY;

    /* Alternate buffer indexes to keep the hardware happy */
    pDriverData->OverlayBufferIndex ^= 1;

    index = pDriverData->OverlayBufferIndex;

    npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].offset =
        pDriverData->OverlaySrcOffset;
    npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].pitch =
        pDriverData->OverlaySrcPitch;
    npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].size =
        pDriverData->OverlaySrcSize;
    /* We won't wait for this notifier so status won't be initialized */
    npDevVideo->dDrawSpare.videoFromMemory.ImageScan[index].notify = 0;

    pDriverData->dwRingZeroMutex = FALSE;
    pDriverData->dDrawSpareSubchannelObject = 0;

    pDriverData->NvVideoFreeCount = (short)videoFreeCount;

    lpSOPData->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

} /* SetOverlayPosition32 */



/*
 * DestroyFloating422Context32
 *
 */

DWORD __stdcall
DestroyFloating422Context32(LPDDRAWI_DDRAWSURFACE_LCL lpDDSurface)
{
    FAST Nv10ControlDma         *npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
    LPDDRAWI_DDRAWSURFACE_LCL   psurf;
    LPDDRAWI_DDRAWSURFACE_GBL   psurf_gbl;
    unsigned long               *dmaPusherPutAddress =
                                    (unsigned long *)pDriverData->NvDmaPusherPutAddress;
    unsigned long               *dmaPusherBufferBase =
                                    (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  CACHE_FREECOUNT
    long                        freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
    long                        freeCount = 0;
#endif  /* CACHE_FREECOUNT */

    NV_DD_DMA_PUSHER_SYNC();

    /* Let D3D code know that we have touched NV */
    pDriverData->TwoDRenderingOccurred = 1;

    psurf = lpDDSurface;
    psurf_gbl = psurf->lpGbl;

   if ((psurf_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
       (psurf_gbl->ddpfSurface.dwFourCC == FOURCC_UYNV)) {
        if ((pDriverData->fNvActiveFloatingContexts &
            NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
           (pDriverData->NvFloating0UYVYSurfaceMem == psurf_gbl->fpVidMem)) {
                long countDown;
            NvNotification *pDmaSyncNotifier =
                (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;

            // MUST wait for any pending notification to prevent possible loss of notification serialization
            countDown = 0x200000;
                while (((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) && (--countDown > 0)) NV_SLEEP;


            while (freeCount < 8)
                NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
            freeCount -= 8;

            pDriverData->dDrawSpareSubchannelObject = 0;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

            /* Trash spare subchannel */
            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
            dmaPusherPutAddress[5] = 0;
            dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
            dmaPusherPutAddress[7] = 0;

            dmaPusherPutAddress += 8;

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            countDown = 0x200000;
                while (((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS)&& (--countDown > 0)) NV_SLEEP; // wait for notification


#ifdef  MULTI_MON
            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                     NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
#else   /* MULTI_MON */
            while (freeCount < 6)
                NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
            freeCount -= 6;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0xC0000;
            dmaPusherPutAddress[3] = 0; // unlock system memory
            dmaPusherPutAddress[4] = 0;
            dmaPusherPutAddress[5] = 0;

            dmaPusherPutAddress += 6;
#endif  /* MULTI_MON */

            pDriverData->fNvActiveFloatingContexts &=
                ~NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        } else if ((pDriverData->fNvActiveFloatingContexts &
            NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
           (pDriverData->NvFloating1UYVYSurfaceMem == psurf_gbl->fpVidMem)) {
            NvNotification *pDmaSyncNotifier =
                (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;

            // MUST wait for any pending notification to prevent possible loss of notification serialization
            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

            while (freeCount < 8)
                NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
            freeCount -= 8;

            pDriverData->dDrawSpareSubchannelObject = 0;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

            /* Trash spare subchannel */
            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
            dmaPusherPutAddress[5] = 0;
            dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
            dmaPusherPutAddress[7] = 0;

            dmaPusherPutAddress += 8;

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

#ifdef  MULTI_MON
            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                     NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
#else   /* MULTI_MON */
            while (freeCount < 6)
                NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
            freeCount -= 6;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0xC0000;
            dmaPusherPutAddress[3] = 0; // unlock system memory
            dmaPusherPutAddress[4] = 0;
            dmaPusherPutAddress[5] = 0;

            dmaPusherPutAddress += 6;
#endif  /* MULTI_MON */

            pDriverData->fNvActiveFloatingContexts &=
                ~NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
        }
    } else { /* YUYV */
        if ((pDriverData->fNvActiveFloatingContexts &
            NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
           (pDriverData->NvFloating0YUYVSurfaceMem == psurf_gbl->fpVidMem)) {
            NvNotification *pDmaSyncNotifier =
                (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;

            // MUST wait for any pending notification to prevent possible loss of notification serialization
            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

            while (freeCount < 8)
                NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
            freeCount -= 8;

            pDriverData->dDrawSpareSubchannelObject = 0;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

            /* Trash spare subchannel */
            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
            dmaPusherPutAddress[5] = 0;
            dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
            dmaPusherPutAddress[7] = 0;

            dmaPusherPutAddress += 8;

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

#ifdef  MULTI_MON
            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                     NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
#else   /* MULTI_MON */
            while (freeCount < 6)
                NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
            freeCount -= 6;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0xC0000;
            dmaPusherPutAddress[3] = 0; // unlock system memory
            dmaPusherPutAddress[4] = 0;
            dmaPusherPutAddress[5] = 0;

            dmaPusherPutAddress += 6;
#endif  /* MULTI_MON */

            pDriverData->fNvActiveFloatingContexts &=
                ~NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

        } else if ((pDriverData->fNvActiveFloatingContexts &
            NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
           (pDriverData->NvFloating1YUYVSurfaceMem == psurf_gbl->fpVidMem)) {
            NvNotification *pDmaSyncNotifier =
                (NvNotification *)pDriverData->NvDmaSyncNotifierFlat;

            // MUST wait for any pending notification to prevent possible loss of notification serialization
            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

            while (freeCount < 8)
                NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
            freeCount -= 8;

            pDriverData->dDrawSpareSubchannelObject = 0;

            pDmaSyncNotifier->status = NV_IN_PROGRESS;

            /* Trash spare subchannel */
            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = NV1_NULL_OBJECT;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NOTIFY_OFFSET | 0x40000;
            dmaPusherPutAddress[5] = 0;
            dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_NO_OPERATION_OFFSET | 0x40000;
            dmaPusherPutAddress[7] = 0;

            dmaPusherPutAddress += 8;

            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

            /* Force write combine buffer to flush */
            pDriverData->NvDmaPusherBufferEnd[0] = 0;
            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
            _outp (0x3d0,0);

            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

            while ((volatile)pDmaSyncNotifier->status == NV_IN_PROGRESS) NV_SLEEP; // wait for notification

#ifdef  MULTI_MON
            NvRmFree(pDriverData->dwRootHandle, pDriverData->dwRootHandle,
                     NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY);
#else   /* MULTI_MON */
            while (freeCount < 6)
                NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
            freeCount -= 6;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
            dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0xC0000;
            dmaPusherPutAddress[3] = 0; // unlock system memory
            dmaPusherPutAddress[4] = 0;
            dmaPusherPutAddress[5] = 0;

            dmaPusherPutAddress += 6;
#endif  /* MULTI_MON */

            pDriverData->fNvActiveFloatingContexts &=
                ~NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
        }
    }

    pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

    /* Force write combine buffer to flush */
    pDriverData->NvDmaPusherBufferEnd[0] = 0;
    /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
    _outp (0x3d0,0);

    pDriverData->dwDmaPusherFreeCount = freeCount;

    npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

    return DDHAL_DRIVER_HANDLED;
}



/*
 * CreateFloating422Context32
 *
 */

DWORD __stdcall
CreateFloating422Context32(LPDDRAWI_DDRAWSURFACE_LCL lpDDSurface, DWORD dwBlockSize)
{
   FAST Nv10ControlDma         *npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;
   NvNotification              *pDmaDmaToMemNotifier =
                                   (NvNotification *)pDriverData->NvDmaDmaToMemNotifierFlat;
   LPDDRAWI_DDRAWSURFACE_LCL   psurf;
   LPDDRAWI_DDRAWSURFACE_GBL   psurf_gbl;
   unsigned long               *dmaPusherPutAddress =
                                   (unsigned long *)pDriverData->NvDmaPusherPutAddress;
   unsigned long               *dmaPusherBufferBase =
                                   (unsigned long *)pDriverData->NvDmaPusherBufferBase;
#ifdef  MULTI_MON
   unsigned long               status;
#endif  /* MULTI_MON */
#ifdef  CACHE_FREECOUNT
   long                        freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
   long                        freeCount = 0;
#endif  /* CACHE_FREECOUNT */

   NV_DD_DMA_PUSHER_SYNC();

   /* Let D3D code know that we have touched NV */
   pDriverData->TwoDRenderingOccurred = 1;

   psurf = lpDDSurface;
   psurf_gbl = psurf->lpGbl;

   if ((psurf_gbl->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
       (psurf_gbl->ddpfSurface.dwFourCC == FOURCC_UYNV)) {
       if ((pDriverData->fNvActiveFloatingContexts &
           NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) == 0) {

#ifdef MULTI_MON
           status = NvRmAllocContextDma(pDriverData->dwRootHandle,
                               NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                               NV01_CONTEXT_DMA,
                               (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                               (PVOID)psurf_gbl->fpVidMem,
                               (dwBlockSize - 1));

           if (status == ALLOC_CTX_DMA_STATUS_SUCCESS) {

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating0UYVYSurfaceMem = psurf_gbl->fpVidMem;

               while (freeCount < 4)
                   NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
               freeCount -= 4;

               pDriverData->dDrawSpareSubchannelObject = 0;

               /* Trash spare subchannel */
               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
               dmaPusherPutAddress[1] = NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
               dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                        SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
               dmaPusherPutAddress[3] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               dmaPusherPutAddress += 4;

               pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

               /* Force write combine buffer to flush */
               pDriverData->NvDmaPusherBufferEnd[0] = 0;
               /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
               _outp (0x3d0,0);

               pDriverData->dwDmaPusherFreeCount = freeCount;

               npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
           }

#else   /* MULTI_MON */

           pDmaDmaToMemNotifier->status = NV_IN_PROGRESS;

           /* Trash spare subchannel */
           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0x80000;
           dmaPusherPutAddress[3] = (unsigned long)psurf_gbl->fpVidMem;
           dmaPusherPutAddress[4] = (unsigned long)NvGetFlatDataSelector();
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_NOTIFY_OFFSET | 0x40000;
           dmaPusherPutAddress[6] = 0;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_LIMIT_OFFSET | 0x40000;
           dmaPusherPutAddress[8] = dwBlockSize - 1;

           dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
           dmaPusherPutAddress[12] = NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

           dmaPusherPutAddress += 13;

           pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

           /* Force write combine buffer to flush */
           pDriverData->NvDmaPusherBufferEnd[0] = 0;
           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
           _outp (0x3d0,0);

           pDriverData->dwDmaPusherFreeCount = freeCount;

           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

           while ((volatile)pDmaDmaToMemNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

           if (pDmaDmaToMemNotifier->status == NV_STATUS_DONE_OK) {

               pDriverData->dDrawSpareSubchannelObject =
                   NV_DD_DMA_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating0UYVYSurfaceMem = psurf_gbl->fpVidMem;
           }
#endif  /* MULTI_MON */
       } else if ((pDriverData->fNvActiveFloatingContexts &
           NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) == 0) {

#ifdef MULTI_MON
           status = NvRmAllocContextDma(pDriverData->dwRootHandle,
                               NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                               NV01_CONTEXT_DMA,
                               (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                               (PVOID)psurf_gbl->fpVidMem,
                               (dwBlockSize - 1));

           if (status == ALLOC_CTX_DMA_STATUS_SUCCESS) {

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating1UYVYSurfaceMem = psurf_gbl->fpVidMem;

               while (freeCount < 4)
                   NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
               freeCount -= 4;

               pDriverData->dDrawSpareSubchannelObject = 0;

               /* Trash spare subchannel */
               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
               dmaPusherPutAddress[1] = NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
               dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                        SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
               dmaPusherPutAddress[3] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               dmaPusherPutAddress += 4;

               pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

               /* Force write combine buffer to flush */
               pDriverData->NvDmaPusherBufferEnd[0] = 0;
               /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
               _outp (0x3d0,0);

               pDriverData->dwDmaPusherFreeCount = freeCount;

               npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
           }

#else   /* MULTI_MON */

           pDmaDmaToMemNotifier->status = NV_IN_PROGRESS;

           /* Trash spare subchannel */
           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0x80000;
           dmaPusherPutAddress[3] = (unsigned long)psurf_gbl->fpVidMem;
           dmaPusherPutAddress[4] = (unsigned long)NvGetFlatDataSelector();
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_NOTIFY_OFFSET | 0x40000;
           dmaPusherPutAddress[6] = 0;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_LIMIT_OFFSET | 0x40000;
           dmaPusherPutAddress[8] = dwBlockSize - 1;

           dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
           dmaPusherPutAddress[12] = NV_DD_DMA_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

           dmaPusherPutAddress += 13;

           pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

           /* Force write combine buffer to flush */
           pDriverData->NvDmaPusherBufferEnd[0] = 0;
           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
           _outp (0x3d0,0);

           pDriverData->dwDmaPusherFreeCount = freeCount;

           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

           while ((volatile)pDmaDmaToMemNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

           if (pDmaDmaToMemNotifier->status == NV_STATUS_DONE_OK) {

               pDriverData->dDrawSpareSubchannelObject =
                   NV_DD_PIO_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating1UYVYSurfaceMem = psurf_gbl->fpVidMem;
           }
#endif  /* MULTI_MON */
       }
   } else if ((psurf_gbl->ddpfSurface.dwFourCC == FOURCC_YUY2) ||
              (psurf_gbl->ddpfSurface.dwFourCC == FOURCC_YUNV)) {
       if ((pDriverData->fNvActiveFloatingContexts &
           NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) == 0) {

#ifdef MULTI_MON
           status = NvRmAllocContextDma(pDriverData->dwRootHandle,
                               NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                               NV01_CONTEXT_DMA,
                               (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                               (PVOID)psurf_gbl->fpVidMem,
                               (dwBlockSize - 1));

           if (status == ALLOC_CTX_DMA_STATUS_SUCCESS) {

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating0YUYVSurfaceMem = psurf_gbl->fpVidMem;

               while (freeCount < 4)
                   NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
               freeCount -= 4;

               pDriverData->dDrawSpareSubchannelObject = 0;

               /* Trash spare subchannel */
               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
               dmaPusherPutAddress[1] = NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
               dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                        SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
               dmaPusherPutAddress[3] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               dmaPusherPutAddress += 4;

               pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

               /* Force write combine buffer to flush */
               pDriverData->NvDmaPusherBufferEnd[0] = 0;
               /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
               _outp (0x3d0,0);

               pDriverData->dwDmaPusherFreeCount = freeCount;

               npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
           }

#else   /* MULTI_MON */

           pDmaDmaToMemNotifier->status = NV_IN_PROGRESS;

           /* Trash spare subchannel */
           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0x80000;
           dmaPusherPutAddress[3] = (unsigned long)psurf_gbl->fpVidMem;
           dmaPusherPutAddress[4] = (unsigned long)NvGetFlatDataSelector();
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_NOTIFY_OFFSET | 0x40000;
           dmaPusherPutAddress[6] = 0;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_LIMIT_OFFSET | 0x40000;
           dmaPusherPutAddress[8] = dwBlockSize - 1;

           dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
           dmaPusherPutAddress[12] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

           dmaPusherPutAddress += 13;

           pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

           /* Force write combine buffer to flush */
           pDriverData->NvDmaPusherBufferEnd[0] = 0;
           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
           _outp (0x3d0,0);

           pDriverData->dwDmaPusherFreeCount = freeCount;

           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

           while ((volatile)pDmaDmaToMemNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

           if (pDmaDmaToMemNotifier->status == NV_STATUS_DONE_OK) {

               pDriverData->dDrawSpareSubchannelObject =
                   NV_DD_PIO_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating0YUYVSurfaceMem = psurf_gbl->fpVidMem;
           }
#endif  /* MULTI_MON */
       } else if ((pDriverData->fNvActiveFloatingContexts &
           NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) == 0) {

#ifdef MULTI_MON
           status = NvRmAllocContextDma(pDriverData->dwRootHandle,
                               NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY,
                               NV01_CONTEXT_DMA,
                               (ALLOC_CTX_DMA_FLAGS_ACCESS_READ_ONLY |
                                ALLOC_CTX_DMA_FLAGS_LOCKED_CACHED),
                               (PVOID)psurf_gbl->fpVidMem,
                               (dwBlockSize - 1));

           if (status == ALLOC_CTX_DMA_STATUS_SUCCESS) {

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating0YUYVSurfaceMem = psurf_gbl->fpVidMem;

               while (freeCount < 4)
                   NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
               freeCount -= 4;

               pDriverData->dDrawSpareSubchannelObject = 0;

               /* Trash spare subchannel */
               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
               dmaPusherPutAddress[1] = NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
               dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                        SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
               dmaPusherPutAddress[3] = NV_DD_DMA_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               dmaPusherPutAddress += 4;

               pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

               /* Force write combine buffer to flush */
               pDriverData->NvDmaPusherBufferEnd[0] = 0;
               /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
               _outp (0x3d0,0);

               pDriverData->dwDmaPusherFreeCount = freeCount;

               npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
           }

#else   /* MULTI_MON */

           pDmaDmaToMemNotifier->status = NV_IN_PROGRESS;

           /* Trash spare subchannel */
           while (freeCount < 13)
               NvGetDmaBufferFreeCount(npDev, freeCount, 13, dmaPusherPutAddress);
           freeCount -= 13;

           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[1] = NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_OFFSET | 0x80000;
           dmaPusherPutAddress[3] = (unsigned long)psurf_gbl->fpVidMem;
           dmaPusherPutAddress[4] = (unsigned long)NvGetFlatDataSelector();
           dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_NOTIFY_OFFSET | 0x40000;
           dmaPusherPutAddress[6] = 0;
           dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    CONTEXT_DMA_FROM_MEM_SET_DMA_SPECIFIER_LIMIT_OFFSET | 0x40000;
           dmaPusherPutAddress[8] = dwBlockSize - 1;

           dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
           dmaPusherPutAddress[10] = NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
           dmaPusherPutAddress[11] = dDrawSubchannelOffset(NV_DD_SPARE) +
                    SCALED_IMAGE_CONTEXT_DMA_IMAGE_OFFSET | 0x40000;
           dmaPusherPutAddress[12] = NV_DD_DMA_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

           dmaPusherPutAddress += 13;

           pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

           /* Force write combine buffer to flush */
           pDriverData->NvDmaPusherBufferEnd[0] = 0;
           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
           _outp (0x3d0,0);

           pDriverData->dwDmaPusherFreeCount = freeCount;

           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

           while ((volatile)pDmaDmaToMemNotifier->status == NV_IN_PROGRESS) NV_SLEEP;

           if (pDmaDmaToMemNotifier->status == NV_STATUS_DONE_OK) {

               pDriverData->dDrawSpareSubchannelObject =
                   NV_DD_PIO_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->fNvActiveFloatingContexts |=
                   NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY;

               pDriverData->NvFloating1YUYVSurfaceMem = psurf_gbl->fpVidMem;
           }
#endif  /* MULTI_MON */
       }
   }

   return DDHAL_DRIVER_HANDLED;
}



/*
 * Video scaler arbitration routines
 */


//
// Calculate the closest arbitration values for a given system configuration
//
// NOTE: Please excuse this code.  In comes from the hardware group...
//
VOID dacCalculateArbitration
(
           fifo_info *fifo,
           sim_state  *arb
)
{
  int data, pagemiss, cas,width, video_enable, color_key_enable, bpp, align;
  int nvclks, mclks, pclks, vpagemiss, crtpagemiss, vbs;
  int found, mclk_extra, mclk_loop, cbs, m1, p1;
  int mclk_freq, pclk_freq, nvclk_freq, mp_enable;
  int us_m, us_n, us_p, video_drain_rate, crtc_drain_rate;
  int vpm_us, us_video, vlwm, video_fill_us, cpm_us, us_crt,clwm;
  int craw, vraw;
  fifo->valid = 1;
  pclk_freq = arb->pclk_khz; // freq in KHz
  mclk_freq = arb->mclk_khz;
  nvclk_freq = arb->nvclk_khz;
  pagemiss = arb->mem_page_miss;
  cas = arb->mem_latency;
  width = arb->memory_width >> 6;
  video_enable = arb->enable_video;
  color_key_enable = arb->gr_during_vid;
  bpp = arb->pix_bpp;
  align = arb->mem_aligned;
  mp_enable = arb->enable_mp;
  clwm = 0;
  vlwm = 0;
  cbs = 128;
  pclks = 2; // lwm detect.

  nvclks = 2; // lwm -> sync.
  nvclks += 2; // fbi bus cycles (1 req + 1 busy)
  nvclks += 1; // fbi reqsync

  mclks = 5; // Fifo
  mclks += 3; // MA0 -> MA2
  mclks += 1; // pad->out
  mclks += cas; // Cas Latency.
  mclks += 1; // pad in
  mclks += 1; // latch data
  mclks += 1; // fifo load
  mclks += 1; // fifo write
  mclk_extra = 3; // Margin of error

  nvclks += 2; // fifo sync
  nvclks += 1; // read data
  nvclks += 1; // fbi_rdata
  nvclks += 1; // crtfifo load

  if(mp_enable)
    mclks+=4; // Mp can get in with a burst of 8.
  // Extra clocks determined by heuristics

  nvclks += 0;
  pclks += 0;
  found = 0;
  while(found != 1) {
    fifo->valid = 1;
    found = 1;
    mclk_loop = mclks+mclk_extra;
    us_m = mclk_loop *1000*1000 / mclk_freq; // Mclk latency in us
    us_n = nvclks*1000*1000 / nvclk_freq;// nvclk latency in us
    us_p = nvclks*1000*1000 / pclk_freq;// nvclk latency in us
    if(video_enable) {
      video_drain_rate = pclk_freq * 2; // MB/s
      crtc_drain_rate = pclk_freq * bpp/8; // MB/s

      vpagemiss = 2; // self generating page miss
      vpagemiss += 1; // One higher priority before

      crtpagemiss = 2; // self generating page miss

      vpm_us = (vpagemiss * pagemiss)*1000*1000/mclk_freq;


      if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
    video_fill_us = cbs*1000*1000 / 16 / nvclk_freq ;
      else
    video_fill_us = cbs*1000*1000 / (8 * width) / mclk_freq;

      us_video = vpm_us + us_m + us_n + us_p + video_fill_us;

      vlwm = us_video * video_drain_rate/(1000*1000);
      vlwm++; // fixed point <= float_point - 1.  Fixes that
      vbs = 128;
      if(vlwm > 146) vbs = 64;

      if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
    video_fill_us = vbs *1000*1000/ 16 / nvclk_freq ;
      else
    video_fill_us = vbs*1000*1000 / (8 * width) / mclk_freq;

      cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
      us_crt =
    us_video  // Wait for video
    +video_fill_us // Wait for video to fill up
    +cpm_us // CRT Page miss
    +us_m + us_n +us_p // other latency
    ;
      clwm = us_crt * crtc_drain_rate/(1000*1000);
      clwm++; // fixed point <= float_point - 1.  Fixes that
    } else {
      crtc_drain_rate = pclk_freq * bpp/8; // bpp * pclk/8

      crtpagemiss = 2; // self generating page miss
      crtpagemiss += 1; // MA0 page miss
      cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
      us_crt =  cpm_us + us_m + us_n + us_p ;
      clwm = us_crt * crtc_drain_rate/(1000*1000);
      clwm++; // fixed point <= float_point - 1.  Fixes that
    }
    /*
      Overfill check:

      */

    m1 = clwm + cbs - 512; /* Amount of overfill */
    p1 = m1 * pclk_freq / mclk_freq; /* pclk cycles to drain */
    p1 = p1 * bpp / 8; // bytes drained.

    if((p1 < m1) && (m1 > 0)) {
    fifo->valid = 0;
    found = 0;
    if(mclk_extra ==0)   found = 1; // Can't adjust anymore!
    mclk_extra--;
    }
    else if(video_enable){
      if((clwm > 511) || (vlwm > 255)) {
    fifo->valid = 0;
    found = 0;
    if(mclk_extra ==0)   found = 1; // Can't adjust anymore!
    mclk_extra--;
      }
    } else {
      if(clwm > 519){ // Have some margin
    fifo->valid = 0;
    found = 0;
    if(mclk_extra ==0)   found = 1; // Can't adjust anymore!
    mclk_extra--;
      }
    }
    craw = clwm;
    vraw = vlwm;
    if(clwm < 384) clwm = 384;
    if(vlwm < 128) vlwm = 128;
    data = (int)(clwm);
    //  printf("CRT LWM: %f bytes, prog: 0x%x, bs: 256\n", clwm, data );
    fifo->graphics_lwm = data;   fifo->graphics_burst_size = 128;
    //    fifo->craw = craw;

    data = (int)((vlwm+15));
    //  printf("VID LWM: %f bytes, prog: 0x%x, bs: %d\n, ", vlwm, data, vbs );
    fifo->video_lwm = data;  fifo->video_burst_size = vbs;
  }
}


#ifndef WINNT // For WINNT, this is a macro which checks the PDEV flag
              // to determine if overlay is supported in this mode.
/*
 * GetVideoScalerBandwidthStatus
 */
BOOL __stdcall GetVideoScalerBandwidthStatus32(unsigned long surfaceFlags)
{
    fifo_info fifo_data;
    sim_state sim_data;
    U032 M, N, O, P, crystal, NVclk, Mclk, Vclk;

    // Get the crystal (F in Hz)
#ifdef  MULTI_MON
    crystal = NvConfigGet(NV_CFG_CRYSTAL_FREQ, pDriverData->dwDeviceIDNum);
    Mclk = NvConfigGet(NV_CFG_DAC_MEMORY_CLOCK, pDriverData->dwDeviceIDNum);
    NVclk = NvConfigGet(NV_CFG_DAC_GRAPHICS_CLOCK, pDriverData->dwDeviceIDNum);
#else   /* MULTI_MON */
    crystal = NvConfigGet(NV_CFG_CRYSTAL_FREQ);
    Mclk = NvConfigGet(NV_CFG_DAC_MEMORY_CLOCK);
    NVclk = NvConfigGet(NV_CFG_DAC_GRAPHICS_CLOCK);
#endif  /* MULTI_MON */

    //
    // Recalculate the current PCLK setting.  We might have changed modes
    // recently.
    //

#ifdef  MULTI_MON
    M = NvConfigGet(NV_CFG_DAC_VPLL_M, pDriverData->dwDeviceIDNum);
    N = NvConfigGet(NV_CFG_DAC_VPLL_N, pDriverData->dwDeviceIDNum);
    O = 1;
    P = NvConfigGet(NV_CFG_DAC_VPLL_P, pDriverData->dwDeviceIDNum);

    Vclk = (N * crystal / (1 << P) / M);  // MHz

#else   /* MULTI_MON */
    M = NvConfigGet(NV_CFG_DAC_VPLL_M);
    N = NvConfigGet(NV_CFG_DAC_VPLL_N);
    O = 1;
    P = NvConfigGet(NV_CFG_DAC_VPLL_P);

    Vclk = (N * crystal / (1 << P) / M);  // MHz

#endif  /* MULTI_MON */

    //
    // Last minute kludge to work around the fact that new NV4 arbitration
    // algorithm NEVER rejects overlay requests.
    //

    if (((pDriverData->bi.biBitCount < 16) && (Vclk > 180000000)) ||
        ((pDriverData->bi.biBitCount == 16) && (Vclk > 130000000)) ||
        ((pDriverData->bi.biBitCount > 16) && (Vclk > 80000000)))
       return (FALSE);

    //
    // Build the sim table using current system settings
    //
    sim_data.pix_bpp        = (char)pDriverData->bi.biBitCount;
    sim_data.enable_video   = 1; // Video (monitor scan out) enabled
    sim_data.enable_mp      = (surfaceFlags & DDSCAPS_VIDEOPORT) ? 1 : 0;
#ifdef  MULTI_MON
    sim_data.memory_width   = NvConfigGet(NV_CFG_DAC_INPUT_WIDTH, pDriverData->dwDeviceIDNum);
#else   /* MULTI_MON */
    sim_data.memory_width   = NvConfigGet(NV_CFG_DAC_INPUT_WIDTH);
#endif  /* MULTI_MON */
    sim_data.mem_latency    = 3;
    sim_data.mem_aligned    = 1;
    sim_data.mem_page_miss  = 10;
    sim_data.gr_during_vid  = 1; // Color key enabled ??? (TONG says there is negligible bandwidth difference here)
    sim_data.pclk_khz       = Vclk / 1000;  // in kHz, not MHz
    sim_data.mclk_khz       = Mclk / 1000;  // in kHz, not MHz
    sim_data.nvclk_khz      = NVclk / 1000; // in kHz, not MHz
    //
    // Get those new numbers
    //
    dacCalculateArbitration(&fifo_data, &sim_data);

    //
    // If valid settings found, then video scaler can be used
    //
    if (fifo_data.valid)
        return(TRUE);
    else
        return(FALSE);
}
#endif // #ifndef WINNT
