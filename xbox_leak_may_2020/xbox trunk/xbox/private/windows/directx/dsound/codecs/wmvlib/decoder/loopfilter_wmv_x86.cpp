/*************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

        vopSes.cpp

Abstract:

        Base class for the encoder for one VOP session.

Author:

        Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
        Chuang Gu (chuanggu@microsoft.com) 10-December-1999

Revision History:

*************************************************************************/
#include "bldsetup.h"

#include "xplatform.h"
#include "typedef.hpp"
#include "wmvdec_member.h"
#include <stdlib.h>
#include "motioncomp_wmv.h"
#include "tables_wmv.h"
#if defined(macintosh) && defined(_MAC_VEC_OPT)
#include "motioncomp_altivec.h"
#endif

#ifdef _WMV_TARGET_X86_
#include "opcodes.h"
#endif 

#ifdef _WMV_TARGET_X86_

Void_WMV g_FilterHorizontalEdge_MMX(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                                 I32_WMV iNumPixel)
{
    static const Int64 x05 = 0x0005000500050005;
    static const Int64 x04 = 0x0004000400040004;

    pV -= iPixelDistance*3;
    iNumPixel >>= 2;
    _asm {        
        mov         eax,iPixelDistance
        mov         esi,pV
        mov         ecx,iNumPixel
        mov         ebx,eax
        neg         ebx
            
HLoop:                         
        // ---------------------------
        // DefaultMode4_KNI
        lea         edi,[esi+eax*4]
        add         edi,eax

        pxor        mm7,mm7
        movq        mm6,x05
        movq        mm5,x04

        //calculate a3.1
        movd        mm0,[esi+ebx]
        punpcklbw   mm0,mm7
        paddw       mm0,mm0
        movd        mm1,[esi]
        punpcklbw   mm1,mm7
        pmullw      mm1,mm6
        psubw       mm0,mm1
        movd        mm1,[esi+eax]
        punpcklbw   mm1,mm7
        movq        mm2,mm1
        pmullw      mm1,mm6
        paddw       mm0,mm1
        movd        mm1,[esi+eax*2]
        punpcklbw   mm1,mm7
        psubw       mm0,mm1
        psubw       mm0,mm1
        paddw       mm0,mm5
        psraw       mm0,3

        //Calculate a3.0
        paddw       mm2,mm2
        pmullw      mm1,mm6
        psubw       mm2,mm1
        movd        mm1,[edi+ebx*2]
        punpcklbw   mm1,mm7
        movq        mm3,mm1
        pmullw      mm1,mm6
        paddw       mm1,mm2
        movd        mm2,[edi+ebx]
        punpcklbw   mm2,mm7
        psubw       mm1,mm2
        psubw       mm1,mm2
        paddw       mm1,mm5
        psraw       mm1,3

        //Calculate a3.2
        paddw       mm3,mm3
        pmullw      mm2,mm6
        psubw       mm3,mm2
        movd        mm2,[edi]
        punpcklbw   mm2,mm7
        pmullw      mm2,mm6
        paddw       mm2,mm3
        movd        mm3,[edi+eax]
        punpcklbw   mm3,mm7
        psubw       mm2,mm3
        psubw       mm2,mm3
        paddw       mm2,mm5
        psraw       mm2,3

        //SIGN(a30)
        movq        mm5,mm1
        psraw       mm5,16
       
        //MIN(abs(a30),abs(a31),abs(a32))
        movq        mm3,mm0  //abs(a3.1)
        psraw       mm3,16
        pxor        mm0,mm3
        psubw       mm0,mm3

        movq        mm3,mm1  //abs(a3.0)
        movq        mm4,mm1
        psraw       mm3,16
        pxor        mm4,mm3
        psubw       mm4,mm3

        movq        mm3,mm2  //abs(a3,1)
        psraw       mm3,16
        pxor        mm2,mm3
        psubw       mm2,mm3

        movq        mm3,mm4  //if (abs(a3.0) < abs(a3.1)) mm0 = abs(a3.0) else mm0 = abs(a3.1)
        pcmpgtw     mm3,mm0  
        pand        mm0,mm3
        pandn       mm3,mm4
        por         mm0,mm3

        movq        mm3,mm2  //if (mm0 < abs(a3,2)) mm0 = a3.2
        pcmpgtw     mm3,mm0
        pand        mm0,mm3
        pandn       mm3,mm2
        por         mm0,mm3

        pxor        mm0,mm5  //a3.0' = mm0 = (mm0 * SIGN(a3.0))
        psubw       mm0,mm5

        //Broadcast iStepSize
        movd        mm5,iStepSize
        punpcklwd   mm5,mm5
        punpckldq   mm5,mm5
        
        // d = CLIP( (5*(A30-a30)/8),0,  ((v4-v5)/2)   *   ((abs(a30) < iStepSize) ? 1 : 0)   );
        movq        mm6,mm5     // generate mask for  * ((abs(a30) < iStepSize) ? 1 : 0)
        pcmpgtw     mm6,mm4

        psubw       mm0,mm1     //d_temp = 5*(A30-a30)/8
        pmullw      mm0,x05
        movq        mm5,mm0
        psraw       mm5,16
        pxor        mm0,mm5
        psubw       mm0,mm5
        psraw       mm0,3
        pxor        mm0,mm5
        psubw       mm0,mm5
        
        movd        mm3,[esi+eax*2]  //max = (v4-v5)/2
        punpcklbw   mm3,mm7
        movq        mm4,mm3
        movd        mm2,[edi+ebx*2]
        punpcklbw   mm2,mm7
        movq        mm7,mm2
        psubw       mm3,mm2
        pxor        mm3,mm5
        psubw       mm3,mm5
        psraw       mm3,1
        pxor        mm3,mm5
        psubw       mm3,mm5
        
        movq        mm5,mm3         //abs(max)     
        psraw       mm5,16
        pxor        mm3,mm5
        psubw       mm3,mm5
        pxor        mm0,mm5         //if (max < 0) d_temp = d_temp*-1;
        psubw       mm0,mm5

        movq        mm2,mm3         //Clip to less than max
        pcmpgtw     mm2,mm0
        pand        mm0,mm2
        pandn       mm2,mm3
        por         mm0,mm2
        
        pxor        mm2,mm2         //Clip to zero
        pcmpgtw     mm2,mm0
        pandn       mm2,mm0

        pxor        mm2,mm5         //Restore original sign
        psubw       mm2,mm5

        pand        mm2,mm6         //mask out 

        psubw       mm4,mm2
        paddw       mm7,mm2

        packuswb    mm4,mm4
        movd        [esi+eax*2],mm4
        packuswb    mm7,mm7
        movd        [edi+ebx*2],mm7

        // ---------------------------------
        add         esi,4
                
        dec         ecx;
        jnz         HLoop;

        emms;
    }
}


Void_WMV g_FilterVerticalEdge_MMX(U8_WMV* pV1, I32_WMV iPixelDistance, 
                              I32_WMV iStepSize, I32_WMV iVertSize)
{
    static const Int64 s1 = 0xFFFB0002FFFB0002;
    static const Int64 s2 = 0xFFFE0005FFFE0005;
    static const Int64 s3 = 0xFFFE0005FFFB0002;
    static const Int64 x00000004 = 0x0000000400000004;       
    static const Int64 x00000005 = 0x0000000500000005;
    // Filter vertical lines

    _asm {
        mov         esi,pV1
        mov         edi,iVertSize
        mov         eax,iPixelDistance
        pxor        mm7,mm7
            
MainLoop:        
        movzx       ecx,byte Ptr [esi+4]
        mov         eax,ecx
        movzx       ebx,byte Ptr [esi+5]
        
        movq        mm0,[esi+1]
        movq        mm5,s1
        movq        mm6,s2
        movq        mm1,mm0
        punpcklbw   mm1,mm7   
        pmaddwd     mm1,mm5
        movq        mm2,mm0
        psrlq       mm2,16
        punpcklbw   mm2,mm7   
        movq        mm3,mm2
        pmaddwd     mm2,mm6
        paddd       mm1,mm2
        paddd       mm1,x00000004
        psrad       mm1,3
        
        pxor        mm6,mm6
        pcmpgtb     mm6,mm1                    
        pxor        mm1,mm6
        psubd       mm1,mm6
        
        movq        mm4,mm1
        psrlq       mm1,32
        
        movd        edx,mm1
        cmp         edx,iStepSize
        jge         EndOfLoop
        
        sub         eax,ebx
        cdq
        xor         eax,edx
        sub         eax,edx
        
        shr         eax,1
        
        test        eax,eax
        jz          EndOfLoop
        
        movq        mm3,mm0
        punpckhbw   mm3,mm7
        pmaddwd     mm3,s3
        movq        mm2,mm3
        psrlq       mm2,32
        paddd       mm3,mm2
        paddd       mm3,x00000004
        psrad       mm3,3
        movq        mm2,mm1
        
        //From here on could be done 2 or 4 at a time
        //mm2=a30
        //mm3=a32
        //mm4=a31
        
        pxor        mm5,mm5
        pcmpgtd     mm5,mm3
        pxor        mm3,mm5
        psubd       mm3,mm5
        
        movq        mm5,mm2
        pcmpgtd     mm5,mm3
        pand        mm3,mm5
        pandn       mm5,mm2
        por         mm3,mm5
        
        movq        mm5,mm4
        pcmpgtd     mm5,mm3
        pand        mm3,mm5
        pandn       mm5,mm4
        por         mm3,mm5
        
        psubd       mm3,mm1
        
        pxor        mm4,mm4
        pcmpgtd     mm4,mm3
        pxor        mm3,mm4
        psubd       mm3,mm4
        
        pmaddwd     mm3,x00000005
        psrad       mm3,3        
        
        pxor        mm3,mm4
        psubd       mm3,mm4
        
        psrlq       mm6,32
        pxor        mm3,mm6
        psubd       mm3,mm6
        
        movd        mm5,edx
        
        pxor        mm3,mm5
        psubd       mm3,mm5
        
        movd        mm0,eax
        movq        mm1,mm0
        pcmpgtd     mm1,mm3
        pand        mm3,mm1
        pandn       mm1,mm0
        por         mm3,mm1
        
        pxor        mm1,mm1
        pcmpgtd     mm1,mm3
        pandn       mm1,mm3
        
        pxor        mm1,mm5
        psubd       mm1,mm5
        
        movd        eax,mm1
        
        sub         ecx,eax
        add         ebx,eax
        
        mov         [esi+4],cl          
        mov         [esi+5],bl
        
        
EndOfLoop:                
        mov        eax,iPixelDistance
        add        esi,eax
        
        dec        edi
        jnz        MainLoop

        emms;
    }

}

#endif // _WMV_TARGET_X86_
