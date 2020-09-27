/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4SWIZ.C                                                         *
*   Texture swizzling routines (some HW dependence).                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 10/20/98 - wrote it                     *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include <assert.h>
#include "nvd3ddrv.h"
#include "ddrvmem.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv4dreg.h"
#include "nv3ddbg.h"

#ifdef NV_TEX2

/*
 * constants
 */
#define L2MB        5
#define MB          (1<<L2MB)   // swizzle macroblock size

#define MB_UMASK    ((MB*MB-1) & 0x55555555)
#define MB_VMASK    ((MB*MB-1) & 0xaaaaaaaa)

/*
 * macros
 */
#define ilcData         global.dwILCData
#define ilcCount        global.dwILCCount
#define ilcMax          global.dwILCMax

#define TEXELCOPY(d,s,bpp)                          \
{                                                   \
    if ((bpp) == 2) *(WORD*)(d)  = *(WORD*)(s);     \
    /*else if ((bpp) == 1) *(BYTE*)(d)  = *(BYTE*)(s);*/\
    else *(DWORD*)(d) = *(DWORD*)(s);               \
}

/*
 * swizzle table buffer layout
 */
BYTE nvTexelScratchBuffer[8192];  // must be as wide as the widest pitch (4 * 2048 = 8192)

#ifdef HW_PAL8
DWORD nvTextureBase_meta; // != NULL for palettized textures
WORD  nvPalette[256];
#endif

/*
 * helpers
 * -------
 */

/*
 * nvSwizzleCalcSwizInfo
 *
 * converts an x,y position to a swizzled blt
 * up to 2048x2048 textures supported
 */
void nvSwizzleCalcSwizzleInfo
(
    DWORD  dwX,
    DWORD  dwY,
    DWORD  dwLogW,
    DWORD  dwLogH,
    DWORD  dwBPP,
    DWORD *pdwOffset,
    DWORD *pdwDUDXor,
    DWORD *pdwDUDXand,
    DWORD *pdwDVDYor,
    DWORD *pdwDVDYand
)
{
    DWORD dwLog       = min(dwLogW,dwLogH);
    DWORD dw2Log      = dwLog << 1;            // # of bits to interleave
    DWORD dwUpperMask = ~((1 << dw2Log) - 1);  // bits to preserve
    DWORD dwLowerMask = ~dwUpperMask;          // bits to interleave

    /*
     * calc offset
     */
    DWORD dwUpperU    = (dwX << dwLog) & dwUpperMask;
    DWORD dwUpperV    = (dwY << dwLog) & dwUpperMask;

    DWORD dwLower     = ((dwX & 0x001) <<  0) | ((dwY & 0x001) <<  1)
                      | ((dwX & 0x002) <<  1) | ((dwY & 0x002) <<  2)
                      | ((dwX & 0x004) <<  2) | ((dwY & 0x004) <<  3)
                      | ((dwX & 0x008) <<  3) | ((dwY & 0x008) <<  4)
                      | ((dwX & 0x010) <<  4) | ((dwY & 0x010) <<  5)
                      | ((dwX & 0x020) <<  5) | ((dwY & 0x020) <<  6)
                      | ((dwX & 0x040) <<  6) | ((dwY & 0x040) <<  7)
                      | ((dwX & 0x080) <<  7) | ((dwY & 0x080) <<  8)
                      | ((dwX & 0x100) <<  8) | ((dwY & 0x100) <<  9)
                      | ((dwX & 0x200) <<  9) | ((dwY & 0x200) << 10)
                      | ((dwX & 0x400) << 10) | ((dwY & 0x400) << 11)
                      | ((dwX & 0x800) << 11) | ((dwY & 0x800) << 12);

    *pdwOffset = ((dwLower & dwLowerMask) | dwUpperU | dwUpperV) * dwBPP;

    /*
     * calc masks
     */
    *pdwDUDXor  = (0xaaaaaaaa & dwLowerMask) * dwBPP;
    *pdwDUDXand = (0x55555555 | dwUpperMask) * dwBPP;
    *pdwDVDYor  = (0x55555555 & dwLowerMask) * dwBPP;
    *pdwDVDYand = (0xaaaaaaaa | dwUpperMask) * dwBPP;
}

/*
 * SOFTWARE SWIZZLE
 * ----------------
 */

/*
 * nvSwizzleSmallBlock
 *
 * swizzles any texture with a dimension less than 8 (i.e. 2048 x 4)
 */
void nvSwizzleSmallBlock
(
    DWORD dwSrcBase,
    DWORD dwSrcPitch,

    DWORD dwDestBase,

    DWORD dwLogW,
    DWORD dwLogH,
    DWORD dwBPP
)
{
    DWORD dwOffset;
    DWORD dwDUDXor;
    DWORD dwDUDXand;
    DWORD dwDVDYor;
    DWORD dwDVDYand;
    DWORD dwCount,dwSize,dwLine;
    DWORD dwScratch0,dwScratch1;
    DWORD u,v,x,y;
    void *pMemory;

    /*
     * optimize for really thin textures (swizzle bits do not work with width==1)
     */
    if (!dwLogW)
    {
        if (dwSrcPitch != (dwBPP << dwLogH))
        {
            switch (dwBPP)
            {
                case 1:
                {
                    u = (DWORD)nvTexelScratchBuffer;
                    for (y = (1 << dwLogH); y; y--)
                    {
                        *(BYTE*)u = *(BYTE*)dwSrcBase;
                        dwSrcBase += dwSrcPitch;
                        u += 1;
                    }
                    break;
                }
                case 2:
                {
                    u = (DWORD)nvTexelScratchBuffer;
                    for (y = (1 << dwLogH); y; y--)
                    {
                        *(WORD*)u = *(WORD*)dwSrcBase;
                        dwSrcBase += dwSrcPitch;
                        u += 2;
                    }
                    break;
                }
                case 4:
                {
                    u = (DWORD)nvTexelScratchBuffer;
                    for (y = (1 << dwLogH); y; y--)
                    {
                        *(DWORD*)u = *(DWORD*)dwSrcBase;
                        dwSrcBase += dwSrcPitch;
                        u += 4;
                    }
                    break;
                }
            }
            nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,dwBPP << dwLogH,NV_MEMCOPY_WANTDESTALIGNED);
            return;
        }
        else
        {
            /*
             * pitch == width, treat as thin horz texture
             */
            dwLogW = dwLogH;
            dwLogH = 0;
        }
    }
    if (!dwLogH)
    {
        nvMemCopy ((DWORD)nvTexelScratchBuffer,dwSrcBase,dwBPP << dwLogW,NV_MEMCOPY_WANTSRCALIGNED);
        nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,dwBPP << dwLogW,NV_MEMCOPY_WANTDESTALIGNED);
        return;
    }

    /*
     * figure out bits
     */
    nvSwizzleCalcSwizzleInfo (0,0,dwLogW,dwLogH,dwBPP,
                              &dwOffset,
                              &dwDUDXor,&dwDUDXand,
                              &dwDVDYor,&dwDVDYand);

    /*
     * figure out memory requirements
     */
    dwCount = 1     << (dwLogW + dwLogH);
    dwSize  = dwBPP << (dwLogW + dwLogH);
    if (dwSize > 4096)
    {
        pMemory = AllocIPM(dwSize * 2);
        dwScratch0 = (DWORD)pMemory;
        dwScratch1 = ((DWORD)pMemory) + dwSize;
    }
    else
    {
        pMemory    = NULL;
        dwScratch0 = (DWORD)(nvTexelScratchBuffer + 0);
        dwScratch1 = (DWORD)(nvTexelScratchBuffer + 4096);
    }

    /*
     * read texels
     */
    dwLine = dwBPP << dwLogW;
    if (dwLine != dwSrcPitch)
    {
        u = dwSrcBase;
        v = dwScratch0;
        for (y = (1 << dwLogH); y; y--)
        {
            nvMemCopy (v,u,dwLine,NV_MEMCOPY_WANTSRCALIGNED);
            v += dwLine;
            u += dwSrcPitch;
        }
    }
    else
    {
        nvMemCopy (dwScratch0,dwSrcBase,dwLine << dwLogH,NV_MEMCOPY_WANTSRCALIGNED);
    }

    /*
     * do swizzle
     */
    switch (dwBPP)
    {
        case 1:
        {
            v = 0;
            for (y = (1 << dwLogH); y; y--)
            {
                u = 0;
                for (x = (1 << dwLogW); x; x--)
                {
                    *(BYTE*)(dwScratch1 + (u | v)) = *(BYTE*)dwScratch0;

                    dwScratch0 += 1;
                    u           = ((u | dwDUDXor) + 1) & dwDUDXand;
                }
                v = ((v | dwDVDYor) + (1*2)) & dwDVDYand;
            }
            break;
        }
        case 2:
        {
            v = 0;
            for (y = (1 << dwLogH); y; y--)
            {
                u = 0;
                for (x = (1 << dwLogW); x; x--)
                {
                    *(WORD*)(dwScratch1 + (u | v)) = *(WORD*)dwScratch0;

                    dwScratch0 += 2;
                    u           = ((u | dwDUDXor) + 2) & dwDUDXand;
                }
                v = ((v | dwDVDYor) + (2*2)) & dwDVDYand;
            }
            break;
        }
        case 4:
        {
            v = 0;
            for (y = (1 << dwLogH); y; y--)
            {
                u = 0;
                for (x = (1 << dwLogW); x; x--)
                {
                    *(DWORD*)(dwScratch1 + (u | v)) = *(DWORD*)dwScratch0;

                    dwScratch0 += 4;
                    u           = ((u | dwDUDXor) + 4) & dwDUDXand;
                }
                v = ((v | dwDVDYor) + (4*2)) & dwDVDYand;
            }
            break;
        }
    }

    /*
     * write texels
     */
#ifdef HW_PAL8
    if (nvTextureBase_meta)
    {
        DWORD i = dwSize;

        u = dwScratch1;
        v = dwScratch0;
        while (i)
        {
            BYTE bIndex = *(BYTE*)u;
            WORD wColor = nvPalette[bIndex];
            *(WORD*)v = wColor;
            u ++;
            v += 2;
            i --;
        }
        u = dwScratch0;
        v = dwDestBase;
        nvMemCopy (v,u,dwSize*2,NV_MEMCOPY_WANTDESTALIGNED);
    }
    else
    {
        u = dwScratch1;
        v = dwDestBase;
        nvMemCopy (v,u,dwSize,NV_MEMCOPY_WANTDESTALIGNED);
    }
#else
    u = dwScratch1;
    v = dwDestBase;
    nvMemCopy (v,u,dwSize,NV_MEMCOPY_WANTDESTALIGNED);
#endif

    /*
     * free memory
     */
    if (pMemory) FreeIPM (pMemory);
}

/*
 * nvSwizzleMacroBlock_LS
 *
 * swizzles a full MBxMB block (lin -> swz)
 */
void nvSwizzleMacroBlock_LS
(
    DWORD dwSrcBase,
    DWORD dwSrcPitch,
    DWORD dwDestBase,
    DWORD dwBPP
)
{
    DWORD src,dst,cnt;
    DWORD x,y,u,v;

    /*
     * read texels
     */
    src = dwSrcBase;
    dst = (DWORD)(nvTexelScratchBuffer + 0);
    cnt = dwBPP << L2MB;
    for (y=MB; y; y--)
    {
        nvMemCopy (dst,src,cnt,NV_MEMCOPY_WANTSRCALIGNED);
        dst += cnt;
        src += dwSrcPitch;
    }

    /*
     * swizzle texels
     */
    src = (DWORD)(nvTexelScratchBuffer + 0);
    dst = (DWORD)(nvTexelScratchBuffer + 4096);
    u   = 0;
    v   = 0;
    switch (dwBPP)
    {
        case 1:
        {
            for (y=MB; y; y--)
            {
                u = 0;
                for (x=MB; x; x--)
                {
                    *(BYTE*)(dst + (u | v)) = *(BYTE*)src;
                    src += 1;
                    u = ((u | (MB_VMASK*1)) + 1*1) & (MB_UMASK*1);
                }
                v = ((v | (MB_UMASK*1)) + 2*1) & (MB_VMASK*1);
            }
            break;
        }
        case 2:
        {
            for (y=MB; y; y--)
            {
                u = 0;
                for (x=MB; x; x--)
                {
                    *(WORD*)(dst + (u | v)) = *(WORD*)src;
                    src += 2;
                    u = ((u | (MB_VMASK*2)) + 1*2) & (MB_UMASK*2);
                }
                v = ((v | (MB_UMASK*2)) + 2*2) & (MB_VMASK*2);
            }
            break;
        }
        case 4:
        {
            for (y=MB; y; y--)
            {
                u = 0;
                for (x=MB; x; x--)
                {
                    *(DWORD*)(dst + (u | v)) = *(DWORD*)src;
                    src += 4;
                    u = ((u | (MB_VMASK*4)) + 1*4) & (MB_UMASK*4);
                }
                v = ((v | (MB_UMASK*4)) + 2*4) & (MB_VMASK*4);
            }
            break;
        }
    }

    /*
     * write out
     */
#ifdef HW_PAL8
    if (nvTextureBase_meta)
    {
        DWORD i;
        src   = (DWORD)(nvTexelScratchBuffer + 4096);
        dst   = (DWORD)(nvTexelScratchBuffer + 0);
        cnt <<= L2MB;

        i = cnt;
        while (i)
        {
            BYTE bIndex = *(BYTE*)src;
            WORD wColor = nvPalette[bIndex];
            *(WORD*)dst = wColor;
            src ++;
            dst += 2;
            i   --;
        }

        src = (DWORD)(nvTexelScratchBuffer + 0);
        dst = (dwDestBase - nvTextureBase_meta) * 2 + nvTextureBase_meta; // fixup dest (all code currently assume src & dst bpp the same)
        nvMemCopy (dst,src,cnt*2,NV_MEMCOPY_WANTDESTALIGNED);
    }
    else
    {
        src   = (DWORD)(nvTexelScratchBuffer + 4096);
        dst   = dwDestBase;
        cnt <<= L2MB;
        nvMemCopy (dst,src,cnt,NV_MEMCOPY_WANTDESTALIGNED);
    }
#else
    src   = (DWORD)(nvTexelScratchBuffer + 4096);
    dst   = dwDestBase;
    cnt <<= L2MB;
    nvMemCopy (dst,src,cnt,NV_MEMCOPY_WANTDESTALIGNED);
#endif
}

/*
 * nvSwizzleBlt_LS_FT
 *
 * software swizzle algorithm for full textures
 *  full texture means:
 *    source width & height == dest width & height
 *    blt rectangle is (0,0) - (w,h)
 */
void nvSwizzleBlt_LS_FT
(
    DWORD dwSrcBase,
    DWORD dwSrcPitch,

    DWORD dwDestBase,

    DWORD dwLogW,
    DWORD dwLogH,
    DWORD dwBPP
)
{
    DWORD dwOffset;
    DWORD dwDUDXor;
    DWORD dwDUDXand;
    DWORD dwDVDYor;
    DWORD dwDVDYand;
    DWORD dwMBPP,dwMBPP2;
    DWORD u,v,x,y;

    /*
     * handle small cases
     */
    if ((dwLogW < L2MB) || (dwLogH < L2MB))
    {
        nvSwizzleSmallBlock (dwSrcBase,dwSrcPitch,
                             dwDestBase,
                             dwLogW,dwLogH,dwBPP);
        return;
    }

    /*
     * adjust width & height for macroblocks
     */
    dwLogW -= L2MB;
    dwLogH -= L2MB;
    dwMBPP  = dwBPP << L2MB;
    dwMBPP2 = dwBPP << (L2MB + L2MB);

    /*
     * optimize for thin vertival case (swizzle bit does not work for this)
     */
    if (!dwLogW)
    {
        for (y = (1 << dwLogH); y; y--)
        {
            nvSwizzleMacroBlock_LS (dwSrcBase,dwSrcPitch,
                                    dwDestBase,
                                    dwBPP);
            dwSrcBase  += dwSrcPitch << L2MB;
            dwDestBase += dwMBPP2;
        }
        return;
    }

    /*
     * figure out bits for macro blocks
     */
    nvSwizzleCalcSwizzleInfo (0,0,dwLogW,dwLogH,dwMBPP2,
                              &dwOffset,
                              &dwDUDXor,&dwDUDXand,
                              &dwDVDYor,&dwDVDYand);

    /*
     * do macroblock swizzle
     */
    v = 0;
    for (y = (1 << dwLogH); y; y--)
    {
        DWORD dwSrc = dwSrcBase;

        u = 0;
        for (x = (1 << dwLogW); x; x--)
        {
            nvSwizzleMacroBlock_LS (dwSrc,dwSrcPitch,
                                    dwDestBase + (u | v),
                                    dwBPP);

            u = ((u | dwDUDXor) + dwMBPP2) & dwDUDXand;
            dwSrc += dwBPP << L2MB;
        }
        v = ((v | dwDVDYor) + dwMBPP2*2) & dwDVDYand;

        dwSrcBase += dwSrcPitch << L2MB;
    }
}

/*
 * nvSwizzleBlt_aa_bb_cc
 *
 *  aa E (LL, SL, LS, SS)
 *  bb E (XX, ST)
 *  cc E (XX, LR, LW)
 *
 * swizzle blts we can build - other indices are undefined
 *
 *  lin -> lin, subtexture, machine dep  linear r/w        nvSwizzleBlt_LL_ST_RW
 *  swz -> lin, subtexture, machine dep, linear write      nvSwizzleBlt_SL_ST_LW
 *  lin -> swz, subtexture, machine dep, linear read       nvSwizzleBlt_LS_ST_LR
 *  swz -> swz, subtexture, machine dep                    nvSwizzleBlt_SS_ST_XX
 *  swz -> swz,             machine dep  linear r/w        nvSwizzleBlt_SS_XX_RW
 */
void nvSwizzleBlt_LL_ST_RW
(
    DWORD dwSrcAddr,
    DWORD dwSrcPitch,

    DWORD dwDestAddr,
    DWORD dwDestPitch,

    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwSubHeight,
    DWORD dwBPP
)
{
    DWORD dwBytes = dwWidth * dwBPP;
    DWORD y;

    for (y = dwHeight; y;)
    {
        DWORD a;
        DWORD h;
        DWORD f = min(dwSubHeight,y);

        /*
         * read linear
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            nvMemCopy (a,dwSrcAddr,dwBytes,NV_MEMCOPY_WANTSRCALIGNED);
            dwSrcAddr += dwSrcPitch;
            a         += dwBytes;
        }

        /*
         * write linear
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            nvMemCopy (dwDestAddr,a,dwBytes,NV_MEMCOPY_WANTDESTALIGNED);
            a          += dwBytes;
            dwDestAddr += dwDestPitch;
        }

        y -= f;
    }
}

void nvSwizzleBlt_SL_ST_LW
(
    DWORD dwSrcBase,
    DWORD dwSrcOffset,
    DWORD dwSrcDUDXor,
    DWORD dwSrcDUDXand,
    DWORD dwSrcDVDYor,
    DWORD dwSrcDVDYand,

    DWORD dwDestAddr,
    DWORD dwDestPitch,

    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwSubHeight,
    DWORD dwBPP
)
{
    DWORD dwBytes   = dwWidth * dwBPP;
    DWORD y;

    for (y = dwHeight; y;)
    {
        DWORD a;
        DWORD h;
        DWORD f = min(dwSubHeight,y);

        /*
         * read swizzle
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            DWORD u;
            DWORD v;
            DWORD b;
            DWORD x;

            v = dwSrcOffset & dwSrcDVDYand;
            b = dwSrcOffset;
            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (a,dwSrcBase + b,dwBPP);
                a += dwBPP;
                u = ((b | dwSrcDUDXor) + dwBPP) & dwSrcDUDXand;
                b = u | v;
            }
            u = dwSrcOffset & dwSrcDUDXand;
            v = ((dwSrcOffset | dwSrcDVDYor) + (dwBPP*2)) & dwSrcDVDYand;
            dwSrcOffset = u | v;
        }

        /*
         * write linear
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            nvMemCopy (dwDestAddr,a,dwBytes,NV_MEMCOPY_WANTDESTALIGNED);
            a          += dwBytes;
            dwDestAddr += dwDestPitch;
        }

        y -= f;
    }
}

void nvSwizzleBlt_LS_ST_LR
(
    DWORD dwSrcAddr,
    DWORD dwSrcPitch,

    DWORD dwDestBase,
    DWORD dwDestOffset,
    DWORD dwDestDUDXor,
    DWORD dwDestDUDXand,
    DWORD dwDestDVDYor,
    DWORD dwDestDVDYand,

    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwSubHeight,
    DWORD dwBPP
)
{
    DWORD dwBytes   = dwWidth * dwBPP;
    DWORD y;

    for (y = dwHeight; y;)
    {
        DWORD a;
        DWORD h;
        DWORD f = min(dwSubHeight,y);

        /*
         * read linear
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            nvMemCopy (a,dwSrcAddr,dwBytes,NV_MEMCOPY_WANTSRCALIGNED);
            a         += dwBytes;
            dwSrcAddr += dwSrcPitch;
        }

        /*
         * write swizzle
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            DWORD u;
            DWORD v;
            DWORD b;
            DWORD x;

            v = dwDestOffset & dwDestDVDYand;
            b = dwDestOffset;
            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (dwDestBase + b,a,dwBPP);
                a += dwBPP;
                u = ((b | dwDestDUDXor) + dwBPP) & dwDestDUDXand;
                b = u | v;
            }
            u = dwDestOffset & dwDestDUDXand;
            v = ((dwDestOffset | dwDestDVDYor) + (dwBPP*2)) & dwDestDVDYand;
            dwDestOffset = u | v;
        }

        y -= f;
    }
}

void nvSwizzleBlt_SS_ST_XX
(
    DWORD dwSrcBase,
    DWORD dwSrcOffset,
    DWORD dwSrcDUDXor,
    DWORD dwSrcDUDXand,
    DWORD dwSrcDVDYor,
    DWORD dwSrcDVDYand,

    DWORD dwDestBase,
    DWORD dwDestOffset,
    DWORD dwDestDUDXor,
    DWORD dwDestDUDXand,
    DWORD dwDestDVDYor,
    DWORD dwDestDVDYand,

    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwSubHeight,
    DWORD dwBPP
)
{
    DWORD dwBytes   = dwWidth * dwBPP;
    DWORD y;

    for (y = dwHeight; y;)
    {
        DWORD a;
        DWORD h;
        DWORD f = min(dwSubHeight,y);

        /*
         * read swizzle
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            DWORD u;
            DWORD v;
            DWORD b;
            DWORD x;

            v = dwSrcOffset & dwSrcDVDYand;
            b = dwSrcOffset;
            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (a,dwSrcBase + b,dwBPP);
                a += dwBPP;
                u = ((b | dwSrcDUDXor) + dwBPP) & dwSrcDUDXand;
                b = u | v;
            }
            u = dwSrcOffset & dwSrcDUDXand;
            v = ((dwSrcOffset | dwSrcDVDYor) + (dwBPP*2)) & dwSrcDVDYand;
            dwSrcOffset = u | v;
        }

        /*
         * write swizzle
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            DWORD u;
            DWORD v;
            DWORD b;
            DWORD x;

            v = dwDestOffset & dwSrcDVDYand;
            b = dwDestOffset;
            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (dwDestBase + b,a,dwBPP);
                a += dwBPP;
                u = ((b | dwDestDUDXor) + dwBPP) & dwDestDUDXand;
                b = u | v;
            }
            u = dwDestOffset & dwDestDUDXand;
            v = ((dwDestOffset | dwDestDVDYor) + (dwBPP*2)) & dwDestDVDYand;
            dwDestOffset = u | v;
        }

        y -= f;
    }
}

void nvSwizzleBlt_SS_XX_RW
(
    DWORD dwSrcAddr,

    DWORD dwDestAddr,

    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwSubHeight,
    DWORD dwBPP
)
{
    DWORD dwBytes = dwWidth * dwBPP;
    DWORD y;

    for (y = dwHeight; y;)
    {
        DWORD f = min(dwSubHeight,y);
        DWORD c = f * dwBytes;

        /*
         * read linear (swizzled texels)
         */
        nvMemCopy ((DWORD)&nvTexelScratchBuffer,dwSrcAddr,c,NV_MEMCOPY_WANTSRCALIGNED);
        dwSrcAddr += c;

        /*
         * write linear (swizzled texels)
         */
        nvMemCopy (dwDestAddr,(DWORD)&nvTexelScratchBuffer,c,NV_MEMCOPY_WANTDESTALIGNED);
        dwDestAddr += c;

        y -= f;
    }
}

/*
 * HARDWARE SWIZZLE
 * ----------------
 */

/*
 * nvHWSwizzleBlt
 *
 * perform a HW swizzle blt
 */
void nvHWSwizzleBlt
(
    DWORD dwSrcOffset,
    DWORD dwSrcPitch,
    DWORD dwImageColorFormat,
    DWORD dwSrcX0,
    DWORD dwSrcY0,
    DWORD dwSrcAlloc,

    DWORD dwDstSwizzleOffset,
    DWORD dwMipMapBaseU,
    DWORD dwMipMapBaseV,
    DWORD dwSurfaceColorFormat,

    DWORD dwDstX0,
    DWORD dwDstY0,

    DWORD dwBlitWidth,
    DWORD dwBlitHeight

#ifdef DIRECTSWIZZLE
    ,DWORD dwBPP
#endif
)
{
#ifdef DIRECTSWIZZLE
    {
        /*
         * target surface
         */
        DWORD dwCount = sizeSetObject * 1 + sizeSetStartMethod * 1 + sizeSetData * 4;
        while (nvFreeCount < dwCount)
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);

        nvglSetObject      (nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_SURFACES_2D);
        nvglSetStartMethod (nvFifo, nvFreeCount, NV_DD_SPARE, NV042_SET_COLOR_FORMAT,4);
        nvglSetData        (nvFifo, nvFreeCount,              dwSurfaceColorFormat);
        nvglSetData        (nvFifo, nvFreeCount,              (dwBPP << (16 + dwMipMapBaseU)) | dwSrcPitch);
        nvglSetData        (nvFifo, nvFreeCount,              dwDstSwizzleOffset); // not used
        nvglSetData        (nvFifo, nvFreeCount,              dwDstSwizzleOffset);
    }
#else //!DIRECTSWIZZLE
    /*
     * target surface
     */
    while (nvFreeCount < (sizeSetObject + sizeSetSwizzledSurface))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetSwizzledSurface));
    nvglSetObject          (nvFifo,nvFreeCount,NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    nvglSetSwizzledSurface (nvFifo,nvFreeCount,NV_DD_SPARE,
                                        (DRF_NUM(052,_SET_FORMAT,_COLOR ,dwSurfaceColorFormat)
                                        |DRF_NUM(052,_SET_FORMAT,_WIDTH ,dwMipMapBaseU)
                                        |DRF_NUM(052,_SET_FORMAT,_HEIGHT,dwMipMapBaseV)),
                                         dwDstSwizzleOffset);
#endif //!DIRECTSWIZZLE

    /*
     * format
     */
    while (nvFreeCount < (sizeSetObject + sizeSetScaledImageFormat))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetScaledImageFormat));
    nvglSetObject               (nvFifo,nvFreeCount,NV_DD_SPARE, D3D_SCALED_IMAGE_FROM_MEMORY);
    nvglSetScaledImageFormat    (nvFifo,nvFreeCount,NV_DD_SPARE, dwImageColorFormat);

    /*
     * source memory context
     */
    while (nvFreeCount < sizeSetScaledImageContextImage)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, sizeSetScaledImageContextImage);
    if (dwSrcAlloc & NV4_TEXLOC_AGP)
    {
        nvglSetScaledImageContextImage(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY);
    } else if (dwSrcAlloc & NV4_TEXLOC_VID)
    {
        nvglSetScaledImageContextImage(nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    } else if (dwSrcAlloc & NV4_TEXLOC_PCI)
    {
        nvglSetScaledImageContextImage(nvFifo, nvFreeCount, NV_DD_SPARE, D3D_CONTEXT_SWIZZLED_TEXTURE_FROM_MEMORY);
    }

    /*
     * clip
     */
    while (nvFreeCount < (sizeScaledImageClip + sizeScaledImageOut + sizeScaledImageDeltaDuDxDvDy))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeScaledImageClip + sizeScaledImageOut + sizeScaledImageDeltaDuDxDvDy));
    nvglScaledImageClip          (nvFifo,nvFreeCount,NV_DD_SPARE, ((DWORD)dwDstX0), ((DWORD)dwDstY0), dwBlitWidth, dwBlitHeight);
    nvglScaledImageOut           (nvFifo,nvFreeCount,NV_DD_SPARE, ((DWORD)dwDstX0), ((DWORD)dwDstY0), dwBlitWidth, dwBlitHeight);
    nvglScaledImageDeltaDuDxDvDy (nvFifo,nvFreeCount,NV_DD_SPARE, (1<<20), (1<<20));

    /*
     * src & go
     */
    while (nvFreeCount < (sizeScaledImageInSize + sizeScaledImageInFormat + sizeScaledImageInOffset + sizeScaledImageInPoint))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeScaledImageInSize + sizeScaledImageInFormat + sizeScaledImageInOffset + sizeScaledImageInPoint));
    nvglScaledImageInSize   (nvFifo, nvFreeCount, NV_DD_SPARE, dwBlitWidth, dwBlitHeight);
    nvglScaledImageInFormat (nvFifo, nvFreeCount, NV_DD_SPARE,
                             (DRF_NUM(077,_IMAGE_IN_FORMAT,_PITCH,        dwSrcPitch)
                             |DRF_DEF(077,_IMAGE_IN_FORMAT,_ORIGIN,       _CORNER)
                             |DRF_DEF(077,_IMAGE_IN_FORMAT,_INTERPOLATOR, _ZOH)));
    nvglScaledImageInOffset (nvFifo, nvFreeCount, NV_DD_SPARE, dwSrcOffset);
    nvglScaledImageInPoint  (nvFifo, nvFreeCount, NV_DD_SPARE, dwSrcX0, dwSrcY0);

    pDriverData->dDrawSpareSubchannelObject = D3D_SCALED_IMAGE_FROM_MEMORY;
}

#ifdef HW_PAL8
/*
 * nvHWSwizzleBltPal8
 *
 * perform a HW swizzle blt from an 8-bit palettized texture
 */
void nvHWSwizzleBltPal8
(
    // LUT parameters
    DWORD dwLUTOffset,

    // indexed (source) image parameters
    DWORD dwSrcImageAddr,

    // destination surface parameters
    DWORD dwDstSwizzleOffset,
    DWORD dwDstLogU,
    DWORD dwDstLogV,
    DWORD dwDstColorFormat,
    DWORD dwDstBPP,

    // general parameters
    DWORD dwWidth,
    DWORD dwHeight
)
{
    DWORD dwMaxScanLinesPerIteration, dwScanLinesLeftToWrite, dwScanLinesToWrite;
    DWORD dwCount, x0, y0;
    DWORD dwSrcSizeWords;

    if (dwWidth*dwHeight < 4) {
        // this is going to be an annoying special case because it
        // means the stupid thing is less than one DWORD. just bail for now.
        dbgD3DError();
        return;
    }

    // we can put at most 0x1c00 bytes into the buffer at a time
    assert(dwWidth < 0x1c00);
    dwMaxScanLinesPerIteration = 0x1c00 / dwWidth;
    dwScanLinesLeftToWrite = dwHeight;

    //
    // set stuff that doesn't change from one iteration to the next
    //

    // target surface parameters
    // nv05 will be able to write to a swizzled surface. alas nv04 cannot
    /*
    while (nvFreeCount < (sizeSetObject + sizeSetSwizzledSurface))
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (sizeSetObject + sizeSetSwizzledSurface));
    nvglSetObject          (nvFifo,nvFreeCount,NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    nvglSetSwizzledSurface (nvFifo,nvFreeCount,NV_DD_SPARE,
                                        (DRF_NUM(052,_SET_FORMAT,_COLOR ,dwDstColorFormat)
                                        |DRF_NUM(052,_SET_FORMAT,_WIDTH ,dwDstLogU))
                                        |DRF_NUM(052,_SET_FORMAT,_HEIGHT,dwDstLogV)),
                                         dwDstSwizzleOffset);  */
    dwCount = sizeSetObject + sizeSetStartMethod + 4*sizeSetData;
    while (nvFreeCount < dwCount)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
    nvglSetObject      (nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_SURFACES_2D);
    nvglSetStartMethod (nvFifo, nvFreeCount, NV_DD_SPARE, NV042_SET_COLOR_FORMAT, 4);
    nvglSetData        (nvFifo, nvFreeCount,              dwDstColorFormat);
    nvglSetData        (nvFifo, nvFreeCount,              ((dwDstBPP << dwDstLogU) << 16) |
                                                           (dwDstBPP << dwDstLogU));  // not used
    nvglSetData        (nvFifo, nvFreeCount,              dwDstSwizzleOffset);  // not used
    nvglSetData        (nvFifo, nvFreeCount,              dwDstSwizzleOffset);

    // source parameters
    dwCount = sizeSetObject + sizeSetIndexedImageContextDmaLUT + sizeSetIndexedImageLUTOffset;
    while (nvFreeCount < dwCount)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
    nvglSetObject (nvFifo,nvFreeCount,NV_DD_SPARE, D3D_INDEXED_IMAGE_FROM_CPU);
    nvglSetIndexedImageContextDmaLUT(nvFifo, nvFreeCount, NV_DD_SPARE,
                                     NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
    nvglSetIndexedImageLUTOffset(nvFifo,nvFreeCount,NV_DD_SPARE, dwLUTOffset);

    //
    // now iterate as many times as necessary to feed the whole thing into the buffer
    //
    x0 = y0 = 0;

    while (dwScanLinesLeftToWrite > 0) {

        dwScanLinesToWrite = (dwScanLinesLeftToWrite > dwMaxScanLinesPerIteration) ?
            dwMaxScanLinesPerIteration : dwScanLinesLeftToWrite;

        // set point and image sizes
        dwCount = sizeSetIndexedImagePoint + sizeSetIndexedImageSizes;
        while (nvFreeCount < dwCount)
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
        nvglSetIndexedImagePoint(nvFifo,nvFreeCount,NV_DD_SPARE, x0,y0);
        nvglSetIndexedImageSizes(nvFifo,nvFreeCount,NV_DD_SPARE,
                                 dwWidth, dwScanLinesToWrite,
                                 dwWidth, dwScanLinesToWrite);

        // put the indices into the push buffer
        assert((dwScanLinesToWrite*dwWidth & 0x3) == 0);
        dwSrcSizeWords = (dwScanLinesToWrite*dwWidth) >> 2;
        while (nvFreeCount < (1+dwSrcSizeWords))
            nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, (1+dwSrcSizeWords));
        nvglSetIndexedImageIndices(nvFifo,nvFreeCount,NV_DD_SPARE,
                                   dwSrcSizeWords, (DWORD *)dwSrcImageAddr);

        // increment / decrement counters and pointers
        y0 += dwScanLinesToWrite;
        dwScanLinesLeftToWrite -= dwScanLinesToWrite;
        (DWORD *)dwSrcImageAddr += dwSrcSizeWords;
    }

    pDriverData->dDrawSpareSubchannelObject = D3D_INDEXED_IMAGE_FROM_CPU;
}

// munge the palette from Microsoft form into X8R8G8B8 form
// and move it into video memory where the HW can use it

BOOL nvPreparePalette(PNVD3DTEXTURE pSrcTexture)
{
    LPPALETTEENTRY pPaletteSrc;
    DWORD pPaletteDst, dwCount;

    // make sure we have a palette
    if ((!pSrcTexture) ||
        (!pSrcTexture->lpLcl) ||
        (!pSrcTexture->lpLcl->lpDDPalette) ||
        (!pSrcTexture->lpLcl->lpDDPalette->lpLcl) ||
        (!pSrcTexture->lpLcl->lpDDPalette->lpLcl->lpGbl) ||
        (!pSrcTexture->lpLcl->lpDDPalette->lpLcl->lpGbl->lpColorTable)) {
        dbgD3DError();
        return FALSE;
    }

    pPaletteSrc = pSrcTexture->lpLcl->lpDDPalette->lpLcl->lpGbl->lpColorTable;
    pPaletteDst = pCurrentContext->dwTexturePaletteAddr;

    // feed the palette through the push buffer via
    // an NV04_IMAGE_FROM_CPU object so as to keep palettes
    // synchronized with the textures that use them.

    // set up the destination surface
    dwCount = sizeSetObject + sizeSetStartMethod + 4*sizeSetData;
    while (nvFreeCount < dwCount)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
    nvglSetObject      (nvFifo, nvFreeCount, NV_DD_SPARE, NV_DD_SURFACES_2D);
    nvglSetStartMethod (nvFifo, nvFreeCount, NV_DD_SPARE, NV042_SET_COLOR_FORMAT, 4);
    nvglSetData        (nvFifo, nvFreeCount,              NV042_SET_COLOR_FORMAT_LE_A8R8G8B8);
    nvglSetData        (nvFifo, nvFreeCount,              ((256*4) << 16) |
                                                           (256*4));        // not used
    nvglSetData        (nvFifo, nvFreeCount,              pPaletteDst);     // not used
    nvglSetData        (nvFifo, nvFreeCount,              pPaletteDst);

    // set image_from_cpu object and feed the palette into the push buffer
    dwCount = sizeSetObject + sizeSetStartMethod + 3*sizeSetData;
    while (nvFreeCount < dwCount)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
    nvglSetObject      (nvFifo, nvFreeCount, NV_DD_SPARE, D3D_IMAGE_FROM_CPU);
    nvglSetStartMethod (nvFifo, nvFreeCount, NV_DD_SPARE, NV061_POINT, 3);
    nvglSetData        (nvFifo, nvFreeCount, 0);                  // point
    nvglSetData        (nvFifo, nvFreeCount, (0x1<<16) | 0x100);  // sizeout = 1H x 256W
    nvglSetData        (nvFifo, nvFreeCount, (0x1<<16) | 0x100);  // sizein = sizeout

    dwCount = sizeSetStartMethod + 256;
    while (nvFreeCount < dwCount)
        nvglGetFreeCount(nvFifo, nvFreeCount, NV_DD_SPARE, dwCount);
    nvglSetStartMethod (nvFifo, nvFreeCount, NV_DD_SPARE, NV061_COLOR(0), 256);

    for (dwCount=0; dwCount<256; dwCount++) {
        *(DWORD *)nvFifo = (((DWORD)(0xff000000))                |  // A
                            ((DWORD)(pPaletteSrc->peRed) << 16)  |  // R
                            ((DWORD)(pPaletteSrc->peGreen) << 8) |  // G
                            ((DWORD)(pPaletteSrc->peBlue) << 0));   // B
        (DWORD *)pPaletteSrc += 1;
        (DWORD *)nvFifo += 1;
    }
    nvFreeCount -= 256;

    //NV_D3D_GLOBAL_SAVE();
    //nvFlushDmaBuffers();

    return TRUE;
}

#endif  // HW_PAL8

/*
 * exported
 * --------
 */

/*
 * nvSwizzleBlt
 *
 * performs swizzle or deswizzle in the most optimal fashion depending
 * on surface location and specified flags
 *
 * if pTexture != NULL we will block CPU before writing (if needed)
 * & also update retirement date for proper syncronization.
 */
BOOL nvSwizzleBlt
(
    DWORD         dwSrcAddr,
    DWORD         dwSrcAlloc,
    DWORD         dwSrcLogW,
    DWORD         dwSrcLogH,
    DWORD         dwSrcX0,
    DWORD         dwSrcY0,
    DWORD         dwSrcX1,
    DWORD         dwSrcY1,
    DWORD         dwSrcPitch,
    PNVD3DTEXTURE pSrcTexture,

    DWORD         dwDestAddr,
    DWORD         dwDestAlloc,
    DWORD         dwDestLogW,
    DWORD         dwDestLogH,
    DWORD         dwDestX,
    DWORD         dwDestY,
    DWORD         dwDestPitch,
    PNVD3DTEXTURE pDestTexture,

    DWORD         dwBPP,
    DWORD         dwFlags
)
{
    /*
     * prepare common variables
     */
    DWORD dwW         = dwSrcX1 - dwSrcX0;
    DWORD dwH         = dwSrcY1 - dwSrcY0;
    BOOL  bWholeSrc   = ((1U << dwSrcLogW)  == dwW) && ((1U << dwSrcLogH)  == dwH);
    BOOL  bWholeDest  = ((1U << dwDestLogW) == dwW) && ((1U << dwDestLogH) == dwH);
    BOOL  bFullTex    = bWholeSrc && bWholeDest && (dwSrcLogW == dwDestLogW) && (dwSrcLogH == dwDestLogH);
    DWORD dwMemory;

    /*
     * sanity check
     */
    if (!dwH || !dwW || !dwSrcAddr || !dwDestAddr) return FALSE;

    /*
     * can we perform this operation using HW?
     */
    if (dwFlags & NV_SWIZFLAG_ALLOWHW)
    {
        /*
         * break into different swizzle cases
         */
        switch (dwFlags & (NV_SWIZFLAG_SRCSWIZZLED | NV_SWIZFLAG_DESTSWIZZLED))
        {
            case NV_SWIZFLAG_DESTSWIZZLED:  // lin -> swz
            {
                /*
                 * is the source and dest in HW reachable locations?
                 */
                if ((dwSrcAlloc & (NV4_TEXLOC_AGP | NV4_TEXLOC_VID | NV4_TEXLOC_PCI))
                 && (dwDestAlloc & NV4_TEXLOC_VID)
                 && pSrcTexture
                 && pDestTexture)
                {
                    DWORD dwIndex = TEX_SWIZZLE_INDEX(pDestTexture->dwTextureFlags);
                    DWORD dwImageColorFormat;
                    DWORD dwSurfaceColorFormat;

                    /*
                     * program HW
                     */
#ifdef HW_PAL8
                    if (dwFlags & NV_SWIZFLAG_PALETTIZED)
                    {
                        // setup
                        switch (pDestTexture->dwTextureColorFormat) {
                            case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                            case NV054_FORMAT_COLOR_LE_R5G6B5:
                            case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                            case NV054_FORMAT_COLOR_LE_A4R4G4B4:
                                dwSurfaceColorFormat = NV042_SET_COLOR_FORMAT_LE_R5G6B5;
                                break;
                            case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                            case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                                dwSurfaceColorFormat = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;
                                break;
                        }

                        // prepare the palette
                        if (!nvPreparePalette(pSrcTexture)) return FALSE;

                        // swizzle the palettized texture
                        nvSwizzleBlt(dwSrcAddr, dwSrcAlloc,
                                     dwSrcLogW, dwSrcLogH,
                                     dwSrcX0, dwSrcY0,
                                     dwSrcX1, dwSrcY1,
                                     dwW,   // pitch == width for 8 bpp
                                     pSrcTexture,
                                     dwDestAddr, dwDestAlloc,
                                     dwDestLogW, dwDestLogH,
                                     dwDestX, dwDestY,
                                     dwW,   // pitch == width for 8 bpp
                                     pDestTexture,
                                     1,     // 1 BPP
                                     dwFlags & ~(NV_SWIZFLAG_PALETTIZED |
                                                 NV_SWIZFLAG_ALLOWHW));

                        // de-palettize the texture
                        nvHWSwizzleBltPal8 (pCurrentContext->dwTexturePaletteOffset,
                                            pSrcTexture->dwLinearAddr,
                                            pDestTexture->dwSwizzleOffset,
                                            pDestTexture->dwMipMapBaseU,
                                            pDestTexture->dwMipMapBaseV,
                                            dwSurfaceColorFormat,
                                            pDestTexture->dwBPP,
                                            dwW,dwH);
                    }
                    else
#endif  // HW_PAL8
                    {
                        /*
                         * prepare for blt
                         */
                        switch (pDestTexture->dwTextureColorFormat) {
                            case NV054_FORMAT_COLOR_LE_X1R5G5B5:
                            case NV054_FORMAT_COLOR_LE_R5G6B5:
                            case NV054_FORMAT_COLOR_LE_A1R5G5B5:
                            case NV054_FORMAT_COLOR_LE_A4R4G4B4:
                                dwImageColorFormat   = NV077_SET_COLOR_FORMAT_LE_R5G6B5;
#ifdef DIRECTSWIZZLE
                                dwSurfaceColorFormat = NV042_SET_COLOR_FORMAT_LE_R5G6B5;
#else
                                dwSurfaceColorFormat = NV052_SET_FORMAT_COLOR_LE_R5G6B5;
#endif
                                break;
                            case NV054_FORMAT_COLOR_LE_X8R8G8B8:
                            case NV054_FORMAT_COLOR_LE_A8R8G8B8:
                                dwImageColorFormat   = NV077_SET_COLOR_FORMAT_LE_A8R8G8B8;
#ifdef DIRECTSWIZZLE
                                dwSurfaceColorFormat = NV042_SET_COLOR_FORMAT_LE_A8R8G8B8;
#else
                                dwSurfaceColorFormat = NV052_SET_FORMAT_COLOR_LE_A8R8G8B8;
#endif
                                break;
                        }

                        nvHWSwizzleBlt (pSrcTexture->dwLinearOffset,
                                        pSrcTexture->dwPitch,
                                        dwImageColorFormat,
                                        dwSrcX0,dwSrcY0,
                                        dwSrcAlloc,
                                        pDestTexture->dwSwizzleOffset[dwIndex],
                                        pDestTexture->dwMipMapBaseU,pDestTexture->dwMipMapBaseV,
                                        dwSurfaceColorFormat,
                                        dwDestX,dwDestY,
                                        dwW,dwH
#ifdef DIRECTSWIZZLE
                                        ,pDestTexture->dwBPP
#endif
                                        );
#if 0
NV_D3D_GLOBAL_SAVE();
nvFlushDmaBuffers();
NV_D3D_GLOBAL_SETUP();
#endif
                    }

                    /*
                     * block other processes from reading or writing until we have finished
                     */
                    pDestTexture->dwRetireDate[dwIndex] = global.dwSWTextureDate;
                    nvTextureSetBlockPoint();

                    /*
                     * done
                     */
                    return TRUE;
                }
            }
        }
    }

    /*
     * quicky memory configuration bit field
     *  1 == src  is uncached,
     *  2 == dest is uncached
     */
    dwMemory    = ((dwSrcAlloc  & (NV4_TEXLOC_AGP | NV4_TEXLOC_VID)) ? 1 : 0)
                | ((dwDestAlloc & (NV4_TEXLOC_AGP | NV4_TEXLOC_VID)) ? 2 : 0);

    /*
     * CPU has to do the work...
     *
     * sync HW & CPU for the given textures
     */
#ifdef NV_PROFILE // remove texture wait from these timings
    NVP_STOP (NVP_T_TEXSWBLT);
    NVP_STOP (NVP_T_TEXHWBLT);
    //NVP_START (NVP_X_FLOAT0);
#endif
    if (pSrcTexture)  nvTextureBlock (pSrcTexture ->dwRetireDate[(dwFlags & NV_SWIZFLAG_SRCSWIZZLED)  ? (TEX_SWIZZLE_INDEX(pSrcTexture-> dwTextureFlags)) : NV4_TEXCOUNT_SWIZZLE]);
    if (pDestTexture) nvTextureBlock (pDestTexture->dwRetireDate[(dwFlags & NV_SWIZFLAG_DESTSWIZZLED) ? (TEX_SWIZZLE_INDEX(pDestTexture->dwTextureFlags)) : NV4_TEXCOUNT_SWIZZLE]);
#ifdef NV_PROFILE
    //NVP_STOP (NVP_X_FLOAT0);
    //nvpLogTime (NVP_X_FLOAT0,nvpTime[NVP_X_FLOAT0]);
    NVP_RESTART (NVP_T_TEXSWBLT);
    NVP_RESTART (NVP_T_TEXHWBLT);
#endif

    /*
     * break into different swizzle cases
     */
    switch (dwFlags & (NV_SWIZFLAG_SRCSWIZZLED | NV_SWIZFLAG_DESTSWIZZLED))
    {
        case 0:                         // lin -> lin
        {
            DWORD dwSubHeight = 4096 / (dwW * dwBPP);
            if (!dwSubHeight) dwSubHeight = 1;
                         else dwSubHeight = min(dwSubHeight,dwH);

            //OutputDebugString ("nvSwizzleBlt_LL_ST_RW\n");
            nvSwizzleBlt_LL_ST_RW (dwSrcAddr + dwSrcY0 * dwSrcPitch + dwSrcX0 * dwBPP,
                                   dwSrcPitch,
                                   dwDestAddr + dwDestY * dwDestPitch + dwDestX * dwBPP,
                                   dwDestPitch,
                                   dwW,dwH,dwSubHeight,dwBPP);
            break;
        }
        case NV_SWIZFLAG_SRCSWIZZLED:   // swz -> lin
        {
            if (/*!bFullTex*/1) // txtodo - fast fulltexture deswizzle needed
            {
                DWORD dwSrcOffset;
                DWORD dwSrcDUDXor;
                DWORD dwSrcDUDXand;
                DWORD dwSrcDVDYor;
                DWORD dwSrcDVDYand;

                DWORD dwSubHeight = 4096 / (dwW * dwBPP);
                if (!dwSubHeight) dwSubHeight = 1;
                             else dwSubHeight = min(dwSubHeight,dwH);

                nvSwizzleCalcSwizzleInfo (dwSrcX0,dwSrcY0,dwSrcLogW,dwSrcLogH,dwBPP,
                                          &dwSrcOffset,
                                          &dwSrcDUDXor,&dwSrcDUDXand,
                                          &dwSrcDVDYor,&dwSrcDVDYand);

                //OutputDebugString ("nvSwizzleBlt_SL_ST_LW\n");
                nvSwizzleBlt_SL_ST_LW (dwSrcAddr,dwSrcOffset,
                                       dwSrcDUDXor,dwSrcDUDXand,
                                       dwSrcDVDYor,dwSrcDVDYand,
                                       dwDestAddr + dwDestY * dwDestPitch + dwDestX * dwBPP,
                                       dwDestPitch,
                                       dwW,dwH,dwSubHeight,dwBPP);
            }
            else
            {
            }
        }
        case NV_SWIZFLAG_DESTSWIZZLED:  // lin -> swz
        {
            if (!bFullTex)
            {
                DWORD dwDestOffset;
                DWORD dwDestDUDXor;
                DWORD dwDestDUDXand;
                DWORD dwDestDVDYor;
                DWORD dwDestDVDYand;

                DWORD dwSubHeight = 4096 / (dwW * dwBPP);
                if (!dwSubHeight) dwSubHeight = 1;
                             else dwSubHeight = min(dwSubHeight,dwH);

                nvSwizzleCalcSwizzleInfo (dwDestX,dwDestY,dwDestLogW,dwDestLogH,dwBPP,
                                          &dwDestOffset,
                                          &dwDestDUDXor,&dwDestDUDXand,
                                          &dwDestDVDYor,&dwDestDVDYand);

                //OutputDebugString ("nvSwizzleBlt_LS_ST_LR\n");
                nvSwizzleBlt_LS_ST_LR (dwSrcAddr + dwSrcY0 * dwSrcPitch + dwSrcX0 * dwBPP,
                                       dwSrcPitch,
                                       dwDestAddr,dwDestOffset,
                                       dwDestDUDXor,dwDestDUDXand,
                                       dwDestDVDYor,dwDestDVDYand,
                                       dwW,dwH,dwSubHeight,dwBPP);
            }
            else
            {
#ifdef HW_PAL8
                if (pDestTexture->dwTextureFlags & NV4_TEXFLAG_PALETTIZED)
                {
                    LPPALETTEENTRY pPalette;
                    DWORD          i;

                    nvTextureBase_meta = dwDestAddr;

                    if (pDestTexture->lpLcl->lpDDPalette)
                    {
                        pPalette = pDestTexture->lpLcl->lpDDPalette->lpLcl->lpGbl->lpColorTable;
                        for (i=0; i<256; i++)
                        {
                            extern WORD nvPalette[256];

                            nvPalette[i] = ((WORD)(pPalette[i].peRed   & 0xF8) << 8)
                                         | ((WORD)(pPalette[i].peGreen & 0xFC) << 3)
                                         | ((WORD)(pPalette[i].peBlue  & 0xF8) >> 3);
                        }
                    }
                    else
                    {
                        // no palette - what now?
                        // we just do the work anyway withou the correct palette
                    }

                    nvSwizzleBlt_LS_FT (dwSrcAddr,dwSrcPitch,
                                        dwDestAddr,
                                        dwSrcLogW,dwSrcLogH,1);
                }
                else
                {
                    nvTextureBase_meta = 0;
                    nvSwizzleBlt_LS_FT (dwSrcAddr,dwSrcPitch,
                                        dwDestAddr,
                                        dwSrcLogW,dwSrcLogH,dwBPP);
                }
#else
                nvSwizzleBlt_LS_FT (dwSrcAddr,dwSrcPitch,
                                    dwDestAddr,
                                    dwSrcLogW,dwSrcLogH,dwBPP);
#endif
            }
            break;
        }
        case NV_SWIZFLAG_SRCSWIZZLED | NV_SWIZFLAG_DESTSWIZZLED: // swz -> swz
        {
            if (!bFullTex)
            {
                DWORD dwSrcOffset;
                DWORD dwSrcDUDXor;
                DWORD dwSrcDUDXand;
                DWORD dwSrcDVDYor;
                DWORD dwSrcDVDYand;

                DWORD dwDestOffset;
                DWORD dwDestDUDXor;
                DWORD dwDestDUDXand;
                DWORD dwDestDVDYor;
                DWORD dwDestDVDYand;

                DWORD dwSubHeight = 4096 / (dwW * dwBPP);
                if (!dwSubHeight) dwSubHeight = 1;
                             else dwSubHeight = min(dwSubHeight,dwH);

                nvSwizzleCalcSwizzleInfo (dwSrcX0,dwSrcY0,dwSrcLogW,dwSrcLogH,dwBPP,
                                          &dwSrcOffset,
                                          &dwSrcDUDXor,&dwSrcDUDXand,
                                          &dwSrcDVDYor,&dwSrcDVDYand);

                nvSwizzleCalcSwizzleInfo (dwDestX,dwDestY,dwDestLogW,dwDestLogH,dwBPP,
                                          &dwDestOffset,
                                          &dwDestDUDXor,&dwDestDUDXand,
                                          &dwDestDVDYor,&dwDestDVDYand);

                //OutputDebugString ("nvSwizzleBlt_SS_ST_XX\n");
                nvSwizzleBlt_SS_ST_XX (dwSrcAddr,dwSrcOffset,
                                       dwSrcDUDXor,dwSrcDUDXand,
                                       dwSrcDVDYor,dwSrcDVDYand,
                                       dwDestAddr,dwDestOffset,
                                       dwDestDUDXor,dwDestDUDXand,
                                       dwDestDVDYor,dwDestDVDYand,
                                       dwW,dwH,dwSubHeight,dwBPP);
            }
            else
            {
                DWORD dwSubHeight = 4096 / (dwBPP << dwSrcLogW);
                if (!dwSubHeight) dwSubHeight = 1;
                             else dwSubHeight = min(dwSubHeight,(DWORD)(1 << dwSrcLogH));

                //OutputDebugString ("nvSwizzleBlt_SS_XX_RW\n");
                nvSwizzleBlt_SS_XX_RW (dwSrcAddr,
                                       dwDestAddr,
                                       1 << dwSrcLogW,1 << dwSrcLogH,dwSubHeight,dwBPP);
            }
            break;
        }
    }

#if 0
    /*
     * fill dest with solid color
     */
    {
        DWORD dl = dwDestAddr;
        DWORD x,y;

        for (y=0; y<dwH; y++)
        {
            DWORD da = dl;

            for (x=0; x<dwW; x++)
            {
                *(WORD*)da = 0xaa55;
                da += 2;
            }

            dl += dwDestPitch;
        }
    }
#endif

#if 0
    /*
     * show what we have done
     */
    /*if (dwFlags & NV_SWIZFLAG_DESTSWIZZLED)*/ {
        DWORD sl = dwSrcAddr;
        DWORD dl = dwDestAddr;
        DWORD vl = pDriverData->CurrentVisibleSurfaceAddress;

        DWORD x,y;

        for (y=0; y<dwH; y++)
        {
            DWORD sa = sl;
            DWORD da = dl;
            DWORD va = vl;

            for (x=0; x<dwW; x++)
            {
                *(WORD*)va            = *(WORD*)sa;
                *(WORD*)(va + dwW* 2) = *(WORD*)da;

                sa += 2;
                da += 2;
                va += 2;
            }

            sl += dwSrcPitch;
            dl += dwDestPitch;
            vl += 1024 * 2;
        }

        DPF("nvSwizzleBlt - dwFlags    = %08x",dwFlags);
        DPF("               dwSrcAddr  = %08x",dwSrcAddr);
        DPF("               dwDestAddr = %08x",dwDestAddr);
        DPF("               dwW        = %d",dwW);
        DPF("               dwH        = %d",dwH);
        __asm int 3;
    }
#endif

    /*
     * done
     */
    return TRUE;
}

#endif //NV_TEX2
