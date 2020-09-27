/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NV4MEM.C                                                          *
*   NVIDIA Fast memory copy loops - copyright strictly enforced             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 11/05/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include <windows.h>
#include "ddrvmem.h"
#include "nvd3ddrv.h"
#include "nv32.h"
#include "nvddobj.h"
#include "d3dinc.h"
#include "nvd3dmac.h"
#include "nv4dreg.h"
#include "nv3ddbg.h"
#include "nv4vxmac.h"
#include "nvheap.h"
#include "..\x86\x86.h"

/*
 * local macros
 */
#define movaps_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x28 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movaps_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x29 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x10 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x11 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)

/*
 * nvMemCopy1
 *
 * copies src to dest using byte atoms
 */
__inline void nvMemCopy1
(
    DWORD dwDest,
    DWORD dwSrc,
    DWORD dwCount
)
{
    while (dwCount)
    {
        *(BYTE*)dwDest = *(BYTE*)dwSrc;
        dwSrc  ++;
        dwDest ++;
        dwCount--;
    }
}

/*
 * nvMemCopy4
 *
 * copies src to dest using dword atoms.
 */
__inline void nvMemCopy4
(
    DWORD dwDest,
    DWORD dwSrc,
    DWORD dwCount   // in dwords
)
{
    while (dwCount)
    {
        *(DWORD*)dwDest = *(DWORD*)dwSrc;
        dwSrc  += 4;
        dwDest += 4;
        dwCount--;
    }
}

/*
 * nvMemCopy16
 *
 * copies src to dest using 4*dword atoms (if possible).
 */
__inline void nvMemCopy16
(
    DWORD dwDest,
    DWORD dwSrc,
    DWORD dwCount
)
{
    if (global.dwCPUFeatureSet & FS_KATMAI)
    {
        if (dwSrc & 15)
        {
            if (dwDest & 15)
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_11: movups_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movups_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_11
                }
            }
            else
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_12: movups_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movaps_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_12
                }
            }
        }
        else
        {
            if (dwDest & 15)
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_13: movaps_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movups_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_13
                }
            }
            else
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_14: movaps_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movaps_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_14
                }
            }
        }
    }
#ifndef WINNT  // BUGBUG - WHY NO WORKY IN NT5 BUILD?
    else if (global.dwCPUFeatureSet & FS_MMX)
    {
        __asm
        {
            mov ebx,[dwSrc]
            mov edx,[dwDest]
            mov ecx,[dwCount]
mc16_2:      movq mm0,[ebx]
             movq mm1,[ebx+8]
             add ebx,16
             movq [edx],mm0
             movq [edx+8],mm1
             add edx,16
             dec ecx
            jnz mc16_2
            emms
        }
    }
#endif  // !WINNT
    else
    {
        nvMemCopy4 (dwDest,dwSrc,dwCount*4);
    }
}

/*
 * nvMemCopy64
 *
 * copies src to dest using 16*dword atoms (if possible).
 */
__inline void nvMemCopy64
(
    DWORD dwDest,
    DWORD dwSrc,
    DWORD dwCount   // in dwords
)
{
    if (global.dwCPUFeatureSet & FS_KATMAI)
    {
        if (dwSrc & 15)
        {
            if (dwDest & 15)
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
        mc64_11:     movups_r_rm8 (rXMM0,rmIND8(rEBX), 0)
                     movups_r_rm8 (rXMM1,rmIND8(rEBX),16)
                     movups_r_rm8 (rXMM2,rmIND8(rEBX),32)
                     movups_r_rm8 (rXMM3,rmIND8(rEBX),48)
                     add ebx,64
                     movups_rm8_r (rmIND8(rEDX), 0,rXMM0)
                     movups_rm8_r (rmIND8(rEDX),16,rXMM1)
                     movups_rm8_r (rmIND8(rEDX),32,rXMM2)
                     movups_rm8_r (rmIND8(rEDX),48,rXMM3)
                     add edx,64
                     dec ecx
                    jnz mc64_11
                }
            }
            else
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
        mc64_12:     movups_r_rm8 (rXMM0,rmIND8(rEBX), 0)
                     movups_r_rm8 (rXMM1,rmIND8(rEBX),16)
                     movups_r_rm8 (rXMM2,rmIND8(rEBX),32)
                     movups_r_rm8 (rXMM3,rmIND8(rEBX),48)
                     add ebx,64
                     movaps_rm8_r (rmIND8(rEDX), 0,rXMM0)
                     movaps_rm8_r (rmIND8(rEDX),16,rXMM1)
                     movaps_rm8_r (rmIND8(rEDX),32,rXMM2)
                     movaps_rm8_r (rmIND8(rEDX),48,rXMM3)
                     add edx,64
                     dec ecx
                    jnz mc64_12
                }
            }
        }
        else
        {
            if (dwDest & 15)
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
        mc64_13:     movaps_r_rm8 (rXMM0,rmIND8(rEBX), 0)
                     movaps_r_rm8 (rXMM1,rmIND8(rEBX),16)
                     movaps_r_rm8 (rXMM2,rmIND8(rEBX),32)
                     movaps_r_rm8 (rXMM3,rmIND8(rEBX),48)
                     add ebx,64
                     movups_rm8_r (rmIND8(rEDX), 0,rXMM0)
                     movups_rm8_r (rmIND8(rEDX),16,rXMM1)
                     movups_rm8_r (rmIND8(rEDX),32,rXMM2)
                     movups_rm8_r (rmIND8(rEDX),48,rXMM3)
                     add edx,64
                     dec ecx
                    jnz mc64_13
                }
            }
            else
            {
                __asm
                {
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
        mc64_14:     movaps_r_rm8 (rXMM0,rmIND8(rEBX), 0)
                     movaps_r_rm8 (rXMM1,rmIND8(rEBX),16)
                     movaps_r_rm8 (rXMM2,rmIND8(rEBX),32)
                     movaps_r_rm8 (rXMM3,rmIND8(rEBX),48)
                     add ebx,64
                     movaps_rm8_r (rmIND8(rEDX), 0,rXMM0)
                     movaps_rm8_r (rmIND8(rEDX),16,rXMM1)
                     movaps_rm8_r (rmIND8(rEDX),32,rXMM2)
                     movaps_rm8_r (rmIND8(rEDX),48,rXMM3)
                     add edx,64
                     dec ecx
                    jnz mc64_14
                }
            }
        }
    }
#ifndef WINNT  // BUGBUG - WHY NO WORKY IN NT5 BUILD?
    else if (global.dwCPUFeatureSet & FS_MMX)
    {
        __asm
        {
            mov ebx,[dwSrc]
            mov edx,[dwDest]
            mov ecx,[dwCount]
mc64_2:      movq mm0,[ebx+ 0]
             movq mm1,[ebx+ 8]
             movq mm2,[ebx+16]
             movq mm3,[ebx+24]
             movq mm4,[ebx+32]
             movq mm5,[ebx+40]
             movq mm6,[ebx+48]
             movq mm7,[ebx+56]
             add ebx,64
             movq [edx+ 0],mm0
             movq [edx+ 8],mm1
             movq [edx+16],mm2
             movq [edx+24],mm3
             movq [edx+32],mm4
             movq [edx+40],mm5
             movq [edx+48],mm6
             movq [edx+56],mm7
             add edx,64
             dec ecx
            jnz mc64_2
            emms
        }
    }
#endif  // !WINNT
    else
    {
        nvMemCopy4 (dwDest,dwSrc,dwCount*16);
    }
}

/*
 * nvMemCopy
 *
 * copy memory as fast as we can
 */
void nvMemCopy
(
    DWORD dwDest,
    DWORD dwSrc,
    DWORD dwCount,
    DWORD dwFlags
)
{
    /*
     * small block optimizations
     *  less than 32 bytes gets unaligned dword accesses followed by some bytes (normal memcpy)
     *  less than 128 bytes and caller does not care for alignment
     */
    if ( (dwCount < 32)
     || ((dwCount < 128) && (dwFlags == NV_MEMCOPY_DONTCARE)))
    {
        memcpy ((void*)dwDest,(void*)dwSrc,dwCount);
        return;
    }

    /*
     * align source or dest depending on flags
     */
    {
        DWORD dwNum = (dwFlags & NV_MEMCOPY_WANTSRCALIGNED)
                    ? (dwSrc  & 15)
                    : (dwDest & 15);

        if (dwCount >= 4)
        {
            /*
             * align to 4
             */
            if (dwNum & 3)
            {
                DWORD dwTemp = 4 - (dwNum & 3);
                nvMemCopy1 (dwDest,dwSrc,dwTemp);
                dwSrc   += dwTemp;
                dwDest  += dwTemp;
                dwCount -= dwTemp;
            }

            if (dwCount >= 16)
            {
                /*
                 * align to 16
                 */
                if (dwNum & 12)
                {
                    DWORD dwTemp = 16 - (dwNum & 12);
                    nvMemCopy4 (dwDest,dwSrc,dwTemp / 4);
                    dwSrc   += dwTemp;
                    dwDest  += dwTemp;
                    dwCount -= dwTemp;
                }
            }
        }
    }

    /*
     * copy remaining data
     */
    {
        DWORD dw64 = dwCount & 0xffffffc0;
        DWORD dw16 = dwCount & 0x00000030;
        DWORD dw4  = dwCount & 0x0000000c;
        DWORD dw1  = dwCount & 0x00000003;

        if (dw64)
        {
            nvMemCopy64 (dwDest,dwSrc,dw64 / 64);
            dwDest += dw64;
            dwSrc  += dw64;
        }
        if (dw16)
        {
            nvMemCopy16 (dwDest,dwSrc,dw16 / 16);
            dwDest += dw16;
            dwSrc  += dw16;
        }
        if (dw4)
        {
            nvMemCopy4 (dwDest,dwSrc,dw4 / 4);
            dwDest += dw4;
            dwSrc  += dw4;
        }
        if (dw1)
        {
            nvMemCopy1 (dwDest,dwSrc,dw1 / 1);
            dwDest += dw1;
            dwSrc  += dw1;
        }
    }
}

/*
 * nvMemTouch
 *
 * moves given data block to L1 cache if it fits - else is wastes time
 */
void nvMemTouch
(
    DWORD dwSrc,
    DWORD dwCount
)
{
    __asm
    {
        mov ecx,[dwCount]
        shr ecx,5
         jz mts

        mov ebx,[dwSrc]

    mtl: mov eax,[ebx]
         add ebx,32
         dec ecx
        jnz mtl

    mts:
    }
}
