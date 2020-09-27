/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVSWIZ.CPP                                                        *
*   CPU Texture swizzling routines                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 10/20/98 - wrote it                     *
*                                                                           *
\***************************************************************************/
#include "precomp.h"
#include "nvutil.h"
#include "nvinit.h"

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

#define movaps_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x28 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movaps_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x29 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x10 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x11 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define prefetch_rm8(h,rm,ofs)      __asm _emit 0x0f __asm _emit 0x18 __asm _emit (mREG(h) | (rm)) __asm _emit (ofs)

/*
 * swizzle table buffer layout
 */
BYTE nvTexelScratchBuffer[8192];  // must be as wide as the widest pitch (4 * 2048 = 8192)

/*****************************************************************************
 * swizzle address generation
 *****************************************************************************/
typedef struct
{
    DWORD dwBase;
    DWORD dwOffset;
    DWORD dwLine;

    DWORD dwDUDXOr;
    DWORD dwDUDXAnd;
    DWORD dwDUDXAdd;

    DWORD dwDVDYOr;
    DWORD dwDVDYAnd;
    DWORD dwDVDYAdd;
} SWIZZLE_ADDR;

__inline DWORD nvSwizzleAddrGet (SWIZZLE_ADDR *lpSA)
{
    return lpSA->dwBase + lpSA->dwOffset;
}

__inline void nvSwizzleAddrNext (SWIZZLE_ADDR *lpSA)
{
    DWORD u,v;

    u              = ((lpSA->dwOffset | lpSA->dwDUDXOr) + lpSA->dwDUDXAdd) & lpSA->dwDUDXAnd;
    v              =   lpSA->dwOffset & lpSA->dwDVDYAnd;
    lpSA->dwOffset = u | v;
}

__inline void nvSwizzleAddrNewLine (SWIZZLE_ADDR *lpSA)
{
    DWORD u,v;

    u            =   lpSA->dwLine & lpSA->dwDUDXAnd;
    v            = ((lpSA->dwLine | lpSA->dwDVDYOr) + lpSA->dwDVDYAdd) & lpSA->dwDVDYAnd;
    lpSA->dwLine = lpSA->dwOffset = u | v;
}

void nvSwizzleAddrCreate
(
    SWIZZLE_ADDR *lpSA,
    DWORD         dwBase,
    DWORD         dwX,
    DWORD         dwY,
    DWORD         dwLogW,
    DWORD         dwLogH,
    DWORD         dwBPP
)
{
    if (!dwLogW)
    {
        lpSA->dwBase    = dwBase;
        lpSA->dwOffset  = lpSA->dwLine
                        = dwY * dwBPP;
        lpSA->dwDUDXAnd = 0;
        lpSA->dwDUDXOr  = 0;
        lpSA->dwDUDXAdd = 0;
        lpSA->dwDVDYAnd = ~0;
        lpSA->dwDVDYOr  = 0;
        lpSA->dwDVDYAdd = dwBPP;
    }
    else if (!dwLogH)
    {
        lpSA->dwBase    = dwBase;
        lpSA->dwOffset  = lpSA->dwLine
                        = dwX * dwBPP;
        lpSA->dwDUDXAnd = ~0;
        lpSA->dwDUDXOr  = 0;
        lpSA->dwDUDXAdd = dwBPP;
        lpSA->dwDVDYAnd = 0;
        lpSA->dwDVDYOr  = 0;
        lpSA->dwDVDYAdd = 0;
    }
    else
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

        lpSA->dwBase   = dwBase;
        lpSA->dwOffset = lpSA->dwLine
                       = ((dwLower & dwLowerMask) | dwUpperU | dwUpperV) * dwBPP;

        /*
         * calc masks
         */
        lpSA->dwDUDXOr  = (0xaaaaaaaa & dwLowerMask) * dwBPP;
        lpSA->dwDUDXAnd = ((dwLogW > dwLogH) ? (0x55555555 | dwUpperMask)
                                             : (0x55555555 & dwLowerMask)) * dwBPP;
        lpSA->dwDUDXAdd = dwBPP;
        lpSA->dwDVDYOr  = (0x55555555 & dwLowerMask) * dwBPP;
        lpSA->dwDVDYAnd = ((dwLogW < dwLogH) ? (0xaaaaaaaa | dwUpperMask)
                                             : (0xaaaaaaaa & dwLowerMask)) * dwBPP;
        lpSA->dwDVDYAdd = dwBPP << 1;
    }
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
    SWIZZLE_ADDR sa;
    DWORD        dwCount,dwSize,dwLine;
    DWORD        dwScratch0,dwScratch1;
    DWORD        x,y;
    void        *pMemory;

    /*
     * figure out memory requirements
     */
    dwCount = 1     << (dwLogW + dwLogH);
    dwSize  = dwBPP << (dwLogW + dwLogH);
    if (dwSize > 4096)
    {
        pMemory = malloc(dwSize * 2);
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
        DWORD u = dwSrcBase;
        DWORD v = dwScratch0;
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
     * prepare swizzle address generation
     */
    nvSwizzleAddrCreate (&sa,dwScratch1,0,0,dwLogW,dwLogH,dwBPP);

    /*
     * do swizzle
     */
    switch (dwBPP)
    {
        case 1:
        {
            for (y = (1 << dwLogH); y; y--)
            {
                for (x = (1 << dwLogW); x; x--)
                {
                    *(BYTE*)nvSwizzleAddrGet(&sa) = *(BYTE*)dwScratch0;

                    dwScratch0 += 1;
                    nvSwizzleAddrNext (&sa);
                }
                nvSwizzleAddrNewLine (&sa);
            }
            break;
        }
        case 2:
        {
            for (y = (1 << dwLogH); y; y--)
            {
                for (x = (1 << dwLogW); x; x--)
                {
                    *(WORD*)nvSwizzleAddrGet(&sa) = *(WORD*)dwScratch0;

                    dwScratch0 += 2;
                    nvSwizzleAddrNext (&sa);
                }
                nvSwizzleAddrNewLine (&sa);
            }
            break;
        }
        case 4:
        {
            for (y = (1 << dwLogH); y; y--)
            {
                for (x = (1 << dwLogW); x; x--)
                {
                    *(DWORD*)nvSwizzleAddrGet(&sa) = *(DWORD*)dwScratch0;

                    dwScratch0 += 4;
                    nvSwizzleAddrNext (&sa);
                }
                nvSwizzleAddrNewLine (&sa);
            }
            break;
        }
    }

    /*
     * write texels
     */
    nvMemCopy (dwDestBase,dwScratch1,dwSize,NV_MEMCOPY_WANTDESTALIGNED);

    /*
     * free memory
     */
    if (pMemory) free (pMemory);
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
    if (g_dwCPUFeatureSet & NV_FS_KATMAI)
    {
        switch (dwBPP)
        {
            case 2: // 16bpp Pentium3
            {
                __asm
                {
                    mov eax,offset nvTexelScratchBuffer
                    mov ecx,(MB * MB * 2) / 32
            xxx16:   prefetch_rm8 (1,rmIND8(rEAX), 0)
                     add eax,32
                     dec ecx
                    jnz xxx16

                    mov eax,[dwSrcBase]
                    prefetch_rm8 (1,rmIND8(rEAX), 0)
                    prefetch_rm8 (1,rmIND8(rEAX),32)
                }

                {
                    DWORD src = dwSrcBase;
                    DWORD dst = (DWORD)nvTexelScratchBuffer;
                    DWORD v = 0;
                    DWORD y;

                    for (y = MB; y; y--)
                    {
                        DWORD addr;

                        addr  = src;
                        src  += dwSrcPitch;
                        __asm
                        {
                            mov eax,[src]
                            prefetch_rm8 (1,rmIND8(rEAX), 0)
                            prefetch_rm8 (1,rmIND8(rEAX),32)
                        }


#define NXT1(u)  ((((u) | (MB_VMASK*2)) + 1*2) & (MB_UMASK*2))
#define NXT2(u)  NXT1(NXT1(u))
#define NXT4(u)  NXT2(NXT2(u))
#define NXT8(u)  NXT4(NXT4(u))
#define NXT16(u) NXT8(NXT8(u))
#define NXT32(u) NXT16(NXT16(u))

                        // MB assumed to be 32
                        __asm {
                            push esi
                            push edi
                            push ebx

                        // load 8 texels
                            mov eax,[addr]
                            mov ebx,[eax]
                            mov ecx,[eax + 4]
                            mov edx,[eax + 8]
                            mov edi,[eax + 12]

                            mov eax,[v]

                            mov esi,eax
                          //or esi,0
                            add esi,[dst]
                            mov [esi],ebx // 2 at a time

                            mov esi,eax
                            or  esi,NXT2(0)
                            add esi,[dst]
                            mov [esi],ecx

                            mov esi,eax
                            or  esi,NXT4(0)
                            add esi,[dst]
                            mov [esi],edx

                            mov esi,eax
                            or  esi,NXT4(NXT2(0))
                            add esi,[dst]
                            mov [esi],edi

                        // 8 more texels
                            mov eax,[addr]
                            mov ebx,[eax + 16]
                            mov ecx,[eax + 20]
                            mov edx,[eax + 24]
                            mov edi,[eax + 28]

                            mov eax,[v]

                            mov esi,eax
                            or  esi,NXT8(0)
                            add esi,[dst]
                            mov [esi],ebx // 2 at a time

                            mov esi,eax
                            or  esi,NXT8(NXT2(0))
                            add esi,[dst]
                            mov [esi],ecx

                            mov esi,eax
                            or  esi,NXT8(NXT4(0))
                            add esi,[dst]
                            mov [esi],edx

                            mov esi,eax
                            or  esi,NXT8(NXT4(NXT2(0)))
                            add esi,[dst]
                            mov [esi],edi

                        // 8 more texels
                            mov eax,[addr]
                            mov ebx,[eax + 32]
                            mov ecx,[eax + 36]
                            mov edx,[eax + 40]
                            mov edi,[eax + 44]

                            mov eax,[v]

                            mov esi,eax
                            or  esi,NXT16(0)
                            add esi,[dst]
                            mov [esi],ebx // 2 at a time

                            mov esi,eax
                            or  esi,NXT16(NXT2(0))
                            add esi,[dst]
                            mov [esi],ecx

                            mov esi,eax
                            or  esi,NXT16(NXT4(0))
                            add esi,[dst]
                            mov [esi],edx

                            mov esi,eax
                            or  esi,NXT16(NXT4(NXT2(0)))
                            add esi,[dst]
                            mov [esi],edi

                        // 8 more texels
                            mov eax,[addr]
                            mov ebx,[eax + 48]
                            mov ecx,[eax + 52]
                            mov edx,[eax + 56]
                            mov edi,[eax + 60]

                            mov eax,[v]

                            mov esi,eax
                            or  esi,NXT16(NXT8(0))
                            add esi,[dst]
                            mov [esi],ebx // 2 at a time

                            mov esi,eax
                            or  esi,NXT16(NXT8(NXT2(0)))
                            add esi,[dst]
                            mov [esi],ecx

                            mov esi,eax
                            or  esi,NXT16(NXT8(NXT4(0)))
                            add esi,[dst]
                            mov [esi],edx

                            mov esi,eax
                            or  esi,NXT16(NXT8(NXT4(NXT2(0))))
                            add esi,[dst]
                            mov [esi],edi

                            pop ebx
                            pop edi
                            pop esi
                        }

                        v = ((v | (MB_UMASK*2)) + 2*2) & (MB_VMASK*2);
                    }

                    nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*2,NV_MEMCOPY_WANTDESTALIGNED);
                }

                return;
            }
            case 4: // 32bpp Pentium3
            {
                __asm
                {
                    mov eax,offset nvTexelScratchBuffer
                    mov ecx,(MB * MB * 4) / 32
            xxx32:   prefetch_rm8 (1,rmIND8(rEAX), 0)
                     add eax,32
                     dec ecx
                    jnz xxx32

                    mov eax,[dwSrcBase]
                    prefetch_rm8 (1,rmIND8(rEAX), 0)
                    prefetch_rm8 (1,rmIND8(rEAX),32)
                }

                {
                    DWORD src = dwSrcBase;
                    DWORD dst = (DWORD)nvTexelScratchBuffer;
                    DWORD v = 0;
                    DWORD y;

                    for (y = MB; y; y--)
                    {
                        DWORD u,x;
                        DWORD addr;

                        addr  = src;
                        src  += dwSrcPitch;
                        __asm
                        {
                            mov eax,[src]
                            prefetch_rm8 (1,rmIND8(rEAX), 0)
                            prefetch_rm8 (1,rmIND8(rEAX),32)
                        }

                        u = 0;
                        for (x = MB; x; x--)
                        {
                            DWORD store = dst + (u | v);
                            *(WORD*)store = *(WORD*)addr;

                            addr += 4;
                            u = ((u | (MB_VMASK*4)) + 1*4) & (MB_UMASK*4);
                        }
                        v = ((v | (MB_UMASK*4)) + 2*4) & (MB_VMASK*4);
                    }

                    nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*4,NV_MEMCOPY_WANTDESTALIGNED);
                }

                return;
            }

        }
    }

    /*
     * default code
     */
    {
        DWORD src = dwSrcBase;
        DWORD dst = (DWORD)nvTexelScratchBuffer;
        DWORD v = 0;
        DWORD y;

        for (y = MB; y; y--)
        {
            DWORD u,x;
            DWORD addr;

            addr  = src;
            src  += dwSrcPitch;

            u = 0;
            for (x = MB; x; x--)
            {
                DWORD store = dst + (u | v);
                *(WORD*)store = *(WORD*)addr;

                addr += dwBPP;
                u     = ((u | (MB_VMASK*dwBPP)) + 1*dwBPP) & (MB_UMASK*dwBPP);
            }
            v = ((v | (MB_UMASK*dwBPP)) + 2*dwBPP) & (MB_VMASK*dwBPP);
        }

        nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*dwBPP,NV_MEMCOPY_WANTDESTALIGNED);
    }
}

/*
 * nvSwizzleBlt_Lin_Swz_FullTexture
 *
 * software swizzle algorithm for full textures
 *  full texture means:
 *    source width & height == dest width & height
 *    blt rectangle is (0,0) - (w,h)
 */
void nvSwizzleBlt_Lin_Swz_FullTexture
(
    DWORD dwSrcBase,
    DWORD dwSrcPitch,

    DWORD dwDestBase,

    DWORD dwLogW,
    DWORD dwLogH,
    DWORD dwBPP
)
{
    SWIZZLE_ADDR sa;
    DWORD        dwMBPP,dwMBPP2;
    DWORD        x,y;

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
     * optimize for thin vertical case (swizzle bit does not work for this)
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
     * prepare swizzle address generation
     */
    nvSwizzleAddrCreate (&sa,dwDestBase,0,0,dwLogW,dwLogH,dwMBPP2);

    /*
     * do macroblock swizzle
     */
    for (y = (1 << dwLogH); y; y--)
    {
        DWORD dwSrc = dwSrcBase;

        for (x = (1 << dwLogW); x; x--)
        {
            nvSwizzleMacroBlock_LS (dwSrc,dwSrcPitch,
                                    nvSwizzleAddrGet(&sa),
                                    dwBPP);

            nvSwizzleAddrNext (&sa);
            dwSrc += dwBPP << L2MB;
        }

        nvSwizzleAddrNewLine (&sa);
        dwSrcBase += dwSrcPitch << L2MB;
    }
}

/*
 * nvSwizzleBlt_Lin_Lin
 *
 * simple SW bit-blt
 */
void nvSwizzleBlt_Lin_Lin
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

/*
 * nvSwizzleBlt_Swz_Lin
 *
 * deswizzle anything
 */
void nvSwizzleBlt_Swz_Lin
(
    SWIZZLE_ADDR *lpSA,

    DWORD         dwDestAddr,
    DWORD         dwDestPitch,

    DWORD         dwWidth,
    DWORD         dwHeight,
    DWORD         dwSubHeight,
    DWORD         dwBPP
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
         * read swizzle
         */
        a = (DWORD)&nvTexelScratchBuffer;

        for (h = f; h; h--)
        {
            DWORD x;

            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (a,nvSwizzleAddrGet(lpSA),dwBPP);
                a += dwBPP;
                nvSwizzleAddrNext (lpSA);
            }

            nvSwizzleAddrNewLine (lpSA);
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

/*
 * nvSwizzleBlt_Lin_Swz
 *
 * swizzles anything
 */
void nvSwizzleBlt_Lin_Swz
(
    DWORD         dwSrcAddr,
    DWORD         dwSrcPitch,

    SWIZZLE_ADDR *lpSA,

    DWORD         dwWidth,
    DWORD         dwHeight,
    DWORD         dwSubHeight,
    DWORD         dwBPP
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
            DWORD x;

            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (nvSwizzleAddrGet(lpSA),a,dwBPP);
                a += dwBPP;
                nvSwizzleAddrNext (lpSA);
            }
            nvSwizzleAddrNewLine (lpSA);
        }

        y -= f;
    }
}

/*
 * nvSwizzleBlt_Swz_Swz
 *
 * swizzle - swizzle copy, handles anything
 */
void nvSwizzleBlt_Swz_Swz
(
    SWIZZLE_ADDR *lpSASrc,

    SWIZZLE_ADDR *lpSADest,

    DWORD         dwWidth,
    DWORD         dwHeight,
    DWORD         dwSubHeight,
    DWORD         dwBPP
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
            DWORD x;

            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (a,nvSwizzleAddrGet(lpSASrc),dwBPP);
                a += dwBPP;
                nvSwizzleAddrNext (lpSASrc);
            }
            nvSwizzleAddrNewLine (lpSASrc);
        }

        /*
         * write swizzle
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            DWORD x;

            for (x = dwWidth; x; x--)
            {
                TEXELCOPY (nvSwizzleAddrGet(lpSADest),a,dwBPP);
                a += dwBPP;
                nvSwizzleAddrNext (lpSADest);
            }
            nvSwizzleAddrNewLine (lpSADest);
        }

        y -= f;
    }
}

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
    DWORD         dwSrcLogW,
    DWORD         dwSrcLogH,
    DWORD         dwSrcX0,
    DWORD         dwSrcY0,
    DWORD         dwSrcX1,
    DWORD         dwSrcY1,
    DWORD         dwSrcPitch,

    DWORD         dwDestAddr,
    DWORD         dwDestLogW,
    DWORD         dwDestLogH,
    DWORD         dwDestX,
    DWORD         dwDestY,
    DWORD         dwDestPitch,

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

    /*
     * sanity check
     */
    if (!dwH || !dwW || !dwSrcAddr || !dwDestAddr) return FALSE;

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

            nvSwizzleBlt_Lin_Lin  (dwSrcAddr + dwSrcY0 * dwSrcPitch + dwSrcX0 * dwBPP,
                                   dwSrcPitch,
                                   dwDestAddr + dwDestY * dwDestPitch + dwDestX * dwBPP,
                                   dwDestPitch,
                                   dwW,dwH,dwSubHeight,dwBPP);
            break;
        }
        case NV_SWIZFLAG_SRCSWIZZLED:   // swz -> lin
        {
            SWIZZLE_ADDR sa;
            DWORD        dwSubHeight = 4096 / (dwW * dwBPP);
            if (!dwSubHeight) dwSubHeight = 1;
                         else dwSubHeight = min(dwSubHeight,dwH);

            nvSwizzleAddrCreate (&sa,dwSrcAddr,dwSrcX0,dwSrcY0,dwSrcLogW,dwSrcLogH,dwBPP);

            nvSwizzleBlt_Swz_Lin (&sa,
                                  dwDestAddr + dwDestY * dwDestPitch + dwDestX * dwBPP,
                                  dwDestPitch,
                                  dwW,dwH,dwSubHeight,dwBPP);
        }
        case NV_SWIZFLAG_DESTSWIZZLED:  // lin -> swz
        {
            if (!bFullTex)
            {
                SWIZZLE_ADDR sa;
                DWORD        dwSubHeight = 4096 / (dwW * dwBPP);
                if (!dwSubHeight) dwSubHeight = 1;
                             else dwSubHeight = min(dwSubHeight,dwH);

                nvSwizzleAddrCreate (&sa,dwDestAddr,dwDestX,dwDestY,dwDestLogW,dwDestLogH,dwBPP);

                nvSwizzleBlt_Lin_Swz (dwSrcAddr + dwSrcY0 * dwSrcPitch + dwSrcX0 * dwBPP,
                                      dwSrcPitch,
                                      &sa,
                                      dwW,dwH,dwSubHeight,dwBPP);
            }
            else
            {
                nvSwizzleBlt_Lin_Swz_FullTexture (dwSrcAddr,dwSrcPitch,
                                                  dwDestAddr,
                                                  dwSrcLogW,dwSrcLogH,dwBPP);
            }
            break;
        }
        case NV_SWIZFLAG_SRCSWIZZLED | NV_SWIZFLAG_DESTSWIZZLED: // swz -> swz
        {
            SWIZZLE_ADDR saSrc;
            SWIZZLE_ADDR saDest;
            DWORD        dwSubHeight = 4096 / (dwW * dwBPP);
            if (!dwSubHeight) dwSubHeight = 1;
                         else dwSubHeight = min(dwSubHeight,dwH);

            nvSwizzleAddrCreate (&saSrc, dwSrcAddr, dwSrcX0,dwSrcY0,dwSrcLogW, dwSrcLogH, dwBPP);
            nvSwizzleAddrCreate (&saDest,dwDestAddr,dwDestX,dwDestY,dwDestLogW,dwDestLogH,dwBPP);

            nvSwizzleBlt_Swz_Swz (&saSrc,
                                  &saDest,
                                  dwW,dwH,dwSubHeight,dwBPP);
            break;
        }
    }

    return TRUE;
}
