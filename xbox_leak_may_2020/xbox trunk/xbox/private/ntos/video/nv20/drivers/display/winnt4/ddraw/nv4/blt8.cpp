/*==========================================================================;
 *
 *  Copyright (C) 1995,1997 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       Nv4Blt8.c
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
 * Nv4Blt8
 *
 * Blt function for 8bpp surfaces.
 */
DWORD __stdcall Nv4Blt8( PDD_BLTDATA pbd )
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

    PDEV   *ppdev;

    ppdev = (PDEV *) pbd->lpDD->dhpdev;
    dstx = pbd->lpDDDestSurface;
    dst = dstx->lpGbl;
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
     * ColorKey ALWAYS comes in BLTFX.   You don't have to look it up in
     * the surface.
     */

    dwFlags   = pbd->dwFlags;
    dwDstCaps = dstx->ddsCaps.dwCaps;

    if (dwFlags & DDBLT_DDFX)
        dwDDFX = pbd->bltFX.dwDDFX;

//    if (dwFlags & DDOVER_ANYALPHA) {
//        DPF("OVERLAY ALPHA FAILED");
//        pbd->ddRVal = DDERR_NOOVERLAYHW;
//        return DDHAL_DRIVER_HANDLED;
//    }

//    if (dwFlags & DDBLT_ANYALPHA) {
//        DPF("ALPHA ON 8BPP SURFACE FAILED");
//        pbd->ddRVal = DDERR_NOALPHAHW;
//        return DDHAL_DRIVER_HANDLED;
//    }

    if (dwFlags & (DDBLT_KEYDEST | DDBLT_KEYDESTOVERRIDE))
        {
        DPF("DEST COLOR KEY FAILED");
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

    npDev = (Nv3ChannelPio *) ppdev->pjMmBase;

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
    if (dwFlags & DDBLT_ASYNC )
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

            while (freeCount < 7*4)
                freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
            freeCount -= 7*4;

            npDev->dDrawRop.SetRop5 = dwRop;
            npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetSource =
                dwDstOffset;

            npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetOffsetDestin =
                dwDstOffset;

            npDev->subchannel[NV_DD_PRIMARY].nv4ContextSurfaces2D.SetPitch = dwDstPitch;

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

            npDev->dDrawRop.SetRop5 = dwRop;

            npDev->subchannel[NV_DD_RECT_AND_TEXT].Nv04GdiRectangleText.Color1A = dwFillColor;

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

            /* We don't handle FOURCC_RGB0 format in 8bpp mode */
            if (//(srcx->dwFlags & DDRAWISURF_HASPIXELFORMAT) &&
                (src->ddpfSurface.dwFlags & DDPF_FOURCC))
                {

                if (src->ddpfSurface.dwFourCC == FOURCC_RGB0)
                    {
                    if ((src->ddpfSurface.dwRBitMask == 0) &&
                        (src->ddpfSurface.dwGBitMask == 0) &&
                        (src->ddpfSurface.dwBBitMask == 0))
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
                pbd->ddRVal = DDERR_UNSUPPORTEDFORMAT;
                ppdev->NVFreeCount = freeCount;
                return DDHAL_DRIVER_HANDLED;
                }

            srcRGB:     if (dwDDFX & DDBLTFX_MIRRORUPDOWN)
                doMirrorUpDown = 1;

            if ((dwSrcWidth != dwDstWidth) || (dwSrcHeight != dwDstHeight))
                {
                wStretch = 1;
                }

            if (dwFlags & DDBLT_KEYSRCOVERRIDE)
                {

                dwColorKey = pbd->bltFX.ddckSrcColorkey.dwColorSpaceLowValue;

                while (freeCount < 2*4)
                    freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                freeCount -= 2*4;

                if (ppdev->dDrawSpareSubchannelObject != NV_DD_IMAGE_SOLID)
                    {
                    ppdev->dDrawSpareSubchannelObject = NV_DD_IMAGE_SOLID;
                    npDev->dDrawSpare.SetObject = NV_DD_IMAGE_SOLID;
                    }

                // load and enable chroma key
                npDev->dDrawTransColor.SetColor =
                (dwColorKey & ppdev->physicalColorMask) | NV_ALPHA_1_08;
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
                    while (freeCount < 4*4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
                    freeCount -= 4*4;

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
                    unsigned short srcOffset;
                    unsigned char *pSrcInitBits;

                    pSrcInitBits = (unsigned char *) src->fpVidMem;
                    if (dwSrcCaps & DDSCAPS_VIDEOMEMORY)
                        pSrcInitBits += (ULONG) ppdev->pjFrameBufbase;

                    srcScanLength = src->lPitch;

                    if (doMirrorUpDown)
                        wSrcXY.xy16.y += (short)(n - 1);

                    pSrcInitBits += (wSrcXY.xy16.y * srcScanLength);

                    srcOffset = wSrcXY.xy16.x & 3;

                    temp = ((wXYext.wh16.w + srcOffset + 3) & 0xFFFC);

                    nxtSrcScan = srcScanLength - temp;

                    if (doMirrorUpDown)
                        nxtSrcScan = -srcScanLength - temp;

                    pSrcInitBits += (wSrcXY.xy16.x & 0xFFFC);

                    ppdev->NVFreeCount = freeCount;
                    DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
                    freeCount = ppdev->NVFreeCount;

                    while (freeCount < 24)
                        freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                    freeCount -= 24;

                    // changing canvas clip avoids manually aligning 8-bit source
                    npDev->subchannel[NV_DD_CLIP].Nv01ContextClipRectangle.SetPoint =
                    wDestXY.xy;
                    npDev->subchannel[NV_DD_CLIP].Nv01ContextClipRectangle.SetSize =
                    wXYext.wh;

                    npDev->dDrawRop.SetRop5 = dwRop;

                    /* compiler screws up this expression when it goes negative in following macro */
                    temp = wDestXY.xy16.x - srcOffset;

                    npDev->dDrawImageFromCpu.Point =
                    asmMergeCoords(temp, wDestXY.xy16.y);
                    npDev->dDrawImageFromCpu.SizeOut =
                    asmMergeCoords(wXYext.wh16.w + srcOffset, wXYext.wh16.h);

                    temp = ((wXYext.wh16.w + srcOffset + 3) & 0xFFFC);

                    npDev->dDrawImageFromCpu.SizeIn =
                    asmMergeCoords(temp, wXYext.wh16.h);

                    {
                        FAST unsigned char *pSrcBits = pSrcInitBits;
                        FAST unsigned long tmpPxls0, tmpPxls1, tmpPxls2;

                        while (--n >= 0)
                            {
                            pxlCount = (short)temp;

                            while (pxlCount >= 32)
                                {
                                pxlCount -= 32;

                                while (freeCount < 32)
                                    freeCount = NvGetFreeCount(npDev, NV_DD_IMAGE);
                                freeCount -= 32;

                                tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                tmpPxls1 = *(unsigned long *)&pSrcBits[4];
                                npDev->dDrawImageFromCpu.Color[0] = tmpPxls0;
                                tmpPxls2 = *(unsigned long *)&pSrcBits[8];
                                npDev->dDrawImageFromCpu.Color[1] = tmpPxls1;
                                tmpPxls0 = *(unsigned long *)&pSrcBits[12];
                                tmpPxls1 = *(unsigned long *)&pSrcBits[16];
                                npDev->dDrawImageFromCpu.Color[2] = tmpPxls2;
                                npDev->dDrawImageFromCpu.Color[3] = tmpPxls0;
                                tmpPxls2 = *(unsigned long *)&pSrcBits[20];
                                tmpPxls0 = *(unsigned long *)&pSrcBits[24];
                                npDev->dDrawImageFromCpu.Color[4] = tmpPxls1;
                                npDev->dDrawImageFromCpu.Color[5] = tmpPxls2;
                                tmpPxls1 = *(unsigned long *)&pSrcBits[28];
                                npDev->dDrawImageFromCpu.Color[6] = tmpPxls0;
                                pSrcBits += 32;
                                npDev->dDrawImageFromCpu.Color[7] = tmpPxls1;
                                }

                            if (pxlCount > 0)
                                {
                                while (freeCount < 32)
                                    freeCount = NvGetFreeCount(npDev, NV_DD_IMAGE);

                                while (pxlCount >= 8)
                                    {
                                    tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                    tmpPxls1 = *(unsigned long *)&pSrcBits[4];
                                    pxlCount -= 8;
                                    freeCount -= 8;
                                    npDev->dDrawImageFromCpu.Color[0] = tmpPxls0;
                                    npDev->dDrawImageFromCpu.Color[1] = tmpPxls1;
                                    pSrcBits += 8;
                                    }

                                if (pxlCount > 0)
                                    {
                                    tmpPxls0 = *(unsigned long *)&pSrcBits[0];
                                    freeCount -= 4;
                                    pxlCount -= 4;
                                    pSrcBits += 4;
                                    npDev->dDrawImageFromCpu.Color[0] = tmpPxls0;
                                    if (pxlCount > 0)
                                        {
                                        tmpPxls1 = *(unsigned long *)&pSrcBits[0];
                                        npDev->dDrawImageFromCpu.Color[0] = tmpPxls1;
                                        pSrcBits += 4;
                                        freeCount -= 4;
                                        }
                                    }
                                }
                            pSrcBits += nxtSrcScan;
                            }
                    }

                    while (freeCount < 8)
                        freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                    freeCount -= 8;

                    // restore canvas clip to something not likely to interfere with rendering
                    npDev->subchannel[NV_DD_CLIP].Nv01ContextClipRectangle.SetPoint =
                    0;
                    npDev->subchannel[NV_DD_CLIP].Nv01ContextClipRectangle.SetSize =
                    asmMergeCoords(ppdev->cxScreen,
                                   ppdev->cyScreen);
                    }
                }
            else
                { // stretch
                DWORD dwStatus;
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

                pSrcInitBits = (unsigned char *) dwSrcOffset;
                if (dwSrcCaps & DDSCAPS_VIDEOMEMORY)
                    pSrcInitBits += (ULONG) ppdev->pjFrameBufbase;

                srcScanLength = dwSrcPitch;

                if (dwSrcPitch <= 800)
                    m = 16;
                else
                    m = 8;

                while (freeCount < 4)
                    freeCount = NvGetFreeCount(npDev, NV_DD_ROP);
                freeCount -= 4;

                npDev->dDrawRop.SetRop5 = dwRop;

                ppdev->NVFreeCount = freeCount;
                DDRAW_SET_PRIMARY(ppdev, dwDstOffset, dwDstPitch);
                freeCount = ppdev->NVFreeCount;

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

                wSrcExt.wh16.w = (unsigned short)((dwSrcWidth + 3) & ~3L);
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
                    ((dwSrcCaps & DDSCAPS_TEXTURE) == 0) ||
                    isOverlappingVmem)
                    {
                    srcInVideoMemory = 1;
                    }
                else
                    {
                    pSaveSrcBits = (unsigned char *)(pSrcInitBits +
                                                     (wSrcXY.xy16.y * srcScanLength));
                    pSaveSrcBits += wSrcXY.xy16.x;
                    pSrcCurScan = pSaveSrcBits;
                    }

                if (srcInVideoMemory)
                    {
                    pDmaToMemNotifier = (NvNotification *)ppdev->Notifiers->DmaToMem;
                    pScanlineBuffer = (unsigned char *)ppdev->NvScanlineBufferFlat;

                    wSrcExt.wh16.w = (unsigned short)((dwSrcWidth + 3) & ~3);
                    wSrcExt.wh16.h = m;

                    srcStrtX = wSrcXY.xy16.x;

                    while (freeCount < 4)
                        freeCount = NvGetFreeCount(npDev, NV_DD_IMAGE);
                    freeCount -= 4;

                    npDev->subchannel[NV_DD_SPARE].SetObject =
                        NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;
                    ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;

                    // wait for any pending notification operation to finish
                    while ((volatile)pDmaToMemNotifier[NV039_NOTIFIERS_BUFFER_NOTIFY].status == NV_IN_PROGRESS);

                    pDmaToMemNotifier[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV_IN_PROGRESS;

                    while (freeCount < 32)
                        freeCount = NvGetFreeCount(npDev, NV_DD_IMAGE);
                    freeCount -= 32;

                    npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
                    dwSrcOffset +
                    ((dwSrcPitch + skipSrcBytes) * (unsigned long)wSrcXY.xy16.y) +
                    ((unsigned long)wSrcXY.xy16.x);

                    npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
                    srcStrtX;

                    npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
                    (dwSrcPitch + skipSrcBytes);

                    npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
                    dwSrcPitch;

                    npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
                    (unsigned long)(wSrcExt.wh16.w);

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

                while (freeCount < 28)
                    freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                freeCount -= 28;

                npDev->subchannel[NV_DD_SPARE].SetObject =
                NV_DD_STRETCHED_IMAGE_FROM_CPU;
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
                            pSrcBits = (unsigned long *)(pScanlineBuffer + srcStrtX);
                        else
                            pSrcBits = (unsigned long *)(pScanlineBuffer +
                                                         (NV_DMA_SCANLINE_BUF_SIZE >> 1) + srcStrtX);

                        pSrcCurScan = (unsigned char *)pSrcBits;

                        if (m > 0)
                            {
                            pDmaToMemNotifier[NV039_NOTIFIERS_BUFFER_NOTIFY].status = NV_IN_PROGRESS;

                            while (freeCount < 36)
                                freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                            freeCount -= 36;

                            npDev->subchannel[NV_DD_SPARE].SetObject = NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;
                            ppdev->dDrawSpareSubchannelObject = NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT;

                            npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetIn =
                            dwSrcOffset +
                            ((dwSrcPitch + skipSrcBytes) * (unsigned long)wSrcXY.xy16.y) +
                            ((unsigned long)wSrcXY.xy16.x);

                            if (bufState == 0)
                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
                                (NV_DMA_SCANLINE_BUF_SIZE >> 1) +
                                srcStrtX;
                            else
                                npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.OffsetOut =
                                srcStrtX;

                            npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchIn =
                            (dwSrcPitch + skipSrcBytes);

                            npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.PitchOut =
                            dwSrcPitch;

                            npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineLengthIn =
                            (unsigned long)(wSrcExt.wh16.w);

                            npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.LineCount =
                            m;

                            npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.Format =
                                1 | (1 << 8);
                            //NV_MTMF_FORMAT_INPUT_INC_1 | NV_MTMF_FORMAT_OUTPUT_INC_1;

                            npDev->subchannel[NV_DD_SPARE].nv3MemoryToMemoryFormat.BufferNotify =
                            0;

                            bufState ^= 1;

                            if (doMirrorUpDown)
                                wSrcXY.xy16.y -= (m + skipV);
                            else
                                wSrcXY.xy16.y += (m + skipV);

                            while (freeCount < 28)
                                freeCount = NvGetFreeCount(npDev, NV_DD_STRETCH);
                            freeCount -= 28;

                            npDev->subchannel[NV_DD_SPARE].SetObject =
                            NV_DD_STRETCHED_IMAGE_FROM_CPU;
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


                    doNxtSrcScan:      pxlCount = (short)((dwSrcWidth + 3) & ~3);

                    while (pxlCount >= 64)
                        {

                        pxlCount -= 64;

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

                    while (pxlCount >= 32)
                        {

                        pxlCount -= 32;

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

                    while (pxlCount >= 4)
                        {
                        tmpPxls0 = pSrcBits[0];
                        pxlCount -= 4;
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
                            if (dwSrcPitch <= 800)
                                {
                                m = 16;
                                curDstY += deltaDstY;
                                }
                            else
                                {
                                m = 8;
                                curDstY += (deltaDstY >> 1);
                                }
                            /* convert 16.16 to 12.4 */
                            wDstPoint.xy16.y = (short)(curDstY >> 12);

                            // wait for DMA operation to finish
                            while ((volatile)pDmaToMemNotifier[NV076_NOTIFIERS_NOTIFY].status == NV_IN_PROGRESS);
                            }
                        }
                    }
                    if (isOverlappingVmem && lpSurfTemp)
                        {
                        NVHEAP_FREE(lpSurfTemp);
                        }
                }

            if (dwFlags & DDBLT_KEYSRCOVERRIDE)
                {
                while (freeCount < 2*4)
                    freeCount = NvGetFreeCount(npDev, NV_DD_SPARE);
                freeCount -= 2*4;
                if (ppdev->dDrawSpareSubchannelObject != NV_DD_IMAGE_SOLID)
                    {
                    npDev->subchannel[NV_DD_SPARE].SetObject = NV_DD_IMAGE_SOLID;
                    ppdev->dDrawSpareSubchannelObject = NV_DD_IMAGE_SOLID;
                    }
                    // disable chroma key
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
        /* Hardware Z buffer fills not currently supported when in 8bpp mode */
        DPF("HARDWARE Z BUFFER BLT FAILED");
        ppdev->NVFreeCount = freeCount;
        return DDHAL_DRIVER_NOTHANDLED;
        }
    else
        {
        DPF("BLT     blt not handled by driver");
        /* We don't handle any other type of BLT operation */
        ppdev->NVFreeCount = freeCount;
        return DDHAL_DRIVER_NOTHANDLED;
        }

    pbd->ddRVal = DD_OK;
    ppdev->NVFreeCount = freeCount;
    return DDHAL_DRIVER_HANDLED;

    } /* Nv4Blt8 */
