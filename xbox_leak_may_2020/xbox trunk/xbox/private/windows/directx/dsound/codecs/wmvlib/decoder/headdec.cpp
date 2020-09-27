/*************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

	vopmbEnc.cpp

Abstract:

	Encoder VOP composed of MB's

Author:

	Ming-Chieh Lee (mingcl@microsoft.com)
	Bruce Lin (blin@microsoft.com)
			
	20-March-1996

Revision History:

*************************************************************************/
#include "bldsetup.h"

#include "xplatform.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "typedef.hpp"
#include "strmdec_wmv.hpp"
#include "wmvdec_member.h"
#include "wmvdec_function.h"

tWMVDecodeStatus decodeMBOverheadOfIVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    CWMVMBMode* pmbmdTop = pmbmd - pWMVDec->m_uintNumMBX;
    CWMVMBMode* pmbmdLeftTop = pmbmdTop - 1;
    CWMVMBMode* pmbmdLeft = pmbmd - 1;
    I32_WMV iCBPCY;
    I32_WMV iPredCBPY1, iPredCBPY2, iPredCBPY3, iPredCBPY4; 
    
    if (imbY == 0) {
        pmbmdTop = pWMVDec->m_pmbmdZeroCBPCY;
        pmbmdLeftTop = pWMVDec->m_pmbmdZeroCBPCY;
    }
    if (imbX == 0) {
        pmbmdLeft = pWMVDec->m_pmbmdZeroCBPCY;
        pmbmdLeftTop = pWMVDec->m_pmbmdZeroCBPCY;
    }

    iCBPCY = Huffman_WMV_get (&pWMVDec->m_hufICBPCYDec, pWMVDec->m_pbitstrmIn);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
		return WMV_CorruptedBits;
	}
	if (iCBPCY < 0 || iCBPCY > 63) {
		return WMV_CorruptedBits;
	}

    pmbmd->m_rgbDCTCoefPredPattern = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
    
	if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
		return WMV_CorruptedBits;
	}
	pmbmd->m_bSkip = FALSE_WMV;
	pmbmd->m_dctMd = INTRA;
    
    I32_WMV iLeftTopCBPY4, iTopCBPY3, iTopCBPY4, iLeftCBPY2, iLeftCBPY4;
    
    iLeftTopCBPY4 = (pmbmdLeftTop->m_rgbCodedBlockPattern & (1<<2)) >> 2;
    iTopCBPY3 = (pmbmdTop->m_rgbCodedBlockPattern & (1<<3)) >> 3;
    iTopCBPY4 = (pmbmdTop->m_rgbCodedBlockPattern & (1<<2)) >> 2;
    iLeftCBPY2 = (pmbmdLeft->m_rgbCodedBlockPattern & (1<<4)) >> 4;
    iLeftCBPY4 = (pmbmdLeft->m_rgbCodedBlockPattern & (1<<2)) >> 2;

    iPredCBPY1 = (iLeftTopCBPY4 == iTopCBPY3 ) ? iLeftCBPY2 : iTopCBPY3;
    iPredCBPY1 ^= ((iCBPCY >> 5) & 0x00000001);

    iPredCBPY2 = (iTopCBPY3 == iTopCBPY4)? iPredCBPY1 : iTopCBPY4;
    iPredCBPY2 ^= ((iCBPCY >> 4) & 0x00000001);

    iPredCBPY3 = (iLeftCBPY2 == iPredCBPY1) ? iLeftCBPY4 : iPredCBPY1;
    iPredCBPY3 ^= ((iCBPCY >> 3) & 0x00000001);

    iPredCBPY4 = (iPredCBPY1 == iPredCBPY2)? iPredCBPY3: iPredCBPY2;
    iPredCBPY4 ^= ((iCBPCY >> 2) & 0x00000001);

    pmbmd->m_rgbCodedBlockPattern = (iPredCBPY1<<5)  | (iPredCBPY2<<4) |
        (iPredCBPY3<<3) | (iPredCBPY4<<2) | (iCBPCY & 0x00000003);

    //
    iCBPCY = iPredCBPY1 | iPredCBPY2 | iPredCBPY3 | iPredCBPY4 | iCBPCY & 0x00000003;
    // 0 :  0
    // 1 : 10
    // 2 : 11
    pmbmd->m_iDCTTable_MB_Index = 0;
    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBit (pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit (pWMVDec->m_pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
    return WMV_Succeeded;
}

tWMVDecodeStatus decodeMBOverheadOfPVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd)
{
    I32_WMV iCBPCY = 0;
	if (pWMVDec->m_bCODFlagOn) {

        if (pWMVDec->m_bSKIPBIT_CODING_ == FALSE) {
            pmbmd->m_bSkip = BS_getBit (pWMVDec->m_pbitstrmIn);
            if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
                return WMV_CorruptedBits;
            }
        }
		if (pmbmd->m_bSkip) {
            pmbmd->m_rgbCodedBlockPattern = 0;
			pmbmd->m_dctMd = INTER;
			return WMV_Succeeded;
		}
	}
	else
		pmbmd->m_bSkip = FALSE_WMV; // might be reset later

    if (pWMVDec->m_bNEW_PCBPCY_TABLE) {
	    iCBPCY  = Huffman_WMV_get (pWMVDec->m_pHufNewPCBPCYDec, pWMVDec->m_pbitstrmIn);
    } else {
        iCBPCY  = Huffman_WMV_get (&pWMVDec->m_hufPCBPCYDec, pWMVDec->m_pbitstrmIn);
    }

    if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
		return WMV_CorruptedBits;
	}
	if (iCBPCY < 0 || iCBPCY > 127) {
		return WMV_CorruptedBits;
	}

    if (iCBPCY & 0x00000040){ // INTER MB iCBPCY == CBPCY + 64
        pmbmd->m_dctMd = INTER;
        iCBPCY ^= 0x00000040;
		pmbmd->m_bCBPAllZero = (iCBPCY == 0);
    //_HYBRID_MV_
    if (pWMVDec->m_bMBHybridMV)
        pmbmd->m_iMVPredDirection = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
    else
        pmbmd->m_iMVPredDirection = 2;

    }
    else{
        pmbmd->m_dctMd = INTRA;

        pmbmd->m_rgbDCTCoefPredPattern = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);

        // YU = 00 :0 
        // YU = 01 :110
        // YU = 10 :10
        // YU = 11 :111
        if (pWMVDec->m_bDCPred_IMBInPFrame){
            I32_WMV ibYUV_IPred;
            ibYUV_IPred = BS_getBit (pWMVDec->m_pbitstrmIn);
            if (!ibYUV_IPred){
				// [V_BLOCK] = [U_BLOCK] = [Y_BLOCK1] = 0
                pmbmd->m_rgbDCTCoefPredPattern &= 0x9D;
            }
            else{
                ibYUV_IPred = BS_getBit (pWMVDec->m_pbitstrmIn);
                if (!ibYUV_IPred){
					// [Y_BLOCK1] = 1
					// [V_BLOCK] = [U_BLOCK] = 0
                    pmbmd->m_rgbDCTCoefPredPattern |= 0x02;
                    pmbmd->m_rgbDCTCoefPredPattern &= 0x9F;
                }
                else if (ibYUV_IPred = BS_getBit (pWMVDec->m_pbitstrmIn)){
					// [V_BLOCK] = [U_BLOCK] = [Y_BLOCK1] = 1
                    pmbmd->m_rgbDCTCoefPredPattern |= 0x62;

                }else{
					// [Y_BLOCK1] = 0
					// [V_BLOCK] = [U_BLOCK] = 1
                    pmbmd->m_rgbDCTCoefPredPattern &= 0xFD;
                    pmbmd->m_rgbDCTCoefPredPattern |= 0x60;
                }
            }

            pmbmd->m_rgbDCTCoefPredPattern &= 0xFB; // [Y_BLOCK2] = 0;
            pmbmd->m_rgbDCTCoefPredPattern |= 0x08; // [Y_BLOCK3] = 1;
        }
        


		if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
			return WMV_CorruptedBits;
		}
    }

    // 0 :  0
    // 1 : 10
    // 2 : 11
    pmbmd->m_iDCTTable_MB_Index = 0;
    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBit (pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit (pWMVDec->m_pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    


    pmbmd->m_rgbCodedBlockPattern = (U8_WMV) iCBPCY; // iCBPC info is already in iCBPCY. 

    if (pWMVDec->m_bXformSwitch)
    {
        pmbmd->m_bBlkXformSwitchOn = FALSE_WMV;

        if (pWMVDec->m_bMBXformSwitching && !pmbmd->m_bCBPAllZero && pmbmd->m_dctMd == INTER)
        {
            if ( !(pmbmd->m_bBlkXformSwitchOn = BS_getBit (pWMVDec->m_pbitstrmIn)) )
            {
                // Block-based transform-switching must not be enabled for this MB so get the
                // transform-type that will be applied to all blocks in this MB
                if (BS_getBit (pWMVDec->m_pbitstrmIn) == 0)
                    pmbmd->m_iMBXformMode = XFORMMODE_8x8;
                else if (BS_getBit (pWMVDec->m_pbitstrmIn) == 0)
                    pmbmd->m_iMBXformMode = XFORMMODE_8x4;
                else
                    pmbmd->m_iMBXformMode = XFORMMODE_4x8;            
            }
        }
    }
    return WMV_Succeeded;
}

#define MVFLCSIZE 6
#define MV_ESCAPE 1099

inline I8_WMV medianof3 (I8_WMV a0, I8_WMV a1, I8_WMV a2)
{
	if (a0 > a1) {
		if (a1 > a2)
			return a1;
		else if (a0 > a2)
			return a2;
		else
			return a0;
	}
	else if (a0 > a2)
		return a0;
	else if (a1 > a2)
		return a2;
	else
		return a1;
}


Void_WMV find16x16MVpred (tWMVDecInternalMember *pWMVDec, CVector& vecPredHalfPel, const CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry) 
{
	CVector vctCandMVHalfPel0, vctCandMVHalfPel1, vctCandMVHalfPel2;

	if (bLeftBndry)
		vctCandMVHalfPel0.x = vctCandMVHalfPel0.y = 0;
	else
		vctCandMVHalfPel0 = *(pmv - 1);

	if (bTopBndry) {
		vecPredHalfPel = vctCandMVHalfPel0;
		return;
	}
	else {
		vctCandMVHalfPel1 = *(pmv - pWMVDec->m_iOffsetToTopMB);
		if (bRightBndry)
			vctCandMVHalfPel2.x = vctCandMVHalfPel2.y = 0;
		else
			vctCandMVHalfPel2 = *(pmv - pWMVDec->m_iOffsetToTopMB + 1);
	}
	vecPredHalfPel.x = medianof3 (vctCandMVHalfPel0.x, vctCandMVHalfPel1.x, vctCandMVHalfPel2.x);
	vecPredHalfPel.y = medianof3 (vctCandMVHalfPel0.y, vctCandMVHalfPel1.y, vctCandMVHalfPel2.y);

    if (pmbmd->m_iMVPredDirection != 2) {
        vecPredHalfPel = (pmbmd->m_iMVPredDirection == 0)? vctCandMVHalfPel0 : vctCandMVHalfPel1;
    }
}


tWMVDecodeStatus decodeMVMSV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry, Huffman_WMV* pHufMVDec, U8_WMV* uXMvFromIndex, U8_WMV* uYMvFromIndex)
{
    CVector vctPred;
    I32_WMV lSymbolx, lSymboly, lJoinSymbol, iVLC;
	
    find16x16MVpred (pWMVDec, vctPred, pmv, pmbmd, bLeftBndry, bRightBndry, bTopBndry);
    lJoinSymbol = Huffman_WMV_get (pHufMVDec, pWMVDec->m_pbitstrmIn);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
		return WMV_CorruptedBits;
	}

    if (lJoinSymbol != MV_ESCAPE){
        lSymbolx = uXMvFromIndex[lJoinSymbol];
        lSymboly = uYMvFromIndex[lJoinSymbol];
    }
    else{
        lSymbolx = BS_getBits (pWMVDec->m_pbitstrmIn, MVFLCSIZE);
        lSymboly = BS_getBits (pWMVDec->m_pbitstrmIn, MVFLCSIZE);
    }

	iVLC = lSymbolx - 32 + vctPred.x;
	if (iVLC > pWMVDec->m_iMVRightBound)
		pmv->x = iVLC - 64;
	else if (iVLC < pWMVDec->m_iMVLeftBound)
		pmv->x = iVLC + 64;
	else
		pmv->x = (I8_WMV) iVLC;

	iVLC = lSymboly - 32 + vctPred.y;
	if (iVLC > pWMVDec->m_iMVRightBound)
		pmv->y = iVLC - 64;
	else if (iVLC < pWMVDec->m_iMVLeftBound)
		pmv->y = iVLC + 64;
	else
		pmv->y = (I8_WMV) iVLC;

	pWMVDec->m_iMixedPelMV = 0;
	if (pWMVDec->m_iMvResolution == 1) {
		if ( ((pmv->x) & 1) != 0 || ((pmv->y) & 1) != 0) {
			pWMVDec->m_iMixedPelMV = BS_getBits (pWMVDec->m_pbitstrmIn, 1);
		} 
	}

	return WMV_Succeeded;
}


Bool_WMV decideHybridMVOn (tWMVDecInternalMember *pWMVDec, CoordI x, Bool_WMV bNot1stRowInSlice, const CVector* pmv)
{
    if (pWMVDec->m_bFrmHybridMVOn && x != 0 && bNot1stRowInSlice && !pWMVDec->m_iMvResolution) {
        CVector vctMvLeft = *(pmv - 1);
        CVector vctMvUp = *(pmv - pWMVDec->m_iOffsetToTopMB);
        I32_WMV iMaxDeltaMV = max(abs(vctMvLeft.x - vctMvUp.x), abs(vctMvLeft.y - vctMvUp.y));;
        return (iMaxDeltaMV >= VAR_FOR_HYBRID_MV);
    } else
        return FALSE_WMV;
}
