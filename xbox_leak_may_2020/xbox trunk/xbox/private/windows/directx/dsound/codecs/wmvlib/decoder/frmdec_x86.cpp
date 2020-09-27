#include "bldsetup.h"

#include "xplatform.h"

#include "limits.h"
//#include "windows.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "typedef.hpp"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "codehead.h"
#include "tables_wmv.h"
#include "strmdec_wmv.hpp"
#include "idctdec_wmv2.h"
#include "motioncomp_wmv.h"


#ifdef _WMV_TARGET_X86_
#include "opcodes.h"
// output is in mm0
// note: this macro is different from the one in motest.cpp, mm6 is not used.
#define ComputeEightBytesCubicMC(pLine1, pLine2, pLine3, pLine4) _asm { \
_asm        movq        mm0, [pLine2]               \
_asm        movq        mm2, [pLine3]               \
_asm        movq        mm1, mm0                    \
_asm        movq        mm3, mm2                    \
_asm        punpcklbw   mm0, mm7                    \
_asm        punpcklbw   mm2, mm7                    \
_asm        movq        mm4, [pLine1]               \
_asm        movq        mm5, mm4                    \
_asm        paddw       mm0, mm2                    \
_asm        punpckhbw   mm1, mm7                    \
_asm        movq        mm2, const_09_09_09_09      \
_asm        punpckhbw   mm3, mm7                    \
_asm        pmullw      mm0, mm2                    \
_asm        paddw       mm1, mm3                    \
_asm        pmullw      mm1, mm2                    \
_asm        punpcklbw   mm4, mm7                    \
_asm        punpckhbw   mm5, mm7                    \
_asm        movq        mm2, [pLine4]               \
_asm        movq        mm3, mm2                    \
_asm        punpcklbw   mm2, mm7                    \
_asm        punpckhbw   mm3, mm7                    \
_asm        paddw       mm2, mm4                    \
_asm        paddw       mm3, mm5                    \
_asm        movq        mm5, const_08_08_08_08      \
_asm        psubw       mm0, mm2                    \
_asm        psubw       mm1, mm3                    \
_asm        paddw       mm0, mm5                    \
_asm        paddw       mm1, mm5                    \
_asm        psraw       mm0, 4                      \
_asm        psraw       mm1, 4                      \
_asm        packuswb    mm0, mm1                    \
}

// assume prediction is in mm0, output is in mm1;
#define AddErrorAndClip(pError)       _asm {            \
_asm                    movq        mm1, [pError]       \
_asm                    movq        mm2, mm0            \
_asm                    punpcklbw   mm0, mm7            \
_asm                    paddw       mm1, mm0            \
_asm                    movq        mm3, [pError + 8]   \
_asm                    punpckhbw   mm2, mm7            \
_asm                    paddw       mm3, mm2            \
_asm                    packuswb    mm1, mm3            \
}

//avg mm0,mm1
#define ComputeBilinear()            _asm {             \
_asm            movq        mm2, x01                    \
_asm            movq        mm3, x7f                    \
_asm            movq        mm5,mm0                     \
_asm            por         mm5,mm1                     \
_asm            psrlw       mm0,1                       \
_asm            psrlw       mm1,1                       \
_asm            pand        mm5,mm2                     \
_asm            pand        mm0,mm3                     \
_asm            pand        mm1,mm3                     \
_asm            paddw       mm0,mm5                     \
_asm            paddw       mm0,mm1                     \
}

Void_WMV MotionCompMixed_MMX (
                        tWMVDecInternalMember *pWMVDec, 
                        PixelC*              ppxlcPredMB,
                        const PixelC*        ppxlcRefMB,
                        I32_WMV                  iWidthPrev,
                        Bool_WMV                 bInterpolateX,
                        Bool_WMV                 bInterpolateY,
                        I32_WMV                  iMixedPelMV
                        )
{    
    PixelC* ppxlcPred = ppxlcPredMB;
        
    static const __int64 const_09_09_09_09 = 0x0009000900090009;
    static const __int64 const_08_08_08_08 = 0x0008000800080008;
    static const __int64 x01 = 0x0101010101010101;
    static const __int64 x7f = 0x7f7f7f7f7f7f7f7f;

    PixelC tempBlock[11 * 8 + 15];
    PixelC *alignedBlock = (PixelC*) (((int)tempBlock + 15) & ~15);
    if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            I32_WMV iy;
            assert(iMixedPelMV == 0);            
            for (iy = 0; iy < 8; iy++) {
                *(U64 *)(ppxlcPredMB) = *(U64 *)(ppxlcRefMB);
                ppxlcRefMB += iWidthPrev;
                ppxlcPredMB += iWidthPrev;
            }
        }
        else {  //bXSubPxl && !bYSubPxl            
            PixelC* pRef = (PixelC*) (ppxlcRefMB + 1);
            if (iMixedPelMV == 0) {
                _asm {
                    mov         esi, pRef;
                    mov         edi, ppxlcPred;
                    mov         edx, iWidthPrev;
                    mov         ecx, 8;
                    pxor        mm7, mm7;

main1a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main1a;
                    emms;
                }
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___
                _asm {
                    mov         esi, pRef;
                    mov         edi, ppxlcPred;
                    mov         edx, iWidthPrev;
                    mov         ecx, 8;
                    pxor        mm7, mm7;
                    
main1b:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        mm1, [esi];
                    ComputeBilinear();
                    //pavgb       mm0_mm1;
                    movq        [edi], mm0;

                    add         esi, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main1b;
                    emms;
                }
            }
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            PixelC* pRef = (PixelC*) (ppxlcRefMB + iWidthPrev);
            const PixelC* pLine4 = pRef + iWidthPrev;
            const PixelC* pLine3 = pRef;
            const PixelC* pLine2 = pLine3 - iWidthPrev;
            const PixelC* pLine1 = pLine2 - iWidthPrev;                                

            if (iMixedPelMV == 0) {                             
                _asm {
                    mov         esi, pLine1;
                    mov         eax, pLine4;
                    mov         edi, ppxlcPred;
                    mov         edx, iWidthPrev;
                    mov         ecx, 8;
                    pxor        mm7, mm7;                   
main2a:
                    ComputeEightBytesCubicMC(esi, esi + edx, esi + 2*edx, eax);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         eax, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main2a;
                    emms;
                }                
            } else {
                //      H Q H' 
                //      I                     
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthPrev;
                    mov         ecx, 11;
                    pxor        mm7, mm7;

main2b:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main2b;
 
                    mov         esi, alignedBlock;
                    mov         edi, ppxlcPred;
                    mov         ecx, 8;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main2c:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    movq        mm1, mm6;
                    ComputeBilinear();
//                    pavgb       mm0_mm6;
                    movq        [edi], mm0;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main2c;
                    emms;
                }        
            }
        }
        else { // bXSubPxl && bYSubPxl
            PixelC* pRef = (PixelC*) (ppxlcRefMB + iWidthPrev + 1);
            const PixelC* pLine4 = pRef + iWidthPrev;
            const PixelC* pLine3 = pRef;
            const PixelC* pLine2 = pLine3 - iWidthPrev;
            const PixelC* pLine1 = pLine2 - iWidthPrev;                
            if (iMixedPelMV == 0) {

                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthPrev;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main3a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main3a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcPred;
                    mov         ecx, 8;
                   
main3b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        [edi], mm0;
                    add         esi, 8;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main3b;
                    emms;
                }   
            } else {
                // H Q H
                //     I
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthPrev;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main4a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main4a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcPred;
                    mov         ecx, 8;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main4b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    movq        mm1, mm6;
                    ComputeBilinear();
/*                     pavgb       mm0_mm6;*/
                    movq        [edi], mm0;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main4b;
                    emms;
                }        
            }
        }
    }
}

Void_WMV MotionCompMixedAndAddError_MMX (
                                   tWMVDecInternalMember *pWMVDec,
                                   PixelC __huge* ppxlcCurrQMB, 
                                   const Buffer __huge* ppxliErrorBuf, 
                                   const PixelC __huge* ppxlcRef, 
                                   I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven, 
                                   I32_WMV iMixedPelMV
                                   )
{

    PixelC tempBlock[11 * 8 + 15];
    PixelC *alignedBlock = (PixelC*) (((int)tempBlock + 15) & ~15);

    static const __int64 const_09_09_09_09 = 0x0009000900090009;
    static const __int64 const_08_08_08_08 = 0x0008000800080008;
    static const __int64 x01 = 0x0101010101010101;
    static const __int64 x7f = 0x7f7f7f7f7f7f7f7f;

    const I16_WMV __huge *ppxliErrorQMB = ppxliErrorBuf->i16;
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                _asm {
                    mov         esi, ppxliErrorQMB;
                    mov         edx, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         ecx, 8;
                    pxor        mm7, mm7;
                    mov         ebx, iWidthFrm;
main0:
                    movq        mm0, [edx];
                    AddErrorAndClip(esi);
                    movq        [edi], mm1;

                    add         esi, 16;
                    add         edx, ebx;
                    add         edi, ebx;
                    dec         ecx;
                    jnz         main0;
                    emms;
                }
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //         ___                               
                _asm {
                    mov         esi, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         edx, ppxliErrorQMB;
                    mov         ecx, 8;
                    mov         ebx, iWidthFrm;
                    pxor        mm7, mm7;
main1:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        mm1, [esi];
//                    pavgb       mm0_mm1;
                    ComputeBilinear();
                    AddErrorAndClip(edx);
                    movq        [edi], mm1;
                    
                    add         esi, ebx;
                    add         edi, ebx;
                    add         edx, 16;
                    dec         ecx;
                    jnz         main1;
                    emms;
                }
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                _asm {
                    mov         esi, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         edx, ppxliErrorQMB;
                    mov         ecx, 8;
                    mov         ebx, iWidthFrm;
                    pxor        mm7, mm7;
main2:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    AddErrorAndClip(edx);
                    movq        [edi], mm1;
                    add         esi, ebx;
                    add         edi, ebx;
                    add         edx, 16;
                    dec         ecx;
                    jnz         main2;
                    emms;
                }
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___   
                _asm {
                    mov         esi, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         edx, ppxliErrorQMB;
                    mov         ecx, 8;
                    mov         ebx, iWidthFrm;
                    pxor        mm7, mm7;
main3:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        mm1, [esi + 1];
                    ComputeBilinear();
//                    pavgb       mm0_mm1;
                    AddErrorAndClip(edx);
                    movq        [edi], mm1;
                    
                    add         esi, ebx;
                    add         edi, ebx;
                    add         edx, 16;
                    dec         ecx;
                    jnz         main3;
                    emms;
                }                
            }
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
            if (iMixedPelMV == 0) {
                const PixelC* pLine1 = ppxlcRef - iWidthFrm;
                const PixelC* pLine4 = ppxlcRef + 2 * iWidthFrm;   

                _asm {
                    mov         esi, pLine1;
                    mov         eax, pLine4;
                    mov         edi, ppxlcCurrQMB;
                    mov         ebx, ppxliErrorQMB;
                    mov         edx, iWidthFrm;
                    mov         ecx, 8;
                    pxor        mm7, mm7;                   
main4:
                    ComputeEightBytesCubicMC(esi, esi + edx, esi + 2*edx, eax);
                    AddErrorAndClip(ebx);
                    movq        [edi], mm1;
                    add         esi, edx;
                    add         eax, edx;
                    add         edi, edx;
                    add         ebx, 16;
                    dec         ecx;
                    jnz         main4;
                    emms;
                } 
            } else {
                //      H Q H' 
                //      I                              
                PixelC* pRef = (PixelC*) (ppxlcRef + iWidthFrm);                   
                const PixelC* pLine4 = pRef + iWidthFrm; 
                const PixelC* pLine1 = pRef - 2 * iWidthFrm;
                I32_WMV iLoop = 8;
                //      H Q H' 
                //      I                     
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthFrm;
                    mov         ecx, 11;
                    pxor        mm7, mm7;

main5a:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main5a;
 
                    mov         esi, alignedBlock;
                    mov         edi, ppxlcCurrQMB; 
                    mov         ecx, ppxliErrorQMB;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main5b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    movq        mm1, mm6;
                    ComputeBilinear();
//                    pavgb       mm0_mm6;
                    AddErrorAndClip(ecx);
                    movq        [edi], mm1;
                    add         ecx, 16;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         iLoop;
                    jnz         main5b;
                    emms;
                }         
            }
        }
        else { // bXSubPxl && bYSubPxl
            if (iMixedPelMV == 0) {
                const PixelC* pLine1 = ppxlcRef - iWidthFrm;
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthFrm;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main6a:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main6a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcCurrQMB;
                    mov         ebx, ppxliErrorQMB
                    mov         ecx, 8;
                   
main6b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    AddErrorAndClip(ebx);
                    movq        [edi], mm1;
                    add         ebx, 16;
                    add         esi, 8;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main6b;
                    emms;
                }                      
            } else {
                // H Q H
                //     I
                //alignedBlock
                PixelC* pRef = (PixelC*) (ppxlcRef + iWidthFrm + 1); 
                const PixelC* pLine4 = pRef + iWidthFrm;
                const PixelC* pLine1 = pRef - 2 * iWidthFrm;
                I32_WMV iLoop = 8;
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthFrm;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main7a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main7a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcCurrQMB;
                    mov         ecx, ppxliErrorQMB;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main7b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    movq        mm1, mm6;
                    ComputeBilinear();
//                    pavgb       mm0_mm6;
                    AddErrorAndClip(ecx);
                    movq        [edi], mm1;
                    add         ecx, 16;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         iLoop;
                    jnz         main7b;
                    emms;
                }                  
            }
        }
    }
}

Void_WMV MotionCompMixed_KNI (
                        tWMVDecInternalMember *pWMVDec,
                        PixelC*              ppxlcPredMB,
                        const PixelC*        ppxlcRefMB,
                        I32_WMV                  iWidthPrev,
                        Bool_WMV                 bInterpolateX,
                        Bool_WMV                 bInterpolateY,
                        I32_WMV                  iMixedPelMV
                        )
{    
    PixelC* ppxlcPred = ppxlcPredMB;
        
    static const __int64 const_09_09_09_09 = 0x0009000900090009;
    static const __int64 const_08_08_08_08 = 0x0008000800080008;

    PixelC tempBlock[11 * 8 + 15];
    PixelC *alignedBlock = (PixelC*) (((int)tempBlock + 15) & ~15);

    if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            I32_WMV iy;
            assert(iMixedPelMV == 0);            
            for (iy = 0; iy < 8; iy++) {
                *(U64 *)(ppxlcPredMB) = *(U64 *)(ppxlcRefMB);
                ppxlcRefMB += iWidthPrev;
                ppxlcPredMB += iWidthPrev;
            }
        }
        else {  //bXSubPxl && !bYSubPxl            
            PixelC* pRef = (PixelC*) (ppxlcRefMB + 1);
            if (iMixedPelMV == 0) {
                _asm {
                    mov         esi, pRef;
                    mov         edi, ppxlcPred;
                    mov         edx, iWidthPrev;
                    mov         ecx, 8;
                    pxor        mm7, mm7;

main1a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main1a;
                    emms;
                }
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___
                _asm {
                    mov         esi, pRef;
                    mov         edi, ppxlcPred;
                    mov         edx, iWidthPrev;
                    mov         ecx, 8;
                    pxor        mm7, mm7;
                    
main1b:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        mm1, [esi];
                    pavgb       mm0_mm1;
                    movq        [edi], mm0;

                    add         esi, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main1b;
                    emms;
                }
            }
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            PixelC* pRef = (PixelC*) (ppxlcRefMB + iWidthPrev);
            const PixelC* pLine4 = pRef + iWidthPrev;
            const PixelC* pLine3 = pRef;
            const PixelC* pLine2 = pLine3 - iWidthPrev;
            const PixelC* pLine1 = pLine2 - iWidthPrev;                                

            if (iMixedPelMV == 0) {                             
                _asm {
                    mov         esi, pLine1;
                    mov         eax, pLine4;
                    mov         edi, ppxlcPred;
                    mov         edx, iWidthPrev;
                    mov         ecx, 8;
                    pxor        mm7, mm7;                   
main2a:
                    ComputeEightBytesCubicMC(esi, esi + edx, esi + 2*edx, eax);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         eax, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main2a;
                    emms;
                }                
            } else {
                //      H Q H' 
                //      I                     
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthPrev;
                    mov         ecx, 11;
                    pxor        mm7, mm7;

main2b:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main2b;
 
                    mov         esi, alignedBlock;
                    mov         edi, ppxlcPred;
                    mov         ecx, 8;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main2c:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    pavgb       mm0_mm6;
                    movq        [edi], mm0;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main2c;
                    emms;
                }        
            }
        }
        else { // bXSubPxl && bYSubPxl
            PixelC* pRef = (PixelC*) (ppxlcRefMB + iWidthPrev + 1);
            const PixelC* pLine4 = pRef + iWidthPrev;
            const PixelC* pLine3 = pRef;
            const PixelC* pLine2 = pLine3 - iWidthPrev;
            const PixelC* pLine1 = pLine2 - iWidthPrev;                
            if (iMixedPelMV == 0) {

                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthPrev;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main3a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main3a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcPred;
                    mov         ecx, 8;
                   
main3b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        [edi], mm0;
                    add         esi, 8;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main3b;
                    emms;
                }   
            } else {
                // H Q H
                //     I
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthPrev;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main4a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main4a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcPred;
                    mov         ecx, 8;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main4b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    pavgb       mm0_mm6;
                    movq        [edi], mm0;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main4b;
                    emms;
                }        
            }
        }
    }
}

Void_WMV MotionCompMixedAndAddError_KNI (
                                   tWMVDecInternalMember *pWMVDec,
                                   PixelC __huge* ppxlcCurrQMB, 
                                   const Buffer __huge* ppxliErrorBuf, 
                                   const PixelC __huge* ppxlcRef, 
                                   I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven, 
                                   I32_WMV iMixedPelMV
                                   )
{
    PixelC tempBlock[11 * 8 + 15];
    PixelC *alignedBlock = (PixelC*) (((int)tempBlock + 15) & ~15);

    static const __int64 const_09_09_09_09 = 0x0009000900090009;
    static const __int64 const_08_08_08_08 = 0x0008000800080008;

    const I16_WMV __huge *ppxliErrorQMB = ppxliErrorBuf->i16;
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                _asm {
                    mov         esi, ppxliErrorQMB;
                    mov         edx, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         ecx, 8;
                    pxor        mm7, mm7;
                    mov         ebx, iWidthFrm;
main0:
                    movq        mm0, [edx];
                    AddErrorAndClip(esi);
                    movq        [edi], mm1;

                    add         esi, 16;
                    add         edx, ebx;
                    add         edi, ebx;
                    dec         ecx;
                    jnz         main0;
                    emms;
                }
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //         ___                               
                _asm {
                    mov         esi, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         edx, ppxliErrorQMB;
                    mov         ecx, 8;
                    mov         ebx, iWidthFrm;
                    pxor        mm7, mm7;
main1:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        mm1, [esi];
                    pavgb       mm0_mm1;
                    AddErrorAndClip(edx);
                    movq        [edi], mm1;
                    
                    add         esi, ebx;
                    add         edi, ebx;
                    add         edx, 16;
                    dec         ecx;
                    jnz         main1;
                    emms;
                }
            }
        }
        else {  //bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                _asm {
                    mov         esi, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         edx, ppxliErrorQMB;
                    mov         ecx, 8;
                    mov         ebx, iWidthFrm;
                    pxor        mm7, mm7;
main2:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    AddErrorAndClip(edx);
                    movq        [edi], mm1;
                    add         esi, ebx;
                    add         edi, ebx;
                    add         edx, 16;
                    dec         ecx;
                    jnz         main2;
                    emms;
                }
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___   
                _asm {
                    mov         esi, ppxlcRef;
                    mov         edi, ppxlcCurrQMB;
                    mov         edx, ppxliErrorQMB;
                    mov         ecx, 8;
                    mov         ebx, iWidthFrm;
                    pxor        mm7, mm7;
main3:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        mm1, [esi + 1];
                    pavgb       mm0_mm1;
                    AddErrorAndClip(edx);
                    movq        [edi], mm1;
                    
                    add         esi, ebx;
                    add         edi, ebx;
                    add         edx, 16;
                    dec         ecx;
                    jnz         main3;
                    emms;
                }                
            }
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
            if (iMixedPelMV == 0) {
                const PixelC* pLine1 = ppxlcRef - iWidthFrm;
                const PixelC* pLine4 = ppxlcRef + 2 * iWidthFrm;   

                _asm {
                    mov         esi, pLine1;
                    mov         eax, pLine4;
                    mov         edi, ppxlcCurrQMB;
                    mov         ebx, ppxliErrorQMB;
                    mov         edx, iWidthFrm;
                    mov         ecx, 8;
                    pxor        mm7, mm7;                   
main4:
                    ComputeEightBytesCubicMC(esi, esi + edx, esi + 2*edx, eax);
                    AddErrorAndClip(ebx);
                    movq        [edi], mm1;
                    add         esi, edx;
                    add         eax, edx;
                    add         edi, edx;
                    add         ebx, 16;
                    dec         ecx;
                    jnz         main4;
                    emms;
                } 
            } else {
                //      H Q H' 
                //      I                              
                PixelC* pRef = (PixelC*) (ppxlcRef + iWidthFrm);                   
                const PixelC* pLine4 = pRef + iWidthFrm; 
                const PixelC* pLine1 = pRef - 2 * iWidthFrm;
                I32_WMV iLoop = 8;
                //      H Q H' 
                //      I                     
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthFrm;
                    mov         ecx, 11;
                    pxor        mm7, mm7;

main5a:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main5a;
 
                    mov         esi, alignedBlock;
                    mov         edi, ppxlcCurrQMB; 
                    mov         ecx, ppxliErrorQMB;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main5b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    pavgb       mm0_mm6;
                    AddErrorAndClip(ecx);
                    movq        [edi], mm1;
                    add         ecx, 16;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         iLoop;
                    jnz         main5b;
                    emms;
                }         
            }
        }
        else { // bXSubPxl && bYSubPxl
            if (iMixedPelMV == 0) {
                const PixelC* pLine1 = ppxlcRef - iWidthFrm;
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthFrm;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main6a:
                    ComputeEightBytesCubicMC(esi - 1, esi, esi + 1, esi + 2);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main6a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcCurrQMB;
                    mov         ebx, ppxliErrorQMB
                    mov         ecx, 8;
                   
main6b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    AddErrorAndClip(ebx);
                    movq        [edi], mm1;
                    add         ebx, 16;
                    add         esi, 8;
                    add         edi, edx;
                    dec         ecx;
                    jnz         main6b;
                    emms;
                }                      
            } else {
                // H Q H
                //     I
                //alignedBlock
                PixelC* pRef = (PixelC*) (ppxlcRef + iWidthFrm + 1); 
                const PixelC* pLine4 = pRef + iWidthFrm;
                const PixelC* pLine1 = pRef - 2 * iWidthFrm;
                I32_WMV iLoop = 8;
                _asm {
                    mov         esi, pLine1;
                    mov         edi, alignedBlock;
                    mov         edx, iWidthFrm;
                    mov         ecx, 11;
                    pxor        mm7, mm7;
main7a:
                    ComputeEightBytesCubicMC(esi - 2, esi - 1, esi, esi + 1);
                    movq        [edi], mm0;
                    add         esi, edx;
                    add         edi, 8;
                    dec         ecx;
                    jnz         main7a;

                    mov         esi, alignedBlock;
                    mov         edi, ppxlcCurrQMB;
                    mov         ecx, ppxliErrorQMB;
                    mov         eax, pLine1;
                    mov         ebx, pLine4;
main7b:
                    ComputeEightBytesCubicMC(esi, esi + 8, esi + 16, esi + 24);
                    movq        mm6, mm0;
                    ComputeEightBytesCubicMC(eax, eax + edx, eax + 2*edx, ebx);
                    pavgb       mm0_mm6;
                    AddErrorAndClip(ecx);
                    movq        [edi], mm1;
                    add         ecx, 16;
                    add         esi, 8;
                    add         eax, edx;
                    add         ebx, edx;
                    add         edi, edx;
                    dec         iLoop;
                    jnz         main7b;
                    emms;
                }                  
            }
        }
    }
}



#define WMV_ESC_Decoding()                                                          \
	if (pWMVDec->m_bFirstEscCodeInFrame){                                                    \
        decodeBitsOfESCCode (pWMVDec);                                                     \
        pWMVDec->m_bFirstEscCodeInFrame = FALSE_WMV;                                             \
	}                                                                               \
	uiRun = BS_getBits (pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_RUN);			                    \
	if (BS_getBit (pWMVDec->m_pbitstrmIn)) /* escape decoding */                              \
		iLevel = -1 * BS_getBits (pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_LEVEL);                 \
	else                                                                            \
		iLevel = BS_getBits (pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_LEVEL);                      

int dbg_cnt1;

//This is not really an MMX routine but works along side the MMX idct
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MMX(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode)
{
	Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec->hufDCTACDec;
	I8_WMV* rgLevelAtIndx = InterDCTTableInfo_Dec->pcLevelAtIndx;
	U8_WMV* rgRunAtIndx = InterDCTTableInfo_Dec->puiRunAtIndx;
//  Change iStartIndxOfLastRun to iStartIndxOfLastRunMinus1 and <= to <
//	iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1;
	U8_WMV* rgIfNotLastNumOfLevelAtRun = InterDCTTableInfo_Dec->puiNotLastNumOfLevelAtRun;
	U8_WMV* rgIfLastNumOfLevelAtRun = InterDCTTableInfo_Dec->puiLastNumOfLevelAtRun; 
	U8_WMV* rgIfNotLastNumOfRunAtLevel = InterDCTTableInfo_Dec->puiNotLastNumOfRunAtLevel;
	U8_WMV* rgIfLastNumOfRunAtLevel = InterDCTTableInfo_Dec->puiLastNumOfRunAtLevel;
//	U32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
	I32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
	I32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec->iTcoef_ESCAPE;

    Bool_WMV bIsLastRun = FALSE_WMV;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
//    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
    //Align on cache line
    I16_WMV *rgiCoefRecon = (I16_WMV*)pWMVDec->m_rgiCoefRecon;
	register I32_WMV iDCTHorzFlags = 0;

//    memset (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));
	_asm {
		xor		edx,edx
		mov		edi,rgiCoefRecon
		mov		dword ptr [edi],edx
		mov		dword ptr [edi+0x4],edx
		mov		dword ptr [edi+0x8],edx
		mov		dword ptr [edi+0xc],edx
		mov		dword ptr [edi+0x10],edx
		mov		dword ptr [edi+0x14],edx
		mov		dword ptr [edi+0x18],edx
		mov		dword ptr [edi+0x1c],edx
		mov		dword ptr [edi+0x20],edx
		mov		dword ptr [edi+0x24],edx
		mov		dword ptr [edi+0x28],edx
		mov		dword ptr [edi+0x2c],edx
		mov		dword ptr [edi+0x30],edx
		mov		dword ptr [edi+0x34],edx
		mov		dword ptr [edi+0x38],edx
		mov		dword ptr [edi+0x3c],edx
		mov		dword ptr [edi+0x40],edx
		mov		dword ptr [edi+0x44],edx
		mov		dword ptr [edi+0x48],edx
		mov		dword ptr [edi+0x4c],edx
		mov		dword ptr [edi+0x50],edx
		mov		dword ptr [edi+0x54],edx
		mov		dword ptr [edi+0x58],edx
		mov		dword ptr [edi+0x5c],edx
		mov		dword ptr [edi+0x60],edx
		mov		dword ptr [edi+0x64],edx
		mov		dword ptr [edi+0x68],edx
		mov		dword ptr [edi+0x6c],edx
		mov		dword ptr [edi+0x70],edx
		mov		dword ptr [edi+0x74],edx
		mov		dword ptr [edi+0x78],edx
		mov		dword ptr [edi+0x7c],edx
	}

    do {

	    dbg_cnt1++;
		I32_WMV lIndex = Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
        register I32_WMV iIndex;

		//lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
			return WMV_CorruptedBits;
		}
        //lIndex = pWMVDec->m_pentrdecDCT->decodeSymbol();
        // if Huffman
        if (lIndex != iTCOEF_ESCAPE)	{
			if (lIndex >= iStartIndxOfLastRun)
				bIsLastRun = TRUE_WMV;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
            if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
                return WMV_CorruptedBits;
            }
        } 
		else {
			if (BS_getBit (pWMVDec->m_pbitstrmIn)){
                I32_WMV lIndex2;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
                    return WMV_CorruptedBits;
                }
				// ESC + '1' + VLC
        		//lIndex2 = hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
				lIndex2 = Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
				//lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn ) || lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
			        return WMV_CorruptedBits;
		        }
				uiRun = rgRunAtIndx[lIndex2];
				iLevel = rgLevelAtIndx[lIndex2];
				if (lIndex2 >= iStartIndxOfLastRun){
					bIsLastRun = TRUE_WMV;
					iLevel += (I8_WMV) rgIfLastNumOfLevelAtRun[uiRun];
				}
				else
					iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRun[uiRun];
				if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
                    return WMV_CorruptedBits;
                }
			}
			else if (BS_getBit (pWMVDec->m_pbitstrmIn)){
                I32_WMV lIndex2;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
                    return WMV_CorruptedBits;
                }
				// ESC + '10' + VLC
        		//lIndex2 = hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
				lIndex2 = Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
				//lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn ) || lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
			        return WMV_CorruptedBits;
		        }
				uiRun = rgRunAtIndx[lIndex2];
				iLevel = rgLevelAtIndx[lIndex2];
				if (lIndex2 >= iStartIndxOfLastRun){
					bIsLastRun = TRUE_WMV;
					uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
				}
				else
					uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
				if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
                    return WMV_CorruptedBits;
                }
			}
			else{
				// ESC + '00' + FLC
				bIsLastRun = (Bool_WMV) BS_getBit (pWMVDec->m_pbitstrmIn); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){  // == WMV1 || WMV2
                    WMV_ESC_Decoding();
                }
                else{
					uiRun = BS_getBits (pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);			
        			iLevel = (I8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
				}
		        if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
			        return WMV_CorruptedBits;
		        }
			}
        }
        uiCoefCounter += uiRun;

		if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
			return WMV_CorruptedBits;
        }

        
        iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];

        // If this coefficient is not in the first column then set the flag which indicates
        // what row it is in. This flag field will be used by the IDCT to see if it can
        // shortcut the IDCT of the row if all coefficients are zero.
		if (iIndex & 0x7)
			iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

        if (iLevel == 1)
			rgiCoefRecon [pZigzagInv [uiCoefCounter]] = (I16_WMV) pWMVDec->m_i2DoublePlusStepSize;
		else if (iLevel == -1)
			rgiCoefRecon [pZigzagInv [uiCoefCounter]] = (I16_WMV) pWMVDec->m_i2DoublePlusStepSizeNeg;
		else if (iLevel > 0)
//			I32_WMV iCoefRecon = pWMVDec->m_iStepSize * ((iLevel << 1) + 1) - pWMVDec->m_bStepSizeIsEven;
			rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
		else
//			I32_WMV iCoefRecon = pWMVDec->m_bStepSizeIsEven - pWMVDec->m_iStepSize * (1 - (iLevel << 1));
			rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;

        uiCoefCounter++;
    } while (!bIsLastRun);
    // Save the DCT row flags. This will be passed to the IDCT routine
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;

    return WMV_Succeeded;
}

I32_WMV BlkAvgX8_MMX(const PixelC* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize)
{
    I32_WMV iSum = 0;
	__asm {
		mov             ebx, ppxlcCurrRecnMB;
        mov             eax, iWidthPrev
		mov             ecx, BLOCK_SIZE

		pxor            mm6, mm6 // keep summation
		pxor            mm7, mm7 // zero

	mainLoopS:
		movq            mm0, [ebx]
		movq            mm2, mm0
		punpcklbw       mm2, mm7
		paddusw         mm6, mm2
		
		punpckhbw       mm0, mm7
		paddusw         mm6, mm0

		add             ebx, eax
		dec             ecx
		jne             mainLoopS

        // sum the four WORD up in one mm6
		movq            mm0, mm6
		punpcklwd       mm0, mm7
		punpckhwd       mm6, mm7
		paddd           mm6, mm0

		movq            mm0, mm6
		punpckldq       mm0, mm7
		punpckhdq       mm6, mm7
		paddd           mm6, mm0

        movd            edi, mm6
        mov             iSum, edi

        emms 
    }
    return ((iSum + 4) >> 3) / iStepSize;  // iSum / 64 * 8
};

Void_WMV g_RepeatRef0Y_MMX (
	PixelC* ppxlcRef0Y,
	CoordI  iStart, 
    CoordI  iEnd,
	I32_WMV     iOldLeftOffet,
	Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthY,
    I32_WMV     iWidthYPlusExp,
    I32_WMV     iWidthPrevY
)
{
	const PixelC* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
	const PixelC* ppxliOldRight = ppxliOldLeft + iWidthY - 1;
	const PixelC* ppxliOldTopLn = ppxliOldLeft - EXPANDY_REFVOP;
	PixelC* ppxliNew = (PixelC*) ppxliOldTopLn;
    I32_WMV iResidue = iWidthPrevY - iWidthYPlusExp;
	
//	I32_WMV iWidthPrevY = m_iWidthPrevY, iWidthYPlusExp = m_iWidthYPlusExp;
	I32_WMV iter = iEnd - iStart;
	const I32_WMV iPnewInc = iter * iWidthPrevY;
	static const __int64	x00000000000000FF = 0x00000000000000FF;
    I32_WMV iWidthPrevYDiv8;
    
	__asm {
        mov			eax, dword ptr [ppxliNew]
        mov			ebx, dword ptr [ppxliOldLeft]
        mov			ecx, dword ptr [iter]
        mov			edx, dword ptr [ppxliOldRight]
        mov			edi, dword ptr [iWidthPrevY]
        mov			esi, dword ptr [iWidthYPlusExp]

        movq		mm7, x00000000000000FF

NEXT_ROW:
        // left
        movd		mm6, [ebx]
        pand		mm6, mm7

        movq		mm0, mm6
        psllq		mm6, 8
        por			mm6, mm0

        movq		mm0, mm6
        psllq		mm6, 16
        por			mm6, mm0

        movq		mm0, mm6
        psllq		mm6, 32
        por			mm6, mm0

        movq		[eax], mm6
        movq		[eax + 8], mm6
        movq		[eax + 16], mm6
        movq		[eax + 24], mm6

        add			eax, esi

        // right
        movd		mm6, [edx]
        pand		mm6, mm7

        movq		mm0, mm6
        psllq		mm6, 8
        por			mm6, mm0

        movq		mm0, mm6
        psllq		mm6, 16
        por			mm6, mm0

        movq		mm0, mm6
        psllq		mm6, 32
        por			mm6, mm0

        movq		[eax], mm6
        movq		[eax + 8], mm6
        movq		[eax + 16], mm6
        movq		[eax + 24], mm6

//        add			eax, EXPANDY_REFVOP
        add			eax, dword ptr [iResidue]
        add			ebx, edi
        add			edx, edi

        dec			ecx
        jne			NEXT_ROW

        emms
	}

	ppxliOldRight = ppxliOldLeft + iWidthY - 1;
	ppxliNew = (PixelC*) ppxliOldTopLn;
    I32_WMV iResidueMB = iWidthPrevY - iWidthYPlusExp - EXPANDY_REFVOP;
	
    if (iResidueMB != 0) {
	    CoordI y;
	    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
		    ppxliNew += iWidthYPlusExp + EXPANDY_REFVOP;		
		    memset (ppxliNew, *ppxliOldRight, iResidueMB);
		    ppxliNew += iResidueMB;
		    ppxliOldRight += iWidthPrevY;
	    }
    } else
        ppxliNew += (iEnd - iStart) * (iWidthYPlusExp + EXPANDY_REFVOP);

//	ppxliNew += iPnewInc;
    
	iWidthPrevYDiv8 = iWidthPrevY >> 3;
	if (fTop) {
		PixelC* ppxliLeftTop = ppxlcRef0Y;
		__asm {
            mov			eax, dword ptr [ppxliLeftTop]	
            mov			edx, EXPANDY_REFVOP
NEXT_ROW2:
            mov			ecx, dword ptr [iWidthPrevYDiv8]
            mov			ebx, dword ptr [ppxliOldTopLn]
NEXT_FIELD2:
            movq		mm0, [ebx]
            movq		[eax], mm0

            add			eax, 8
            add			ebx, 8

            dec			ecx
            jne			NEXT_FIELD2

            dec			edx
            jne			NEXT_ROW2

            emms
		}
	}
	if (fBottom) {
        I32_WMV iHeightYPadded = (iEnd + 15) & ~15;
        I32_WMV iBotExtend = iHeightYPadded - iEnd + EXPANDY_REFVOP;
		const PixelC* ppxliOldBotLn = ppxliNew - iWidthPrevY;
		__asm {
            mov			eax, dword ptr [ppxliNew]	
            mov			edx, dword ptr [iBotExtend]
NEXT_ROW3:
            mov			ecx, dword ptr [iWidthPrevYDiv8]
            mov			ebx, dword ptr [ppxliOldBotLn]
NEXT_FIELD3:
            movq		mm0, [ebx]
            movq		[eax], mm0

            add			eax, 8
            add			ebx, 8

            dec			ecx
            jne			NEXT_FIELD3

            dec			edx
            jne			NEXT_ROW3

            emms
		}
	}
}

Void_WMV g_RepeatRef0UV_MMX (
	PixelC* ppxlcRef0U,
	PixelC* ppxlcRef0V,
	CoordI  iStart, 
    CoordI  iEnd,
	I32_WMV     iOldLeftOffet,
	Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV
)
{
	const PixelC* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
	const PixelC* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
	const PixelC* ppxliOldRightU = ppxliOldLeftU + iWidthUV - 1;
	const PixelC* ppxliOldRightV = ppxliOldLeftV + iWidthUV - 1;
	const PixelC* ppxliOldTopLnU = ppxliOldLeftU - EXPANDUV_REFVOP;
	const PixelC* ppxliOldTopLnV = ppxliOldLeftV - EXPANDUV_REFVOP;
	PixelC* ppxliNewU = (PixelC*) ppxliOldTopLnU;
	PixelC* ppxliNewV = (PixelC*) ppxliOldTopLnV;
    I32_WMV iResidue = iWidthPrevUV - iWidthUVPlusExp;
	CoordI y;
    I32_WMV iWidthPrevUVDiv8;
	for (y = iStart; y < iEnd; y++) { // x-direction interpolation
		memset (ppxliNewU, *ppxliOldLeftU, EXPANDUV_REFVOP);
		memset (ppxliNewV, *ppxliOldLeftV, EXPANDUV_REFVOP);
		ppxliNewU += iWidthUVPlusExp;		
		ppxliNewV += iWidthUVPlusExp;		
//		memset(ppxliNewU, *ppxliOldRightU, EXPANDUV_REFVOP);
//		memset(ppxliNewV, *ppxliOldRightV, EXPANDUV_REFVOP);
//		ppxliNewU += EXPANDUV_REFVOP;		
//		ppxliNewV += EXPANDUV_REFVOP;		
		memset(ppxliNewU, *ppxliOldRightU, iResidue);
		memset(ppxliNewV, *ppxliOldRightV, iResidue);
		ppxliNewU += iResidue;		
		ppxliNewV += iResidue;		
		ppxliOldLeftU += iWidthPrevUV;
		ppxliOldLeftV += iWidthPrevUV;
		ppxliOldRightU += iWidthPrevUV;
		ppxliOldRightV += iWidthPrevUV;
	}
	iWidthPrevUVDiv8 = iWidthPrevUV >> 3;
	if (fTop) {
		PixelC* ppxliLeftTopU = ppxlcRef0U;
		PixelC* ppxliLeftTopV = ppxlcRef0V;
		__asm {
            mov			eax, dword ptr [ppxliLeftTopU]	
            mov			edx, EXPANDUV_REFVOP
            mov			edi, dword ptr [ppxliLeftTopV]	
NEXT_ROW2:
            mov			ebx, dword ptr [ppxliOldTopLnU]
            mov			ecx, dword ptr [iWidthPrevUVDiv8]
            mov			esi, dword ptr [ppxliOldTopLnV]
NEXT_FIELD2:
            // U
            movq		mm0, [ebx]
            movq		[eax], mm0

            add			eax, 8
            add			ebx, 8

            // V
            movq		mm0, [esi]
            movq		[edi], mm0

            add			esi, 8
            add			edi, 8

            dec			ecx
            jne			NEXT_FIELD2

            dec			edx
            jne			NEXT_ROW2

            emms
		}
	}
	if (fBottom) {
		const PixelC* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
		const PixelC* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;
        I32_WMV iHeightUVPadded = ((iEnd * 2 + 15) & ~15)/2;
        I32_WMV iBotExtend = iHeightUVPadded - iEnd + EXPANDUV_REFVOP; 

		__asm {
            mov			eax, dword ptr [ppxliNewU]	
//            mov			edx, EXPANDUV_REFVOP
            mov			edx, dword ptr [iBotExtend]
            mov			edi, dword ptr [ppxliNewV]	
NEXT_ROW3:
            mov			ecx, dword ptr [iWidthPrevUVDiv8]
            mov			ebx, dword ptr [ppxliOldBotLnU]
            mov			esi, dword ptr [ppxliOldBotLnV]
NEXT_FIELD3:
            // U
            movq		mm0, [ebx]
            movq		[eax], mm0

            add			eax, 8
            add			ebx, 8

            // V
            movq		mm0, [esi]
            movq		[edi], mm0

            add			esi, 8
            add			edi, 8

            dec			ecx
            jne			NEXT_FIELD3

            dec			edx
            jne			NEXT_ROW3

            emms
		}
	}
}
#endif // _WMV_TARGET_X86_ || _Embedded_x86
