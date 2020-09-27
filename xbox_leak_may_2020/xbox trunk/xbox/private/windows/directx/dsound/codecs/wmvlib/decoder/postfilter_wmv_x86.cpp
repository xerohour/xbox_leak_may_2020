#ifdef _SUPPORT_POST_FILTERS_
#include "bldsetup.h"

#include "xplatform.h"
#include "limits.h"
#include "stdio.h"
#include "stdlib.h"
#include "wmvdec_member.h"
#include "typedef.hpp"
#include "postfilter_wmv.hpp"
#include "cpudetect.h"
#include "opcodes.h"
#include "tables_wmv.h"

//THR1 = the threshold before we consider neighboring pixels to be "diffrent"
#define THR1 2       
//THR2 = the total number of "diffrent" pixels under which we use stronger filter
#define THR2 6
#define INV_THR2 3

#ifdef _WMV_TARGET_X86_
#pragma warning (disable:4799)
#pragma optimize ("",off)

Void_WMV FilterHorizantalEdge(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{
 
    static const Int64 thr1 = THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56);
    //static const Int64 neg_thr1 = NTHR1 | (NTHR1 << 8) | (NTHR1 << 16) | (NTHR1 << 24) | ((Int64)NTHR1 << 32) | ((Int64)NTHR1 << 40) | ((Int64)NTHR1 << 48) | ((Int64)NTHR1 << 56);
    
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
    static const Int64 x80 = 0x8080808080808080;
    static const Int64 x06 = 0x0006000600060006;
    static const Int64 x08 = 0x0008000800080008;
    static const Int64 x05 = 0x0005000500050005;
    static const Int64 x04 = 0x0004000400040004;
    static const Int64 x0001 = 0x0001000100010001;
    static const Int64 x0002 = 0x0002000200020002;

    Int64 act;
    Int64 iStepBroad, i2StepBroad;
    Int64 lminmax;
    Int64 lminmax2;
    Int64 mask;    
    I32_WMV i;
    I32_WMV mytemp;
    U8_WMV* pV0 = ppxlcCenter - (iPixelDistance*5);
    
    _asm {

        sub esp, 64 // IW
        movd        mm0,iStepSize       //Broadcast iStepSize
        punpcklbw   mm0,mm0
        punpcklwd   mm0,mm0
        punpckldq   mm0,mm0
        movq        mm5,mm0

/*        psllq       mm5,8
        por         mm0,mm5
        movq        mm5,mm0
        psllq       mm5,16
        por         mm0,mm5
        movq        mm5,mm0
        psllq       mm5,32
        por         mm0,mm5
        movq        mm5,mm0
  */      
        
        psllq       mm0,1
        psubb       mm5,x01 
        movq        iStepBroad,mm5
        movq        i2StepBroad,mm0
        
        mov         eax,iPixelDistance
        mov         esi,pV0
        lea         esi,[esi+eax*2]

        mov         ecx,iEdgeLength
        shr         ecx,3
HLoop:
        mov         i,ecx

        mov         eax,iPixelDistance
        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,thr1
        movq        mm6,x01

        //phi(v0 - v1)
        movq        mm0,[esi+ebx*2]     //Load
        movq        mm1,[esi+ebx]
        psubb       mm0,mm1             //Subtract
        pxor        mm2,mm2             //Absolute Value
        pcmpgtb     mm2,mm0
        pxor        mm0,mm2
        psubb       mm0,mm2
        pcmpgtb     mm0,mm7	            //if (mm0 <= THR1) mm0 = 1 else mm0 = 0
        pandn       mm0,mm6

        //phi(v1 - v2)
        movq        mm2,[esi]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        pand        mm3,mm6
        paddb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             

        //phi(v2 - v3)
        movq        mm1,[esi+eax]
        psubb       mm2,mm1 
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             

        //phi(v3 - v4)
        movq        mm2,[esi+eax*2]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             

        //phi(v4 - v5)
        movq        mm1,[edi+ebx*2]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             

        //phi(v5 - v6)
        movq        mm2,[edi+ebx]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             
        
        //phi(v6 - v7)
        movq        mm1,[edi]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             
        
        //phi(v7 - v8)
        movq        mm2,[edi+eax]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             
        
        //phi(v8 - v9)
        movq        mm1,[edi+eax*2]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             
        
        pcmpgtb     mm0,thr2

        movq        act,mm0

        movq        mm1,mm0
        movq        mm2,mm0
        psrlq       mm2,32
        psrlq       mm1,32
        pand        mm2,mm0
        por         mm1,mm0

        
        movq        mask,mm0

        movd        ebx,mm2     //check for 8 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetMode2

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultMode //Default Mode for all 8

        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetMode //DC Offset Mode for all 4

        mov         eax,iPixelDistance  //Setup pointers
        call        MinMaxMask_MMX
        movq        lminmax,mm6

        movd        mm7,mask           
        call        MixedMode4_MMX

        add         esi,4

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
        
        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetModeA //DC Offset Mode for all 8
            
MixedModeA:

        mov         eax,iPixelDistance
        movq        mm6,lminmax
        movq        mm7,mask           
        psrlq       mm7,32
        call        MixedMode4_MMX 
        
        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DcOffsetMode:    
        mov         eax,iPixelDistance  //Setup pointers
        call        MinMaxMask_MMX

        mov         eax,iPixelDistance
        movq        lminmax,mm6
        call        DcOffsetMode4_MMX

        add         esi,4

        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
       
        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jnz         MixedModeA
            
DcOffsetModeA:    
        mov         eax,iPixelDistance
        movq        mm6,lminmax
        call        DcOffsetMode4_MMX
        
        
        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DcOffsetMode2:    

        call        MinMaxMask_MMX
        movq        lminmax,mm6
  
            
        mov         eax,iPixelDistance
        call        DcOffsetMode4_MMX
        
        add         esi,4

        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        mov         eax,iPixelDistance
        movq        mm6,lminmax
        call        DcOffsetMode4_MMX
        

        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DefaultMode:
        mov         eax,iPixelDistance
        call        DefaultMode4_MMX

        add         esi,4

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
    
        movd        mytemp,mm0

        mov         eax,iPixelDistance  //Setup pointers
        call        MinMaxMask_MMX
        movq        lminmax,mm6


        mov         ebx,mytemp    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetModeA //DC Offset Mode for all 8
        
        jmp         MixedModeA

DefaultModeA:

        mov         eax,iPixelDistance
        call        DefaultMode4_MMX

        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop

end:    emms
        add esp, 64 // IW
        }

        return;

    _asm{        
MinMaxMask_MMX:
        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80

        movq        mm0,[esi+ebx]       //Load first line
        pxor        mm0,mm7
        movq        mm1,[esi]           //Load second line
        pxor        mm1,mm7
        movq        mm2,mm1             //Min Max first 2 lines
        pcmpgtb     mm2,mm0
        movq        mm4,mm0
        pand        mm0,mm2         
        pxor        mm4,mm0
        pandn       mm2,mm1
        pxor        mm1,mm2
        por         mm0,mm2
        por         mm1,mm4

        movq        mm2,[esi+eax]       //Load third line
        pxor        mm2,mm7
        movq        mm3,[esi+eax*2]     //Load forth line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min first 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max first 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        movq        mm2,[edi+ebx*2]     //Load fifth line
        pxor        mm2,mm7
        movq        mm3,[edi+ebx]       //Load sixth line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min last 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max last 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        movq        mm2,[edi]           //Load 7'th line
        pxor        mm2,mm7
        movq        mm3,[edi+eax]       //Load 8'th line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min last 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max last 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        pxor        mm0,mm7
        pxor        mm1,mm7

        psubb       mm1,mm0

        movq        mm0,i2StepBroad

        pxor        mm0,mm7
        pxor        mm1,mm7
        pcmpgtb     mm0,mm1             // (max-min) < 2*iStepSize

        movq        mm6,mm0

        ret
        }

    _asm{        
DcOffsetMode4_MMX:
        movq        lminmax2,mm6

        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80
        movd        mm1,[esi+ebx*2]           //calculate value that will be used as v0
        movd        mm0,[esi+ebx]
        movq        mm2,mm1
        movq        mm3,mm0
        psubusb     mm2,mm0
        psubusb     mm3,mm1
        por         mm2,mm3
        movq        mm5,iStepBroad
        pxor        mm2,mm7
        pxor        mm5,mm7
        pcmpgtb     mm2,mm5
        pand        mm0,mm2
        pandn       mm2,mm1
        por         mm0,mm2             //if (abs(v1-v0) < iStepSize) mm0 = v0 else mm0 = v1

        pxor        mm7,mm7
        
        movq        mm2,x06             //6*v0
        punpcklbw   mm0,mm7             
        movq        mm5,mm0
        movq        mm4,mm0
        psllw       mm5,2
        pmullw      mm0,mm2
                                        
        movd        mm2,[esi+ebx]       //v1
        punpcklbw   mm2,mm7
        movq        mm3,mm2
        paddw       mm5,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm6,mm4
        psllw       mm2,2
        paddw       mm0,mm2

        movd        mm2,[esi]           //v2 
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm0,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2

        movd        mm2,[esi+eax]       //v3
        punpcklbw   mm2,mm7
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2

        movd        mm2,[esi+eax*2]     //v4
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2
        paddw       mm6,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2

        movd        mm2,[edi+ebx*2]     //v5
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm1,x08
        paddw       mm4,mm4
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm3,mm2

        paddw       mm0,mm1
        psrlw       mm0,4

        packuswb    mm0,mm0

        movq        mm7,lminmax2          //Output masked  v1
        movd        mm2,[esi+ebx]
        pand        mm0,mm7
        pandn       mm7,mm2
        por         mm0,mm7
        movd        [esi+ebx],mm0    
        pxor        mm7,mm7

        movd        mm2,[edi+ebx]       //+ v6
        punpcklbw   mm2,mm7
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        movq        mm0,mm2

        paddw       mm5,mm1
        psrlw       mm5,4

        packuswb    mm5,mm5

        movq        mm7,lminmax2          //Output masked  v2
        movd        mm2,[esi]
        pand        mm5,mm7
        pandn       mm7,mm2
        por         mm5,mm7
        movd        [esp+8],mm5     
        pxor        mm7,mm7

        movd        mm2,[edi]           //+v7     
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm2,mm2
        paddw       mm3,mm2

        movq        mm5,mm2

        paddw       mm4,mm1
        psrlw       mm4,4

        packuswb    mm4,mm4
        
        movq        mm7,lminmax2          //Output masked  v3
        movd        mm2,[esi+eax]
        pand        mm4,mm7
        pandn       mm7,mm2
        por         mm4,mm7
        movd        [esp+12],mm4     
  
        movq        mm7,mm0
        paddw       mm0,mm5
        movq        mm4,mm0
        paddw       mm0,mm7
        paddw       mm5,mm5
        paddw       mm5,mm7
        pxor        mm7,mm7
     
        movd        mm2,[edi+eax]       //v8
        punpcklbw   mm2,mm7
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        movq        mm7,x08
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm2

        paddw       mm6,mm7
        psrlw       mm6,4

        packuswb    mm6,mm6
        
        movq        mm7,lminmax2          //Output masked   v4
        movd        mm2,[esi+eax*2]
        pand        mm6,mm7
        pandn       mm7,mm2
        por         mm6,mm7
        movd        [esp+16],mm6     
        movq        mm6,mm0

        movq        mm7,mm3                   //calculate value that will be used as v9
        movd        mm1,[edi+eax*2]           
        movd        mm0,[edi+eax]
        movq        mm2,mm0
        movq        mm3,mm1
        psubusb     mm2,mm1
        psubusb     mm3,mm0
        por         mm2,mm3
        movq        mm3,mm7
        movq        mm7,iStepBroad
        pxor        mm7,x80
        pxor        mm2,x80
        pcmpgtb     mm2,mm7
        pand        mm0,mm2
        pandn       mm2,mm1
        por         mm0,mm2             //if (abs(v8-v9) < iStepSize) mm0 = v9 else mm0 = v8
        movq        mm1,mm3
        pxor        mm7,mm7

        movq        mm3,mm0             //v9
        punpcklbw   mm3,mm7
        paddw       mm1,mm3
        paddw       mm3,mm3
        paddw       mm6,mm3
        paddw       mm5,mm3
        paddw       mm5,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3


        movd        mm0,[esi]           //v2
        punpcklbw   mm0,mm7
        paddw       mm6,mm0
        movd        mm0,[esp+8] 
        movd        [esi],mm0    

        movd        mm0,[esi+eax]       //v3
        punpcklbw   mm0,mm7
        paddw       mm6,mm0
        paddw       mm5,mm0
        movd        mm0,[esp+12] 
        movd        [esi+eax],mm0    

        movd        mm0,[esi+eax*2]     //v4
        punpcklbw   mm0,mm7
        paddw       mm5,mm0
        paddw       mm4,mm0
        paddw       mm0,mm0
        paddw       mm6,mm0
        movd        mm0,[esp+16] 
        movd        [esi+eax*2],mm0    
        
        movd        mm0,[edi+ebx*2]     //v5
        punpcklbw   mm0,mm7
        paddw       mm4,mm0
        paddw       mm0,mm0
        movq        mm2,x08
        paddw       mm6,mm0
        paddw       mm5,mm0
        
        paddw       mm1,mm2
        psrlw       mm1,4

        packuswb    mm1,mm1

        movq        mm7,lminmax2          //Output masked v5
        movq        mm3,mm7
        movd        mm0,[edi+ebx*2]
        pand        mm1,mm3
        pandn       mm3,mm0
        por         mm1,mm3
        movd        [edi+ebx*2],mm1

        paddw       mm6,mm2
        psrlw       mm6,4

        packuswb    mm6,mm6

        movq        mm3,mm7             //Output masked v6
        movd        mm1,[edi+ebx]
        pand        mm6,mm3
        pandn       mm3,mm1
        por         mm6,mm3
        movd        [edi+ebx],mm6    

        paddw       mm5,mm2
        psrlw       mm5,4

        packuswb    mm5,mm5

        movq        mm3,mm7             //Output masked v7
        movd        mm1,[edi]
        pand        mm5,mm3
        pandn       mm3,mm1
        por         mm5,mm3
        movd        [edi],mm5    
        
        paddw       mm4,mm2
        psrlw       mm4,4

        packuswb    mm4,mm4
        
        movd        mm1,[edi+eax]        //Output masked v8
        pand        mm4,mm7
        pandn       mm7,mm1
        por         mm4,mm7
        movd        [edi+eax],mm4   

        ret
    }                           

    _asm{
DefaultMode4_MMX:
        mov         ebx,eax
        neg         ebx
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

        ret
   }


    _asm{        

MixedMode4_MMX:

        movq        [esp+40],mm7 
        pand        mm6,mm7
        movq        lminmax2,mm6

        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80
        movd        mm0,[esi+ebx*2]           //calculate value that will be used as v0
        movd        mm1,[esi+ebx]
        movq        mm2,mm0
        movq        mm3,mm1
        psubusb     mm2,mm1
        psubusb     mm3,mm0
        por         mm2,mm3
        movq        mm5,iStepBroad
        pxor        mm2,mm7
        pxor        mm5,mm7
        pcmpgtb     mm2,mm5
        pand        mm1,mm2
        pandn       mm2,mm0
        por         mm1,mm2             //if (abs(v1-v0) < iStepSize) mm1 = v0 else mm1 = v1

        movq        mm0,mm1

        pxor        mm7,mm7

        movq        mm2,x06             //6*v0
        punpcklbw   mm0,mm7             
        movq        mm5,mm0
        movq        mm4,mm0
        psllw       mm5,2
        pmullw      mm0,mm2
                                        
        movd        mm2,[esi+ebx]       //v1
        punpcklbw   mm2,mm7
        movq        mm3,mm2
        paddw       mm5,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm6,mm4
        psllw       mm2,2
        paddw       mm0,mm2

        movd        mm2,[esi]           //v2 
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm0,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2

        movd        mm2,[esi+eax]       //v3
        punpcklbw   mm2,mm7
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2


        movd        mm2,[esi+eax*2]     //v4
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2
        paddw       mm6,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2

        movd        mm2,[edi+ebx*2]     //v5
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm4
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm3,mm2

        paddw       mm0,x08
        psrlw       mm0,4
        packuswb    mm0,mm0

        movq        mm7,lminmax2          //Output masked  v1
        movd        mm2,[esi+ebx]
        pand        mm0,mm7
        pandn       mm7,mm2
        por         mm0,mm7
        movd        [esp+4],mm0 
        pxor        mm7,mm7


        movd        mm2,[edi+ebx]         //+ v6
        punpcklbw   mm2,mm7
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        movq        mm0,mm2

        paddw       mm5,x08
        psrlw       mm5,4
        packuswb    mm5,mm5

        movq        mm7,lminmax2          //Output masked  v2
        movd        mm2,[esi]
        pand        mm5,mm7
        pandn       mm7,mm2
        por         mm5,mm7
        movd        [esp+8],mm5     
        pxor        mm7,mm7


        movd        mm2,[edi]           //+v7     
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm2,mm2
        paddw       mm3,mm2

        movq        mm5,mm2

        paddw       mm4,x08
        psrlw       mm4,4
        packuswb    mm4,mm4
        
        movq        mm7,lminmax2          //Output masked  v3
        movd        mm2,[esi+eax]
        pand        mm4,mm7
        pandn       mm7,mm2
        por         mm4,mm7
        movd        [esp+12],mm4     
  
        movq        mm7,mm0
        paddw       mm0,mm5
        movq        mm4,mm0
        paddw       mm0,mm7
        paddw       mm5,mm5
        paddw       mm5,mm7
        pxor        mm7,mm7
    
        movd        mm2,[edi+eax]      //v8
        punpcklbw   mm2,mm7
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm2

        paddw       mm6,x08
        psrlw       mm6,4
        packuswb    mm6,mm6
        
        movq        mm7,lminmax2          //Output masked   v4
        movd        mm2,[esi+eax*2]
        pand        mm6,mm7
        pandn       mm7,mm2
        por         mm6,mm7
        movd        [esp+16],mm6     
        movq        mm6,mm0

        movq        mm7,mm3             //calculate value that will be used as v9
        movd        mm1,[edi+eax*2]           
        movd        mm0,[edi+eax]
        movq        mm2,mm0
        movq        mm3,mm1
        psubusb     mm2,mm1
        psubusb     mm3,mm0
        por         mm2,mm3
        movq        mm3,mm7
        movq        mm7,iStepBroad
        pxor        mm7,x80
        pxor        mm2,x80
        pcmpgtb     mm2,mm7
        pand        mm0,mm2
        pandn       mm2,mm1
        por         mm0,mm2             //if (abs(v8-v9) < iStepSize) mm0 = v9 else mm0 = v8
        movq        mm1,mm3
        pxor        mm7,mm7

        movq        mm3,mm0             //v9
        punpcklbw   mm3,mm7
        paddw       mm1,mm3
        paddw       mm3,mm3
        paddw       mm6,mm3
        movq        mm2,x08
        paddw       mm5,mm3
        paddw       mm5,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3

        paddw       mm1,mm2
        psrlw       mm1,4
        packuswb    mm1,mm1

        movq        mm3,lminmax2          //Output masked   v5
        movd        mm0,[edi+ebx*2]
        pand        mm1,mm3
        pandn       mm3,mm0
        por         mm1,mm3
        movd        [esp+20],mm1 

        movd        mm1,[esi]           //v2
        punpcklbw   mm1,mm7
        paddw       mm6,mm1

        movd        mm1,[esi+eax]       //v3
        punpcklbw   mm1,mm7
        paddw       mm6,mm1
        paddw       mm5,mm1

        movd        mm1,[esi+eax*2]     //v4
        punpcklbw   mm1,mm7
        paddw       mm5,mm1
        paddw       mm4,mm1
        paddw       mm1,mm1
        paddw       mm6,mm1
        
        movd        mm1,[edi+ebx*2]     //v5
        punpcklbw   mm1,mm7
        paddw       mm4,mm1
        paddw       mm1,mm1
        paddw       mm6,mm1
        paddw       mm5,mm1
        
        paddw       mm6,mm2
        psrlw       mm6,4
        packuswb    mm6,mm6

        movq        mm3,lminmax2          //Output masked v6
        movd        mm1,[edi+ebx]
        pand        mm6,mm3
        pandn       mm3,mm1
        por         mm6,mm3
        movd        [esp+24],mm6 

        paddw       mm5,mm2
        psrlw       mm5,4
        packuswb    mm5,mm5

        movq        mm3,lminmax2          //Output masked v7
        movd        mm0,[edi]
        pand        mm5,mm3
        pandn       mm3,mm0
        por         mm5,mm3
        movd        [esp+28],mm5 

        paddw       mm4,mm2
        psrlw       mm4,4
        packuswb    mm4,mm4
        
        movq        mm3,lminmax2          //Output masked  v8
        movd        mm0,[edi+eax]
        pand        mm4,mm3
        pandn       mm3,mm0
        por         mm4,mm3
        
        movd        [esp+32],mm4    

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

        movd        mm2,[esp+40] 
        movq        mm3,mm2

        packuswb    mm4,mm4
        pandn       mm2,mm4
        
        packuswb    mm7,mm7
        pandn       mm3,mm7
        
        movd        mm0,[esp+4]            
        movd        [esi+ebx],mm0           //v1

        movd        mm0,[esp+8] 
        movd        [esi],mm0               //v2

        movd        mm0,[esp+12] 
        movd        [esi+eax],mm0           //v3

        movd        mm0,[esp+16] 
        movd        mm1,[esp+40] 
        pand        mm1,mm0
        por         mm1,mm2
        movd        [esi+eax*2],mm1         //v4

        movd        mm0,[esp+20] 
        movd        mm1,[esp+40] 
        pand        mm1,mm0
        por         mm1,mm3
        movd        [edi+ebx*2],mm1         //v5

        movd        mm0,[esp+24]            
        movd        [edi+ebx],mm0           //v6

        movd        mm0,[esp+28]            
        movd        [edi],mm0               //v7
        
        movd        mm0,[esp+32]            
        movd        [edi+eax],mm0           //v8
        ret
  
    } 
}      


Void_WMV FilterHalfHorizantalEdge(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{
 
    static const Int64 thr1 = THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56);
    //static const Int64 neg_thr1 = NTHR1 | (NTHR1 << 8) | (NTHR1 << 16) | (NTHR1 << 24) | ((Int64)NTHR1 << 32) | ((Int64)NTHR1 << 40) | ((Int64)NTHR1 << 48) | ((Int64)NTHR1 << 56);
    
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
    static const Int64 x80 = 0x8080808080808080;
    static const Int64 x06 = 0x0006000600060006;
    static const Int64 x08 = 0x0008000800080008;
    static const Int64 x05 = 0x0005000500050005;
    static const Int64 x04 = 0x0004000400040004;
    static const Int64 x0001 = 0x0001000100010001;
    static const Int64 x0002 = 0x0002000200020002;

    Int64 act;
    Int64 iStepBroad, i2StepBroad;
    Int64 lminmax;
    Int64 lminmax2;
    Int64 mask;
    I32_WMV mytemp;    
    I32_WMV i;
    U8_WMV* pV0 = ppxlcCenter - (iPixelDistance*5);
    
    
            _asm {

        sub esp, 64  // IW

        movd        mm0,iStepSize       //Broadcast iStepSize
        movq        mm5,mm0
        psllq       mm5,8
        por         mm0,mm5
        movq        mm5,mm0
        psllq       mm5,16
        por         mm0,mm5
        movq        mm5,mm0
        psllq       mm5,32
        por         mm0,mm5
        movq        mm5,mm0
        psllq       mm0,1
        psubb       mm5,x01 
        movq        iStepBroad,mm5
        movq        i2StepBroad,mm0
        
        mov         eax,iPixelDistance
        mov         esi,pV0
        lea         esi,[esi+eax*2]

        mov         ecx,iEdgeLength
        shr         ecx,3
HLoop:
        mov         i,ecx

        mov         eax,iPixelDistance
        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,thr1
        movq        mm6,x01

        //phi(v0 - v1)
        movq        mm0,[esi+ebx*2]     //Load
        movq        mm1,[esi+ebx]
        psubb       mm0,mm1             //Subtract
        pxor        mm2,mm2             //Absolute Value
        pcmpgtb     mm2,mm0
        pxor        mm0,mm2
        psubb       mm0,mm2
        pcmpgtb     mm0,mm7	            //if (mm0 <= THR1) mm0 = 1 else mm0 = 0
        pandn       mm0,mm6

        //phi(v1 - v2)
        movq        mm2,[esi]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        pand        mm3,mm6
        paddb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             

        //phi(v2 - v3)
        movq        mm1,[esi+eax]
        psubb       mm2,mm1 
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             

        //phi(v3 - v4)
        movq        mm2,[esi+eax*2]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             

        //phi(v4 - v5)
        movq        mm1,[edi+ebx*2]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             

        //phi(v5 - v6)
        movq        mm2,[edi+ebx]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             
        
        //phi(v6 - v7)
        movq        mm1,[edi]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             
        
        //phi(v7 - v8)
        movq        mm2,[edi+eax]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             
        
        //phi(v8 - v9)
        movq        mm1,[edi+eax*2]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             
        
        pcmpgtb     mm0,thr2

        movq        act,mm0

        movq        mm1,mm0
        movq        mm2,mm0
        psrlq       mm2,32
        psrlq       mm1,32
        pand        mm2,mm0
        por         mm1,mm0

        
        movq        mask,mm0

        movd        ebx,mm2     //check for 8 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetMode2

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultMode //Default Mode for all 8

        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetMode //DC Offset Mode for all 4

        mov         eax,iPixelDistance  //Setup pointers
        call        HalfMinMaxMask
        movq        lminmax,mm6

        movd        mm7,mask           
        call        HalfMixedMode4 

        add         esi,4

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
        
        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetModeA //DC Offset Mode for all 8
            
MixedModeA:

        mov         eax,iPixelDistance
        movq        mm6,lminmax
        movq        mm7,mask           
        psrlq       mm7,32
        call        HalfMixedMode4 
        
        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DcOffsetMode:    
        mov         eax,iPixelDistance  //Setup pointers
        call        HalfMinMaxMask

        mov         eax,iPixelDistance
        movq        lminmax,mm6
        call        HalfDcOffsetMode4

        add         esi,4

        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
       
        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jnz         MixedModeA
            
DcOffsetModeA:    
        mov         eax,iPixelDistance
        movq        mm6,lminmax
        call        HalfDcOffsetMode4
        
        
        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DcOffsetMode2:    

        call        HalfMinMaxMask
        movq        lminmax,mm6
  
            
        mov         eax,iPixelDistance
        call        HalfDcOffsetMode4
        
        add         esi,4

        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        mov         eax,iPixelDistance
        movq        mm6,lminmax
        call        HalfDcOffsetMode4
        

        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DefaultMode:
        mov         eax,iPixelDistance
        call        HalfDefaultMode4

        add         esi,4

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
    
        movd        mytemp,mm0

        mov         eax,iPixelDistance  //Setup pointers
        call        HalfMinMaxMask
        movq        lminmax,mm6


        mov         ebx,mytemp    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetModeA //DC Offset Mode for all 8
        
        jmp         MixedModeA

DefaultModeA:

        mov         eax,iPixelDistance
        call        HalfDefaultMode4

        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop

end:    emms
        add esp, 64  // IW
        }
        return;
    _asm{
HalfDefaultMode4:

        mov         ebx,eax
        neg         ebx
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
        ret
   }

    _asm{     
HalfMinMaxMask:  // same as MinMaxMask      
        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80

        movq        mm0,[esi+ebx]       //Load first line
        pxor        mm0,mm7
        movq        mm1,[esi]           //Load second line
        pxor        mm1,mm7
        movq        mm2,mm1             //Min Max first 2 lines
        pcmpgtb     mm2,mm0
        movq        mm4,mm0
        pand        mm0,mm2         
        pxor        mm4,mm0
        pandn       mm2,mm1
        pxor        mm1,mm2
        por         mm0,mm2
        por         mm1,mm4

        movq        mm2,[esi+eax]       //Load third line
        pxor        mm2,mm7
        movq        mm3,[esi+eax*2]     //Load forth line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min first 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max first 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        movq        mm2,[edi+ebx*2]     //Load fifth line
        pxor        mm2,mm7
        movq        mm3,[edi+ebx]       //Load sixth line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min last 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max last 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        movq        mm2,[edi]           //Load 7'th line
        pxor        mm2,mm7
        movq        mm3,[edi+eax]       //Load 8'th line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min last 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max last 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        pxor        mm0,mm7
        pxor        mm1,mm7

        psubb       mm1,mm0

        movq        mm0,i2StepBroad

        pxor        mm0,mm7
        pxor        mm1,mm7
        pcmpgtb     mm0,mm1             // (max-min) < 2*iStepSize

        movq        mm6,mm0
        ret
        }

    _asm{      
HalfDcOffsetMode4:        
        movq        lminmax2,mm6

        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80
        movd        mm1,[esi+ebx*2]           //calculate value that will be used as v0
        movd        mm0,[esi+ebx]
        movq        mm2,mm1
        movq        mm3,mm0
        psubusb     mm2,mm0
        psubusb     mm3,mm1
        por         mm2,mm3
        movq        mm5,iStepBroad
        pxor        mm2,mm7
        pxor        mm5,mm7
        pcmpgtb     mm2,mm5
        pand        mm0,mm2
        pandn       mm2,mm1
        por         mm0,mm2             //if (abs(v1-v0) < iStepSize) mm0 = v0 else mm0 = v1

        pxor        mm7,mm7
        
        movq        mm2,x06             //6*v0
        punpcklbw   mm0,mm7             
        movq        mm5,mm0
        movq        mm4,mm0
        psllw       mm5,2
        pmullw      mm0,mm2
                                        
        movd        mm2,[esi+ebx]       //v1
        punpcklbw   mm2,mm7
        movq        mm3,mm2
        paddw       mm5,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm6,mm4
        psllw       mm2,2
        paddw       mm0,mm2

        movd        mm2,[esi]           //v2 
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm0,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2

        movd        mm2,[esi+eax]       //v3
        punpcklbw   mm2,mm7
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2


        movd        mm2,[esi+eax*2]     //v4
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2
        paddw       mm6,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2

        movd        mm2,[edi+ebx*2]     //v5
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm1,x08
        paddw       mm4,mm4
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm3,mm2

        paddw       mm0,mm1
        psrlw       mm0,4

        packuswb    mm0,mm0

        movq        mm7,lminmax2          //Output masked  v1
        movd        mm2,[esi+ebx]
        pand        mm0,mm7
        pandn       mm7,mm2
        por         mm0,mm7
        movd        [esi+ebx],mm0    
        pxor        mm7,mm7

        movd        mm2,[edi+ebx]       //+ v6
        punpcklbw   mm2,mm7
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        movq        mm0,mm2

        paddw       mm5,mm1
        psrlw       mm5,4

        packuswb    mm5,mm5

        movq        mm7,lminmax2          //Output masked  v2
        movd        mm2,[esi]
        pand        mm5,mm7
        pandn       mm7,mm2
        por         mm5,mm7
        movd        [esi],mm5    

        pxor        mm7,mm7


        movd        mm2,[edi]           //+v7     
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm2,mm2
        paddw       mm3,mm2

        movq        mm5,mm2

        paddw       mm4,mm1
        psrlw       mm4,4

        packuswb    mm4,mm4
        
        movq        mm7,lminmax2          //Output masked  v3
        movd        mm2,[esi+eax]
        pand        mm4,mm7
        pandn       mm7,mm2
        por         mm4,mm7
        movd        [esi+eax],mm4    
  
        movq        mm7,mm0
        paddw       mm0,mm5
        movq        mm4,mm0
        paddw       mm0,mm7
        paddw       mm5,mm5
        paddw       mm5,mm7
        pxor        mm7,mm7
     
        movd        mm2,[edi+eax]       //v8
        punpcklbw   mm2,mm7
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        movq        mm7,x08
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm2

        paddw       mm6,mm7
        psrlw       mm6,4

        packuswb    mm6,mm6
        
        movq        mm7,lminmax2          //Output masked   v4
        movd        mm2,[esi+eax*2]
        pand        mm6,mm7
        pandn       mm7,mm2
        por         mm6,mm7
        movd        [esi+eax*2],mm6    
        ret
    }                           

    _asm{ 
        
HalfMixedMode4:
        movq        [esp+40],mm7
        pand        mm6,mm7
        movq        lminmax2,mm6

        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80
        movd        mm0,[esi+ebx*2]           //calculate value that will be used as v0
        movd        mm1,[esi+ebx]
        movq        mm2,mm0
        movq        mm3,mm1
        psubusb     mm2,mm1
        psubusb     mm3,mm0
        por         mm2,mm3
        movq        mm5,iStepBroad
        pxor        mm2,mm7
        pxor        mm5,mm7
        pcmpgtb     mm2,mm5
        pand        mm1,mm2
        pandn       mm2,mm0
        por         mm1,mm2             //if (abs(v1-v0) < iStepSize) mm1 = v0 else mm1 = v1

        movq        mm0,mm1

        pxor        mm7,mm7

        movq        mm2,x06             //6*v0
        punpcklbw   mm0,mm7             
        movq        mm5,mm0
        movq        mm4,mm0
        psllw       mm5,2
        pmullw      mm0,mm2
                                        
        movd        mm2,[esi+ebx]       //v1
        punpcklbw   mm2,mm7
        movq        mm3,mm2
        paddw       mm5,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm6,mm4
        psllw       mm2,2
        paddw       mm0,mm2

        movd        mm2,[esi]           //v2 
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm0,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2

        movd        mm2,[esi+eax]       //v3
        punpcklbw   mm2,mm7
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2


        movd        mm2,[esi+eax*2]     //v4
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2
        paddw       mm6,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2

        movd        mm2,[edi+ebx*2]     //v5
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm4
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm3,mm2

        paddw       mm0,x08
        psrlw       mm0,4
        packuswb    mm0,mm0

        movq        mm7,lminmax2          //Output masked  v1
        movd        mm2,[esi+ebx]
        pand        mm0,mm7
        pandn       mm7,mm2
        por         mm0,mm7
        movd        [esp+4],mm0
        pxor        mm7,mm7


        movd        mm2,[edi+ebx]         //+ v6
        punpcklbw   mm2,mm7
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        movq        mm0,mm2

        paddw       mm5,x08
        psrlw       mm5,4
        packuswb    mm5,mm5

        movq        mm7,lminmax2          //Output masked  v2
        movd        mm2,[esi]
        pand        mm5,mm7
        pandn       mm7,mm2
        por         mm5,mm7
        movd        [esp+8],mm5    
        pxor        mm7,mm7


        movd        mm2,[edi]           //+v7     
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm2,mm2
        paddw       mm3,mm2

        movq        mm5,mm2

        paddw       mm4,x08
        psrlw       mm4,4
        packuswb    mm4,mm4
        
        movq        mm7,lminmax2          //Output masked  v3
        movd        mm2,[esi+eax]
        pand        mm4,mm7
        pandn       mm7,mm2
        por         mm4,mm7
        movd        [esp+12],mm4    
  
        movq        mm7,mm0
        paddw       mm0,mm5
        movq        mm4,mm0
        paddw       mm0,mm7
        paddw       mm5,mm5
        paddw       mm5,mm7
        pxor        mm7,mm7
    
        movd        mm2,[edi+eax]      //v8
        punpcklbw   mm2,mm7
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm2

        paddw       mm6,x08
        psrlw       mm6,4
        packuswb    mm6,mm6
        
        movq        mm7,lminmax2          //Output masked   v4
        movd        mm2,[esi+eax*2]
        pand        mm6,mm7
        pandn       mm7,mm2
        por         mm6,mm7
        movd        [esp+16],mm6    
        movq        mm6,mm0

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
        /*
        movq        mm6,mm5
        psllq       mm5,16
        por         mm6,mm5
        movq        mm5,mm6
        psllq       mm5,32
        por         mm5,mm6
          */
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

        movd        mm2,[esp+40]
        movq        mm3,mm2

        packuswb    mm4,mm4
        pandn       mm2,mm4
        
        packuswb    mm7,mm7
        pandn       mm3,mm7
        
        movd        mm0,[esp+4]           
        movd        [esi+ebx],mm0           //v1

        movd        mm0,[esp+8]
        movd        [esi],mm0               //v2

        movd        mm0,[esp+12]
        movd        [esi+eax],mm0           //v3

        movd        mm0,[esp+16]
        movd        mm1,[esp+40]
        pand        mm1,mm0
        por         mm1,mm2
        movd        [esi+eax*2],mm1         //v4

        ret
  
    }                           


} 

#undef USE_SPEEDUP
I8_WMV negtab[100] = {-1,-2,-3,-4,-5,-6,-6,-5,-4,-3,-2,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
I8_WMV g_deblockTab[100] = {0,1,2,3,4,5,6,6,5,4,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
Void_WMV FilterVerticalEdge(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    static const Int64 x80 = 0x8080808080808080;
    static const Int64 thr1 = (THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56)) ^ x80;
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
  
    static const Int64 xFFFFFFFF00000000 = 0xFFFFFFFF00000000;
    static const Int64 x000000000000FFFF = 0x000000000000FFFF;
    static const Int64 xFFFF000000000000 = 0xFFFF000000000000;

    static const Int64 x0008 = 0x0008000800080008;
    static const Int64 s1 = 0xFFFB0002FFFB0002;
    static const Int64 s2 = 0xFFFE0005FFFE0005;
    static const Int64 s3 = 0xFFFE0005FFFB0002;
    static const Int64 x00000004 = 0x0000000400000004;       
    static const Int64 x00000005 = 0x0000000500000005;

    //For Long Filter
    static const Int64 m2226 = 0x0006000200020002;
    static const Int64 m2240 = 0x0000000400020002;
    static const Int64 m1221 = 0x0001000200020001;
    static const Int64 m6222 = 0x0002000200020006;
    static const Int64 m0422 = 0x0002000200040000;
    
    

       _asm {
            mov         esi,ppxlcCenter
            sub         esi,5
            mov         edi,iEdgeLength
MainLoop:
            
            //Determine Which Filter To Use Long or short    

#ifdef USE_SPEEDUP
            movzx       ebx,byte Ptr [esi+4]
            movzx       ecx,byte Ptr [esi+5]
            cmp         ebx,ecx
            jz          EndOfLoop
#endif
            movq        mm0,[esi]
            movq        mm1,[esi+1]

            movq        mm2,mm0
            psubusb     mm2,mm1
            psubusb     mm1,mm0
            por         mm2,mm1
            
            movq        mm3,x80
            movq        mm4,thr1
            pxor        mm2,mm3

            pcmpgtb     mm2,mm4
            pandn       mm2,x01
            movq        mm1,mm2
            psrlq       mm2,32
            paddb       mm2,mm1
            movq        mm1,mm2
            psrld       mm2,16
            paddb       mm2,mm1
            movq        mm1,mm2
            psrlw       mm2,8
            paddb       mm2,mm1

#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       ebx,byte Ptr [esi+9]
            sub         eax,ebx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         ebx,eax
            sub         ebx,(THR1+1)
            sar         ebx,31
#endif

            movd        eax,mm2
            movzx       eax,al
#ifndef USE_SPEEDUP
            sub         eax,ebx
#endif
            cmp         eax,THR2
            jl          ShortFilter
  
            //Determine MIN & MAX values to see if filtering should be used
            
#ifndef USE_SPEEDUP
            movq        mm0,[esi+1]
            movq        mm4,x80
            pxor        mm0,mm4

            movq        mm1,mm0
            punpckldq   mm0,mm0
            punpckhdq   mm1,mm1

            movq        mm3,xFFFFFFFF00000000

            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrld       mm1,16
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrlw       mm1,8
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2
            pxor        mm0,mm4
            
            movq        mm2,mm0
            psrlq       mm2,32
            psubb       mm0,mm2
            
            movd        eax,mm0
            movzx       eax,al //max-min
            mov         edx,iStepSize
            shl         edx,1

            cmp         eax,edx
            jge         EndOfLoop
#else
            movzx       eax,byte Ptr [esi]      //Can probobly skip these lines if the speedup is necessary
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            shl         edx,1
            cmp         eax,edx
            jg          EndOfLoop
#endif

  
            pxor        mm7,mm7
  
            movq        mm5,[esi+1]
            movq        mm3,mm5
            punpcklbw   mm3,mm7
            movq        mm4,mm3
            paddw       mm3,mm3

            movq        mm2,mm5
            psrlq       mm2,8
            paddw       mm3,mm4
            movq        mm6,mm5
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm2

            psrlq       mm6,16
            movq        mm2,mm6
            punpcklbw   mm6,mm7
            paddw       mm6,mm6
                      
            psrlq       mm2,8
            paddw       mm4,mm6
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm4
            paddw       mm4,mm2

            movq        mm0,m6222
            punpckhbw   mm5,mm7
            paddw       mm3,mm5
            paddw       mm5,mm5                           
            paddw       mm4,mm5                           
            paddw       mm4,mm5

            movd        mm1,[esi]
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi]
            movzx       edx,byte Ptr [esi+1]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV0
            movq        mm5,x000000000000FFFF
            psrlq       mm2,16
            pand        mm2,mm5
            pandn       mm5,mm1
            por         mm2,mm5
            movq        mm1,mm2
UseV0:
#endif
            movq        mm5,m0422
            pmullw      mm1,mm0
            paddw       mm3,mm1
            
            movq        mm0,m1221
            psllq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm3,mm2

            movq        mm5,m2226
            psllq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm3,mm1

            psllq       mm2,16
            paddw       mm3,mm2

            movd        mm1,[esi+6]                
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV9
            movq        mm0,xFFFF000000000000
            psllq       mm2,16
            pand        mm2,mm0
            pandn       mm0,mm1
            por         mm2,mm0
            movq        mm1,mm2
            movq        mm0,m1221
UseV9:
#endif
            pmullw      mm1,mm5
            paddw       mm4,mm1

            movq        mm5,m2240
            psrlq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm4,mm2

            psrlq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm4,mm1

            movq        mm0,x0008
            psrlq       mm2,16
            paddw       mm4,mm2

            paddw       mm3,mm0
            psrlw       mm3,4
            paddw       mm4,mm0
            psrlw       mm4,4
            packuswb    mm3,mm4
            movq        [esi+1],mm3

            jmp         EndOfLoop

ShortFilter:
#ifdef USE_SPEEDUP
            movd        mm0,ecx
            movzx       eax,[esi+3]
            movzx       edx,[esi+6]
            sub         ecx,ebx
            shl         ecx,3
            sub         eax,edx
            mov         edx,eax
            shl         eax,1
            add         eax,edx
            add         eax,ecx
            sar         eax,4
            movd        ecx,mm0
            movzx       eax,byte Ptr [g_deblockTab+eax]
            add         ebx,eax
            sub         ecx,eax
            test        ebx,0xFF00
            jnz         Clip2
            mov         [esi+4],bl
Clip2Ret:   test        ecx,0xFF00
            jnz         Clip1
            mov         [esi+5],cl
Clip1Ret:
#else
            pxor        mm7,mm7
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

            test        ecx,0x100
            jnz         Clip1
            mov         [esi+4],cl          
            
Clip1Ret:   test        ebx,0x100
            jnz         Clip2
            mov         [esi+5],bl
Clip2Ret:
#endif
EndOfLoop:                
             mov        eax,iPixelIncrement
             add        esi,eax
             
             dec        edi
             jnz        MainLoop

            }
return;
        _asm {
Clip1:      cmp         ecx,0
            jl          Clip1Down
            mov         [esi+4],0xFF
            jmp         Clip1Ret
Clip1Down:  mov         [esi+4],0
            jmp         Clip1Ret

Clip2:      cmp         ebx,0
            jl          Clip2Down
            mov         [esi+5],0xFF
            jmp         Clip2Ret
Clip2Down:  mov         [esi+5],0
            jmp         Clip2Ret
        }
}


Void_WMV FilterHalfVerticalEdge(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    static const Int64 x80 = 0x8080808080808080;
    static const Int64 thr1 = (THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56)) ^ x80;
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
  
    static const Int64 xFFFFFFFF00000000 = 0xFFFFFFFF00000000;
    static const Int64 x000000000000FFFF = 0x000000000000FFFF;
    static const Int64 xFFFF000000000000 = 0xFFFF000000000000;

    static const Int64 x0008 = 0x0008000800080008;
    static const Int64 s1 = 0xFFFB0002FFFB0002;
    static const Int64 s2 = 0xFFFE0005FFFE0005;
    static const Int64 s3 = 0xFFFE0005FFFB0002;
    static const Int64 x00000004 = 0x0000000400000004;       
    static const Int64 x00000005 = 0x0000000500000005;

    //For Long Filter
    static const Int64 m2226 = 0x0006000200020002;
    static const Int64 m2240 = 0x0000000400020002;
    static const Int64 m1221 = 0x0001000200020001;
    static const Int64 m6222 = 0x0002000200020006;
    static const Int64 m0422 = 0x0002000200040000;
    
    

       _asm {
            mov         esi,ppxlcCenter
            sub         esi,5
            mov         edi,iEdgeLength
MainLoop:
            
            //Determine Which Filter To Use Long or short    

#ifdef USE_SPEEDUP
            movzx       ebx,byte Ptr [esi+4]
            movzx       ecx,byte Ptr [esi+5]
            cmp         ebx,ecx
            jz          EndOfLoop
#endif
            movq        mm0,[esi]
            movq        mm1,[esi+1]

            movq        mm2,mm0
            psubusb     mm2,mm1
            psubusb     mm1,mm0
            por         mm2,mm1
            
            movq        mm3,x80
            movq        mm4,thr1
            pxor        mm2,mm3

            pcmpgtb     mm2,mm4
            pandn       mm2,x01
            movq        mm1,mm2
            psrlq       mm2,32
            paddb       mm2,mm1
            movq        mm1,mm2
            psrld       mm2,16
            paddb       mm2,mm1
            movq        mm1,mm2
            psrlw       mm2,8
            paddb       mm2,mm1

#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       ebx,byte Ptr [esi+9]
            sub         eax,ebx
            cdq
            xor         eax,edx
            sub         eax,edx
            xor         ebx,ebx
            cmp         eax,THR1
            jg          DontAdd1
            mov         ebx,1
DontAdd1:
#endif

            movd        eax,mm2
            movzx       eax,al
#ifndef USE_SPEEDUP
            add         eax,ebx
#endif
            cmp         eax,THR2
            jl          ShortFilter
  
            //Determine MIN & MAX values to see if filtering should be used
            
#ifndef USE_SPEEDUP
            movq        mm0,[esi+1]
            movq        mm4,x80
            pxor        mm0,mm4

            movq        mm1,mm0
            punpckldq   mm0,mm0
            punpckhdq   mm1,mm1

            movq        mm3,xFFFFFFFF00000000

            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrld       mm1,16
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrlw       mm1,8
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2
            pxor        mm0,mm4
            
            movq        mm2,mm0
            psrlq       mm2,32
            psubb       mm0,mm2
            
            movd        eax,mm0
            movzx       eax,al //max-min
            mov         edx,iStepSize
            shl         edx,1

            cmp         eax,edx
            jge         EndOfLoop
#else
            movzx       eax,byte Ptr [esi]
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            shl         edx,1
            cmp         eax,edx
            jg          EndOfLoop
#endif            
  
            pxor        mm7,mm7
  
            movq        mm5,[esi+1]
            movq        mm3,mm5
            punpcklbw   mm3,mm7
            movq        mm4,mm3
            paddw       mm3,mm3

            movq        mm2,mm5
            psrlq       mm2,8
            paddw       mm3,mm4
            movq        mm6,mm5
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm2

            psrlq       mm6,16
            movq        mm2,mm6
            punpcklbw   mm6,mm7
            paddw       mm6,mm6
                      
            psrlq       mm2,8
            paddw       mm4,mm6
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm4
            paddw       mm4,mm2

            movq        mm0,m6222
            punpckhbw   mm5,mm7
            paddw       mm3,mm5
            paddw       mm5,mm5                           
            paddw       mm4,mm5                           
            paddw       mm4,mm5

            movd        mm1,[esi]
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi]
            movzx       edx,byte Ptr [esi+1]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV0
            movq        mm5,x000000000000FFFF
            psrlq       mm2,16
            pand        mm2,mm5
            pandn       mm5,mm1
            por         mm2,mm5
            movq        mm1,mm2
UseV0:
#endif
            movq        mm5,m0422
            pmullw      mm1,mm0
            paddw       mm3,mm1
            
            movq        mm0,m1221
            psllq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm3,mm2

            movq        mm5,m2226
            psllq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm3,mm1

            psllq       mm2,16
            paddw       mm3,mm2

            movd        mm1,[esi+6]                
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV9
            movq        mm0,xFFFF000000000000
            psllq       mm2,16
            pand        mm2,mm0
            pandn       mm0,mm1
            por         mm2,mm0
            movq        mm1,mm2
            movq        mm0,m1221
UseV9:
#endif
            pmullw      mm1,mm5
            paddw       mm4,mm1

            movq        mm5,m2240
            psrlq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm4,mm2

            psrlq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm4,mm1

            movq        mm0,x0008
            psrlq       mm2,16
            paddw       mm4,mm2

            paddw       mm3,mm0
            psrlw       mm3,4
            paddw       mm4,mm0
            psrlw       mm4,4
            packuswb    mm3,mm4
            movd        [esi+1],mm3

            jmp         EndOfLoop

ShortFilter:
#ifdef USE_SPEEDUP
            movd        mm0,ecx
            movzx       eax,[esi+3]
            movzx       edx,[esi+6]
            sub         ecx,ebx
            shl         ecx,3
            sub         eax,edx
            mov         edx,eax
            shl         eax,1
            add         eax,edx
            add         eax,ecx
            sar         eax,4
            movd        ecx,mm0
            movzx       eax,byte Ptr [g_deblockTab+eax]
            add         ebx,eax
            sub         ecx,eax
            test        ebx,0xFF00
            jnz         Clip2
            mov         [esi+4],bl
#else
            pxor        mm7,mm7
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

            test        ecx,0x100
            jnz         Clip1
            mov         [esi+4],cl          
            
Clip1Ret: 
#endif
EndOfLoop:                
             mov        eax,iPixelIncrement
             add        esi,eax
             
             dec        edi
             jnz        MainLoop

            }
return;
#ifdef USE_SPEEDUP
        _asm {
Clip2:      cmp         ebx,0
            jl          Clip2Down
            mov         [esi+5],0xFF
            jmp         EndOfLoop
Clip2Down:  mov         [esi+5],0
            jmp         EndOfLoop
        }
#else
        _asm {
Clip1:      cmp         ecx,0
            jl          Clip1Down
            mov         [esi+4],0xFF
            jmp         Clip1Ret
Clip1Down:  mov         [esi+4],0
            jmp         Clip1Ret

        }
#endif
}


#define USE_SPEEDUP
Void_WMV FilterVerticalEdge_FASTEST(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    static const Int64 x80 = 0x8080808080808080;
    static const Int64 thr1 = (THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56)) ^ x80;
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
  
    static const Int64 xFFFFFFFF00000000 = 0xFFFFFFFF00000000;
    static const Int64 x000000000000FFFF = 0x000000000000FFFF;
    static const Int64 xFFFF000000000000 = 0xFFFF000000000000;

    static const Int64 x0008 = 0x0008000800080008;
    static const Int64 s1 = 0xFFFB0002FFFB0002;
    static const Int64 s2 = 0xFFFE0005FFFE0005;
    static const Int64 s3 = 0xFFFE0005FFFB0002;
    static const Int64 x00000004 = 0x0000000400000004;       
    static const Int64 x00000005 = 0x0000000500000005;

    //For Long Filter
    static const Int64 m2226 = 0x0006000200020002;
    static const Int64 m2240 = 0x0000000400020002;
    static const Int64 m1221 = 0x0001000200020001;
    static const Int64 m6222 = 0x0002000200020006;
    static const Int64 m0422 = 0x0002000200040000;
    
    

       _asm {
            mov         esi,ppxlcCenter
            sub         esi,5
            mov         edi,iEdgeLength
MainLoop:
            
            //Determine Which Filter To Use Long or short    

#ifdef USE_SPEEDUP
            movzx       ebx,byte Ptr [esi+4]
            movzx       ecx,byte Ptr [esi+5]
            cmp         ebx,ecx
            jz          EndOfLoop
#endif
            movq        mm0,[esi]
            movq        mm1,[esi+1]

            movq        mm2,mm0
            psubusb     mm2,mm1
            psubusb     mm1,mm0
            por         mm2,mm1
            
            movq        mm3,x80
            movq        mm4,thr1
            pxor        mm2,mm3

            pcmpgtb     mm2,mm4
            pandn       mm2,x01
            movq        mm1,mm2
            psrlq       mm2,32
            paddb       mm2,mm1
            movq        mm1,mm2
            psrld       mm2,16
            paddb       mm2,mm1
            movq        mm1,mm2
            psrlw       mm2,8
            paddb       mm2,mm1

#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       ebx,byte Ptr [esi+9]
            sub         eax,ebx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         ebx,eax
            sub         ebx,(THR1+1)
            sar         ebx,31
#endif

            movd        eax,mm2
            movzx       eax,al
#ifndef USE_SPEEDUP
            sub         eax,ebx
#endif
            cmp         eax,THR2
            jl          ShortFilter
  
            //Determine MIN & MAX values to see if filtering should be used
            
#ifndef USE_SPEEDUP
            movq        mm0,[esi+1]
            movq        mm4,x80
            pxor        mm0,mm4

            movq        mm1,mm0
            punpckldq   mm0,mm0
            punpckhdq   mm1,mm1

            movq        mm3,xFFFFFFFF00000000

            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrld       mm1,16
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrlw       mm1,8
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2
            pxor        mm0,mm4
            
            movq        mm2,mm0
            psrlq       mm2,32
            psubb       mm0,mm2
            
            movd        eax,mm0
            movzx       eax,al //max-min
            mov         edx,iStepSize
            shl         edx,1

            cmp         eax,edx
            jge         EndOfLoop
#else
            movzx       eax,byte Ptr [esi]      //Can probobly skip these lines if the speedup is necessary
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            shl         edx,1
            cmp         eax,edx
            jg          EndOfLoop
#endif

  
            pxor        mm7,mm7
  
            movq        mm5,[esi+1]
            movq        mm3,mm5
            punpcklbw   mm3,mm7
            movq        mm4,mm3
            paddw       mm3,mm3

            movq        mm2,mm5
            psrlq       mm2,8
            paddw       mm3,mm4
            movq        mm6,mm5
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm2

            psrlq       mm6,16
            movq        mm2,mm6
            punpcklbw   mm6,mm7
            paddw       mm6,mm6
                      
            psrlq       mm2,8
            paddw       mm4,mm6
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm4
            paddw       mm4,mm2

            movq        mm0,m6222
            punpckhbw   mm5,mm7
            paddw       mm3,mm5
            paddw       mm5,mm5                           
            paddw       mm4,mm5                           
            paddw       mm4,mm5

            movd        mm1,[esi]
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi]
            movzx       edx,byte Ptr [esi+1]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV0
            movq        mm5,x000000000000FFFF
            psrlq       mm2,16
            pand        mm2,mm5
            pandn       mm5,mm1
            por         mm2,mm5
            movq        mm1,mm2
UseV0:
#endif
            movq        mm5,m0422
            pmullw      mm1,mm0
            paddw       mm3,mm1
            
            movq        mm0,m1221
            psllq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm3,mm2

            movq        mm5,m2226
            psllq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm3,mm1

            psllq       mm2,16
            paddw       mm3,mm2

            movd        mm1,[esi+6]                
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV9
            movq        mm0,xFFFF000000000000
            psllq       mm2,16
            pand        mm2,mm0
            pandn       mm0,mm1
            por         mm2,mm0
            movq        mm1,mm2
            movq        mm0,m1221
UseV9:
#endif
            pmullw      mm1,mm5
            paddw       mm4,mm1

            movq        mm5,m2240
            psrlq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm4,mm2

            psrlq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm4,mm1

            movq        mm0,x0008
            psrlq       mm2,16
            paddw       mm4,mm2

            paddw       mm3,mm0
            psrlw       mm3,4
            paddw       mm4,mm0
            psrlw       mm4,4
            packuswb    mm3,mm4
            movq        [esi+1],mm3

            jmp         EndOfLoop

ShortFilter:
#ifdef USE_SPEEDUP
            movd        mm0,ecx
            movzx       eax,[esi+3]
            movzx       edx,[esi+6]
            sub         ecx,ebx
            shl         ecx,3
            sub         eax,edx
            mov         edx,eax
            shl         eax,1
            add         eax,edx
            add         eax,ecx
            sar         eax,4
            movd        ecx,mm0
            movzx       eax,byte Ptr [g_deblockTab+eax]
            add         ebx,eax
            sub         ecx,eax
            test        ebx,0xFF00
            jnz         Clip2
            mov         [esi+4],bl
Clip2Ret:   test        ecx,0xFF00
            jnz         Clip1
            mov         [esi+5],cl
Clip1Ret:
#else
            pxor        mm7,mm7
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

            test        ecx,0x100
            jnz         Clip1
            mov         [esi+4],cl          
            
Clip1Ret:   test        ebx,0x100
            jnz         Clip2
            mov         [esi+5],bl
Clip2Ret:
#endif
EndOfLoop:                
             mov        eax,iPixelIncrement
             add        esi,eax
             
             dec        edi
             jnz        MainLoop

            }
return;
        _asm {
Clip1:      cmp         ecx,0
            jl          Clip1Down
            mov         [esi+4],0xFF
            jmp         Clip1Ret
Clip1Down:  mov         [esi+4],0
            jmp         Clip1Ret

Clip2:      cmp         ebx,0
            jl          Clip2Down
            mov         [esi+5],0xFF
            jmp         Clip2Ret
Clip2Down:  mov         [esi+5],0
            jmp         Clip2Ret
        }
}

Void_WMV FilterHalfVerticalEdge_FASTEST(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    static const Int64 x80 = 0x8080808080808080;
    static const Int64 thr1 = (THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56)) ^ x80;
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
  
    static const Int64 xFFFFFFFF00000000 = 0xFFFFFFFF00000000;
    static const Int64 x000000000000FFFF = 0x000000000000FFFF;
    static const Int64 xFFFF000000000000 = 0xFFFF000000000000;

    static const Int64 x0008 = 0x0008000800080008;
    static const Int64 s1 = 0xFFFB0002FFFB0002;
    static const Int64 s2 = 0xFFFE0005FFFE0005;
    static const Int64 s3 = 0xFFFE0005FFFB0002;
    static const Int64 x00000004 = 0x0000000400000004;       
    static const Int64 x00000005 = 0x0000000500000005;

    //For Long Filter
    static const Int64 m2226 = 0x0006000200020002;
    static const Int64 m2240 = 0x0000000400020002;
    static const Int64 m1221 = 0x0001000200020001;
    static const Int64 m6222 = 0x0002000200020006;
    static const Int64 m0422 = 0x0002000200040000;
    
    

       _asm {
            mov         esi,ppxlcCenter
            sub         esi,5
            mov         edi,iEdgeLength
MainLoop:
            
            //Determine Which Filter To Use Long or short    

#ifdef USE_SPEEDUP
            movzx       ebx,byte Ptr [esi+4]
            movzx       ecx,byte Ptr [esi+5]
            cmp         ebx,ecx
            jz          EndOfLoop
#endif
            movq        mm0,[esi]
            movq        mm1,[esi+1]

            movq        mm2,mm0
            psubusb     mm2,mm1
            psubusb     mm1,mm0
            por         mm2,mm1
            
            movq        mm3,x80
            movq        mm4,thr1
            pxor        mm2,mm3

            pcmpgtb     mm2,mm4
            pandn       mm2,x01
            movq        mm1,mm2
            psrlq       mm2,32
            paddb       mm2,mm1
            movq        mm1,mm2
            psrld       mm2,16
            paddb       mm2,mm1
            movq        mm1,mm2
            psrlw       mm2,8
            paddb       mm2,mm1

#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       ebx,byte Ptr [esi+9]
            sub         eax,ebx
            cdq
            xor         eax,edx
            sub         eax,edx
            xor         ebx,ebx
            cmp         eax,THR1
            jg          DontAdd1
            mov         ebx,1
DontAdd1:
#endif

            movd        eax,mm2
            movzx       eax,al
#ifndef USE_SPEEDUP
            add         eax,ebx
#endif
            cmp         eax,THR2
            jl          ShortFilter
  
            //Determine MIN & MAX values to see if filtering should be used
            
#ifndef USE_SPEEDUP
            movq        mm0,[esi+1]
            movq        mm4,x80
            pxor        mm0,mm4

            movq        mm1,mm0
            punpckldq   mm0,mm0
            punpckhdq   mm1,mm1

            movq        mm3,xFFFFFFFF00000000

            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrld       mm1,16
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2

            movq        mm1,mm0
            psrlw       mm1,8
            movq        mm2,mm0
            pcmpgtb     mm2,mm1
            pxor        mm2,mm3
            pand        mm0,mm2
            pandn       mm2,mm1
            por         mm0,mm2
            pxor        mm0,mm4
            
            movq        mm2,mm0
            psrlq       mm2,32
            psubb       mm0,mm2
            
            movd        eax,mm0
            movzx       eax,al //max-min
            mov         edx,iStepSize
            shl         edx,1

            cmp         eax,edx
            jge         EndOfLoop
#else
            movzx       eax,byte Ptr [esi]
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            shl         edx,1
            cmp         eax,edx
            jg          EndOfLoop
#endif            
  
            pxor        mm7,mm7
  
            movq        mm5,[esi+1]
            movq        mm3,mm5
            punpcklbw   mm3,mm7
            movq        mm4,mm3
            paddw       mm3,mm3

            movq        mm2,mm5
            psrlq       mm2,8
            paddw       mm3,mm4
            movq        mm6,mm5
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm2

            psrlq       mm6,16
            movq        mm2,mm6
            punpcklbw   mm6,mm7
            paddw       mm6,mm6
                      
            psrlq       mm2,8
            paddw       mm4,mm6
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm4
            paddw       mm4,mm2

            movq        mm0,m6222
            punpckhbw   mm5,mm7
            paddw       mm3,mm5
            paddw       mm5,mm5                           
            paddw       mm4,mm5                           
            paddw       mm4,mm5

            movd        mm1,[esi]
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi]
            movzx       edx,byte Ptr [esi+1]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV0
            movq        mm5,x000000000000FFFF
            psrlq       mm2,16
            pand        mm2,mm5
            pandn       mm5,mm1
            por         mm2,mm5
            movq        mm1,mm2
UseV0:
#endif
            movq        mm5,m0422
            pmullw      mm1,mm0
            paddw       mm3,mm1
            
            movq        mm0,m1221
            psllq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm3,mm2

            movq        mm5,m2226
            psllq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm3,mm1

            psllq       mm2,16
            paddw       mm3,mm2

            movd        mm1,[esi+6]                
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV9
            movq        mm0,xFFFF000000000000
            psllq       mm2,16
            pand        mm2,mm0
            pandn       mm0,mm1
            por         mm2,mm0
            movq        mm1,mm2
            movq        mm0,m1221
UseV9:
#endif
            pmullw      mm1,mm5
            paddw       mm4,mm1

            movq        mm5,m2240
            psrlq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm4,mm2

            psrlq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm4,mm1

            movq        mm0,x0008
            psrlq       mm2,16
            paddw       mm4,mm2

            paddw       mm3,mm0
            psrlw       mm3,4
            paddw       mm4,mm0
            psrlw       mm4,4
            packuswb    mm3,mm4
            movd        [esi+1],mm3

            jmp         EndOfLoop

ShortFilter:
#ifdef USE_SPEEDUP
            movd        mm0,ecx
            movzx       eax,[esi+3]
            movzx       edx,[esi+6]
            sub         ecx,ebx
            shl         ecx,3
            sub         eax,edx
            mov         edx,eax
            shl         eax,1
            add         eax,edx
            add         eax,ecx
            sar         eax,4
            movd        ecx,mm0
            movzx       eax,byte Ptr [g_deblockTab+eax]
            add         ebx,eax
            sub         ecx,eax
            test        ebx,0xFF00
            jnz         Clip2
            mov         [esi+4],bl
#else
            pxor        mm7,mm7
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

            test        ecx,0x100
            jnz         Clip1
            mov         [esi+4],cl          
            
Clip1Ret: 
#endif
EndOfLoop:                
             mov        eax,iPixelIncrement
             add        esi,eax
             
             dec        edi
             jnz        MainLoop

            }
return;
#ifdef USE_SPEEDUP
        _asm {
Clip2:      cmp         ebx,0
            jl          Clip2Down
            mov         [esi+5],0xFF
            jmp         EndOfLoop
Clip2Down:  mov         [esi+5],0
            jmp         EndOfLoop
        }
#else
        _asm {
Clip1:      cmp         ecx,0
            jl          Clip1Down
            mov         [esi+4],0xFF
            jmp         Clip1Ret
Clip1Down:  mov         [esi+4],0
            jmp         Clip1Ret

        }
#endif
}
#undef USE_SPEEDUP


//Find maxumum and minimum values in a 10x10 block
#pragma warning (disable:4799)
Void_WMV DetermineThreshold_MMX(U8_WMV *srcptr, I32_WMV *thr, I32_WMV *range, I32_WMV width) 
{
    static const Int64 x80               = 0x8080808080808080;
    static const Int64 x000000000000FFFF = 0x000000000000FFFF;
    static const Int64 xFFFFFFFF00000000 = 0xFFFFFFFF00000000;
    static const Int64 x00000000FFFFFFFF = 0x00000000FFFFFFFF;

    _asm {

        mov         eax,width           //Setup pointers
        mov         ebx,eax
        neg         ebx
        mov         esi,srcptr
        dec         esi
        lea         esi,[esi+eax]
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80
        movq        mm6,x000000000000FFFF

        movq        mm0,[esi+ebx*2]     //Load first line
        pxor        mm0,mm7
        movq        mm1,[esi+ebx]       //Load second line
        pxor        mm1,mm7
        
        movq        mm2,mm1             //Min Max first 2 lines
        pcmpgtb     mm2,mm0
        movq        mm4,mm0
        pand        mm0,mm2         
        pxor        mm4,mm0
        pandn       mm2,mm1
        pxor        mm1,mm2
        por         mm0,mm2
        por         mm1,mm4

        movq        mm2,[esi]           //Load third line
        pxor        mm2,mm7
        movq        mm3,[esi+eax]       //Load forth line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min first 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max first 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        movq        mm2,[esi+eax*2]     //Load fifth line
        pxor        mm2,mm7
        movq        mm3,[edi+ebx*2]     //Load sixth line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min last 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max last 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        movq        mm2,[edi+ebx]       //Load 7'th line
        pxor        mm2,mm7
        movq        mm3,[edi]           //Load 8'th line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min last 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max last 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4

        movq        mm2,[edi+eax]       //Load 9'th line
        pxor        mm2,mm7
        movq        mm3,[edi+eax*2]     //Load 10'th line
        pxor        mm3,mm7
        movq        mm4,mm3             //Min Max next 2 lines
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5
        
        movq        mm4,mm2             //Min last 2 pairs
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max last 2 pairs
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4
        
        movd        mm2,[esi+ebx*2+8]   //Load last 2 bytes of lines (0-8)
        movd        mm3,[esi+ebx+8]
        punpcklwd   mm2,mm3
        movd        mm3,[esi+8]
        movd        mm4,[esi+eax+8]
        punpcklwd   mm3,mm4
        punpckldq   mm2,mm3
        movd        mm3,[esi+eax*2+8]
        movd        mm4,[edi+ebx*2+8]
        punpcklwd   mm3,mm4
        movd        mm4,[edi+ebx+8]
        movd        mm5,[edi+8]
        punpcklwd   mm4,mm5
        punpckldq   mm3,mm4
        pxor        mm2,mm7             //Min Max last 2 bytes of each line
        pxor        mm3,mm7
        movq        mm4,mm3             
        pcmpgtb     mm4,mm2
        movq        mm5,mm2
        pand        mm2,mm4         
        pxor        mm5,mm2
        pandn       mm4,mm3
        pxor        mm3,mm4
        por         mm2,mm4
        por         mm3,mm5

        movq        mm4,mm2             //Min 
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm1             //Max 
        pcmpgtb     mm4,mm3
        pand        mm1,mm4
        pandn       mm4,mm3
        por         mm1,mm4


        movq        mm5,x00000000FFFFFFFF

        movd        mm2,[edi+eax+8]     //Load last 2 bytes of last 2 lines
        movd        mm3,[edi+eax*2+8]
        punpcklwd   mm2,mm3
        punpckldq   mm2,mm2
        pxor        mm2,mm7

        movq        mm4,mm2             //Min
        pcmpgtb     mm4,mm0
        pand        mm0,mm4
        pandn       mm4,mm2
        por         mm0,mm4

        movq        mm4,mm2             //Max
        pcmpgtb     mm4,mm1
        pand        mm2,mm4
        pandn       mm4,mm1
        por         mm2,mm4
        
        movq        mm1,mm0             //Max values in high 1/2 min values in low 1/2
        punpckldq   mm0,mm2
        punpckhdq   mm1,mm2
        movq        mm2,mm1
        
        movq        mm6,xFFFFFFFF00000000
        
        movq        mm1,mm0
        pcmpgtb     mm1,mm2
        pxor        mm1,mm6
        pand        mm2,mm1
        pandn       mm1,mm0
        por         mm2,mm1

        movq        mm0,mm2
        psrld       mm0,16
        movq        mm1,mm0
        pcmpgtb     mm1,mm2
        pxor        mm1,mm5
        pand        mm0,mm1
        pandn       mm1,mm2
        por         mm0,mm1

        movq        mm2,mm0
        psrld       mm0,8
        movq        mm1,mm0
        pcmpgtb     mm1,mm2
        pxor        mm1,mm5
        pand        mm0,mm1
        pandn       mm1,mm2
        por         mm0,mm1

        pxor        mm0,mm7
        movd        eax,mm0
        psrlq       mm0,32
        and         eax,0xFF
        movd        edx,mm0
        and         edx,0xFF


        lea         ebx,[eax+edx+1]
        shr         ebx,1
        mov         esi,thr
        mov         [esi],ebx

        sub         edx,eax
        mov         esi,range
        mov         [esi],edx
    }
}    


Void_WMV ApplySmoothing_MMX(U8_WMV *pixel,I32_WMV width, I32_WMV max_diff,I32_WMV thr)
{

    static const Int64 x80   = 0x8080808080808080;
    static const Int64 xF7   = 0xF7F7F7F7F7F7F7F7;
    static const Int64 x0008 = 0x0008000800080008;
    static const Int64 x01   = 0x0101010101010101;

	I8_WMV alignedBuf1[16*16+32];
	I8_WMV alignedBuf2[3*8+8];

	Int64* pAlignedBuf1 = (Int64*)(((I32_WMV)alignedBuf1 + 31) & ~31); // TEST
	Int64* pAlignedBuf2 = (Int64*)(((I32_WMV)alignedBuf2 + 7) & ~7);

    _asm {
        mov         eax,width           //Setup pointers
        mov         ebx,eax
        neg         ebx
        mov         esi,pixel
        dec         esi
        lea         esi,[esi+eax]
        lea         edi,[esi+eax*4]
        add         edi,eax
        movq        mm6,x80
		mov			ecx,pAlignedBuf1

        movd        mm7,thr             //Broadcast Threshold
        punpcklbw   mm7,mm7
        punpcklwd   mm7,mm7
        punpckldq   mm7,mm7
        psubusb     mm7,x01
        pxor        mm7,mm6
          
        movq        mm0,[esi+ebx*2]     //Compare first line with threshold
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx],mm0
        movd        mm0,[esi+ebx*2+8]   
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+8],mm0

        movq        mm0,[esi+ebx]       //Second Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+16],mm0
        movq        mm0,[esi+ebx+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+24],mm0

        movq        mm0,[esi]           //Third Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+32],mm0
        movq        mm0,[esi+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+40],mm0

        movq        mm0,[esi+eax]       //Forth Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+48],mm0
        movq        mm0,[esi+eax+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+56],mm0

        movq        mm0,[esi+eax*2]     //Sixth Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+64],mm0
        movq        mm0,[esi+eax*2+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+72],mm0

        movq        mm0,[edi+ebx*2]     //Sixth Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+80],mm0
        movq        mm0,[edi+ebx*2+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+88],mm0

        movq        mm0,[edi+ebx]       //7'th Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+96],mm0
        movq        mm0,[edi+ebx+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+104],mm0

        movq        mm0,[edi]           //8'th Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+112],mm0
        movq        mm0,[edi+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+120],mm0

        movq        mm0,[edi+eax]       //9'th Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+128],mm0
        movq        mm0,[edi+eax+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+136],mm0

        movq        mm0,[edi+eax*2]     //9'th Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+144],mm0
        movq        mm0,[edi+eax*2+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+152],mm0

        lea         edx,[edi+eax*2]
        movq        mm0,[edx+eax]       //10'th Line
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movq        [ecx+160],mm0
        movq        mm0,[edx+eax+8]
        pxor        mm0,mm6
        pcmpgtb     mm0,mm7
        movd        [ecx+168],mm0

        //Smoothing Filter
        pxor        mm7,mm7

        movd        mm0,max_diff        //Broadcast max_diff
        punpcklbw   mm0,mm0
        punpcklwd   mm0,mm0
        punpckldq   mm0,mm0

        movq        mm1,[esi+ebx*2]     //Upper Left Pixel
        movq        mm2,mm1
        punpcklbw   mm1,mm7
        punpckhbw   mm2,mm7

        movq        mm3,[esi+ebx*2+1]   //Upper Middel Pixel
        movq        mm4,mm3
        punpcklbw   mm3,mm7
        punpckhbw   mm4,mm7
        paddw       mm1,mm3
        paddw       mm2,mm4
        paddw       mm1,mm3
        paddw       mm2,mm4

        movq        mm3,[esi+ebx*2+2]   //Upper Right Pixel
        movq        mm4,mm3
        punpcklbw   mm3,mm7
        punpckhbw   mm4,mm7
        paddw       mm1,mm3
        paddw       mm2,mm4

        movq        mm3,[esi+ebx]       //Center Left Pixel
        movq        mm4,mm3
        punpcklbw   mm3,mm7
        punpckhbw   mm4,mm7

        movq        mm5,[esi+ebx+1]     //Center Middel Pixel
        movq        mm6,mm5
        punpcklbw   mm5,mm7
        punpckhbw   mm6,mm7
        paddw       mm3,mm5
        paddw       mm4,mm6
        paddw       mm3,mm5
        paddw       mm4,mm6

        movq        mm5,[esi+ebx+2]     //Center Right Pixel
        movq        mm6,mm5
        punpcklbw   mm5,mm7
        punpckhbw   mm6,mm7
        paddw       mm3,mm5
        paddw       mm4,mm6
        
		mov			edi,pAlignedBuf2
        mov         edx,8

        movq        [edi],mm3           //Save for next loop itteration
        movq        [edi+8],mm4
        movq        [edi+16],mm0

SmoothingLoop:
        
        pxor        mm7,mm7
        movq        mm6,xF7             //Generate mask
        movq        mm0,[ecx]
        movq        mm5,[ecx+1]
        paddb       mm0,mm5
        movq        mm5,[ecx+2]
        paddb       mm0,mm5
        movq        mm5,[ecx+16]
        paddb       mm0,mm5
        movq        mm5,[ecx+17]
        paddb       mm0,mm5
        movq        mm5,[ecx+18]
        paddb       mm0,mm5
        movq        mm5,[ecx+32]
        paddb       mm0,mm5
        movq        mm5,[ecx+33]
        paddb       mm0,mm5
        movq        mm5,[ecx+34]
        paddb       mm0,mm5
        movq        mm5,mm0
        pcmpeqb     mm0,mm7
        pcmpeqb     mm5,mm6
        por         mm0,mm5

        paddw       mm1,mm3             //Top + 2*Center
        paddw       mm2,mm4
        paddw       mm1,mm3
        paddw       mm2,mm4

        movq        mm3,[esi]           //Bottom Left Pixel
        movq        mm4,mm3             
        punpcklbw   mm3,mm7             
        punpckhbw   mm4,mm7             
                                        
        movq        mm5,[esi+1]         //Bottom Middel Pixel
        movq        mm6,mm5             
        punpcklbw   mm5,mm7             
        punpckhbw   mm6,mm7             
        paddw       mm3,mm5             
        paddw       mm4,mm6             
        paddw       mm3,mm5             
        paddw       mm4,mm6             
                                        
        movq        mm5,[esi+2]         //Bottom Right Pixel
        movq        mm6,mm5             
        punpcklbw   mm5,mm7             
        punpckhbw   mm6,mm7             
        paddw       mm3,mm5             
        paddw       mm4,mm6             
                                        
        movq        mm5,x0008           //((Top + 2*Center) + Bottom + 8) >> 4
        paddw       mm1,mm3                 
        paddw       mm2,mm4
        paddw       mm1,mm5
        paddw       mm2,mm5
        psrlw       mm1,4
        psrlw       mm2,4
        packuswb    mm1,mm2
                                            
        movq        mm2,[esi+ebx+1]     //Move Masked

        movq        mm6,[edi+16]             //Compare diffrence with max_diff
        movq        mm7,mm2    
        psubusb     mm7,mm1
        movq        mm5,mm1    
        psubusb     mm5,mm2
        psubusb     mm7,mm6
        paddusb     mm1,mm7
        psubusb     mm5,mm6
        psubusb     mm1,mm5

        pand        mm1,mm0                 
        pandn       mm0,mm2                 
        por         mm0,mm1                 
        movq        [esi+ebx+1],mm0         
        
        movq        mm1,[edi]
        movq        mm2,[edi+8]

        movq        [edi],mm3           //Save for next loop itteration
        movq        [edi+8],mm4

        add         esi,eax             //Adjust Pointers  & Loop
        add         ecx,16                  
        dec         edx
        jnz         SmoothingLoop
    }
}

//////////////////////////// Katmai Versions /////////////////////////////////
Void_WMV DetermineThreshold_KNI(U8_WMV *ptr, I32_WMV *thr, I32_WMV *range, I32_WMV width) 
{
    I32_WMV max = 0;                                         
    I32_WMV min = 255;                                       
	I32_WMV asmRange, asmThr;
	U8_WMV * pPcStartPtr = ptr - 1 - width;

	__asm 
	{
		mov			esi, pPcStartPtr
		mov			edi, width
		pcmpeqw		mm4, mm4		// use for Min, 1st 8
		pcmpeqw		mm5, mm5		// use for Min, last 2
		pxor		mm6, mm6		// use for Max, 1st 8
		pxor		mm7, mm7		// use for Max, last 2
		mov			ecx, 5

	Compute:
		movq		mm0, [esi]		// row -1
		pminub		mm4_mm0
		pmaxub		mm6_mm0
		movd		mm1, [esi+8]
		pminub		mm5_mm1
		pmaxub		mm7_mm1

		movq		mm0, [esi][edi]		// row 0
		pminub		mm4_mm0
		pmaxub		mm6_mm0
		movd		mm1, [esi+8][edi]
		pminub		mm5_mm1
		pmaxub		mm7_mm1

		lea			esi, [esi+edi*2]
		dec			ecx
		jg			Compute

		movq		mm0, mm4		// min
		movq		mm1, mm6		// max

		psrlq		mm0, 32
		psrlq		mm1, 32

		pminub		mm4_mm0
		pmaxub		mm6_mm1

		movq		mm0, mm4		// min
		movq		mm1, mm6		// max

		psrld		mm0, 16
		psrld		mm1, 16

		pminub		mm4_mm0		    // only 2 now so factor in last 2
		pmaxub		mm6_mm1	    	// only 2 now so factor in last 2

		pminub		mm4_mm5
		pmaxub		mm6_mm7

		movq		mm0, mm4		// min
		movq		mm1, mm6		// max

		psrlw		mm0, 8
		psrlw		mm1, 8

		pminub		mm4_mm0 		// final min
		pmaxub		mm6_mm1 		// final max

		movq		mm7, mm6
		psubb		mm7, mm4		// range = max - min
		pavgb		mm6_mm4   		// thr = (max + min + 1) / 2;

		movd		eax, mm6
		movd		ebx, mm7
		movzx		eax, al
		movzx		ebx, bl			
		
		mov			asmThr, eax
		mov			asmRange, ebx
	}

    *thr = asmThr;					// (max + min + 1) / 2;
    *range = asmRange;				// max - min;
}    

Void_WMV FilterVerticalEdge_KNI(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{

    static const Int64 x80 = 0x8080808080808080;
    static const Int64 thr1 = (THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56)) ^ x80;
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
  
    static const Int64 xFFFFFFFF00000000 = 0xFFFFFFFF00000000;
    static const Int64 x000000000000FFFF = 0x000000000000FFFF;
    static const Int64 xFFFF000000000000 = 0xFFFF000000000000;

    static const Int64 x0008 = 0x0008000800080008;
    static const Int64 s1 = 0xFFFB0002FFFB0002;
    static const Int64 s2 = 0xFFFE0005FFFE0005;
    static const Int64 s3 = 0xFFFE0005FFFB0002;
    static const Int64 x00000004 = 0x0000000400000004;       
    static const Int64 x00000005 = 0x0000000500000005;

    //For Long Filter
    static const Int64 m2226 = 0x0006000200020002;
    static const Int64 m2240 = 0x0000000400020002;
    static const Int64 m1221 = 0x0001000200020001;
    static const Int64 m6222 = 0x0002000200020006;
    static const Int64 m0422 = 0x0002000200040000;
    
    

       _asm {
            mov         esi,ppxlcCenter
            sub         esi,5
            mov         edi,iEdgeLength
MainLoop:
            
            //Determine Which Filter To Use Long or short    

#ifdef USE_SPEEDUP
            movzx       ebx,byte Ptr [esi+4]
            movzx       ecx,byte Ptr [esi+5]
            cmp         ebx,ecx
            jz          EndOfLoop
#endif
            movq        mm0,[esi]
            movq        mm1,[esi+1]

            movq        mm2,mm0
            psubusb     mm2,mm1
            psubusb     mm1,mm0
            por         mm2,mm1
            
            movq        mm3,x80
            movq        mm4,thr1
            pxor        mm2,mm3

            pcmpgtb     mm2,mm4
            pxor        mm4,mm4
            pandn       mm2,x01
            psadbw      mm2_mm4
            movq        mm1,mm2
            psrlq       mm2,32
            paddb       mm2,mm1
            movq        mm1,mm2
            psrld       mm2,16
            paddb       mm2,mm1

#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       ebx,byte Ptr [esi+9]
            sub         eax,ebx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         ebx,eax
            sub         ebx,(THR1+1)
            sar         ebx,31
#endif

            movd        eax,mm2
            movzx       eax,al
#ifndef USE_SPEEDUP
            sub         eax,ebx
#endif
            cmp         eax,THR2
            jl          ShortFilter
  
            //Determine MIN & MAX values to see if filtering should be used
            
#ifndef USE_SPEEDUP
            movq        mm0, [esi+1]
			movq		mm1, mm0
            punpckldq   mm0, mm0
            punpckhdq   mm1, mm1
			movq		mm2, mm0

			pmaxub		mm0_mm1
			pminub		mm2_mm1

			movq		mm1, mm0
			movq		mm3, mm2

			psrld		mm1, 16
			psrld		mm3, 16

			pmaxub		mm0_mm1
			pminub		mm2_mm3

			movq		mm1, mm0
			movq		mm3, mm2

			psrlw		mm1, 8
			psrlw		mm3, 8

			pmaxub		mm0_mm1
			pminub		mm2_mm3

            psubb       mm0,mm2
            
            movd        eax,mm0
            movzx       eax,al //max-min
            mov         edx,iStepSize
            shl         edx,1

            cmp         eax,edx
            jge         EndOfLoop
#else
            movzx       eax,byte Ptr [esi]      //Can probobly skip these lines if the speedup is necessary
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            shl         edx,1
            cmp         eax,edx
            jg          EndOfLoop
#endif

  
            pxor        mm7,mm7
  
            movq        mm5,[esi+1]
            movq        mm3,mm5
            punpcklbw   mm3,mm7
            movq        mm4,mm3
            paddw       mm3,mm3

            movq        mm2,mm5
            psrlq       mm2,8
            paddw       mm3,mm4
            movq        mm6,mm5
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm2

            psrlq       mm6,16
            movq        mm2,mm6
            punpcklbw   mm6,mm7
            paddw       mm6,mm6
                      
            psrlq       mm2,8
            paddw       mm4,mm6
            punpcklbw   mm2,mm7
            paddw       mm4,mm2
            paddw       mm3,mm4
            paddw       mm4,mm2

            movq        mm0,m6222
            punpckhbw   mm5,mm7
            paddw       mm3,mm5
            paddw       mm5,mm5                           
            paddw       mm4,mm5                           
            paddw       mm4,mm5

            movd        mm1,[esi]
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi]
            movzx       edx,byte Ptr [esi+1]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV0
            movq        mm5,x000000000000FFFF
            psrlq       mm2,16
            pand        mm2,mm5
            pandn       mm5,mm1
            por         mm2,mm5
            movq        mm1,mm2
UseV0:
#endif
            movq        mm5,m0422
            pmullw      mm1,mm0
            paddw       mm3,mm1
            
            movq        mm0,m1221
            psllq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm3,mm2

            movq        mm5,m2226
            psllq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm3,mm1

            psllq       mm2,16
            paddw       mm3,mm2

            movd        mm1,[esi+6]                
            punpcklbw   mm1,mm7
            movq        mm2,mm1
#ifndef USE_SPEEDUP
            movzx       eax,byte Ptr [esi+8]
            movzx       edx,byte Ptr [esi+9]
            sub         eax,edx
            cdq
            xor         eax,edx
            sub         eax,edx
            mov         edx,iStepSize
            cmp         eax,edx
            jl          UseV9
            movq        mm0,xFFFF000000000000
            psllq       mm2,16
            pand        mm2,mm0
            pandn       mm0,mm1
            por         mm2,mm0
            movq        mm1,mm2
            movq        mm0,m1221
UseV9:
#endif
            pmullw      mm1,mm5
            paddw       mm4,mm1

            movq        mm5,m2240
            psrlq       mm2,16
            movq        mm1,mm2
            pmullw      mm2,mm5
            paddw       mm4,mm2

            psrlq       mm1,16
            movq        mm2,mm1
            pmullw      mm1,mm0
            paddw       mm4,mm1

            movq        mm0,x0008
            psrlq       mm2,16
            paddw       mm4,mm2

            paddw       mm3,mm0
            psrlw       mm3,4
            paddw       mm4,mm0
            psrlw       mm4,4
            packuswb    mm3,mm4
            movq        [esi+1],mm3

            jmp         EndOfLoop

ShortFilter:
#ifdef USE_SPEEDUP
            movd        mm0,ecx
            movzx       eax,[esi+3]
            movzx       edx,[esi+6]
            sub         ecx,ebx
            shl         ecx,3
            sub         eax,edx
            mov         edx,eax
            shl         eax,1
            add         eax,edx
            add         eax,ecx
            sar         eax,4
            movd        ecx,mm0
            movzx       eax,byte Ptr [g_deblockTab+eax]
            add         ebx,eax
            sub         ecx,eax
            test        ebx,0xFF00
            jnz         Clip2
            mov         [esi+4],bl
Clip2Ret:   test        ecx,0xFF00
            jnz         Clip1
            mov         [esi+5],cl
Clip1Ret:
#else
            pxor        mm7,mm7
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

            test        ecx,0x100
            jnz         Clip1
            mov         [esi+4],cl          
            
Clip1Ret:   test        ebx,0x100
            jnz         Clip2
            mov         [esi+5],bl
Clip2Ret:
#endif
EndOfLoop:                
             mov        eax,iPixelIncrement
             add        esi,eax
             
             dec        edi
             jnz        MainLoop

            }
return;
        _asm {
Clip1:      cmp         ecx,0
            jl          Clip1Down
            mov         [esi+4],0xFF
            jmp         Clip1Ret
Clip1Down:  mov         [esi+4],0
            jmp         Clip1Ret

Clip2:      cmp         ebx,0
            jl          Clip2Down
            mov         [esi+5],0xFF
            jmp         Clip2Ret
Clip2Down:  mov         [esi+5],0
            jmp         Clip2Ret
        }
}


Void_WMV FilterHorizantalEdge_KNI(U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, 
                              I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize)
{
 
    static const Int64 thr1 = THR1 | (THR1 << 8) | (THR1 << 16) | (THR1 << 24) | ((Int64)THR1 << 32) | ((Int64)THR1 << 40) | ((Int64)THR1 << 48) | ((Int64)THR1 << 56);
    static const Int64 thr2 = (THR2-1) | ((THR2-1) << 8) | ((THR2-1) << 16) | ((THR2-1) << 24) | ((Int64)(THR2-1) << 32) | ((Int64)(THR2-1) << 40) | ((Int64)(THR2-1) << 48) | ((Int64)(THR2-1) << 56);
    static const Int64 x01 = 0x0101010101010101;
    static const Int64 x80 = 0x8080808080808080;
    static const Int64 x06 = 0x0006000600060006;
    static const Int64 x08 = 0x0008000800080008;
    static const Int64 x05 = 0x0005000500050005;
    static const Int64 x04 = 0x0004000400040004;
    static const Int64 x0001 = 0x0001000100010001;
    static const Int64 x0002 = 0x0002000200020002;

    Int64 act;
    Int64 iStepBroad;
    Int64 i2StepBroad;
    Int64 lminmax, lminmax2;
    Int64 mask;
    I32_WMV i;
    I32_WMV mytemp;
    U8_WMV* pV0 = ppxlcCenter - (iPixelDistance*5);
   
    _asm {

        sub esp, 64  // IW
        movd        mm0,iStepSize       //Broadcast iStepSize
        punpcklbw   mm0,mm0
  		pshufw		mm0_mm0 emit(0)	// broadcast

        movq        mm5,mm0
        psllq       mm0,1
        psubb       mm5,x01 
        movq        iStepBroad,mm5
        movq        i2StepBroad,mm0
        
        mov         eax,iPixelDistance
        mov         esi,pV0
        lea         esi,[esi+eax*2]

        mov         ecx,iEdgeLength
        shr         ecx,3
HLoop:
        mov         i,ecx

        mov         eax,iPixelDistance
        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,thr1
        movq        mm6,x01

        //phi(v0 - v1)
        movq        mm0,[esi+ebx*2]     //Load
        movq        mm1,[esi+ebx]
        psubb       mm0,mm1             //Subtract
        pxor        mm2,mm2             //Absolute Value
        pcmpgtb     mm2,mm0
        pxor        mm0,mm2
        psubb       mm0,mm2
        pcmpgtb     mm0,mm7	            //if (mm0 <= THR1) mm0 = 1 else mm0 = 0
        pandn       mm0,mm6

        //phi(v1 - v2)
        movq        mm2,[esi]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        pand        mm3,mm6
        paddb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             

        //phi(v2 - v3)
        movq        mm1,[esi+eax]
        psubb       mm2,mm1 
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             

        //phi(v3 - v4)
        movq        mm2,[esi+eax*2]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             

        //phi(v4 - v5)
        movq        mm1,[edi+ebx*2]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             

        //phi(v5 - v6)
        movq        mm2,[edi+ebx]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             
        
        //phi(v6 - v7)
        movq        mm1,[edi]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             
        
        //phi(v7 - v8)
        movq        mm2,[edi+eax]
        psubb       mm1,mm2     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm1
        pxor        mm1,mm3
        psubb       mm1,mm3
        pcmpgtb     mm1,mm7	    
        pandn       mm1,mm6
        paddb       mm0,mm1             
        
        //phi(v8 - v9)
        movq        mm1,[edi+eax*2]
        psubb       mm2,mm1     
        pxor        mm3,mm3     
        pcmpgtb     mm3,mm2
        pxor        mm2,mm3
        psubb       mm2,mm3
        pcmpgtb     mm2,mm7	    
        pandn       mm2,mm6
        paddb       mm0,mm2             
        
        pcmpgtb     mm0,thr2

        movq        act,mm0

        movq        mm1,mm0
        movq        mm2,mm0
        psrlq       mm2,32
        psrlq       mm1,32
        pand        mm2,mm0
        por         mm1,mm0

        
        movq        mask,mm0

        movd        ebx,mm2     //check for 8 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetMode2

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultMode //Default Mode for all 8

        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetMode //DC Offset Mode for all 4

        mov         eax,iPixelDistance  //Setup pointers
        call        MinMaxMask_KNI
        movq        lminmax,mm6

        movd        mm7,mask           
        call        MixedMode4_KNI 

        add         esi,4

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
        
        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetModeA //DC Offset Mode for all 8
            
MixedModeA:

        mov         eax,iPixelDistance
        movq        mm6,lminmax
        movq        mm7,mask           
        psrlq       mm7,32
        call        MixedMode4_KNI 
        
        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DcOffsetMode:    
        mov         eax,iPixelDistance  //Setup pointers
        call        MinMaxMask_KNI

        mov         eax,iPixelDistance
        movq        lminmax,mm6
        call        DcOffsetMode4_KNI

        add         esi,4

        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
       
        movd        ebx,mm0    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jnz         MixedModeA
            
DcOffsetModeA:    
        mov         eax,iPixelDistance
        movq        mm6,lminmax
        call        DcOffsetMode4_KNI
        
        
        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DcOffsetMode2:    

        call        MinMaxMask_KNI
        movq        lminmax,mm6
  
            
        mov         eax,iPixelDistance
        call        DcOffsetMode4_KNI
        
        add         esi,4

        movq        mm6,lminmax
        psrlq       mm6,32
        movq        lminmax,mm6

        mov         eax,iPixelDistance
        movq        mm6,lminmax
        call        DcOffsetMode4_KNI
        

        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop
        jmp         end

DefaultMode:
        mov         eax,iPixelDistance
        call        DefaultMode4_KNI

        add         esi,4

        movq        mm0,act
        psrlq       mm0,32
//        movd        eax,mm0

        movd        eax,mm0     //check for all 8 using default mode (short filter)
        test        eax,eax
        jz          DefaultModeA //Default Mode for all 8
    
        movd        mytemp,mm0

        mov         eax,iPixelDistance  //Setup pointers
        call        MinMaxMask_KNI
        movq        lminmax,mm6


        mov         ebx,mytemp    //check for 4 using DC offset mode (long filter)
        cmp         ebx,0xFFFFFFFF
        jz          DcOffsetModeA //DC Offset Mode for all 8
        
        jmp         MixedModeA

DefaultModeA:

        mov         eax,iPixelDistance
        call        DefaultMode4_KNI

        add         esi,4
        mov         ecx,i
        dec         ecx
        jnz         HLoop

end:    emms
        add esp, 64  // IW
        }
    return;

    _asm{
DefaultMode4_KNI:
        mov         ebx,eax
        neg         ebx
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

		pminsw		mm0_mm4	  //if (abs(a3.0) < abs(a3.1)) mm0 = abs(a3.0) else mm0 = abs(a3.1)
		pminsw		mm0_mm2

        pxor        mm0,mm5  //a3.0' = mm0 = (mm0 * SIGN(a3.0))
        psubw       mm0,mm5

        //Broadcast iStepSize
        movd        mm5, iStepSize
		pshufw		mm5_mm5 emit(0)	// broadcast

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

		pminsw		mm0_mm3         //Clip to less than max

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

        ret
   }

   _asm{        
MixedMode4_KNI:
        movq        [esp+40],mm7
        pand        mm6,mm7
        movq        lminmax2,mm6

        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax

        movq        mm7,x80
        movd        mm0,[esi+ebx*2]           //calculate value that will be used as v0
        movd        mm1,[esi+ebx]
        movq        mm2,mm0
        movq        mm3,mm1
        psubusb     mm2,mm1
        psubusb     mm3,mm0
        por         mm2,mm3
        movq        mm5,iStepBroad
        pxor        mm2,mm7
        pxor        mm5,mm7
        pcmpgtb     mm2,mm5
        pand        mm1,mm2
        pandn       mm2,mm0
        por         mm1,mm2             //if (abs(v1-v0) < iStepSize) mm1 = v0 else mm1 = v1

        movq        mm0,mm1

        pxor        mm7,mm7

        movq        mm2,x06             //6*v0
        punpcklbw   mm0,mm7             
        movq        mm5,mm0
        movq        mm4,mm0
        psllw       mm5,2
        pmullw      mm0,mm2
                                        
        movd        mm2,[esi+ebx]       //v1
        punpcklbw   mm2,mm7
        movq        mm3,mm2
        paddw       mm5,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm6,mm4
        psllw       mm2,2
        paddw       mm0,mm2

        movd        mm2,[esi]           //v2 
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm0,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2

        movd        mm2,[esi+eax]       //v3
        punpcklbw   mm2,mm7
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2


        movd        mm2,[esi+eax*2]     //v4
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2
        paddw       mm6,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2

        movd        mm2,[edi+ebx*2]     //v5
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm4
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm3,mm2

        paddw       mm0,x08
        psrlw       mm0,4
        packuswb    mm0,mm0

        movq        mm7,lminmax2          //Output masked  v1
        movd        mm2,[esi+ebx]
        pand        mm0,mm7
        pandn       mm7,mm2
        por         mm0,mm7
        movd        [esp+4],mm0
        pxor        mm7,mm7


        movd        mm2,[edi+ebx]         //+ v6
        punpcklbw   mm2,mm7
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        movq        mm0,mm2

        paddw       mm5,x08
        psrlw       mm5,4
        packuswb    mm5,mm5

        movq        mm7,lminmax2          //Output masked  v2
        movd        mm2,[esi]
        pand        mm5,mm7
        pandn       mm7,mm2
        por         mm5,mm7
        movd        [esp+8],mm5    
        pxor        mm7,mm7


        movd        mm2,[edi]           //+v7     
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm2,mm2
        paddw       mm3,mm2

        movq        mm5,mm2

        paddw       mm4,x08
        psrlw       mm4,4
        packuswb    mm4,mm4
        
        movq        mm7,lminmax2          //Output masked  v3
        movd        mm2,[esi+eax]
        pand        mm4,mm7
        pandn       mm7,mm2
        por         mm4,mm7
        movd        [esp+12],mm4    
  
        movq        mm7,mm0
        paddw       mm0,mm5
        movq        mm4,mm0
        paddw       mm0,mm7
        paddw       mm5,mm5
        paddw       mm5,mm7
        pxor        mm7,mm7
    
        movd        mm2,[edi+eax]      //v8
        punpcklbw   mm2,mm7
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm2

        paddw       mm6,x08
        psrlw       mm6,4
        packuswb    mm6,mm6
        
        movq        mm7,lminmax2          //Output masked   v4
        movd        mm2,[esi+eax*2]
        pand        mm6,mm7
        pandn       mm7,mm2
        por         mm6,mm7
        movd        [esp+16],mm6    
        movq        mm6,mm0

        movq        mm7,mm3             //calculate value that will be used as v9
        movd        mm1,[edi+eax*2]           
        movd        mm0,[edi+eax]
        movq        mm2,mm0
        movq        mm3,mm1
        psubusb     mm2,mm1
        psubusb     mm3,mm0
        por         mm2,mm3
        movq        mm3,mm7
        movq        mm7,iStepBroad
        pxor        mm7,x80
        pxor        mm2,x80
        pcmpgtb     mm2,mm7
        pand        mm0,mm2
        pandn       mm2,mm1
        por         mm0,mm2             //if (abs(v8-v9) < iStepSize) mm0 = v9 else mm0 = v8
        movq        mm1,mm3
        pxor        mm7,mm7

        movq        mm3,mm0             //v9
        punpcklbw   mm3,mm7
        paddw       mm1,mm3
        paddw       mm3,mm3
        paddw       mm6,mm3
        movq        mm2,x08
        paddw       mm5,mm3
        paddw       mm5,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3

        paddw       mm1,mm2
        psrlw       mm1,4
        packuswb    mm1,mm1

        movq        mm3,lminmax2          //Output masked   v5
        movd        mm0,[edi+ebx*2]
        pand        mm1,mm3
        pandn       mm3,mm0
        por         mm1,mm3
        movd        [esp+20],mm1

        movd        mm1,[esi]           //v2
        punpcklbw   mm1,mm7
        paddw       mm6,mm1

        movd        mm1,[esi+eax]       //v3
        punpcklbw   mm1,mm7
        paddw       mm6,mm1
        paddw       mm5,mm1

        movd        mm1,[esi+eax*2]     //v4
        punpcklbw   mm1,mm7
        paddw       mm5,mm1
        paddw       mm4,mm1
        paddw       mm1,mm1
        paddw       mm6,mm1
        
        movd        mm1,[edi+ebx*2]     //v5
        punpcklbw   mm1,mm7
        paddw       mm4,mm1
        paddw       mm1,mm1
        paddw       mm6,mm1
        paddw       mm5,mm1
        
        paddw       mm6,mm2
        psrlw       mm6,4
        packuswb    mm6,mm6

        movq        mm3,lminmax2          //Output masked v6
        movd        mm1,[edi+ebx]
        pand        mm6,mm3
        pandn       mm3,mm1
        por         mm6,mm3
        movd        [esp+24],mm6

        paddw       mm5,mm2
        psrlw       mm5,4
        packuswb    mm5,mm5

        movq        mm3,lminmax2          //Output masked v7
        movd        mm0,[edi]
        pand        mm5,mm3
        pandn       mm3,mm0
        por         mm5,mm3
        movd        [esp+28],mm5

        paddw       mm4,mm2
        psrlw       mm4,4
        packuswb    mm4,mm4
        
        movq        mm3,lminmax2          //Output masked  v8
        movd        mm0,[edi+eax]
        pand        mm4,mm3
        pandn       mm3,mm0
        por         mm4,mm3
        
        movd        [esp+32],mm4   

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

		pminsw		mm0_mm4	  //if (abs(a3.0) < abs(a3.1)) mm0 = abs(a3.0) else mm0 = abs(a3.1)
		pminsw		mm0_mm2

        pxor        mm0,mm5  //a3.0' = mm0 = (mm0 * SIGN(a3.0))
        psubw       mm0,mm5

        //Broadcast iStepSize
        movd        mm5, iStepSize
		pshufw		mm5_mm5 emit(0)	// broadcast

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

        pminsw		mm0_mm3         //Clip to less than max
        
        pxor        mm2,mm2         //Clip to zero
        pcmpgtw     mm2,mm0
        pandn       mm2,mm0

        pxor        mm2,mm5         //Restore original sign
        psubw       mm2,mm5

        pand        mm2,mm6         //mask out 

        psubw       mm4,mm2
        paddw       mm7,mm2

        movd        mm2,[esp+40]
        movq        mm3,mm2

        packuswb    mm4,mm4
        pandn       mm2,mm4
        
        packuswb    mm7,mm7
        pandn       mm3,mm7
        
        movd        mm0,[esp+4]           
        movd        [esi+ebx],mm0           //v1

        movd        mm0,[esp+8]
        movd        [esi],mm0               //v2

        movd        mm0,[esp+12]
        movd        [esi+eax],mm0           //v3

        movd        mm0,[esp+16]
        movd        mm1,[esp+40]
        pand        mm1,mm0
        por         mm1,mm2
        movd        [esi+eax*2],mm1         //v4

        movd        mm0,[esp+20]
        movd        mm1,[esp+40]
        pand        mm1,mm0
        por         mm1,mm3
        movd        [edi+ebx*2],mm1         //v5

        movd        mm0,[esp+24]           
        movd        [edi+ebx],mm0           //v6

        movd        mm0,[esp+28]           
        movd        [edi],mm0               //v7
        
        movd        mm0,[esp+32]           
        movd        [edi+eax],mm0           //v8

        ret
    }

    _asm{        
MinMaxMask_KNI:
        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax
        movq        mm7,x80

        movq        mm0,[esi+ebx]       //Load first line
        movq        mm1,[esi]           //Load second line
		movq		mm2, mm0			
		pminub		mm0_mm1
		pmaxub		mm1_mm2
				
        movq        mm2,[esi+eax]       //Load third line
        movq        mm3,[esi+eax*2]     //Load forth line
		
		pminub		mm0_mm2
		pmaxub		mm1_mm2

		pminub		mm0_mm3
		pmaxub		mm1_mm3


        movq        mm2,[edi+ebx*2]     //Load fifth line
        movq        mm3,[edi+ebx]       //Load sixth line

		pminub		mm0_mm2
		pmaxub		mm1_mm2

		pminub		mm0_mm3
		pmaxub		mm1_mm3

        movq        mm2,[edi]           //Load 7'th line
        movq        mm3,[edi+eax]       //Load 8'th line
		
		pminub		mm0_mm2
		pmaxub		mm1_mm2

		pminub		mm0_mm3
		pmaxub		mm1_mm3

        psubb       mm1,mm0

        movq        mm0,i2StepBroad

        pxor        mm0,mm7
        pxor        mm1,mm7
        pcmpgtb     mm0,mm1             // (max-min) < 2*iStepSize

        movq        mm6,mm0
        ret
        }

    _asm {
DcOffsetMode4_KNI: //(/*esi=pV0,eax=iPixelDistance,mm6=minmax*/) -- really the same as DcOffsetMode4
        movq        lminmax2,mm6
        
        mov         ebx,eax
        neg         ebx
        lea         edi,[esi+eax*4]
        add         edi,eax
        
        movq        mm7,x80
        movd        mm1,[esi+ebx*2]           //calculate value that will be used as v0
        movd        mm0,[esi+ebx]
        movq        mm2,mm1
        movq        mm3,mm0
        psubusb     mm2,mm0
        psubusb     mm3,mm1
        por         mm2,mm3
        movq        mm5,iStepBroad
        pxor        mm2,mm7
        pxor        mm5,mm7
        pcmpgtb     mm2,mm5
        pand        mm0,mm2
        pandn       mm2,mm1
        por         mm0,mm2             //if (abs(v1-v0) < iStepSize) mm0 = v0 else mm0 = v1
        
        pxor        mm7,mm7
        
        movq        mm2,x06             //6*v0
        punpcklbw   mm0,mm7             
        movq        mm5,mm0
        movq        mm4,mm0
        psllw       mm5,2
        pmullw      mm0,mm2
        
        movd        mm2,[esi+ebx]       //v1
        punpcklbw   mm2,mm7
        movq        mm3,mm2
        paddw       mm5,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm6,mm4
        psllw       mm2,2
        paddw       mm0,mm2
        
        movd        mm2,[esi]           //v2 
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm0,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2
        
        movd        mm2,[esi+eax]       //v3
        punpcklbw   mm2,mm7
        paddw       mm2,mm2
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        
        movd        mm2,[esi+eax*2]     //v4
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm5,mm2
        paddw       mm6,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        
        movd        mm2,[edi+ebx*2]     //v5
        punpcklbw   mm2,mm7
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        movq        mm1,x08
        paddw       mm4,mm4
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm3,mm2
        
        paddw       mm0,mm1
        psrlw       mm0,4
        
        packuswb    mm0,mm0
        
        movq        mm7,lminmax2          //Output masked  v1
        movd        mm2,[esi+ebx]
        pand        mm0,mm7
        pandn       mm7,mm2
        por         mm0,mm7
        movd        [esi+ebx],mm0    
        pxor        mm7,mm7
        
        movd        mm2,[edi+ebx]       //+ v6
        punpcklbw   mm2,mm7
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm2,mm2
        paddw       mm6,mm2
        paddw       mm3,mm2
        movq        mm0,mm2
        
        paddw       mm5,mm1
        psrlw       mm5,4
        
        packuswb    mm5,mm5
        
        movq        mm7,lminmax2          //Output masked  v2
        movd        mm2,[esi]
        pand        mm5,mm7
        pandn       mm7,mm2
        por         mm5,mm7
        movd        [esp+8],mm5    
        pxor        mm7,mm7
        
        movd        mm2,[edi]           //+v7     
        punpcklbw   mm2,mm7
        paddw       mm4,mm2
        paddw       mm6,mm2
        paddw       mm2,mm2
        paddw       mm3,mm2
        
        movq        mm5,mm2
        
        paddw       mm4,mm1
        psrlw       mm4,4
        
        packuswb    mm4,mm4
        
        movq        mm7,lminmax2          //Output masked  v3
        movd        mm2,[esi+eax]
        pand        mm4,mm7
        pandn       mm7,mm2
        por         mm4,mm7
        movd        [esp+12],mm4    
        
        movq        mm7,mm0
        paddw       mm0,mm5
        movq        mm4,mm0
        paddw       mm0,mm7
        paddw       mm5,mm5
        paddw       mm5,mm7
        pxor        mm7,mm7
        
        movd        mm2,[edi+eax]       //v8
        punpcklbw   mm2,mm7
        paddw       mm6,mm2
        paddw       mm3,mm2
        paddw       mm2,mm2
        movq        mm7,x08
        paddw       mm0,mm2
        paddw       mm5,mm2
        paddw       mm4,mm2
        paddw       mm4,mm2
        
        paddw       mm6,mm7
        psrlw       mm6,4
        
        packuswb    mm6,mm6
        
        movq        mm7,lminmax2          //Output masked   v4
        movd        mm2,[esi+eax*2]
        pand        mm6,mm7
        pandn       mm7,mm2
        por         mm6,mm7
        movd        [esp+16],mm6    
        movq        mm6,mm0
        
        movq        mm7,mm3                   //calculate value that will be used as v9
        movd        mm1,[edi+eax*2]           
        movd        mm0,[edi+eax]
        movq        mm2,mm0
        movq        mm3,mm1
        psubusb     mm2,mm1
        psubusb     mm3,mm0
        por         mm2,mm3
        movq        mm3,mm7
        movq        mm7,iStepBroad
        pxor        mm7,x80
        pxor        mm2,x80
        pcmpgtb     mm2,mm7
        pand        mm0,mm2
        pandn       mm2,mm1
        por         mm0,mm2             //if (abs(v8-v9) < iStepSize) mm0 = v9 else mm0 = v8
        movq        mm1,mm3
        pxor        mm7,mm7
        
        movq        mm3,mm0             //v9
        punpcklbw   mm3,mm7
        paddw       mm1,mm3
        paddw       mm3,mm3
        paddw       mm6,mm3
        paddw       mm5,mm3
        paddw       mm5,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3
        paddw       mm4,mm3
        
        
        movd        mm0,[esi]           //v2
        punpcklbw   mm0,mm7
        paddw       mm6,mm0
        movd        mm0,[esp+8]
        movd        [esi],mm0    
        
        movd        mm0,[esi+eax]       //v3
        punpcklbw   mm0,mm7
        paddw       mm6,mm0
        paddw       mm5,mm0
        movd        mm0,[esp+12]
        movd        [esi+eax],mm0    
        
        movd        mm0,[esi+eax*2]     //v4
        punpcklbw   mm0,mm7
        paddw       mm5,mm0
        paddw       mm4,mm0
        paddw       mm0,mm0
        paddw       mm6,mm0
        movd        mm0,[esp+16]
        movd        [esi+eax*2],mm0    
        
        movd        mm0,[edi+ebx*2]     //v5
        punpcklbw   mm0,mm7
        paddw       mm4,mm0
        paddw       mm0,mm0
        movq        mm2,x08
        paddw       mm6,mm0
        paddw       mm5,mm0
        
        paddw       mm1,mm2
        psrlw       mm1,4
        
        packuswb    mm1,mm1
        
        movq        mm7,lminmax2          //Output masked v5
        movq        mm3,mm7
        movd        mm0,[edi+ebx*2]
        pand        mm1,mm3
        pandn       mm3,mm0
        por         mm1,mm3
        movd        [edi+ebx*2],mm1
        
        paddw       mm6,mm2
        psrlw       mm6,4
        
        packuswb    mm6,mm6
        
        movq        mm3,mm7             //Output masked v6
        movd        mm1,[edi+ebx]
        pand        mm6,mm3
        pandn       mm3,mm1
        por         mm6,mm3
        movd        [edi+ebx],mm6    
        
        paddw       mm5,mm2
        psrlw       mm5,4
        
        packuswb    mm5,mm5
        
        movq        mm3,mm7             //Output masked v7
        movd        mm1,[edi]
        pand        mm5,mm3
        pandn       mm3,mm1
        por         mm5,mm3
        movd        [edi],mm5    
        
        paddw       mm4,mm2
        psrlw       mm4,4
        
        packuswb    mm4,mm4
        
        movd        mm1,[edi+eax]        //Output masked v8
        pand        mm4,mm7
        pandn       mm7,mm1
        por         mm4,mm7
        movd        [edi+eax],mm4   
        
        ret
    }                           

} 
#pragma optimize ("",on)
#pragma warning (default:4799)
#endif // _WMV_TARGET_X86_

#if defined(_WMV_TARGET_X86_) || defined(_Embedded_x86)
Void_WMV DeblockMB_MMX (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{

    if (bDoTop) {
        // Filter the top Y, U and V edges.
        FilterHorizantalEdge(ppxliY,iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHorizantalEdge(ppxliU,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHorizantalEdge(ppxliV,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    }

    if (bDoBottomHalfEdge) {
        // Filter the bottom Y, U and V edges.
        FilterHalfHorizantalEdge(ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHalfHorizantalEdge(ppxliU + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHalfHorizantalEdge(ppxliV + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    } 

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterHorizantalEdge(ppxliY+ iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterVerticalEdge(ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterVerticalEdge(ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterVerticalEdge(ppxliU,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterVerticalEdge(ppxliV,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfVerticalEdge(ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterHalfVerticalEdge(ppxliU + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterHalfVerticalEdge(ppxliV + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    } 
}

Void_WMV DeblockMB_FASTEST_MMX (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{

    if (bDoTop) {
        // Filter the top Y, U and V edges.
        FilterHorizantalEdge(ppxliY,iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHorizantalEdge(ppxliU,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHorizantalEdge(ppxliV,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    }

    if (bDoBottomHalfEdge) {
        // Filter the bottom Y, U and V edges.
        FilterHalfHorizantalEdge(ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHalfHorizantalEdge(ppxliU + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHalfHorizantalEdge(ppxliV + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    } 

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterHorizantalEdge(ppxliY+ iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterVerticalEdge_FASTEST(ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterVerticalEdge_FASTEST(ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterVerticalEdge_FASTEST(ppxliU,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterVerticalEdge_FASTEST(ppxliV,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfVerticalEdge_FASTEST(ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterHalfVerticalEdge_FASTEST(ppxliU + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterHalfVerticalEdge_FASTEST(ppxliV + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    } 
}
#endif

#ifdef _WMV_TARGET_X86_

Void_WMV DeblockMB_KNI (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoLeft,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoTop,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
)
{
    if (bDoTop) {
        // Filter the top Y, U and V edges.
        FilterHorizantalEdge_KNI(ppxliY,iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHorizantalEdge_KNI(ppxliU,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHorizantalEdge_KNI(ppxliV,iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    }

    if (bDoBottomHalfEdge) {
        // Filter the bottom Y, U and V edges.
        FilterHalfHorizantalEdge(ppxliY + (iWidthPrevY * MB_SIZE),iWidthPrevY,1,MB_SIZE,iStepSize);
        FilterHalfHorizantalEdge(ppxliU + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
        FilterHalfHorizantalEdge(ppxliV + (iWidthPrevUV * BLOCK_SIZE),iWidthPrevUV,1,BLOCK_SIZE,iStepSize);
    } 

    if (bDoMiddle) {
        // Filter the middle horizontal Y edge
        FilterHorizantalEdge_KNI(ppxliY+ iWidthPrevY * 8,iWidthPrevY,1,MB_SIZE,iStepSize);

        // Filter the middle vertical Y edge
        FilterVerticalEdge_KNI(ppxliY + 8,1,iWidthPrevY,MB_SIZE,iStepSize);     
    }

    if (bDoLeft) {
        // Filter the left vertical Y, U and V edges.
        FilterVerticalEdge_KNI(ppxliY,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterVerticalEdge_KNI(ppxliU,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterVerticalEdge_KNI(ppxliV,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    }

    if (bDoRightHalfEdge) {
        // Filter the right vertical Y, U and V edges.
        FilterHalfVerticalEdge(ppxliY + MB_SIZE,1,iWidthPrevY,MB_SIZE,iStepSize);
        FilterHalfVerticalEdge(ppxliU + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
        FilterHalfVerticalEdge(ppxliV + BLOCK_SIZE,1,iWidthPrevUV,BLOCK_SIZE,iStepSize);
    } 
}

#endif
#endif
