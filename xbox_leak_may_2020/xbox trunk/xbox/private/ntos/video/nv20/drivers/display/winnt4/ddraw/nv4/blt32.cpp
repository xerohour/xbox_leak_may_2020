/*==========================================================================;
 *
 *  Copyright (C) 1995,1997 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       Nv4Blt32.c
 *  Content:    Windows95 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"
#include "nvalpha.h"
#include "ddminint.h"
#include "nv32.h"

#include "nvos.h"
#include "nvntioctl.h"
#include "nvntddctl.h"
#include "nvapi.h"
#include "nvsubch.h"

#define NvGetFreeCount(NN, CH)\
                 (NN)->subchannel[CH].control.Free
/*
 * Nv4Blt32
 *
 * NV4 Blt function for 32bpp surfaces.
 */
DWORD __stdcall Nv4Blt32( PDD_BLTDATA pbd )
    {
    FAST Nv3ChannelPio *npDev;
    FAST USHORT freeCount;
    DWORD       dwFlags;
    DWORD       dwDDFX = 0;
    DWORD       dwDstCaps;
    DWORD       dwSrcCaps;
    DWORD       dwRop;
    NVCOORD     wXYext;
    NVCOORD     wDestXY;
    NVCOORD     wSrcXY;
    BYTE        wStretch = 0;
    BYTE        doMirrorUpDown = 0;
    BYTE        isVideo = 0;
//    BYTE        doAlpha = 0;

    DWORD       dwFillColor;
    DWORD       dwColorKey;

    DWORD       dwDstOffset;
    DWORD       dwDstPitch;
    DWORD       dwDstWidth;
    DWORD       dwDstHeight;

    DWORD       dwSrcOffset;
    DWORD       dwSrcPitch;
    DWORD       dwSrcWidth;
    DWORD       dwSrcHeight;

    PDD_SURFACE_LOCAL  srcx;
    PDD_SURFACE_LOCAL  dstx;
    PDD_SURFACE_GLOBAL  src;
    PDD_SURFACE_GLOBAL  dst;
    PDEV    *ppdev;

    dstx = pbd->lpDDDestSurface;
    dst = dstx->lpGbl;
    ppdev = (PDEV *) pbd->lpDD->dhpdev;

    /*
     * NOTES:
     *
     * Everything you need is in pdb->bltFX .
     * Look at pdb->dwFlags to determine what kind of blt you are doing,
     * DDBLT_xxxx are the flags.
     *
     * ALPHA NOTES:
     *
     * Alpha ALWAYS comes in BLTFX.   You don't need to go looking for
     * the attached surface. If DDBLT_ALPHA is specified, then either a
     * constant alpha or alpha surface has been specified. Just look for
     * DDBLT_ALPHASURFACEOVERRIDE or DDBLT_ALPHACONSTANTOVERRIDE
     *
     * Look for DDBLT_ALPHASURFACEDESTRECT to use the destination rectangle
     * for choosing the rectangle in the alpha surface.   Otherwise use the
     * source rectangle.
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

    if (dwFlags & DDBLT_DDFX)
        dwDDFX = pbd->bltFX.dwDDFX;

//    if (dwFlags & DDOVER_ANYALPHA) {
//        DPF("Overlay Alpha failed");
//        pbd->ddRVal = DDERR_NOOVERLAYHW;
//        return DDHAL_DRIVER_HANDLED;
//    }

//    if (dwFlags & DDBLT_ANYALPHA) {
//        DPF("Alpha requested");
//        if ((dwFlags & DDBLT_ALPHASRCCONSTOVERRIDE) == 0) {
//            pbd->ddRVal = DDERR_NOALPHAHW;
//            return DDHAL_DRIVER_HANDLED;
//        } else
//            doAlpha = 1;
//    }

    if (dwFlags & (DDBLT_KEYDEST | DDBLT_KEYDESTOVERRIDE))
        {
        DPF("Dest color key failed");
        pbd->ddRVal = DDERR_NOCOLORKEYHW;
        return DDHAL_DRIVER_HANDLED;
        }


    /*
     * get offset, width, and height for destination
     */
    dwDstOffset = dst->fpVidMem;
    dwDstPitch  = dst->lPitch;
    dwDstWidth  = pbd->rDest.right  - pbd->rDest.left;
    dwDstHeight = pbd->rDest.bottom - pbd->rDest.top;
    wXYext.wh16.w = (unsigned short)dwDstWidth;
    wXYext.wh16.h = (unsigned short)dwDstHeight;

    wDestXY.xy16.x = (short)pbd->rDest.left;
    wDestXY.xy16.y = (short)pbd->rDest.top;

    if (dwDstCaps & (DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE | DDSCAPS_NONLOCALVIDMEM))
        return DDHAL_DRIVER_NOTHANDLED;

    npDev = (Nv3ChannelPio*)ppdev->pjMmBase;

    if (npDev == NULL)
        {
        pbd->ddRVal = DDERR_SURFACELOST;
        return DDHAL_DRIVER_HANDLED;
        }

    freeCount = ppdev->NVFreeCount;

    /*
     * If async, then only work if bltter isn't busy
     * This should probably be a little more specific to each call, but
     * waiting for 32 is pretty close
     */
    if ( dwFlags & DDBLT_ASYNC )
        {
        if (freeCount < 32)
            freeCount = NvGetFreeCount(npDev, NV_DD_ROP);

        if ( freeCount < 32 )
            {
            DPF("ASYNC FAILED");
            pbd->ddRVal = DDERR_WASSTILLDRAWING;
            ppdev->NVFreeCount = freeCount;
            return DDHAL_DRIVER_HANDLED;
            }
        }

    if (dwFlags & DDBLT_ROP)
        {

        dwRop = pbd->bltFX.dwROP >> 16;

        if ((dwRop == BLACKNESSINDEX) || (dwRop == WHITENESSINDEX))
            {
            ppdev->NVFreeCount = freeCount;
            DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
            freeCount = ppdev->NVFreeCount;

            while (freeCount < 4*4)
                freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
            freeCount -= 4*4;

            npDev->dDrawRop.SetRop5 = dwRop;
            npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.Color1A = 0;

            npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.UnclippedRectangle[0].point =
            asmMergeCoords(wDestXY.xy16.y, wDestXY.xy16.x);

            npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.UnclippedRectangle[0].size =
            asmMergeCoords(wXYext.wh16.h, wXYext.wh16.w);
            }
        else if ((dwRop == PATCOPYINDEX) || (dwRop == PATINVERTINDEX))
            {

            dwFillColor = (pbd->bltFX.dwFillColor & ppdev->physicalColorMask) | ppdev->AlphaEnableValue;

            if (dwRop == PATCOPYINDEX)
                dwRop = SRCCOPYINDEX;
            else if (dwRop == PATINVERTINDEX)
                dwRop = SRCINVERTINDEX;


            ppdev->NVFreeCount = freeCount;
            DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
            freeCount = ppdev->NVFreeCount;

            while (freeCount < 4*4)
                freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
            freeCount -= 4*4;

            npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.Color1A = dwFillColor;
            npDev->dDrawRop.SetRop5 = dwRop;
            npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.UnclippedRectangle[0].point =
            asmMergeCoords(wDestXY.xy16.y, wDestXY.xy16.x);

            npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.UnclippedRectangle[0].size =
            asmMergeCoords(wXYext.wh16.h, wXYext.wh16.w);

            }
        else if ((dwRop == SRCCOPYINDEX) || (dwRop == SRCPAINTINDEX) ||
                 (dwRop == SRCANDINDEX) || (dwRop == NOTSRCCOPYINDEX) ||
                 (dwRop == SRCINVERTINDEX) || (dwRop == MERGEPAINTINDEX) ||
                 (dwRop == SRCERASEINDEX) || (dwRop == NOTSRCERASEINDEX))
            {

            srcx = pbd->lpDDSrcSurface;
            src = srcx->lpGbl;

            dwSrcCaps = srcx->ddsCaps.dwCaps;

            /* We must treat AGP source memory as system memory */
            if (dwSrcCaps & DDSCAPS_NONLOCALVIDMEM)
                {
                dwSrcCaps &= ~(DDSCAPS_VIDEOMEMORY | DDSCAPS_NONLOCALVIDMEM);
                }

            dwSrcOffset = src->fpVidMem;
            dwSrcPitch  = src->lPitch;
            dwSrcWidth  = pbd->rSrc.right  - pbd->rSrc.left;
            dwSrcHeight = pbd->rSrc.bottom - pbd->rSrc.top;
            dwColorKey  = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;

            wSrcXY.xy16.x = (short)pbd->rSrc.left;
            wSrcXY.xy16.y = (short)pbd->rSrc.top;

            if (//(srcx->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
                (src->ddpfSurface.dwFlags & DDPF_FOURCC))
                {

                if (src->ddpfSurface.dwFourCC == FOURCC_RGB0)
                    {
                    if ((src->ddpfSurface.dwRBitMask == 0x00FF0000) &&
                        (src->ddpfSurface.dwGBitMask == 0x0000FF00) &&
                        (src->ddpfSurface.dwBBitMask == 0x000000FF))
                        {
                        if (dwSrcCaps & DDSCAPS_SYSTEMMEMORY)
                            isVideo = 1;
                        goto srcRGB;
                        }
                    else
                        {
                        pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                        ppdev->NVFreeCount = freeCount;
                        return DDHAL_DRIVER_HANDLED;
                        }
                    }

                while (freeCount < 4 )
                    freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
                freeCount -= 4;

                npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;

                ppdev->NVFreeCount = freeCount;

                DDRAW_SET_SOURCE(ppdev, dwSrcOffset, dwSrcPitch);
                DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);

                freeCount = ppdev->NVFreeCount;

                switch (src->ddpfSurface.dwFourCC)
                    {
                    case FOURCC_YUY2:
                    case FOURCC_UYVY:

//                     if (dwSrcWidth & 1) {  NEC MPEG CODEC DOESN'T RESPOND TO THIS ERROR
//                           pbd->ddRVal = DDERR_INVALIDRECT;
//                           return DDHAL_DRIVER_HANDLED;
//                       }

                        if (wSrcXY.xy16.x & 1)
                            {
                            pbd->ddRVal = DDERR_INVALIDRECT;
                            ppdev->NVFreeCount = freeCount;
                            return DDHAL_DRIVER_HANDLED;
                            }

                        if (src->ddpfSurface.dwFourCC == FOURCC_UYVY)
                            {
                            isVideo = 2;
                            // Is this surface DMAable ?
                            if ((ppdev->fNvActiveFloatingContexts &
                                 NV_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                (ppdev->NvFloating0UYVYSurfaceMem == src->fpVidMem))
                                isVideo = 4;
                            else if ((ppdev->fNvActiveFloatingContexts &
                                      NV_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                     (ppdev->NvFloating1UYVYSurfaceMem == src->fpVidMem))
                                isVideo = 5;
                            }
                        else
                            { // YUY2
                            isVideo = 3;
                            // Is this surface DMAable ?
                            if ((ppdev->fNvActiveFloatingContexts &
                                 NV_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                (ppdev->NvFloating0YUYVSurfaceMem == src->fpVidMem))
                                isVideo = 6;
                            else if ((ppdev->fNvActiveFloatingContexts &
                                      NV_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY) &&
                                     (ppdev->NvFloating1YUYVSurfaceMem == src->fpVidMem))
                                isVideo = 7;
                            }
                        goto srcRGB;
                        break;
#ifdef YV12_SUPPORT
                    case FOURCC_YV12:
                        // Size has to be always * 4, to use VU subsampling
                        if ((dwSrcWidth & 3) || (dwSrcHeight & 3))
                            {
                            pbd->ddRVal = DDERR_INVALIDRECT;
                            ppdev->NVFreeCount = freeCount;
                            return DDHAL_DRIVER_HANDLED;
                            }

//                     pbd->ddRVal = nvStretchYVU12ToDev (pbd, src->ddpfSurface.dwFourCC);
                        ppdev->NVFreeCount = freeCount;
                        return DDHAL_DRIVER_HANDLED;
                        break;
#endif // #ifdef YV12_SUPPORT
                    }
                pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                ppdev->NVFreeCount = freeCount;
                return DDHAL_DRIVER_HANDLED;
                }

            srcRGB:     if (dwDDFX & DDBLTFX_MIRRORUPDOWN)
                doMirrorUpDown = 1;

            if ((dwSrcWidth != dwDstWidth) || (dwSrcHeight != dwDstHeight))
                wStretch = 1;

            if (isVideo > 1) /* YUV video */
                wStretch = 1;

            if (dwFlags & DDBLT_KEYSRCOVERRIDE)
                {

                dwColorKey = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;

                while (freeCount < 2*4)
                    freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);

                // load and enable chroma key
                freeCount -= 2*4;
                if (ppdev->dDrawSpareSubchannelObject != NV_DD_IMAGE_SOLID)
                    {
                    npDev->dDrawSpare.SetObject = NV_DD_IMAGE_SOLID;
                    ppdev->dDrawSpareSubchannelObject = NV_DD_IMAGE_SOLID;
                    }
                npDev->dDrawTransColor.SetColor =
                (dwColorKey & ppdev->physicalColorMask) | NV_ALPHA_1_32;

                }

            // screen to screen bitblt
            if (wStretch == 0)
                {
                if ((dwDstCaps & DDSCAPS_VIDEOMEMORY) &&
                    (dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                    ((dwSrcCaps & DDSCAPS_TEXTURE) == 0) &&
                    (doMirrorUpDown == 0) &&
                    (isVideo == 0))
                    {
                    ppdev->NVFreeCount = freeCount;
                    DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
                    DDRAW_SET_SOURCE(ppdev, dwSrcOffset, dwSrcPitch);
                    freeCount = ppdev->NVFreeCount;

                    while (freeCount < 16)
                        freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
                    freeCount -= 16;

                    npDev->dDrawRop.SetRop5 = dwRop;
                    npDev->dDrawBlit.ControlPointIn = wSrcXY.xy;
                    npDev->dDrawBlit.ControlPointOut = wDestXY.xy;
                    npDev->dDrawBlit.Size = wXYext.wh;

                    }
                else
                    {
                    // memory to screen blt (src may be in system memory or in video memory)
                    unsigned long temp;
                    long srcScanLength;
                    long nxtSrcScan;
                    short n = wXYext.wh16.h;
                    FAST short pxlCount;
                    unsigned long *pSrcInitBits;

                    pSrcInitBits = (unsigned long *)src->fpVidMem;
                    if (dwSrcCaps & DDSCAPS_VIDEOMEMORY)
                        pSrcInitBits += (ULONG) ppdev->pjFrameBufbase;

                    srcScanLength = src->lPitch >> 2;

                    if (doMirrorUpDown)
                        wSrcXY.xy16.y += (short)(n - 1);

                    pSrcInitBits += (wSrcXY.xy16.y * srcScanLength);

                    temp = wXYext.wh16.w;

                    nxtSrcScan = srcScanLength - temp;

                    if (doMirrorUpDown)
                        nxtSrcScan = -srcScanLength - temp;

                    pSrcInitBits += wSrcXY.xy16.x;

                    ppdev->NVFreeCount = freeCount;
                    DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
                    freeCount = ppdev->NVFreeCount;

                    while (freeCount < 4*4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
                    freeCount -= 4*4;

                    npDev->dDrawRop.SetRop5 = dwRop;

                    npDev->dDrawImageFromCpu.Point = wDestXY.xy;
                    npDev->dDrawImageFromCpu.SizeOut = wXYext.wh;
                    npDev->dDrawImageFromCpu.SizeIn = wXYext.wh;

                    {
                        FAST unsigned long *pSrcBits = (unsigned long *)pSrcInitBits;
                        FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

                        while (--n >= 0)
                            {
                            pxlCount = (short)temp;

                            while (pxlCount >= 16)
                                {
                                pxlCount -= 16;

                                while (freeCount < 64)
                                    freeCount = NvGetFreeCount(npDev, NV_DD_IMAGE);
                                freeCount -= 64;

                                tmpPxls0 = pSrcBits[0];
                                tmpPxls1 = pSrcBits[1];
                                npDev->dDrawImageFromCpu.Color[0] = tmpPxls0;
                                tmpPxls2 = pSrcBits[2];
                                npDev->dDrawImageFromCpu.Color[1] = tmpPxls1;
                                tmpPxls0 = pSrcBits[3];
                                tmpPxls1 = pSrcBits[4];
                                npDev->dDrawImageFromCpu.Color[2] = tmpPxls2;
                                npDev->dDrawImageFromCpu.Color[3] = tmpPxls0;
                                tmpPxls2 = pSrcBits[5];
                                tmpPxls0 = pSrcBits[6];
                                npDev->dDrawImageFromCpu.Color[4] = tmpPxls1;
                                npDev->dDrawImageFromCpu.Color[5] = tmpPxls2;
                                tmpPxls1 = pSrcBits[7];
                                tmpPxls2 = pSrcBits[8];
                                npDev->dDrawImageFromCpu.Color[6] = tmpPxls0;
                                npDev->dDrawImageFromCpu.Color[7] = tmpPxls1;
                                tmpPxls0 = pSrcBits[9];
                                tmpPxls1 = pSrcBits[10];
                                npDev->dDrawImageFromCpu.Color[8] = tmpPxls2;
                                npDev->dDrawImageFromCpu.Color[9] = tmpPxls0;
                                tmpPxls2 = pSrcBits[11];
                                tmpPxls0 = pSrcBits[12];
                                npDev->dDrawImageFromCpu.Color[10] = tmpPxls1;
                                npDev->dDrawImageFromCpu.Color[11] = tmpPxls2;
                                tmpPxls1 = pSrcBits[13];
                                tmpPxls2 = pSrcBits[14];
                                npDev->dDrawImageFromCpu.Color[12] = tmpPxls0;
                                npDev->dDrawImageFromCpu.Color[13] = tmpPxls1;
                                tmpPxls0 = pSrcBits[15];
                                npDev->dDrawImageFromCpu.Color[14] = tmpPxls2;
                                pSrcBits += 16;
                                npDev->dDrawImageFromCpu.Color[15] = tmpPxls0;
                                }

                            if (pxlCount > 0)
                                {
                                while (freeCount < 64)
                                    freeCount = NvGetFreeCount(npDev, NV_DD_IMAGE);

                                while (--pxlCount >= 0)
                                    {
                                    tmpPxls0 = pSrcBits[0];
                                    freeCount -= 4;
                                    pSrcBits += 1;
                                    npDev->dDrawImageFromCpu.Color[0] = tmpPxls0;
                                    }
                                }

                            pSrcBits += nxtSrcScan;
                            }
                    }
                    }
                }
            else
                { // stretch
                DWORD dwStatus;
                RECTL rDummy;
                BOOL isOverlappingVmem = FALSE;
                ULONG lpSurfTemp = 0;
                // if the source and destination overlap, then we must do this in 2 passes, first copy the source
                // image somewhere else, and then so the stretch from there.
                if ((dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                    dwSrcOffset == dwDstOffset &&
                    bIntersect((RECTL *)(&pbd->rSrc), (RECTL *)(&pbd->rDest), &rDummy))
                    {
                    ULONG tmpBmpPitch = (((dwSrcWidth * (ppdev->cBitsPerPel / 8)) + ppdev->ulSurfaceAlign) & ~ppdev->ulSurfaceAlign);
                    NVHEAP_ALLOC(dwStatus,lpSurfTemp, (DWORD)dwSrcHeight * tmpBmpPitch,  TYPE_IMAGE);
                    if (dwStatus!=0)
                    {
                        bMoveAllDfbsFromOffscreenToDibs(ppdev);
                        NVHEAP_ALLOC(dwStatus,lpSurfTemp, (DWORD)dwSrcHeight * tmpBmpPitch,  TYPE_IMAGE);
                    }

                    if (dwStatus==0)
                        {
                        isOverlappingVmem = TRUE;
                        DDRAW_SET_PRIMARY(ppdev, lpSurfTemp, tmpBmpPitch);
                        DDRAW_SET_SOURCE(ppdev, dwSrcOffset, dwSrcPitch);

                        while (freeCount < 6*4)
                            freeCount = NvGetFreeCount(npDev, 0);
                        freeCount -= 6*4;

						{
							NVCOORD     srcSize;

							srcSize.wh16.w = (unsigned short)dwSrcWidth;
							srcSize.wh16.h = (unsigned short)dwSrcHeight;

							npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;
							npDev->dDrawBlit.ControlPointIn = wSrcXY.xy;
							npDev->dDrawBlit.ControlPointOut = 0;
							npDev->dDrawBlit.Size = srcSize.wh;
						}

                        dwSrcOffset = lpSurfTemp;
                        dwSrcPitch = tmpBmpPitch;
                        wSrcXY.xy = 0;
                        wSrcXY.xy16.x = 0;
                        wSrcXY.xy16.y = 0;
                        }
                    else
                        {
                        // FIXME: vmem heap alloc failed, must do system mem alloc and copy there
                        }
                        // WARNING: carefully consider each use of src->fpVidMem and src->lPitch beyond this point
                        //          it could screw up overlapping stretchblits
                    }
                if (dwFlags & DDBLT_KEYSRCOVERRIDE)
                    { // If color key then MUST use STRETCHED IMAGE instead of SCALED IMAGE
                    NvNotification *pDmaToMemNotifier;
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
                    long deltaDstY;
                    long curDstY;
                    long lastDstY;
                    long skipSrcBytes = 0;
                    NVCOORD wSrcExt;
                    NVCOORD wDstPoint;
                    unsigned long srcStrtX;
                    BYTE srcInVideoMemory = 0;
                    BYTE bufState = 0;
                    short skipV = 0;

                    pSrcInitBits = (unsigned char *) dwSrcOffset;
                    if (dwSrcCaps & DDSCAPS_VIDEOMEMORY)
                        pSrcInitBits += (ULONG) ppdev->pjFrameBufbase;

                    srcScanLength = dwSrcPitch;

                    if (dwSrcPitch <= 1600)
                        m = 8;
                    else if (dwSrcPitch <= 3200)
                        m = 4;
                    else
                        m = 2;

                    ppdev->NVFreeCount = freeCount;
                    DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
                    freeCount = ppdev->NVFreeCount;

                    while (freeCount < 4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
                    freeCount -= 4;

                    npDev->dDrawRop.SetRop5 = dwRop;

                    /* calculate deltas in 12.20 format */
                    n = (dwDstWidth << 20) / dwSrcWidth;

                    if ((n & 0xFFFFF) == 0)
                        n = 0;
                    else
                        {
                        n >>= 20;
                        n += 1;
                        }

                    deltaX = (dwDstWidth + n) << 20;
                    deltaX /= dwSrcWidth;

                    n = (dwDstHeight << 20) / dwSrcHeight;

                    if ((n & 0xFFFFF) == 0)
                        n = 0;
                    else
                        {
                        n >>= 20;
                        n += 1;
                        }

                    deltaY = (dwDstHeight + n) << 20;
                    deltaY /= dwSrcHeight;

                    deltaDstY = (dwDstHeight << 20);
                    deltaDstY /= dwSrcHeight;

                    wSrcExt.wh16.w = (unsigned short)dwSrcWidth;
                    wSrcExt.wh16.h = (unsigned short)dwSrcHeight;

                    n = (short)dwSrcHeight;

                    if ((ULONG) (wSrcXY.xy16.y + n) > src->wHeight)
                        n = src->wHeight - wSrcXY.xy16.y;

                    nxtSrcScan = srcScanLength;

                    if (doMirrorUpDown)
                        {
                        wSrcXY.xy16.y += (short)(n - 1);
                        nxtSrcScan = -nxtSrcScan;
                        skipSrcBytes = -skipSrcBytes;
                        }

                    if ((dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                        ((dwSrcCaps & DDSCAPS_TEXTURE) == 0)
                        || isOverlappingVmem)
                        {
                        srcInVideoMemory = 1;
                        }
                    else
                        {
                        pSaveSrcBits = (unsigned char *)(pSrcInitBits +
                                                         (wSrcXY.xy16.y * srcScanLength));
                        pSaveSrcBits += (wSrcXY.xy16.x << 2);
                        pSrcCurScan = pSaveSrcBits;
                        }

                    if (srcInVideoMemory)
                        {
                        pDmaToMemNotifier = (NvNotification *)ppdev->Notifiers->DmaToMem;
                        pScanlineBuffer = (unsigned char *)ppdev->NvScanlineBufferFlat;

                        wSrcExt.wh16.w = (unsigned short)dwSrcWidth;
                        wSrcExt.wh16.h = m;

                        srcStrtX = wSrcXY.xy16.x;

                        while (freeCount < 4)
                            freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                        freeCount -= 4;

                        npDev->subchannel[NV_DD_SPARE].SetObject =
                        NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;
                        ppdev->dDrawSpareSubchannelObject =
                        NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;

                        // wait for any pending notification operation to finish
                        while ((volatile)pDmaToMemNotifier[NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV_IN_PROGRESS);

                        pDmaToMemNotifier[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV_IN_PROGRESS;

                        while (freeCount < 32)
                            freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                        freeCount -= 32;

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
                        dwSrcOffset +
                        ((dwSrcPitch + skipSrcBytes) * (unsigned long)wSrcXY.xy16.y) +
                        ((unsigned long)wSrcXY.xy16.x << 2);

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
                        (srcStrtX << 2);

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
                        (dwSrcPitch + skipSrcBytes);

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
                        dwSrcPitch;

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
                        (unsigned long)(wSrcExt.wh16.w << 2);

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
                        m;

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
                        1 | (1 << 8);
                        //NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1;

                        npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify =
                        0;

                        // wait for Dma operation to finish
                        while ((volatile)pDmaToMemNotifier[NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV_IN_PROGRESS);

                        if (doMirrorUpDown)
                            wSrcXY.xy16.y -= (m + skipV);
                        else
                            wSrcXY.xy16.y += (m + skipV);
                        }

                    /* Convert to 12.4 format */
                    wDstPoint.xy16.x = (wDestXY.xy16.x << 4);
                    wDstPoint.xy16.y = (wDestXY.xy16.y << 4);

                    /* convert to 16.16 */
                    lastDstY = (wDestXY.xy16.y + wXYext.wh16.h) << 16;

                    /* convert to 16.16 */
                    curDstY = (wDestXY.xy16.y << 16);

                    while (freeCount < 7*4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                    freeCount -= 7*4;

                    npDev->dDrawStretch.SetObject = NV_DD_STRETCHED_IMAGE_FROM_CPU;
                    ppdev->dDrawSpareSubchannelObject = NV_DD_STRETCHED_IMAGE_FROM_CPU;

                    npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipPoint =
                    wDestXY.xy;

                    npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipSize =
                    wXYext.wh;

                    npDev->dDrawStretch.nv4StretchedImageFromCpu.SizeIn = wSrcExt.wh;

                    npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDxDu = deltaX;

                    npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDyDv = deltaY;

                    npDev->dDrawStretch.nv4StretchedImageFromCpu.Point12d4 = wDstPoint.xy;

                    while ((n > 0) && (curDstY < lastDstY))
                        {
                        FAST unsigned long *pSrcBits;
                        FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

                        if (srcInVideoMemory)
                            {
                            if (bufState == 0)
                                pSrcBits = (unsigned long *)(pScanlineBuffer +
                                                             (srcStrtX << 2));
                            else
                                pSrcBits = (unsigned long *)(pScanlineBuffer +
                                                             (NV_DMA_SCANLINE_BUF_SIZE >> 1) +
                                                             (srcStrtX << 2));

                            pSrcCurScan = (unsigned char *)pSrcBits;

                            if (m > 0)
                                {
                                pDmaToMemNotifier[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV_IN_PROGRESS;

                                while (freeCount < 9*4)
                                    freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                                freeCount -= 9*4;
                                npDev->subchannel[NV_DD_SPARE].SetObject = NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;
                                ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;

                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
                                dwSrcOffset +
                                ((dwSrcPitch + skipSrcBytes) * (unsigned long)wSrcXY.xy16.y) +
                                ((unsigned long)wSrcXY.xy16.x << 2);

                                if (bufState == 0)
                                    npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
                                    (NV_DMA_SCANLINE_BUF_SIZE >> 1) +
                                    (srcStrtX << 2);
                                else
                                    npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
                                    (srcStrtX << 2);

                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
                                (dwSrcPitch + skipSrcBytes);

                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
                                dwSrcPitch;

                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
                                (unsigned long)(wSrcExt.wh16.w << 2);

                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
                                m;

                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
                                1 | (1 << 8);
                                //NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1;

                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify =                                0;

                                bufState ^= 1;

                                if (doMirrorUpDown)
                                    wSrcXY.xy16.y -= (m + skipV);
                                else
                                    wSrcXY.xy16.y += (m + skipV);

                                while (freeCount < 7*4)
                                    freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                                freeCount -= 7*4;
                                npDev->dDrawStretch.SetObject = NV_DD_STRETCHED_IMAGE_FROM_CPU;
                                ppdev->dDrawSpareSubchannelObject = NV_DD_STRETCHED_IMAGE_FROM_CPU;

                                npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipPoint =
                                wDestXY.xy;
                                npDev->dDrawStretch.nv4StretchedImageFromCpu.ClipSize =
                                wXYext.wh;
                                npDev->dDrawStretch.nv4StretchedImageFromCpu.SizeIn =
                                wSrcExt.wh;
                                npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDxDu =
                                deltaX;
                                npDev->dDrawStretch.nv4StretchedImageFromCpu.DeltaDyDv =
                                deltaY;
                                npDev->dDrawStretch.nv4StretchedImageFromCpu.Point12d4 =
                                wDstPoint.xy;
                                }
                            }
                        else
                            { // source in system memory
                            pSrcBits = (unsigned long *)pSrcCurScan;
                            pSrcCurScan += (nxtSrcScan + skipSrcBytes);
                            n--;
                            }

                        doNxtSrcScan:          pxlCount = (short)dwSrcWidth;

                        while (pxlCount >= 16)
                            {

                            pxlCount -= 16;

                            while (freeCount < 64)
                                freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                            freeCount -= 64;

                            tmpPxls0 = pSrcBits[0];
                            tmpPxls1 = pSrcBits[1];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[0] = tmpPxls0;
                            tmpPxls2 = pSrcBits[2];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[1] = tmpPxls1;
                            tmpPxls0 = pSrcBits[3];
                            tmpPxls1 = pSrcBits[4];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[2] = tmpPxls2;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[3] = tmpPxls0;
                            tmpPxls2 = pSrcBits[5];
                            tmpPxls0 = pSrcBits[6];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[4] = tmpPxls1;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[5] = tmpPxls2;
                            tmpPxls1 = pSrcBits[7];
                            tmpPxls2 = pSrcBits[8];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[6] = tmpPxls0;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[7] = tmpPxls1;
                            tmpPxls0 = pSrcBits[9];
                            tmpPxls1 = pSrcBits[10];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[8] = tmpPxls2;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[9] = tmpPxls0;
                            tmpPxls2 = pSrcBits[11];
                            tmpPxls0 = pSrcBits[12];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[10] = tmpPxls1;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[11] = tmpPxls2;
                            tmpPxls1 = pSrcBits[13];
                            tmpPxls2 = pSrcBits[14];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[12] = tmpPxls0;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[13] = tmpPxls1;
                            tmpPxls0 = pSrcBits[15];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[14] = tmpPxls2;
                            pSrcBits += 16;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[15] = tmpPxls0;
                            }

                        while (pxlCount >= 8)
                            {

                            pxlCount -= 8;

                            while (freeCount < 32)
                                freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                            freeCount -= 32;

                            tmpPxls0 = pSrcBits[0];
                            tmpPxls1 = pSrcBits[1];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[0] = tmpPxls0;
                            tmpPxls2 = pSrcBits[2];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[1] = tmpPxls1;
                            tmpPxls0 = pSrcBits[3];
                            tmpPxls1 = pSrcBits[4];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[2] = tmpPxls2;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[3] = tmpPxls0;
                            tmpPxls2 = pSrcBits[5];
                            tmpPxls0 = pSrcBits[6];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[4] = tmpPxls1;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[5] = tmpPxls2;
                            tmpPxls1 = pSrcBits[7];
                            pSrcBits += 8;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[6] = tmpPxls0;
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[7] = tmpPxls1;
                            }

                        while (freeCount < 32)
                            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                        freeCount -= 32;

                        while (--pxlCount >= 0)
                            {
                            tmpPxls0 = pSrcBits[0];
                            npDev->dDrawStretch.nv4StretchedImageFromCpu.Color[0] = tmpPxls0;
                            pSrcBits += 1;
                            }

                        if (srcInVideoMemory)
                            {
                            if (--m > 0)
                                {
                                pSrcCurScan += dwSrcPitch;

                                pSrcBits = (unsigned long *)pSrcCurScan;

                                goto doNxtSrcScan;
                                }
                            else
                                {
                                if (dwSrcPitch <= 1600)
                                    {
                                    m = 8;
                                    curDstY += (deltaDstY >> 1);
                                    }
                                else if (dwSrcPitch <= 3200)
                                    {
                                    m = 4;
                                    curDstY += (deltaDstY >> 2);
                                    }
                                else
                                    {
                                    m = 2;
                                    curDstY += (deltaDstY >> 3);
                                    }
                                /* convert 16.16 to 12.4 */
                                wDstPoint.xy16.y = (short)(curDstY >> 12);

                                // wait for DMA operation to finish
                                while ((volatile)pDmaToMemNotifier[NV076_NOTIFIERS_NOTIFY].status == NV_IN_PROGRESS);
                                }
                            }
                        }
                    /* restore default object */
                    while (freeCount < 4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                    freeCount -= 4;

                    npDev->dDrawStretch.SetObject = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
                    ppdev->dDrawSpareSubchannelObject = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;

                    }
                else
                    { // OK to use SCALED IMAGE
                    NvNotification *pDmaToMemNotifier;
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
                    unsigned long dstDeltaScans;
                    unsigned long dstDeltaX;
                    unsigned long dstDeltaY;
                    long srcInitFract;
                    long srcStartFract;
                    long srcCurFract;
                    long srcDeltaFract;
                    long srcDeltaScans;
                    long xStretch;
                    long yStretch;
                    long srcBufferPitch;
                    long skipSrcBytes = 0;
                    long dstStartY;
                    long dstCurY;
                    long curV;
                    long skipV = 0;
                    long m;
                    long srcFinalHeight;
                    NVCOORD wSrcPoint;
                    NVCOORD wSrcExt;
                    NVCOORD wDstPoint;
                    NVCOORD wDstExt;
                    BYTE srcInVideoMemory = 0;
                    BYTE srcTooWide = 0;
                    short curBuffer;

                    pSrcInitBits = (unsigned char *) dwSrcOffset;
                    if (dwSrcCaps & DDSCAPS_VIDEOMEMORY)
                        pSrcInitBits += (ULONG) ppdev->pjFrameBufbase;

                    if (isVideo < 2)
                        srcScanLength = dwSrcPitch;
                    else
                        srcScanLength = dwSrcPitch >> 1;

                    ppdev->NVFreeCount = freeCount;
                    DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
                    freeCount = ppdev->NVFreeCount;

                    while (freeCount < 4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
                    freeCount -= 4;

                    npDev->dDrawRop.SetRop5 = dwRop;

                    if (isVideo < 2)
                        srcBufferPitch = dwSrcWidth << 2;
                    else
                        srcBufferPitch = dwSrcWidth << 1;

                    srcDeltaScans = (NV_DMA_SCANLINE_BUF_SIZE >> 1) -   /* allow for 2 extra src scanlines */
                                    - (srcBufferPitch << 1);
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

                    /* check for divide by 0 */
                    if ((dwDstWidth < 1) || (dwDstHeight < 1))
                        {
                        pbd->ddRVal = DDERR_INVALIDRECT;
                        ppdev->NVFreeCount = freeCount;
                        return DDHAL_DRIVER_HANDLED;
                        }

                    n = (dwSrcWidth << 20) / dwDstWidth;

                    if (n == 0x100000)
                        n = 0;
                    else
                        {
                        n >>= 20;
                        n += 1;
                        }

                    srcDeltaX = dwSrcWidth << 20;
                    srcDeltaX /= (dwDstWidth + n);

                    n = (dwSrcHeight << 20) / dwDstHeight;

                    if (n == 0x100000)
                        n = 0;
                    else
                        {
                        n >>= 20;
                        n += 1;
                        }

                    srcDeltaY = dwSrcHeight << 20;
                    srcDeltaY /= (dwDstHeight + n);

                    /* Algorithm can't handle a scale factor that is too large */
                    if (srcDeltaY > 0x40000000)
                        {
                        pbd->ddRVal = DDERR_INVALIDRECT;
                        ppdev->NVFreeCount = freeCount;
                        return DDHAL_DRIVER_HANDLED;
                        }

                    /* If shrinking in vertical direction then adjust to skip src scanlines */
                    if (srcDeltaY >= (srcDeltaScans << 20))
                        { /* limit Y delta range to acceptable range */
                        skipSrcBytes = srcScanLength;
                        skipV = 1;
                        while (srcDeltaY >= (srcDeltaScans << 20))
                            {
                            srcDeltaY >>= 1;
                            skipSrcBytes <<= 1;
                            skipV <<= 1;
                            }
                        skipSrcBytes -= srcScanLength;
                        skipV -= 1;
                        }

                    /* Algorithm can't handle scale factor that is too small */
                    if ((srcDeltaY & 0xFFFFFC00) == 0)
                        {
                        pbd->ddRVal = DDERR_INVALIDRECT;
                        ppdev->NVFreeCount = freeCount;
                        return DDHAL_DRIVER_HANDLED;
                        }

                    yStretch = 0x40000000 / ((srcDeltaY + 0x00000200) >> 10);

                    dstDeltaY = srcDeltaScans * yStretch;  // how many dst scanlines will srcDeltaScans scanlines generate

                    dstDeltaScans = dstDeltaY >> 20;

                    if (dstDeltaScans == 0)
                        { /* if shrinking too much then can't handle */
                        pbd->ddRVal = DDERR_INVALIDRECT;
                        ppdev->NVFreeCount = freeCount;
                        return DDHAL_DRIVER_HANDLED;
                        }

                    if (wSrcXY.xy16.y + dwSrcHeight > src->wHeight)
                        dwSrcHeight = src->wHeight - wSrcXY.xy16.y;

                    if (isVideo < 2)
                        nxtSrcScan = (srcScanLength >> 2);
                    else
                        nxtSrcScan = (srcScanLength >> 1);

                    if (doMirrorUpDown)
                        {
                        wSrcXY.xy16.y += (short)(dwSrcHeight - 1);
                        srcDeltaY =- srcDeltaY;
                        nxtSrcScan = -nxtSrcScan;
                        skipSrcBytes = -skipSrcBytes;
                        }

                    pDmaToMemNotifier = (NvNotification *)ppdev->Notifiers->DmaToMem;
                    pScanlineBuffer = (unsigned char *)ppdev->NvScanlineBufferFlat;

                    srcDeltaFract = (dstDeltaScans << 20) / srcDeltaScans;

                    srcDeltaFract &= 0xFFFFF;

                    wSrcPoint.xy = 0;

                    /* Calculate initial source fractional X */
                    if ((srcDeltaX > 0x40000000) || ((srcDeltaX & 0xFFFFFC00) == 0))
                        {
                        pbd->ddRVal = DDERR_INVALIDRECT;
                        ppdev->NVFreeCount = freeCount;
                        return DDHAL_DRIVER_HANDLED;
                        }

                    /* Do this math in 16.16 precision to avoid 12. overflow */
                    dstDeltaX = ((long)wSrcXY.xy16.x << 16) / (srcDeltaX >> 4);

                    /* Do this math in 16.16 precision to avoid 12. overflow */
                    srcInitFract = dstDeltaX * (srcDeltaX >> 4);

                    /* Do this math in 16.16 precision to avoid 12. overflow */
                    while (srcInitFract >> 16 < wSrcXY.xy16.x)
                        srcInitFract += (srcDeltaX >> 4);

                    /* Convert to 12.4 format */
                    wSrcPoint.xy16.x = (short)(srcInitFract & 0xFFFF) >> 12;

                    // DMAable ?
                    if ((isVideo > 3) || ((dwSrcCaps & DDSCAPS_VIDEOMEMORY) &&
                                          ((dwSrcCaps & DDSCAPS_TEXTURE) == 0) ||
                                          isOverlappingVmem))
                        {

                        /* Calculate initial source fractional Y */
                        /* Do this math in 16.16 precision to avoid 12. overflow */
                        dstDeltaY = ((long)wSrcXY.xy16.y << 16) / (srcDeltaY >> 4);

                        /* Do this math in 16.16 precision to avoid 12. overflow */
                        srcInitFract = dstDeltaY * (srcDeltaY >> 4);

                        /* Do this math in 16.16 precision to avoid 12. overflow */
                        while (srcInitFract >> 16 < wSrcXY.xy16.y)
                            srcInitFract += (srcDeltaY >> 4);

                        /* Convert result back to 12.20 precision */
                        srcInitFract <<= 4;

                        srcInitFract &= 0xFFFFF;

                        srcCurFract = srcInitFract;

                        if (isVideo < 4)
                            {
                            srcInVideoMemory = 1;

                            wSrcPoint.xy16.y += (short)srcCurFract >> 16;

                            if (doMirrorUpDown)
                                {
                                pSaveSrcBits = (unsigned long *)
                                               (pSrcInitBits - ppdev->pjFrameBufbase);
                                wSrcPoint.xy16.y += ((short)(dwSrcHeight - 1) << 4);
                                }
                            else
                                {
                                if (isVideo < 2)
                                    pSaveSrcBits = (unsigned long *)
                                                   ((pSrcInitBits - ppdev->pjFrameBufbase) +
                                                    (wSrcXY.xy16.y * srcScanLength));
                                else
                                    pSaveSrcBits = (unsigned long *)
                                                   ((pSrcInitBits - ppdev->pjFrameBufbase) +
                                                    (wSrcXY.xy16.y * (srcScanLength << 1)));
                                }
                            }
                        else
                            {

                            wSrcPoint.xy16.y += (short)srcCurFract >> 16;

                            if (doMirrorUpDown)
                                {
                                pSaveSrcBits = 0;
                                wSrcPoint.xy16.y += ((short)(dwSrcHeight - 1) << 4);
                                }
                            else
                                {
                                pSaveSrcBits = (unsigned long *)
                                               (wSrcXY.xy16.y * (srcScanLength << 1));
                                }
                            }

                        if (isVideo < 2)
                            pSaveSrcBits += wSrcXY.xy16.x;
                        else
                            pSaveSrcBits += (wSrcXY.xy16.x >> 1);

                        while (freeCount < 4)
                            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                        freeCount -= 4;

                        if (isVideo < 2)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY;
                            }
                        else if (isVideo == 2)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY;
                            }
                        else if (isVideo == 3)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY;
                            }
                        else if (isVideo == 4)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                            }
                        else if (isVideo == 5)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                            }
                        else if (isVideo == 6)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
                            }
                        else if (isVideo == 7)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
                            }

                        }
                    else
                        {
                        if (isVideo < 2)
                            {
                            pSaveSrcBits = (unsigned long *)(pSrcInitBits +
                                                             (wSrcXY.xy16.y * srcScanLength));
                            pSaveSrcBits += wSrcXY.xy16.x;
                            }
                        else
                            {
                            pSaveSrcBits = (unsigned long *)(pSrcInitBits +
                                                             (wSrcXY.xy16.y * (srcScanLength << 1)));
                            pSaveSrcBits += (wSrcXY.xy16.x >> 1);
                            }

                        pSrcCurScan = pSaveSrcBits;

                        while (freeCount < 1*4)
                            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                        freeCount -= 1*4;

                        if (isVideo == 2)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY;
                            }
                        else if (isVideo == 3)
                            {
                            npDev->subchannel[NV_DD_STRETCH].SetObject =
                            NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY;
                            }
                        }

                    while (freeCount < 4*4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                    freeCount -= 4*4;

                    pSaveSrcInitBits = (unsigned char *)pSaveSrcBits;

                    npDev->dDrawStretch.nv4ScaledImageFromMemory.ClipPoint =
                    wDestXY.xy;

                    npDev->dDrawStretch.nv4ScaledImageFromMemory.ClipSize =
                    wXYext.wh;

                    npDev->dDrawStretch.nv4ScaledImageFromMemory.DeltaDuDx = srcDeltaX;
                    npDev->dDrawStretch.nv4ScaledImageFromMemory.DeltaDvDy = srcDeltaY;


                    wDstExt.wh = wXYext.wh;

                    /* NV3 hardware can only handle a maximum source width of 1024 */
                    if (dwSrcWidth > 1024)
                        {
                        srcTooWide = 1;
                        xStretch = 0x40000000 / (srcDeltaX >> 10);
                        dstDeltaX = (xStretch * 1024) >> 20;
                        wDstExt.wh16.w = (unsigned short)dstDeltaX;
                        srcBufferPitch = 4096;
                        }

                    // wait for any pending notification operation to finish
                    while ((volatile)pDmaToMemNotifier[NV077_NOTIFIERS_NOTIFY].status == NV_IN_PROGRESS);

                    // Let the world know we are doing DMA operations
                    ppdev->NvDmaFromMemSurfaceMem = dwSrcOffset;
                    if (dwSrcCaps & DDSCAPS_VIDEOMEMORY)
                        ppdev->NvDmaFromMemSurfaceMem += (ULONG) ppdev->pjFrameBufbase;


                    oneMorePass:       n = dwDstHeight;

                    /* Calculate initial source fractional Y */
                    /* Do this math in 16.16 precision to avoid 12. overflow */
                    dstDeltaY = ((long)wSrcXY.xy16.y << 16) / (srcDeltaY >> 4);

                    /* Do this math in 16.16 precision to avoid 12. overflow */
                    srcInitFract = dstDeltaY * (srcDeltaY >> 4);

                    /* Do this math in 16.16 precision to avoid 12. overflow */
                    while (srcInitFract >> 16 < wSrcXY.xy16.y)
                        srcInitFract += (srcDeltaY >> 4);

                    /* Convert result back to 12.20 precision */
                    srcInitFract <<= 4;

                    srcInitFract &= 0xFFFFF;

                    srcCurFract = srcInitFract;

                    curV = 0;

                    wDstPoint.xy = wDestXY.xy;

                    dstStartY = dstCurY = wDestXY.xy16.y;

                    curBuffer = 0;

                    if ((srcInVideoMemory) || (isVideo > 3))
                        { // DMAable ?

                        // wait for any pending notification operation to finish
                        while ((volatile)pDmaToMemNotifier[NV077_NOTIFIERS_NOTIFY].status == NV_IN_PROGRESS);

                        pDmaToMemNotifier[NV077_NOTIFIERS_NOTIFY].status = NV_IN_PROGRESS;

                        while (freeCount < 28)
                            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                        freeCount -= 28;

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageOutPoint =
                        wDstPoint.xy;
                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageOutSize =
                        wDstExt.wh;

                        if ((dwSrcWidth < 16))
                            { // workaround for NV5 hardware bug
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInSize =
                            asmMergeCoords((unsigned short)((dwSrcPitch + 1) & ~1), dwSrcHeight);
                            }
                        else
                            {
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInSize =
                            asmMergeCoords((unsigned short) ((dwSrcWidth + 1) & ~1), dwSrcHeight);
                            }

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInFormat =
                            (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |
                            (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                            dwSrcPitch;


                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInOffset =
                        (unsigned long)pSaveSrcBits;
                        npDev->dDrawStretch.nv4ScaledImageFromMemory.Notify =
                        0;
                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInPoint =
                        wSrcPoint.xy;

                        n = 0;
                        }

                    while (n >= dstDeltaScans)
                        {
                        FAST unsigned long *pSrcBits;

                        n -= dstDeltaScans;
                        srcStartFract = srcCurFract;
                        srcCurFract += srcDeltaFract;
                        if (curBuffer == 0)
                            npPixelBuf = (unsigned char *)pScanlineBuffer;
                        else
                            npPixelBuf = (unsigned char *)(pScanlineBuffer +
                                                           (NV_DMA_SCANLINE_BUF_SIZE >> 1));

                        /* Must recalculate on each pass since src scanlines may be used more than once */
                        pSrcBits = (unsigned long *)(pSaveSrcInitBits +
                                                     (curV * (nxtSrcScan << 2)));

                        pSrcCurScan = pSrcBits;

                        m = srcDeltaScans + (srcCurFract >> 20) + 1;

                        if (skipV)
                            {
                            if ((long)dwSrcHeight - curV < (m * (skipV + 1)))
                                m = ((long)dwSrcHeight - curV) / (skipV + 1);
                            }
                        else
                            {
                            if ((long)dwSrcHeight - curV < m)
                                m = (long)dwSrcHeight - curV;
                            }

                        /* Copy and convert a block of source pixels to transfer buffer */
                        while (--m >= 0)
                            {
                            if (pSrcBits >= (unsigned long *)pSrcInitBits)
                                {
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
                            curV += skipV;
                            }

                        /* wait for Dma operation to finish */
                        while (pDmaToMemNotifier[NV077_NOTIFIERS_NOTIFY].status == NV_IN_PROGRESS);

                        pDmaToMemNotifier[NV077_NOTIFIERS_NOTIFY].status = NV_IN_PROGRESS;

                        wDstPoint.xy16.y = (short)dstCurY;
                        wDstExt.wh16.h = (unsigned short)dstDeltaScans;
                        wSrcPoint.xy16.y = (short)srcStartFract >> 16; /* convert V to 12.4 */
                        wSrcExt.wh = srcDeltaScans + (srcCurFract >> 20) + 1;
                        wSrcExt.wh <<= 16;    /* shift V extent into place */
                        wSrcExt.wh |= (unsigned short) ((dwSrcWidth + 1) & ~1);

                        while (freeCount < 28)
                            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                        freeCount -= 28;

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageOutPoint =
                        wDstPoint.xy;
                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageOutSize =
                        wDstExt.wh;
                        if (dwSrcWidth < 16)
                            {
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInSize =
                                (wSrcExt.wh & 0xffff0000) | dwSrcPitch;
                            }
                        else
                            {
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInSize =
                            wSrcExt.wh;
                            }

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInFormat =
                            (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |
                            (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                            srcBufferPitch;

                        if (curBuffer == 0)
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInOffset =
                            0;
                        else
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInOffset =
                            (NV_DMA_SCANLINE_BUF_SIZE >> 1);

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.Notify =
                        0;
                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInPoint =
                        wSrcPoint.xy;

                        curBuffer ^= 1;
                        dstCurY += dstDeltaScans;

                        srcCurFract = ((dstCurY - dstStartY) * srcDeltaY) + srcInitFract;

                        curV = srcCurFract >> 20;
                        srcCurFract &= 0xFFFFF;

                        curV += (curV - 1) * skipV;
                        if (curV > (long)dwSrcHeight - 1)
                            curV = (long)dwSrcHeight - 1;
                        }

                    if (n > 0)
                        {
                        FAST unsigned long *pSrcBits;

                        srcStartFract = srcCurFract;
                        srcCurFract += srcDeltaFract;
                        if (curBuffer == 0)
                            npPixelBuf = (unsigned char *)pScanlineBuffer;
                        else
                            npPixelBuf = (unsigned char *)(pScanlineBuffer +
                                                           (NV_DMA_SCANLINE_BUF_SIZE >> 1));

                        /* Must recalculate since src scanlines may be used more than once */
                        pSrcBits = (unsigned long *)(pSaveSrcInitBits +
                                                     (curV * (nxtSrcScan << 2)));

                        pSrcCurScan = pSrcBits;

                        m = srcDeltaScans + (srcCurFract >> 20) + 1;

                        srcFinalHeight = m;

                        if (skipV)
                            {
                            if ((long)dwSrcHeight - curV < (m * (skipV + 1)))
                                srcFinalHeight = ((long)dwSrcHeight - curV) / (skipV + 1);
                            }
                        else
                            {
                            if ((long)dwSrcHeight - curV < m)
                                srcFinalHeight = (long)dwSrcHeight - curV;
                            }

                        /* Copy and convert a block of source pixels to transfer buffer */
                        while (--m >= 0)
                            {
                            if (pSrcBits >= (unsigned long *)pSrcInitBits)
                                {
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
                            if (--srcFinalHeight > 0)
                                pSrcCurScan += (nxtSrcScan + skipSrcBytes);
                            pSrcBits = (unsigned long *)pSrcCurScan;
                            curV += skipV;
                            }

                        /* wait for Dma operation to finish */
                        while (pDmaToMemNotifier[NV077_NOTIFIERS_NOTIFY].status == NV_IN_PROGRESS);

                        pDmaToMemNotifier[NV077_NOTIFIERS_NOTIFY].status = NV_IN_PROGRESS;

                        wDstPoint.xy16.y = (short)dstCurY;
                        wDstExt.wh16.h = (unsigned short)n;
                        wSrcPoint.xy16.y = (short)srcStartFract >> 16; /* convert V to 12.4 */
                        wSrcExt.wh = srcDeltaScans + (srcCurFract >> 20) + 1;
                        wSrcExt.wh <<= 16;    /* shift V extent into place */
                        wSrcExt.wh |= (unsigned short)((dwSrcWidth + 1) & ~1);

                        while (freeCount < 28)
                            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                        freeCount -= 28;

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageOutPoint =
                        wDstPoint.xy;
                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageOutSize =
                        wDstExt.wh;

                        // NV5 hw bug workaround
                        if (dwSrcWidth < 16)
                            {
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInSize =
                                (wSrcExt.wh & 0xffff0000) | dwSrcPitch;
                            }
                        else
                            {
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInSize =
                            wSrcExt.wh;
                            }

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInFormat =
                            (NV077_IMAGE_IN_FORMAT_INTERPOLATOR_FOH << 24) |
                            (NV077_IMAGE_IN_FORMAT_ORIGIN_CENTER << 16) |
                            srcBufferPitch;

                        if (curBuffer == 0)
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInOffset =
                            0;
                        else
                            npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInOffset =
                            (NV_DMA_SCANLINE_BUF_SIZE >> 1);

                        npDev->dDrawStretch.nv4ScaledImageFromMemory.Notify =
                        0;
                        npDev->dDrawStretch.nv4ScaledImageFromMemory.ImageInPoint =
                        wSrcPoint.xy;
                        }

                    if (srcTooWide)
                        {
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
                        goto oneMorePass;
                        }

                    if ((srcInVideoMemory) || (isVideo > 1))
                        {

                        while (freeCount < 4)
                            freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                        freeCount -= 4;

                        npDev->subchannel[NV_DD_STRETCH].SetObject =
                        NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
                        ppdev->dDrawSpareSubchannelObject = NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY;
                        }
                    }
                    if (isOverlappingVmem && lpSurfTemp)
                        {
                        NVHEAP_FREE(lpSurfTemp);
                        }
                }

            if (dwFlags & DDBLT_KEYSRCOVERRIDE)
                {
                /* disable chroma key */
                while (freeCount < 2*4)
                    freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                freeCount -= 2*4;
                if (ppdev->dDrawSpareSubchannelObject != NV_DD_IMAGE_SOLID)
                    {
                    npDev->dDrawSpare.SetObject = NV_DD_IMAGE_SOLID;
                    ppdev->dDrawSpareSubchannelObject = NV_DD_IMAGE_SOLID;
                    }
                npDev->dDrawTransColor.SetColor = 0;
                }
            }
        else
            {
            DPF("BLT     blt not handled by driver");
            ppdev->NVFreeCount = freeCount;
            return DDHAL_DRIVER_NOTHANDLED;
            }
        }
    else if (dwFlags & DDBLT_COLORFILL)
        {
        dwFillColor = (pbd->bltFX.dwFillColor & ppdev->physicalColorMask) | ppdev->AlphaEnableValue;

        ppdev->NVFreeCount = freeCount;
        DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
        freeCount = ppdev->NVFreeCount;

        while (freeCount < 16)
            freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
        freeCount -= 16;

        npDev->dDrawRop.SetRop5 = SRCCOPYINDEX;
        npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.Color1A = dwFillColor;

        npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.UnclippedRectangle[0].point =
        asmMergeCoords(wDestXY.xy16.y, wDestXY.xy16.x);

        npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.UnclippedRectangle[0].size =
        asmMergeCoords(wXYext.wh16.h, wXYext.wh16.w);

        }
    else if (dwFlags & DDBLT_ZBUFFER)
        {
        /* Hardware Z buffer fills not currently supported when in 32bpp mode */
        DPF("HARDWARE Z BUFFER BLT FAILED");
        ppdev->NVFreeCount = freeCount;
        return DDHAL_DRIVER_NOTHANDLED;
        }
    else
        {
        /* We don't handle any other type of BLT operation */
        ppdev->NVFreeCount = freeCount;
        return DDHAL_DRIVER_NOTHANDLED;
        }

    ppdev->NVFreeCount = freeCount;
    pbd->ddRVal = DD_OK;
    return DDHAL_DRIVER_HANDLED;

    } /* Nv4Blt32 */
