/*==========================================================================;
 *
 *  Copyright (C) 1995, 1998 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       blt3232.c
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
#include "nvrmapi.h"
#include "nvrmarch.inc"
#include "nvheap.h"

extern DWORD ResetNV();

/*
 * Blt32bpp32
 *
 * 32-bit Blt function for 32bpp surfaces.
 */
DWORD __stdcall Blt32bpp32( LPDDHAL_BLTDATA pbd )
{
    FAST Nv10ControlDma *npDev;
    DWORD               dwFlags;
#ifdef  DX7
    DWORD               dwAFlags;
#endif  /* DX7 */
    DWORD               dwDDFX = 0;
    DWORD               dwDstCaps;
    DWORD               dwSrcCaps;
    DWORD               dwRop;
    NVCOORD             wXYext;
    NVCOORD             wDestXY;
    NVCOORD             wSrcXY;
    BYTE                wStretch = 0;
    BYTE                doMirrorUpDown = 0;
    BYTE                doBilinearFiltering = 0;
    BYTE                isVideo = 0;
    BYTE                isAlphaBlt = 0;
    BYTE                isZBlt = 0;
    BYTE                isNoAlphaSrc = 0;
    BYTE                isKeySrc = 0;
    BYTE                isClipped = 0;

    DWORD               dwFillColor;
    DWORD               dwMultipliers;
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
     * ColorKey ALWAY comes in BLTFX.   You don't have to look it up in
     * the surface.
     */

    dwFlags   = pbd->dwFlags;
    dwDstCaps = dstx->ddsCaps.dwCaps;

#ifdef  DX7
    /*
     * Check for AlphaBlt call
     */
    dwAFlags  = pbd->dwAFlags;

    if ((dwFlags & DDBLT_AFLAGS) && (dwAFlags & DDABLT_SRCOVERDEST)) {
        isAlphaBlt = 1;
        if (dwAFlags & DDABLT_KEYDEST) {
            DPF("AlphaBlt Dest color key failed");
            pbd->ddRVal = DDERR_NOCOLORKEYHW;
            return DDHAL_DRIVER_HANDLED;
        }
        if (dwAFlags & DDABLT_MIRRORLEFTRIGHT) {
            DPF("AlphaBlt mirror left/right failed");
            pbd->ddRVal = DDERR_NOMIRRORHW;
            return DDHAL_DRIVER_HANDLED;
        }
        if (pbd->IsClipped)
            isClipped = 1;
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

    if (dwFlags & DDBLT_DDFX)
        dwDDFX = pbd->bltFX.dwDDFX;

    if (dwFlags & (DDBLT_KEYDEST | DDBLT_KEYDESTOVERRIDE)) {
        DPF("Dest color key failed");
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

#ifndef NV_TEX2
    /*
     * Check for a 32 Bpp texture blit right away.
     */
    if (dwDstCaps & DDSCAPS_TEXTURE) {

#ifdef WINNT
    /*
     * Some MS apps insist on passing us DX7 AlphaBlts even though we never
     * claim DX7 support. Our texblt code requires a source, so return
     * an error if the src is not present.
     */
        if (!pbd->lpDDSrcSurface) {
            pbd->ddRVal = DDERR_INVALIDPARAMS;
            return DDHAL_DRIVER_HANDLED;
        }
#endif
        /*
         * Call the special D3D Texture blit code to handle this blit.
         */
        nvTextureBlit32Bpp(pbd);

        /*
         * Return successfully.
         */
        pbd->ddRVal = DD_OK;
        return DDHAL_DRIVER_HANDLED;
    }
#endif //!NV_TEX2

    if (dwDstCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_NONLOCALVIDMEM))
        return DDHAL_DRIVER_NOTHANDLED;

    /*
     * Let 3D code know we did some 2D rendering
     */
    pDriverData->TwoDRenderingOccurred = 1;

    npDev = (Nv10ControlDma *)pDriverData->NvDevFlatDma;

    if (npDev == NULL) {
        if (!ResetNV()) {
            pbd->ddRVal = DDERR_GENERIC;
            return(DDHAL_DRIVER_HANDLED);
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

    if (pDriverData->ddClipUpdate) {

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

        pDriverData->ddClipUpdate = FALSE;
    }

    /*
     * If async, then only work if bltter isn't busy
     * This should probably be a little more specific to each call, but
     * waiting for 32 is pretty close
     */
    if( dwFlags & DDBLT_ASYNC ) {

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

    /* Check for AlphaBlt fill operation */
    if ((isAlphaBlt) && (pbd->lpDDSrcSurface == 0)) {

        while (freeCount < 12)
            NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
        freeCount -= 12;

        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;
        dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;

        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_DEST_OFFSET | 0x40000;
        dmaPusherPutAddress[3] = dwDstOffset;
        dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_PITCH_OFFSET | 0x40000;
        dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

        dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
        dmaPusherPutAddress[7] = NV_DD_CONTEXT_BETA4;
#ifdef  DX7
        dwMultipliers = pbd->ddrgbaScaleFactors.alpha << 24;
        dwMultipliers |= pbd->ddrgbaScaleFactors.red << 16;
        dwMultipliers |= pbd->ddrgbaScaleFactors.green << 8;
        dwMultipliers |= pbd->ddrgbaScaleFactors.blue;
#endif  /* DX7 */
        dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
            BETA4_SET_BETA_FACTOR_OFFSET | 0x40000;
        dmaPusherPutAddress[9] = dwMultipliers;

        dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | 0x40000;
        dmaPusherPutAddress[11] = NV_DD_ABLEND_RECT_AND_TEXT;

        dmaPusherPutAddress += 12;

        if (isClipped) {
            DWORD nRects = pbd->dwRectCnt;
            LPRECT pClipRects = pbd->prDestRects;
            NVCOORD clipPoint0;
            NVCOORD clipPoint1;

            wDestXY.xy16.x = (short)pbd->rOrigDest.left;
            wDestXY.xy16.y = (short)pbd->rOrigDest.top;
            wXYext.xy16.x  = (short)pbd->rOrigDest.right;
            wXYext.xy16.y  = (short)pbd->rOrigDest.bottom;

            while (freeCount < 2)
                NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
            freeCount -= 2;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_COLOR1A_OFFSET | 0x40000;
            dmaPusherPutAddress[1] = 0xFFFFFFFF;

            dmaPusherPutAddress += 2;

            while (--nRects >= 0) {

                clipPoint0.xy16.x = (short)pClipRects->left;
                clipPoint0.xy16.y = (short)pClipRects->top;
                clipPoint1.xy16.x = (short)pClipRects->right;
                clipPoint1.xy16.y = (short)pClipRects->bottom;
                pClipRects++;

                while (freeCount < 6)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
                freeCount -= 6;

                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_CLIPPOINT0B_OFFSET | 0x80000;
                dmaPusherPutAddress[1] = clipPoint0.xy;
                dmaPusherPutAddress[2] = clipPoint1.xy;
                dmaPusherPutAddress[3] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                    RECT_AND_TEXT_CLIPPEDRECT_POINT0_OFFSET | 0x80000;
                dmaPusherPutAddress[4] = wDestXY.xy;
                dmaPusherPutAddress[5] = wXYext.xy;

                dmaPusherPutAddress += 6;
            }

        } else {

            /* unclipped AlphaBlt MUST clip to destination */
            if (wDestXY.xy16.x < 0)
                wDestXY.xy16.x = 0;
            if (wDestXY.xy16.y < 0)
                wDestXY.xy16.y = 0;
            if (wDestXY.xy16.x + wXYext.wh16.w > dst->wWidth)
                wXYext.wh16.w = dst->wWidth - wDestXY.xy16.x;
            if (wDestXY.xy16.y + wXYext.wh16.h > dst->wHeight)
                wXYext.wh16.h = dst->wHeight - wDestXY.xy16.y;
            /* handle NULL destination rectangle case */
            if ((wDestXY.xy16.x == 0) && (wXYext.wh16.w == 0))
                wXYext.wh16.w = dst->wWidth;
            if ((wDestXY.xy16.y == 0) && (wXYext.wh16.h == 0))
                wXYext.wh16.h = dst->wHeight;

            while (freeCount < 4)
                NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
            freeCount -= 4;

            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) +
                RECT_AND_TEXT_COLOR1A_OFFSET | 0xC0000;
            dmaPusherPutAddress[1] = 0xFFFFFFFF;
            dmaPusherPutAddress[2] = asmMergeCoords(wDestXY.xy16.y, wDestXY.xy16.x);
            dmaPusherPutAddress[3] = asmMergeCoords(wXYext.wh16.h, wXYext.wh16.w);

            dmaPusherPutAddress += 4;
        }

        while (freeCount < 4)
            NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
        freeCount -= 4;

        /* Restore default objects */
        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP_RECT_AND_TEXT) | 0x40000;
        dmaPusherPutAddress[1] = NV_DD_ROP_GDI_RECT_AND_TEXT;

        /* Restore default destination color format */
        dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
            SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;
        dmaPusherPutAddress[3] = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

        dmaPusherPutAddress += 4;

        pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

        /* Force write combine buffer to flush */
        pDriverData->NvDmaPusherBufferEnd[0] = 0;
        /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
        _outp (0x3d0,0);

        pDriverData->dwDmaPusherFreeCount = freeCount;

        npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

        pbd->ddRVal = DD_OK;

        return DDHAL_DRIVER_HANDLED;
    }

    if ((dwFlags & DDBLT_ROP) || ((isAlphaBlt) && (pbd->lpDDSrcSurface != 0))) {

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

        if (isAlphaBlt)
            dwRop = SRCCOPYINDEX;
        else
            dwRop = pbd->bltFX.dwROP >> 16;

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
            if (dwSrcCaps & DDSCAPS_NONLOCALVIDMEM)
                dwSrcCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);

            dwSrcOffset = src->fpVidMem    - pDriverData->BaseAddress;
            dwSrcPitch  = src->lPitch;
            dwSrcWidth  = pbd->rSrc.right  - pbd->rSrc.left;
            dwSrcHeight = pbd->rSrc.bottom - pbd->rSrc.top;

            dwColorKey  = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;

            wSrcXY.xy16.x = (short)pbd->rSrc.left;
            wSrcXY.xy16.y = (short)pbd->rSrc.top;

            if (srcx->dwFlags & DDRAWISURF_HASPIXELFORMAT) {
                if (src->ddpfSurface.dwFlags & DDPF_ALPHAPREMULT) {
                   if ((src->ddpfSurface.dwRGBAlphaBitMask != 0xFF000000) ||
                       (src->ddpfSurface.dwRBitMask != 0x00FF0000) ||
                       (src->ddpfSurface.dwGBitMask != 0x0000FF00) ||
                       (src->ddpfSurface.dwBBitMask != 0x000000FF)) {
                           pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                           return DDHAL_DRIVER_HANDLED;
                       }
                } else if (src->ddpfSurface.dwFlags & DDPF_FOURCC) {

                   if (src->ddpfSurface.dwFourCC == FOURCC_RGB0) {
                       if ((src->ddpfSurface.dwRBitMask == 0x00FF0000) &&
                           (src->ddpfSurface.dwGBitMask == 0x0000FF00) &&
                           (src->ddpfSurface.dwBBitMask == 0x000000FF)) {
                           if (dwSrcCaps & DDSCAPS_SYSTEMMEMORY)
                               isVideo = 1;
                           goto srcRGB;
                       } else {
                           pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                           return DDHAL_DRIVER_HANDLED;
                       }
                   } else if (src->ddpfSurface.dwFourCC == FOURCC_RAW8) {
                       pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                       return DDHAL_DRIVER_HANDLED;
                   }

                   while (freeCount < 6)
                       NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
                   freeCount -= 6;

                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                       SET_ROP_OFFSET | 0x40000;
                   dmaPusherPutAddress[1] = SRCCOPYINDEX;

                   dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                       SURFACES_2D_DEST_OFFSET | 0x40000;
                   dmaPusherPutAddress[3] = dwDstOffset;
                   dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                       SURFACES_2D_PITCH_OFFSET | 0x40000;
                   dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

                   dmaPusherPutAddress += 6;

                   /* Just in case we return unexpectedly */
                   pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                   switch (src->ddpfSurface.dwFourCC) {
                       case FOURCC_UYVY:
                       case FOURCC_UYNV:
                       case FOURCC_YUY2:
                       case FOURCC_YUNV:
    //                     if (dwSrcWidth & 1) {  NEC MPEG CODEC DOESN'T RESPOND TO THIS ERROR
    //                           pbd->ddRVal = DDERR_INVALIDRECT;
    //                           return DDHAL_DRIVER_HANDLED;
    //                       }

                           if (wSrcXY.xy16.x & 1) {
                               pbd->ddRVal = DDERR_INVALIDRECT;
                               return DDHAL_DRIVER_HANDLED;
                           }

                           if ((src->ddpfSurface.dwFourCC == FOURCC_UYVY) ||
                               (src->ddpfSurface.dwFourCC == FOURCC_UYNV)) {
                               isVideo = 2;
                               // Is this surface DMAable ?
                               if ((pDriverData->fNvActiveFloatingContexts &
                                    NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                   (pDriverData->NvFloating0UYVYSurfaceMem == src->fpVidMem))
                                       isVideo = 4;
                               else if ((pDriverData->fNvActiveFloatingContexts &
                                    NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                   (pDriverData->NvFloating1UYVYSurfaceMem == src->fpVidMem))
                                       isVideo = 5;
                           } else { // YUY2
                               isVideo = 3;
                               // Is this surface DMAable ?
                               if ((pDriverData->fNvActiveFloatingContexts &
                                    NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                   (pDriverData->NvFloating0YUYVSurfaceMem == src->fpVidMem))
                                       isVideo = 6;
                               else if ((pDriverData->fNvActiveFloatingContexts &
                                    NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                   (pDriverData->NvFloating1YUYVSurfaceMem == src->fpVidMem))
                                       isVideo = 7;
                           }
                           goto srcRGB;
                       break;

                       case FOURCC_YV12:
                           // Size has to be always * 4, to use VU subsampling
                           if ((dwSrcWidth & 3) || (dwSrcHeight & 3)) {
                               pbd->ddRVal = DDERR_INVALIDRECT;
                               return DDHAL_DRIVER_HANDLED;
                           }
                           return DDHAL_DRIVER_HANDLED;
                       break;
                   }
                   pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                   return DDHAL_DRIVER_HANDLED;
                }
            }

srcRGB:     if (dwDDFX & DDBLTFX_MIRRORUPDOWN)
                doMirrorUpDown = 1;
#ifdef  DX7
            if ((isAlphaBlt) && (dwAFlags & DDABLT_MIRRORUPDOWN))
                doMirrorUpDown = 1;
#endif  /* DX7 */
            if ((dwSrcWidth != dwDstWidth) || (dwSrcHeight != dwDstHeight)) {
                wStretch = 1;
#ifdef  DX7
                if ((isAlphaBlt) && (dwAFlags & DDABLT_BILINEARFILTER))
                    doBilinearFiltering = 1;
#endif  /* DX7 */
            }

            if (isVideo > 1) { /* YUV video */
                wStretch = 1;
                doBilinearFiltering = 1;
            }

#ifdef  DX7
            if ((dwFlags & DDBLT_KEYSRCOVERRIDE)  || ((isAlphaBlt) && (dwAFlags & DDABLT_KEYSRC))) {
#else   /* DX7 */
            if (dwFlags & DDBLT_KEYSRCOVERRIDE) {
#endif  /* DX7 */
               if (isAlphaBlt) { /* NV4 doesn't support source color key in blended image blit operations */
                   isKeySrc = 1;
                   dwColorKey = srcx->ddckCKSrcBlt.dwColorSpaceLowValue;
               } else
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
                    (dwColorKey & pDriverData->physicalColorMask) | NV_ALPHA_1_032;

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
                    (isNoAlphaSrc == 0) &&
                    (isKeySrc == 0) &&
                    (doMirrorUpDown == 0) &&
                    (isVideo == 0)) {

                    if (isAlphaBlt) {

                        while (freeCount < 9)
                            NvGetDmaBufferFreeCount(npDev, freeCount, 9, dmaPusherPutAddress);
                        freeCount -= 9;

                        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                            SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x100000;
                        dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;
                        dmaPusherPutAddress[2] = (dwDstPitch << 16) | dwSrcPitch;
                        dmaPusherPutAddress[3] = dwSrcOffset;
                        dmaPusherPutAddress[4] = dwDstOffset;

                        dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                        dmaPusherPutAddress[6] = NV_DD_CONTEXT_BETA4;
#ifdef  DX7
                        dwMultipliers = pbd->ddrgbaScaleFactors.alpha << 24;
                        dwMultipliers |= pbd->ddrgbaScaleFactors.red << 16;
                        dwMultipliers |= pbd->ddrgbaScaleFactors.green << 8;
                        dwMultipliers |= pbd->ddrgbaScaleFactors.blue;
#endif  /* DX7 */
                        dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_SPARE) +
                            BETA4_SET_BETA_FACTOR_OFFSET | 0x40000;
                        dmaPusherPutAddress[8] = dwMultipliers;

                        dmaPusherPutAddress += 9;

                        if (isClipped) {
                            DWORD nRects = pbd->dwRectCnt;
                            LPRECT pClipRects = pbd->prDestRects;
                            NVCOORD wOrigDestXY;
                            NVCOORD wOrigSrcXY;

                            wOrigDestXY.xy16.x = (short)pbd->rOrigDest.left;
                            wOrigDestXY.xy16.y = (short)pbd->rOrigDest.top;
                            wOrigSrcXY.xy16.x  = (short)pbd->rOrigSrc.left;
                            wOrigSrcXY.xy16.y  = (short)pbd->rOrigSrc.top;
#ifdef  DX7
                            if (dwAFlags & DDABLT_NOBLEND) {

                                while (freeCount < 2)
                                    NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                                freeCount -= 2;

                                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000;
                                dmaPusherPutAddress[1] = NV_DD_SRCCOPY_IMAGE_BLIT;

                                dmaPusherPutAddress += 2;
                            }
#endif  /* DX7 */
                            /* This will need redone to deal with directional copying */
                            while (--nRects >= 0) {
                                wDestXY.xy16.x = (short)pClipRects->left;
                                wDestXY.xy16.y = (short)pClipRects->top;
                                wXYext.wh16.w = (unsigned short)(pClipRects->right - pClipRects->left);
                                wXYext.wh16.h = (unsigned short)(pClipRects->bottom - pClipRects->top);
                                pClipRects++;

                                wSrcXY.xy16.x = wOrigSrcXY.xy16.x +
                                    wDestXY.xy16.x - wOrigDestXY.xy16.x;
                                wSrcXY.xy16.y = wOrigSrcXY.xy16.y +
                                    wDestXY.xy16.y - wOrigDestXY.xy16.y;

                                while (freeCount < 4)
                                    NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                                freeCount -= 4;

                                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_BLIT) +
                                    BLIT_POINT_IN_OFFSET | 0xC0000;
                                dmaPusherPutAddress[1] = wSrcXY.xy;
                                dmaPusherPutAddress[2] = wDestXY.xy;
                                dmaPusherPutAddress[3] = wXYext.wh;

                                dmaPusherPutAddress += 4;
                            }
#ifdef  DX7
                            if (dwAFlags & DDABLT_NOBLEND) {

                                while (freeCount < 2)
                                    NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                                freeCount -= 2;

                                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000;
                                dmaPusherPutAddress[1] = NV_DD_IMAGE_BLIT;

                                dmaPusherPutAddress += 2;
                            }
#endif  /* DX7 */
                        } else { /* unclipped */
#ifdef  DX7
                            if (dwAFlags & DDABLT_NOBLEND) {

                                while (freeCount < 2)
                                    NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                                freeCount -= 2;

                                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000;
                                dmaPusherPutAddress[1] = NV_DD_SRCCOPY_IMAGE_BLIT;

                                dmaPusherPutAddress += 2;
                            }
#endif  /* DX7 */
                            /* unclipped AlphaBlt MUST clip to destination */
                            if (wDestXY.xy16.x < 0)
                                wDestXY.xy16.x = 0;
                            if (wDestXY.xy16.y < 0)
                                wDestXY.xy16.y = 0;
                            if (wDestXY.xy16.x + wXYext.wh16.w > dst->wWidth)
                                wXYext.wh16.w = dst->wWidth - wDestXY.xy16.x;
                            if (wDestXY.xy16.y + wXYext.wh16.h > dst->wHeight)
                                wXYext.wh16.h = dst->wHeight - wDestXY.xy16.y;
                            /* handle NULL destination rectangle case */
                            if ((wDestXY.xy16.x == 0) && (wXYext.wh16.w == 0))
                                wXYext.wh16.w = dst->wWidth;
                            if ((wDestXY.xy16.y == 0) && (wXYext.wh16.h == 0))
                                wXYext.wh16.h = dst->wHeight;

                            while (freeCount < 4)
                                NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                            freeCount -= 4;

                            dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_BLIT) +
                                BLIT_POINT_IN_OFFSET | 0xC0000;
                            dmaPusherPutAddress[1] = wSrcXY.xy;
                            dmaPusherPutAddress[2] = wDestXY.xy;
                            dmaPusherPutAddress[3] = wXYext.wh;

                            dmaPusherPutAddress += 4;
#ifdef  DX7
                            if (dwAFlags & DDABLT_NOBLEND) {

                                while (freeCount < 2)
                                    NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                                freeCount -= 2;

                                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_BLIT) | 0x40000;
                                dmaPusherPutAddress[1] = NV_DD_IMAGE_BLIT;

                                dmaPusherPutAddress += 2;
                            }
#endif  /* DX7 */
                        }

                        while (freeCount < 2)
                            NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                        freeCount -= 2;

                        /* Restore default destination color format */
                        dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                            SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x80000;
                        dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

                        dmaPusherPutAddress += 2;

                    } else { /* regular non-alpha blit */

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
                    }

                } else {
                    // memory to screen blt (src may be in system memory or in video memory)
                   unsigned long temp;
                   long srcScanLength;
                   long nxtSrcScan;
                   short n = wXYext.wh16.h;
                   FAST short pxlCount;
                   unsigned long *pSrcInitBits;

                   pSrcInitBits = (unsigned long *)src->fpVidMem;
                   srcScanLength = src->lPitch >> 2;

                   if (doMirrorUpDown)
                       wSrcXY.xy16.y += (n - 1);

                   pSrcInitBits += (wSrcXY.xy16.y * srcScanLength);

                   temp = wXYext.wh16.w;

                   nxtSrcScan = srcScanLength - temp;

                   if (doMirrorUpDown)
                       nxtSrcScan = -srcScanLength - temp;

                   pSrcInitBits += wSrcXY.xy16.x;

                   if (isAlphaBlt) {

                       while (freeCount < 12)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
                       freeCount -= 12;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;
                       dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;

                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_DEST_OFFSET | 0x40000;
                       dmaPusherPutAddress[3] = dwDstOffset;
                       dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_PITCH_OFFSET | 0x40000;
                       dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

                       dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                       dmaPusherPutAddress[7] = NV_DD_CONTEXT_BETA4;
#ifdef  DX7
                       dwMultipliers = pbd->ddrgbaScaleFactors.alpha << 24;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.red << 16;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.green << 8;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.blue;
#endif  /* DX7 */
                       dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
                           BETA4_SET_BETA_FACTOR_OFFSET | 0x40000;
                       dmaPusherPutAddress[9] = dwMultipliers;
#ifdef  DX7
                       dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000;

                       if (dwAFlags & DDABLT_NOBLEND)
                           dmaPusherPutAddress[11] =
                               NV_DD_SRCCOPY_IMAGE_FROM_CPU;
                       else
                           dmaPusherPutAddress[11] =
                               NV_DD_ABLEND_IMAGE_FROM_CPU;

                       dmaPusherPutAddress += 12;
#else   /* DX7 */
                       dmaPusherPutAddress += 10;
#endif  /* DX7 */

                       if (isClipped) {
                           DWORD nRects = pbd->dwRectCnt;
                           LPRECT pClipRects = pbd->prDestRects;
                           NVCOORD wOrigDestXY;
                           NVCOORD wOrigSrcXY;

                           wOrigDestXY.xy16.x = (short)pbd->rOrigDest.left;
                           wOrigDestXY.xy16.y = (short)pbd->rOrigDest.top;
                           wOrigSrcXY.xy16.x  = (short)pbd->rOrigSrc.left;
                           wOrigSrcXY.xy16.y  = (short)pbd->rOrigSrc.top;

                           while (--nRects >= 0) {
                               wDestXY.xy16.x = (short)pClipRects->left;
                               wDestXY.xy16.y = (short)pClipRects->top;
                               wXYext.wh16.w = (unsigned short)(pClipRects->right - pClipRects->left);
                               wXYext.wh16.h = (unsigned short)(pClipRects->bottom - pClipRects->top);
                               pClipRects++;

                               wSrcXY.xy16.x = wOrigSrcXY.xy16.x +
                                   wDestXY.xy16.x - wOrigDestXY.xy16.x;
                               wSrcXY.xy16.y = wOrigSrcXY.xy16.y +
                                   wDestXY.xy16.y - wOrigDestXY.xy16.y;

                               pSrcInitBits = (unsigned long *)src->fpVidMem;

                               n = wXYext.wh16.h;

                               if (doMirrorUpDown)
                                   wSrcXY.xy16.y += (n - 1);

                               pSrcInitBits += (wSrcXY.xy16.y * srcScanLength);

                               temp = wXYext.wh16.w;

                               nxtSrcScan = srcScanLength - temp;

                               if (doMirrorUpDown)
                                   nxtSrcScan = -srcScanLength - temp;

                               pSrcInitBits += wSrcXY.xy16.x;

                               while (freeCount < 4)
                                   NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                               freeCount -= 4;

                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                   IMAGE_FROM_CPU_POINT_OFFSET | 0xC0000;
                               dmaPusherPutAddress[1] = wDestXY.xy;
                               dmaPusherPutAddress[2] = wXYext.wh;
                               dmaPusherPutAddress[3] = wXYext.wh;

                               dmaPusherPutAddress += 4;

                               {
                                   FAST unsigned long *pSrcBits = (unsigned long *)pSrcInitBits;
                                   FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

                                   if (temp < 1792) {
                                       while (--n >= 0) {
                                           pxlCount = (short)temp;

                                           // Get enough buffer space for this scanline
                                           while (freeCount < (long)(1 + temp))
                                               NvGetDmaBufferFreeCount(npDev, freeCount, (long)(1 + temp), dmaPusherPutAddress);
                                           freeCount -= (long)(1 + temp);

                                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                               IMAGE_FROM_CPU_COLOR_OFFSET | (temp << 18);

                                           dmaPusherPutAddress += 1;

                                           while (pxlCount >= 16) {
                                               pxlCount -= 16;

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

                                           if (pxlCount > 0) {
                                               while (--pxlCount >= 0) {
                                                   tmpPxls0 = pSrcBits[0];
                                                   pSrcBits += 1;
                                                   dmaPusherPutAddress[0] = tmpPxls0;
                                                   dmaPusherPutAddress++;
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
                                   } else { /* more than 1792 pixels per scanline */
                                       while (--n >= 0) {
                                           pxlCount = (short)temp;

                                           while (pxlCount >= 16) {
                                               pxlCount -= 16;

                                               while (freeCount < (1 + 16))
                                                   NvGetDmaBufferFreeCount(npDev, freeCount, (1 + 16), dmaPusherPutAddress);
                                               freeCount -= (1 + 16);

                                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                                   IMAGE_FROM_CPU_COLOR_OFFSET | (16 << 18);

                                               tmpPxls0 = pSrcBits[0];
                                               tmpPxls1 = pSrcBits[1];
                                               dmaPusherPutAddress[1] = tmpPxls0;
                                               tmpPxls2 = pSrcBits[2];
                                               dmaPusherPutAddress[2] = tmpPxls1;
                                               tmpPxls0 = pSrcBits[3];
                                               tmpPxls1 = pSrcBits[4];
                                               dmaPusherPutAddress[3] = tmpPxls2;
                                               dmaPusherPutAddress[4] = tmpPxls0;
                                               tmpPxls2 = pSrcBits[5];
                                               tmpPxls0 = pSrcBits[6];
                                               dmaPusherPutAddress[5] = tmpPxls1;
                                               dmaPusherPutAddress[6] = tmpPxls2;
                                               tmpPxls1 = pSrcBits[7];
                                               tmpPxls2 = pSrcBits[8];
                                               dmaPusherPutAddress[7] = tmpPxls0;
                                               dmaPusherPutAddress[8] = tmpPxls1;
                                               tmpPxls0 = pSrcBits[9];
                                               tmpPxls1 = pSrcBits[10];
                                               dmaPusherPutAddress[9] = tmpPxls2;
                                               dmaPusherPutAddress[10] = tmpPxls0;
                                               tmpPxls2 = pSrcBits[11];
                                               tmpPxls0 = pSrcBits[12];
                                               dmaPusherPutAddress[11] = tmpPxls1;
                                               dmaPusherPutAddress[12] = tmpPxls2;
                                               tmpPxls1 = pSrcBits[13];
                                               tmpPxls2 = pSrcBits[14];
                                               dmaPusherPutAddress[13] = tmpPxls0;
                                               dmaPusherPutAddress[14] = tmpPxls1;
                                               tmpPxls0 = pSrcBits[15];
                                               dmaPusherPutAddress[15] = tmpPxls2;
                                               pSrcBits += 16;
                                               dmaPusherPutAddress[16] = tmpPxls0;

                                               dmaPusherPutAddress += 17;
                                           }

                                           if (pxlCount > 0) {
                                               while (freeCount < (1 + pxlCount))
                                                   NvGetDmaBufferFreeCount(npDev, freeCount, (1 + pxlCount), dmaPusherPutAddress);
                                               freeCount -= (1 + pxlCount);

                                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                                   IMAGE_FROM_CPU_COLOR_OFFSET | (pxlCount << 18);

                                               dmaPusherPutAddress += 1;

                                               while (--pxlCount >= 0) {
                                                   tmpPxls0 = pSrcBits[0];
                                                   pSrcBits += 1;
                                                   dmaPusherPutAddress[0] = tmpPxls0;
                                                   dmaPusherPutAddress++;
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
                           }
                       } else { // unclipped

                           /* unclipped AlphaBlt MUST clip to destination */
                           if (wDestXY.xy16.x < 0)
                               wDestXY.xy16.x = 0;
                           if (wDestXY.xy16.y < 0)
                               wDestXY.xy16.y = 0;
                           if (wDestXY.xy16.x + wXYext.wh16.w > dst->wWidth)
                               wXYext.wh16.w = dst->wWidth - wDestXY.xy16.x;
                           if (wDestXY.xy16.y + wXYext.wh16.h > dst->wHeight)
                               wXYext.wh16.h = dst->wHeight - wDestXY.xy16.y;
                           /* handle NULL destination rectangle case */
                           if ((wDestXY.xy16.x == 0) && (wXYext.wh16.w == 0))
                               wXYext.wh16.w = dst->wWidth;
                           if ((wDestXY.xy16.y == 0) && (wXYext.wh16.h == 0))
                               wXYext.wh16.h = dst->wHeight;

                           while (freeCount < 4)
                               NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                           freeCount -= 4;

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                               IMAGE_FROM_CPU_POINT_OFFSET | 0xC0000;
                           dmaPusherPutAddress[1] = wDestXY.xy;
                           dmaPusherPutAddress[2] = wXYext.wh;
                           dmaPusherPutAddress[3] = wXYext.wh;

                           dmaPusherPutAddress += 4;

                           {
                               FAST unsigned long *pSrcBits = (unsigned long *)pSrcInitBits;
                               FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

                               if (temp < 1792) {

                                   while (--n >= 0) {
                                       pxlCount = (short)temp;

                                       // Get enough buffer space for this scanline
                                       while (freeCount < (long)(1 + temp))
                                           NvGetDmaBufferFreeCount(npDev, freeCount, (long)(1 + temp), dmaPusherPutAddress);
                                       freeCount -= (long)(1 + temp);

                                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                           IMAGE_FROM_CPU_COLOR_OFFSET | (temp << 18);

                                       dmaPusherPutAddress += 1;

                                       while (pxlCount >= 16) {
                                           pxlCount -= 16;

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

                                       if (pxlCount > 0) {
                                           while (--pxlCount >= 0) {
                                               tmpPxls0 = pSrcBits[0];
                                               pSrcBits += 1;
                                               dmaPusherPutAddress[0] = tmpPxls0;
                                               dmaPusherPutAddress++;
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
                               } else { /* more than 1792 pixels per scanline */

                                   while (--n >= 0) {
                                       pxlCount = (short)temp;

                                       while (pxlCount >= 16) {
                                           pxlCount -= 16;

                                           while (freeCount < (1 + 16))
                                               NvGetDmaBufferFreeCount(npDev, freeCount, (1 + 16), dmaPusherPutAddress);
                                           freeCount -= (1 + 16);

                                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                               IMAGE_FROM_CPU_COLOR_OFFSET | (16 << 18);

                                           tmpPxls0 = pSrcBits[0];
                                           tmpPxls1 = pSrcBits[1];
                                           dmaPusherPutAddress[1] = tmpPxls0;
                                           tmpPxls2 = pSrcBits[2];
                                           dmaPusherPutAddress[2] = tmpPxls1;
                                           tmpPxls0 = pSrcBits[3];
                                           tmpPxls1 = pSrcBits[4];
                                           dmaPusherPutAddress[3] = tmpPxls2;
                                           dmaPusherPutAddress[4] = tmpPxls0;
                                           tmpPxls2 = pSrcBits[5];
                                           tmpPxls0 = pSrcBits[6];
                                           dmaPusherPutAddress[5] = tmpPxls1;
                                           dmaPusherPutAddress[6] = tmpPxls2;
                                           tmpPxls1 = pSrcBits[7];
                                           tmpPxls2 = pSrcBits[8];
                                           dmaPusherPutAddress[7] = tmpPxls0;
                                           dmaPusherPutAddress[8] = tmpPxls1;
                                           tmpPxls0 = pSrcBits[9];
                                           tmpPxls1 = pSrcBits[10];
                                           dmaPusherPutAddress[9] = tmpPxls2;
                                           dmaPusherPutAddress[10] = tmpPxls0;
                                           tmpPxls2 = pSrcBits[11];
                                           tmpPxls0 = pSrcBits[12];
                                           dmaPusherPutAddress[11] = tmpPxls1;
                                           dmaPusherPutAddress[12] = tmpPxls2;
                                           tmpPxls1 = pSrcBits[13];
                                           tmpPxls2 = pSrcBits[14];
                                           dmaPusherPutAddress[13] = tmpPxls0;
                                           dmaPusherPutAddress[14] = tmpPxls1;
                                           tmpPxls0 = pSrcBits[15];
                                           dmaPusherPutAddress[15] = tmpPxls2;
                                           pSrcBits += 16;
                                           dmaPusherPutAddress[16] = tmpPxls0;
                                           dmaPusherPutAddress += 17;
                                       }

                                       if (pxlCount > 0) {
                                           while (freeCount < (1 + pxlCount))
                                               NvGetDmaBufferFreeCount(npDev, freeCount, (1 + pxlCount), dmaPusherPutAddress);
                                           freeCount -= (1 + pxlCount);

                                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                               IMAGE_FROM_CPU_COLOR_OFFSET | (pxlCount << 18);

                                           dmaPusherPutAddress += 1;

                                           while (--pxlCount >= 0) {
                                               tmpPxls0 = pSrcBits[0];
                                               pSrcBits += 1;
                                               dmaPusherPutAddress[0] = tmpPxls0;
                                               dmaPusherPutAddress++;
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
                       }

                       while (freeCount < 4)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 4, dmaPusherPutAddress);
                       freeCount -= 4;

                       /* Restore default destination color format */
                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x80000;
                       dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

                       /* Restore default object */
                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000;
                       dmaPusherPutAddress[3] = NV_DD_IMAGE_FROM_CPU;

                       dmaPusherPutAddress += 4;

                   } else { /* non-alpha blit */

                       FAST unsigned long *pSrcBits = (unsigned long *)pSrcInitBits;
                       FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

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

                       dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                           IMAGE_FROM_CPU_POINT_OFFSET | 0xC0000;
                       dmaPusherPutAddress[7] = wDestXY.xy;
                       dmaPusherPutAddress[8] = wXYext.wh;
                       dmaPusherPutAddress[9] = wXYext.wh;

                       dmaPusherPutAddress += 10;

                       if (temp < 1792) {

                           while (--n >= 0) {
                               pxlCount = (short)temp;

                               // Get enough buffer space for this scanline
                               while (freeCount < (long)(1 + temp))
                                   NvGetDmaBufferFreeCount(npDev, freeCount, (long)(1 + temp), dmaPusherPutAddress);
                               freeCount -= (long)(1 + temp);

                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                   IMAGE_FROM_CPU_COLOR_OFFSET | (temp << 18);

                               dmaPusherPutAddress += 1;

                               while (pxlCount >= 16) {
                                   pxlCount -= 16;

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

                               if (pxlCount > 0) {
                                   while (--pxlCount >= 0) {
                                       tmpPxls0 = pSrcBits[0];
                                       pSrcBits += 1;
                                       dmaPusherPutAddress[0] = tmpPxls0;
                                       dmaPusherPutAddress++;
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
                       } else { /* more than 1792 pixels per scanline */

                           while (--n >= 0) {
                               pxlCount = (short)temp;

                               while (pxlCount >= 16) {
                                   pxlCount -= 16;

                                   while (freeCount < (1 + 16))
                                       NvGetDmaBufferFreeCount(npDev, freeCount, (1 + 16), dmaPusherPutAddress);
                                   freeCount -= (1 + 16);

                                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                       IMAGE_FROM_CPU_COLOR_OFFSET | (16 << 18);

                                   tmpPxls0 = pSrcBits[0];
                                   tmpPxls1 = pSrcBits[1];
                                   dmaPusherPutAddress[1] = tmpPxls0;
                                   tmpPxls2 = pSrcBits[2];
                                   dmaPusherPutAddress[2] = tmpPxls1;
                                   tmpPxls0 = pSrcBits[3];
                                   tmpPxls1 = pSrcBits[4];
                                   dmaPusherPutAddress[3] = tmpPxls2;
                                   dmaPusherPutAddress[4] = tmpPxls0;
                                   tmpPxls2 = pSrcBits[5];
                                   tmpPxls0 = pSrcBits[6];
                                   dmaPusherPutAddress[5] = tmpPxls1;
                                   dmaPusherPutAddress[6] = tmpPxls2;
                                   tmpPxls1 = pSrcBits[7];
                                   tmpPxls2 = pSrcBits[8];
                                   dmaPusherPutAddress[7] = tmpPxls0;
                                   dmaPusherPutAddress[8] = tmpPxls1;
                                   tmpPxls0 = pSrcBits[9];
                                   tmpPxls1 = pSrcBits[10];
                                   dmaPusherPutAddress[9] = tmpPxls2;
                                   dmaPusherPutAddress[10] = tmpPxls0;
                                   tmpPxls2 = pSrcBits[11];
                                   tmpPxls0 = pSrcBits[12];
                                   dmaPusherPutAddress[11] = tmpPxls1;
                                   dmaPusherPutAddress[12] = tmpPxls2;
                                   tmpPxls1 = pSrcBits[13];
                                   tmpPxls2 = pSrcBits[14];
                                   dmaPusherPutAddress[13] = tmpPxls0;
                                   dmaPusherPutAddress[14] = tmpPxls1;
                                   tmpPxls0 = pSrcBits[15];
                                   dmaPusherPutAddress[15] = tmpPxls2;
                                   pSrcBits += 16;
                                   dmaPusherPutAddress[16] = tmpPxls0;
                                   dmaPusherPutAddress += 17;
                               }

                               if (pxlCount > 0) {
                                   while (freeCount < (1 + pxlCount))
                                       NvGetDmaBufferFreeCount(npDev, freeCount, (1 + pxlCount), dmaPusherPutAddress);
                                   freeCount -= (1 + pxlCount);

                                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                       IMAGE_FROM_CPU_COLOR_OFFSET | (pxlCount << 18);

                                   dmaPusherPutAddress += 1;

                                   while (--pxlCount >= 0) {
                                       tmpPxls0 = pSrcBits[0];
                                       pSrcBits += 1;
                                       dmaPusherPutAddress[0] = tmpPxls0;
                                       dmaPusherPutAddress++;
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
               }
            } else { // stretch
                RECT rDummy;
                BOOL isOverlappingVmem = FALSE;
                FLATPTR lpSurfTemp = 0;
                DWORD dwSrcBitsPP = 32;
                BOOL useStretched = FALSE;

                /*
                 *  Fractional positioning bands of the SCALED IMAGE class when downscaling
                 *  vertically by small amounts is just too hard to get correct so we switch to the
                 *  STRETCHED IMAGE class when that occurs.
                 */

                if ((!(dwSrcCaps & DDSCAPS_VIDEOMEMORY)) && (dwSrcHeight > dwDstHeight))
                    useStretched = TRUE;

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

               if ((dwFlags & DDBLT_KEYSRCOVERRIDE) || (isKeySrc) || (useStretched)) { // If color key then MUST use STRETCHED IMAGE instead of SCALED IMAGE
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
                   NVCOORD wSrcExt;
                   NVCOORD wDstPoint;
                   unsigned long srcStrtX;
                   BYTE srcInVideoMemory = 0;
                   BYTE bufState = 0;
                   short skipV = 0;
                   DWORD nRects;
                   LPRECT pClipRects;
                   NVCOORD wOrigDestXY;
                   NVCOORD wOrigSrcXY;
                   unsigned long dwSrcXPos;
                   unsigned long dwSrcYPos;

//                   pSrcInitBits = (unsigned char *)src->fpVidMem;
//                   srcScanLength = src->lPitch;
                   pSrcInitBits = (unsigned char *)(dwSrcOffset + pDriverData->BaseAddress);
                   srcScanLength = dwSrcPitch;

                   if (dwSrcPitch <= 1600)
                       m = 8;
                   else if (dwSrcPitch <= 3200)
                       m = 4;
                   else
                       m = 2;

                   if (isAlphaBlt) {

                       while (freeCount < 12)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
                       freeCount -= 12;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;
                       dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;

                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_DEST_OFFSET | 0x40000;
                       dmaPusherPutAddress[3] = dwDstOffset;
                       dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_PITCH_OFFSET | 0x40000;
                       dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

                       dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                       dmaPusherPutAddress[7] = NV_DD_CONTEXT_BETA4;
#ifdef  DX7
                       dwMultipliers = pbd->ddrgbaScaleFactors.alpha << 24;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.red << 16;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.green << 8;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.blue;
#endif  /* DX7 */
                       dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
                           BETA4_SET_BETA_FACTOR_OFFSET | 0x40000;
                       dmaPusherPutAddress[9] = dwMultipliers;
#ifdef  DX7
                       dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;

                       if (dwAFlags & DDABLT_NOBLEND)
                           dmaPusherPutAddress[11] =
                               NV_DD_SRCCOPY_STRETCHED_IMAGE_FROM_CPU;
                       else
                           dmaPusherPutAddress[11] =
                               NV_DD_ABLEND_STRETCHED_IMAGE_FROM_CPU;

                       dmaPusherPutAddress += 12;
#else   DX7
                       dmaPusherPutAddress += 10;
#endif  /* DX7 */
                   } else { // regular blit

                       while (freeCount < 8)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
                       freeCount -= 8;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_ROP) +
                           SET_ROP_OFFSET | 0x40000;
                       dmaPusherPutAddress[1] = dwRop;

                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_DEST_OFFSET | 0x40000;
                       dmaPusherPutAddress[3] = dwDstOffset;
                       dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_PITCH_OFFSET | 0x40000;
                       dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

                       dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;
                       dmaPusherPutAddress[7] = NV_DD_STRETCHED_IMAGE_FROM_CPU;

                       dmaPusherPutAddress += 8;
                   }

                   if (isClipped) {

                       nRects = pbd->dwRectCnt;
                       pClipRects = pbd->prDestRects;

                       wOrigDestXY.xy16.x = (short)pbd->rOrigDest.left;
                       wOrigDestXY.xy16.y = (short)pbd->rOrigDest.top;
                       wOrigSrcXY.xy16.x  = (short)pbd->rOrigSrc.left;
                       wOrigSrcXY.xy16.y  = (short)pbd->rOrigSrc.top;

                       dwDstWidth = pbd->rOrigDest.right - pbd->rOrigDest.left;
                       dwDstHeight = pbd->rOrigDest.bottom - pbd->rOrigDest.top;
                       dwSrcWidth = pbd->rOrigSrc.right - pbd->rOrigSrc.left;
                       dwSrcHeight = pbd->rOrigSrc.bottom - pbd->rOrigSrc.top;

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
                   } else { // unclipped

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
                   }

nxtStretchedRect:  if (isClipped) {

                       skipV = 0;

                       wDestXY.xy16.x = (short)pClipRects->left;
                       wDestXY.xy16.y = (short)pClipRects->top;
                       wXYext.wh16.w = (unsigned short)(pClipRects->right - pClipRects->left);
                       wXYext.wh16.h = (unsigned short)(pClipRects->bottom - pClipRects->top);
                       pClipRects++;

                       /* calculate starting source X position */
                       dwSrcXPos = (long)(wDestXY.xy16.x - wOrigDestXY.xy16.x) << 20;

                       if (deltaX >= 0x100000)
                           dwSrcXPos /= (deltaX >> 16); /* result is 16:16 */
                       else {
                           dwSrcXPos /= deltaX; /* result is 16:00 */
                           dwSrcXPos <<= 16;
                       }
                                                           /* truncate fraction */
                       wSrcXY.xy16.x = wOrigSrcXY.xy16.x + (short)(dwSrcXPos >> 16);

                       /* calculate starting source Y position */
                       dwSrcYPos = (long)(wDestXY.xy16.y - wOrigDestXY.xy16.y) << 20;

                       if (deltaY >= 0x100000)
                           dwSrcYPos /= (deltaY >> 16); /* result is 16:16 */
                       else {
                           dwSrcYPos /= deltaY; /* result is 16:00 */
                           dwSrcYPos <<= 16;
                       }
                                                           /* truncate fraction */
                       wSrcXY.xy16.y = wOrigSrcXY.xy16.y + (short)(dwSrcYPos >> 16);

//                       pSrcInitBits = (unsigned char *)src->fpVidMem;
                       pSrcInitBits = (unsigned char *)(dwSrcOffset + pDriverData->BaseAddress);

                       /* calculate source X extent */
                       dwSrcWidth = (unsigned long)wXYext.wh16.w << 20;

                       if (deltaX >= 0x100000)
                           dwSrcWidth /= (deltaX >> 16); /* result is 16:16 */
                       else {
                           dwSrcWidth /= deltaX; /* result is 16:00 */
                           dwSrcWidth <<= 16;
                       }

                       /* calculate source Y extent */
                       dwSrcHeight = (unsigned long)wXYext.wh16.h << 20;

                       if (deltaY >= 0x100000)
                           dwSrcHeight /= (deltaY >> 16); /* result is 16:16 */
                       else {
                           dwSrcHeight /= deltaY; /* result is 16:00 */
                           dwSrcHeight <<= 16;
                       }

                       wSrcExt.wh16.w = (unsigned short)(dwSrcXPos + dwSrcWidth) >> 16;
                       wSrcExt.wh16.h = (unsigned short)(dwSrcYPos + dwSrcHeight) >> 16;
                   } else {
                       wSrcExt.wh16.w = (unsigned short)dwSrcWidth;
                       wSrcExt.wh16.h = (unsigned short)dwSrcHeight;
                   }

                   n = (short)wSrcExt.wh16.h;

                   if (wSrcXY.xy16.y + n > src->wHeight)
                       n = src->wHeight - wSrcXY.xy16.y;

                   nxtSrcScan = srcScanLength;

                   if (doMirrorUpDown) {
                       wSrcXY.xy16.y += (n - 1);
                       nxtSrcScan = -nxtSrcScan;
                   }

                   if ((dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                       ((dwSrcCaps & DDSCAPS_TEXTURE) == 0) &&
                       ((src->fpVidMem >= pDriverData->BaseAddress) || isOverlappingVmem)) {
                       srcInVideoMemory = 1;
                   } else {
                       pSaveSrcBits = (unsigned char *)(pSrcInitBits +
                                      (wSrcXY.xy16.y * srcScanLength));
                       pSaveSrcBits += (wSrcXY.xy16.x << 2);
                       pSrcCurScan = pSaveSrcBits;
                   }

                   if (srcInVideoMemory) {
                       pDmaBufferNotifier = (NvNotification *)pDriverData->NvDmaBufferNotifierFlat;
                       pDmaBufferNotifier++;
                       pScanlineBuffer = (unsigned char *)pDriverData->NvScanlineBufferFlat;

                       if (isClipped)
                           wSrcExt.wh16.w = (unsigned short)(dwSrcXPos + dwSrcWidth) >> 16;
                       else
                           wSrcExt.wh16.w = (unsigned short)dwSrcWidth;

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
                           (dwSrcPitch * (unsigned long)wSrcXY.xy16.y) +
                           ((unsigned long)wSrcXY.xy16.x << 2);

                       dmaPusherPutAddress[2] = NV_DD_NOTIFIER_BLOCK_SIZE + (srcStrtX << 2);

                       dmaPusherPutAddress[3] = dwSrcPitch;

                       dmaPusherPutAddress[4] = dwSrcPitch;

                       dmaPusherPutAddress[5] = (unsigned long)(wSrcExt.wh16.w << 2);

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
                               pSrcBits = (unsigned long *)(pScanlineBuffer +
                                   (srcStrtX << 2));
                           else
                               pSrcBits = (unsigned long *)(pScanlineBuffer +
                                   (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1) +
                                   (srcStrtX << 2));

                           pSrcCurScan = (unsigned char *)pSrcBits;

                           if (m > 0) {
                               pDmaBufferNotifier->status = NV_IN_PROGRESS;

                               while (freeCount < 9)
                                   NvGetDmaBufferFreeCount(npDev, freeCount, 9, dmaPusherPutAddress);
                               freeCount -= 9;

                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) +
                                   MEM_TO_MEM_FORMAT_OFFSET_IN_OFFSET | 0x200000;

                               dmaPusherPutAddress[1] = dwSrcOffset +
                                   (dwSrcPitch * (unsigned long)wSrcXY.xy16.y) +
                                   ((unsigned long)wSrcXY.xy16.x << 2);

                               if (bufState == 0)
                                   dmaPusherPutAddress[2] =
                                       NV_DD_NOTIFIER_BLOCK_SIZE +
                                       (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1) +
                                       (srcStrtX << 2);
                               else
                                   dmaPusherPutAddress[2] =
                                       NV_DD_NOTIFIER_BLOCK_SIZE +
                                       (srcStrtX << 2);

                               dmaPusherPutAddress[3] = dwSrcPitch;

                               dmaPusherPutAddress[4] = dwSrcPitch;

                               dmaPusherPutAddress[5] = (unsigned long)(wSrcExt.wh16.w << 2);

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
                           pSrcCurScan += nxtSrcScan;
                           n--;
                       }

doNxtBltSrcScan:       if (isClipped) {
                           pxlCount = (short)(dwSrcXPos + dwSrcWidth) >> 16; /* Include fractional starting position */

                           pxlCount -= (short)(dwSrcXPos >> 16);
                       } else {
                           pxlCount = (short)dwSrcWidth;
                       }

                       if (pxlCount < 1792) {

                           // Get enough room for this scanline
                           while (freeCount < (1 + pxlCount))
                               NvGetDmaBufferFreeCount(npDev, freeCount, (1 + pxlCount), dmaPusherPutAddress);
                           freeCount -= (1 + pxlCount);

                           dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                               STRETCHED_IMAGE_COLOR_OFFSET | (pxlCount << 18);

                           dmaPusherPutAddress += 1;

                           while (pxlCount >= 16) {

                               pxlCount -= 16;

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

                           while (pxlCount >= 8) {

                               pxlCount -= 8;

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

                           while (--pxlCount >= 0) {
                               tmpPxls0 = pSrcBits[0];
                               pSrcBits += 1;
                               dmaPusherPutAddress[0] = tmpPxls0;

                               dmaPusherPutAddress += 1;
                           }

                       } else { /* more than 1792 pixels per scanline */
                           while (pxlCount >= 16) {

                               pxlCount -= 16;

                               while (freeCount < (1 + 16))
                                   NvGetDmaBufferFreeCount(npDev, freeCount, (1 + 16), dmaPusherPutAddress);
                               freeCount -= (1 + 16);

                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                                   STRETCHED_IMAGE_COLOR_OFFSET | (16 << 18);

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

                           while (pxlCount >= 8) {

                               pxlCount -= 8;

                               while (freeCount < (1 + 8))
                                   NvGetDmaBufferFreeCount(npDev, freeCount, (1 + 8), dmaPusherPutAddress);
                               freeCount -= (1 + 8);

                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                                   STRETCHED_IMAGE_COLOR_OFFSET | (8 << 18);

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

                           if (pxlCount > 0) {
                               while (freeCount < (1 + pxlCount))
                                   NvGetDmaBufferFreeCount(npDev, freeCount, (1 + pxlCount), dmaPusherPutAddress);
                               freeCount -= (1 + pxlCount);

                               dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                                   STRETCHED_IMAGE_COLOR_OFFSET | (pxlCount << 18);

                               dmaPusherPutAddress++;
                           }

                           while (--pxlCount >= 0) {
                               tmpPxls0 = pSrcBits[0];
                               pSrcBits += 1;
                               dmaPusherPutAddress[0] = tmpPxls0;

                               dmaPusherPutAddress++;
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

                               goto doNxtBltSrcScan;
                           } else {
                               if (dwSrcPitch <= 1600) {
                                   m = 8;
                                   curDstY += (deltaY >> 1);
                               } else if (dwSrcPitch <= 3200) {
                                   m = 4;
                                   curDstY += (deltaY >> 2);
                               } else {
                                   m = 2;
                                   curDstY += (deltaY >> 3);
                               }
                                                   /* convert 16.16 to 12.4 */
                               wDstPoint.xy16.y = (short)(curDstY >> 12);

                               // wait for DMA operation to finish
                               while ((volatile)pDmaBufferNotifier->status == NV_IN_PROGRESS);
                           }
                       }
                   }

                   if (isClipped) {
                       if (--nRects > 0)
                           goto nxtStretchedRect;
                   }

                   if (srcInVideoMemory) {
                       while (freeCount < 2)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                       freeCount -= 2;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_IMAGE) | 0x40000;
                       dmaPusherPutAddress[1] = NV_DD_IMAGE_FROM_CPU;

                       dmaPusherPutAddress += 2;
                   }

                   /* restore default object */
                   while (freeCount < 2)
                       NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                   freeCount -= 2;

                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;
                   dmaPusherPutAddress[1] = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;

                   dmaPusherPutAddress += 2;

                   if (isAlphaBlt) {
                       /* Restore default destination color format */
                       while (freeCount < 2)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                       freeCount -= 2;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;

                       dmaPusherPutAddress[1] =
                           NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

                       dmaPusherPutAddress += 2;
                   }

               } else { // OK to use SCALED IMAGE
                   NvNotification *pDmaDmaFromMemNotifier;
                   unsigned char *pScanlineBuffer;
                   unsigned char *npPixelBuf;
                   long srcScanLength;
                   long nxtSrcScan;
                   unsigned long n;
                   unsigned char *pSrcInitBits;
                   unsigned char *pSaveSrcInitBits;
                   unsigned long *pSaveSrcBits;
                   unsigned long *pSrcCurScan;
                   long srcDeltaX;
                   long srcDeltaY;
                   long skipDeltaY = 0;
                   unsigned long dstDeltaScans;
                   unsigned long dstDeltaX;
                   long dstDeltaY;
                   long srcInitFract;
                   long srcStartFract;
                   long srcStartY;
                   long srcCurFract;
                   long srcCurY;
                   long srcDeltaYPerBand;
                   long srcDeltaScans;
                   long srcCurDeltaScans;
                   long xStretch;
                   long yStretch;
                   long srcBufferPitch;
                   long skipSrcBytes = 0;
                   long dstStartY;
                   long dstCurY;
                   long curV;
                   long skipV = 0;
                   long m;
                   NVCOORD wSrcPoint;
                   NVCOORD wSrcExt;
                   NVCOORD wDstPoint;
                   NVCOORD wDstExt;
                   BYTE srcInVideoMemory = 0;
                   BYTE srcTooWide = 0;
                   short curBuffer;
                   DWORD nRects = 0;
                   LPRECT pClipRects;
                   NVCOORD wOrigDestXY;
                   NVCOORD wOrigSrcXY;
                   long origSrcDeltaX;
                   long origSrcDeltaY;
                   long srcClippedInitFract;
//// These are for working around NV4 scaled image bug
                   DWORD scaledImageObjectID = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
                   DWORD curClipPoint;
                   DWORD curClipExtent;
                   DWORD curDUDX;
                   DWORD curDVDY;
//// End of NV4 workaround variables

//                   pSrcInitBits = (unsigned char *)src->fpVidMem;
                   pSrcInitBits = (unsigned char *)(dwSrcOffset + pDriverData->BaseAddress);

                   if (isVideo < 2)
//                       srcScanLength = src->lPitch;
                       srcScanLength = dwSrcPitch;
                   else
//                       srcScanLength = src->lPitch >> 1;
                       srcScanLength = dwSrcPitch >> 1;

                   if (isAlphaBlt) {

                       while (freeCount < 12)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 12, dmaPusherPutAddress);
                       freeCount -= 12;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;
                       dmaPusherPutAddress[1] = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;

                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_DEST_OFFSET | 0x40000;
                       dmaPusherPutAddress[3] = dwDstOffset;
                       dmaPusherPutAddress[4] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                           SURFACES_2D_PITCH_OFFSET | 0x40000;
                       dmaPusherPutAddress[5] = (dwDstPitch << 16) | dwDstPitch;

                       dmaPusherPutAddress[6] = dDrawSubchannelOffset(NV_DD_SPARE) | 0x40000;
                       dmaPusherPutAddress[7] = NV_DD_CONTEXT_BETA4;
#ifdef  DX7
                       dwMultipliers = pbd->ddrgbaScaleFactors.alpha << 24;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.red << 16;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.green << 8;
                       dwMultipliers |= pbd->ddrgbaScaleFactors.blue;
#endif  /* DX7 */
                       dmaPusherPutAddress[8] = dDrawSubchannelOffset(NV_DD_SPARE) +
                           BETA4_SET_BETA_FACTOR_OFFSET | 0x40000;
                       dmaPusherPutAddress[9] = dwMultipliers;
#ifdef  DX7
                       dmaPusherPutAddress[10] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;

                       if (dwAFlags & DDABLT_NOBLEND)
                           scaledImageObjectID =
                               NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
                       else
                           scaledImageObjectID =
                               NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;

                       dmaPusherPutAddress[11] = scaledImageObjectID;

                       dmaPusherPutAddress += 12;
#else   /* DX7 */
                       dmaPusherPutAddress += 10;
#endif  /* DX7 */
                   } else { // regular blit

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
                   }

                   pDriverData->dwDmaPusherFreeCount = freeCount;

                   /* Just in case we return unexpectedly */
                   pDriverData->NvDmaPusherPutAddress = (unsigned long)dmaPusherPutAddress;

                   if (isClipped) {

                       nRects = pbd->dwRectCnt;
                       pClipRects = pbd->prDestRects;

                       wOrigDestXY.xy16.x = (short)pbd->rOrigDest.left;
                       wOrigDestXY.xy16.y = (short)pbd->rOrigDest.top;
                       wOrigSrcXY.xy16.x  = (short)pbd->rOrigSrc.left;
                       wOrigSrcXY.xy16.y  = (short)pbd->rOrigSrc.top;

                       dwDstWidth = pbd->rOrigDest.right - pbd->rOrigDest.left;
                       dwDstHeight = pbd->rOrigDest.bottom - pbd->rOrigDest.top;
                       dwSrcWidth = pbd->rOrigSrc.right - pbd->rOrigSrc.left;
                       dwSrcHeight = pbd->rOrigSrc.bottom - pbd->rOrigSrc.top;

                       /* check for divide by 0 */
                       if ((dwDstWidth < 1) || (dwDstHeight < 1)) {
                           pbd->ddRVal = DDERR_INVALIDRECT;
                           return DDHAL_DRIVER_HANDLED;
                       }

                       /* Check if within NV hardware supported range */
                       if ((dwDstWidth > 2047) || (dwDstHeight > 2047)) {
                           return DDHAL_DRIVER_NOTHANDLED;
                       }

                       origSrcDeltaX = dwSrcWidth << 20;
                       origSrcDeltaX /= dwDstWidth;

                       origSrcDeltaY = dwSrcHeight << 20;
                       origSrcDeltaY /= dwDstHeight;

                       /* Algorithm can't handle a scale factor that is too large */
                       if (origSrcDeltaY > 0x40000000) {
                           pbd->ddRVal = DDERR_INVALIDRECT;
                           return DDHAL_DRIVER_HANDLED;
                       }

                   } else { // unclipped

                       /* check for divide by 0 */
                       if ((dwDstWidth < 1) || (dwDstHeight < 1)) {
                           pbd->ddRVal = DDERR_INVALIDRECT;
                           return DDHAL_DRIVER_HANDLED;
                       }

                       /* Check if within NV hardware supported range */
                       if ((dwDstWidth > 2047) || (dwDstHeight > 2047)) {
                           return DDHAL_DRIVER_NOTHANDLED;
                       }

                       origSrcDeltaX = dwSrcWidth << 20;
                       origSrcDeltaX /= dwDstWidth;

                       /*
                        * This fudge factor prevents us from running out of source pixels
                        * when the source starting position is fractional resulting in
                        * a true final width of dwSrcWidth - fraction. Unfortunately
                        * this fudge factor also causes problems when it is important
                        * to end on exactly the right source pixel.  The uglier of the 2 cases
                        * is to run out of source pixels so I opted to leave the fudge
                        * factor in. It works by enlarging the destination by a factor
                        * equivalent to 1 source pixel resulting in slightly enlarging
                        * the stretch factor on each source pixel.
                        */

                       if (origSrcDeltaX < 0x100000) {
                           xStretch = 0x40000000 / (origSrcDeltaX >> 10);
                           xStretch >>= 20;
                           origSrcDeltaX = dwSrcWidth << 20;
                           origSrcDeltaX /= (dwDstWidth + xStretch);
                       }

                       origSrcDeltaY = dwSrcHeight << 20;
                       origSrcDeltaY /= dwDstHeight;

                       /* Algorithm can't handle a scale factor that is too large */
                       if (origSrcDeltaY > 0x40000000) {
                           pbd->ddRVal = DDERR_INVALIDRECT;
                           return DDHAL_DRIVER_HANDLED;
                       }

                   }

nxtScaledRect:     srcDeltaX = origSrcDeltaX;
                   srcDeltaY = origSrcDeltaY;

                   if (isClipped) {
                       unsigned long dwSrcXPos;
                       unsigned long dwSrcYPos;

                       skipDeltaY = 0;
                       skipSrcBytes = 0;
                       skipV = 0;
                       srcTooWide = 0;

                       wDestXY.xy16.x = (short)pClipRects->left;
                       wDestXY.xy16.y = (short)pClipRects->top;
                       wXYext.wh16.w = (unsigned short)(pClipRects->right - pClipRects->left);
                       wXYext.wh16.h = (unsigned short)(pClipRects->bottom - pClipRects->top);
                       pClipRects++;

                       dwDstWidth = (unsigned long)wXYext.wh16.w;
                       dwDstHeight = (unsigned long)wXYext.wh16.h;

                       /* calculate starting source X position */
                       dwSrcXPos = (long)(wDestXY.xy16.x - wOrigDestXY.xy16.x);

                       dwSrcXPos *= origSrcDeltaX; /* result is 12:20 */

                                                           /* truncate fraction */
                       wSrcXY.xy16.x = wOrigSrcXY.xy16.x + (short)(dwSrcXPos >> 20);

                       wSrcPoint.xy16.x = (short)((dwSrcXPos & 0xFFFFF) >> 16);

                       /* calculate starting source Y position */
                       dwSrcYPos = (long)(wDestXY.xy16.y - wOrigDestXY.xy16.y);

                       dwSrcYPos *= origSrcDeltaY; /* result is 12.20 */

                                                           /* truncate fraction */
                       wSrcXY.xy16.y = wOrigSrcXY.xy16.y + (short)(dwSrcYPos >> 20);

                       srcClippedInitFract = dwSrcYPos & 0xFFFFF;

                       /* calculate source X extent */
                       dwSrcWidth = (unsigned long)wXYext.wh16.w;

                       dwSrcWidth *= origSrcDeltaX; /* result is 12.20 */

                       dwSrcWidth += (dwSrcXPos & 0xFFFFF); /* include starting fraction of source */

                       dwSrcWidth >>= 20; /* we lose precision here, may need changed later */

                       if (wSrcXY.xy16.x + dwSrcWidth > src->wWidth)
                           dwSrcWidth = src->wWidth - wSrcXY.xy16.x;

                       /* calculate source Y extent */
                       dwSrcHeight = (unsigned long)wXYext.wh16.h;

                       dwSrcHeight *= origSrcDeltaY; /* result is 12.20 */

                       dwSrcHeight += (dwSrcYPos & 0xFFFFF); /* include starting fraction of source */

                       dwSrcHeight >>= 20; /* we lose precision here, may need changed later */


                   }

                   if (isVideo < 2)
                       srcBufferPitch = dwSrcWidth << 2;
                   else
                       srcBufferPitch = dwSrcWidth << 1;

                   srcDeltaScans = (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1) -   /* allow for 2 extra src scanlines */
                            NV_DD_NOTIFIER_BLOCK_SIZE - (srcBufferPitch << 1); /* buffer contains several separate notifiers */

                   srcDeltaScans /= srcBufferPitch;

                   /* limiting source scans per DMA increases parallelism */
                   if (srcDeltaScans > 32)
                       srcDeltaScans = 32;
                   else if (srcDeltaScans > 16)
                       srcDeltaScans = 16;
                   else if (srcDeltaScans > 8)
                       srcDeltaScans = 8;
                   else if (srcDeltaScans > 4)
                       srcDeltaScans = 4;
                   else
                       srcDeltaScans = 2;


                   /* If shrinking in vertical direction then adjust to skip src scanlines */
                   if (srcDeltaY >= (srcDeltaScans << 20)) { /* limit Y delta range to acceptable range */
                       skipDeltaY = srcDeltaY;
                       skipSrcBytes = srcScanLength;
                       skipV = 1;
                       while (srcDeltaY >= (srcDeltaScans << 20)) {
                           srcDeltaY >>= 1;
                           skipSrcBytes <<= 1;
                           skipV <<= 1;
                           skipDeltaY <<= 1;
                       }
                       skipSrcBytes -= srcScanLength;
                       skipV -= 1;
                       skipDeltaY -= srcDeltaY;
                   }

                   /* Algorithm can't handle scale factor that is too small */
                   if ((srcDeltaY & 0xFFFFFC00) == 0) {
                       pbd->ddRVal = DDERR_INVALIDRECT;
                       return DDHAL_DRIVER_HANDLED;
                   }

                   yStretch = 0x40000000 / ((srcDeltaY + 0x00000200) >> 10);

                   dstDeltaY = srcDeltaScans * yStretch;  // how many dst scanlines will srcDeltaScans scanlines generate

                   dstDeltaScans = dstDeltaY >> 20;

                   if (dstDeltaScans == 0) { /* if shrinking too much then can't handle */
                       return DDHAL_DRIVER_NOTHANDLED;
                   }

                   if (wSrcXY.xy16.y + dwSrcHeight > src->wHeight)
                       dwSrcHeight = src->wHeight - wSrcXY.xy16.y;

                   if (isVideo < 2)
                       nxtSrcScan = (srcScanLength >> 2);
                   else
                       nxtSrcScan = (srcScanLength >> 1);

                   srcDeltaYPerBand = dstDeltaScans * (srcDeltaY + skipDeltaY);

                   if (doMirrorUpDown) {
                       wSrcXY.xy16.y += (short)(dwSrcHeight - 1);
                       srcDeltaY = -srcDeltaY;
                       srcDeltaYPerBand = -srcDeltaYPerBand;
                       skipDeltaY = -skipDeltaY;
                       nxtSrcScan = -nxtSrcScan;
                       skipSrcBytes = -skipSrcBytes;
                   }

                   pDmaDmaFromMemNotifier = (NvNotification *)pDriverData->NvDmaDmaFromMemNotifierFlat;
                   pScanlineBuffer = (unsigned char *)pDriverData->NvScanlineBufferFlat;

                   if (srcDeltaY > 0)
                       srcCurY = 0;
                   else
                       srcCurY = ((long)dwSrcHeight << 20);

                   /* Calculate initial source fractional X */
                   if ((srcDeltaX > 0x40000000) || ((srcDeltaX & 0xFFFFFC00) == 0)) {
                       pbd->ddRVal = DDERR_INVALIDRECT;
                       return DDHAL_DRIVER_HANDLED;
                   }

                   if (!(isClipped)) {
                       wSrcPoint.xy = 0;

                       /* Do this math in 16.16 precision to avoid 12. overflow */
                       dstDeltaX = ((long)wSrcXY.xy16.x << 16) / (srcDeltaX >> 4);

                       /* Do this math in 16.16 precision to avoid 12. overflow */
                       srcInitFract = dstDeltaX * (srcDeltaX >> 4);

                       /* Do this math in 16.16 precision to avoid 12. overflow */
                       while (srcInitFract >> 16 < wSrcXY.xy16.x)
                           srcInitFract += (srcDeltaX >> 4);

                       /* Convert to 12.4 format */
                       wSrcPoint.xy16.x = (short)(srcInitFract & 0xFFFF) >> 12;
                   }

                       // DMAable ?
                   if ((isVideo > 3) || ((dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                       ((dwSrcCaps & DDSCAPS_TEXTURE) == 0) &&
                       ((src->fpVidMem >= pDriverData->BaseAddress) || isOverlappingVmem))) {

                       if (isClipped) {
                           srcInitFract = srcClippedInitFract;
                       } else {
                           srcInitFract = 0;

                           if (srcDeltaY > 0) {
                               /* Calculate initial source fractional Y */
                               /* Do this math in 16.16 precision to avoid 12. overflow */
                               dstDeltaY = ((long)wSrcXY.xy16.y << 16) / (srcDeltaY  >> 4);

                               /* Do this math in 16.16 precision to avoid 12. overflow */
                               srcInitFract = dstDeltaY * (srcDeltaY >> 4);

                               /* Do this math in 16.16 precision to avoid 12. overflow */
                               while (srcInitFract >> 16 < wSrcXY.xy16.y)
                                   srcInitFract += (srcDeltaY >> 4);

                               /* Convert result back to 12.20 precision */
                               srcInitFract <<= 4;

                               srcInitFract &= 0xFFFFF;
                           }
                       }

                       srcCurY += srcInitFract;

                       srcCurFract = srcInitFract;

                       if (isVideo < 4) {
                           srcInVideoMemory = 1;

                           if (doMirrorUpDown) {
                               pSaveSrcBits = (unsigned long *)
                                  (pSrcInitBits - pDriverData->BaseAddress);
                               wSrcPoint.xy16.y += ((short)(dwSrcHeight - 1) << 4);
                           } else {
                               wSrcPoint.xy16.y += (short)(srcCurFract >> 16);

                               if (isVideo < 2)
                                   pSaveSrcBits = (unsigned long *)
                                      ((pSrcInitBits - pDriverData->BaseAddress) +
                                       (wSrcXY.xy16.y * srcScanLength));
                               else
                                   pSaveSrcBits = (unsigned long *)
                                      ((pSrcInitBits - pDriverData->BaseAddress) +
                                       (wSrcXY.xy16.y * (srcScanLength << 1)));
                           }
                       } else {

                           if (doMirrorUpDown) {
                               pSaveSrcBits = 0;
                               wSrcPoint.xy16.y += ((short)(dwSrcHeight - 1) << 4);
                           } else {
                               wSrcPoint.xy16.y += (short)(srcCurFract >> 16);

                               pSaveSrcBits = (unsigned long *)
                                   (wSrcXY.xy16.y * (srcScanLength << 1));
                           }
                       }

                       if (isVideo < 2)
                           pSaveSrcBits += wSrcXY.xy16.x;
                       else
                           pSaveSrcBits += (wSrcXY.xy16.x >> 1);

                       while (freeCount < 2)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                       freeCount -= 2;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;

                       if (isAlphaBlt) {
#ifdef  DX7
                           if (dwAFlags & DDABLT_NOBLEND)
                               scaledImageObjectID =
                                   NV_DD_SRCCOPY_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
                           else
                               scaledImageObjectID =
                                   NV_DD_ABLEND_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
#endif  /* DX7 */
                       } else if (isVideo < 2)
                           scaledImageObjectID =
                               NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
                       else if (isVideo == 2)
                           scaledImageObjectID =
                               NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
                       else if (isVideo == 3)
                           scaledImageObjectID =
                               NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
                       else if (isVideo == 4)
                           scaledImageObjectID =
                               NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                       else if (isVideo == 5)
                           scaledImageObjectID =
                               NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                       else if (isVideo == 6)
                           scaledImageObjectID =
                               NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
                       else if (isVideo == 7)
                           scaledImageObjectID =
                               NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;

                       dmaPusherPutAddress[1] = scaledImageObjectID;

                       dmaPusherPutAddress += 2;

                   } else {
                       if (isVideo < 2) {
                           pSaveSrcBits = (unsigned long *)(pSrcInitBits +
                               (wSrcXY.xy16.y * srcScanLength));
                           pSaveSrcBits += wSrcXY.xy16.x;
                       } else {
                           pSaveSrcBits = (unsigned long *)(pSrcInitBits +
                               (wSrcXY.xy16.y * (srcScanLength << 1)));
                           pSaveSrcBits += (wSrcXY.xy16.x >> 1);
                       }

                       pSrcCurScan = pSaveSrcBits;

                       while (freeCount < 2)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                       freeCount -= 2;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;

                       if (isVideo == 2)
                           scaledImageObjectID = NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                       else if (isVideo == 3)
                           scaledImageObjectID = NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;

                       /* NOTE: This may load the default scaledImageObjectID */
                       dmaPusherPutAddress[1] = scaledImageObjectID;

                       dmaPusherPutAddress += 2;
                   }

                   pSaveSrcInitBits = (unsigned char *)pSaveSrcBits;

                   curClipPoint = wDestXY.xy;
                   curClipExtent = wXYext.wh;
                   curDUDX = srcDeltaX;
                   if (srcDeltaY > 0)
                       curDVDY = srcDeltaY;
                   else
                       curDVDY = -srcDeltaY;

                   while (freeCount < 6)
                       NvGetDmaBufferFreeCount(npDev, freeCount, 6, dmaPusherPutAddress);
                   freeCount -= 6;

                   dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                       SCALED_IMAGE_CLIPPOINT_OFFSET | 0x80000;
                   dmaPusherPutAddress[1] = curClipPoint;
                   dmaPusherPutAddress[2] = curClipExtent;
                   dmaPusherPutAddress[3] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                       SCALED_IMAGE_DELTADUDX_OFFSET | 0x80000;
                   dmaPusherPutAddress[4] = curDUDX;
                   dmaPusherPutAddress[5] = curDVDY;

                   dmaPusherPutAddress += 6;

                   wDstExt.wh = wXYext.wh;

                   /* NV4 Scaled Image class requires source width to be even */
                   dwSrcWidth = (dwSrcWidth + 1) & ~1;

                   /* NV3 hardware can only handle a maximum source width of 1024 */
                   if (dwSrcWidth > 1024) {
                       srcTooWide = 1;
                       xStretch = 0x40000000 / (srcDeltaX >> 10);
                       dstDeltaX = (xStretch * 1024) >> 20;
                       wDstExt.wh16.w = (unsigned short)dstDeltaX;
                       srcBufferPitch = 4096;
                   }

                   // Let the world know we are doing DMA operations
//                   pDriverData->NvDmaFromMemSurfaceMem = src->fpVidMem;
                   pDriverData->NvDmaFromMemSurfaceMem = dwSrcOffset + pDriverData->BaseAddress;

oneMorePass:       n = dwDstHeight;

                   srcCurY &= 0xFFF00000;

                   if (isClipped) {
                       srcInitFract = srcClippedInitFract;
                   } else {
                       srcInitFract = 0;

                       if (srcDeltaY > 0) {
                           /* Calculate initial source fractional Y */
                           /* Do this math in 16.16 precision to avoid 12. overflow */
                           dstDeltaY = ((long)wSrcXY.xy16.y << 16) / (srcDeltaY  >> 4);

                           /* Do this math in 16.16 precision to avoid 12. overflow */
                           srcInitFract = dstDeltaY * (srcDeltaY >> 4);

                           /* Do this math in 16.16 precision to avoid 12. overflow */
                           while (srcInitFract >> 16 < wSrcXY.xy16.y)
                               srcInitFract += (srcDeltaY >> 4);

                           /* Convert result back to 12.20 precision */
                           srcInitFract <<= 4;

                           srcInitFract &= 0xFFFFF;
                       }
                   }

                   srcCurY += srcInitFract;

                   srcCurFract = srcInitFract;

                   curV = srcCurY >> 20;

                   wDstPoint.xy = wDestXY.xy;

                   dstStartY = dstCurY = wDestXY.xy16.y;

                   curBuffer = 0;

                   if ((srcInVideoMemory) || (isVideo > 3)) { // DMAable ?

                       // wait for any pending notification operation to finish
//this shouldn't be necessary  while ((volatile)pDmaDmaFromMemNotifier->status == NV_IN_PROGRESS);

                       pDmaDmaFromMemNotifier->status = NV_IN_PROGRESS;

                       while (freeCount < 11)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
                       freeCount -= 11;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_OUT_POINT_OFFSET | 0x80000;
                       dmaPusherPutAddress[1] = wDstPoint.xy;
                       dmaPusherPutAddress[2] = wDstExt.wh;
                       dmaPusherPutAddress[3] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0xC0000;

                       if ((dwSrcWidth < 16) && (isVideo < 4)) { // workaround for NV3 Rev B hardware bug (only happens if source is in video memory)
                           dmaPusherPutAddress[4] =
                               asmMergeCoords(dwSrcPitch, dwSrcHeight);
                       } else {
                           dmaPusherPutAddress[4] =
                               asmMergeCoords(dwSrcWidth, dwSrcHeight);
                       }

                       if ((isAlphaBlt) && (doBilinearFiltering == 0))
                           dmaPusherPutAddress[5] =
                               (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) |
                               (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16) |
                               dwSrcPitch;
                       else // filter
                           dmaPusherPutAddress[5] =
                               (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |
                               (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                               dwSrcPitch;

                       dmaPusherPutAddress[6] = (unsigned long)pSaveSrcBits;

                       dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_NOTIFY_OFFSET | 0x40000;
                       dmaPusherPutAddress[8] = 0;
                       dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_IN_POINT_OFFSET | 0x40000;
                       dmaPusherPutAddress[10] = wSrcPoint.xy;

                       dmaPusherPutAddress += 11;

                       /* Force write combine buffer to flush */
                       pDriverData->NvDmaPusherBufferEnd[0] = 0;
                       /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                       _outp (0x3d0,0);

                       pDriverData->dwDmaPusherFreeCount = freeCount;

                       npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                       n = 0;
                   } else {
                       // wait for any pending notification operation to finish
                       while ((volatile)pDmaDmaFromMemNotifier->status == NV_IN_PROGRESS);
                   }

                   while (n >= dstDeltaScans) {
                       FAST unsigned long *pSrcBits;

                       n -= dstDeltaScans;
                       srcStartY = srcCurY;
                       srcStartFract = srcCurFract;
                       srcCurY += srcDeltaYPerBand;

                       if (doMirrorUpDown) {
                           if (((srcCurY - 0x100000) >> 20) < 0)
                               srcCurY = 0x100000;
                       } else {
                           if (((srcCurY + 0x100000) >> 20) >  ((long)dwSrcHeight - 1))
                               srcCurY = ((long)dwSrcHeight - 2) << 20;
                       }

                       srcCurFract = srcCurY & 0xFFFFF;

                       if (curBuffer == 0)
                           npPixelBuf = (unsigned char *)pScanlineBuffer;
                       else
                           npPixelBuf = (unsigned char *)(pScanlineBuffer +
                               (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1));

                       /* Must recalculate on each pass since src scanlines may be used more than once */
                       pSrcBits = (unsigned long *)(pSaveSrcInitBits +
                           (curV * (nxtSrcScan << 2)));

                       pSrcCurScan = pSrcBits;

                       if (doMirrorUpDown)
                           m = (srcStartY >> 20) - ((srcCurY - 0x100000) >> 20);
                       else
                           m = ((srcCurY + 0x100000) >> 20) - (srcStartY >> 20);

                       srcCurDeltaScans = 0;

                       /* Copy and convert a block of source pixels to transfer buffer */
                       while (--m >= 0) {
                           if (pSrcBits >= (unsigned long *)pSrcInitBits) {
  //                           asmCopyScan(pSrcBits, npPixelBuf, srcBufferPitch);
                               _asm
                               {
#ifdef P6
                                   mov ecx, dword ptr srcBufferPitch
                                   mov esi, dword ptr pSrcBits
                                   shr ecx, 5
                     nxtCacheLine: mov eax, [esi]
                                   add esi, 32
                                   dec ecx
                                   jg nxtCacheLine
#endif /* P6 */
                                   mov ecx, dword ptr srcBufferPitch
                                   mov esi, dword ptr pSrcBits
                                   shr ecx, 2
                                   mov edi, dword ptr npPixelBuf
                                   test ecx, 1
                                   jz doQwords
                                   mov eax, [esi]
                                   add esi, 4
                                   mov [edi], eax
                                   add edi, 4
                         doQwords: shr ecx, 1
                                   jz copyDone
                         nxtQword: mov eax, [esi]
                                   mov edx, [esi+4]
                                   add edi, 8
                                   add esi, 8
                                   dec ecx
                                   mov [edi-8], eax
                                   mov [edi-4], edx
                         copyDone: jg nxtQword
                               }
                           }
                           npPixelBuf += srcBufferPitch;
                           pSrcCurScan += (nxtSrcScan + skipSrcBytes);
                           pSrcBits = (unsigned long *)pSrcCurScan;
                           srcCurDeltaScans++;
                           m -= skipV;
                       }

                       /* wait for Dma operation to finish */
                       while (pDmaDmaFromMemNotifier->status == NV_IN_PROGRESS);

                       pDmaDmaFromMemNotifier->status = NV_IN_PROGRESS;

                       wDstPoint.xy16.y = (short)dstCurY;
                       wDstExt.wh16.h = (unsigned short)dstDeltaScans;
                       wSrcPoint.xy16.y = (short)(srcStartFract >> 16); /* convert V to 12.4 */
                       wSrcExt.wh = srcCurDeltaScans;
                       wSrcExt.wh <<= 16;    /* shift V extent into place */
                       wSrcExt.wh |= dwSrcWidth;

//// BEGIN WORKAROUND FOR NV4 SCALED IMAGE HARDWARE BUG

                       while (freeCount < 8)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
                       freeCount -= 8;

                       /* Reload object */
                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;
                       dmaPusherPutAddress[1] = scaledImageObjectID;
                       /* Reinitialize volatile methods */
                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_CLIPPOINT_OFFSET | 0x80000;
                       dmaPusherPutAddress[3] = curClipPoint;
                       dmaPusherPutAddress[4] = curClipExtent;
                       dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_DELTADUDX_OFFSET | 0x80000;
                       dmaPusherPutAddress[6] = curDUDX;
                       dmaPusherPutAddress[7] = curDVDY;

                       dmaPusherPutAddress += 8;

//// END WORKAROUND FOR NV4 SCALED IMAGE HARDWARE BUG

                       while (freeCount < 11)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
                       freeCount -= 11;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_OUT_POINT_OFFSET | 0x80000;
                       dmaPusherPutAddress[1] = wDstPoint.xy;
                       dmaPusherPutAddress[2] = wDstExt.wh;
                       dmaPusherPutAddress[3] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0xC0000;
                       dmaPusherPutAddress[4] = wSrcExt.wh;

                       if ((isAlphaBlt) && (doBilinearFiltering == 0))
                           dmaPusherPutAddress[5] =
                               (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) |
                               (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16) |
                               srcBufferPitch;
                       else // filter
                           dmaPusherPutAddress[5] =
                               (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |
                               (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                               srcBufferPitch;

                       if (curBuffer == 0)
                           dmaPusherPutAddress[6] =
                               NV_DD_NOTIFIER_BLOCK_SIZE; /* There are several notifiers in the buffer */
                       else
                           dmaPusherPutAddress[6] =
                               NV_DD_NOTIFIER_BLOCK_SIZE + (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1);

                       dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_NOTIFY_OFFSET | 0x40000;
                       dmaPusherPutAddress[8] = 0;
                       dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_IN_POINT_OFFSET | 0x40000;
                       dmaPusherPutAddress[10] = wSrcPoint.xy;

                       dmaPusherPutAddress += 11;

                       /* Force write combine buffer to flush */
                       pDriverData->NvDmaPusherBufferEnd[0] = 0;
                       /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                       _outp (0x3d0,0);

                       pDriverData->dwDmaPusherFreeCount = freeCount;

                       npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;

                       curBuffer ^= 1;

                       dstCurY += dstDeltaScans;

                       curV = srcCurY >> 20;
                   }

                   if (n > 0) {
                       FAST unsigned long *pSrcBits;

                       srcStartY = srcCurY;
                       srcStartFract = srcCurFract;
                       srcCurY += srcDeltaYPerBand;

                       if (doMirrorUpDown) {
                           if (((srcCurY - 0x100000) >> 20) < 0)
                               srcCurY = 0x100000;
                       } else {
                           if (((srcCurY + 0x100000) >> 20) > ((long)dwSrcHeight - 1))
                               srcCurY = ((long)dwSrcHeight - 2) << 20;
                       }

                       srcCurFract = srcCurY & 0xFFFFF;

                       if (curBuffer == 0)
                           npPixelBuf = (unsigned char *)pScanlineBuffer;
                       else
                           npPixelBuf = (unsigned char *)(pScanlineBuffer +
                               (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1));

                       /* Must recalculate since src scanlines may be used more than once */
                       pSrcBits = (unsigned long *)(pSaveSrcInitBits +
                           (curV * (nxtSrcScan << 2)));

                       pSrcCurScan = pSrcBits;

                       if (doMirrorUpDown)
                           m = (srcStartY >> 20) - ((srcCurY - 0x100000) >> 20);
                       else
                           m = ((srcCurY + 0x100000) >> 20) - (srcStartY >> 20);

                       srcCurDeltaScans = 0;

                       /* Copy and convert a block of source pixels to transfer buffer */
                       while (--m >= 0) {
                           if (pSrcBits >= (unsigned long *)pSrcInitBits) {
  //                           asmCopyScan(pSrcBits, npPixelBuf, srcBufferPitch);
                               _asm
                               {
#ifdef P6
                                   mov ecx, dword ptr srcBufferPitch
                                   mov esi, dword ptr pSrcBits
                                   shr ecx, 5
                    nxtCacheLine2: mov eax, [esi]
                                   add esi, 32
                                   dec ecx
                                   jg nxtCacheLine2
#endif /* P6 */
                                   mov ecx, dword ptr srcBufferPitch
                                   mov esi, dword ptr pSrcBits
                                   shr ecx, 2
                                   mov edi, dword ptr npPixelBuf
                                   test ecx, 1
                                   jz doQwords2
                                   mov eax, [esi]
                                   add esi, 4
                                   mov [edi], eax
                                   add edi, 4
                        doQwords2: shr ecx, 1
                                   jz copyDone2
                        nxtQword2: mov eax, [esi]
                                   mov edx, [esi+4]
                                   add edi, 8
                                   add esi, 8
                                   dec ecx
                                   mov [edi-8], eax
                                   mov [edi-4], edx
                        copyDone2: jg nxtQword2
                               }
                           }
                           npPixelBuf += srcBufferPitch;
                           pSrcCurScan += (nxtSrcScan + skipSrcBytes);
                           pSrcBits = (unsigned long *)pSrcCurScan;
                           srcCurDeltaScans++;
                           m -= skipV;
                       }

                       /* wait for Dma operation to finish */
                       while (pDmaDmaFromMemNotifier->status == NV_IN_PROGRESS);

                       pDmaDmaFromMemNotifier->status = NV_IN_PROGRESS;

                       wDstPoint.xy16.y = (short)dstCurY;
                       wDstExt.wh16.h = (unsigned short)n;
                       wSrcPoint.xy16.y = (short)(srcStartFract >> 16); /* convert V to 12.4 */
                       wSrcExt.wh = srcCurDeltaScans;
                       wSrcExt.wh <<= 16;    /* shift V extent into place */
                       wSrcExt.wh |= dwSrcWidth;

//// BEGIN WORKAROUND FOR NV4 SCALED IMAGE HARDWARE BUG

                       while (freeCount < 8)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 8, dmaPusherPutAddress);
                       freeCount -= 8;

                       /* Reload object */
                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) | 0x40000;
                       dmaPusherPutAddress[1] = scaledImageObjectID;
                       /* Reinitialize volatile methods */
                       dmaPusherPutAddress[2] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_CLIPPOINT_OFFSET | 0x80000;
                       dmaPusherPutAddress[3] = curClipPoint;
                       dmaPusherPutAddress[4] = curClipExtent;
                       dmaPusherPutAddress[5] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_DELTADUDX_OFFSET | 0x80000;
                       dmaPusherPutAddress[6] = curDUDX;
                       dmaPusherPutAddress[7] = curDVDY;

                       dmaPusherPutAddress += 8;

//// END WORKAROUND FOR NV4 SCALED IMAGE HARDWARE BUG

                       while (freeCount < 11)
                           NvGetDmaBufferFreeCount(npDev, freeCount, 11, dmaPusherPutAddress);
                       freeCount -= 11;

                       dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_OUT_POINT_OFFSET | 0x80000;
                       dmaPusherPutAddress[1] = wDstPoint.xy;
                       dmaPusherPutAddress[2] = wDstExt.wh;
                       dmaPusherPutAddress[3] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_IN_SIZE_OFFSET | 0xC0000;
                       dmaPusherPutAddress[4] = wSrcExt.wh;

                       if ((isAlphaBlt) && (doBilinearFiltering == 0))
                           dmaPusherPutAddress[5] =
                               (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_ZOH << 24) |
                               (NV077_IMAGE_IN_FORMAT_ORIGIN_CORNER << 16) |
                               srcBufferPitch;
                       else // filter
                           dmaPusherPutAddress[5] =
                               (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |
                               (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                               srcBufferPitch;

                       if (curBuffer == 0)
                           dmaPusherPutAddress[6] =
                               NV_DD_NOTIFIER_BLOCK_SIZE; /* There are several notifiers in the buffer */
                       else
                           dmaPusherPutAddress[6] =
                               NV_DD_NOTIFIER_BLOCK_SIZE + (NV_DD_COMMON_DMA_BUFFER_SIZE >> 1);

                       dmaPusherPutAddress[7] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_NOTIFY_OFFSET | 0x40000;
                       dmaPusherPutAddress[8] = 0;
                       dmaPusherPutAddress[9] = dDrawSubchannelOffset(NV_DD_STRETCH) +
                           SCALED_IMAGE_IMAGE_IN_POINT_OFFSET | 0x40000;
                       dmaPusherPutAddress[10] = wSrcPoint.xy;

                       dmaPusherPutAddress += 11;

                       /* Force write combine buffer to flush */
                       pDriverData->NvDmaPusherBufferEnd[0] = 0;
                       /* Don't want this, but it's currently the only known way to guarantee a write combine flush. */
                       _outp (0x3d0,0);

                       pDriverData->dwDmaPusherFreeCount = freeCount;

                       npDev->Put = (dmaPusherPutAddress - dmaPusherBufferBase) << 2;
                   }

                   if (srcTooWide) {
                       srcTooWide = 0;
                       wDestXY.xy16.x += (short)dstDeltaX;
                       wDstExt.wh16.w = wXYext.wh16.w - wDstExt.wh16.w;
                       if ((srcInVideoMemory) || (isVideo > 3))
                           wSrcPoint.xy16.x += (1024 << 4);
                       srcBufferPitch = dwSrcWidth << 2;
                       srcBufferPitch -= 4096;
                       pSaveSrcInitBits += 1024;
                       pSaveSrcBits = (unsigned long *)pSaveSrcInitBits;
                       pSrcCurScan = pSaveSrcBits;
                       srcCurY = ((long)wSrcXY.xy16.y << 20);
                       goto oneMorePass;
                   }

                   if (isClipped) {
                       if (--nRects > 0)
                           goto nxtScaledRect;
                   }
               }    // stretch

               if (isOverlappingVmem && lpSurfTemp) {
                   // FIXME: wait for blt to complete
                   NVHEAP_FREE(lpSurfTemp);
               }
            }

            if (dwFlags & DDBLT_KEYSRCOVERRIDE) {
                /* disable chroma key */
                while (freeCount < 2)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                freeCount -= 2;

                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_TRANSCOLOR) +
                    SET_TRANSCOLOR_OFFSET | 0x40000;
                dmaPusherPutAddress[1] = 0;

                dmaPusherPutAddress += 2;
            }

            if (isAlphaBlt) {
                /* Restore default destination color format */
                while (freeCount < 2)
                    NvGetDmaBufferFreeCount(npDev, freeCount, 2, dmaPusherPutAddress);
                freeCount -= 2;

                dmaPusherPutAddress[0] = dDrawSubchannelOffset(NV_DD_SURFACES) +
                    SURFACES_2D_SET_COLOR_FORMAT_OFFSET | 0x40000;

                dmaPusherPutAddress[1] =
                    NV042_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8;

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

    } else if (dwFlags & DDBLT_DEPTHFILL) {

        if (!(dwDstCaps & DDSCAPS_ZBUFFER)) {
            pbd->ddRVal = DDERR_INVALIDSURFACETYPE;
            return DDHAL_DRIVER_HANDLED;
        }

        pDriverData->dwDmaPusherFreeCount = freeCount;
        (unsigned long *)pDriverData->NvDmaPusherPutAddress = dmaPusherPutAddress;

        nvD3DClearZBuffer(dstx, pbd->bltFX.dwFillDepth);

        dmaPusherPutAddress = (unsigned long *)pDriverData->NvDmaPusherPutAddress;
#ifdef  CACHE_FREECOUNT
        freeCount = pDriverData->dwDmaPusherFreeCount;
#ifdef  DEBUG
        NvCheckCachedFreeCount(freeCount, dmaPusherPutAddress);
#endif  /* DEBUG */
#else   /* CACHE_FREECOUNT */
        freeCount = 0;
#endif  /* CACHE_FREECOUNT */

    } else {
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

} /* Blt32bpp32 */
