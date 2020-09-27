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
#include "wmvdec_function.h"
#include <stdlib.h>
#include "motioncomp_wmv.h"
#include "tables_wmv.h"
#if defined(macintosh) && defined(_MAC_VEC_OPT)
#include "motioncomp_altivec.h"
#endif

#ifdef _WMV_TARGET_X86_
#include "opcodes.h"
#endif 

#ifndef OPT_REPEATPAD_ARM 
Void_WMV g_RepeatRef0Y (
	U8_WMV* ppxlcRef0Y,
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
	const U8_WMV* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
	const U8_WMV* ppxliOldRight = ppxliOldLeft + iWidthY - 1;
	const U8_WMV* ppxliOldTopLn = ppxliOldLeft - EXPANDY_REFVOP;
	U8_WMV* ppxliNew = (U8_WMV*) ppxliOldTopLn;
	U8_WMV* ppxliNewPlusWidth;
    I32_WMV iResidue = iWidthPrevY - iWidthYPlusExp;
	
	CoordI y;
#ifndef HITACHI	
	for (y = iStart; y < iEnd; y++) { // x-direction interpolation
		U32_WMV uipadValueLeft;
		U32_WMV uipadValueRight;
        I32_WMV x;
		uipadValueLeft = *ppxliOldLeft;	
		uipadValueRight = *ppxliOldRight;
		uipadValueLeft |= (uipadValueLeft << 8);
		uipadValueRight |= (uipadValueRight << 8);
		uipadValueLeft |= (uipadValueLeft << 16);
		uipadValueRight |= (uipadValueRight << 16);
//		uipadValueLeft |= (uipadValueLeft << 32);
//		uipadValueRight |= (uipadValueRight << 32);
		ppxliNewPlusWidth = ppxliNew + iWidthYPlusExp;
		for (x = 0; x < EXPANDY_REFVOP; x += 4) {
			*(U32_WMV *)(ppxliNew + x) = uipadValueLeft;
			*(U32_WMV *)(ppxliNewPlusWidth + x) = uipadValueRight;
		}
		ppxliNew += iWidthYPlusExp;
		ppxliNew += iResidue;
		ppxliOldLeft += iWidthPrevY;
		ppxliOldRight += iWidthPrevY;
	}
#else
	for (y = iStart; y < iEnd; y++) { // x-direction interpolation
		U64 uipadValueLeft;
		U64 uipadValueRight;
        I32_WMV x;
		uipadValueLeft.dwLo = (DWORD)*ppxliOldLeft;	
		uipadValueRight.dwLo = (DWORD)*ppxliOldRight;
		uipadValueLeft.dwLo |= (uipadValueLeft.dwLo << 8);
		uipadValueRight.dwLo |= (uipadValueRight.dwLo << 8);
		uipadValueLeft.dwLo |= (uipadValueLeft.dwLo << 16);
		uipadValueRight.dwLo |= (uipadValueRight.dwLo << 16);
		uipadValueLeft.dwHi = (uipadValueLeft.dwLo );
		uipadValueRight.dwHi = (uipadValueRight.dwLo );
		ppxliNewPlusWidth = ppxliNew + iWidthYPlusExp;
		for (x = 0; x < EXPANDY_REFVOP; x += 8) {
			*(U64 *)(ppxliNew + x) = uipadValueLeft;
			*(U64 *)(ppxliNewPlusWidth + x) = uipadValueRight;
		}
		ppxliNew += iWidthYPlusExp;
		ppxliNew += iResidue;
		ppxliOldLeft += iWidthPrevY;
		ppxliOldRight += iWidthPrevY;
	}
#endif
    {
	    I32_WMV iWidthPrevYDiv8 = iWidthPrevY >> 3;
	    U8_WMV* ppxliSrc;
	    U8_WMV* ppxliDst;
	    if (fTop) {
		    U8_WMV* ppxliLeftTop = ppxlcRef0Y;
		    for (y = 0; y < EXPANDY_REFVOP; y++) {
                I32_WMV x;
			    //memcpy (ppxliLeftTop, ppxliOldTopLn, iWidthPrevY);
			    ppxliSrc = (U8_WMV*) ppxliOldTopLn;
			    ppxliDst = (U8_WMV*) ppxliLeftTop;
			    for (x = 0; x < iWidthPrevYDiv8; x++) {
				    *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
				    ppxliSrc += 8;
				    ppxliDst += 8;
			    }
			    ppxliLeftTop += iWidthPrevY;
		    }
	    }
	    if (fBottom) {
		    const U8_WMV* ppxliOldBotLn = ppxliNew - iWidthPrevY;
            I32_WMV iHeightYPadded = (iEnd + 15) & ~15;
            I32_WMV iBotExtend = iHeightYPadded - iEnd + EXPANDY_REFVOP;
		    for (y = 0; y < iBotExtend; y++) {
                I32_WMV x;
			    //memcpy (ppxliNew, ppxliOldBotLn, iWidthPrevY);
			    ppxliSrc = (U8_WMV*) ppxliOldBotLn;
			    ppxliDst = (U8_WMV*) ppxliNew;
			    for (x = 0; x < iWidthPrevYDiv8; x++) {
				    *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
				    ppxliSrc += 8;
				    ppxliDst += 8;
			    }
			    ppxliNew += iWidthPrevY;
		    }
	    }
    }
}

Void_WMV g_RepeatRef0UV (
	U8_WMV* ppxlcRef0U,
	U8_WMV* ppxlcRef0V,
	CoordI  iStart, CoordI iEnd,
	I32_WMV     iOldLeftOffet,
	Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV
)
{
	const U8_WMV* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
	const U8_WMV* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
	const U8_WMV* ppxliOldRightU = ppxliOldLeftU + iWidthUV - 1;
	const U8_WMV* ppxliOldRightV = ppxliOldLeftV + iWidthUV - 1;
	const U8_WMV* ppxliOldTopLnU = ppxliOldLeftU - EXPANDUV_REFVOP;
	const U8_WMV* ppxliOldTopLnV = ppxliOldLeftV - EXPANDUV_REFVOP;
	U8_WMV* ppxliNewU = (U8_WMV*) ppxliOldTopLnU;
	U8_WMV* ppxliNewV = (U8_WMV*) ppxliOldTopLnV;
	U8_WMV* ppxliNewUPlusWidth;
	U8_WMV* ppxliNewVPlusWidth;
    I32_WMV iResidue = iWidthPrevUV - iWidthUVPlusExp;

	CoordI y;
#ifndef HITACHI
	for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        I32_WMV x;
		U32_WMV uipadValueLeftU, uipadValueLeftV;
		U32_WMV uipadValueRightU, uipadValueRightV;
		uipadValueLeftU = *ppxliOldLeftU;
		uipadValueLeftV = *ppxliOldLeftV;
		uipadValueLeftU |= (uipadValueLeftU << 8);
		uipadValueLeftV |= (uipadValueLeftV << 8);
		uipadValueLeftU |= (uipadValueLeftU << 16);
		uipadValueLeftV |= (uipadValueLeftV << 16);
	//	uipadValueLeftU |= (uipadValueLeftU << 32);
	//	uipadValueLeftV |= (uipadValueLeftV << 32);
		uipadValueRightU = *ppxliOldRightU;
		uipadValueRightV = *ppxliOldRightV;
		uipadValueRightU |= (uipadValueRightU << 8);
		uipadValueRightV |= (uipadValueRightV << 8);
		uipadValueRightU |= (uipadValueRightU << 16);
		uipadValueRightV |= (uipadValueRightV << 16);
	//	uipadValueRightU |= (uipadValueRightU << 32);
	//	uipadValueRightV |= (uipadValueRightV << 32);
		ppxliNewUPlusWidth = ppxliNewU + iWidthUVPlusExp;
		ppxliNewVPlusWidth = ppxliNewV + iWidthUVPlusExp;
		for (x = 0; x < EXPANDUV_REFVOP; x += 4) {
			*(U32_WMV *)(ppxliNewU + x) = uipadValueLeftU;
			*(U32_WMV *)(ppxliNewV + x) = uipadValueLeftV;
			*(U32_WMV *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
			*(U32_WMV *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
		}

		ppxliNewU += iWidthUVPlusExp;		
		ppxliNewV += iWidthUVPlusExp;
		ppxliNewU += iResidue;		
		ppxliNewV += iResidue;		
		ppxliOldLeftU += iWidthPrevUV;
		ppxliOldLeftV += iWidthPrevUV;
		ppxliOldRightU += iWidthPrevUV;
		ppxliOldRightV += iWidthPrevUV;
	}
#else
	for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        I32_WMV x;
		U64 uipadValueLeftU, uipadValueLeftV;
		U64 uipadValueRightU, uipadValueRightV;
		uipadValueLeftU.dwLo = (DWORD)*ppxliOldLeftU;
		uipadValueLeftV.dwLo = (DWORD)*ppxliOldLeftV;
		uipadValueLeftU.dwLo |= (uipadValueLeftU.dwLo << 8);
		uipadValueLeftV.dwLo |= (uipadValueLeftV.dwLo << 8);
		uipadValueLeftU.dwLo |= (uipadValueLeftU.dwLo << 16);
		uipadValueLeftV.dwLo |= (uipadValueLeftV.dwLo << 16);
		uipadValueLeftU.dwHi = (uipadValueLeftU.dwLo);
		uipadValueLeftV.dwHi = (uipadValueLeftV.dwLo);
		uipadValueRightU.dwLo = (DWORD)*ppxliOldRightU;
		uipadValueRightV.dwLo = (DWORD)*ppxliOldRightV;
		uipadValueRightU.dwLo |= (uipadValueRightU.dwLo << 8);
		uipadValueRightV.dwLo |= (uipadValueRightV.dwLo << 8);
		uipadValueRightU.dwLo |= (uipadValueRightU.dwLo << 16);
		uipadValueRightV.dwLo |= (uipadValueRightV.dwLo << 16);
		uipadValueRightU.dwHi = (uipadValueRightU.dwLo );
		uipadValueRightV.dwHi = (uipadValueRightV.dwLo);
		ppxliNewUPlusWidth = ppxliNewU + iWidthUVPlusExp;
		ppxliNewVPlusWidth = ppxliNewV + iWidthUVPlusExp;
		for (x = 0; x < EXPANDUV_REFVOP; x += 8) {
			*(U64 *)(ppxliNewU + x) = uipadValueLeftU;
			*(U64 *)(ppxliNewV + x) = uipadValueLeftV;
			*(U64 *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
			*(U64 *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
		}
		ppxliNewU += iWidthUVPlusExp;		
		ppxliNewV += iWidthUVPlusExp;
		ppxliNewU += iResidue;		
		ppxliNewV += iResidue;		
		ppxliOldLeftU += iWidthPrevUV;
		ppxliOldLeftV += iWidthPrevUV;
		ppxliOldRightU += iWidthPrevUV;
		ppxliOldRightV += iWidthPrevUV;
	}
#endif
    {
        I32_WMV iWidthPrevUVDiv8 = iWidthPrevUV >> 3;
        U8_WMV* ppxliSrcU;
        U8_WMV* ppxliSrcV;
        U8_WMV* ppxliDstU;
        U8_WMV* ppxliDstV;
        if (fTop) {
            U8_WMV* ppxliLeftTopU = ppxlcRef0U;
            U8_WMV* ppxliLeftTopV = ppxlcRef0V;
            for (y = 0; y < EXPANDUV_REFVOP; y++) {
                I32_WMV x;
                ppxliSrcU = (U8_WMV*) ppxliOldTopLnU;
                ppxliSrcV = (U8_WMV*) ppxliOldTopLnV;
                ppxliDstU = ppxliLeftTopU;
                ppxliDstV = ppxliLeftTopV;
                for (x = 0; x < iWidthPrevUVDiv8; x++) {
                    *(U64 *) ppxliDstU = *(U64 *) ppxliSrcU;
                    ppxliSrcU += 8;
                    ppxliDstU += 8;
                    *(U64 *) ppxliDstV = *(U64 *) ppxliSrcV;
                    ppxliSrcV += 8;
                    ppxliDstV += 8;
                }
                ppxliLeftTopU += iWidthPrevUV;
                ppxliLeftTopV += iWidthPrevUV;
            }
        }
        if (fBottom) {
            const U8_WMV* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
            const U8_WMV* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;
            I32_WMV iHeightUVPadded = ((iEnd * 2 + 15) & ~15)/2;
            I32_WMV iBotExtend = iHeightUVPadded - iEnd + EXPANDUV_REFVOP;       
            for (y = 0; y < iBotExtend; y++) {
                I32_WMV x;
                ppxliSrcU = (U8_WMV*) ppxliOldBotLnU;
                ppxliSrcV = (U8_WMV*) ppxliOldBotLnV;
                ppxliDstU = ppxliNewU;
                ppxliDstV = ppxliNewV;
                for (x = 0; x < iWidthPrevUVDiv8; x++) {
                    *(U64 *) ppxliDstU = *(U64 *) ppxliSrcU;
                    ppxliSrcU += 8;
                    ppxliDstU += 8;
                    *(U64 *) ppxliDstV = *(U64 *) ppxliSrcV;
                    ppxliSrcV += 8;
                    ppxliDstV += 8;
                }
                ppxliNewU += iWidthPrevUV;
                ppxliNewV += iWidthPrevUV;
            }
        }
    }
}
#endif //OPT_REPEATPAD_ARM

//THR1 = the threshold before we consider neighboring pixels to be "diffrent"
#define THR1_TMP 2
//THR2 = the total number of "diffrent" pixels under which we use stronger filter
#define THR2_TMP 6

#define phi(a) ((abs(a) <= THR1_TMP) ? 1 : 0)

#if !defined(_ARM_ASM_LOOPFILTER_OPT_)&&!defined(_MIPS_ASM_LOOPFILTER_OPT_)&&!defined(_SH4_ASM_LOOPFILTER_OPT_)

Void_WMV g_FilterHorizontalEdge_WMV(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                            I32_WMV iNumPixel)
{
    // Filter horizontal line        
    for (I32_WMV i = 0; i < iNumPixel; ++i) {                                                                                                                                                                                                    
        I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;
        I32_WMV v4_v5, a30, absA30;
        U8_WMV *pVtmp = pV5;

        v5 = *pV5;
        pVtmp -= iPixelDistance;
        v4 = *pVtmp;
        pVtmp -= iPixelDistance;
        v3 = *pVtmp;
        pVtmp -= iPixelDistance;
        v2 = *pVtmp;
        pVtmp -= iPixelDistance;
        v1 = *pVtmp;
        pVtmp = pV5 + iPixelDistance;
        v6 = *pVtmp;        
        pVtmp += iPixelDistance;
        v7 = *pVtmp;
        pVtmp += iPixelDistance;
        v8 = *pVtmp;

        v4_v5 = v4 - v5;
        a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
        absA30 = abs(a30);
        if (absA30 < iStepSize) {

            I32_WMV v2_v3 = v2 - v3;
            I32_WMV v6_v7 = v6 - v7;
            I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
            I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
            I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 

            if (iMina31_a32 < absA30){
                I32_WMV a, c;                
                c = v4_v5/2;
               
                if (0 < c) {
                    if (a30 < 0) {
                        I32_WMV dA30;
                        dA30 = absA30 - iMina31_a32 ;  // > 0
                        a = (5 * dA30) >> 3; // >= 0
                        if (a > c) a = c;
                        *(pV5 - iPixelDistance) = v4 - a;                                                                                         
                        *(pV5) = v5 + a;
                    } 
                } else if (c < 0) {
                    if (a30 >= 0) {                       
                        I32_WMV dA30;
                        dA30 =  iMina31_a32 - absA30; // < 0
                        a = (5 * dA30 + 7) >> 3; // <= 0
                        if (a < c) a = c;
                        *(pV5 - iPixelDistance) = v4 - a;                                                                                         
                        *(pV5) = v5 + a;
                    }
                }
            }
        }        
        pV5 ++;
    }
}


Void_WMV g_FilterVerticalEdge_WMV(U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                          I32_WMV iNumPixel)
{
    U8_WMV *pVh;
    // Filter vertical line
    for (int i = 0; i < iNumPixel; ++i) {
        
        I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;  
        I32_WMV v4_v5, a30, absA30;
        pVh = pVhstart;            

        v1 = pVh[1]; 
        v2 = pVh[2];
        v3 = pVh[3]; 
        v4 = pVh[4];
        v5 = pVh[5];
        v6 = pVh[6]; 
        v7 = pVh[7]; 
        v8 = pVh[8]; 

        v4_v5 = v4 - v5;
        a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
        absA30 = abs(a30);
        if (absA30 < iStepSize) {

            I32_WMV v2_v3 = v2 - v3;
            I32_WMV v6_v7 = v6 - v7;
            I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
            I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
            I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 

            if (iMina31_a32 < absA30){
                I32_WMV a, c;                
                c = v4_v5/2;
               
                if (0 < c) {
                    if (a30 < 0) {
                        I32_WMV dA30;
                        dA30 = absA30 - iMina31_a32 ;  // > 0
                        a = (5 * dA30) >> 3; // >= 0
                        if (a > c) a = c;
                        pVh[4] = v4 - a;                                                                                         
                        pVh[5] = v5 + a;
                    } 
                } else if (c < 0) {
                    if (a30 >= 0) {                       
                        I32_WMV dA30;
                        dA30 =  iMina31_a32 - absA30; // < 0
                        a = (5 * dA30 + 7) >> 3; // <= 0
                        if (a < c) a = c;
                        pVh[4] = v4 - a;                                                                                         
                        pVh[5] = v5 + a;
                    }
                }
            }
        }       
        pVhstart += iPixelDistance;                
    }
}

#endif

Void_WMV FilterEdgeShortTagMBRow(
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxlcCenter, 
    I32_WMV iPixelDistance, 
    I32_WMV iStepSize, 
    I32_WMV iMBsPerRow, 
    Bool_WMV bFirst, 
    Bool_WMV bLast
    )
{
    U8_WMV* pVhstart;
    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV5x = pV5 + 8*iPixelDistance;

    I32_WMV iVertSize = 16;
    I32_WMV iVertOffset = -4;
    I32_WMV imbX;

    assert(bFirst == FALSE);

    if (bLast)
        iVertSize += 4;

    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
    pV5 += 4;
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 4);    
    pV5x += 4;

    // Loop through the macroblocks in the row filtering horiz. line first then vertical
    pVhstart = ppxlcCenter + iVertOffset*iPixelDistance + 3;

    for (imbX = 0; imbX < iMBsPerRow - 1; imbX++) {
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 16);
        pV5 += 16;//iPixelDistance*8;          
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 16);
        pV5x += 16;

        (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
        (*pWMVDec->m_pFilterVerticalEdge)(pVhstart + 8, iPixelDistance, iStepSize, iVertSize);
        pVhstart += 16;
    }

    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 12);
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 12);
}

Void_WMV FilterEdgeShortTagBlockRow(tWMVDecInternalMember *pWMVDec, U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iBlocksPerRow, Bool_WMV bFirst, Bool_WMV bLast)
{    
    U8_WMV *pVhstart;
    U8_WMV *pV5 = ppxlcCenter;

    I32_WMV iVertSize = 8;
    I32_WMV iVertOffset = -4;
    I32_WMV iblkX;

    if (bFirst)
    {
        iVertSize = 12;
        iVertOffset = -8;
    }
    if (bLast)
        iVertSize += 4;

    pVhstart = ppxlcCenter + iPixelDistance*iVertOffset + 3;

    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
    pV5 += 4;

    // Loop through the blocks in the row filtering horiz line first then vertical
    for (iblkX = 0; iblkX < iBlocksPerRow - 1; iblkX++) {
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 8);
        pV5 += 8;
        (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize,  iVertSize);        
        pVhstart += 8;
    }

    // Filter last four pixels in the horizontal line
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);

}

Void_WMV DeblockSLFrame (
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    Bool_WMV bMultiThread, U32_WMV iThreadID, I32_WMV iMBStartX, I32_WMV iMBEndX, I32_WMV iMBStartY, I32_WMV iMBEndY
) // Frame-based deblocking
{
    FUNCTION_PROFILE_DECL_START(fp,DEBLOCKSLFRAME_PROFILE);
	U8_WMV* ppxliPost;
	I32_WMV iblkSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY / 2;
    I32_WMV iMBStartUV = iMBStartY;
    I32_WMV iVertSize = (iMBEndY - iMBStartY) * MB_SIZE;
    I32_WMV iblkY;

    if (iThreadID == pWMVDec->m_uiNumProcessors - 1)
    {
        iMBEndY--;
        iVertSize += 4;
    }
    if (iThreadID == 0)
        iMBStartUV = 2;

    // Deblock Y
    if (iThreadID == 0) {
        FilterEdgeShortTagBlockRow (pWMVDec, ppxliY + iblkSizeXWidthPrevY, pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 2*iMBEndX, TRUE_WMV, FALSE);
    }
    ppxliPost = ppxliY + iMBStartY * pWMVDec->m_iMBSizeXWidthPrevY;
    for (iblkY = iMBStartY; iblkY < iMBEndY; iblkY++) {
        FilterEdgeShortTagMBRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, iMBEndX, FALSE, FALSE);
        ppxliPost += pWMVDec->m_iMBSizeXWidthPrevY;
    }
    if (iThreadID == pWMVDec->m_uiNumProcessors - 1)
        FilterEdgeShortTagMBRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, iMBEndX, FALSE, TRUE_WMV);

    (*pWMVDec->m_pFilterVerticalEdge)(ppxliY + (iMBStartY * MB_SIZE - 4) * pWMVDec->m_iWidthPrevY + (iMBEndX * MB_SIZE) - 8 - (5*1 /* adjust to point to the top i.e. pV1)*/), pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, iVertSize);
    // Deblock U
    if (iThreadID == 0) {
        FilterEdgeShortTagBlockRow (pWMVDec, ppxliU + pWMVDec->m_iBlkSizeXWidthPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, iMBEndX, TRUE_WMV, FALSE);
    }
    ppxliPost = ppxliU + iMBStartUV * pWMVDec->m_iBlkSizeXWidthPrevUV;
    for (iblkY = iMBStartUV; iblkY < iMBEndY; iblkY++) {
        FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, iMBEndX, FALSE, FALSE);
        ppxliPost += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }
    if (iThreadID == pWMVDec->m_uiNumProcessors - 1)
        FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, iMBEndX, FALSE, TRUE_WMV);

    // Deblock V
    if (iThreadID == 0) {
        FilterEdgeShortTagBlockRow (pWMVDec, ppxliV + pWMVDec->m_iBlkSizeXWidthPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, iMBEndX, TRUE_WMV, FALSE);
    }
    ppxliPost = ppxliV + iMBStartUV * pWMVDec->m_iBlkSizeXWidthPrevUV;
    for (iblkY = iMBStartUV; iblkY < iMBEndY; iblkY++) {
        FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, iMBEndX, FALSE, FALSE);
        ppxliPost += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }
    if (iThreadID == pWMVDec->m_uiNumProcessors - 1)
        FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, iMBEndX, FALSE, TRUE_WMV);
    FUNCTION_PROFILE_STOP(&fp);
}

I32_WMV BlkAvgX8(const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize)
{
	I32_WMV iSum = 0;
	for (I32_WMV i = 0; i < BLOCK_SIZE; i++){
		for (I32_WMV j = 0; j < BLOCK_SIZE; j++){
			iSum += ppxlcCurrRecnMB[i* iWidthPrev + j];
		}
	}
	return ((iSum + 4) >> 3) / iStepSize;  // iSum / 64 * 8
}


Void_WMV g_InitDecGlobalVars ()
{
}

