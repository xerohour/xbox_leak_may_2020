 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvSwiz.cpp                                                        *
*   Texture swizzling routines (some HW dependence).                        *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 10/20/98 - wrote it                     *
*       Ben de Waal                 07/07/99 - rewrote it                   *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#include "x86.h"

// swizzling flags
#define NV_SWIZFLAG_SRCLINEAR       0x00000000
#define NV_SWIZFLAG_SRCSWIZZLED     0x00000001
#define NV_SWIZFLAG_DESTLINEAR      0x00000000
#define NV_SWIZFLAG_DESTSWIZZLED    0x00000002
#define NV_SWIZFLAG_PALETTIZED      0x00000004
#define NV_SWIZFLAG_ALLOWHW         0x80000000

#define NV_SWIZVAL_MAX              16

BOOL nvSwizzleBlt (DWORD     dwSrcAddr,
                   DWORD     dwSrcAlloc,
                   DWORD     dwSrcLogW,
                   DWORD     dwSrcLogH,
                   DWORD     dwSrcX0,
                   DWORD     dwSrcY0,
                   DWORD     dwSrcX1,
                   DWORD     dwSrcY1,
                   DWORD     dwSrcPitch,
                   CTexture *pSrcTexture,  /* may be NULL if no texture is involved */
                   DWORD     dwDestAddr,
                   DWORD     dwDestAlloc,
                   DWORD     dwDestLogW,
                   DWORD     dwDestLogH,
                   DWORD     dwDestX,
                   DWORD     dwDestY,
                   DWORD     dwDestPitch,
                   CTexture *pDestTexture, /* may be NULL if no texture is involved */
                   DWORD     dwBPP,
                   DWORD     dwFlags);



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

#define TEXELCOPY(d,s,bpp)                            \
{                                                     \
    if ((bpp) == 2) *(WORD*)(d)  = *(WORD*)(s);       \
    else if ((bpp) == 4) *(DWORD*)(d) = *(DWORD*)(s); \
    else *(BYTE*)(d)  = *(BYTE*)(s);                  \
}

#define PREF_POLICY 0
#define NUM_LINES 2
#define movaps_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x28 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movaps_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x29 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x10 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x11 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define prefetch_rm8(h,rm,ofs)      __asm _emit 0x0f __asm _emit 0x18 __asm _emit (mREG(h) | (rm)) __asm _emit (ofs)
#define _femms                      __asm _emit 0x0f __asm _emit 0x0e

// Prefetch the first couple of lines of a macro block for the Texture Blt code
// This handles 16bpp textures

__forceinline void MB_prefetch16(DWORD dwSrcBase, DWORD dwSrcPitch)
{
#ifdef PREFETCH_TESTING
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_PREFETCH)
    {
        __asm
        {
            mov eax,[dwSrcBase]
    #if (NUM_LINES > 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
    #if (NUM_LINES > 1)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
    #if (NUM_LINES > 2)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
    #if (NUM_LINES > 3)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
    #if (NUM_LINES > 4)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
    #if (NUM_LINES > 5)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
    #if (NUM_LINES > 6)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
    #if (NUM_LINES > 7)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
    #endif
        }
    }
#else
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & (FS_WILLAMETTE | FS_ATHLON))
    {
        __asm
        {
            mov eax,[dwSrcBase]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
        }
    }
    else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
    {
        __asm
        {
            mov eax,[dwSrcBase]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
        }

    }


#endif

}

// Prefetch the first couple of lines of a macro block for the Texture Blt code
// This handles 32bpp textures

__forceinline void MB_prefetch32(DWORD dwSrcBase, DWORD dwSrcPitch)
{
#ifdef PREFETCH_TESTING
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_PREFETCH)
    {
        __asm
        {
            mov eax,[dwSrcBase]
    #if (NUM_LINES > 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 1)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 2)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 3)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 4)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 5)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 6)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 7)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
    #if (NUM_LINES > 8)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
    #endif
        }
    }
#else
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & (FS_WILLAMETTE | FS_ATHLON))
    {
        __asm
        {
            mov eax,[dwSrcBase]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
        }
    }
    else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
    {
        __asm
        {
            mov eax,[dwSrcBase]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
            add eax,[dwSrcPitch]
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 0)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 32)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 64)
            prefetch_rm8 (PREF_POLICY,rmIND8(rEAX), 96)
        }
    }
#endif
}


/*
 * swizzle table buffer layout
 */
BYTE nvTexelScratchBuffer[8192];  // must be as wide as the widest pitch (4 * 2048 = 8192)

/*****************************************************************************
 * swizzle address generation
 *****************************************************************************/
struct SWIZZLE_ADDR
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
};

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
        DWORD u = dwSrcBase;
        DWORD v = dwScratch0;
        for (y = (1 << dwLogH); y; y--)
        {
            nvMemCopy (v,u,dwLine);
            v += dwLine;
            u += dwSrcPitch;
        }
    }
    else
    {
        nvMemCopy (dwScratch0,dwSrcBase,dwLine << dwLogH);
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
    nvMemCopy (dwDestBase,dwScratch1,dwSize);

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
    DWORD numLines = (pDriverData->nvD3DPerfData.dwCPUFeatureSet & (FS_WILLAMETTE | FS_ATHLON)) ? 3 : 2;

    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_PREFETCH)
    {
        switch (dwBPP)
        {
            case 1:
            {
                break;  //falling through to default supporrt
                        //TODO add KNI support for 8 bpp textures
            }
            case 2: // 16bpp Pentium3
            {

                {
                    DWORD src = dwSrcBase;
                    DWORD pref = src+numLines*dwSrcPitch;
                    DWORD dst = (DWORD)nvTexelScratchBuffer;
                    DWORD v = 0;
                    DWORD y;

                    for (y = MB; y; y--)
                    {
                        DWORD addr;

                        addr  = src;
                        src  += dwSrcPitch;

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
#if (NUM_LINES > 0)
                            mov ecx,[pref]
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX), 0)
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX),32)
                            add ecx, [dwSrcPitch]
                            mov [pref], ecx
#endif

                            pop ebx
                            pop edi
                            pop esi
                        }

                        v = ((v | (MB_UMASK*2)) + 2*2) & (MB_VMASK*2);
                    }

                    nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*2);
                }

                return;
            }

            case 4:
            {
                DWORD src = dwSrcBase;
                DWORD pref = src+numLines*dwSrcPitch;
                DWORD dst = (DWORD)nvTexelScratchBuffer;
                DWORD v = 0;
                DWORD y;

                if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
                {
                    _femms
                }
                for (y = MB; y; y--)
                {
                    DWORD u;
                    DWORD addr;

                    addr  = src;
                    src  += dwSrcPitch;

                    u = 0;
#undef NXT1
#undef NXT2
#undef NXT4
#undef NXT8
#undef NXT16
#undef NXT32
#define NXT1(u)  ((((u) | (MB_VMASK*4)) + 1*4) & (MB_UMASK*4))
#define NXT2(u)  NXT1(NXT1(u))
#define NXT4(u)  NXT2(NXT2(u))
#define NXT8(u)  NXT4(NXT4(u))
#define NXT16(u) NXT8(NXT8(u))
#define NXT32(u) NXT16(NXT16(u))
                        __asm {
                            push esi
                            push edi
                            push ebx

#if (NUM_LINES == 0)
                            mov ecx,[pref]
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX), 0)
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX),32)
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX), 64)
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX), 96)
                            add ecx, [dwSrcPitch]
                            mov [pref], ecx
#endif
                        // load 8 texels
                            mov eax,[addr]
                            movq    mm0 ,[eax]
                            movq    mm1,[eax + 8]
                            movq    mm2,[eax + 16]
                            movq    mm3,[eax + 24]

                            mov eax,[v]

                            mov esi,eax
                          //or esi,0
                            add esi,[dst]
                            movq [esi],mm0 // 2 at a time

                            mov esi,eax
                            or  esi,NXT2(0)
                            add esi,[dst]
                            movq [esi],mm1

                            mov esi,eax
                            or  esi,NXT4(0)
                            add esi,[dst]
                            movq [esi],mm2

                            mov esi,eax
                            or  esi,NXT4(NXT2(0))
                            add esi,[dst]
                            movq [esi],mm3

                        // 8 more texels
                            mov eax,[addr]
                            movq mm0,[eax + 32]
                            movq mm1,[eax + 40]
                            movq mm2,[eax + 48]
                            movq mm3,[eax + 56]

                            mov eax,[v]

                            mov esi,eax
                            or  esi,NXT8(0)
                            add esi,[dst]
                            movq [esi],mm0 // 2 at a time

                            mov esi,eax
                            or  esi,NXT8(NXT2(0))
                            add esi,[dst]
                            movq [esi],mm1

                            mov esi,eax
                            or  esi,NXT8(NXT4(0))
                            add esi,[dst]
                            movq [esi],mm2

                            mov esi,eax
                            or  esi,NXT8(NXT4(NXT2(0)))
                            add esi,[dst]
                            movq [esi],mm3


                        // 8 more texels
                            mov eax,[addr]
                            movq mm0,[eax + 64]
                            movq mm1,[eax + 72]
                            movq mm2,[eax + 80]
                            movq mm3,[eax + 88]

                            mov eax,[v]

                            mov esi,eax
                            or  esi,NXT16(0)
                            add esi,[dst]
                            movq [esi],mm0 // 2 at a time

                            mov esi,eax
                            or  esi,NXT16(NXT2(0))
                            add esi,[dst]
                            movq [esi],mm1

                            mov esi,eax
                            or  esi,NXT16(NXT4(0))
                            add esi,[dst]
                            movq [esi],mm2

                            mov esi,eax
                            or  esi,NXT16(NXT4(NXT2(0)))
                            add esi,[dst]
                            movq [esi],mm3


                            // 8 more texels
                            mov eax,[addr]
                            movq mm0,[eax + 96]
                            movq mm1,[eax + 104]
                            movq mm2,[eax + 112]
                            movq mm3,[eax + 120]

                            mov eax,[v]

                            mov esi,eax
                            or  esi,NXT16(NXT8(0))
                            add esi,[dst]
                            movq [esi],mm0 // 2 at a time

                            mov esi,eax
                            or  esi,NXT16(NXT8(NXT2(0)))
                            add esi,[dst]
                            movq [esi],mm1

                            mov esi,eax
                            or  esi,NXT16(NXT8(NXT4(0)))
                            add esi,[dst]
                            movq [esi],mm2

                            mov esi,eax
                            or  esi,NXT16(NXT8(NXT4(NXT2(0))))
                            add esi,[dst]
                            movq [esi],mm3

#if (NUM_LINES > 0)
                            mov ecx,[pref]
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX), 0)
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX),32)
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX), 64)
                            prefetch_rm8 (PREF_POLICY,rmIND8(rECX), 96)
                            add ecx, [dwSrcPitch]
                            mov [pref], ecx
#endif

                            pop ebx
                            pop edi
                            pop esi
                        }
                    v = ((v | (MB_UMASK*4)) + 2*4) & (MB_VMASK*4);
                }

                nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*4);
                if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
                {
                    _femms
                }
                else
                __asm { emms };
                return;
            }


        }
    }

    /*
     * default code
     */
    switch (dwBPP)
    {
        case 1:
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
                    DWORD store = dst + (u | v) * 1;
                    *(BYTE*)store = *(BYTE*)addr;

                    addr += 1;
                    u     = ((u | (MB_VMASK*1)) + 1*1) & (MB_UMASK*1);
                }
                v = ((v | (MB_UMASK*1)) + 2*1) & (MB_VMASK*1);
            }

            nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*1);
            break;
        }

        case 2:
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

                    addr += 2;
                    u     = ((u | (MB_VMASK*2)) + 1*2) & (MB_UMASK*2);
                }
                v = ((v | (MB_UMASK*2)) + 2*2) & (MB_VMASK*2);
            }

            nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*2);
            break;
        }

        case 4:
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
                    *(DWORD*)store = *(DWORD*)addr;

                    addr += 4;
                    u     = ((u | (MB_VMASK*4)) + 1*4) & (MB_UMASK*4);
                }
                v = ((v | (MB_UMASK*4)) + 2*4) & (MB_VMASK*4);
            }

            nvMemCopy (dwDestBase,(DWORD)nvTexelScratchBuffer,MB*MB*4);
            break;
        }
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

    dbgTracePush ("nvSwizzleBlt_Lin_Swz_FullTexture");

    /*
     * handle small cases
     */
    if ((dwLogW < L2MB) || (dwLogH < L2MB))
    {
        nvSwizzleSmallBlock (dwSrcBase,dwSrcPitch,
                             dwDestBase,
                             dwLogW,dwLogH,dwBPP);
        dbgTracePop();
        return;
    }

    /*
     * adjust width & height for macroblocks
     */
    dwLogW -= L2MB;
    dwLogH -= L2MB;
    dwMBPP  = dwBPP << L2MB;
    dwMBPP2 = dwBPP << (L2MB + L2MB);


    // Prefetch the Scratch buffer

    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_PREFETCH)
    {
        __asm
        {

            mov eax,offset nvTexelScratchBuffer
            mov ecx,(MB * MB * 4) / 32
    xxx32:   prefetch_rm8 (1,rmIND8(rEAX), 0)
             add eax,32
             dec ecx
            jnz xxx32
        }
    }

    /*
     * optimize for thin vertical case (swizzle bit does not work for this)
     */
    if (!dwLogW)
    {
        // Prefetch first 2 lines of the texture. nVSwizzleMacroBlock_LS prefetches 2 ahead
        if (dwBPP == 4) MB_prefetch32(dwSrcBase, dwSrcPitch);
        if (dwBPP == 2) MB_prefetch16(dwSrcBase, dwSrcPitch);

        for (y = (1 << dwLogH); y; y--)
        {
            nvSwizzleMacroBlock_LS (dwSrcBase,dwSrcPitch,
                                    dwDestBase,
                                    dwBPP);
            dwSrcBase  += dwSrcPitch << L2MB;
            dwDestBase += dwMBPP2;
        }
        dbgTracePop();
        return;
    }

    // Prefetch first 2 lines of the texture. nVSwizzleMacroBlock_LS prefetches 2 ahead
    if (dwBPP == 4) MB_prefetch32(dwSrcBase, dwSrcPitch);
    if (dwBPP == 2) MB_prefetch16(dwSrcBase, dwSrcPitch);
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

    dbgTracePop();
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

    dbgTracePush ("nvSwizzleBlt_Lin_Lin");

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
            nvMemCopy (a,dwSrcAddr,dwBytes);
            dwSrcAddr += dwSrcPitch;
            a         += dwBytes;
        }

        /*
         * write linear
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            nvMemCopy (dwDestAddr,a,dwBytes);
            a          += dwBytes;
            dwDestAddr += dwDestPitch;
        }

        y -= f;
    }

    dbgTracePop();
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

    dbgTracePush ("nvSwizzleBlt_Swz_Lin");

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
            nvMemCopy (dwDestAddr,a,dwBytes);
            a          += dwBytes;
            dwDestAddr += dwDestPitch;
        }

        y -= f;
    }

    dbgTracePop();
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

    dbgTracePush ("nvSwizzleBlt_Lin_Swz");

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
            nvMemCopy (a,dwSrcAddr,dwBytes);
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

    dbgTracePop();
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

    dbgTracePush ("nvSwizzleBlt_Swz_Swz");

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

    dbgTracePop();
}

/*****************************************************************************
 * HARDWARE SWIZZLE
 *****************************************************************************/

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
    DWORD dwLogWidth,
    DWORD dwLogHeight,
    DWORD dwSurfaceColorFormat,

    DWORD dwDstX0,
    DWORD dwDstY0,

    DWORD dwBlitWidth,
    DWORD dwBlitHeight
)
{
    dbgTracePush ("nvHWSwizzleBlt");

    // target surface
    nvglSetObject          (NV_DD_SPARE, D3D_CONTEXT_SURFACE_SWIZZLED);
    nvglSetSwizzledSurface (NV_DD_SPARE,
                            (DRF_NUM(052,_SET_FORMAT,_COLOR ,dwSurfaceColorFormat)
                            |DRF_NUM(052,_SET_FORMAT,_WIDTH ,dwLogWidth)
                            |DRF_NUM(052,_SET_FORMAT,_HEIGHT,dwLogHeight)),
                             dwDstSwizzleOffset);

    // format
    nvglSetObject               (NV_DD_SPARE, D3D_SCALED_IMAGE_FROM_MEMORY);
    nvglSetScaledImageFormat    (NV_DD_SPARE, dwImageColorFormat);

    // source memory context
    switch (dwSrcAlloc & CSimpleSurface::HEAP_LOCATION_MASK) {
        case CSimpleSurface::HEAP_AGP:
            nvglSetScaledImageContextImage (NV_DD_SPARE, D3D_CONTEXT_DMA_HOST_MEMORY);
            break;
        case CSimpleSurface::HEAP_VID:
            nvglSetScaledImageContextImage (NV_DD_SPARE, NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY);
            break;
        case CSimpleSurface::HEAP_PCI:
            nvglSetScaledImageContextImage (NV_DD_SPARE, D3D_CONTEXT_DMA_HOST_MEMORY);
            break;
        default:
            DPF ("unknown context");
            dbgD3DError();
            break;
    }

    // clip
    nvglScaledImageClip          (NV_DD_SPARE, ((DWORD)dwDstX0), ((DWORD)dwDstY0), dwBlitWidth, dwBlitHeight);
    nvglScaledImageOut           (NV_DD_SPARE, ((DWORD)dwDstX0), ((DWORD)dwDstY0), dwBlitWidth, dwBlitHeight);
    nvglScaledImageDeltaDuDxDvDy (NV_DD_SPARE, (1<<20), (1<<20));

    // Need to make sure the boundary is big enough to work around an anomoly in the scaled image from
    // memory object.
    if (dwBlitWidth < NV4_SCALED_IMAGE_CUTOFF) dwBlitWidth = NV4_SCALED_IMAGE_CUTOFF;

    // even align the input width to prevent an RM exception
    nvglScaledImageInSize   (NV_DD_SPARE, (dwSrcX0 + dwBlitWidth + 1) & ~1, dwSrcY0 + dwBlitHeight);
    nvglScaledImageInFormat (NV_DD_SPARE,
                             (DRF_NUM(077,_IMAGE_IN_FORMAT,_PITCH,        dwSrcPitch)
                             |DRF_DEF(077,_IMAGE_IN_FORMAT,_ORIGIN,       _CORNER)
                             |DRF_DEF(077,_IMAGE_IN_FORMAT,_INTERPOLATOR, _ZOH)));
    nvglScaledImageInOffset (NV_DD_SPARE, dwSrcOffset);
    nvglScaledImageInPoint  (NV_DD_SPARE, dwSrcX0 << 4, dwSrcY0 << 4);

    pDriverData->dDrawSpareSubchannelObject   = D3D_SCALED_IMAGE_FROM_MEMORY;
#if (NVARCH >= 0x010)
    getDC()->dwOutOfContextCelsiusDirtyFlags |= CELSIUS_DIRTY_SURFACE; // this logic kills celcius surface state somehow.
#endif  // NVARCH >= 0x010

    dbgTracePop();
}

/*
  BUGBUG
  new swizzle entry points sit on top of old logic.
  will rewrite / integrate later.
*/

/*****************************************************************************
 * nvSwizBlt_cpu_lin2lin
 *
 * CPU swizzle blt from linear to linear
 */

void nvSwizBlt_cpu_lin2lin
(
    DWORD dwSrcAddr,
    DWORD dwSrcPitch,
    DWORD dwSrcX,
    DWORD dwSrcY,
    DWORD dwDstAddr,
    DWORD dwDstPitch,
    DWORD dwDstX,
    DWORD dwDstY,
    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwBPP
)
{
    dbgTracePush ("nvSwizBlt_cpu_lin2lin");

    DWORD        dwSubHeight = 2048 / (dwWidth * dwBPP);
    if (!dwSubHeight) dwSubHeight = 1;
                 else dwSubHeight = min(dwSubHeight,dwHeight);

    nvSwizzleBlt_Lin_Lin (dwSrcAddr + dwSrcY * dwSrcPitch + dwSrcX * dwBPP,dwSrcPitch,
                          dwDstAddr + dwDstY * dwDstPitch + dwDstX * dwBPP,dwDstPitch,
                          dwWidth,dwHeight,dwSubHeight,dwBPP);

    dbgTracePop();
}


// Converts bits IHGFEDCBA to 00I00H00G00F00E00D00C00B00A
DWORD swizTable3D_512[] = {
    0x0000000, 0x0000001, 0x0000008, 0x0000009, 0x0000040, 0x0000041, 0x0000048, 0x0000049, 0x0000200, 0x0000201, 0x0000208, 0x0000209, 0x0000240, 0x0000241, 0x0000248, 0x0000249, 0x0001000, 0x0001001, 0x0001008, 0x0001009, 0x0001040, 0x0001041, 0x0001048, 0x0001049, 0x0001200, 0x0001201, 0x0001208, 0x0001209, 0x0001240, 0x0001241, 0x0001248, 0x0001249,
    0x0008000, 0x0008001, 0x0008008, 0x0008009, 0x0008040, 0x0008041, 0x0008048, 0x0008049, 0x0008200, 0x0008201, 0x0008208, 0x0008209, 0x0008240, 0x0008241, 0x0008248, 0x0008249, 0x0009000, 0x0009001, 0x0009008, 0x0009009, 0x0009040, 0x0009041, 0x0009048, 0x0009049, 0x0009200, 0x0009201, 0x0009208, 0x0009209, 0x0009240, 0x0009241, 0x0009248, 0x0009249,
    0x0040000, 0x0040001, 0x0040008, 0x0040009, 0x0040040, 0x0040041, 0x0040048, 0x0040049, 0x0040200, 0x0040201, 0x0040208, 0x0040209, 0x0040240, 0x0040241, 0x0040248, 0x0040249, 0x0041000, 0x0041001, 0x0041008, 0x0041009, 0x0041040, 0x0041041, 0x0041048, 0x0041049, 0x0041200, 0x0041201, 0x0041208, 0x0041209, 0x0041240, 0x0041241, 0x0041248, 0x0041249,
    0x0048000, 0x0048001, 0x0048008, 0x0048009, 0x0048040, 0x0048041, 0x0048048, 0x0048049, 0x0048200, 0x0048201, 0x0048208, 0x0048209, 0x0048240, 0x0048241, 0x0048248, 0x0048249, 0x0049000, 0x0049001, 0x0049008, 0x0049009, 0x0049040, 0x0049041, 0x0049048, 0x0049049, 0x0049200, 0x0049201, 0x0049208, 0x0049209, 0x0049240, 0x0049241, 0x0049248, 0x0049249,
    0x0200000, 0x0200001, 0x0200008, 0x0200009, 0x0200040, 0x0200041, 0x0200048, 0x0200049, 0x0200200, 0x0200201, 0x0200208, 0x0200209, 0x0200240, 0x0200241, 0x0200248, 0x0200249, 0x0201000, 0x0201001, 0x0201008, 0x0201009, 0x0201040, 0x0201041, 0x0201048, 0x0201049, 0x0201200, 0x0201201, 0x0201208, 0x0201209, 0x0201240, 0x0201241, 0x0201248, 0x0201249,
    0x0208000, 0x0208001, 0x0208008, 0x0208009, 0x0208040, 0x0208041, 0x0208048, 0x0208049, 0x0208200, 0x0208201, 0x0208208, 0x0208209, 0x0208240, 0x0208241, 0x0208248, 0x0208249, 0x0209000, 0x0209001, 0x0209008, 0x0209009, 0x0209040, 0x0209041, 0x0209048, 0x0209049, 0x0209200, 0x0209201, 0x0209208, 0x0209209, 0x0209240, 0x0209241, 0x0209248, 0x0209249,
    0x0240000, 0x0240001, 0x0240008, 0x0240009, 0x0240040, 0x0240041, 0x0240048, 0x0240049, 0x0240200, 0x0240201, 0x0240208, 0x0240209, 0x0240240, 0x0240241, 0x0240248, 0x0240249, 0x0241000, 0x0241001, 0x0241008, 0x0241009, 0x0241040, 0x0241041, 0x0241048, 0x0241049, 0x0241200, 0x0241201, 0x0241208, 0x0241209, 0x0241240, 0x0241241, 0x0241248, 0x0241249,
    0x0248000, 0x0248001, 0x0248008, 0x0248009, 0x0248040, 0x0248041, 0x0248048, 0x0248049, 0x0248200, 0x0248201, 0x0248208, 0x0248209, 0x0248240, 0x0248241, 0x0248248, 0x0248249, 0x0249000, 0x0249001, 0x0249008, 0x0249009, 0x0249040, 0x0249041, 0x0249048, 0x0249049, 0x0249200, 0x0249201, 0x0249208, 0x0249209, 0x0249240, 0x0249241, 0x0249248, 0x0249249,
    0x1000000, 0x1000001, 0x1000008, 0x1000009, 0x1000040, 0x1000041, 0x1000048, 0x1000049, 0x1000200, 0x1000201, 0x1000208, 0x1000209, 0x1000240, 0x1000241, 0x1000248, 0x1000249, 0x1001000, 0x1001001, 0x1001008, 0x1001009, 0x1001040, 0x1001041, 0x1001048, 0x1001049, 0x1001200, 0x1001201, 0x1001208, 0x1001209, 0x1001240, 0x1001241, 0x1001248, 0x1001249,
    0x1008000, 0x1008001, 0x1008008, 0x1008009, 0x1008040, 0x1008041, 0x1008048, 0x1008049, 0x1008200, 0x1008201, 0x1008208, 0x1008209, 0x1008240, 0x1008241, 0x1008248, 0x1008249, 0x1009000, 0x1009001, 0x1009008, 0x1009009, 0x1009040, 0x1009041, 0x1009048, 0x1009049, 0x1009200, 0x1009201, 0x1009208, 0x1009209, 0x1009240, 0x1009241, 0x1009248, 0x1009249,
    0x1040000, 0x1040001, 0x1040008, 0x1040009, 0x1040040, 0x1040041, 0x1040048, 0x1040049, 0x1040200, 0x1040201, 0x1040208, 0x1040209, 0x1040240, 0x1040241, 0x1040248, 0x1040249, 0x1041000, 0x1041001, 0x1041008, 0x1041009, 0x1041040, 0x1041041, 0x1041048, 0x1041049, 0x1041200, 0x1041201, 0x1041208, 0x1041209, 0x1041240, 0x1041241, 0x1041248, 0x1041249,
    0x1048000, 0x1048001, 0x1048008, 0x1048009, 0x1048040, 0x1048041, 0x1048048, 0x1048049, 0x1048200, 0x1048201, 0x1048208, 0x1048209, 0x1048240, 0x1048241, 0x1048248, 0x1048249, 0x1049000, 0x1049001, 0x1049008, 0x1049009, 0x1049040, 0x1049041, 0x1049048, 0x1049049, 0x1049200, 0x1049201, 0x1049208, 0x1049209, 0x1049240, 0x1049241, 0x1049248, 0x1049249,
    0x1200000, 0x1200001, 0x1200008, 0x1200009, 0x1200040, 0x1200041, 0x1200048, 0x1200049, 0x1200200, 0x1200201, 0x1200208, 0x1200209, 0x1200240, 0x1200241, 0x1200248, 0x1200249, 0x1201000, 0x1201001, 0x1201008, 0x1201009, 0x1201040, 0x1201041, 0x1201048, 0x1201049, 0x1201200, 0x1201201, 0x1201208, 0x1201209, 0x1201240, 0x1201241, 0x1201248, 0x1201249,
    0x1208000, 0x1208001, 0x1208008, 0x1208009, 0x1208040, 0x1208041, 0x1208048, 0x1208049, 0x1208200, 0x1208201, 0x1208208, 0x1208209, 0x1208240, 0x1208241, 0x1208248, 0x1208249, 0x1209000, 0x1209001, 0x1209008, 0x1209009, 0x1209040, 0x1209041, 0x1209048, 0x1209049, 0x1209200, 0x1209201, 0x1209208, 0x1209209, 0x1209240, 0x1209241, 0x1209248, 0x1209249,
    0x1240000, 0x1240001, 0x1240008, 0x1240009, 0x1240040, 0x1240041, 0x1240048, 0x1240049, 0x1240200, 0x1240201, 0x1240208, 0x1240209, 0x1240240, 0x1240241, 0x1240248, 0x1240249, 0x1241000, 0x1241001, 0x1241008, 0x1241009, 0x1241040, 0x1241041, 0x1241048, 0x1241049, 0x1241200, 0x1241201, 0x1241208, 0x1241209, 0x1241240, 0x1241241, 0x1241248, 0x1241249,
    0x1248000, 0x1248001, 0x1248008, 0x1248009, 0x1248040, 0x1248041, 0x1248048, 0x1248049, 0x1248200, 0x1248201, 0x1248208, 0x1248209, 0x1248240, 0x1248241, 0x1248248, 0x1248249, 0x1249000, 0x1249001, 0x1249008, 0x1249009, 0x1249040, 0x1249041, 0x1249048, 0x1249049, 0x1249200, 0x1249201, 0x1249208, 0x1249209, 0x1249240, 0x1249241, 0x1249248, 0x1249249
};

DWORD swiz3DMasks[] = { 0x0000000, 0x0000001, 0x0000009, 0x0000049, 0x0000249, 0x0001249, 0x0009249, 0x0049249, 0x0249249, 0x1249249 };



/*****************************************************************************
 * nvSwizBlt_cpu_lin2swz
 *
 * CPU swizzle blt from linear to swizzle
 */
void nvSwizBlt_cpu_lin2swz
(
    DWORD dwSrcAddr,
    DWORD dwSrcPitch,
    DWORD dwSrcX,
    DWORD dwSrcY,
    DWORD dwDstAddr,
    DWORD dwDstX,
    DWORD dwDstY,
    DWORD dwDstLogWidth,
    DWORD dwDstLogHeight,
    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwBPP
)
{
    dbgTracePush ("nvSwizBlt_cpu_lin2swz");

    /* DO NOT REMOVE
    DWORD dwSize      = dwWidth * dwBPP;
    DWORD dwDstPitch  = dwBPP << dwDstLogWidth;
    dwSrcAddr        += dwSrcY * dwSrcPitch + dwSrcX * dwBPP;
    dwDstAddr        += (dwDstY << dwDstLogWidth) + dwDstX * dwBPP;
    for (DWORD y = dwHeight; y; y--)
    {
        nvMemCopy (dwDstAddr,dwSrcAddr,dwSize);
        dwDstAddr += dwDstPitch;
        dwSrcAddr += dwSrcPitch;
    }

    return;
//*/

    BOOL bFullTexture = !dwSrcX && !dwSrcY
                     && !dwDstX && !dwDstY
                     && (dwWidth  == (1U << dwDstLogWidth))
                     && (dwHeight == (1U << dwDstLogHeight));

    if (bFullTexture)
    {
        nvSwizzleBlt_Lin_Swz_FullTexture (dwSrcAddr,dwSrcPitch,
                                          dwDstAddr,dwDstLogWidth,dwDstLogHeight,
                                          dwBPP);
/* DO NOT REMOVE
        DWORD dwSrc = dwSrcAddr;
        DWORD dwDst = VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
        for (DWORD y = 0; y < dwHeight; y++)
        {
            nvMemCopy ((void*)dwDst,(void*)dwSrc,dwWidth * dwBPP);
            dwDst += ((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deDeltaScan;
            dwSrc += dwSrcPitch;
        }
__asm int 3;
        dwSrc = dwDstAddr;
        dwDst = VIDMEM_ADDR(pDriverData->CurrentVisibleSurfaceAddress);
        for (y = 0; y < dwHeight; y++)
        {
            nvMemCopy ((void*)dwDst,(void*)dwSrc,dwBPP << dwDstLogWidth);
            dwDst += ((DIBENGINEHDR1 *)pDXShare->pFlatDibeng)->deDeltaScan;
            dwSrc += dwBPP << dwDstLogWidth;
        }
__asm int 3;
//*/
    }
    else
    {
        SWIZZLE_ADDR Dest;
        DWORD        dwSubHeight = 2048 / (dwWidth * dwBPP);
        if (!dwSubHeight) dwSubHeight = 1;
                     else dwSubHeight = min(dwSubHeight,dwHeight);

        nvSwizzleAddrCreate (&Dest,dwDstAddr,dwDstX,dwDstY,dwDstLogWidth,dwDstLogHeight,dwBPP);

        nvSwizzleBlt_Lin_Swz (dwSrcAddr + dwSrcY * dwSrcPitch + dwSrcX * dwBPP,dwSrcPitch,
                              &Dest,
                              dwWidth,dwHeight,dwSubHeight,dwBPP);
    }

    dbgTracePop();
}

//
// The slowest CPU volume swizzler ever...
//
void nvVolumeSwizBlt_cpu_lin2swz(
    DWORD dwSrcAddr,
    DWORD dwSrcPitch,
    DWORD dwSrcSlicePitch,
    DWORD dwSrcX,
    DWORD dwSrcY,
    DWORD dwSrcZ,
    DWORD dwDstAddr,
    DWORD dwDstX,
    DWORD dwDstY,
    DWORD dwDstZ,
    DWORD dwDstLogWidth,
    DWORD dwDstLogHeight,
    DWORD dwDstLogDepth,
    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwDepth,
    DWORD dwBPP
) {
    dbgTracePush ("nvVolumeSwizBlt_cpu_lin2swz");

    nvAssert(dwWidth <= 512);
    nvAssert(dwHeight <= 512);
    nvAssert(dwDepth <= 512);

    DWORD dwDstIndex, dwTempDstIndex, dwSrcIndex;
    DWORD i, j, k;

    DWORD dwMask = swiz3DMasks[dwDstLogWidth] | (swiz3DMasks[dwDstLogHeight] << 1) | (swiz3DMasks[dwDstLogDepth] << 2);
    DWORD dwTotalBits = dwDstLogWidth + dwDstLogHeight + dwDstLogDepth;
    DWORD dwTempMask, dwMaskIndex;

#define VOLUME_SWIZZLE_BLIT(type)                                                                               \
    for (k=0; k<dwDepth; k++) {                                                                                 \
        for (j=0; j<dwHeight; j++) {                                                                            \
            for (i=0; i<dwWidth; i++) {                                                                         \
                dwTempDstIndex  =  swizTable3D_512[i+dwDstX]       |                                            \
                                  (swizTable3D_512[j+dwDstY] << 1) |                                            \
                                  (swizTable3D_512[k+dwDstZ] << 2);                                             \
                dwTempMask = dwMask;                                                                            \
                dwDstIndex = 0;                                                                                 \
                dwMaskIndex = 0;                                                                                \
                while (dwTempMask) {                                                                            \
                    while (!(dwTempMask & 1)) {                                                                 \
                        dwTempDstIndex >>= 1;                                                                   \
                        dwTempMask >>= 1;                                                                       \
                    }                                                                                           \
                    dwDstIndex |= dwTempDstIndex & (1 << dwMaskIndex);                                          \
                    dwTempMask >>= 1;                                                                           \
                    dwMaskIndex++;                                                                              \
                }                                                                                               \
                dwSrcIndex = (dwSrcX+i)*dwBPP + dwSrcPitch*(j+dwSrcY) + dwSrcSlicePitch*(k+dwSrcZ);             \
                ((type *)dwDstAddr)[dwDstIndex] = *(type *)&((char *)dwSrcAddr)[dwSrcIndex];                    \
            }                                                                                                   \
        }                                                                                                       \
    }

    switch (dwBPP) {
        case 1:
            VOLUME_SWIZZLE_BLIT(BYTE);
            break;
        case 2:
            VOLUME_SWIZZLE_BLIT(WORD);
            break;
        case 4:
            VOLUME_SWIZZLE_BLIT(DWORD);
            break;
        default:
            // Bad bit depth
            nvAssert(0);
    }

    dbgTracePop();
}

/*****************************************************************************
 * nvSwizBlt_lin2swz
 *
 * CPU and GPU combined swizzle blt from linear to swizzle
 *
 * KKT 3/5/01:  Called only from nvTextureBltDX7.
 */

DWORD lutFormat[5] = { ~0, NV_SURFACE_FORMAT_Y8, NV_SURFACE_FORMAT_R5G6B5, ~0, NV_SURFACE_FORMAT_A8R8G8B8 };

void nvSwizBlt_lin2swz
(
    PNVD3DCONTEXT pContext,
    DWORD         dwSrcAddr,
    DWORD         dwSrcPitch,
    DWORD         dwSrcSlicePitch,
    DWORD         dwSrcX,
    DWORD         dwSrcY,
    DWORD         dwSrcZ,
    CTexture     *pDstTexture,
    DWORD         dwDstAddr,
    DWORD         dwDstX,
    DWORD         dwDstY,
    DWORD         dwDstZ,
    DWORD         dwDstLogWidth,
    DWORD         dwDstLogHeight,
    DWORD         dwDstLogDepth,
    DWORD         dwWidth,
    DWORD         dwHeight,
    DWORD         dwDepth,
    DWORD         dwBPP
)
{
    dbgTracePush ("nvSwizBlt_lin2swz");

    BOOL bFullTexture = !dwSrcX && !dwSrcY
                     && !dwDstX && !dwDstY
                     && (dwWidth  == (1U << dwDstLogWidth))
                     && (dwHeight == (1U << dwDstLogHeight));

    // get size of texture
    DWORD dwSize = dwSrcPitch * dwHeight;

    if (dwDstLogDepth) {
        // Blitting a volume texture
        nvVolumeSwizBlt_cpu_lin2swz(dwSrcAddr, dwSrcPitch, dwSrcSlicePitch,
                                    dwSrcX, dwSrcY, dwSrcZ,
                                    dwDstAddr, dwDstX, dwDstY, dwDstZ,
                                    dwDstLogWidth, dwDstLogHeight, dwDstLogDepth, dwWidth,
                                    dwHeight, dwDepth, dwBPP);
        dbgTracePop();
        return;
    }

    if ((pDriverData->nvD3DPerfData.dwPerformanceStrategy & PS_TEXTURE_USEHW)       // hw allowed blts enabled
     && ((CURRENT_FLIP - COMPLETED_FLIP) < 1)                                       // hw sufficiently idle
     && !(pDstTexture->getSwizzled()->getOffset() & 63)                             // hw can address dest
     && (pDstTexture->getSwizzled()->getHeapLocation() == CSimpleSurface::HEAP_VID) // hw can address dest
     && (dwWidth >= NV4_SCALED_IMAGE_CUTOFF) && (dwHeight >= 4)                     // anything smaller than this isn't worth bothering the hardware with
     && (dwWidth <= 2046) && (dwHeight <= 2047)                                     // dimensions small enough
     && (dwSize < getDC()->defaultVB.getSize() / 4)) {                              // enough staging space

        if (dwBPP == 1) goto cpuCopy;

        // get space
        DWORD dwVBOffset = getDC()->defaultVB.waitForSpace (dwSize + 64 * 12, TRUE);
        if (dwVBOffset == ~0) goto cpuCopy;

        // copy unswizzled texels to scratch surface
        dwVBOffset = (dwVBOffset + 64) & ~63;

                #ifdef NV_PROFILE_COPIES
                NVP_START(NVP_T_LIN2SWZ2);
                #endif
        nvMemCopy (getDC()->defaultVB.getAddress() + dwVBOffset, dwSrcAddr + dwSrcPitch * dwSrcY, dwSize);
                #ifdef NV_PROFILE_COPIES
                NVP_STOP(NVP_T_LIN2SWZ2);
                nvpLogTime (NVP_T_LIN2SWZ2,nvpTime[NVP_T_LIN2SWZ2]);
                #endif
        // capture BLT source
#ifdef CAPTURE
        if (getDC()->nvD3DRegistryData.regCaptureEnable & D3D_REG_CAPTUREENABLE_RECORD) {
            CAPTURE_MEMORY_WRITE memwr;
            memwr.dwJmpCommand         = CAPTURE_JMP_COMMAND;
            memwr.dwExtensionSignature = CAPTURE_EXT_SIGNATURE;
            memwr.dwExtensionID        = CAPTURE_XID_MEMORY_WRITE;
            memwr.dwCtxDMAHandle       = (getDC()->defaultVB.getHeap() == CSimpleSurface::HEAP_VID)
                                       ? NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY
                                       : D3D_CONTEXT_DMA_HOST_MEMORY;
            memwr.dwOffset             = getDC()->defaultVB.getOffset() + dwVBOffset;
            memwr.dwSize               = dwSize;
            captureLog (&memwr,sizeof(memwr));
            captureLog ((void*)getDC()->defaultVB.getAddress(),memwr.dwSize);
        }
#endif

        // lock the texture so that noone else can read from it until we're done writing to it
        pDstTexture->hwLock (pContext, CSimpleSurface::LOCK_NORMAL);

        // program HW to do swizzle
        nvSwizBlt_gpu_lin2swz (getDC()->defaultVB.getOffset() + dwVBOffset,
                               getDC()->defaultVB.getHeap(), dwSrcPitch /*dwBPP << dwDstLogWidth*/, dwSrcX, 0,
                               pDstTexture->getSwizzled()->getOffset(),dwDstX,dwDstY,dwDstLogWidth,dwDstLogHeight,
                               dwWidth,dwHeight,dwBPP);

        // unlock texture
        pDstTexture->hwUnlock();

        getDC()->defaultVB.updateOffset (dwVBOffset + dwSize);
        getDC()->defaultVB.updateMap();

        dbgTracePop();
        return;
    }

cpuCopy:

    if (bFullTexture)
    {
        nvSwizzleBlt_Lin_Swz_FullTexture (dwSrcAddr,dwSrcPitch,
                                          dwDstAddr,dwDstLogWidth,dwDstLogHeight,
                                          dwBPP);
    }
    else
    {
        SWIZZLE_ADDR Dest;
        DWORD        dwSubHeight = 2048 / (dwWidth * dwBPP);
        if (!dwSubHeight) dwSubHeight = 1;
                     else dwSubHeight = min(dwSubHeight,dwHeight);

        nvSwizzleAddrCreate (&Dest,dwDstAddr,dwDstX,dwDstY,dwDstLogWidth,dwDstLogHeight,dwBPP);

        nvSwizzleBlt_Lin_Swz (dwSrcAddr + dwSrcY * dwSrcPitch + dwSrcX * dwBPP,dwSrcPitch,
                              &Dest,
                              dwWidth,dwHeight,dwSubHeight,dwBPP);
    }

    dbgTracePop();
}

/*****************************************************************************
 * nvSwizBlt_cpu_lin2swz
 *
 * CPU swizzle blt from swizzle to linear
 */
void nvSwizBlt_cpu_swz2lin
(
    DWORD dwSrcAddr,
    DWORD dwSrcX,
    DWORD dwSrcY,
    DWORD dwSrcLogWidth,
    DWORD dwSrcLogHeight,
    DWORD dwDstAddr,
    DWORD dwDstPitch,
    DWORD dwDstX,
    DWORD dwDstY,
    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwBPP
)
{
    dbgTracePush ("nvSwizBlt_cpu_swz2lin");

    SWIZZLE_ADDR Source;
    DWORD        dwSubHeight = 2048 / (dwWidth * dwBPP);
    if (!dwSubHeight) dwSubHeight = 1;
                 else dwSubHeight = min(dwSubHeight,dwHeight);

    nvSwizzleAddrCreate (&Source,dwSrcAddr,dwSrcX,dwSrcY,dwSrcLogWidth,dwSrcLogHeight,dwBPP);

    nvSwizzleBlt_Swz_Lin (&Source,
                          dwDstAddr + dwDstY * dwDstPitch + dwDstX * dwBPP,dwDstPitch,
                          dwWidth,dwHeight,dwSubHeight,dwBPP);

    dbgTracePop();
}

/*****************************************************************************
 * nvSwizBlt_cpu_swz2swz
 *
 * CPU swizzle blt from swizzle to swizzle
 */
void nvSwizBlt_cpu_swz2swz
(
    DWORD dwSrcAddr,
    DWORD dwSrcX,
    DWORD dwSrcY,
    DWORD dwSrcLogWidth,
    DWORD dwSrcLogHeight,
    DWORD dwDstAddr,
    DWORD dwDstX,
    DWORD dwDstY,
    DWORD dwDstLogWidth,
    DWORD dwDstLogHeight,
    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwBPP
)
{
    dbgTracePush ("nvSwizBlt_cpu_swz2swz");

    SWIZZLE_ADDR Source;
    SWIZZLE_ADDR Dest;
    DWORD        dwSubHeight = 2048 / (dwWidth * dwBPP);
    if (!dwSubHeight) dwSubHeight = 1;
                 else dwSubHeight = min(dwSubHeight,dwHeight);

    nvSwizzleAddrCreate (&Source,dwSrcAddr,dwSrcX,dwSrcY,dwSrcLogWidth,dwSrcLogHeight,dwBPP);
    nvSwizzleAddrCreate (&Dest,  dwDstAddr,dwDstX,dwDstY,dwDstLogWidth,dwDstLogHeight,dwBPP);

    // todo - check for FT blts, use linear copy

    nvSwizzleBlt_Swz_Swz (&Source,
                          &Dest,
                          dwWidth,dwHeight,dwSubHeight,dwBPP);

    dbgTracePop();
}


/*****************************************************************************
 * nvSwizBlt_gpu_lin2swz
 *
 * GPU swizzle blt from linear to swizzle
 */
void nvSwizBlt_gpu_lin2swz
(
    DWORD dwSrcOffset,
    DWORD dwSrcHeap,
    DWORD dwSrcPitch,
    DWORD dwSrcX,
    DWORD dwSrcY,
    DWORD dwDstOffset,
    DWORD dwDstX,
    DWORD dwDstY,
    DWORD dwDstLogWidth,
    DWORD dwDstLogHeight,
    DWORD dwWidth,
    DWORD dwHeight,
    DWORD dwBPP
)
{
    dbgTracePush ("nvSwizBlt_gpu_lin2swz");
    DWORD dwImageColorFormat;
    DWORD dwFormat = lutFormat[dwBPP];
    nvAssert(dwFormat != ~0);

#if (NVARCH >= 0x010)
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0089_SCALEDIMG) {
        dwImageColorFormat = nv089SurfaceFormat[dwFormat];
    }
    else
#endif  // NVARCH >= 0x010
    if (pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_0077_SCALEDIMG) {
        dwImageColorFormat = nv077SurfaceFormat[dwFormat];
    }

    DWORD dwSurfaceColorFormat = nv052SurfaceFormat[dwFormat];

    assert (dwImageColorFormat   != ~0);
    assert (dwSurfaceColorFormat != ~0);

    // NOTE: HW may not swizzle 8bpp correctly or at all!
    nvHWSwizzleBlt (dwSrcOffset,dwSrcPitch,dwImageColorFormat,dwSrcX,dwSrcY,dwSrcHeap,
                    dwDstOffset,dwDstLogWidth,dwDstLogHeight,dwSurfaceColorFormat,dwDstX,dwDstY,
                    dwWidth,dwHeight);

    dbgFlushType (NVDBG_FLUSH_2D);

    dbgTracePop();
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

extern "C++"
{

#define CPB_templ template<const DWORD cdwAMask,const DWORD cdwABitPos,const DWORD cdwABitCount,\
                           const DWORD cdwRMask,const DWORD cdwRBitPos,const DWORD cdwRBitCount,\
                           const DWORD cdwGMask,const DWORD cdwGBitPos,const DWORD cdwGBitCount,\
                           const DWORD cdwBMask,const DWORD cdwBBitPos,const DWORD cdwBBitCount,\
                           const DWORD cdwBPP,const DWORD cdwPalType,\
                           const DWORD cdwGetOrMask>

#define CPB_args cdwAMask,cdwABitPos,cdwABitCount,\
                 cdwRMask,cdwRBitPos,cdwRBitCount,\
                 cdwGMask,cdwGBitPos,cdwGBitCount,\
                 cdwBMask,cdwBBitPos,cdwBBitCount,\
                 cdwBPP,cdwPalType,\
                 cdwGetOrMask

CPB_templ class CPBAbstract
{
protected:
    struct PE
    {
        DWORD color;
        DWORD next;
        DWORD prev;
        DWORD index;
        DWORD count;
        DWORD numChildren;
        DWORD child[16];

        inline void init (void) { memset(this, 0xFF, sizeof(*this)); count = numChildren = 0; }
    };
    PE    *m_aSet;
    DWORD  m_adwRoot[9];
    DWORD  m_dwTop;
    DWORD  m_dwNextAlloc;
    DWORD  m_dwSize;
    DWORD  m_dwColors;

protected:
    inline DWORD mask (DWORD dwPel) const { return dwPel | cdwGetOrMask; }
    inline DWORD get  (BYTE *p)     const { return mask((cdwBPP == 4) ? (*(DWORD*)p) : ((DWORD)*(WORD*)p)); }
    inline DWORD getA (DWORD dwPel) const { return (dwPel & cdwAMask) >> cdwABitPos; }
    inline DWORD getR (DWORD dwPel) const { return (dwPel & cdwRMask) >> cdwRBitPos; }
    inline DWORD getG (DWORD dwPel) const { return (dwPel & cdwGMask) >> cdwGBitPos; }
    inline DWORD getB (DWORD dwPel) const { return (dwPel & cdwBMask) >> cdwBBitPos; }

    inline void put (BYTE *p,DWORD dw)
    {
        if (cdwBPP == 4) *(DWORD*)p = dw;
                    else *(WORD*)p = (WORD)dw;
    }

protected:
    BOOL grow (void);

    inline DWORD allocNode (void)
    {
        // march on
        m_dwNextAlloc++;
        if (m_dwNextAlloc >= m_dwSize)
        {
            // we are out of space - grow
            if (!grow()) return ~0;
        }

        // return free node
        m_aSet[m_dwNextAlloc].init();
        return m_dwNextAlloc;
    }
    inline DWORD lookup (DWORD dwPel)
    {
        DWORD a = getA(dwPel);
        DWORD r = getR(dwPel);
        DWORD g = getG(dwPel);
        DWORD b = getB(dwPel);

        DWORD curr = m_dwTop;
        for (DWORD i = 0; i < 9; i++)
        {
            // found it yet?
            if (!m_aSet[curr].numChildren)
            {
                return m_aSet[curr].index;
            }

            // get child
            DWORD child = ((a & 128) >> 4) | ((r & 128) >> 5) | ((g & 128) >> 6) | ((b & 128) >> 7);

            // next
            curr = m_aSet[curr].child[child];
            a <<= 1;
            r <<= 1;
            g <<= 1;
            b <<= 1;
        }

        return m_aSet[curr].index;
    }

protected:
    BOOL addToTable (DWORD dwPel);
    void swizzle    (DWORD dwSrcAddr,DWORD dwSrcPitch,SWIZZLE_ADDR *lpSA,DWORD dwWidth,DWORD dwHeight,DWORD dwSubHeight);

public:
    BOOL initTable  (void);
    BOOL buildTable (DWORD dwSource,DWORD dwWidth,DWORD dwHeight);
    BOOL palettize  (BYTE *pbPalette);
    BOOL apply      (DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwLogWidth,DWORD dwHeight,DWORD dwLogHeight);
    BOOL freeTable  (void);

public:
    BOOL depalettizeInPlace      (DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwHeight,DWORD *pPalette);
    BOOL depalettizeAndDeswizzle (DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwLogWidth,DWORD dwHeight,DWORD dwLogHeight,DWORD dwDeviceMemPaletteAddr);
};

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::grow
(
    void
)
{
    m_dwSize *= 2;
    if (!ReallocIPM(m_aSet,m_dwSize * sizeof(PE),(void**)&m_aSet)) return FALSE; // out of memory
    if (!m_aSet) return FALSE; // out of memory
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::addToTable
(
    DWORD dwPel
)
{
    DWORD a = getA(dwPel);
    DWORD r = getR(dwPel);
    DWORD g = getG(dwPel);
    DWORD b = getB(dwPel);

    DWORD curr   = m_dwTop;
    BOOL  unique = FALSE;
    for (DWORD i = 0; i < 8; i++)
    {
        // get child
        DWORD child = ((a & 128) >> 4) | ((r & 128) >> 5) | ((g & 128) >> 6) | ((b & 128) >> 7);

        // bump usage count
        m_aSet[curr].count ++;

        // traverse or add?
        if (m_aSet[curr].child[child] == ~0)
        {
            unique = TRUE;

            // add new node
            DWORD next = allocNode();
            if (next == ~0) return FALSE; // abort
            m_aSet[curr].child[child] = next;
            m_aSet[curr].numChildren  ++;

            // add to linked list
            m_aSet[next].prev = ~0;
            m_aSet[next].next = m_adwRoot[i+1];
            if (m_adwRoot[i+1] != ~0) m_aSet[m_adwRoot[i+1]].prev = next;
            m_adwRoot[i+1]    = next;

            // next pos
            curr = next;
        }
        else
        {
            // next pos
            curr = m_aSet[curr].child[child];
        }

        // next
        a <<= 1;
        r <<= 1;
        g <<= 1;
        b <<= 1;
    }

    // set color of leaf
    m_aSet[curr].count ++;
    m_aSet[curr].color = dwPel;
    if (unique) m_dwColors++; // unique color count

    // done
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
void CPBAbstract<CPB_args>::swizzle
(
    DWORD         dwSrcAddr,
    DWORD         dwSrcPitch,

    SWIZZLE_ADDR *lpSA,

    DWORD         dwWidth,
    DWORD         dwHeight,
    DWORD         dwSubHeight
)
{
    DWORD dwBytes = dwWidth * cdwBPP;
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
            nvMemCopy (a,dwSrcAddr,dwBytes);
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
                DWORD dwPel = 0;
                if (cdwBPP == 4) dwPel = *(DWORD*)a;
                            else dwPel = *(WORD*)a;

                DWORD dwIndex = lookup(mask(dwPel));
                *(BYTE*)(nvSwizzleAddrGet(lpSA)) = (BYTE)dwIndex;

                a += cdwBPP;
                nvSwizzleAddrNext (lpSA);
            }
            nvSwizzleAddrNewLine (lpSA);
        }

        y -= f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::initTable
(
    void
)
{
    // init table
    memset(m_adwRoot, 0xFF, sizeof(m_adwRoot));
    m_dwSize      = 256;
    m_aSet        = (PE*)AllocIPM(m_dwSize * sizeof(PE));
    if (!m_aSet) return FALSE;
    m_aSet[0].init();
    m_adwRoot[0]  = 0;
    m_dwTop       = 0;
    m_dwNextAlloc = 0;

    // init stats
    m_dwColors = 0;

    // done
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::buildTable
(
    DWORD  dwSource,
    DWORD  dwWidth,
    DWORD  dwHeight
)
{
    // build table
    DWORD dwLine  = dwSource;
    DWORD dwPitch = dwWidth * cdwBPP;
    for (DWORD y = 0; y < dwHeight; y++,dwLine += dwPitch)
    {
        for (DWORD xx = 0,x = 0; x < dwWidth; x++, xx += cdwBPP)
        {
            DWORD pel = get((BYTE*)(dwLine+xx));
            if (!addToTable(pel)) return FALSE;
            if (m_dwColors >= 256) return FALSE;
        }
    }

    // done
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::palettize
(
    BYTE *pbPalette
)
{
    // do color reduction
    int lowest = 7;
    while (m_dwColors > 256)
    {
        // find the node with the lowest use count that is not a leaf
        DWORD max = ~0;
        DWORD pos = ~0;
        for (DWORD curr = m_adwRoot[lowest]; curr != ~0; curr = m_aSet[curr].next)
        {
            PE *p = m_aSet + curr;

            if (p->numChildren && (p->count < max))
            {
                max = p->count;
                pos = curr;

                if ((p->numChildren == 1) || (p->count <= 2)) break; // shortcut when useful stuff is found
            }
        }
        if (pos == ~0)
        {
            lowest--;
            continue;
        }

        // combine the child nodes
        DWORD  cmax = 0;
        PE     *p    = m_aSet + pos;
        for (int j = 0; j < 16; j++)
        {
            if (p->child[j] != ~0)
            {
                PE *c = m_aSet + p->child[j];

                assert (!c->numChildren);

                if (c->count > cmax)
                {
                    cmax     = c->count;
                    p->color = c->color;
                }

                DWORD a = c->prev;
                DWORD b = c->next;
                if (a != ~0) m_aSet[a].next = b; else m_adwRoot[lowest+1] = b;
                if (b != ~0) m_aSet[b].prev = a;

                m_dwColors--;
                p->child[j] = ~0;
            }
        }
        m_dwColors++;
        p->numChildren = 0;
    }

    // enumerate and build pallette
    DWORD index = 0;
    for (DWORD i = 0; i < 9; i++)
    {
        for (DWORD curr = m_adwRoot[i]; curr != ~0; curr = m_aSet[curr].next)
        {
            PE *p = m_aSet + curr;
            if (!p->numChildren)
            {
                assert (index < 256); // color reduction failed

                if (cdwBPP == 4) *(DWORD*)(pbPalette + index * 4) = p->color;
                            else *(WORD*)(pbPalette + index * 2) = (WORD)p->color;

                p->index          = index;
                index++;
            }
        }
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::apply
(
    DWORD  dwSource,
    DWORD  dwDest,
    DWORD  dwWidth,
    DWORD  dwLogWidth,
    DWORD  dwHeight,
    DWORD  dwLogHeight
)
{
    // convert image and swizzle
    SWIZZLE_ADDR dest;
    DWORD        dwSubHeight = 2048 / (dwWidth * cdwBPP);
    DWORD        dwPitch     = dwWidth * cdwBPP;
    if (!dwSubHeight) dwSubHeight = 1;
                 else dwSubHeight = min(dwSubHeight,dwHeight);
    nvSwizzleAddrCreate (&dest,dwDest,0,0,dwLogWidth,dwLogHeight,1);
    swizzle(dwSource,dwPitch,&dest,dwWidth,dwHeight,dwSubHeight);

    // done
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::freeTable
(
    void
)
{
    if (m_aSet) FreeIPM (m_aSet);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::depalettizeInPlace
(
    DWORD dwSource,
    DWORD dwDest,
    DWORD dwWidth,
    DWORD dwHeight,
    DWORD *pPalette
)
{
    // depalettize (bottom up since we do this in place)

    nvAssert(cdwBPP == 4);

    DWORD *dwPixels = (DWORD *)dwDest - 1;         //shift the array to optimize the loop
    BYTE * pIndices = (BYTE *)dwSource - 1;

    for (DWORD a = dwHeight*dwWidth; a; a--)
        dwPixels[a]=pPalette[pIndices[a]];

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

CPB_templ
BOOL CPBAbstract<CPB_args>::depalettizeAndDeswizzle
(
    DWORD dwSource,
    DWORD dwDest,
    DWORD dwWidth,
    DWORD dwLogWidth,
    DWORD dwHeight,
    DWORD dwLogHeight,
    DWORD dwDeviceMemPaletteAddr
)
{
    // copy palette away
    DWORD adwPalette[256];
    memcpy (adwPalette,(char*)dwDeviceMemPaletteAddr,256 * cdwBPP);

    // deswizzle and depalettize
    SWIZZLE_ADDR source;
    DWORD        dwSubHeight = 2048 / (dwWidth * cdwBPP);
    DWORD        dwPitch     = dwWidth * cdwBPP;
    if (!dwSubHeight) dwSubHeight = 1;
                 else dwSubHeight = min(dwSubHeight,dwHeight);
    nvSwizzleAddrCreate (&source,dwSource,0,0,dwLogWidth,dwLogHeight,1);

    DWORD        dwBytes = dwWidth * cdwBPP;
    DWORD        y;
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
                DWORD dwPel;
                if (cdwBPP == 4) dwPel = adwPalette[*(BYTE*)nvSwizzleAddrGet(&source)];
                            else dwPel = *(WORD*)(DWORD(adwPalette) + 2 * (DWORD)*(BYTE*)nvSwizzleAddrGet(&source));
                put ((BYTE*)a,dwPel);

                a += cdwBPP;
                nvSwizzleAddrNext (&source);
            }

            nvSwizzleAddrNewLine (&source);
        }

        /*
         * write linear
         */
        a = (DWORD)&nvTexelScratchBuffer;
        for (h = f; h; h--)
        {
            nvMemCopy (dwDest,a,dwBytes);
            a          += dwBytes;
            dwDest     += dwPitch;
        }

        y -= f;
    }

    // done
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

// 32
CPBAbstract<0xff000000,24,8,
          0x00ff0000,16,8,
          0x0000ff00, 8,8,
          0x000000ff, 0,8,
          4,PT_8888,
          0x00000000>        g_PB8888;
CPBAbstract<0x00000000, 0,0,
          0x00ff0000,16,8,
          0x0000ff00, 8,8,
          0x000000ff, 0,8,
          4,PT_X888,
          0xff000000>        g_PBX888;
// 16
CPBAbstract<0x0000,12,4,
          0x0000, 8,4,
          0x0000, 4,4,
          0x0000, 0,4,
          2,PT_4444,
          0x00000000>        g_PB4444;
CPBAbstract<0x8000,15,1,
          0x7c00,10,5,
          0x03e0, 5,5,
          0x001f, 0,5,
          2,PT_1555,
          0x0000>            g_PB1555;
CPBAbstract<0x0000, 0,0,
          0x7c00,10,5,
          0x03e0, 5,5,
          0x001f, 0,5,
          2,PT_X555,
          0x8000>            g_PBX555;
CPBAbstract<0x0000, 0,0,
          0xfe00,11,5,
          0x07e0, 5,6,
          0x001f, 0,5,
          2,PT_565,
          0x0000>            g_PB565;

BOOL CPaletteBuilder::initTable  (DWORD dwPalType)
{
    switch (dwPalType)
    {
        case PT_8888: return g_PB8888.initTable();
        case PT_X888: return g_PBX888.initTable();
        case PT_4444: return g_PB4444.initTable();
        case PT_1555: return g_PB1555.initTable();
        case PT_X555: return g_PBX555.initTable();
        case PT_565 : return g_PB565 .initTable();
    }
    return FALSE;
}
BOOL CPaletteBuilder::buildTable (DWORD dwPalType,DWORD dwSource,DWORD dwWidth,DWORD dwHeight)
{
    switch (dwPalType)
    {
        case PT_8888: return g_PB8888.buildTable(dwSource,dwWidth,dwHeight);
        case PT_X888: return g_PBX888.buildTable(dwSource,dwWidth,dwHeight);
        case PT_4444: return g_PB4444.buildTable(dwSource,dwWidth,dwHeight);
        case PT_1555: return g_PB1555.buildTable(dwSource,dwWidth,dwHeight);
        case PT_X555: return g_PBX555.buildTable(dwSource,dwWidth,dwHeight);
        case PT_565 : return g_PB565 .buildTable(dwSource,dwWidth,dwHeight);
    }
    return FALSE;
}
BOOL CPaletteBuilder::palettize (DWORD dwPalType,BYTE *pbPalette)
{
    switch (dwPalType)
    {
        case PT_8888: return g_PB8888.palettize(pbPalette);
        case PT_X888: return g_PBX888.palettize(pbPalette);
        case PT_4444: return g_PB4444.palettize(pbPalette);
        case PT_1555: return g_PB1555.palettize(pbPalette);
        case PT_X555: return g_PBX555.palettize(pbPalette);
        case PT_565 : return g_PB565 .palettize(pbPalette);
    }
    return FALSE;
}
BOOL CPaletteBuilder::apply (DWORD dwPalType,DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwLogWidth,DWORD dwHeight,DWORD dwLogHeight)
{
    switch (dwPalType)
    {
        case PT_8888: return g_PB8888.apply(dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight);
        case PT_X888: return g_PBX888.apply(dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight);
        case PT_4444: return g_PB4444.apply(dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight);
        case PT_1555: return g_PB1555.apply(dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight);
        case PT_X555: return g_PBX555.apply(dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight);
        case PT_565 : return g_PB565 .apply(dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight);
    }
    return FALSE;
}
BOOL CPaletteBuilder::freeTable (DWORD dwPalType)
{
    switch (dwPalType)
    {
        case PT_8888: return g_PB8888.freeTable();
        case PT_X888: return g_PBX888.freeTable();
        case PT_4444: return g_PB4444.freeTable();
        case PT_1555: return g_PB1555.freeTable();
        case PT_X555: return g_PBX555.freeTable();
        case PT_565 : return g_PB565 .freeTable();
    }
    return FALSE;
}
BOOL CPaletteBuilder::depalettizeInPlace (DWORD dwPalType,DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwHeight,DWORD *pPalette)
{
    switch (dwPalType)
    {
        case PT_8888: return g_PB8888.depalettizeInPlace(dwSource,dwDest,dwWidth,dwHeight,pPalette);
        case PT_X888: return g_PBX888.depalettizeInPlace(dwSource,dwDest,dwWidth,dwHeight,pPalette);
        case PT_4444: return g_PB4444.depalettizeInPlace(dwSource,dwDest,dwWidth,dwHeight,pPalette);
        case PT_1555: return g_PB1555.depalettizeInPlace(dwSource,dwDest,dwWidth,dwHeight,pPalette);
        case PT_X555: return g_PBX555.depalettizeInPlace(dwSource,dwDest,dwWidth,dwHeight,pPalette);
        case PT_565 : return g_PB565 .depalettizeInPlace(dwSource,dwDest,dwWidth,dwHeight,pPalette);
    }
    return FALSE;
}
BOOL CPaletteBuilder::depalettizeAndDeswizzle (DWORD dwPalType,DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwLogWidth,DWORD dwHeight,DWORD dwLogHeight,DWORD dwDeviceMemPaletteAddr)
{
    switch (dwPalType)
    {
        case PT_8888: return g_PB8888.depalettizeAndDeswizzle (dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight,dwDeviceMemPaletteAddr);
        case PT_X888: return g_PBX888.depalettizeAndDeswizzle (dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight,dwDeviceMemPaletteAddr);
        case PT_4444: return g_PB4444.depalettizeAndDeswizzle (dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight,dwDeviceMemPaletteAddr);
        case PT_1555: return g_PB1555.depalettizeAndDeswizzle (dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight,dwDeviceMemPaletteAddr);
        case PT_X555: return g_PBX555.depalettizeAndDeswizzle (dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight,dwDeviceMemPaletteAddr);
        case PT_565 : return g_PB565 .depalettizeAndDeswizzle (dwSource,dwDest,dwWidth,dwLogWidth,dwHeight,dwLogHeight,dwDeviceMemPaletteAddr);
    }
    return FALSE;
}

}

#endif  // NVARCH >= 0x04

