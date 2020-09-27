/*************************************************************************

Copyright (c) 2001  Microsoft Corporation

Module Name:
        intradec_x86.cpp

Abstract:
        X8 decoding functions specific to X86 platform (many with MMX)

Author:
        Sridhar Srinivasan (sridhsri@microsoft.com)
        24 April 2001

*************************************************************************/
#ifdef _WMV_TARGET_X86_

#include "bldsetup.h"

#include "xplatform.h"
#include "limits.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "typedef.hpp"
#include "wmvdec_member.h"
#include "strmdec_wmv.hpp"
#include "idctdec_wmv2.h"
#include "localhuffman_wmv.h"
#include "spatialpredictor_wmv.hpp"

extern U16_WMV aNullPredWts_WMV[];

#pragma warning(disable: 4799)  // emms warning disable

Void_WMV predictDec_MMX(t_SpatialPredictor *pSp, const I32_WMV iOrient,
                        PixelC *pRef, const I32_WMV iRefStride, PixelI *pDelta,
                        Bool_WMV bClear)
{

//#ifdef  _WMV_TARGET_X86_  // put the ifdef here first...  move to outside of the function when
                 // everything is fixed
// all instances of m_pSide have been replaced by the constant 8 for speed
    const PixelC  *pVal;
    PixelI  *pDeltaOrig = pDelta;

    if (pSp->m_bFlat) {
            I32_WMV iDc = pSp->m_iDcValue;
            _asm {
                mov     ecx, 8    ; counter
                movd    mm1, iDc
                punpcklwd   mm1, mm1
                punpcklwd   mm1, mm1

                mov     edx, [pRef]
                mov     ebx, [pDelta]

StartOfLoopFlat:
                movq    mm2, [ebx]
                paddw   mm2, mm1

                movq    mm3, [ebx + 8]
                paddw   mm3, mm1

                packuswb    mm2, mm3
                movq    [edx], mm2

                ; increment pointers
                add     edx, iRefStride
                add     ebx, 16

                dec     ecx
                jnz     StartOfLoopFlat
            }
            if (bClear)
                goto Clear;
            else
                return;
    }

    switch(iOrient) {
    case 0:
        SetupZeroPredictor_MMX (pSp);
        {

            U16_WMV  *pTSum;
            U16_WMV  *pWts = aNullPredWts_WMV;
                pTSum = pSp->m_pTopSum;
                U16_WMV *pLSum = pSp->m_pLeftSum;
                _asm {
                    mov     eax, 0x8000
                    movd    mm7, eax
                    punpckldq   mm7, mm7    ; mm7 = 0000 8000 0000 8000

                    xor     ecx, ecx        ; counter i
                    pxor    mm0, mm0

                    mov     edi, [pTSum]
                    mov     esi, [pWts]
                    mov     edx, [pRef]
                    mov     ebx, [pDelta]

StartOfLoop:
                    mov     eax, [pLSum]
                    mov     ax, [eax + ecx * 2]
                    movd    mm2, eax
                    punpcklwd   mm2, mm2    ; mm2 = xxxx xxxx pLSum[i] pLSum[i]

                    ; first 4 elements
                    ; read in 4 deltas
                    movq    mm5, [ebx]      ; mm4 = pDelta[3] pDelta[2] pDelta[1] pDelta[0]

                    ; first 2 elements
                    movq    mm1, [esi]      ; mm1 = pWts[3] pWts[2] pWts[1] pWts[0]
                    movd    mm3, [edi]      ; mm3 = xxxx xxxx pTSum[1] pTSum[0]
                    punpcklwd   mm3, mm2    ; mm3 = pLSum[i] pTSum[1] pLSum[i] pTSum[0]
                    pmaddwd     mm3, mm1
                    paddd   mm3, mm7
                    psrad   mm3, 16

                    ; second 2 elements
                    movq    mm1, [esi + 8]
                    movd    mm4, [edi + 4]
                    punpcklwd   mm4, mm2
                    pmaddwd     mm4, mm1
                    paddd   mm4, mm7
                    psrad   mm4, 16

                    ; pack into mmx reg
                    packssdw    mm3, mm4
                    paddw   mm3, mm5

                    ; pack into output
                    packuswb    mm3, mm3
                    movd    [edx], mm3


                    ; second 4 elements
                    ; read in 4 deltas
                    movq    mm5, [ebx + 8]

                    ; first 2 elements
                    movq    mm1, [esi + 16]
                    movd    mm3, [edi + 8]
                    punpcklwd   mm3, mm2
                    pmaddwd     mm3, mm1
                    paddd   mm3, mm7
                    psrad   mm3, 16

                    ; second 2 elements
                    movq    mm1, [esi + 24]
                    movd    mm4, [edi + 12]
                    punpcklwd   mm4, mm2
                    pmaddwd     mm4, mm1
                    paddd   mm4, mm7
                    psrad   mm4, 16

                    ; pack into mmx reg
                    packssdw    mm3, mm4
                    paddw   mm3, mm5

                    ; pack into output
                    packuswb    mm3, mm3
                    movd    [edx + 4], mm3


                    ; increment pointers
                    add     esi, 32     ; pWts
                    add     ebx, 16     ; pDelta
                    add     edx, iRefStride     ; pRef

                    inc     ecx
                    cmp     ecx, 8
                    jnz     StartOfLoop
                }
            
        }
        break;
    case 4:
        {
                pVal = pSp->m_pTop;
                _asm {
                    ; set avg of top rows
                    mov     esi, [pVal]
                    pxor    mm0, mm0
                    pcmpeqw mm5, mm5    ; mm5 is all one
                    movq    mm7, [esi]
                    movq    mm1, [esi + 16]
                    movq    mm6, mm7
                    movq    mm2, mm1

                    ; first four words
                    punpcklbw   mm7, mm0
                    punpcklbw   mm1, mm0
                    punpckhbw   mm6, mm0
                    paddw   mm7, mm1
                    punpckhbw   mm2, mm0
                    psubw   mm7, mm5    ; mm7++

                    ; second four words
                    paddw   mm6, mm2
                    psraw   mm7, 1
                    psubw   mm6, mm5    ; mm6++
                    mov     ecx, 4      ; counter
                    psraw   mm6, 1

                    mov     eax, iRefStride
                    mov     edx, [pRef]
                    mov     ebx, [pDelta]

StartOfLoop4:
                    ; loop iter 1
                    movq    mm2, [ebx]
                    paddw   mm2, mm7
                    movq    mm3, [ebx + 8]
                    paddw   mm3, mm6
                    packuswb    mm2, mm3
                    movq    [edx], mm2

                    add     edx, eax

                    ; loop iter 2
                    movq    mm2, [ebx + 16]
                    paddw   mm2, mm7
                    movq    mm3, [ebx + 24]
                    paddw   mm3, mm6
                    packuswb    mm2, mm3
                    movq    [edx], mm2

                    ; increment pointers
                    add     ebx, 32
                    add     edx, eax

                    dec     ecx
                    jnz     StartOfLoop4
                }
        }
        break;
    default:
        predictDec (pSp, iOrient, pRef, iRefStride, pDelta, 0);
    }
    if (!bClear)
        return;

Clear:
    _asm {
        mov     edi, [pDeltaOrig]
        pxor    mm0, mm0
        ; rows 1 to 4
        movq    [edi], mm0
        movq    [edi + 8], mm0
        movq    [edi + 16], mm0
        movq    [edi + 24], mm0
        movq    [edi + 32], mm0
        movq    [edi + 40], mm0
        movq    [edi + 48], mm0
        movq    [edi + 56], mm0
        ; rows 5 to 8
        movq    [edi + 64], mm0
        movq    [edi + 72], mm0
        movq    [edi + 80], mm0
        movq    [edi + 88], mm0
        movq    [edi + 96], mm0
        movq    [edi + 104], mm0
        movq    [edi + 112], mm0
        movq    [edi + 120], mm0
    }
    return;
}

Void_WMV predict_0_MMX (U16_WMV *pTSum, U16_WMV *pLSum, U16_WMV *pWts, PixelC *pRef, I32_WMV iRefStride)
{
    _asm {
        mov     eax, 0x8000
        movd    mm7, eax
        punpckldq   mm7, mm7    ; mm7 = 0000 8000 0000 8000

        xor     ecx, ecx        ; counter i
        pxor    mm0, mm0

        mov     edi, [pTSum]
        mov     esi, [pWts]
        mov     edx, [pRef]

StartOfLoop:
        mov     eax, [pLSum]
        mov     ax, [eax + ecx * 2]
        movd    mm2, eax
        punpcklwd   mm2, mm2    ; mm2 = xxxx xxxx pLSum[i] pLSum[i]

        ; first 4 elements
        ; read in 4 deltas

        ; first 2 elements
        movq    mm1, [esi]      ; mm1 = pWts[3] pWts[2] pWts[1] pWts[0]
        movd    mm3, [edi]      ; mm3 = xxxx xxxx pTSum[1] pTSum[0]
        punpcklwd   mm3, mm2    ; mm3 = pLSum[i] pTSum[1] pLSum[i] pTSum[0]
        pmaddwd     mm3, mm1
        paddd   mm3, mm7
        psrad   mm3, 16

        ; second 2 elements
        movq    mm1, [esi + 8]
        movd    mm4, [edi + 4]
        punpcklwd   mm4, mm2
        pmaddwd     mm4, mm1
        paddd   mm4, mm7
        psrad   mm4, 16

        ; pack I32_WMVo mmx reg
        packssdw    mm3, mm4

        ; pack I32_WMVo output
        packuswb    mm3, mm3
        movd    [edx], mm3


        ; second 4 elements
        ; read in 4 deltas

        ; first 2 elements
        movq    mm1, [esi + 16]
        movd    mm3, [edi + 8]
        punpcklwd   mm3, mm2
        pmaddwd     mm3, mm1
        paddd   mm3, mm7
        psrad   mm3, 16

        ; second 2 elements
        movq    mm1, [esi + 24]
        movd    mm4, [edi + 12]
        punpcklwd   mm4, mm2
        pmaddwd     mm4, mm1
        paddd   mm4, mm7
        psrad   mm4, 16

        ; pack into mmx reg
        packssdw    mm3, mm4

        ; pack intoo output
        packuswb    mm3, mm3
        movd    [edx + 4], mm3


        ; increment pointers
        add     esi, 32     ; pWts
        add     edx, iRefStride     ; pRef

        inc     ecx
        cmp     ecx, 8
        jnz     StartOfLoop
    }
}

/****************************************************************************************
  setupZeroPredictor_MMX : sets up boundary arrays for zero prediction mode (MMX version)
****************************************************************************************/
Void_WMV SetupZeroPredictor_MMX (t_SpatialPredictor *pSp)
{
    U8_WMV  *mpTop = pSp->m_pTop;
    U8_WMV  *mpLeft = pSp->m_pLeft;
    I16_WMV      *pTop  = pSp->m_pBuffer16 + 12;
    I16_WMV      *pLeft = pSp->m_pBuffer16;
    U16_WMV     *pTsum = pSp->m_pTopSum;
    U16_WMV     *pLsum = pSp->m_pLeftSum;

    I64  S1[5], S3[5], *pS1, *pS3;
    pS1 = S1;
    *pS1 = 0;
    pS3 = S3;

#ifdef  _WMV_TARGET_X86_
    _asm {
        mov     esi, [pTop]
        mov     edi, [pLeft]

        mov     edx, [mpTop]
        pxor    mm0, mm0
        movq    mm1, [edx]
        movq    mm2, mm1
        punpcklbw   mm1, mm0
        movq    [esi], mm1
        punpckhbw   mm2, mm0
        movq    [esi + 8], mm2
        movd    mm1, [edx + 8]
        punpcklbw   mm1, mm0
        movq    [esi + 16], mm1

        mov     edx, [mpLeft]
        mov     eax, [edx - 4]
        mov     ebx, [edx - 8]
        bswap   eax
        bswap   ebx
        movd    mm1, eax
        movd    mm2, ebx
        punpcklbw   mm1, mm0
        punpcklbw   mm2, mm0
        movq    [edi], mm1
        movq    [edi + 8], mm2

        mov     edx, [pS1]
        mov     ecx, [pS3]

        ; k = 0, 1
        movd    mm2, [esi]
        movd    mm1, [edi]
        punpckldq   mm1, mm2    ; mm1 = pTop[1] pTop[0] pLeft[1] pLeft[0]
        psllw   mm1, 4      ; << D
        movq    mm7, mm1
        movq    [edx + 8], mm1

        ; k = 2, 3
        psraw   mm1, 1
        movd    mm2, [esi + 4]
        movd    mm6, [edi + 4]
        punpckldq   mm6, mm2
        psllw   mm6, 4
        paddw   mm1, mm6
        movq    [edx + 16], mm1

        ; k = 4, 5
        psraw   mm1, 1
        movd    mm2, [esi + 8]
        movd    mm5, [edi + 8]
        punpckldq   mm5, mm2
        psllw   mm5, 4
        paddw   mm1, mm5
        movq    [edx + 24], mm1

        ; k = 6, 7
        psraw   mm1, 1
        movd    mm2, [esi + 12]
        movd    mm4, [edi + 12]
        punpckldq   mm4, mm2
        psllw   mm4, 4
        paddw   mm1, mm4
        movq    mm2, mm4
        movq    [edx + 32], mm1

        psraw   mm2, 1
        paddw   mm5, mm2
        movq    [ecx + 16], mm5

        psraw   mm5, 1
        paddw   mm6, mm5
        movq    [ecx + 8], mm6

        psraw   mm6, 1
        paddw   mm7, mm6
        movq    [ecx], mm7

        ; 8, 9
        movd    mm2, [esi + 16]
        pxor    mm1, mm1
        punpckldq   mm1, mm2
        psllw   mm1, 3
        paddw   mm4, mm1
        movq    [ecx + 24], mm4
        psllw   mm1, 1

        ; 10, 11
        movd    mm2, [esi + 20]
        pxor    mm4, mm4
        punpckldq   mm4, mm2
        psllw   mm4, 3
        paddw   mm1, mm4
        movq    [ecx + 32], mm1

        ; add s??[1] terms (straight adds), * 181, + 128 and shift down
        mov     eax, 0x00800080 ; 128 128
        movd    mm3, eax
        punpcklwd   mm3, mm0    ; mm3 = 0 128 0 128
        xor     ebx, ebx
        mov     esi, [pLsum]
        mov     edi, [pTsum]

StartOfLoop:
        movq    mm7, [ecx + ebx * 8]
        movq    mm6, [edx + ebx * 8]
        movq    mm5, mm7
        movq    mm4, mm6
        paddw   mm7, mm6
        movq    mm6, mm7
        punpcklwd   mm7, mm0
        punpckhwd   mm6, mm0
        movq    mm1, mm7
        movq    mm2, mm6
        ; multiplying by 181
        pslld   mm1, 2
        pslld   mm2, 2
        paddd   mm7, mm1
        paddd   mm6, mm2

        pslld   mm1, 2
        pslld   mm2, 2
        paddd   mm7, mm1
        paddd   mm6, mm2
        paddd   mm7, mm1
        paddd   mm6, mm2
        paddd   mm7, mm1
        paddd   mm6, mm2

        pslld   mm1, 3
        pslld   mm2, 3
        paddd   mm7, mm1
        paddd   mm6, mm2
        ; adding 128
        paddd   mm7, mm3
        paddd   mm6, mm3
        ; shifting
        psrad   mm7, 8
        psrad   mm6, 8
        packssdw    mm7, mm6

        ; add first st of terms
        movq    mm1, mm3
        movq    mm6, mm5
        pcmpgtw mm1, mm0    ; 0 -1 0 -1

        pand    mm6, mm1
        movq    mm2, mm1
        pandn   mm2, mm4
        por     mm6, mm2

        movq    mm2, mm1
        pand    mm4, mm1
        pandn   mm1, mm5
        por     mm1, mm4
        psraw   mm1, 1
        paddw   mm6, mm1

        ; exchange mm6 words and add to mm7
        movq    mm5, mm6
        psrlq   mm5, 16
        pand    mm5, mm2
        psllq   mm6, 16
        pandn   mm2, mm6
        por     mm2, mm5
        paddw   mm7, mm2

        movd    [esi + ebx * 4 - 2], mm7
        psrlq   mm7, 32
        movd    [edi + ebx * 4 - 2], mm7

        inc     ebx
        cmp     ebx, 5
        jl      StartOfLoop;
    }
#endif // _WMV_TARGET_X86_
}

/****************************************************************************************
  GetRange : Finds range and sum
****************************************************************************************/
Void_WMV GetRange (U8_WMV *pTop, U8_WMV *pLeft, I32_WMV &iRange, I32_WMV &iDC)
{
    I32_WMV  iDC1, iRange1;
    _asm {
        pxor    mm0, mm0
        mov     esi, [pTop]
        mov     edi, [pLeft]
        mov     eax, 0x80808080

        ; subtract 128 ... 128
        movd    mm7, eax
        punpcklbw   mm7, mm7
        movq    mm1, [esi]
        movq    mm2, [edi]
        psubb   mm1, mm7
        psubb   mm2, mm7
        movq    mm3, mm1
        movq    mm4, mm2
        movq    mm5, mm3

        pcmpgtb mm1, mm2
        movq    mm7, mm1
        pand    mm5, mm1
        pand    mm4, mm1
        pandn   mm1, mm2
        pandn   mm7, mm3
        por     mm5, mm1    ; max
        por     mm4, mm7    ; min

        ; take negative of min
        pxor    mm6, mm6
        pcmpeqb mm6, mm6    ; mm6 = -1 ... -1
        pxor    mm1, mm1
        psubb   mm1, mm4
        paddb   mm1, mm6

        ; repack min-max
        movq    mm4, mm1
        punpckldq   mm4, mm5
        punpckhdq   mm1, mm5

        ; first: four compares each
        movq    mm7, mm1
        pcmpgtb mm7, mm4
        pand    mm1, mm7
        pandn   mm7, mm4
        por     mm1, mm7    ; mm1 has data

        ; second: two compares each
        movq    mm7, mm1
        psrld   mm1, 16
        movq    mm4, mm1
        pcmpgtb mm4, mm7
        pand    mm1, mm4
        pandn   mm4, mm7
        por     mm1, mm4    ; mm1 has data

        ; third: one compare each
        movq    mm7, mm1
        psrld   mm1, 8
        movq    mm4, mm1
        pcmpgtb mm4, mm7
        pand    mm1, mm4
        pandn   mm4, mm7
        por     mm1, mm4    ; mm1 has data

        pslld   mm1, 24
        psrad   mm1, 24
        movq    mm4, mm1
        psrlq   mm4, 32
        paddd   mm1, mm4

        movd    ebx, mm1
        inc     ebx
        mov     iRange1, ebx

        ; find sum
        movd    mm7, eax
        punpcklbw   mm7, mm7
        paddb   mm2, mm7
        paddb   mm3, mm7

        movq    mm1, mm3
        punpcklbw   mm1, mm0
        punpckhbw   mm3, mm0
        paddw   mm3, mm1
        movq    mm1, mm2
        punpcklbw   mm1, mm0
        punpckhbw   mm2, mm0
        paddw   mm2, mm1
        paddw   mm2, mm3
        movq    mm3, mm2
        psrlq   mm3, 32
        paddw   mm2, mm3
        movq    mm3, mm2
        psrlq   mm3, 16
        paddw   mm2, mm3    ; sum
        movd    eax, mm2
        shl     eax, 16
        shr     eax, 16
        mov     iDC1, eax
    }
    iDC += iDC1;
    iRange = iRange1;
}

#pragma warning(default: 4799)  // reset emms warning disable
#endif // _WMV_TARGET_X86_
