/************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

	blkdec.cpp

Abstract:

	block decoding functions.

Author:

	Wei-ge Chen (wchen@microsoft.com) 4-Sept-1996
	Bruce Lin (blin@microsoft.com) 02-Nov-1996

Revision History:

*************************************************************************/

#include "bldsetup.h"

#include "xplatform.h"
#include "wmvdec_api.h"

#include <stdlib.h>
#include <math.h>
#include "limits.h"
#include "typedef.hpp"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "codehead.h"
#include "strmdec_wmv.hpp"
#include "idctdec_wmv2.h"

#ifdef __MFC_
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW				
#endif // __MFC_
//#define MAXCODEWORDLNGIntraDCy 7
//#define MAXCODEWORDLNGIntraDCc 8

tWMVDecodeStatus decodeIntraBlockAcPred (
    tWMVDecInternalMember *pWMVDec,
	CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, 
	CWMVMBMode*        pmbmd,
    U8_WMV   *ppxliTextureQMB,        //place to set the pixels
    I32_WMV             iOffsetToNextRowForDCT,
	I32_WMV             ib, 
    I16_WMV*            piQuanCoefACPred, 
    I16_WMV*            pRef, 
    Bool_WMV            bPredPattern
)
{

    tWMVDecodeStatus tWMVStatus;

	//Bool_WMV bPredPattern;
	U8_WMV* piZigzagInv; 
    assert(MB_SQUARE_SIZE - 4 <= UINT_MAX);
    memset (pWMVDec->m_rgiCoefReconPlus1, 0, (size_t)(MB_SQUARE_SIZE - 4));
	//piQuanCoefACPred [0] = m_rgiCoefRecon[0] + pRef [0];
	//m_rgiCoefRecon[0] = (piQuanCoefACPred [0] << 3);

    if ((pmbmd->m_rgbDCTCoefPredPattern & 1)){
		if (bPredPattern != pWMVDec->m_bRotatedIdct){
            I32_WMV i;
			for (i = 1; i < 8; i++) {
				piQuanCoefACPred [i + BLOCK_SIZE] = 0;
				piQuanCoefACPred [i] = pRef [i] ;
			}
            //piZigzagInv = g_pHorizontalZigzagInv; Old Symmetric matrix, 
    		if (pWMVDec->m_bRotatedIdct) //bPredPattern == 0, m_bRotatedIdct == 1 
    			piZigzagInv = (U8_WMV*) pWMVDec->m_pVerticalZigzagInvRotated;
            else //bPredPattern == 1, m_bRotatedIdct == 0 
    			piZigzagInv = (U8_WMV*) pWMVDec->m_pHorizontalZigzagInv;
		}
		else{
            I32_WMV i;
			for (i = 1; i < 8; i++) {
				piQuanCoefACPred [i + BLOCK_SIZE] = pRef [i + BLOCK_SIZE];
				piQuanCoefACPred [i] = 0;
			}
            // piZigzagInv = g_pVerticalZigzagInv; Old Symmetric matrix, 
    		if (pWMVDec->m_bRotatedIdct) //bPredPattern == 1 , m_bRotatedIdct == 1
    			piZigzagInv = (U8_WMV*) pWMVDec->m_pHorizontalZigzagInvRotated;
            else //bPredPattern == 0, m_bRotatedIdct == 0
    			piZigzagInv = (U8_WMV*) pWMVDec->m_pVerticalZigzagInv;
		}
	}
	else{
        I32_WMV i;
		for (i = 1; i < 8; i++) {
			piQuanCoefACPred [i + BLOCK_SIZE] = 0;
			piQuanCoefACPred [i] = 0;
		}
		if (pWMVDec->m_bRotatedIdct)
            piZigzagInv = pWMVDec->m_pZigzagInvRotated_I;
        else
            piZigzagInv = pWMVDec->m_pZigzagInv_I;
	}

    // reverse ib because of the internal representation of m_rgbCodedBlockPattern
    if (pWMVDec->m_cvCodecVersion != MP4S)
        tWMVStatus = DecodeInverseIntraBlockQuantizeAcPred (
            pWMVDec,
			IntraDCTTableInfo_Dec,
			(pmbmd->m_rgbCodedBlockPattern & (1<<(5-ib))), 
			piQuanCoefACPred, 
			piZigzagInv
        );//get the quantized block	
    else
    	tWMVStatus = DecodeInverseIntraBlockQuantizeAcPred_MPEG4 (
            pWMVDec,
			IntraDCTTableInfo_Dec,
			(pmbmd->m_rgbCodedBlockPattern & (1<<(5-ib))), 
			piQuanCoefACPred, 
			piZigzagInv
            );//get the quantized block	

    
    if (WMV_Succeeded != tWMVStatus) {
		return tWMVStatus;
	}
    FUNCTION_PROFILE_DECL_START(fpIDCT,IDCT_PROFILE);
	(*pWMVDec->m_pIntraIDCT_Dec) (ppxliTextureQMB, iOffsetToNextRowForDCT, pWMVDec->m_rgiCoefRecon);
    FUNCTION_PROFILE_STOP(&fpIDCT);
    return WMV_Succeeded;
}



//Use 0-5 in decoder but Y_BLOCK1-V_BLOCK in encoder. the mess needs to be fixed.
I16_WMV* decodeDiffDCTCoef(tWMVDecInternalMember *pWMVDec, I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB, Bool_WMV& bPredPattern)
{
//	I16_WMV* pDct = piQuanCoefACPred;
	//PixelI* pCoefRecon = m_rgiCoefRecon;
	I16_WMV *pDctLeft, *pDctTop, *pDctLeftTop;

    pDctLeft = piQuanCoefACPredTable[bLeftMB];
    pDctTop = piQuanCoefACPredTable[2+bTopMB];
    pDctLeftTop = piQuanCoefACPredTable[4+bLeftTopMB];

    if (abs (*pDctLeftTop - *pDctTop) < abs (*pDctLeftTop - *pDctLeft) + pWMVDec->m_iDCPredCorrect){
        bPredPattern = 0;
        return pDctLeft;
    }
    else {
        bPredPattern = 1;
        return pDctTop;
    }
}
Void_WMV decodeBitsOfESCCode (tWMVDecInternalMember *pWMVDec)
{

    if (pWMVDec->m_iStepSize >= 8){
        I32_WMV iBitsCnt = 0, iBit = 0;
        while (iBitsCnt < 6 && !iBit){
            iBit = BS_getBit ( pWMVDec->m_pbitstrmIn);
            iBitsCnt++;
        }
        if (iBit)
            pWMVDec->m_iNUMBITS_ESC_LEVEL = iBitsCnt + 1; // from 2-7
        else
            pWMVDec->m_iNUMBITS_ESC_LEVEL = 8; // from 2-7
    }
    else{
        pWMVDec->m_iNUMBITS_ESC_LEVEL = BS_getBits ( pWMVDec->m_pbitstrmIn, 3);  // 1-7, 0 is ESC
        if (!pWMVDec->m_iNUMBITS_ESC_LEVEL){
            pWMVDec->m_iNUMBITS_ESC_LEVEL = 8 + BS_getBit ( pWMVDec->m_pbitstrmIn);  // ESC + 0 == 8  ESC + 1 == 9
        }
    }

    pWMVDec->m_iNUMBITS_ESC_RUN = 3 + BS_getBits ( pWMVDec->m_pbitstrmIn, 2);


}



#define WMV_ESC_Decoding()                                                          \
	if (pWMVDec->m_bFirstEscCodeInFrame){                                                    \
        decodeBitsOfESCCode (pWMVDec);                                                     \
        pWMVDec->m_bFirstEscCodeInFrame = FALSE_WMV;                                             \
	}                                                                               \
	uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);			                    \
	if (BS_getBit ( pWMVDec->m_pbitstrmIn)) /* escape decoding */                              \
		iLevel = -1 * BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                 \
	else                                                                            \
		iLevel = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                      


// Sridhar:  This function isn't called (unles MMX stream) so don't bother for now
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode)
{
	FUNCTION_PROFILE_DECL_START(fpDecode,DECINVIBQUANTESCCODE_PROFILE );
	
	Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec->hufDCTACDec;
	I8_WMV* rgLevelAtIndx = InterDCTTableInfo_Dec->pcLevelAtIndx;
	U8_WMV* rgRunAtIndx = InterDCTTableInfo_Dec->puiRunAtIndx;
//  Change iStartIndxOfLastRun to iStartIndxOfLastRunMinus1 and <= to <
//	iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1;
	U8_WMV* rgIfNotLastNumOfLevelAtRun = InterDCTTableInfo_Dec->puiNotLastNumOfLevelAtRun;
	U8_WMV* rgIfLastNumOfLevelAtRun = InterDCTTableInfo_Dec->puiLastNumOfLevelAtRun; 
	U8_WMV* rgIfNotLastNumOfRunAtLevel = InterDCTTableInfo_Dec->puiNotLastNumOfRunAtLevel;
	U8_WMV* rgIfLastNumOfRunAtLevel = InterDCTTableInfo_Dec->puiLastNumOfRunAtLevel;
	U32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
	I32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec->iTcoef_ESCAPE;

    Bool_WMV bIsLastRun = FALSE_WMV;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
	register I32_WMV iDCTHorzFlags = 0;

    memset (pWMVDec->m_rgiCoefRecon, 0, (size_t) MB_SQUARE_SIZE);
    do {
        register I32_WMV iIndex;
		lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
		//lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            FUNCTION_PROFILE_STOP(&fpDecode);
			return WMV_CorruptedBits;
		}
        //lIndex = m_pentrdecDCT->decodeSymbol();
        // if Huffman
        if (lIndex != iTCOEF_ESCAPE)	{
			if (lIndex >= iStartIndxOfLastRun)
				bIsLastRun = TRUE_WMV;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return WMV_CorruptedBits;
            }
        } 
		else {
			if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
				// ESC + '1' + VLC
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
        		//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
				//lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    FUNCTION_PROFILE_STOP(&fpDecode);
			        return WMV_CorruptedBits;
		        }
				uiRun = rgRunAtIndx[lIndex];
				iLevel = rgLevelAtIndx[lIndex];
				if (lIndex >= iStartIndxOfLastRun){
					bIsLastRun = TRUE_WMV;
					iLevel += (I8_WMV) rgIfLastNumOfLevelAtRun[uiRun];
				}
				else
					iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRun[uiRun];
				if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
			}
			else if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
				// ESC + '10' + VLC
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
        		//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
				//lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    FUNCTION_PROFILE_STOP(&fpDecode);
			        return WMV_CorruptedBits;
		        }
				uiRun = rgRunAtIndx[lIndex];
				iLevel = rgLevelAtIndx[lIndex];
				if (lIndex >= iStartIndxOfLastRun){
					bIsLastRun = TRUE_WMV;
					uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
				}
				else
					uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
				if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
			}
			else{
				// ESC + '00' + FLC
				bIsLastRun = (Bool_WMV) BS_getBit ( pWMVDec->m_pbitstrmIn); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){
                    WMV_ESC_Decoding();
                }
                else{
					uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);			
        			iLevel = (I8_WMV)BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
				}
		        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
			        return WMV_CorruptedBits;
		        }
			}
        }
        uiCoefCounter += uiRun;

		if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
            FUNCTION_PROFILE_STOP(&fpDecode);
			return WMV_CorruptedBits;
        }
        
        iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];

        // If this coefficient is not in the first column then set the flag which indicates
        // what row it is in. This flag field will be used by the IDCT to see if it can
        // shortcut the IDCT of the row if all coefficients are zero.
		if (iIndex & 0x7)
			iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

        if (iLevel == 1)
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = pWMVDec->m_i2DoublePlusStepSize;
		else if (iLevel == -1)
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = pWMVDec->m_i2DoublePlusStepSizeNeg;
		else if (iLevel > 0)
//			I32_WMV iCoefRecon = m_iStepSize * ((iLevel << 1) + 1) - m_bStepSizeIsEven;
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
		else
//			I32_WMV iCoefRecon = m_bStepSizeIsEven - m_iStepSize * (1 - (iLevel << 1));
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;

        uiCoefCounter++;
    } while (!bIsLastRun);
    // Save the DCT row flags. This will be passed to the IDCT routine
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;

    FUNCTION_PROFILE_STOP(&fpDecode);
    return WMV_Succeeded;
}


Void_WMV decodeIntraDCAcPredMSV (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE)
{
    U8_WMV lSzDiffIntraDC;
	lSzDiffIntraDC = (U8_WMV) Huffman_WMV_get (hufDCTDCDec, pWMVDec->m_pbitstrmIn);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn)){
        *pError = WMV_CorruptedBits;
    }
    if (lSzDiffIntraDC != iTCOEF_ESCAPE)	{
        if (lSzDiffIntraDC != 0)
            pWMVDec->m_rgiCoefRecon [0] = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? -lSzDiffIntraDC : lSzDiffIntraDC;
        else
            pWMVDec->m_rgiCoefRecon [0] = 0;
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            *pError = WMV_CorruptedBits;
        }
    }
	else {
		lSzDiffIntraDC = (U8_WMV) BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
        pWMVDec->m_rgiCoefRecon [0] = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? -lSzDiffIntraDC : lSzDiffIntraDC;
    }
	*pError = WMV_Succeeded;
}

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred (
    tWMVDecInternalMember *pWMVDec,
	CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, 
	Bool_WMV CBP,
	I16_WMV* pDct, 
	U8_WMV* piZigzagInv 	
)
{
#   if !defined(_SH4_)
    FUNCTION_PROFILE_DECL_START(fpDecode,DECINVIBQUANTESCCODE_PROFILE );
#	endif
	
	Huffman_WMV* hufDCTACDec = IntraDCTTableInfo_Dec->hufDCTACDec;
	I8_WMV* rgLevelAtIndx = IntraDCTTableInfo_Dec->pcLevelAtIndx;
	U8_WMV* rgRunAtIndx = IntraDCTTableInfo_Dec->puiRunAtIndx;
//  Change iStartIndxOfLastRun to iStartIndxOfLastRunMinus1 and <= to <
//	iStartIndxOfLastRun = IntraDCTTableInfo_Dec->iStartIndxOfLastRunMinus1;
	U8_WMV* rgIfNotLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec->puiNotLastNumOfLevelAtRun;
	U8_WMV* rgIfLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec->puiLastNumOfLevelAtRun; 
	U8_WMV* rgIfNotLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec->puiNotLastNumOfRunAtLevel;
	U8_WMV* rgIfLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec->puiLastNumOfRunAtLevel;
	U32_WMV iStartIndxOfLastRun = IntraDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
	I32_WMV iTCOEF_ESCAPE = IntraDCTTableInfo_Dec->iTcoef_ESCAPE;


    Bool_WMV bIsLastRun = FALSE_WMV;
    U32_WMV uiRun; // = 0;
    I32_WMV iLevel; // = 0;
    U32_WMV uiCoefCounter = 1;
    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;

#ifndef _NEW_INTRA_QUANT_RECON_
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
#else
	I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepSize - 1;
#endif
    I32_WMV i;

	if (CBP) {
		do {
            lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
            //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
			//lIndex = m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT,decodeCodeTableDCT);
            assert(lIndex <= iTCOEF_ESCAPE);
			if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {

#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif

				               
				return WMV_CorruptedBits;
			}
			if (lIndex != iTCOEF_ESCAPE)	{
				bIsLastRun = (lIndex >= iStartIndxOfLastRun);
				uiRun = rgRunAtIndx[lIndex];
				iLevel = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
					rgLevelAtIndx[lIndex];
				if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {

#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
					
					return WMV_CorruptedBits;
				}
			}
			else {
				if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {


#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif

						return WMV_CorruptedBits;
                    }
					// ESC + '1' + VLC
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
					//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
					//lIndex = pWMVDec->m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
			            return WMV_CorruptedBits;
		            }
					uiRun = rgRunAtIndx [lIndex];
					iLevel = rgLevelAtIndx[lIndex];
					if (lIndex >= iStartIndxOfLastRun){
						bIsLastRun = TRUE_WMV;
						iLevel += (I8_WMV) rgIfLastNumOfLevelAtRunIntra[uiRun];
					}
					else
						iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRunIntra[uiRun];
					if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
						iLevel = -iLevel;
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
                        return WMV_CorruptedBits;
                    }
				}
				else if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
                        return WMV_CorruptedBits;
                    }
					// ESC + '01' + VLC
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
					//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
					//lIndex = pWMVDec->m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
			            return WMV_CorruptedBits;
		            }
					uiRun = rgRunAtIndx [lIndex];
					iLevel = rgLevelAtIndx[lIndex];
					if (lIndex >= iStartIndxOfLastRun){
						bIsLastRun = TRUE_WMV;
						uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
					}
					else
						uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
					if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
						iLevel = -iLevel;
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
                        return WMV_CorruptedBits;
                    }
				}
				else{
					// ESC + '00' + FLC
					bIsLastRun = (Bool_WMV) BS_getBit ( pWMVDec->m_pbitstrmIn); // escape decoding
                    if (pWMVDec->m_cvCodecVersion >= WMV1){
                        WMV_ESC_Decoding();
                    }
                    else{
					    uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);			
        			    iLevel = (I8_WMV)BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
					}
		            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
			            return WMV_CorruptedBits;
		            }
				}
			}
            assert(uiRun<= 63);
			uiCoefCounter += uiRun;

			if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
				return WMV_CorruptedBits;
			}
			// Use 8x8 for each blcok to aVoid_WMV IFs. 
			if ((piZigzagInv[uiCoefCounter] & 0xfffffff8) == 0)  // cnt >= 0(1) && <=7
				pDct[piZigzagInv[uiCoefCounter]] = iLevel + pDct[piZigzagInv[uiCoefCounter]];
			else if ((piZigzagInv[uiCoefCounter] & 0x00000007) == 0) // cnt mod 8 == 0
				pDct[BLOCK_SIZE + (piZigzagInv[uiCoefCounter] >> 3)] = iLevel + pDct[BLOCK_SIZE + (piZigzagInv [uiCoefCounter] >> 3)];
			else {
				if (iLevel > 0)
//					pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel * 2 + 1)) - pWMVDec->m_bStepSizeIsEven);
					pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
				else // iLevel < 0 (note: iLevel != 0)
//					pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel * 2 - 1)) + pWMVDec->m_bStepSizeIsEven);
					pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;
			}
			uiCoefCounter++;
		} while (!bIsLastRun);
	}

	for (i = 1; i < 8; i++){
		I32_WMV iLevel1 = pDct [i];
		if (iLevel1 == 0)
			pWMVDec->m_rgiCoefRecon [i] = 0;
        else if (iLevel1 > 0)
//            pWMVDec->m_rgiCoefRecon [i] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel1 * 2 + 1)) - pWMVDec->m_bStepSizeIsEven);
            pWMVDec->m_rgiCoefRecon [i] = iDoubleStepSize * iLevel1 + iStepMinusStepIsEven;
		else
//            pWMVDec->m_rgiCoefRecon [i] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel1 * 2 - 1)) + pWMVDec->m_bStepSizeIsEven);
            pWMVDec->m_rgiCoefRecon [i] = iDoubleStepSize * iLevel1 - iStepMinusStepIsEven;
		
        iLevel1 = pDct [i + BLOCK_SIZE];
		if (iLevel1 == 0)
			pWMVDec->m_rgiCoefRecon [i << 3] = 0;
        else if (iLevel1 > 0)
//            pWMVDec->m_rgiCoefRecon [i << 3] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel1 * 2 + 1)) - pWMVDec->m_bStepSizeIsEven);
            pWMVDec->m_rgiCoefRecon [i << 3] = iDoubleStepSize * iLevel1 + iStepMinusStepIsEven;
		else
//            pWMVDec->m_rgiCoefRecon [i << 3] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel1 * 2 - 1)) + pWMVDec->m_bStepSizeIsEven);
            pWMVDec->m_rgiCoefRecon [i << 3] = iDoubleStepSize * iLevel1 - iStepMinusStepIsEven;
	}
#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif
    return WMV_Succeeded;
}

//Use 0-5 in decoder but Y_BLOCK1-V_BLOCK in encoder. the mess needs to be fixed.
I16_WMV* decodeDiffDCTCoef_WMV(tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliTextureQMB, I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB, Bool_WMV bLeftMBAndInter, Bool_WMV bTopMBAndInter, Bool_WMV bLeftTopMBAndInter, Bool_WMV& bPredPattern, I32_WMV iWidthPrev, I32_WMV iStepSize)
{
	I16_WMV *pDctLeft, *pDctTop;
    //I16_WMV iDctLeftTop;
    I16_WMV *pAvgDct;

    if (!bPredPattern){
        if (bLeftMBAndInter){
            pAvgDct = piQuanCoefACPredTable[1];
            if (1)//!pAvgDct[0])
                pAvgDct[0] = (I16_WMV) (*pWMVDec->m_pBlkAvgX8_MMX) (ppxliTextureQMB - BLOCK_SIZE, iWidthPrev, iStepSize);
            pWMVDec->m_pAvgQuanDctCoefDecLeft[0] = pAvgDct[0];
            pDctLeft = pWMVDec->m_pAvgQuanDctCoefDecLeft;
        }
        else{
            pDctLeft = piQuanCoefACPredTable[bLeftMB];
        }
        //bPredPattern = 0;
        return pDctLeft;
    }
    else{
        if (bTopMBAndInter){
            pAvgDct = piQuanCoefACPredTable[3];
            if (1)//!pAvgDct[0])
                pAvgDct[0] = (I16_WMV) (*pWMVDec->m_pBlkAvgX8_MMX) (ppxliTextureQMB - (iWidthPrev << 3) , iWidthPrev, iStepSize);
            pWMVDec->m_pAvgQuanDctCoefDecTop[0] = pAvgDct[0];
            pDctTop = pWMVDec->m_pAvgQuanDctCoefDecTop;
        }
        else{
            pDctTop = piQuanCoefACPredTable[2+bTopMB];
        }
        //bPredPattern = 1;
        return pDctTop;
    }
}

I16_WMV* decodeDiffDCTCoef_WMV_Y23(I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB, Bool_WMV& bPredPattern)
{
	I16_WMV *pDctLeft, *pDctTop;//, *pDctLeftTop;

    pDctLeft = piQuanCoefACPredTable[bLeftMB];
    pDctTop = piQuanCoefACPredTable[2+bTopMB];
    //pDctLeftTop = piQuanCoefACPredTable[4+bLeftTopMB];

    if (!bPredPattern)
        return pDctLeft;
    else
        return pDctTop;

}

#ifndef _EMB_WMV2_
// MB routines from Vopdec.cpp
tWMVDecodeStatus DecodePMB (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
//    const Bool_WMV* rgCBP, 
    U8_WMV* ppxlcCurrQYMB,
    U8_WMV* ppxlcCurrQUMB,
    U8_WMV* ppxlcCurrQVMB,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
)
{
	FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);

    const Bool_WMV rgCBP = pmbmd->m_rgbCodedBlockPattern;
    Bool_WMV bTop, bBottom, bLeft, bRight;

    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;

    tWMVDecodeStatus tWMVStatus;
    // Y
    Buffer __huge* ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;;
    U8_WMV* ppxlcRefY = pWMVDec->m_ppxliRef0Y + ((yRefY >> 1) + EXPANDY_REFVOP) * pWMVDec->m_iWidthPrevY + (xRefY >> 1) + EXPANDY_REFVOP;
    Bool_WMV bXEvenY = !(xRefY & 1), bYEvenY = !(yRefY & 1);

    // UV
    Bool_WMV bXEvenUV = !(xRefUV & 1), bYEvenUV = !(yRefUV & 1);
    I32_WMV iPxLoc = ((yRefUV >> 1) + EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (xRefUV >> 1) + EXPANDUV_REFVOP;
    const U8_WMV* ppxlcRefU = pWMVDec->m_ppxliRef0U + iPxLoc;
    const U8_WMV* ppxlcRefV = pWMVDec->m_ppxliRef0V + iPxLoc;

    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    //CDCTTableInfo_Dec* pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec* pInterDCTTableInfo_Dec;
    if (pWMVDec->m_bDCTTable_MB){
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
	    //m_pIntraDCTTableInfo_Dec = m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
    }
    else{
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec;
    }

    // Y-blocks first (4 blocks)
    if (rgCBP & 32) {
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8); //get the quantized block      
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }
            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }
        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }                
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
	    (*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    } 
    else {
	    (*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    ppxlcCurrQYMB += BLOCK_SIZE;
    ppxlcRefY += BLOCK_SIZE;
    
    if (rgCBP & 16) {
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8); //get the quantized block       
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }
            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }

		(*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    else { 
		(*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY,  bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    ppxlcCurrQYMB += pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    ppxlcRefY += pWMVDec->m_iWidthPrevYxBlkMinusBlk;

    if (rgCBP & 8) {
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8); //get the quantized block       
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }
            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block     
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }                
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
		(*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    else {
	    (*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    ppxlcCurrQYMB += BLOCK_SIZE;
    ppxlcRefY += BLOCK_SIZE;

    if (rgCBP & 4) {
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8); //get the quantized block       
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }
            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
	    (*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    else {
		(*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }

    // UV

    // U-block
    if (rgCBP & 2) {
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8); //get the quantized block  
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }        
            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }


        (*pWMVDec->m_pMotionCompAndAddErrorUV) (
            pWMVDec, ppxlcCurrQUMB, ppxliErrorQMB, ppxlcRefU, 
            pWMVDec->m_iWidthPrevUV, bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);
    } 
    else 
        (*pWMVDec->m_pMotionCompUV) (pWMVDec, ppxlcCurrQUMB, ppxlcRefU, pWMVDec->m_iWidthPrevUV,  bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);

    // V-block
    if (rgCBP & 1) {
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8); //get the quantized block  
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            memset(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, pInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8); //get the quantized block      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
       (*pWMVDec->m_pMotionCompAndAddErrorUV) (
            pWMVDec, ppxlcCurrQVMB, ppxliErrorQMB, ppxlcRefV, 
            pWMVDec->m_iWidthPrevUV, bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);
    }
    else 
        (*pWMVDec->m_pMotionCompUV) (pWMVDec, ppxlcCurrQVMB, ppxlcRefV, pWMVDec->m_iWidthPrevUV, bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);
            
    FUNCTION_PROFILE_STOP(&fpDecode);
    return WMV_Succeeded;
}
#endif //_EMB_WMV2_

#define INTRADCYTCOEF_ESCAPE_MSV 119							// see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119							// see table.13/H.263
tWMVDecodeStatus DecodeIMBAcPred (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV*      ppxliTextureQMBY,
    U8_WMV*      ppxliTextureQMBU, 
    U8_WMV*      ppxliTextureQMBV, 
    I16_WMV*                piQuanCoefACPred, 
    I16_WMV**                piQuanCoefACPredTable, 
    Bool_WMV                bLeftMB, 
    Bool_WMV                bTopMB, 
    Bool_WMV                bLeftTopMB)
{
    tWMVDecodeStatus tWMVStatus;

    // Y-blocks first (4 blocks)
    U8_WMV * ppxliCodedBlkY = ppxliTextureQMBY;
    Bool_WMV bPredPattern;
    I16_WMV* pRef; 
    Bool_WMV bMBPattern[4] = {bLeftTopMB, bTopMB, bLeftMB, bLeftTopMB};
    U32_WMV ib;

    CDCTTableInfo_Dec* pInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec* pIntraDCTTableInfo_Dec;
    if (pWMVDec->m_bDCTTable_MB){
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
	    pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
    }
    else{
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec;
        pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec;
    }

    for (ib = 0; ib < 4; ib++) {
        decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV);

        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }

        pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bMBPattern[ib], bPredPattern);
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
        //pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] << 3);
        pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);

        tWMVStatus = decodeIntraBlockAcPred (
            pWMVDec, 
			pIntraDCTTableInfo_Dec,
            pmbmd, 
            ppxliCodedBlkY, 
            pWMVDec->m_iWidthPrevY,
            ib,
            piQuanCoefACPred,
            pRef,
            bPredPattern
            );
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }
        ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
        piQuanCoefACPred += BLOCK_SIZE_TIMES2;
        piQuanCoefACPredTable += 6;
    }

    // U-blocks
//#ifdef __MSV

    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV);
    
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB,bPredPattern);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
//    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] << 3);
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

//#ifdef __MSV
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
		pInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBU,
        pWMVDec->m_iWidthPrevUV,
        4,
        piQuanCoefACPred,
        pRef,
        bPredPattern
        );

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    // V-blocks
//#ifdef __MSV

    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB,bPredPattern);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
//    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] << 3);
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

//#ifdef __MSV
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
		pInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBV,
        pWMVDec->m_iWidthPrevUV,
        5,
        piQuanCoefACPred,
        pRef,
        bPredPattern
        );

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    return WMV_Succeeded;
}


tWMVDecodeStatus DecodeIMBAcPred_PFrame (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV*      ppxliTextureQMBY,
    U8_WMV*      ppxliTextureQMBU, 
    U8_WMV*      ppxliTextureQMBV, 
    I16_WMV*                piQuanCoefACPred, 
    I16_WMV**               piQuanCoefACPredTable, 
    Bool_WMV                bLeftMB, 
    Bool_WMV                bTopMB, 
    Bool_WMV                bLeftTopMB)
{
    Bool_WMV bLeftMBAndIntra, bLeftMBAndInter;
    Bool_WMV bTopMBAndIntra, bTopMBAndInter;
    Bool_WMV bLeftTopMBAndIntra, bLeftTopMBAndInter;
    Bool_WMV bMBPattern[4];
    CDCTTableInfo_Dec* pInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec* pIntraDCTTableInfo_Dec;

    // Y
    tWMVDecodeStatus tWMVStatus;
    U8_WMV * ppxliCodedBlkY = ppxliTextureQMBY;
    Bool_WMV bPredPattern;
    I16_WMV* pRef; 
    U32_WMV ib;

	if (bLeftMB){
		if ((pmbmd - 1)->m_dctMd == INTRA){
			bLeftMBAndIntra = TRUE_WMV;
			bLeftMBAndInter = FALSE_WMV;
		}
		else{
			bLeftMBAndIntra = FALSE_WMV;
			bLeftMBAndInter = TRUE_WMV;
		}
	}
	else{
		bLeftMBAndInter = bLeftMBAndIntra = FALSE_WMV;
	}

	if (bTopMB){
		if ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA){
			bTopMBAndIntra = TRUE_WMV;
			bTopMBAndInter = FALSE_WMV;
		}
		else{
			bTopMBAndIntra = FALSE_WMV;
			bTopMBAndInter = TRUE_WMV;
		}
	}
	else{
        bTopMBAndInter = bTopMBAndIntra = FALSE_WMV;
	}

	if (bLeftTopMB){
		if ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA){
			bLeftTopMBAndIntra = TRUE_WMV;
			bLeftTopMBAndInter = FALSE_WMV;
		}
		else{
			bLeftTopMBAndIntra = FALSE_WMV;
			bLeftTopMBAndInter = TRUE_WMV;
		}
	}
	else{
        bLeftTopMBAndInter = bLeftTopMBAndIntra = FALSE_WMV;
	}
	// bMBPattern is not used anymore.

    bMBPattern[0] = bLeftTopMBAndIntra;
    bMBPattern[1] = bTopMBAndIntra;
    bMBPattern[2] = bLeftMBAndIntra;
    bMBPattern[3] = bLeftTopMBAndIntra;


    if (pWMVDec->m_bDCTTable_MB){
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
	    pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
    }
    else{
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec;
        pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec;
    }


    // Y-blocks first (4 blocks)


    // Y0 block first 
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    ib = 0;

    bPredPattern = (pmbmd->m_rgbDCTCoefPredPattern & 2) >> 1;
    pRef = decodeDiffDCTCoef_WMV(pWMVDec, ppxliTextureQMBY, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, bLeftMBAndInter, bTopMBAndInter, bLeftTopMBAndInter, bPredPattern, pWMVDec->m_iWidthPrevY, pWMVDec->m_iDCStepSize);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
		pIntraDCTTableInfo_Dec,
        pmbmd, 
        ppxliCodedBlkY, 
        pWMVDec->m_iWidthPrevY,
        ib,
        piQuanCoefACPred,
        pRef,
        bPredPattern
        );
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    for (ib = 1; ib < 3; ib++) {
        decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV);
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }
        // Y_BLOCK1 = 1; Y_BLOCK2 = 2; Y_BLOCK3 = 3;

        bPredPattern = (pmbmd->m_rgbDCTCoefPredPattern & (1<<(ib+1))) >> (ib+1);

        //pRef = decodeDiffDCTCoef(piQuanCoefACPredTable, bLeftMBAndIntra, bTopMBAndIntra, bMBPattern[ib], bPredPattern);
        pRef = decodeDiffDCTCoef_WMV_Y23(piQuanCoefACPredTable, bLeftMBAndIntra, bTopMBAndIntra, bMBPattern[ib], bPredPattern);
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
        pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);
        tWMVStatus = decodeIntraBlockAcPred (
            pWMVDec, 
			pIntraDCTTableInfo_Dec,
            pmbmd, 
            ppxliCodedBlkY, 
            pWMVDec->m_iWidthPrevY,
            ib,
            piQuanCoefACPred,
            pRef,
            bPredPattern
            );
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }
        ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
        piQuanCoefACPred += BLOCK_SIZE_TIMES2;
        piQuanCoefACPredTable += 6;
    }

    // Y4
    ib = 3;
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMBAndIntra, bTopMBAndIntra, bMBPattern[ib], bPredPattern);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
		pIntraDCTTableInfo_Dec,
        pmbmd, 
        ppxliCodedBlkY, 
        pWMVDec->m_iWidthPrevY,
        ib,
        piQuanCoefACPred,
        pRef,
        bPredPattern
        );
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    // U-blocks
//#ifdef __MSV
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    bPredPattern = (pmbmd->m_rgbDCTCoefPredPattern & (1<<5))>>5;
    pRef = decodeDiffDCTCoef_WMV(pWMVDec, ppxliTextureQMBU, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, bLeftMBAndInter, bTopMBAndInter, bLeftTopMBAndInter, bPredPattern, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iDCStepSizeC);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

//#ifdef __MSV
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
		pInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBU,
        pWMVDec->m_iWidthPrevUV,
        4,
        piQuanCoefACPred,
        pRef,
        bPredPattern
        );

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    // V-blocks
//#ifdef __MSV
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    bPredPattern = (pmbmd->m_rgbDCTCoefPredPattern & (1<<6)) >> 6;
    pRef = decodeDiffDCTCoef_WMV(pWMVDec, ppxliTextureQMBV, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, bLeftMBAndInter, bTopMBAndInter, bLeftTopMBAndInter, bPredPattern, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iDCStepSizeC);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
		pInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBV,
        pWMVDec->m_iWidthPrevUV,
        5,
        piQuanCoefACPred,
        pRef,
        bPredPattern
        );

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    return WMV_Succeeded;
}


#ifndef _EMB_WMV2_
// _MIXEDPEL_

#define TWOPOINT(output, a, b)                  \
    output = rgiClapTab[(((I32_WMV)a + (I32_WMV)b + 1) >> 1)];

#define FOURPOINT(output, a, b, c, d)           \
    output = rgiClapTab[(I32_WMV)((9 * ((I32_WMV)b + (I32_WMV)c) - ((I32_WMV)a + (I32_WMV)d) + 8) >> 4)];

Void_WMV MotionCompMixed (
                        tWMVDecInternalMember *pWMVDec,
                        U8_WMV*              ppxlcPredMB,
                        const U8_WMV*        ppxlcRefMB,
                        I32_WMV                  iWidthPrev,
                        Bool_WMV                 bInterpolateX,
                        Bool_WMV                 bInterpolateY,
                        I32_WMV                  iMixedPelMV
                        )
{    
	FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMPMIXED_PROFILE);

    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U8_WMV* ppxlcPred = ppxlcPredMB;

    U8_WMV tempBlock[11 * 8 + 15];
    U8_WMV *alignedBlock = (U8_WMV*) (((int)tempBlock + 15) & ~15);
    
    if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            assert(iMixedPelMV == 0);
            I32_WMV iy;
            for (iy = 0; iy < 8; iy++) {
				memcpy(ppxlcPredMB,ppxlcRefMB,8);                
                ppxlcRefMB += iWidthPrev;
                ppxlcPredMB += iWidthPrev;
            }
        }
        else {  //bXSubPxl && !bYSubPxl            
            U8_WMV* pRef = (U8_WMV*) (ppxlcRefMB + 1);
            
            if (iMixedPelMV == 0) {
                I32_WMV iy; 
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pRef[ix-2], pRef[ix-1], pRef[ix], pRef[ix+1]);
                    }
                    ppxlcPred += iWidthPrev;
                    pRef += iWidthPrev;
                } 
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___
                I32_WMV iy;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pRef[ix-2], pRef[ix-1], pRef[ix], pRef[ix+1]);
                        TWOPOINT(ppxlcPred[ix], ppxlcPred[ix], pRef[ix]);
                    }

                    ppxlcPred  += iWidthPrev;
                    pRef += iWidthPrev;
                }
            }
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            U8_WMV* pRef = (U8_WMV*) (ppxlcRefMB + iWidthPrev);           
            const U8_WMV* pLine4 = pRef + iWidthPrev;
            const U8_WMV* pLine3 = pRef;
            const U8_WMV* pLine2 = pLine3 - iWidthPrev;
            const U8_WMV* pLine1 = pLine2 - iWidthPrev;        
            
            if (iMixedPelMV == 0) {   
                I32_WMV iy;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix; 
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                    }
                    
                    ppxlcPred += iWidthPrev;
                    pLine1 += iWidthPrev;
                    pLine2 += iWidthPrev;
                    pLine3 += iWidthPrev;
                    pLine4 += iWidthPrev;
                }

            } else {
                //      H Q H' 
                //      I                        
                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 1], pLine[ix], pLine[ix + 1], pLine[ix + 2]);
                    }

                    pLine += iWidthPrev;
                    pBlock += 8;
                }
            
                pBlock = alignedBlock;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        I32_WMV iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);                        
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(ppxlcPred[ix], iDiagResult, iVertResult);
                    }

                    ppxlcPred += iWidthPrev;
                    pBlock += 8;
                    pLine1 += iWidthPrev;
                    pLine2 += iWidthPrev;
                    pLine3 += iWidthPrev;
                    pLine4 += iWidthPrev; 
                }     

            }
        }
        else { // bXSubPxl && bYSubPxl
            U8_WMV* pRef = (U8_WMV*) (ppxlcRefMB + iWidthPrev + 1);             
            const U8_WMV* pLine4 = pRef + iWidthPrev;
            const U8_WMV* pLine3 = pRef;
            const U8_WMV* pLine2 = pLine3 - iWidthPrev;
            const U8_WMV* pLine1 = pLine2 - iWidthPrev;                
            if (iMixedPelMV == 0) {

                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 2], pLine[ix - 1], pLine[ix], pLine[ix + 1]);
                    }
                    pLine += iWidthPrev;
                    pBlock += 8;
                }
            
                pBlock = alignedBlock;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                     }
                    ppxlcPred += iWidthPrev;
                    pBlock += 8;
                }   
            } else {
                // H Q H
                //     I

                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 2], pLine[ix - 1], pLine[ix], pLine[ix + 1]);
                    }
                    pLine += iWidthPrev;
                    pBlock += 8;
                }
            
                pBlock = alignedBlock;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        I32_WMV iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(ppxlcPred[ix], iDiagResult, iVertResult);
                     }
                    ppxlcPred += iWidthPrev;
                    pBlock += 8;
                    pLine1 += iWidthPrev;
                    pLine2 += iWidthPrev;
                    pLine3 += iWidthPrev;
                    pLine4 += iWidthPrev; 
                }  

            }
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}

Void_WMV MotionCompMixedAndAddError (
                                   tWMVDecInternalMember *pWMVDec,
                                   U8_WMV* ppxlcCurrQMB, 
                                   const Buffer __huge* ppxliErrorBuf, 
                                   const U8_WMV* ppxlcRef, 
                                   I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven, 
                                   I32_WMV iMixedPelMV
                                   )
{
	FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMPMIXEDADDERROR_PROFILE);

    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 

    U8_WMV tempBlock[11 * 8 + 15];
    U8_WMV *alignedBlock = (U8_WMV*) (((int)tempBlock + 15) & ~15);

    const PixelI32 *ppxliErrorQMB = ppxliErrorBuf->i32;
    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        ppxlcCurrQMB [ix] = rgiClapTab [ppxliErrorQMB [ix] + ppxlcRef [ix]];
                    }
                    ppxlcRef += iWidthFrm;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }

            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //         ___           				
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, ppxlcRef[ix - 1], ppxlcRef[ix], ppxlcRef[ix + 1], ppxlcRef[ix + 2]);
                        TWOPOINT(iResult, iResult, ppxlcRef[ix]);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                    ppxlcRef += iWidthFrm;
                }    

            }
        }
        else {  //bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {   
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, ppxlcRef[ix - 1], ppxlcRef[ix], ppxlcRef[ix + 1], ppxlcRef[ix + 2]);
                        ppxlcCurrQMB [ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    ppxlcRef += iWidthFrm;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }

            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___               
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, ppxlcRef[ix - 1], ppxlcRef[ix], ppxlcRef[ix + 1], ppxlcRef[ix + 2]);
                        TWOPOINT(iResult, iResult, ppxlcRef[ix + 1]);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + 
                            iResult];	
                    }
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                    ppxlcRef += iWidthFrm;
                }   

            }
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
            if (iMixedPelMV == 0) {

                const U8_WMV* pLine1 = ppxlcRef - iWidthFrm;
                const U8_WMV* pLine2 = ppxlcRef;
                const U8_WMV* pLine3 = pLine2 + iWidthFrm;
                const U8_WMV* pLine4 = pLine3 + iWidthFrm;   
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {  
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {   
                        I32_WMV iResult;
                        FOURPOINT(iResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        ppxlcCurrQMB [ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    pLine1 += iWidthFrm;
                    pLine2 += iWidthFrm;
                    pLine3 += iWidthFrm;
                    pLine4 += iWidthFrm;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }

            } else {
                //      H Q H' 
                //      I          

                U8_WMV* pRef = (U8_WMV*) (ppxlcRef + iWidthFrm);                   
                const U8_WMV* pLine4 = pRef + iWidthFrm; 
                const U8_WMV* pLine3 = pRef;
                const U8_WMV* pLine2 = pLine3 - iWidthFrm;
                const U8_WMV* pLine1 = pLine2 - iWidthFrm;
                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 1], pLine[ix], pLine[ix + 1], pLine[ix + 2]);
                    }
                    pLine += iWidthFrm;
                    pBlock += 8;
                }
                pBlock = alignedBlock;
                
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult, iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(iResult, iDiagResult, iVertResult);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    pBlock += 8;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                    pLine1 += iWidthFrm;
                    pLine2 += iWidthFrm;
                    pLine3 += iWidthFrm;
                    pLine4 += iWidthFrm;
                }   

            }
        }
        else { // bXSubPxl && bYSubPxl
            if (iMixedPelMV == 0) {

                U8_WMV* pRef = (U8_WMV*) ppxlcRef; 
                const U8_WMV* pLine1 = pRef - iWidthFrm;
                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 1], pLine[ix], pLine[ix + 1], pLine[ix + 2]);
                    }
                    pLine += iWidthFrm;
                    pBlock += 8;
                }
                pBlock = alignedBlock;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    pBlock += 8;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }      

            } else {
                // H Q H
                //     I
                //alignedBlock

                U8_WMV* pRef = (U8_WMV*) (ppxlcRef + iWidthFrm + 1); 
                const U8_WMV* pLine4 = pRef + iWidthFrm;
                const U8_WMV* pLine3 = pRef;
                const U8_WMV* pLine2 = pLine3 - iWidthFrm;
                const U8_WMV* pLine1 = pLine2 - iWidthFrm;

                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix- 2], pLine[ix - 1], pLine[ix], pLine[ix + 1]);
                    }
                    pLine += iWidthFrm;
                    pBlock += 8;
                }
                pBlock = alignedBlock;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult, iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(iResult, iDiagResult, iVertResult);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];

                    }
                    pBlock += 8;
                    ppxliErrorQMB += BLOCK_SIZE;
                    ppxlcCurrQMB += iWidthFrm;
                    pLine4 += iWidthFrm;
                    pLine3 += iWidthFrm;
                    pLine2 += iWidthFrm;
                    pLine1 += iWidthFrm;
                }      

            }
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}
#endif //_EMB_WMV2_
