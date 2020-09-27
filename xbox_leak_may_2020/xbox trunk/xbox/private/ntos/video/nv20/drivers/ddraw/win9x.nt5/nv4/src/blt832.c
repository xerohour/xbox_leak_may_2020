/*==========================================================================;
 *
 *  Copyright (C) 1995, 1998 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       blt832.c
 *  Content:    Windows95 DirectDraw 32 bit driver
 *
 ***************************************************************************/

#include "windows.h"
#include <conio.h>
#include "nvd3ddrv.h"
#include "nv32.h"
#include "nvddobj.h"
#include "ddrvmem.h"
#include "d3dinc.h"
#include "nvheap.h"

extern DWORD ResetNV();

/*
 * Blt8bpp32
 *
 * 32-bit Blt function for 8bpp surfaces.
 */
DWORD __stdcall Blt8bpp32( LPDDHAL_BLTDATA pbd )
{
    FAST Nv10ControlDma *npDev;
    DWORD               dwFlags;
#ifdef  DX7
    DWORD               dwAFlags;
#endif  // DX7
    DWORD               dwDDFX = 0;
    DWORD               dwDstCaps;
    DWORD               dwSrcCaps;
    DWORD               dwRop;
    NVCOORD             wXYext;
    NVCOORD             wDestXY;
    NVCOORD             wSrcXY;
    BYTE                wStretch = 0;
    BYTE                doMirrorUpDown = 0;
    BYTE                isVideo = 0;
    BYTE                isZBlt = 0;

    DWORD               dwFillColor;
    DWORD               dwColorKey;

    DWORD               dwDstOffset;
    DWORD               dwDstPitch;
    DWORD               dwDstWidth;
    DWORD               dwDstHeight;

    DWORD               dwSrcOffset;
    DWORD               dwSrcPitch;
    DWORD               dwSrcWidth;
    DWORD               dwSrcHeight;

    LPDDRAWI_DDRAWSURFACE_LCL  srcx;
    LPDDRAWI_DDRAWSURFACE_LCL  dstx;
    LPDDRAWI_DDRAWSURFACE_GBL  src;
    LPDDRAWI_DDRAWSURFACE_GBL  dst;

    unsigned long       *dmaPusherPutAddress =
                            (unsigned long *)pDriverData->NvDmaPusherPutAddress;
    unsigned long       *dmaPusherBufferBase =
                            (unsigned long *)pDriverData->NvDmaPusherBufferBase;
    long                freeCount;

    dstx = pbd->lpDDDestSurface;
    dst = dstx->lpGbl;

    /*
     * NOTES:
     *
     * Everything you need is in pdb->bltFX .
     * Look at pdb->dwFlags to determine what kind of blt you are doing,
     * DDBLT_xxxx are the flags.
     *
     * Z BUFFER NOTES:
     *
     * ZBuffer ALWAYS comes in BLTFX.   You don't need to go looking for
     * the attached surface. If DDBLT_ZBUFFER is specified, then either a
     * constant z or z buffer surface has been specified for the source and
     * destination. Just look for the DDBLT_ZBUFFERDESTOVERRIDE,
     * DDBLT_ZBUFFERSRCOVERRIDE, DDBLT_ZBUFFERCONSTANTDESTOVERRIDE, or
     * DDBLT_ZBUFFERCONSTANTSRCOVERRIDE.
     *
     * COLORKEY NOTES:
     *
     * ColorKey ALWAYS comes in BLTFX.   You don't have to look it up in
     * the surface.
     */

    dwFlags   = pbd->dwFlags;
    dwDstCaps = dstx->ddsCaps.dwCaps;

    if (dwFlags & DDBLT_DDFX)
        dwDDFX = pbd->bltFX.dwDDFX;
#ifdef  DX7
    dwAFlags  = pbd->dwAFlags;

    if ((dwFlags & DDBLT_AFLAGS) && (dwAFlags & DDABLT_SRCOVERDEST)) {
        DPF("ALPHA ON 8BPP SURFACE FAILED");
        pbd->ddRVal = DDERR_NOALPHAHW;
        return DDHAL_DRIVER_HANDLED;
    }
#endif  /* DX7 */

    // check for z buffer blit
    // assume that DD runtime has enough sense to make alpha blits mutually exclusive
    if ((dwDstCaps & DDSCAPS_ZBUFFER) && (dstx->dwFlags & DDRAWISURF_HASPIXELFORMAT)) {
        if (dst->ddpfSurface.dwZBufferBitDepth == 32)
            isZBlt = NV042_SET_COLOR_FORMAT_LE_Y32;
        else if (dst->ddpfSurface.dwZBufferBitDepth == 16)
            isZBlt = NV042_SET_COLOR_FORMAT_LE_Y16;
    }

    if (dwFlags & (DDBLT_KEYDEST | DDBLT_KEYDESTOVERRIDE)) {
        DPF("DEST COLOR KEY FAILED");
        pbd->ddRVal = DDERR_NOCOLORKEYHW;
        return DDHAL_DRIVER_HANDLED;
    }

    /*
     * get offset, width, and height for destination
     */
    dwDstOffset = dst->fpVidMem     - pDriverData->BaseAddress;
    dwDstPitch  = dst->lPitch;
    dwDstWidth  = pbd->rDest.right  - pbd->rDest.left;
    dwDstHeight = pbd->rDest.bottom - pbd->rDest.top;
    wXYext.wh16.w = (unsigned short)dwDstWidth;
    wXYext.wh16.h = (unsigned short)dwDstHeight;

    wDestXY.xy16.x = (short)pbd->rDest.left;
    wDestXY.xy16.y = (short)pbd->rDest.top;

    if (dwDstCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE | DDSCAPS_NONLOCALVIDMEM))
        return DDHAL_DRIVER_NOTHANDLED;

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

    if (npDev == NULL) {
        if (!ResetNV()) {
            pbd->ddRVal = DDERR_GENERIC;
            return DDHAL_DRIVER_HANDLED;
        }
        pDriverData->fReset = FALSE;
        npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

        if (npDev == NULL) {
            pbd->ddRVal = DDERR_SURFACELOST;
            return DDHAL_DRIVER_HANDLED;
        }
    }

    dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;

#ifdef  CACHE_FREECOUNT
    freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
    NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
    freeCount = 0;
#endif  /* CACHE_FREECOUNT */

    /*
     * If async, then only work if bltter isn't busy
     * This should probably be a little more specific to each call, but
     * waiting for 32 is pretty close
     */
    if (dwFlags & DDBLT_ASYNC ) {

        NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);

        pDriverData->dwDmaPusherFreeCount = freeCount;

        /* Must save pointer in case it was changed by above macro */
        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

        if (freeCount < 10) {
            DPF("ASYNC FAILED");
            pbd->ddRVal = DDERR_WASSTILLDRAWING;
            return DDHAL_DRIVER_HANDLED;
        }
    }

    if (dwFlags & DDBLT_ROP) {
        dwRop = pbd->bltFX.dwROP >> 16;

        // set pixel depth for z buffer blit
        if (isZBlt) {
            while (freeCount < 2)
                NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
            freeCount -= 2;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = isZBlt;

            dmaPusherPutAddress += 2;
        }
        if ((dwRop == BLACKNESSINDEX) || (dwRop == WHITENESSINDEX)) {

            while (freeCount < 10)
                NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
            freeCount -= 10;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                SET_ROP_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = dwRop;

            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                SURFACES_2D_DEST_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = dwDstOffset;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                SURFACES_2D_PITCH_OFFSET | 0x40000;
            dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

            dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000;
            dmaPusherPutAddress[7] = 0;
            dmaPusherPutAddress[8] = asmMergeCoords(wDestXY.xy16.y, wDestXY.xy16.x);
            dmaPusherPutAddress[9] = asmMergeCoords(wXYext.wh16.h, wXYext.wh16.w);

            dmaPusherPutAddress += 10;

        } else if ((dwRop == PATCOPYINDEX) || (dwRop == PATINVERTINDEX)) {

            dwFillColor = pbd->bltFX.dwFillColor & pDriverData->physicalColorMask;

            if (dwRop == PATCOPYINDEX)
                dwRop = SRCCOPYINDEX;
            else if (dwRop == PATINVERTINDEX)
                dwRop = SRCINVERTINDEX;

            while (freeCount < 10)
                NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
            freeCount -= 10;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                SET_ROP_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = dwRop;

            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                SURFACES_2D_DEST_OFFSET | 0x40000;
            dmaPusherPutAddress[3] = dwDstOffset;
            dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                SURFACES_2D_PITCH_OFFSET | 0x40000;
            dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

            dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000;
            dmaPusherPutAddress[7] = dwFillColor;
            dmaPusherPutAddress[8] = asmMergeCoords(wDestXY.xy16.y, wDestXY.xy16.x);
            dmaPusherPutAddress[9] = asmMergeCoords(wXYext.wh16.h, wXYext.wh16.w);

            dmaPusherPutAddress += 10;

        } else if ((dwRop == SRCCOPYINDEX) || (dwRop == SRCPAINTINDEX) ||
                   (dwRop == SRCANDINDEX) || (dwRop == NOTSRCCOPYINDEX) ||
                   (dwRop == SRCINVERTINDEX) || (dwRop == MERGEPAINTINDEX) ||
                   (dwRop == SRCERASEINDEX) || (dwRop == NOTSRCERASEINDEX)) {

            srcx = pbd->lpDDSrcSurface;
            src = srcx->lpGbl;

            dwSrcCaps = srcx->ddsCaps.dwCaps;

            /* We must treat AGP source memory as system memory */
            if (dwSrcCaps & DDSCAPS_NONLOCALVIDMEM) {
                dwSrcCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
            }

            dwSrcOffset = src->fpVidMem    - pDriverData->BaseAddress;
            dwSrcPitch  = src->lPitch;
            dwSrcWidth  = pbd->rSrc.right  - pbd->rSrc.left;
            dwSrcHeight = pbd->rSrc.bottom - pbd->rSrc.top;
            dwColorKey  = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;

            wSrcXY.xy16.x = (short)pbd->rSrc.left;
            wSrcXY.xy16.y = (short)pbd->rSrc.top;

            /* We don't handle FOURCC_RGB0 format in 8bpp mode */
            if ((srcx->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
                (src->ddpfSurface.dwFlags & DDPF_FOURCC)) {

               if (src->ddpfSurface.dwFourCC == FOURCC_RGB0) {
                   if ((src->ddpfSurface.dwRBitMask == 0) &&
                       (src->ddpfSurface.dwGBitMask == 0) &&
                       (src->ddpfSurface.dwBBitMask == 0)) {
                       if (dwSrcCaps & DDSCAPS_SYSTEMMEMORY)
                           isVideo = 1;
                       goto srcRGB;
                   } else {
                       pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                       return DDHAL_DRIVER_HANDLED;
                   }
               }
               pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
               return DDHAL_DRIVER_HANDLED;
            }

srcRGB:     if (dwDDFX & DDBLTFX_MIRRORUPDOWN)
                doMirrorUpDown = 1;

            if ((dwSrcWidth != dwDstWidth) || (dwSrcHeight != dwDstHeight)) {
                wStretch = 1;
            }

            if (dwFlags & DDBLT_KEYSRCOVERRIDE) {

                dwColorKey = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;

                /* Let D3D know that we changed the object in subchannel 6 */
                pDriverData->dDrawSpareSubchannel6Object = NV_DD_CONTEXT_COLOR_KEY;

                while (freeCount < 4)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                freeCount -= 4;

                // load and enable chroma key
                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_TRANSCOLOR) | 0x40000;
                dmaPusherPutAddress[1] = NV_DD_CONTEXT_COLOR_KEY;
                dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_TRANSCOLOR) +
                    SET_TRANSCOLOR_OFFSET | 0x40000;
                dmaPusherPutAddress[3] =
                    (dwColorKey & pDriverData->physicalColorMask) | NV_ALPHA_1_008;

                dmaPusherPutAddress += 4;

                pDriverData->dwDmaPusherFreeCount = freeCount;

                /* Just in case we return unexpectedly */
                pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;
            }

            // screen to screen bitblt
            if (wStretch == 0) {

                if ((dwDstCaps & DDSCAPS_VIDEOMEMORY) &&
                    (dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                    ((dwSrcCaps & DDSCAPS_TEXTURE) == 0) &&
                    (doMirrorUpDown == 0) &&
                    (isVideo == 0)) {

                    if (pDriverData->dwSharedClipChangeCount != pDriverData->dwDDMostRecentClipChangeCount) {

                        while (freeCount < 7)
                            NvGetDmaBufferFreeCount(npDev, freeCount, 7, dmaPusherPutAddress);
                        freeCount -= 7;

                        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
                        dmaPusherPutAddress[1] = NV_DD_IMAGE_BLACK_RECTANGLE;
                        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                            IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000;
                        dmaPusherPutAddress[3] = 0;
                        dmaPusherPutAddress[4] = asmMergeCoords(NV_MAX_X_CLIP, NV_MAX_Y_CLIP);
                        dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
                        dmaPusherPutAddress[6] = NV_DD_SURFACES_2D;

                        dmaPusherPutAddress += 7;

                        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                        /* Force write combine buffer to flush */
                        pDriverData->NvDmaPusherBufferEnd[0] = 0;
                        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                        _outp (0x3d0,0);

                        pDriverData->dwDmaPusherFreeCount = freeCount;

                        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                        pDriverData->dwSharedClipChangeCount++;

                        pDriverData->dwDDMostRecentClipChangeCount = pDriverData->dwSharedClipChangeCount;
                    }

                    while (freeCount < 10)
                        NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
                    freeCount -= 10;

                    dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                        SET_ROP_OFFSET | 0x40000;
                    dmaPusherPutAddress[1] = dwRop;

                    dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                        SURFACES_2D_PITCH_OFFSET | 0xC0000;
                    dmaPusherPutAddress[3] = (dwDstPitch << 16) | dwSrcPitch;
                    dmaPusherPutAddress[4] = dwSrcOffset;
                    dmaPusherPutAddress[5] = dwDstOffset;

                    dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_BLIT) +
                        BLIT_POINT_IN_OFFSET | 0xC0000;
                    dmaPusherPutAddress[7] = wSrcXY.xy;
                    dmaPusherPutAddress[8] = wDestXY.xy;
                    dmaPusherPutAddress[9] = wXYext.wh;

                    dmaPusherPutAddress += 10;

               } else {
                   // memory to screen blt (src may be in system memory or in video memory)
                   unsigned long temp;
                   long srcScanLength;
                   long nxtSrcScan;
                   short n = wXYext.wh16.h;
                   FAST short pxlCount;
                   unsigned short srcOffset;
                   unsigned char *pSrcInitBits;

                   pSrcInitBits = (unsigned char *)src->fpVidMem;

                   srcScanLength = src->lPitch;

                   if (doMirrorUpDown)
                       wSrcXY.xy16.y += (n - 1);

                   pSrcInitBits += (wSrcXY.xy16.y * srcScanLength);

                   srcOffset = wSrcXY.xy16.x & 3;

                   temp = ((wXYext.wh16.w + srcOffset + 3) & 0xFFFC);

                   nxtSrcScan = srcScanLength - temp;

                   if (doMirrorUpDown)
                       nxtSrcScan = -srcScanLength - temp;

                   pSrcInitBits += (wSrcXY.xy16.x & 0xFFFC);

                   while (freeCount < 15)
                       NvGetDmaBufferFreeCount(npDev, freeCount, 15, dmaPusherPutAddress);
                   freeCount -= 15;

                   if (pDriverData->dDrawSpareSubchannelObject !=
                       NV_DD_IMAGE_BLACK_RECTANGLE) {

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                       dmaPusherPutAddress[1] = NV_DD_IMAGE_BLACK_RECTANGLE;

                       dmaPusherPutAddress += 2;

                       pDriverData->dDrawSpareSubchannelObject =
                       NV_DD_IMAGE_BLACK_RECTANGLE;
                   }

                   /* Let others know that we changed the clip */
                   pDriverData->dwSharedClipChangeCount++;

                   // changing canvas clip avoids manually aligning 8-bit source
                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) +
                       IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000;
                   dmaPusherPutAddress[1] = wDestXY.xy;
                   dmaPusherPutAddress[2] = wXYext.wh;

                   dmaPusherPutAddress[3] = dDrawSubchannelOffset(NV_DD_ROP) +
                       SET_ROP_OFFSET | 0x40000;
                   dmaPusherPutAddress[4] = dwRop;

                   dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                       SURFACES_2D_DEST_OFFSET | 0x40000;
                   dmaPusherPutAddress[6] = dwDstOffset;
                   dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                       SURFACES_2D_PITCH_OFFSET | 0x40000;
                   dmaPusherPutAddress[8] = (dwDstPitch << 16) | dwDstPitch;

                   /* compiler screws up this expression when it goes negative in following macro */
                   temp = wDestXY.xy16.x - srcOffset;

                   dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                       IMAGE_FROM_CPU_POINT_OFFSET | 0xC0000;
                   dmaPusherPutAddress[10] =
                       asmMergeCoords(temp, wDestXY.xy16.y);
                   dmaPusherPutAddress[11] =
                       asmMergeCoords(wXYext.wh16.w + srcOffset, wXYext.wh16.h);

                   temp = ((wXYext.wh16.w + srcOffset + 3) & 0xFFFC);

                   dmaPusherPutAddress[12] =
                       asmMergeCoords(temp, wXYext.wh16.h);

                   dmaPusherPutAddress += 13;

                   {
                       FAST unsigned char *pSrcBits = pSrcInitBits;
                       FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

                       if (temp < (1792 << 2)) {
                           while (--n >= 0) {
                               pxlCount = (short)temp;

                               // Get enough buffer space for this scanline
                               while (freeCount < (long)(1 + (temp >> 2)))
                                   NvGetDmaBufferFreeCount(npDev, freeCount, (long)(1 + (temp >> 2)), dmaPusherPutAddress);
                               freeCount -= (long)(1 + (temp >> 2));

                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                   IMAGE_FROM_CPU_COLOR_OFFSET | (temp << 16);
                               dmaPusherPutAddress += 1;

                               while (pxlCount >= 32) {
                                   pxlCount -= 32;

                                   tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                   tmpPxls1 = *(unsigned long *)&pSrcBits[4];
                                   dmaPusherPutAddress[0] = tmpPxls0;
                                   tmpPxls2 = *(unsigned long *)&pSrcBits[8];
                                   dmaPusherPutAddress[1] = tmpPxls1;
                                   tmpPxls0 = *(unsigned long *)&pSrcBits[12];
                                   tmpPxls1 = *(unsigned long *)&pSrcBits[16];
                                   dmaPusherPutAddress[2] = tmpPxls2;
                                   dmaPusherPutAddress[3] = tmpPxls0;
                                   tmpPxls2 = *(unsigned long *)&pSrcBits[20];
                                   tmpPxls0 = *(unsigned long *)&pSrcBits[24];
                                   dmaPusherPutAddress[4] = tmpPxls1;
                                   dmaPusherPutAddress[5] = tmpPxls2;
                                   tmpPxls1 = *(unsigned long *)&pSrcBits[28];
                                   dmaPusherPutAddress[6] = tmpPxls0;
                                   pSrcBits += 32;
                                   dmaPusherPutAddress[7] = tmpPxls1;

                                   dmaPusherPutAddress += 8;
                               }

                               if (pxlCount > 0) {

                                   while (pxlCount >= 8) {
                                       pxlCount -= 8;
                                       tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                       tmpPxls1 = *(unsigned long *)&pSrcBits[4];
                                       pSrcBits += 8;
                                       dmaPusherPutAddress[0] = tmpPxls0;
                                       dmaPusherPutAddress[1] = tmpPxls1;

                                       dmaPusherPutAddress += 2;
                                   }

                                   if (pxlCount > 0) {
                                       pxlCount -= 4;
                                       tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                       pSrcBits += 4;
                                       dmaPusherPutAddress[0] = tmpPxls0;

                                       dmaPusherPutAddress++;

                                       if (pxlCount > 0) {
                                           tmpPxls1 = *(unsigned long *)&pSrcBits[0];
                                           pSrcBits += 4;
                                           dmaPusherPutAddress[0] = tmpPxls1;

                                           dmaPusherPutAddress++;
                                       }
                                   }
                               }
                               pSrcBits += nxtSrcScan;

                               /* Force write combine buffer to flush */
                               pDriverData->NvDmaPusherBufferEnd[0] = 0;
                               /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                               _outp (0x3d0,0);

                               pDriverData->dwDmaPusherFreeCount = freeCount;

                               npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
                            }
                        } else { /* more than 1792 dwords per scanline */

                           while (--n >= 0) {
                               pxlCount = (short)temp;

                               while (pxlCount >= 32) {
                                   pxlCount -= 32;

                                   while (freeCount < (1 + (32 >> 2)))
                                       NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (32 >> 2)), dmaPusherPutAddress);
                                   freeCount -= (1 + (32 >> 2));

                                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                       IMAGE_FROM_CPU_COLOR_OFFSET | (32 << 16);

                                   tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                   tmpPxls1 = *(unsigned long *)&pSrcBits[4];
                                   dmaPusherPutAddress[1] = tmpPxls0;
                                   tmpPxls2 = *(unsigned long *)&pSrcBits[8];
                                   dmaPusherPutAddress[2] = tmpPxls1;
                                   tmpPxls0 = *(unsigned long *)&pSrcBits[12];
                                   tmpPxls1 = *(unsigned long *)&pSrcBits[16];
                                   dmaPusherPutAddress[3] = tmpPxls2;
                                   dmaPusherPutAddress[4] = tmpPxls0;
                                   tmpPxls2 = *(unsigned long *)&pSrcBits[20];
                                   tmpPxls0 = *(unsigned long *)&pSrcBits[24];
                                   dmaPusherPutAddress[5] = tmpPxls1;
                                   dmaPusherPutAddress[6] = tmpPxls2;
                                   tmpPxls1 = *(unsigned long *)&pSrcBits[28];
                                   dmaPusherPutAddress[7] = tmpPxls0;
                                   pSrcBits += 32;
                                   dmaPusherPutAddress[8] = tmpPxls1;

                                   dmaPusherPutAddress += 9;
                               }

                               if (pxlCount > 0) {

                                   while (freeCount < (1 + (pxlCount >> 2)))
                                       NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (pxlCount >> 2)), dmaPusherPutAddress);
                                   freeCount -= (1 + (pxlCount >> 2));

                                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                       IMAGE_FROM_CPU_COLOR_OFFSET | (pxlCount << 16);
                                   dmaPusherPutAddress += 1;

                                   while (pxlCount >= 8) {
                                       pxlCount -= 8;
                                       tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                       tmpPxls1 = *(unsigned long *)&pSrcBits[4];
                                       pSrcBits += 8;
                                       dmaPusherPutAddress[0] = tmpPxls0;
                                       dmaPusherPutAddress[1] = tmpPxls1;

                                       dmaPusherPutAddress += 2;
                                   }

                                   if (pxlCount > 0) {
                                       pxlCount -= 4;
                                       tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                       pSrcBits += 4;
                                       dmaPusherPutAddress[0] = tmpPxls0;

                                       dmaPusherPutAddress++;

                                       if (pxlCount > 0) {
                                           tmpPxls1 = *(unsigned long *)&pSrcBits[0];
                                           pSrcBits += 4;
                                           dmaPusherPutAddress[0] = tmpPxls1;

                                           dmaPusherPutAddress++;
                                       }
                                   }
                               }
                               pSrcBits += nxtSrcScan;

                               /* Force write combine buffer to flush */
                               pDriverData->NvDmaPusherBufferEnd[0] = 0;
                               /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                               _outp (0x3d0,0);

                               pDriverData->dwDmaPusherFreeCount = freeCount;

                               npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
                            }
                        }
                    }

                    pDriverData->dwSharedClipChangeCount++;

                    pDriverData->dwDDMostRecentClipChangeCount = pDriverData->dwSharedClipChangeCount;

                    while (freeCount < 3)
                        NvGetDmaBufferFreeCount(npDev, freeCount, 3, dmaPusherPutAddress);
                    freeCount -= 3;

                    // restore canvas clip
                    dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SPARE) +
                        IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000;
                    dmaPusherPutAddress[1] = 0;
                    dmaPusherPutAddress[2] = asmMergeCoords(NV_MAX_X_CLIP, NV_MAX_Y_CLIP);

                    dmaPusherPutAddress += 3;
                }
            } else { // stretch
               NvNotification *pDmaBufferNotifier;
               unsigned char *pScanlineBuffer;
               long srcScanLength;
               long nxtSrcScan;
               long n;
               short m;
               FAST short pxlCount;
               unsigned char *pSrcInitBits;
               unsigned char *pSaveSrcBits;
               unsigned char *pSrcCurScan;
               long deltaX;
               long deltaY;
               long curDstY;
               long lastDstY;
               long skipSrcBytes = 0;
               NVCOORD wSrcExt;
               NVCOORD wDstPoint;
               unsigned long srcStrtX;
               BYTE srcInVideoMemory = 0;
               BYTE bufState = 0;
               short skipV = 0;

               RECT rDummy;
               BOOL isOverlappingVmem = FALSE;
               FLATPTR lpSurfTemp = 0;
               DWORD dwSrcBitsPP = 8;
               // if the source and destination overlap, then we must do this in 2 passes, first copy the source
               // image somewhere else, and then so the stretch from there.
               if ((dwSrcCaps & DDSCAPS_VIDEOMEMORY) && dwSrcOffset == dwDstOffset && IntersectRect(&rDummy, (LPRECT)(&pbd->rSrc), (LPRECT)(&pbd->rDest))) {
                    NVHEAP_ALLOC(lpSurfTemp, dwSrcHeight * ((dwSrcWidth * dwSrcBitsPP / 8 + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad), TYPE_IMAGE);
                    if (lpSurfTemp) {
                        isOverlappingVmem = TRUE;
                        if (pDriverData->dwSharedClipChangeCount != pDriverData->dwDDMostRecentClipChangeCount) {

                            while (freeCount < 7)
                                NvGetDmaBufferFreeCount(npDev, freeCount, 7, dmaPusherPutAddress);
                            freeCount -= 7;

                            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
                            dmaPusherPutAddress[1] = NV_DD_IMAGE_BLACK_RECTANGLE;
                            dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                                IMAGE_BLACK_RECTANGLE_POINT_OFFSET | 0x80000;
                            dmaPusherPutAddress[3] = 0;
                            dmaPusherPutAddress[4] = asmMergeCoords(NV_MAX_X_CLIP, NV_MAX_Y_CLIP);
                            dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SURFACES) | 0x40000;
                            dmaPusherPutAddress[6] = NV_DD_SURFACES_2D;

                            dmaPusherPutAddress += 7;

                            pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                            /* Force write combine buffer to flush */
                            pDriverData->NvDmaPusherBufferEnd[0] = 0;
                            /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                            _outp (0x3d0,0);

                            pDriverData->dwDmaPusherFreeCount = freeCount;

                            npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                            pDriverData->dwSharedClipChangeCount++;

                            pDriverData->dwDDMostRecentClipChangeCount = pDriverData->dwSharedClipChangeCount;
                        }

                        while (freeCount < 10)
                            NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
                        freeCount -= 10;

                        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                            SET_ROP_OFFSET | 0x40000;
                        dmaPusherPutAddress[1] = dwRop;

                        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                            SURFACES_2D_PITCH_OFFSET | 0xC0000;
                        dmaPusherPutAddress[3] = ((((dwSrcWidth * dwSrcBitsPP / 8 + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad)) << 16) | dwSrcPitch;
                        dmaPusherPutAddress[4] = dwSrcOffset;
                        dmaPusherPutAddress[5] = lpSurfTemp - pDriverData->BaseAddress;

                        dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_BLIT) +
                            BLIT_POINT_IN_OFFSET | 0xC0000;
                        dmaPusherPutAddress[7] = wSrcXY.xy;
                        dmaPusherPutAddress[8] = 0;
                        dmaPusherPutAddress[9] = wXYext.wh;

                        dmaPusherPutAddress += 10;

                        dwSrcOffset = lpSurfTemp - pDriverData->BaseAddress;
                        dwSrcPitch = ((dwSrcWidth * dwSrcBitsPP / 8 + pDriverData->dwSurfaceAlignPad) & ~pDriverData->dwSurfaceAlignPad);
                        wSrcXY.xy = 0;
                        wSrcXY.xy16.x = 0;
                        wSrcXY.xy16.y = 0;
                    } else {
                        // FIXME: vmem heap alloc failed, must do system mem alloc and copy there
                    }
               }
               // WARNING: carefully consider each use of src->fpVidMem and src->lPitch beyond this point
               //          it could screw up overlapping stretchblits

//               pSrcInitBits = (unsigned char *)src->fpVidMem;
//               srcScanLength = src->lPitch;
               pSrcInitBits = (unsigned char *)(dwSrcOffset + pDriverData->BaseAddress);
               srcScanLength = dwSrcPitch;

               if (dwSrcPitch <= 800)
                   m = 16;
               else
                   m = 8;

               while (freeCount < 6)
                   NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
               freeCount -= 6;

               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                   SET_ROP_OFFSET | 0x40000;
               dmaPusherPutAddress[1] = dwRop;

               dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                   SURFACES_2D_DEST_OFFSET | 0x40000;
               dmaPusherPutAddress[3] = dwDstOffset;
               dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                   SURFACES_2D_PITCH_OFFSET | 0x40000;
               dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

               dmaPusherPutAddress += 6;

               /* calculate deltas in 12.20 format */
               deltaX = dwDstWidth << 20;
               deltaX /= dwSrcWidth;

               if ((deltaX > 0x100000) && (deltaX & 0xFFFFF)) {
                   long xStretch = deltaX & 0xFFF00000;
                   deltaX = (dwDstWidth << 20) + xStretch;
                   deltaX /= dwSrcWidth;
               }

               if ((deltaX < 0x100000) && (deltaX & 0xFFFFF)) {
                   deltaX = (dwDstWidth << 20) + 0x100000;
                   deltaX /= dwSrcWidth;
               }

               deltaY = dwDstHeight << 20;
               deltaY /= dwSrcHeight;

               if ((deltaY > 0x100000) && (deltaY & 0xFFFFF)) {
                   long yStretch = deltaY & 0xFFF00000;
                   deltaY = (dwDstHeight << 20) + yStretch;
                   deltaY /= dwSrcHeight;
               }

               if ((deltaY < 0x100000) && (deltaY & 0xFFFFF)) {
                   deltaY = (dwDstHeight << 20) + 0x100000;
                   deltaY /= dwSrcHeight;
               }

               wSrcExt.wh16.w = (unsigned short)((dwSrcWidth + 3) & ~3L);
               wSrcExt.wh16.h = (unsigned short)dwSrcHeight;

               n = (short)dwSrcHeight;

               if (wSrcXY.xy16.y + n > src->wHeight)
                   n = src->wHeight - wSrcXY.xy16.y;

               nxtSrcScan = srcScanLength;

               if (doMirrorUpDown) {
                   wSrcXY.xy16.y += (n - 1);
                   nxtSrcScan = -nxtSrcScan;
                   skipSrcBytes = -skipSrcBytes;
               }

               if ((dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                   ((dwSrcCaps & DDSCAPS_TEXTURE) == 0) &&
                   ((src->fpVidMem >= pDriverData->BaseAddress) || isOverlappingVmem)) {
                   srcInVideoMemory = 1;
               } else {
                   pSaveSrcBits = (unsigned char *)(pSrcInitBits +
                                  (wSrcXY.xy16.y * srcScanLength));
                   pSaveSrcBits += wSrcXY.xy16.x;
                   pSrcCurScan = pSaveSrcBits;
               }

               if (srcInVideoMemory) {
                   pDmaBufferNotifier = (NvNotification *)pDriverData->NvDmaBufferNotifierFlat;
                   pDmaBufferNotifier++;
                   pScanlineBuffer = (unsigned char *)pDriverData->NvScanlineBufferFlat;

                   wSrcExt.wh16.w = (unsigned short)((dwSrcWidth + 3) & ~3);
                   wSrcExt.wh16.h = m;

                   srcStrtX = wSrcXY.xy16.x;

                   while (freeCount < 2)
                       NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                   freeCount -= 2;

                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000;
                   dmaPusherPutAddress[1] = NV_DD_VIDEO_MEMORY_TO_SYSTEM_MEMORY_FORMAT;

                   dmaPusherPutAddress += 2;

                   // wait for any pending notification operation to finish
                   while ((volatile)pDmaBufferNotifier->status == NV_IN_PROGRESS);

                   pDmaBufferNotifier->status = NV_IN_PROGRESS;

                   while (freeCount < 9)
                       NvGetDmaBufferFreeCount(npDev, freeCount, 9, dmaPusherPutAddress);
                   freeCount -= 9;

                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                       MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;

                   dmaPusherPutAddress[1] = dwSrcOffset +
                       ((dwSrcPitch + skipSrcBytes) * (unsigned long)wSrcXY.xy16.y) +
                       ((unsigned long)wSrcXY.xy16.x);

                   dmaPusherPutAddress[2] = NV_DD_NOTIFIER_BLOCK_SIZE + srcStrtX;

                   dmaPusherPutAddress[3] = (dwSrcPitch + skipSrcBytes);

                   dmaPusherPutAddress[4] = dwSrcPitch;

                   dmaPusherPutAddress[5] = (unsigned long)wSrcExt.wh16.w;

                   dmaPusherPutAddress[6] = m;

                   dmaPusherPutAddress[7] = (1 << 8) | 1;

                   dmaPusherPutAddress[8] = 0;

                   dmaPusherPutAddress += 9;

                   /* Force write combine buffer to flush */
                   pDriverData->NvDmaPusherBufferEnd[0] = 0;
                   /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                   _outp (0x3d0,0);

                   pDriverData->dwDmaPusherFreeCount = freeCount;

                   npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                   // wait for Dma operation to finish
                   while ((volatile)pDmaBufferNotifier->status == NV_IN_PROGRESS);

                   if (doMirrorUpDown)
                       wSrcXY.xy16.y -= (m + skipV);
                   else
                       wSrcXY.xy16.y += (m + skipV);
               }

               /* Convert to 12.4 format */
               wDstPoint.xy16.x = (wDestXY.xy16.x << 4);
               wDstPoint.xy16.y = (wDestXY.xy16.y << 4);

               /* convert to 16.16 */
               lastDstY = (long)(wDestXY.xy16.y + wXYext.wh16.h) << 16;

               /* convert to 16.16 */
               curDstY = ((long)wDestXY.xy16.y << 16);

               while (freeCount < 7)
                   NvGetDmaBufferFreeCount(npDev, freeCount, 7, dmaPusherPutAddress);
               freeCount -= 7;

               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                   STRETCHED_IMAGE_SIZE_IN_OFFSET | 0x180000;
               dmaPusherPutAddress[1] = wSrcExt.wh;
               dmaPusherPutAddress[2] = deltaX;
               dmaPusherPutAddress[3] = deltaY;
               dmaPusherPutAddress[4] = wDestXY.xy;
               dmaPusherPutAddress[5] = wXYext.wh;
               dmaPusherPutAddress[6] = wDstPoint.xy;

               dmaPusherPutAddress += 7;

               while ((n > 0) && (curDstY < lastDstY)) {
                   FAST unsigned long *pSrcBits;
                   FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

                   if (srcInVideoMemory) {
                       if (bufState == 0)
                           pSrcBits = (unsigned long *)(pScanlineBuffer + srcStrtX);
                       else
                           pSrcBits = (unsigned long *)(pScanlineBuffer +
                               (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1) + srcStrtX);

                       pSrcCurScan = (unsigned char *)pSrcBits;

                       if (m > 0) {
                           pDmaBufferNotifier->status = NV_IN_PROGRESS;

                           while (freeCount < 9)
                               NvGetDmaBufferFreeCount(npDev, freeCount, 9, dmaPusherPutAddress);
                           freeCount -= 9;

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                               MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;

                           dmaPusherPutAddress[1] = dwSrcOffset +
                               ((dwSrcPitch + skipSrcBytes) * (unsigned long)wSrcXY.xy16.y) +
                               ((unsigned long)wSrcXY.xy16.x);

                           if (bufState == 0)
                               dmaPusherPutAddress[2] =
                                   NV_DD_NOTIFIER_BLOCK_SIZE +
                                   (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1) +
                                   srcStrtX;
                           else
                               dmaPusherPutAddress[2] =
                                   NV_DD_NOTIFIER_BLOCK_SIZE +
                                   srcStrtX;

                           dmaPusherPutAddress[3] = (dwSrcPitch + skipSrcBytes);

                           dmaPusherPutAddress[4] = dwSrcPitch;

                           dmaPusherPutAddress[5] = (unsigned long)wSrcExt.wh16.w;

                           dmaPusherPutAddress[6] = m;

                           dmaPusherPutAddress[7] = (1 << 8) | 1;

                           dmaPusherPutAddress[8] = 0;

                           dmaPusherPutAddress += 9;

                           /* Force write combine buffer to flush */
                           pDriverData->NvDmaPusherBufferEnd[0] = 0;
                           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                           _outp (0x3d0,0);

                           pDriverData->dwDmaPusherFreeCount = freeCount;

                           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                           bufState ^= 1;

                           if (doMirrorUpDown)
                               wSrcXY.xy16.y -= (m + skipV);
                           else
                               wSrcXY.xy16.y += (m + skipV);

                           while (freeCount < 7)
                               NvGetDmaBufferFreeCount(npDev, freeCount, 7, dmaPusherPutAddress);
                           freeCount -= 7;

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                               STRETCHED_IMAGE_SIZE_IN_OFFSET | 0x180000;

                           dmaPusherPutAddress[1] = wSrcExt.wh;

                           dmaPusherPutAddress[2] = deltaX;

                           dmaPusherPutAddress[3] = deltaY;

                           dmaPusherPutAddress[4] = wDestXY.xy;

                           dmaPusherPutAddress[5] = wXYext.wh;

                           dmaPusherPutAddress[6] = wDstPoint.xy;

                           dmaPusherPutAddress += 7;

                           /* Force write combine buffer to flush */
                           pDriverData->NvDmaPusherBufferEnd[0] = 0;
                           /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                           _outp (0x3d0,0);

                           pDriverData->dwDmaPusherFreeCount = freeCount;

                           npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
                       }
                   } else { // source in system memory
                       pSrcBits = (unsigned long *)pSrcCurScan;
                       pSrcCurScan += (nxtSrcScan + skipSrcBytes);
                       n--;
                   }

doNxtSrcScan:      pxlCount = (short)((dwSrcWidth + 3) & ~3);

                   if (pxlCount < (1792 << 2)) {

                       // Get enough room for this scanline
                       while (freeCount < (1 + (pxlCount >> 2)))
                           NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (pxlCount >> 2)), dmaPusherPutAddress);
                       freeCount -= (1 + (pxlCount >> 2));

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           STRETCHED_IMAGE_COLOR_OFFSET | (pxlCount << 16);

                       dmaPusherPutAddress += 1;

                       while (pxlCount >= 64) {

                           pxlCount -= 64;

                           tmpPxls0 = pSrcBits[0];
                           tmpPxls1 = pSrcBits[1];
                           dmaPusherPutAddress[0] = tmpPxls0;
                           tmpPxls2 = pSrcBits[2];
                           dmaPusherPutAddress[1] = tmpPxls1;
                           tmpPxls0 = pSrcBits[3];
                           tmpPxls1 = pSrcBits[4];
                           dmaPusherPutAddress[2] = tmpPxls2;
                           dmaPusherPutAddress[3] = tmpPxls0;
                           tmpPxls2 = pSrcBits[5];
                           tmpPxls0 = pSrcBits[6];
                           dmaPusherPutAddress[4] = tmpPxls1;
                           dmaPusherPutAddress[5] = tmpPxls2;
                           tmpPxls1 = pSrcBits[7];
                           tmpPxls2 = pSrcBits[8];
                           dmaPusherPutAddress[6] = tmpPxls0;
                           dmaPusherPutAddress[7] = tmpPxls1;
                           tmpPxls0 = pSrcBits[9];
                           tmpPxls1 = pSrcBits[10];
                           dmaPusherPutAddress[8] = tmpPxls2;
                           dmaPusherPutAddress[9] = tmpPxls0;
                           tmpPxls2 = pSrcBits[11];
                           tmpPxls0 = pSrcBits[12];
                           dmaPusherPutAddress[10] = tmpPxls1;
                           dmaPusherPutAddress[11] = tmpPxls2;
                           tmpPxls1 = pSrcBits[13];
                           tmpPxls2 = pSrcBits[14];
                           dmaPusherPutAddress[12] = tmpPxls0;
                           dmaPusherPutAddress[13] = tmpPxls1;
                           tmpPxls0 = pSrcBits[15];
                           dmaPusherPutAddress[14] = tmpPxls2;
                           pSrcBits += 16;
                           dmaPusherPutAddress[15] = tmpPxls0;

                           dmaPusherPutAddress += 16;
                       }

                       while (pxlCount >= 32) {

                           pxlCount -= 32;

                           tmpPxls0 = pSrcBits[0];
                           tmpPxls1 = pSrcBits[1];
                           dmaPusherPutAddress[0] = tmpPxls0;
                           tmpPxls2 = pSrcBits[2];
                           dmaPusherPutAddress[1] = tmpPxls1;
                           tmpPxls0 = pSrcBits[3];
                           tmpPxls1 = pSrcBits[4];
                           dmaPusherPutAddress[2] = tmpPxls2;
                           dmaPusherPutAddress[3] = tmpPxls0;
                           tmpPxls2 = pSrcBits[5];
                           tmpPxls0 = pSrcBits[6];
                           dmaPusherPutAddress[4] = tmpPxls1;
                           dmaPusherPutAddress[5] = tmpPxls2;
                           tmpPxls1 = pSrcBits[7];
                           pSrcBits += 8;
                           dmaPusherPutAddress[6] = tmpPxls0;
                           dmaPusherPutAddress[7] = tmpPxls1;

                           dmaPusherPutAddress += 8;
                       }

                       while (pxlCount >= 4) {
                           pxlCount -= 4;
                           tmpPxls0 = pSrcBits[0];
                           pSrcBits += 1;
                           dmaPusherPutAddress[0] = tmpPxls0;

                           dmaPusherPutAddress += 1;
                       }

                   } else { /* more than 1792 dwords per scanline */

                       while (pxlCount >= 64) {

                           pxlCount -= 64;

                           while (freeCount < (1 + (64 >> 2)))
                               NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (64 >> 2)), dmaPusherPutAddress);
                           freeCount -= (1 + (64 >> 2));

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                               STRETCHED_IMAGE_COLOR_OFFSET | (64 << 16);

                           dmaPusherPutAddress += 1;

                           tmpPxls0 = pSrcBits[0];
                           tmpPxls1 = pSrcBits[1];
                           dmaPusherPutAddress[0] = tmpPxls0;
                           tmpPxls2 = pSrcBits[2];
                           dmaPusherPutAddress[1] = tmpPxls1;
                           tmpPxls0 = pSrcBits[3];
                           tmpPxls1 = pSrcBits[4];
                           dmaPusherPutAddress[2] = tmpPxls2;
                           dmaPusherPutAddress[3] = tmpPxls0;
                           tmpPxls2 = pSrcBits[5];
                           tmpPxls0 = pSrcBits[6];
                           dmaPusherPutAddress[4] = tmpPxls1;
                           dmaPusherPutAddress[5] = tmpPxls2;
                           tmpPxls1 = pSrcBits[7];
                           tmpPxls2 = pSrcBits[8];
                           dmaPusherPutAddress[6] = tmpPxls0;
                           dmaPusherPutAddress[7] = tmpPxls1;
                           tmpPxls0 = pSrcBits[9];
                           tmpPxls1 = pSrcBits[10];
                           dmaPusherPutAddress[8] = tmpPxls2;
                           dmaPusherPutAddress[9] = tmpPxls0;
                           tmpPxls2 = pSrcBits[11];
                           tmpPxls0 = pSrcBits[12];
                           dmaPusherPutAddress[10] = tmpPxls1;
                           dmaPusherPutAddress[11] = tmpPxls2;
                           tmpPxls1 = pSrcBits[13];
                           tmpPxls2 = pSrcBits[14];
                           dmaPusherPutAddress[12] = tmpPxls0;
                           dmaPusherPutAddress[13] = tmpPxls1;
                           tmpPxls0 = pSrcBits[15];
                           dmaPusherPutAddress[14] = tmpPxls2;
                           pSrcBits += 16;
                           dmaPusherPutAddress[15] = tmpPxls0;

                           dmaPusherPutAddress += 16;
                       }

                       while (pxlCount >= 32) {

                           pxlCount -= 32;

                           while (freeCount < (1 + (32 >> 2)))
                               NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (32 >> 2)), dmaPusherPutAddress);
                           freeCount -= (1 + (32 >> 2));

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                               STRETCHED_IMAGE_COLOR_OFFSET | (32 << 16);

                           dmaPusherPutAddress += 1;

                           tmpPxls0 = pSrcBits[0];
                           tmpPxls1 = pSrcBits[1];
                           dmaPusherPutAddress[0] = tmpPxls0;
                           tmpPxls2 = pSrcBits[2];
                           dmaPusherPutAddress[1] = tmpPxls1;
                           tmpPxls0 = pSrcBits[3];
                           tmpPxls1 = pSrcBits[4];
                           dmaPusherPutAddress[2] = tmpPxls2;
                           dmaPusherPutAddress[3] = tmpPxls0;
                           tmpPxls2 = pSrcBits[5];
                           tmpPxls0 = pSrcBits[6];
                           dmaPusherPutAddress[4] = tmpPxls1;
                           dmaPusherPutAddress[5] = tmpPxls2;
                           tmpPxls1 = pSrcBits[7];
                           pSrcBits += 8;
                           dmaPusherPutAddress[6] = tmpPxls0;
                           dmaPusherPutAddress[7] = tmpPxls1;

                           dmaPusherPutAddress += 8;
                       }

                       if (pxlCount >= 4) {
                           while (freeCount < (1 + (pxlCount >> 2)))
                               NvGetDmaBufferFreeCount(npDev, freeCount, (1 + (pxlCount >> 2)), dmaPusherPutAddress);
                           freeCount -= (1 + (pxlCount >> 2));

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                               STRETCHED_IMAGE_COLOR_OFFSET | (pxlCount << 16);

                           dmaPusherPutAddress++;
                       }

                       while (pxlCount >= 4) {
                           pxlCount -= 4;
                           tmpPxls0 = pSrcBits[0];
                           pSrcBits += 1;
                           dmaPusherPutAddress[0] = tmpPxls0;

                           dmaPusherPutAddress += 1;
                       }
                   }

                   /* Force write combine buffer to flush */
                   pDriverData->NvDmaPusherBufferEnd[0] = 0;
                   /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                   _outp (0x3d0,0);

                   pDriverData->dwDmaPusherFreeCount = freeCount;

                   npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                   if (srcInVideoMemory) {
                       if (--m > 0) {
                           pSrcCurScan += dwSrcPitch;

                           pSrcBits = (unsigned long *)pSrcCurScan;

                           goto doNxtSrcScan;
                       } else {
                           if (dwSrcPitch <= 800) {
                               m = 16;
                               curDstY += deltaY;
                           } else {
                               m = 8;
                               curDstY += (deltaY >> 1);
                           }
                                               /* convert 16.16 to 12.4 */
                           wDstPoint.xy16.y = (short)(curDstY >> 12);

                           // wait for DMA operation to finish
                           while ((volatile)pDmaBufferNotifier->status == NV_IN_PROGRESS);
                       }
                   }    // stretch
                   if (isOverlappingVmem && lpSurfTemp) {
                       // FIXME: wait for blt to complete
                       NVHEAP_FREE(lpSurfTemp);
                   }
               }

               if (srcInVideoMemory) {

                   while (freeCount < 2)
                       NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                   freeCount -= 2;

                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000;
                   dmaPusherPutAddress[1] = NV_DD_IMAGE_FROM_CPU;

                   dmaPusherPutAddress += 2;
               }
            }

            if (dwFlags & DDBLT_KEYSRCOVERRIDE) {
                // disable chroma key
                while (freeCount < 2)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                freeCount -= 2;

                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_TRANSCOLOR) +
                    SET_TRANSCOLOR_OFFSET | 0x40000;
                dmaPusherPutAddress[1] = 0;

                dmaPusherPutAddress += 2;
            }

        } else {
            DPF("BLT     blt not handled by driver");
            return DDHAL_DRIVER_NOTHANDLED;
        }

        // restore pixel depth for z blts
        if (isZBlt) {
            while (freeCount < 2)
                NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
            freeCount -= 2;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

            dmaPusherPutAddress += 2;
        }

    } else if (dwFlags & DDBLT_COLORFILL) {
        dwFillColor = pbd->bltFX.dwFillColor & pDriverData->physicalColorMask;

        while (freeCount < 10)
            NvGetDmaBufferFreeCount(npDev, freeCount, 10, dmaPusherPutAddress);
        freeCount -= 10;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
            SET_ROP_OFFSET | 0x40000;
        dmaPusherPutAddress[1] = SRCCOPYINDEX;

        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_DEST_OFFSET | 0x40000;
        dmaPusherPutAddress[3] = dwDstOffset;
        dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_PITCH_OFFSET | 0x40000;
        dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

        dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
            RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000;
        dmaPusherPutAddress[7] = dwFillColor;
        dmaPusherPutAddress[8] = asmMergeCoords(wDestXY.xy16.y, wDestXY.xy16.x);
        dmaPusherPutAddress[9] = asmMergeCoords(wXYext.wh16.h, wXYext.wh16.w);

        dmaPusherPutAddress += 10;

    } else if (dwFlags & DDBLT_ZBUFFER) {
        /* Hardware Z buffer fills not currently supported when in 8bpp mode */
        DPF("HARDWARE Z BUFFER BLT FAILED");
        return DDHAL_DRIVER_NOTHANDLED;
    } else {
        DPF("BLT     blt not handled by driver");
        /* We don't handle any other type of BLT operation */
        return DDHAL_DRIVER_NOTHANDLED;
    }

    pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

    /* Force write combine buffer to flush */
    pDriverData->NvDmaPusherBufferEnd[0] = 0;
    /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
    _outp (0x3d0,0);

    pDriverData->dwDmaPusherFreeCount = freeCount;

    npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

    pbd->ddRVal = DD_OK;

    return DDHAL_DRIVER_HANDLED;

} /* Blt8bpp32 */
