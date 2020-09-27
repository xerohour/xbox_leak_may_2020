/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvMem.cpp                                                         *
*   NVIDIA Fast memory copy loops - copyright strictly enforced             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 11/05/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

#include "x86.h"

/*
 * local macros
 */
#define movaps_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x28 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movaps_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x29 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_r_rm8(r,rm,ofs)      __asm _emit 0x0f __asm _emit 0x10 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define movups_rm8_r(rm,ofs,r)      __asm _emit 0x0f __asm _emit 0x11 __asm _emit (mREG(r) | (rm)) __asm _emit (ofs)
#define prefetch_rm8(h,rm,ofs)      __asm _emit 0x0f __asm _emit 0x18 __asm _emit (mREG(h) | (rm)) __asm _emit (ofs)

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
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
    {
        if (dwSrc & 15)
        {
            if (dwDest & 15)
            {
                __asm
                {
                    push ebx
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_11: movups_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movups_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_11
                    pop ebx
                }
            }
            else
            {
                __asm
                {
                    push ebx
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_12: movups_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movaps_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_12
                    pop ebx
                }
            }
        }
        else
        {
            if (dwDest & 15)
            {
                __asm
                {
                    push ebx
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_13: movaps_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movups_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_13
                    pop ebx
                }
            }
            else
            {
                __asm
                {
                    push ebx
                    mov ebx,[dwSrc]
                    mov edx,[dwDest]
                    mov ecx,[dwCount]
            mc16_14: movaps_r_rm8 (rXMM0,rmIND8(rEBX),0)
                     add ebx,16
                     movaps_rm8_r (rmIND8(rEDX),rXMM0,0)
                     add edx,16
                     dec ecx
                    jnz mc16_14
                    pop ebx
                }
            }
        }
    }
    else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_MMX)
    {
        __asm
        {
            push ebx
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
            pop ebx
        }
    }
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
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_KATMAI)
    {
        if (dwSrc & 15)
        {
            if (dwDest & 15)
            {
                __asm
                {
                    push ebx
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
                    pop ebx
                }
            }
            else
            {
                __asm
                {
                    push ebx
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
                    pop ebx
                }
            }
        }
        else
        {
            if (dwDest & 15)
            {
                __asm
                {
                    push ebx
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
                    pop ebx
                }
            }
            else
            {
                __asm
                {
                    push ebx
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
                    pop ebx
                }
            }
        }
    }
    else if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_MMX)
    {
        __asm
        {
            push ebx
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
            pop ebx
        }
    }
    else
    {
        nvMemCopy4 (dwDest,dwSrc,dwCount*16);
    }
}

typedef void (__cdecl *PFNCUSTOMCOPY)(DWORD dwDest,DWORD dwSrc);
typedef void (__cdecl *PFNCUSTOMCOPY2)(DWORD dwDest,DWORD dwSrc,DWORD dwCnt);

/*
 * nvMemCopy
 *
 * copy memory as fast as we can
 */
void nvMemCopy
(
    DWORD dwDest,
    DWORD dwSrc,
    DWORD dwCount
)
{
#ifdef DEBUG
    if (IsBadReadPtr((void*)dwSrc,dwCount)
     || IsBadWritePtr((void*)dwDest,dwCount))
    {
        DPF ("nvMemCopy: bad copy requested. Call ignored.");
        __asm int 3;
        return;
    }
#endif
    if (pDriverData->nvD3DPerfData.dwCPUFeatureSet & FS_ATHLON)
    {                        
        if ( (dwCount >= 2048) && (dwDest & 0x3f) )
        {
            if (!global.adwMemCopy[0])
            {
                global.adwMemCopy[0] = nvBuildCustomCopyRoutineMisAligned();
            }
            PFNCUSTOMCOPY2 pfn = (PFNCUSTOMCOPY2)(global.dwILCData + global.adwMemCopy[0]);
            pfn (dwDest,dwSrc,dwCount);
        }
        else
        {
            
            //
            // copy large blocks
            //
            if (dwCount >= 256)
            {
                if (!global.adwMemCopy[128])
                {
                    global.adwMemCopy[128] = nvBuildCustomCopyRoutine(128);
                }

                PFNCUSTOMCOPY pfn = (PFNCUSTOMCOPY)(global.dwILCData + global.adwMemCopy[128]);
                DWORD dwPrefetchSrc = dwSrc;
                while (dwCount >= 128)
                {
                    if (dwPrefetchSrc <= dwSrc)
                    {
                        DWORD dwPrefetchCount = min(2048,dwCount);
                        __asm
                        {
                            mov ecx,[dwPrefetchCount]
                            mov eax,[dwPrefetchSrc]
                        next0:
                             cmp ecx,4
                             jl  done0
                             mov edx,[eax]
                             sub ecx,32
                             add eax,32
                            jmp next0
                        done0:
                            mov [dwPrefetchSrc],eax
                        }
                    }

                    pfn (dwDest,dwSrc);
                    dwSrc   += 128;
                    dwDest  += 128;
                    dwCount -= 128;
                }
            }

            //
            // copy small blocks
            //
            if (dwCount)
            {
                if (!global.adwMemCopy[dwCount])
                {
                    // build routine
                    global.adwMemCopy[dwCount] = nvBuildCustomCopyRoutine(dwCount);
                }
                ((PFNCUSTOMCOPY)(global.dwILCData + global.adwMemCopy[dwCount]))(dwDest,dwSrc);
            }
        }
    }
    else
   {


        //
        // copy large blocks
        //
        if (dwCount >= 256)
        {
            if (!global.adwMemCopy[128])
            {
                global.adwMemCopy[128] = nvBuildCustomCopyRoutine(128);
            }

            PFNCUSTOMCOPY pfn = (PFNCUSTOMCOPY)(global.dwILCData + global.adwMemCopy[128]);
            DWORD dwPrefetchSrc = dwSrc;
            while (dwCount >= 128)
            {
                if (dwPrefetchSrc <= dwSrc)
                {
                    DWORD dwPrefetchCount = min(2048,dwCount);
                    __asm
                    {
                        mov ecx,[dwPrefetchCount]
                        mov eax,[dwPrefetchSrc]
                    next:
                         cmp ecx,4
                         jl  done
                         mov edx,[eax]
                         sub ecx,32
                         add eax,32
                        jmp next
                    done:
                        mov [dwPrefetchSrc],eax
                    }
                }

                pfn (dwDest,dwSrc);
                dwSrc   += 128;
                dwDest  += 128;
                dwCount -= 128;
            }
        }

        //
        // copy small blocks
        //
        if (dwCount)
        {
            if (!global.adwMemCopy[dwCount])
            {
                // build routine
                global.adwMemCopy[dwCount] = nvBuildCustomCopyRoutine(dwCount);
            }
            ((PFNCUSTOMCOPY)(global.dwILCData + global.adwMemCopy[dwCount]))(dwDest,dwSrc);
        }
    }
}

#endif  // NVARCH >= 0x04

