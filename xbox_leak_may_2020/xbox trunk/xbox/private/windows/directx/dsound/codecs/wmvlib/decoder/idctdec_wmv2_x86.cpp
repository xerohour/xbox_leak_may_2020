#include "bldsetup.h"

#include "xplatform.h"
#include "typedef.hpp"
#include "idctdec_wmv2.h"
#include "cpudetect.h"
#include "wmvdec_api.h"


#ifdef _WMV_TARGET_X86_

#ifndef __DCT_COEF_INT_
#define __DCT_COEF_INT_

#define W1 2841L /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676L /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408L /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609L /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108L /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565L  /* 2048*sqrt(2)*cos(7*pi/16) */

#define W1a 1892L /* 2048*cos(1*pi/8) */
#define W2a 1448L /* 2048*cos(2*pi/8) */
#define W3a 784L  /* 2048*cos(3*pi/8) */

#define W1_W7 2276L //(W1 - W7)
#define W1pW7 3406L //(W1 + W7)
#define W3_W5 799L  //(W3 - W5)
#define W3pW5 4017L //(W3 + W5)
#define W2_W6 1568L //(W2 - W6)
#define W2pW6 3784L //(W2 + W6)

#endif // __DCT_COEF_INT_

// MMX Version
// Input: 32-bit
// Output: 8-bit
Void_WMV g_IDCTDecMMX_WMV2 (U8_WMV* piDst, I32_WMV iOffsetToNextRowForDCT, const PixelI32 __huge* rgiCoefRecon)
{
  	I32_WMV buf [72];
    PixelI32 *rgblk = (PixelI32 *) (((I32_WMV)buf+32)&0xFFFFFFE0);

    static const Int64 M1 = 0x023508E4023508E4;     //W7,W1-W7
    static const Int64 M2 = 0x0235F2B20235F2B2;     //W7,0-(W1+W7)
    static const Int64 M3 = 0x0968FCE10968FCE1;     //W3,0-(W3-W5)
    static const Int64 M4 = 0x0968F04F0968F04F;     //W3,0-(W3+W5)
    static const Int64 M5 = 0x0454F1380454F138;     //W6,0-(W2+W6)
    static const Int64 M6 = 0x0454062004540620;     //W6,W2-W6
    static const Int64 M7 = 0x0000008000000080;
    static const Int64 M8 = 0x00B55a8000B55a80;     //181,(181<<16)
    static const Int64 M9 = 0x0000023500000235;
    static const Int64 M10 = 0x000000B5000000B5;    //181,181
    static const Int64 low15mask = 0x00007fff00007fff;
    Int64 t64;

    _asm{
            mov         esi,rgiCoefRecon
            mov         edi,rgblk
            mov         ecx,-4
            xor         ebx,ebx
HLoop:
            lea         eax,[esi+ecx*8]

            pcmpeqb     mm0,mm0
            psrld       mm0,16

            movq        mm4,[eax+32+16*2]
            movq        mm1,mm4
            movq        mm5,[eax+32+112*2]
            paddd       mm1,mm5
            pslld       mm1,16
            pand        mm4,mm0
            pand        mm5,mm0
            por         mm5,mm1
            por         mm4,mm1
            movq        mm1,M1
            pmaddwd     mm4,mm1
            movq        mm1,M2
            pmaddwd     mm5,mm1
            
            movq        mm6,[eax+32+80*2]
            movq        mm1,mm6
            movq        mm7,[eax+32+48*2]
            paddd       mm1,mm7
            pslld       mm1,16
            pand        mm6,mm0
            pand        mm7,mm0
            por         mm6,mm1
            por         mm7,mm1
            movq        mm1,M3
            pmaddwd     mm6,mm1
            movq        mm1,M4
            pmaddwd     mm7,mm1

            movq        mm3,[eax+32+32*2]
            movq        mm1,mm3
            movq        mm2,[eax+32+96*2]
            paddd       mm1,mm2
            pslld       mm1,16
            pand        mm3,mm0
            pand        mm2,mm0
            por         mm3,mm1
            por         mm2,mm1
            movq        mm1,M6
            pmaddwd     mm3,mm1
            movq        mm1,M5
            pmaddwd     mm2,mm1

            movq        mm0,mm6
            paddd       mm6,mm4
            psubd       mm4,mm0

            movq        mm0,mm7
            paddd       mm7,mm5
            psubd       mm5,mm0

            movq        mm0,mm5
            paddd       mm5,mm4
            psubd       mm4,mm0

            movq        mm0,mm5
            pand        mm0,low15mask
            movq        mm1,M10
            pmaddwd     mm0,mm1
            psrld       mm5,15
            pmaddwd     mm5,mm1
            pslld       mm5,15
            paddd       mm5,mm0
            movq        mm0,M7
            paddd       mm5,mm0
            psrad       mm5,8

            movq        mm0,mm4
            pand        mm0,low15mask
            pmaddwd     mm0,mm1
            psrld       mm4,15
            pmaddwd     mm4,mm1
            pslld       mm4,15
            paddd       mm4,mm0
            movq        mm1,M7
            paddd       mm4,mm1
            psrad       mm4,8

            movq        mm0,[esi+ecx*8+32]
            lea         eax,[esi+ecx*8]
            movq        [edi+4],mm4
            pslld       mm0,16
            psrad       mm0,5
            movq        mm1,M7
            paddd       mm0,mm1
            movq        mm1,[eax+32+64*2]
            pslld       mm1,16
            psrad       mm1,5
            movq        mm4,mm0
            psubd       mm0,mm1
            paddd       mm1,mm4

            movq        mm4,mm1
            psubd       mm1,mm3
            paddd       mm3,mm4

            movq        mm4,mm0
            psubd       mm0,mm2
            paddd       mm2,mm4

            movq        mm4,mm3
            psubd       mm3,mm6
            paddd       mm6,mm4
            psrad       mm3,8
            psrad       mm6,8

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases
            
            movq        mm4,mm5
            pslld       mm4,17
            psrlw       mm4,1
            pslld       mm5,1
            psrld       mm5,16
            psllw       mm5,8
            por         mm5,mm4
            movq        mm4,M8
            pmaddwd     mm5,mm4
            movq        mm4,M7
            paddd       mm5,mm4
            psrad       mm5,8
#endif

            movq        mm4,mm2
            psubd       mm2,mm5
            paddd       mm5,mm4
            psrad       mm2,8
            psrad       mm5,8

            movq        mm4,mm6
            punpckldq   mm6,mm5
            punpckhdq   mm4,mm5
            packssdw    mm6,mm6
            
            movq        mm5,mm2
            punpckldq   mm2,mm3
            movd        [edi],mm6
            punpckhdq   mm5,mm3
            packssdw    mm5,mm5
            movd        [edi+12+16],mm5
            packssdw    mm2,mm4
            movq        mm4,[edi+4]
            movq        [edi+12],mm2

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases

            movq        mm2,mm4
            pslld       mm4,17
            psrlw       mm4,1
            pslld       mm2,1
            psrld       mm2,16
            psllw       mm2,8
            por         mm2,mm4
            movq        mm4,M8
            pmaddwd     mm4,mm2
            movq        mm2,M7
            paddd       mm4,mm2
            psrad       mm4,8
#endif
            
            movq        mm2,mm0
            psubd       mm0,mm4
            paddd       mm4,mm2
            psrad       mm0,8
            psrad       mm4,8

            movq        mm2,mm1
            psubd       mm1,mm7
            paddd       mm7,mm2
            psrad       mm1,8
            psrad       mm7,8

            movq        mm2,mm4
            punpckldq   mm4,mm7
            punpckhdq   mm2,mm7

            movq        mm7,mm1
            punpckldq   mm1,mm0
            punpckhdq   mm7,mm0
            packssdw    mm4,mm1
            movq        [edi+4],mm4
            packssdw    mm2,mm7
            inc         ecx
            movq        [edi+4+16],mm2
            lea         edi,[edi+32]
            jl          HLoop

            xor         ecx,ecx
            mov         esi,rgblk
            mov         edi,piDst
            mov         edx,iOffsetToNextRowForDCT
VLoop:

            pcmpeqb     mm1,mm1    //Generate constant 4
            psrld       mm1,31
            pslld       mm1,2

            movd        mm4,[esi+ecx*2+16]
            movd        mm5,[esi+ecx*2+112]
            movq        mm0,mm4
            paddw       mm0,mm5
            punpcklwd   mm4,mm0
            punpcklwd   mm5,mm0
            movq        mm0,M1
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            movq        mm0,M2
            pmaddwd     mm5,mm0
            paddd       mm5,mm1
            psrad       mm4,3
            psrad       mm5,3
        
            movd        mm6,[esi+ecx*2+80]
            movd        mm7,[esi+ecx*2+48]
            movq        mm0,mm6
            paddw       mm0,mm7
            punpcklwd   mm6,mm0
            punpcklwd   mm7,mm0
            movq        mm0,M3
            pmaddwd     mm6,mm0
            paddd       mm6,mm1
            movq        mm0,M4
            pmaddwd     mm7,mm0
            paddd       mm7,mm1
            psrad       mm6,3
            psrad       mm7,3

            movd        mm2,[esi+ecx*2+96]
            movd        mm3,[esi+ecx*2+32]
            movq        mm0,mm2
            paddw       mm0,mm3
            punpcklwd   mm2,mm0
            punpcklwd   mm3,mm0
            movq        mm0,M5
            pmaddwd     mm2,mm0
            paddd       mm2,mm1
            movq        mm0,M6
            pmaddwd     mm3,mm0
            paddd       mm3,mm1
            psrad       mm2,3
            psrad       mm3,3

            movq        mm0,mm4
            psubd       mm4,mm6
            paddd       mm6,mm0

            movq        mm0,mm5
            psubd       mm5,mm7
            paddd       mm7,mm0

            movq        mm0,mm4
            psubd       mm4,mm5
            paddd       mm5,mm0
 
            movq        mm0,mm5
            pand        mm0,low15mask
            movq        mm1,M10
            pmaddwd     mm0,mm1
            psrld       mm5,15
            pmaddwd     mm5,mm1
            pslld       mm5,15
            paddd       mm5,mm0
            movq        mm0,M7
            paddd       mm5,mm0
            psrad       mm5,8            

            movq        mm0,mm4
            pand        mm0,low15mask
            pmaddwd     mm0,mm1
            psrld       mm4,15
            pmaddwd     mm4,mm1
            pslld       mm4,15
            paddd       mm4,mm0
            movq        mm1,M7
            paddd       mm4,mm1
            psrad       mm4,8

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases
            
            pslld       mm1,5  //Generate constant 128
            movq        mm0,mm5
            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm5,1
            psrld       mm5,16
            psllw       mm5,8
            por         mm5,mm0
            movq        mm0,M8
            pmaddwd     mm5,mm0
            movq        mm0,mm4
            paddd       mm5,mm1
            psrad       mm5,8

            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm4,1
            psrld       mm4,16
            psllw       mm4,8
            por         mm4,mm0
            movq        mm0,M8
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            psrad       mm4,8
#endif

            movq        t64,mm4

            pslld       mm1,6   //Generate 8192

            movd        mm0,[esi+ecx*2]
            punpcklwd   mm0,mm0
            pslld       mm0,16
            psrad       mm0,8
            paddd       mm0,mm1
            movd        mm4,[esi+ecx*2+64]
            punpcklwd   mm4,mm4
            pslld       mm4,16
            psrad       mm4,8
            movq        mm1,mm0
            psubd       mm0,mm4
            paddd       mm4,mm1

            movq        mm1,mm4
            psubd       mm4,mm3
            paddd       mm3,mm1

            movq        mm1,mm0
            psubd       mm0,mm2
            paddd       mm2,mm1

            movq        mm1,mm3
            psubd       mm3,mm6
            psrad       mm3,14
            paddd       mm6,mm1
            psrad       mm6,14

            packssdw    mm6,mm6
            packuswb    mm6,mm6
            movd        eax,mm6
            mov         [edi],ax

            lea         ebx,[edi+edx*8]
            sub         ebx,edx
            packssdw    mm3,mm3
            packuswb    mm3,mm3
            movd        eax,mm3
            mov         [ebx],ax

            movq        mm6,t64

            movq        mm1,mm4
            psubd       mm4,mm7
            psrad       mm4,14
            paddd       mm7,mm1
            psrad       mm7,14

            packssdw    mm4,mm4
            packuswb    mm4,mm4
            movd        eax,mm4
            mov         [edi+edx*4],ax
            lea         ebx,[edi+edx*2]
            packssdw    mm7,mm7
            packuswb    mm7,mm7
            movd        eax,mm7
            mov         [ebx+edx],ax

            movq        mm1,mm2
            psubd       mm2,mm5
            psrad       mm2,14
            paddd       mm5,mm1
            psrad       mm5,14

            lea         ebx,[ebx+edx*2]
            packssdw    mm2,mm2
            packuswb    mm2,mm2
            movd        eax,mm2
            mov         [ebx+edx*2],ax
            
            packssdw    mm5,mm5
            packuswb    mm5,mm5
            movd        eax,mm5
            mov         [edi+edx],ax
  
            movq        mm3,mm0
            psubd       mm0,mm6
            psrad       mm0,14
            paddd       mm6,mm3
            psrad       mm6,14
            packssdw    mm0,mm0
            packuswb    mm0,mm0
            movd        eax,mm0
            mov         [ebx+edx],ax
            packssdw    mm6,mm6
            packuswb    mm6,mm6
            movd        eax,mm6
            mov         [edi+edx*2],ax

            add         edi,2
            add         ecx,2
            cmp         ecx,8
            jnz         VLoop
            emms
        }    
}

// MMX Version
// Input:  16-bit
// Output: 16-bit
Void_WMV g_8x4IDCTDecMMX_WMV2 (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalf)
{
    const PixelI __huge* rgiCoefRecon =  (PixelI*)rgiCoefReconBuf;
    PixelI __huge* piDst = piDstBuf->i16 + (iHalf*32);
	PixelI * blk = piDst;
	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
	I32_WMV i;

    for (i = 0; i < 4; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE)
    {
        x4 = rgiCoefRecon [1];		
        x3 = rgiCoefRecon [2];
	    x7 = rgiCoefRecon [3];
	    x1 = (I32_WMV)rgiCoefRecon [4] << 11;
	    x6 = rgiCoefRecon [5];
	    x2 = rgiCoefRecon [6];		
	    x5 = rgiCoefRecon [7];

        *blk = rgiCoefRecon [0];

		x0 = (I32_WMV)((I32_WMV)blk [0] << 11) + 128L; /* for proper rounding in the fourth stage */
		/* first stage */
		x8 = W7 * (x4 + x5);
		x4 = x8 + (W1 - W7) * x4;
		x5 = x8 - (W1 + W7) * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - (W3 - W5) * x6;
		x7 = x8 - (W3 + W5) * x7;

		/* second stage */
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - (W2 + W6) * x2;
		x3 = x1 + (W2 - W6) * x3;
		x1 = x4 + x6;
		x4 -= x6;
		x6 = x5 + x7;
		x5 -= x7;

		/* third stage */
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		x2 = (I32_WMV)(181L * (x4 + x5) + 128L) >> 8;
		x4 = (I32_WMV)(181L * (x4 - x5) + 128L) >> 8;

		/* fourth stage */
		blk [0] = (PixelI) ((x7 + x1) >> 8);
		blk [1] = (PixelI) ((x3 + x2) >> 8);
		blk [2] = (PixelI) ((x0 + x4) >> 8);
		blk [3] = (PixelI) ((x8 + x6) >> 8);
		blk [4] = (PixelI) ((x8 - x6) >> 8);
		blk [5] = (PixelI) ((x0 - x4) >> 8);
		blk [6] = (PixelI) ((x3 - x2) >> 8);
		blk [7] = (PixelI) ((x7 - x1) >> 8);
    }

    {
        PixelI* blk0 = piDst;
        PixelI* blk1 = blk0 + iOffsetToNextRowForDCT;
        PixelI* blk2 = blk1 + iOffsetToNextRowForDCT;
        PixelI* blk3 = blk2 + iOffsetToNextRowForDCT;
        
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            x4 = blk0[i];
            x5 = blk1[i];
            x6 = blk2[i];
            x7 = blk3[i];
            
            x0 = (x4 + x6)*W2a;
            x1 = (x4 - x6)*W2a;
            x2 = x5*W1a + x7*W3a;
            x3 = x5*W3a - x7*W1a;
            
            blk0[i] = (PixelI)((x0 + x2 + 32768L)>>16);
            blk1[i] = (PixelI)((x1 + x3 + 32768L)>>16);
            blk2[i] = (PixelI)((x1 - x3 + 32768L)>>16);
            blk3[i] = (PixelI)((x0 - x2 + 32768L)>>16);
        }
    }
}

// MMX Version
// Input:  16-bit
// Output: 16-bit
Void_WMV g_4x8IDCTDecMMX_WMV2 (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalf)
{
    const PixelI __huge* rgiCoefRecon = (PixelI*)rgiCoefReconBuf;
	PixelI __huge* piDst = piDstBuf->i16 + (iHalf*4);
	PixelI * blk = piDst;
	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
	I32_WMV i;

    for (i = 0; i < BLOCK_SIZE; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += 4){

        x4 = rgiCoefRecon[0];
        x5 = rgiCoefRecon[1];
        x6 = rgiCoefRecon[2];
        x7 = rgiCoefRecon[3];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk[0] = (PixelI)((x0 + x2 + 64)>>7);
        blk[1] = (PixelI)((x1 + x3 + 64)>>7);
        blk[2] = (PixelI)((x1 - x3 + 64)>>7);
        blk[3] = (PixelI)((x0 - x2 + 64)>>7);
    }

    {
        PixelI* blk0 = piDst;
        PixelI* blk1 = blk0 + iOffsetToNextRowForDCT;
        PixelI* blk2 = blk1 + iOffsetToNextRowForDCT;
        PixelI* blk3 = blk2 + iOffsetToNextRowForDCT;
        PixelI* blk4 = blk3 + iOffsetToNextRowForDCT;
        PixelI* blk5 = blk4 + iOffsetToNextRowForDCT;
        PixelI* blk6 = blk5 + iOffsetToNextRowForDCT;
        PixelI* blk7 = blk6 + iOffsetToNextRowForDCT;
        
        for (i = 0; i < 4; i++)
        {
            x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
            x1 = (I32_WMV)blk4[i] << 8;
            x2 = blk6[i];
            x3 = blk2[i];
            x4 = blk1[i];
            x5 = blk7[i];
            x6 = blk5[i];
            x7 = blk3[i];
            
            /* first stage */
            x8 = W7 * (x4 + x5) + 4;
            x4 = (x8 + (W1 - W7) * x4) >> 3;
            x5 = (x8 - (W1 + W7) * x5) >> 3;
            x8 = W3 * (x6 + x7) + 4;
            x6 = (x8 - (W3 - W5) * x6) >> 3;
            x7 = (x8 - (W3 + W5) * x7) >> 3;
            
            /* second stage */
            x8 = x0 + x1;
            x0 -= x1;
            x1 = W6 * (x3 + x2) + 4;
            x2 = (x1 - (W2 + W6) * x2) >> 3;
            x3 = (x1 + (W2 - W6) * x3) >> 3;
            x1 = x4 + x6;
            x4 -= x6;
            x6 = x5 + x7;
            x5 -= x7;
            
            /* third stage */
            x7 = x8 + x3;
            x8 -= x3;
            x3 = x0 + x2;
            x0 -= x2;
            x2 = (I32_WMV) (181L * (x4 + x5) + 128L) >> 8;
            x4 = (I32_WMV) (181L * (x4 - x5) + 128L) >> 8;
            
            /* fourth stage */
            blk0[i] = (PixelI) ((x7 + x1) >> 14);
            blk1[i] = (PixelI) ((x3 + x2) >> 14);
            blk2[i] = (PixelI) ((x0 + x4) >> 14);
            blk3[i] = (PixelI) ((x8 + x6) >> 14);
            blk4[i] = (PixelI) ((x8 - x6) >> 14);
            blk5[i] = (PixelI) ((x0 - x4) >> 14);
            blk6[i] = (PixelI) ((x3 - x2) >> 14);
            blk7[i] = (PixelI) ((x7 - x1) >> 14);
        }
    }
}

// MMX Version
// Input:  16-bit
// Output: 16-bit
Void_WMV g_IDCTDecMMX_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
    static const Int64 M1 = 0x023508E4023508E4;     //W7,W1-W7
    static const Int64 M2 = 0x0235F2B20235F2B2;     //W7,0-(W1+W7)
    static const Int64 M3 = 0x0968FCE10968FCE1;     //W3,0-(W3-W5)
    static const Int64 M4 = 0x0968F04F0968F04F;     //W3,0-(W3+W5)
    static const Int64 M5 = 0x0454F1380454F138;     //W6,0-(W2+W6)
    static const Int64 M6 = 0x0454062004540620;     //W6,W2-W6
    static const Int64 M7 = 0x0000008000000080;
    static const Int64 M8 = 0x00B55a8000B55a80;     //181,(181<<16)
    static const Int64 M9 = 0x0000023500000235;
    static const Int64 M10 = 0x000000B5000000B5;    //181,181
    static const Int64 low15mask =  0x00007fff00007fff;
    I16_WMV buf[96];
    __int64 t64;
    I32_WMV iDCTFlag = iDCTHorzFlags;

    PixelI *rgiCoefRecon = (PixelI*)piSrc;
    PixelI *blk          = buf; 

        _asm{
            mov         esi,rgiCoefRecon
            mov         edi,blk
            mov         ecx,-4
            xor         ebx,ebx
HLoop:
            mov         eax,iDCTFlag
            shr         iDCTFlag,2
            and         eax,3
            test        eax,eax
            jnz         Calculate
            
#if 0            
            lea         eax,[esi+ecx*4]
            movq        mm0,[eax+16+64]
            movq        mm4,[eax+16+16]
            por         mm0,mm4
            movq        mm5,[eax+16+112]
            por         mm0,mm5
            movq        mm6,[eax+16+80]
            por         mm0,mm6
            movq        mm7,[eax+16+48]
            por         mm0,mm7
            movq        mm3,[eax+16+32]
            por         mm0,mm3
            movq        mm2,[eax+16+96]
            por         mm0,mm2

            movd        eax,mm0
            test        eax,eax
            jnz         Calculate
#endif

            movd        mm2,[esi+ecx*4+16]
            psllw       mm2,3
            punpcklwd   mm2,mm2
            movq        mm1,mm2
            punpckldq   mm2,mm2
            movq        [edi]   ,mm2
            punpckhdq   mm1,mm1
            movq        [edi+8] ,mm2
            psrlq       mm0,32
            movq        [edi+16],mm1
//            movd        eax,mm0
            inc         ecx
            movq        [edi+24],mm1
            lea         edi,[edi+32]
            jnl         EndHLoop
            jmp         HLoop

#if 0            
            test        eax,eax
            jnz         CalcSpecial

            movd        mm2,[esi+ecx*4+16]
            psllw       mm2,3
            punpcklwd   mm2,mm2
            movq        mm1,mm2
            punpckldq   mm2,mm2
            movq        [edi]   ,mm2
            punpckhdq   mm1,mm1
            movq        [edi+8] ,mm2
            inc         ecx
            movq        [edi+16],mm1
            movq        [edi+24],mm1
            lea         edi,[edi+32]
            jnl         EndHLoop
            jmp         HLoop

CalcSpecial:
            psrlq       mm4,32
            psrlq       mm5,32
            movd        mm2,[esi+ecx*4+16+96]
            psrlq       mm6,32
            psrlq       mm7,32
            psrlq       mm3,32
#endif
            
Calculate:  
            lea         eax,[esi+ecx*4]
//            movq        mm0,[eax+16+64]
            movq        mm4,[eax+16+16]
            movq        mm5,[eax+16+112]
            movq        mm6,[eax+16+80]
            movq        mm7,[eax+16+48]
            movq        mm3,[eax+16+32]
            movq        mm2,[eax+16+96]

            mov         ebx,esi
   
            movq        mm0,mm4
            paddw       mm0,mm5
            punpcklwd   mm4,mm0
            punpcklwd   mm5,mm0
            movq        mm1,M1
            movq        mm0,mm6
            pmaddwd     mm4,mm1
            movq        mm1,M2
            paddw       mm0,mm7
         
            punpcklwd   mm6,mm0
            pmaddwd     mm5,mm1
            punpcklwd   mm7,mm0
            movq        mm1,M3
            movq        mm0,mm3
            pmaddwd     mm6,mm1
            movq        mm1,M4
            paddw       mm0,mm2
            
            punpcklwd   mm3,mm0
            pmaddwd     mm7,mm1
            punpcklwd   mm2,mm0
            movq        mm1,M6
            pmaddwd     mm3,mm1
            movq        mm0,mm6
            movq        mm1,M5
            paddd       mm6,mm4
            pmaddwd     mm2,mm1

            psubd       mm4,mm0

            movq        mm0,mm7
            paddd       mm7,mm5
            psubd       mm5,mm0

            movq        mm0,mm5
            paddd       mm5,mm4
            psubd       mm4,mm0

            movq        mm0,mm5
            pand        mm0,low15mask
            movq        mm1,M10
            pmaddwd     mm0,mm1
            psrld       mm5,15
            pmaddwd     mm5,mm1
            pslld       mm5,15
            paddd       mm5,mm0
            movq        mm0,M7
            paddd       mm5,mm0
            psrad       mm5,8

            movq        mm0,mm4
            pand        mm0,low15mask
            pmaddwd     mm0,mm1
            psrld       mm4,15
            pmaddwd     mm4,mm1
            pslld       mm4,15
            paddd       mm4,mm0
            movq        mm1,M7
            paddd       mm4,mm1
            psrad       mm4,8

            movd        mm0,[esi+ecx*4+16]
            lea         eax,[esi+ecx*4]
            movq        [edi+4],mm4
            punpcklwd   mm0,mm0
            pslld       mm0,16
            psrad       mm0,5
            movq        mm1,M7
            paddd       mm0,mm1
            movd        mm1,[eax+16+64]
            punpcklwd   mm1,mm1
            pslld       mm1,16
            psrad       mm1,5
            movq        mm4,mm0
            psubd       mm0,mm1
            paddd       mm1,mm4

            movq        mm4,mm1
            psubd       mm1,mm3
            paddd       mm3,mm4

            movq        mm4,mm0
            psubd       mm0,mm2
            paddd       mm2,mm4

            movq        mm4,mm3
            psubd       mm3,mm6
            paddd       mm6,mm4
            psrad       mm3,8
            psrad       mm6,8

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases
            movq        mm4,mm5
            pslld       mm4,17
            psrlw       mm4,1
            pslld       mm5,1
            psrld       mm5,16
            psllw       mm5,8
            por         mm5,mm4
            movq        mm4,M8
            pmaddwd     mm5,mm4
            movq        mm4,M7
            paddd       mm5,mm4
            psrad       mm5,8
#endif

            movq        mm4,mm2
            psubd       mm2,mm5
            paddd       mm5,mm4
            psrad       mm2,8
            psrad       mm5,8

            movq        mm4,mm6
            punpckldq   mm6,mm5
            punpckhdq   mm4,mm5
            packssdw    mm6,mm6
            
            movq        mm5,mm2
            punpckldq   mm2,mm3
            movd        [edi],mm6
            punpckhdq   mm5,mm3
            packssdw    mm5,mm5
            movd        [edi+12+16],mm5
            packssdw    mm2,mm4
            movq        mm4,[edi+4]
            movq        [edi+12],mm2

#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases
            movq        mm2,mm4
            pslld       mm4,17
            psrlw       mm4,1
            pslld       mm2,1
            psrld       mm2,16
            psllw       mm2,8
            por         mm2,mm4
            movq        mm4,M8
            pmaddwd     mm4,mm2
            movq        mm2,M7
            paddd       mm4,mm2
            psrad       mm4,8
#endif
            
            movq        mm2,mm0
            psubd       mm0,mm4
            paddd       mm4,mm2
            psrad       mm0,8
            psrad       mm4,8

            movq        mm2,mm1
            psubd       mm1,mm7
            paddd       mm7,mm2
            psrad       mm1,8
            psrad       mm7,8

            movq        mm2,mm4
            punpckldq   mm4,mm7
            punpckhdq   mm2,mm7

            movq        mm7,mm1
            punpckldq   mm1,mm0
            punpckhdq   mm7,mm0
            packssdw    mm4,mm1
            movq        [edi+4],mm4
            packssdw    mm2,mm7
            inc         ecx
            movq        [edi+4+16],mm2
            lea         edi,[edi+32]
            jl          HLoop
EndHLoop:
            test        ebx,ebx
            jz          VFast

            xor         ecx,ecx
VLoop:                
            mov         esi,blk
            mov         edi,piDst

            lea         edi,[edi+ecx*2]
            mov         edx,iOffsetToNextRowForDCT
            shl         edx,1

            movd        mm4,[esi+ecx*2+16]
            pcmpeqb     mm1,mm1    //Generate constant 4
            psrld       mm1,31
            movd        mm5,[esi+ecx*2+112]
            pslld       mm1,2
            
            movq        mm0,mm4
            paddw       mm0,mm5
            punpcklwd   mm4,mm0
            punpcklwd   mm5,mm0
            movq        mm0,M1
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            movq        mm0,M2
            pmaddwd     mm5,mm0
            movd        mm6,[esi+ecx*2+80]
            paddd       mm5,mm1
            psrad       mm4,3
            movd        mm7,[esi+ecx*2+48]
            psrad       mm5,3
      
            movq        mm0,mm6
            paddw       mm0,mm7
            punpcklwd   mm6,mm0
            punpcklwd   mm7,mm0
            movq        mm0,M3
            pmaddwd     mm6,mm0
            paddd       mm6,mm1
            movq        mm0,M4
            pmaddwd     mm7,mm0
            movd        mm2,[esi+ecx*2+96]
            paddd       mm7,mm1
            psrad       mm6,3
            movd        mm3,[esi+ecx*2+32]
            psrad       mm7,3

            movq        mm0,mm2
            paddw       mm0,mm3
            punpcklwd   mm2,mm0
            punpcklwd   mm3,mm0
            movq        mm0,M5
            pmaddwd     mm2,mm0
            paddd       mm2,mm1
            movq        mm0,M6
            pmaddwd     mm3,mm0
            paddd       mm3,mm1
            psrad       mm2,3
            psrad       mm3,3

            movq        mm0,mm4
            psubd       mm4,mm6
            paddd       mm6,mm0

            movq        mm0,mm5
            psubd       mm5,mm7
            paddd       mm7,mm0

            movq        mm0,mm4
            psubd       mm4,mm5
            paddd       mm5,mm0

            movq        mm0,mm5
            pand        mm0,low15mask
            movq        mm1,M10
            pmaddwd     mm0,mm1
            psrld       mm5,15
            pmaddwd     mm5,mm1
            pslld       mm5,15
            paddd       mm5,mm0
            movq        mm0,M7
            paddd       mm5,mm0
            psrad       mm5,8            

            movq        mm0,mm4
            pand        mm0,low15mask
            pmaddwd     mm0,mm1
            psrld       mm4,15
            pmaddwd     mm4,mm1
            pslld       mm4,15
            paddd       mm4,mm0
            movq        mm1,M7
            paddd       mm4,mm1
            psrad       mm4,8
            
#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases

            pslld       mm1,5  //Generate constant 128
            movq        mm0,mm5
            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm5,1
            psrld       mm5,16
            psllw       mm5,8
            por         mm5,mm0
            movq        mm0,M8
            pmaddwd     mm5,mm0
            movq        mm0,mm4
            paddd       mm5,mm1
            psrad       mm5,8

            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm4,1
            psrld       mm4,16
            psllw       mm4,8
            por         mm4,mm0
            movq        mm0,M8
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            psrad       mm4,8
#endif           
           
            movq        t64,mm4

            pslld       mm1,6   //Generate 8192

            movd        mm0,[esi+ecx*2]
            punpcklwd   mm0,mm0
            pslld       mm0,16
            psrad       mm0,8
            paddd       mm0,mm1
            movd        mm4,[esi+ecx*2+64]
            punpcklwd   mm4,mm4
            pslld       mm4,16
            psrad       mm4,8
            movq        mm1,mm0
            psubd       mm0,mm4
            paddd       mm4,mm1

            movq        mm1,mm4
            psubd       mm4,mm3
            paddd       mm3,mm1

            movq        mm1,mm0
            psubd       mm0,mm2
            paddd       mm2,mm1

            movq        mm1,mm3
            psubd       mm3,mm6
            psrad       mm3,14
            paddd       mm6,mm1
            psrad       mm6,14

            packssdw    mm6,mm6
            movd        [edi],mm6                        

            lea         ebx,[edi+edx*8]
            sub         ebx,edx

            packssdw    mm3,mm3
            movd        [ebx],mm3                   

            movq        mm6,t64

            movq        mm1,mm4
            psubd       mm4,mm7
            psrad       mm4,14
            paddd       mm7,mm1
            psrad       mm7,14

            packssdw    mm4,mm4
            movd        [edi+edx*4],mm4            

            lea         ebx,[edi+edx*2]
            movq        mm1,mm2
            
            packssdw    mm7,mm7
            movd        [ebx+edx],mm7             
            
            psubd       mm2,mm5
            psrad       mm2,14
            paddd       mm5,mm1
            psrad       mm5,14

            lea         ebx,[ebx+edx*2]
            packssdw    mm2,mm2
            movd        [ebx+edx*2],mm2
  
            movq        mm3,mm0
            psubd       mm0,mm6

            packssdw    mm5,mm5
            movd        [edi+edx],mm5           
            
            psrad       mm0,14
            paddd       mm6,mm3
            psrad       mm6,14

            packssdw    mm0,mm0
            movd        [ebx+edx],mm0       

            add         ecx,2
            cmp         ecx,8

            packssdw    mm6,mm6
            movd        [edi+edx*2],mm6     

            jnz         VLoop
            jmp         End

VFast:
            mov         esi,blk
            mov         edi,piDst

            mov         edx,iOffsetToNextRowForDCT
            shl         edx,1

            movd        mm4,[esi+16]
            pcmpeqb     mm1,mm1    //Generate constant 4
            psrld       mm1,31
            movd        mm5,[esi+112]
            pslld       mm1,2
            
            movq        mm0,mm4
            paddw       mm0,mm5
            punpcklwd   mm4,mm0
            punpcklwd   mm5,mm0
            movq        mm0,M1
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            movq        mm0,M2
            pmaddwd     mm5,mm0
            movd        mm6,[esi+80]
            paddd       mm5,mm1
            psrad       mm4,3
            movd        mm7,[esi+48]
            psrad       mm5,3
      
            movq        mm0,mm6
            paddw       mm0,mm7
            punpcklwd   mm6,mm0
            punpcklwd   mm7,mm0
            movq        mm0,M3
            pmaddwd     mm6,mm0
            paddd       mm6,mm1
            movq        mm0,M4
            pmaddwd     mm7,mm0
            movd        mm2,[esi+96]
            paddd       mm7,mm1
            psrad       mm6,3
            movd        mm3,[esi+32]
            psrad       mm7,3

            movq        mm0,mm2
            paddw       mm0,mm3
            punpcklwd   mm2,mm0
            punpcklwd   mm3,mm0
            movq        mm0,M5
            pmaddwd     mm2,mm0
            paddd       mm2,mm1
            movq        mm0,M6
            pmaddwd     mm3,mm0
            paddd       mm3,mm1
            psrad       mm2,3
            psrad       mm3,3

            movq        mm0,mm4
            psubd       mm4,mm6
            paddd       mm6,mm0

            movq        mm0,mm5
            psubd       mm5,mm7
            paddd       mm7,mm0

            movq        mm0,mm4
            psubd       mm4,mm5
            paddd       mm5,mm0

            movq        mm0,mm5
            pand        mm0,low15mask
            movq        mm1,M10
            pmaddwd     mm0,mm1
            psrld       mm5,15
            pmaddwd     mm5,mm1
            pslld       mm5,15
            paddd       mm5,mm0
            movq        mm0,M7
            paddd       mm5,mm0
            psrad       mm5,8            

            movq        mm0,mm4
            pand        mm0,low15mask
            pmaddwd     mm0,mm1
            psrld       mm4,15
            pmaddwd     mm4,mm1
            pslld       mm4,15
            paddd       mm4,mm0
            movq        mm1,M7
            paddd       mm4,mm1
            psrad       mm4,8
            
#if 0 // Old way of generating (181*(x4 + x5) + 128)>>8. Overflowed in certain cases

            pslld       mm1,5  //Generate constant 128
            movq        mm0,mm5
            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm5,1
            psrld       mm5,16
            psllw       mm5,8
            por         mm5,mm0
            movq        mm0,M8
            pmaddwd     mm5,mm0
            movq        mm0,mm4
            paddd       mm5,mm1
            psrad       mm5,8

            pslld       mm0,17
            psrlw       mm0,1
            pslld       mm4,1
            psrld       mm4,16
            psllw       mm4,8
            por         mm4,mm0
            movq        mm0,M8
            pmaddwd     mm4,mm0
            paddd       mm4,mm1
            psrad       mm4,8
#endif
            
            movq        t64,mm4

            pslld       mm1,6   //Generate 8192

            movd        mm0,[esi]
            punpcklwd   mm0,mm0
            pslld       mm0,16
            psrad       mm0,8
            paddd       mm0,mm1
            movd        mm4,[esi+64]
            punpcklwd   mm4,mm4
            pslld       mm4,16
            psrad       mm4,8
            movq        mm1,mm0
            psubd       mm0,mm4
            paddd       mm4,mm1

            movq        mm1,mm4
            psubd       mm4,mm3
            paddd       mm3,mm1

            movq        mm1,mm0
            psubd       mm0,mm2
            paddd       mm2,mm1

            movq        mm1,mm3
            psubd       mm3,mm6
            psrad       mm3,14
            paddd       mm6,mm1
            psrad       mm6,14

            packssdw    mm6,mm6    
            movq        [edi],mm6

            lea         ebx,[edi+edx*8]
            sub         ebx,edx
            movq        [edi+8],mm6
            packssdw    mm3,mm3
            movq        [ebx],mm3

            movq        mm6,t64

            movq        mm1,mm4
            movq        [ebx+8],mm3
            psubd       mm4,mm7
            psrad       mm4,14
            paddd       mm7,mm1
            psrad       mm7,14

            packssdw    mm4,mm4
            movq        [edi+edx*4],mm4

            lea         ebx,[edi+edx*2]

            movq        mm1,mm2
            movq        [edi+edx*4+8],mm4
            psubd       mm2,mm5
            packssdw    mm7,mm7
            movq        [ebx+edx],mm7

            psrad       mm2,14
            paddd       mm5,mm1
            movq        [ebx+edx+8],mm7
            psrad       mm5,14

            lea         ebx,[ebx+edx*2]
            packssdw    mm2,mm2
            movq        [ebx+edx*2],mm2
  
            packssdw    mm5,mm5
            movq        [ebx+edx*2+8],mm2

            movq        mm3,mm0
            psubd       mm0,mm6
            movq        [edi+edx],mm5
            psrad       mm0,14
            paddd       mm6,mm3
            movq        [edi+edx+8],mm5
            psrad       mm6,14

            packssdw    mm0,mm0
            movq        [ebx+edx],mm0
            movq        [ebx+edx+8],mm0

            packssdw    mm6,mm6
            movq        [edi+edx*2],mm6
            movq        [edi+edx*2+8],mm6


End:        emms

            } 
}
#endif // _WMV_TARGET_X86_
