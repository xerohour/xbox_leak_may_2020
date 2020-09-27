#include "bldsetup.h"

#include "xplatform.h"
#include "typedef.hpp"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "strmdec_wmv.hpp"
#include "codehead.h"
#include "dectable_mp4x.hpp"

#ifdef DO_BOE
#define BOE DebugBreak()
#else
#define BOE	
#endif


#ifdef _WMVONLY_
    // Dummy MP4X's routines.
    tWMVDecodeStatus DecodeIMBAcPred_MP4X(tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, U8_WMV* ppxliTextureQMBY, U8_WMV* ppxliTextureQMBU, U8_WMV* ppxliTextureQMBV, 
        I16_WMV* piQuanCoefACPred, I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB){return WMV_Succeeded;};
    Void_WMV decodeIntraDCAcPred (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC){};
    tWMVDecodeStatus decodeMBOverheadOfIVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY){return WMV_Succeeded;};
    tWMVDecodeStatus decodeMBOverheadOfPVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd){return WMV_Succeeded;};
    tWMVDecodeStatus decodeMV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry){return WMV_Succeeded;};
    I32_WMV checkResyncMarker(tWMVDecInternalMember *pWMVDec){return FALSE_WMV;};
    tWMVDecodeStatus decodeMBOverheadOfIVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY){return WMV_Succeeded;};
    tWMVDecodeStatus decodeMBOverheadOfPVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd){return WMV_Succeeded;};
    Void_WMV decodeIntraDCAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC){};
    tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode){return WMV_Succeeded;};
    tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MMX_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode){return WMV_Succeeded;};
    tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, Bool_WMV CBP, I16_WMV* pDct, U8_WMV* piZigzagInv){return WMV_Succeeded;};
    tWMVDecodeStatus decodeVOLHeadMP4S(tWMVDecInternalMember *pWMVDec){return WMV_Succeeded;};

    I32_WMV	decodeVideoPacketHeader(tWMVDecInternalMember *pWMVDec, I32_WMV& iCurrentQP){return	TRUE_WMV;};
#else
// -------- MP4S - MP43
#define MAXCODEWORDLNGMV 13
#define MAXCODEWORDLNGMCBPCintra 9
#define MAXCODEWORDLNGMCBPCintra_New 3
#define MAXCODEWORDLNGMCBPY 6
#define MAXCODEWORDLNGMCBPCinter 9
#define MAXCODEWORDLNGMCBPCinter_New 7
#define MAXCODEWORDLNGMCBPY 6


#define MAXCODEWORDLNGIntraDCy 7
#define MAXCODEWORDLNGIntraDCc 8
#define MAXCODEWORDLNGIntraDCMPEG4y 11
#define MAXCODEWORDLNGIntraDCMPEG4c 12
#define INTRADCYTCOEF_ESCAPE_MSV 119							// see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119							// see table.13/H.263

#define sign(x) ((x) > 0 ? 1 : -1)

I32_WMV divroundnearest(I32_WMV i, I32_WMV iDenom)
{
    assert(iDenom>0);
    if(i>=0)
        return (i+(iDenom>>1))/iDenom;
    else
        return (i-(iDenom>>1))/iDenom;
}


tWMVDecodeStatus DecodeIMBAcPred_MP4X (
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
    CDCTTableInfo_Dec* pInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec* pIntraDCTTableInfo_Dec;
    tWMVDecodeStatus tWMVStatus;
    U8_WMV * ppxliCodedBlkY = ppxliTextureQMBY;
    Bool_WMV bPredPattern;
    I16_WMV* pRef; 
    Bool_WMV bMBPattern[4] = {bLeftTopMB, bTopMB, bLeftMB, bLeftTopMB};
    U32_WMV ib;

    if (pWMVDec->m_bDCTTable_MB){
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd-> m_iDCTTable_MB_Index];
	    pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd-> m_iDCTTable_MB_Index];
    }
    else{
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec;
        pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec;
    }

    // Y-blocks first (4 blocks)
    for (ib = 0; ib < 4; ib++) {
        if (pWMVDec->m_cvCodecVersion >= MP43){
            decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV);
        }
        else{
            if(pWMVDec->m_cvCodecVersion == MP42) 
                decodeIntraDCAcPred (pWMVDec, &tWMVStatus, gDecodeCodeTableIntraDCy, MAXCODEWORDLNGIntraDCy);
            else
                decodeIntraDCAcPred_MPEG4 (pWMVDec, &tWMVStatus, gDecodeCodeTableIntraDCMPEG4y, MAXCODEWORDLNGIntraDCMPEG4y);
        }
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }

        pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bMBPattern[ib], bPredPattern);
        if (pWMVDec->m_cvCodecVersion != MP4S) {
            piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
            pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);
        }
        else {
            piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + divroundnearest(pRef[0], pWMVDec->m_iDCStepSize);
            pWMVDec->m_rgiCoefRecon[0] = piQuanCoefACPred [0] = piQuanCoefACPred[0] * pWMVDec->m_iDCStepSize;
        }
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
    if (pWMVDec->m_cvCodecVersion >= MP43){
        decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV);
    }
    else{
        if (pWMVDec->m_cvCodecVersion == MP42) 
            decodeIntraDCAcPred (pWMVDec, &tWMVStatus, gDecodeCodeTableIntraDCc, MAXCODEWORDLNGIntraDCc);
        else
            decodeIntraDCAcPred_MPEG4 (pWMVDec, &tWMVStatus, gDecodeCodeTableIntraDCMPEG4c, MAXCODEWORDLNGIntraDCMPEG4c);
    }
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB,bPredPattern);
    if(pWMVDec->m_cvCodecVersion != MP4S) {
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
        pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);
    }
    else {
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + divroundnearest(pRef[0], pWMVDec->m_iDCStepSizeC);
        pWMVDec->m_rgiCoefRecon[0] = piQuanCoefACPred [0] = piQuanCoefACPred[0] * pWMVDec->m_iDCStepSizeC;
    }

    if (pWMVDec->m_cvCodecVersion != MP4S)
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
    else
        tWMVStatus = decodeIntraBlockAcPred (
            pWMVDec, 
		    pIntraDCTTableInfo_Dec,
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
    if (pWMVDec->m_cvCodecVersion >= MP43){
        decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV);
    }
    else{
        if(pWMVDec->m_cvCodecVersion == MP42)
            decodeIntraDCAcPred (pWMVDec, &tWMVStatus, gDecodeCodeTableIntraDCc, MAXCODEWORDLNGIntraDCc);
        else 
            decodeIntraDCAcPred_MPEG4 (pWMVDec, &tWMVStatus, gDecodeCodeTableIntraDCMPEG4c, MAXCODEWORDLNGIntraDCMPEG4c);
    }
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB,bPredPattern);
    if(pWMVDec->m_cvCodecVersion != MP4S) {
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
        pWMVDec->m_rgiCoefRecon[0] = (pWMVDec, piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);
    }
    else {
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + divroundnearest(pRef[0], pWMVDec->m_iDCStepSizeC);
        pWMVDec->m_rgiCoefRecon[0] =  piQuanCoefACPred [0] = piQuanCoefACPred[0] * pWMVDec->m_iDCStepSizeC;
    }

    if (pWMVDec->m_cvCodecVersion != MP4S)
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
    else
        tWMVStatus = decodeIntraBlockAcPred (
            pWMVDec, 
		    pIntraDCTTableInfo_Dec,
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

/// MV
tWMVDecodeStatus decodeMV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry)
{
    pWMVDec->m_iMixedPelMV = pWMVDec->m_iMvResolution = 0;
    CVector vctPred;
	find16x16MVpred (pWMVDec, vctPred, pmv, pmbmd, bLeftBndry, bRightBndry, bTopBndry);

    if (pWMVDec->m_cvCodecVersion != MP4S) {
        I32_WMV iVLC;
        I32_WMV lSymbol = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMV,gDecodeCodeTableMV);
	    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
	    	return WMV_CorruptedBits;
    	}
	    iVLC = lSymbol - 32 + vctPred.x;
    	if (iVLC > pWMVDec->m_iMVRightBound)
	    	pmv->x = iVLC - 64;
    	else if (iVLC < pWMVDec->m_iMVLeftBound)
	    	pmv->x = iVLC + 64;
    	else
		    pmv->x = (I8_WMV) iVLC;

	    lSymbol = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMV,gDecodeCodeTableMV);
    	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
	    	return WMV_CorruptedBits;
    	}
	    iVLC = lSymbol - 32 + vctPred.y;
    	if (iVLC > pWMVDec->m_iMVRightBound)
	    	pmv->y = iVLC - 64;
    	else if (iVLC < pWMVDec->m_iMVLeftBound)
		    pmv->y = iVLC + 64;
	    else
		    pmv->y = (I8_WMV) iVLC;
        return WMV_Succeeded;
    }
    else {
        CVector vctDiff, vctDecode;
        getDiffMV(pWMVDec, vctDiff);
        vctDecode.x = vctDiff.x + vctPred.x;
        vctDecode.y = vctDiff.y + vctPred.y;
        fitMvInRange (vctDecode, pWMVDec->iRange);
        pmv->x = vctDecode.x;
        pmv->y = vctDecode.y;
        return WMV_Succeeded;
    }
}

Void_WMV decodeIntraDCAcPred (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC)//CHuffmanDecoder* pentrdecIntraDC)
{
    I8_WMV lSzDiffIntraDC;
	lSzDiffIntraDC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, iMaxCodeWordLngIntraDC, DecodeCodeTableIntraDC);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn))
        goto Error;
    if (lSzDiffIntraDC)	{
        U8_WMV chDiffIntraDC = (U8_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, lSzDiffIntraDC);
        if (BS_invalid ( pWMVDec->m_pbitstrmIn))
            goto Error;
	    *pError = WMV_Succeeded;
        if ((1 << (lSzDiffIntraDC - 1)) & chDiffIntraDC)
            pWMVDec->m_rgiCoefRecon [0] = chDiffIntraDC;
		else
            pWMVDec->m_rgiCoefRecon [0] = (chDiffIntraDC - (1 << lSzDiffIntraDC) + 1);
    }
	else{
		*pError = WMV_Succeeded;
		pWMVDec->m_rgiCoefRecon [0] = 0;
	}
	return;

Error:
    *pError = WMV_CorruptedBits;
	pWMVDec->m_rgiCoefRecon [0] = 0;
}

tWMVDecodeStatus decodeMBOverheadOfIVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
	I32_WMV iCBPY = 0;
	I32_WMV iCBPC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPCintra_New, gDecodeCodeTableMCBPCintra_New);

	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	if (iCBPC < 0 || iCBPC > 3) {
		return WMV_CorruptedBits;
	}

	pmbmd-> m_rgbDCTCoefPredPattern = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	pmbmd-> m_bSkip = FALSE_WMV;
	pmbmd-> m_dctMd = INTRA;
	iCBPY = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY, gDecodeCodeTableCBPY);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
    }

    pmbmd->m_rgbCodedBlockPattern = (iCBPY << 2) | iCBPC;

	return WMV_Succeeded;
}
tWMVDecodeStatus decodeMBOverheadOfPVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd)
{
    I32_WMV iMCBPC = 0;
    I32_WMV iCBPC = 0;
    I32_WMV iCBPY = 0;
    I32_WMV iMBtype;

	if (pWMVDec->m_bCODFlagOn) {
		pmbmd-> m_bSkip = BS_getBit (pWMVDec->m_pbitstrmIn);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			return WMV_CorruptedBits;
		}

		if (pmbmd-> m_bSkip) {
            pmbmd-> m_rgbCodedBlockPattern = 0;
			pmbmd-> m_dctMd = INTER;
			return WMV_Succeeded;
		}
	}
	else
		pmbmd-> m_bSkip = FALSE_WMV; // might be reset later
	iMCBPC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPCinter_New, gDecodeCodeTableMCBPCinter_New);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	if (iMCBPC < 0 || iMCBPC > 7) {
		return WMV_CorruptedBits;
	}

	iMBtype = iMCBPC >> 2;
	iCBPC = iMCBPC % 4;
	switch (iMBtype) {			
	case 0:
		pmbmd-> m_dctMd = INTER;
		if (iCBPC == 3)
			iCBPY = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY,gDecodeCodeTableCBPY);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		else
			iCBPY = 15 - BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY,gDecodeCodeTableCBPY);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		pmbmd-> m_bCBPAllZero = (iCBPC == 0 && iCBPY == 0);
		break;
	case 1:
        pmbmd-> m_dctMd = INTRA;
		pmbmd-> m_rgbDCTCoefPredPattern = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			return WMV_CorruptedBits;
		}
		iCBPY = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY, gDecodeCodeTableCBPY);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
        break;
    default:
        return WMV_CorruptedBits;
    }

	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	if (iCBPY < 0 || iCBPY > 15) {
		return WMV_CorruptedBits;
    }

    pmbmd->m_rgbCodedBlockPattern = (iCBPY << 2) | iCBPC;
    //  Make it work with WMV2 codes
    pmbmd-> m_iMVPredDirection = 2;
    
    return WMV_Succeeded;
}


//// MPEG4_ISO


tWMVDecodeStatus decodeMBOverheadOfIVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
	I32_WMV iCBPY = 0;
	I32_WMV iCBPC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPCintra,gDecodeCodeTableMCBPCintra);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);


	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	if (iCBPC < 0 || iCBPC > 20) {
		return WMV_CorruptedBits;
	}


	pmbmd-> m_rgbDCTCoefPredPattern = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	pmbmd-> m_bSkip = FALSE_WMV;
	pmbmd-> m_dctMd = INTRA;
	iCBPY = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY, gDecodeCodeTableCBPY);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
    }

    pmbmd->m_rgbCodedBlockPattern = (iCBPY << 2) | iCBPC;

	return WMV_Succeeded;
}

tWMVDecodeStatus decodeMBOverheadOfPVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd)
{
    I32_WMV iMCBPC = 0;
    I32_WMV iCBPC = 0;
    I32_WMV iCBPY = 0;
    I32_WMV iMBtype;

	pmbmd-> m_bSkip = BS_getBit (pWMVDec->m_pbitstrmIn);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	if (pmbmd-> m_bSkip) {
        pmbmd-> m_rgbCodedBlockPattern = 0;
		pmbmd-> m_dctMd = INTER;
		return WMV_Succeeded;
	}
	else
		pmbmd-> m_bSkip = FALSE_WMV; // might be reset later
	iMCBPC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPCinter,gDecodeCodeTableMCBPCinter);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	if (iMCBPC < 0 || iMCBPC > 20) {
		return WMV_CorruptedBits;
	}

    iMBtype = iMCBPC >> 2;
	iCBPC = iMCBPC % 4;
	switch (iMBtype) {			
	case 0:
		pmbmd-> m_dctMd = INTER;
			iCBPY = 15 - BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY,gDecodeCodeTableCBPY);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		pmbmd-> m_bCBPAllZero = (iCBPC == 0 && iCBPY == 0);
		break;
	case 3:
        pmbmd-> m_dctMd = INTRA;
		pmbmd-> m_rgbDCTCoefPredPattern = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			return WMV_CorruptedBits;
		}
		iCBPY = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY, gDecodeCodeTableCBPY);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
        break;
    default:
        return WMV_CorruptedBits;
    }

	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		return WMV_CorruptedBits;
	}

	if (iCBPY < 0 || iCBPY > 15) {
		return WMV_CorruptedBits;
    }

    pmbmd->m_rgbCodedBlockPattern = (iCBPY << 2) | iCBPC;


//  Make it work with WMV2 codes
    pmbmd-> m_iMVPredDirection = 2;
    return WMV_Succeeded;
}

I32_WMV checkResyncMarker(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV nBitsPeeked;
    I32_WMV iStuffedBits;
    if (pWMVDec->m_bResyncDisable)
        return FALSE_WMV;

	
	iStuffedBits = BS_peekBitsTillByteAlign ( pWMVDec->m_pbitstrmIn, nBitsPeeked);
	assert (nBitsPeeked > 0 && nBitsPeeked <= 8);
	if (iStuffedBits == ((1 << (nBitsPeeked - 1)) - 1))
		return (BS_peekBitsFromByteAlign (pWMVDec->m_pbitstrmIn, NUMBITS_VP_RESYNC_MARKER + pWMVDec->uiFCode - 1)
				== RESYNC_MARKER);
	return FALSE_WMV;
}

I32_WMV	decodeVideoPacketHeader(tWMVDecInternalMember *pWMVDec, I32_WMV& iCurrentQP)
{
    U32_WMV uiResyncMarker;
    I32_WMV	NumOfMB;
    I32_WMV iVal;
    I32_WMV iLengthOfMBNumber;
    U32_WMV uiMBNumber;
    I32_WMV stepDecoded;
    U32_WMV uiHEC;

	BS_flushMPEG4(pWMVDec->m_pbitstrmIn, 8);
	uiResyncMarker = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VP_RESYNC_MARKER+pWMVDec->uiFCode - 1);

	NumOfMB = pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY;
	assert (NumOfMB>0);

	iVal = NumOfMB - 1;
	iLengthOfMBNumber = 0;
	for(; iVal; iLengthOfMBNumber++)
		iVal>>=1;

	uiMBNumber = 0;
	if(NumOfMB>1)
		uiMBNumber = BS_getBits  ( pWMVDec->m_pbitstrmIn, iLengthOfMBNumber);
	
	pWMVDec->m_iVPMBnum = uiMBNumber;

    stepDecoded = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VP_QUANTIZER);
    iCurrentQP = stepDecoded; 

	uiHEC = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VP_HEC);
	if (uiHEC){

		// Time reference and VOP_pred_type
		I32_WMV iModuloInc = 0;
        Time tCurrSec, tVopIncr;
        U32_WMV uiMarker;
        tFrameType_WMV tFrameType;
        I32_WMV	iIntraDcSwitchThr; 
        
		while (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1) != 0)
			iModuloInc++;
		tCurrSec = iModuloInc + pWMVDec->m_tOldModuloBaseDecd;
		uiMarker = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		assert(uiMarker == 1);
		tVopIncr = BS_getBits  ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNumBitsTimeIncr);
		uiMarker = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		assert(uiMarker == 1);
		// this is bogus - swinder.
		//assert (pWMVDec->m_t == tCurrSec * 60 + tVopIncr * 60 / pWMVDec->m_volmd.iClockRate); //in terms of 60Hz clock ticks

		tFrameType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VP_PRED_TYPE);

		iIntraDcSwitchThr = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VP_INTRA_DC_SWITCH_THR);
			if (pWMVDec->m_tFrmType == PVOP) {
				U32_WMV uiFCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_FCODE);
			}
	}
	return	TRUE_WMV;
}

/// MV


I32_WMV deScaleMV (I32_WMV iVLC, I32_WMV iResidual, I32_WMV iScaleFactor)
{
    if (iVLC == 0 && iResidual == 0)
        return 0;
    else if (iScaleFactor == 1)
        return (iVLC);
    else {
        I32_WMV iAbsDiffMVcomponent = abs (iVLC) * iScaleFactor + iResidual - iScaleFactor + 1; //changed a'c to enc
        return (sign (iVLC) * iAbsDiffMVcomponent);
    }
}

Void_WMV fitMvInRange (CVector& vctSrc, I32_WMV iMvRange)
{
    if (vctSrc.x < -1 * iMvRange)                   //* 2 to get length of [-range, range]
        vctSrc.x += 2 * iMvRange;
    else if (vctSrc.x >= iMvRange)
        vctSrc.x  -= 2 * iMvRange;

    if (vctSrc.y < -1 * iMvRange)
        vctSrc.y += 2 * iMvRange;
    else if (vctSrc.y >= iMvRange)
        vctSrc.y  -= 2 * iMvRange;
}

Void_WMV getDiffMV (tWMVDecInternalMember *pWMVDec, CVector& vctDiffMV)     //get half pel
{
    I32_WMV iResidual;
    I32_WMV lSymbol = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMV,gDecodeCodeTableMV);
    I32_WMV iVLC = lSymbol - 32;
    if (iVLC != 0)
        iResidual = BS_getBits  ( pWMVDec->m_pbitstrmIn, pWMVDec->uiFCode - 1);
    else
        iResidual = 0;
    vctDiffMV.x  = (I8_WMV) deScaleMV (iVLC, iResidual, pWMVDec->iScaleFactor);

    lSymbol = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMV,gDecodeCodeTableMV);
    iVLC = lSymbol - 32;
    if (iVLC != 0)
        iResidual = BS_getBits  ( pWMVDec->m_pbitstrmIn, pWMVDec->uiFCode - 1);
    else
        iResidual = 0;
    vctDiffMV.y  = (I8_WMV) deScaleMV (iVLC, iResidual, pWMVDec->iScaleFactor);
}


Void_WMV decodeIntraDCAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC)//CHuffmanDecoder* pentrdecIntraDC)
{
    I8_WMV lSzDiffIntraDC;
	lSzDiffIntraDC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, iMaxCodeWordLngIntraDC, DecodeCodeTableIntraDC);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn))
        goto Error;
    if (lSzDiffIntraDC!=0)	{
        if(lSzDiffIntraDC <= 8) {
            U8_WMV chDiffIntraDC = (U8_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, lSzDiffIntraDC);
            if (BS_invalid ( pWMVDec->m_pbitstrmIn))
                goto Error;
            *pError = WMV_Succeeded;
            if (!((1 << (lSzDiffIntraDC - 1)) & chDiffIntraDC))
                pWMVDec->m_rgiCoefRecon [0] = -1 * (( 0x00FF >> (8 - lSzDiffIntraDC)) & (~chDiffIntraDC));
	        else
                pWMVDec->m_rgiCoefRecon [0] = (I32_WMV)chDiffIntraDC;
        }
        else {
            U32_WMV uiDiffIntraDC =
                (U32_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, lSzDiffIntraDC);
            if (BS_invalid ( pWMVDec->m_pbitstrmIn))
                goto Error;
            *pError = WMV_Succeeded;
            if (!((1 << (lSzDiffIntraDC - 1)) & uiDiffIntraDC))
                pWMVDec->m_rgiCoefRecon [0] = -1 * ((0xFFFF >> (16 - lSzDiffIntraDC)) & (~uiDiffIntraDC));
            else
                pWMVDec->m_rgiCoefRecon [0] = (I32_WMV) uiDiffIntraDC;
            BS_getBit (pWMVDec->m_pbitstrmIn); // get marker bit
        }
    }
	else{
		*pError = WMV_Succeeded;
		pWMVDec->m_rgiCoefRecon [0] = 0;
	}
//     fprintf(fp_head,"dc=%d %d ", pWMVDec->m_rgiCoefRecon[0], lSzDiffIntraDC);
	return;

Error:
    *pError = WMV_CorruptedBits;
	pWMVDec->m_rgiCoefRecon [0] = 0;
}

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred_MPEG4 (
    tWMVDecInternalMember *pWMVDec, 
	CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, 
	Bool_WMV CBP, 
	I16_WMV* pDct, 
	U8_WMV* piZigzagInv 	
)
{
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
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
    I32_WMV i;

	if (CBP) {
        I32_WMV iMaxAC = (1<<11) -1; 
		do {
			lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
			//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, iMAXCODEWORDLNGDCT,decodeCodeTableDCT);
            assert(lIndex <= iTCOEF_ESCAPE);
			if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
				return WMV_CorruptedBits;
			}
			if (lIndex != iTCOEF_ESCAPE)	{
				bIsLastRun = (lIndex >= iStartIndxOfLastRun);
				uiRun = rgRunAtIndx[lIndex];
				iLevel = (BS_getBit (pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
					rgLevelAtIndx[lIndex];
				if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
					return WMV_CorruptedBits;
				}
			}
			else {
				if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
    					BOE;
                        return WMV_CorruptedBits;
                    }
					// ESC + '0' + VLC
					//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
					//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
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
					if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
						iLevel = -iLevel;
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                        return WMV_CorruptedBits;
                    }
				}
				else if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {    					
                        return WMV_CorruptedBits;
                    }
					// ESC + '10' + VLC
					//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
					//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
			            return WMV_CorruptedBits;
		            }
					uiRun = rgRunAtIndx [lIndex];
					iLevel = rgLevelAtIndx[lIndex];
					if (lIndex >= iStartIndxOfLastRun){
						bIsLastRun = TRUE_WMV;
						uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel]+1);
					}
					else
						uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel]+1);
					if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
						iLevel = -iLevel;
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                        return WMV_CorruptedBits;
                    }
				}
				else{
					// ESC + '11' + FLC
					bIsLastRun = (Bool_WMV) BS_getBit (pWMVDec->m_pbitstrmIn); // escape decoding
					uiRun = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);			
				    BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
					iLevel = (I32_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, 12);
                    assert (iLevel != iMaxAC +1);
                    if(iLevel > iMaxAC)
                        iLevel -= (1<<12);
                    assert (iLevel != 0);
				    BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
		            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			            return WMV_CorruptedBits;
		            }
				}
			}
//             fprintf(fp_head,"(%d %d)", uiRun,iLevel);
            assert(uiRun<= 63);
			uiCoefCounter += uiRun;

			if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
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
//     fprintf(fp_head,"\n");
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
	
    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MMX_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode)
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
	U32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
	I32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec->iTcoef_ESCAPE;

    Bool_WMV bIsLastRun = FALSE_WMV;
	U32_WMV uiRun; // = 0;
    I32_WMV	 iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
    //Align on cache line
//    I16_WMV *rgiCoefRecon = (I16_WMV*)(((I32_WMV)pWMVDec->m_rgiCoefRecon+32)&0xFFFFFFE0);
    I16_WMV *rgiCoefRecon = (I16_WMV*)pWMVDec->m_rgiCoefRecon;
	register I32_WMV iDCTHorzFlags = 0;

    memset (pWMVDec->m_rgiCoefRecon, 0, (size_t) MB_SQUARE_SIZE);
    I32_WMV iMaxAC = (1<<11) -1; 
    do {
		//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
        lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
		//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			return WMV_CorruptedBits;
		}
        //lIndex = pWMVDec->m_pentrdecDCT->decodeSymbol();
        // if Huffman
        if (lIndex != iTCOEF_ESCAPE)	{
			if (lIndex >= iStartIndxOfLastRun)
				bIsLastRun = TRUE_WMV;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit (pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                return WMV_CorruptedBits;
            }
        } 
		else {
			if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
				// ESC + '0' + VLC
        		//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
				//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
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
				if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
			}
			else if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
				// ESC + '10' + VLC
        		//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
				//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
			        return WMV_CorruptedBits;
		        }
				uiRun = rgRunAtIndx[lIndex];
				iLevel = rgLevelAtIndx[lIndex];
				if (lIndex >= iStartIndxOfLastRun){
					bIsLastRun = TRUE_WMV;
					uiRun += (rgIfLastNumOfRunAtLevel[iLevel]+1);
				}
				else
					uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel]+1);
				if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
			}
			else{
				// ESC + '11' + FLC
				bIsLastRun = (Bool_WMV) BS_getBit (pWMVDec->m_pbitstrmIn); // escape decoding
				uiRun =	BS_getBits (pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);			
				BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
				iLevel = (I32_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, 12);
                assert (iLevel != iMaxAC +1);
                if(iLevel > iMaxAC)
                    iLevel -= (1<<12);
				BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
		        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			        return WMV_CorruptedBits;
		        }
			}
        }
//        fprintf(fp_head,"(%d %d)", uiRun,iLevel);
        uiCoefCounter += uiRun;

		if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
			BOE;
			return WMV_CorruptedBits;
        }
        register I32_WMV iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];
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
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode)
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
	U32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
	I32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec->iTcoef_ESCAPE;

    Bool_WMV bIsLastRun = FALSE_WMV;
	U32_WMV uiRun; // = 0;
    I32_WMV	 iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
	register I32_WMV iDCTHorzFlags = 0;

#if !defined(_WIN32) && !defined(_Embedded_x86)
    assert(BLOCK_SQUARE_SIZE_2 <= UINT_MAX);
#endif
    memset (pWMVDec->m_rgiCoefRecon, 0, (size_t) MB_SQUARE_SIZE);
    I32_WMV iMaxAC = (1<<11) -1; 
    do {
		//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
        lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
		//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			return WMV_CorruptedBits;
		}
        //lIndex = pWMVDec->m_pentrdecDCT->decodeSymbol();
        // if Huffman
        if (lIndex != iTCOEF_ESCAPE)	{
			if (lIndex >= iStartIndxOfLastRun)
				bIsLastRun = TRUE_WMV;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit (pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                return WMV_CorruptedBits;
            }
        } 
		else {
			if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
				// ESC + '0' + VLC
        		//lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
				//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
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
				if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
			}
			else if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
				// ESC + '10' + VLC
                //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
				//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
			        return WMV_CorruptedBits;
		        }
				uiRun = rgRunAtIndx[lIndex];
				iLevel = rgLevelAtIndx[lIndex];
				if (lIndex >= iStartIndxOfLastRun){
					bIsLastRun = TRUE_WMV;
					uiRun += (rgIfLastNumOfRunAtLevel[iLevel]+1);
				}
				else
					uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel]+1);
				if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
					iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
			}
			else{
				// ESC + '11' + FLC
				bIsLastRun = (Bool_WMV) BS_getBit (pWMVDec->m_pbitstrmIn); // escape decoding
				uiRun =	BS_getBits (pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);			
				BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
				iLevel = (I32_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, 12);
                assert (iLevel != iMaxAC +1);
                if(iLevel > iMaxAC)
                    iLevel -= (1<<12);
				BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
		        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			        return WMV_CorruptedBits;
		        }
			}
        }
        uiCoefCounter += uiRun;
//         fprintf(fp_head,"(%d %d)", uiRun,iLevel);

		if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
			return WMV_CorruptedBits;
        }

        register I32_WMV iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];
		if (iIndex & 0x7)
			iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

		if (iLevel == 1)
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = pWMVDec->m_i2DoublePlusStepSize;
		else if (iLevel == -1)
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = pWMVDec->m_i2DoublePlusStepSizeNeg;
		else if (iLevel > 0)
//			I32_WMV iCoefRecon = pWMVDec->m_iStepSize * ((iLevel << 1) + 1) - pWMVDec->m_bStepSizeIsEven;
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
		else
//			I32_WMV iCoefRecon = pWMVDec->m_bStepSizeIsEven - pWMVDec->m_iStepSize * (1 - (iLevel << 1));
			pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;

		uiCoefCounter++;
    } while (!bIsLastRun);
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
#ifdef __MACVIDEO__
    pWMVDec->m_iDCTHorzFlags = 0xffffffff;
#endif
    return WMV_Succeeded;
}

tWMVDecodeStatus decodeVOLHeadMP4S(tWMVDecInternalMember *pWMVDec)
{
//	assert (pWMVDec->m_cvCodecVersion == MP4S || pWMVDec->m_cvCodecVersion == mp4s);

    I32_WMV iCodeWord, iMarkerBit;
    U32_WMV uiVoStartCode, uiVoID, uiVolStartCode, volID, uiOLType, uiOLI;
    U32_WMV iAspectRatio, uiCTP, uiAUsage, uiTimeIncrRes, uiSprite;
    Bool_WMV bRandom, bFixFrameRate, bInterlaced, bOBMCDisable, bNot8BitVideo;
    I32_WMV iQuantizer, bScalability;
    Bool_WMV bComplexityEstDisable, bDummyDP;

    iCodeWord = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_START_CODE_PREFIX);
    if (iCodeWord != START_CODE_PREFIX)
        return WMV_CorruptedBits;

    uiVoStartCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VO_START_CODE);
    if (uiVoStartCode != VO_START_CODE) 
        return WMV_CorruptedBits;

    uiVoID = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VO_ID);
    
    iCodeWord = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_START_CODE_PREFIX);
    if (iCodeWord != START_CODE_PREFIX)
        return WMV_CorruptedBits;

    uiVolStartCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOL_START_CODE);
    if (uiVolStartCode != VOL_START_CODE) 
        return WMV_CorruptedBits;
    volID = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOL_ID);

    bRandom = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    if (bRandom != FALSE)  // randopWMVDec->m_access
        return WMV_CorruptedBits;
    uiOLType = BS_getBits  ( pWMVDec->m_pbitstrmIn, 8); //VOL_Type_Indication
    uiOLI = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // iis-Object-layer-identifier
    if (uiOLI != 0) 
        return WMV_CorruptedBits;
    iAspectRatio = BS_getBits  ( pWMVDec->m_pbitstrmIn, 4); // aspect ratio
    if (iAspectRatio != 1) 
        return WMV_CorruptedBits;
    uiCTP = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // control parameter
    if (uiCTP != 0) 
        return WMV_CorruptedBits;
    uiAUsage = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOL_SHAPE);

    iMarkerBit = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // marker bit
    if (iMarkerBit != 1) 
        return WMV_CorruptedBits;

    uiTimeIncrRes = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_TIME_RESOLUTION);
	if (uiTimeIncrRes == 0)
		return WMV_CorruptedBits;
	pWMVDec->m_iClockRate = uiTimeIncrRes;
	// changed this back to old way because we cant change encoder - swinder
	for (pWMVDec->m_iNumBitsTimeIncr = 1; pWMVDec->m_iNumBitsTimeIncr < 16; pWMVDec->m_iNumBitsTimeIncr++) {
		if (uiTimeIncrRes == 1)
			break;
		uiTimeIncrRes >>= 1;
	}

    iMarkerBit = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // marker bit
    if (iMarkerBit != 1) 
        return WMV_CorruptedBits;
    bFixFrameRate = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // fixed_vop_rate
    if (bFixFrameRate != FALSE) 
        return WMV_CorruptedBits;
    iMarkerBit = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // marker bit
    if (iMarkerBit != 1) 
        return WMV_CorruptedBits;
    pWMVDec->m_iFrmWidthSrc = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_WIDTH);
    iMarkerBit  = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    if (iMarkerBit != 1) 
        return WMV_CorruptedBits;
    pWMVDec->m_iFrmHeightSrc = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_HEIGHT);
    iMarkerBit  = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    if (iMarkerBit != 1) 
        return WMV_CorruptedBits;
    
    bInterlaced = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // interlace
    if (bInterlaced != FALSE) 
        return WMV_CorruptedBits;
  
    bOBMCDisable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // obmc-disable
    if (bOBMCDisable != TRUE_WMV) 
        return WMV_CorruptedBits;

    uiSprite = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SPRITE_USAGE); // sprite used?
    if (uiSprite != 0) 
        return WMV_CorruptedBits;
 
    bNot8BitVideo = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // 8-bit video
    if (bNot8BitVideo != FALSE) 
        return WMV_CorruptedBits;

    if (bNot8BitVideo != FALSE) 
        return WMV_CorruptedBits;
//    else
//        pWMVDec->m_volmd.uiQuantPrecision = 5;
    
    iQuantizer = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    if (iQuantizer  == TRUE_WMV) // loadable quant table
        return WMV_CorruptedBits;
    
    bComplexityEstDisable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // 
    if (bComplexityEstDisable == FALSE) 
        return WMV_CorruptedBits;

	pWMVDec->m_bResyncDisable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); //  resync marker
	bDummyDP = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    if (bDummyDP == TRUE_WMV)
        return WMV_CorruptedBits;

    bScalability = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); //  scalable? 0
    if (bScalability == TRUE_WMV) 
        return WMV_CorruptedBits;

    BS_flushMPEG4 (pWMVDec->m_pbitstrmIn);
    setSliceCode (pWMVDec, 0);
    return WMV_Succeeded;
}
#endif


#ifdef _MPG4_
/// V1
I32_WMV CVideoObjectDecoder::DecodeIMB (
    tWMVDecInternalMember *pWMVDec, 
    const Bool_WMV*         rgCBP,
    U8_WMV*      ppxliTextureQMBY,
    U8_WMV*      ppxliTextureQMBU,
    U8_WMV*      ppxliTextureQMBV)
{
    CDCTTableInfo_Dec* pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec* pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec;
    //CDCTTableInfo_Dec* pInterDCTTableInfo_Dec = &InterDCTTableInfo_Dec_MPEG4;
    //CDCTTableInfo_Dec* pIntraDCTTableInfo_Dec = &IntraDCTTableInfo_Dec_MPEG4;

    I32_WMV result;

    // Y-blocks first (4 blocks)
    U8_WMV * ppxliCodedBlkY = ppxliTextureQMBY;
    for (U32_WMV ib = 0; ib < 4; ib++) {
        decodeIntraDCY(&result);
        if (ICERR_OK != result) {
            BOE;
            return result;
        }
        pWMVDec->m_rgiCoefRecon [0] = (PixelI32)(pWMVDec->m_rgiDCRef [0] * pWMVDec->m_iDCStepSize);

        result = decodeIntraBlock (
            pIntraDCTTableInfo_Dec,
            rgCBP[ib],
            ppxliCodedBlkY,
            pWMVDec->m_iWidthPrevY
            );

        //result = decodeIntraBlock (rgCBP[ib], ppxliCodedBlkY, pWMVDec->m_iWidthPrevY, pWMVDec->m_pentrdecDCTY, grgLevelAtIndxOfIntraY, grgRunAtIndxOfIntraY, STARTINDEXOFLASTRUNOFINTRAY);
        if (ICERR_OK != result) {
            BOE;
            return result;
        }
        ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    }

    // U-blocks
    decodeIntraDCU (&result);
    if (ICERR_OK != result) {
        BOE;
        return result;
    }

    pWMVDec->m_rgiCoefRecon [0] = (PixelI32) (pWMVDec->m_rgiDCRef [1] << 3);

    result = decodeIntraBlock(
        pInterDCTTableInfo_Dec,
        rgCBP [4],
        ppxliTextureQMBU,
        pWMVDec->m_iWidthPrevUV
        );

    //result = decodeIntraBlock(rgCBP [4], ppxliTextureQMBU, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pentrdecDCT, grgLevelAtIndx, grgRunAtIndx, STARTINDEXOFLASTRUN);
    if (ICERR_OK != result) {
        BOE;
        return result;
    }

    // V-blocks
    decodeIntraDCV (&result);

    if (ICERR_OK != result) {
        BOE;
        return result;
    }

    pWMVDec->m_rgiCoefRecon [0] = (PixelI32) (pWMVDec->m_rgiDCRef [2] << 3);

    result = decodeIntraBlock(
        pInterDCTTableInfo_Dec,
        rgCBP [5],
        ppxliTextureQMBV,
        pWMVDec->m_iWidthPrevUV
        );

    //result = decodeIntraBlock(rgCBP [5], ppxliTextureQMBV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pentrdecDCT, grgLevelAtIndx, grgRunAtIndx, STARTINDEXOFLASTRUN);

    if (ICERR_OK != result) {
        BOE;
        return result;
    }

    return ICERR_OK;
}

I32_WMV CVideoObjectDecoder::decodeMBOverheadOfIVOP_Old (tWMVDecInternalMember *pWMVDec, CMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPC = 0;
    I32_WMV iCBPY = 0;
	I32_WMV iMCBPC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPCintra,gDecodeCodeTableMCBPCintra);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);

	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		BOE;
		return ICERR_ERROR;
	}
	if (iMCBPC < 0 || iMCBPC > 7) {
		BOE;
		return ICERR_ERROR;
	}

    pmbmd-> m_bSkip = FALSE_WMV;
	pmbmd-> m_dctMd = INTRA;
	iCBPC = iMCBPC % 4;
	iCBPY = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY,gDecodeCodeTableCBPY);
	if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
		BOE;
		return ICERR_ERROR;
    }

    pmbmd->m_rgbCodedBlockPattern = (iCBPY << 2) | iCBPC;


	return ICERR_OK;
}

I32_WMV CVideoObjectDecoder::decodeMBOverheadOfPVOP_Old (tWMVDecInternalMember *pWMVDec, CMBMode* pmbmd)
{
    I32_WMV iMCBPC = 0;
    I32_WMV iCBPC = 0;
    I32_WMV iCBPY = 0;
    pmbmd-> m_bSkip = BS_getBit (pWMVDec->m_pbitstrmIn);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
        BOE;
        return ICERR_ERROR;
    }

	if (pmbmd-> m_bSkip) {
        pmbmd-> m_rgbCodedBlockPattern = 0;
		pmbmd-> m_dctMd = INTER;
		return ICERR_OK;
	}
	else {
		iMCBPC = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPCinter,gDecodeCodeTableMCBPCinter);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			BOE;
			return ICERR_ERROR;
		}

		if (iMCBPC < 0 || iMCBPC > 20) {
			BOE;
			return ICERR_ERROR;
		}
		pmbmd-> m_dctMd = (DCTMode) (iMCBPC >> 2);
        iCBPC = iMCBPC % 4;
		iCBPY = 15 - BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGMCBPY,gDecodeCodeTableCBPY);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
			BOE;
			return ICERR_ERROR;
		}
		if (iCBPY < 0 || iCBPY > 15) {
			BOE;
			return ICERR_ERROR;
		}
	}

    pmbmd->m_rgbCodedBlockPattern = (iCBPY << 2) | iCBPC;
  
    return ICERR_OK;
}

I32_WMV CVideoObjectDecoder::decodeIntraBlock (
    tWMVDecInternalMember *pWMVDec, 
	CDCTTableInfo_Dec* IntraDCTTableInfo_Dec, 
    Bool_WMV         CBP,                    //= 0 if no AC coef
    U8_WMV *ppxliTextureQMB,        //place to set the pixels
    I32_WMV          iOffsetToNextRowForDCT
)
{
    I32_WMV result;

	if (CBP) {
#if !defined(_WIN32)
        assert(BLOCK_SQUARE_SIZE_2MINUS2 <= UINT_MAX);
#endif
        memset (pWMVDec->m_rgiCoefReconPlus1, 0, (size_t) BLOCK_SQUARE_SIZE_2MINUS2);
        result = DecodeInverseIntraBlockQuantize (IntraDCTTableInfo_Dec);							//get the quantized block	
		if (ICERR_OK != result) {
			BOE;
			return result;
		}
		(*pWMVDec->m_pIntraIDCT_Dec) (ppxliTextureQMB, iOffsetToNextRowForDCT, pWMVDec->m_rgiCoefRecon);
	}
	else {
		//I32_WMV iWidth = iOffsetToNextRowForDCT;
		// loop through ppxliTextureQMBV 0 replaced by a const
		// Take another look at negative rounding. /blin
		U8_WMV tmp = (U8_WMV) ((pWMVDec->m_rgiCoefRecon [0] + 4) >> 3);
		for (U8_WMV i = 0; i < BLOCK_SIZE; i++) {
#if !defined(_WIN32)
			assert(BLOCK_SIZE <= UINT_MAX);
#endif
			memset (ppxliTextureQMB, tmp, (size_t) BLOCK_SIZE);
			ppxliTextureQMB += iOffsetToNextRowForDCT;
		}
	}
    return ICERR_OK;
}

I32_WMV CVideoObjectDecoder::DecodeInverseIntraBlockQuantize (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* IntraDCTTableInfo_Dec)
{
	Huffman* hufDCTACDec = IntraDCTTableInfo_Dec->hufDCTACDec;
	I8_WMV* rgLevelAtIndx = IntraDCTTableInfo_Dec->pcLevelAtIndx;
	U8_WMV* rgRunAtIndx = IntraDCTTableInfo_Dec->puiRunAtIndx;
//  Change iStartIndxOfLastRun to iStartIndxOfLastRunMinus1 and <= to <
//	iStartIndxOfLastRun = IntraDCTTableInfo_Dec->iStartIndxOfLastRunMinus1;
	U32_WMV iStartIndxOfLastRun = IntraDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
	I32_WMV iTCOEF_ESCAPE = IntraDCTTableInfo_Dec->iTcoef_ESCAPE;

    Bool_WMV bIsLastRun;
    U32_WMV uiRun; // = 0;
    I8_WMV iLevel; // = 0;
    U32_WMV uiCoefCounter = 1;
    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;

    do {
		//lIndex = hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
        lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
		//lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
		if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            BOE;
            return ICERR_ERROR;
        }
        if (lIndex != iTCOEF_ESCAPE)	{
            bIsLastRun = (lIndex >= iStartIndxOfLastRun);
            uiRun = rgRunAtIndx [lIndex];
            iLevel = (BS_getBit (pWMVDec->m_pbitstrmIn)) ? -rgLevelAtIndx [lIndex] :
                rgLevelAtIndx[lIndex];
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                BOE;
                return ICERR_ERROR;
            }
        }
		else {
            bIsLastRun = (Bool_WMV) BS_getBit (pWMVDec->m_pbitstrmIn); // escape decoding
            uiRun = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);			
			iLevel = (I8_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
			if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
				BOE;
				return ICERR_ERROR;
			}
        }
        uiCoefCounter += uiRun;

        if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
            BOE;
            return ICERR_ERROR;
        }

		if (iLevel > 0)
//			I32_WMV iCoefRecon = pWMVDec->m_iStepSize * ((iLevel << 1) + 1) - pWMVDec->m_bStepSizeIsEven;
            pWMVDec->m_rgiCoefRecon [grgiZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
        else 
//			I32_WMV iCoefRecon = pWMVDec->m_bStepSizeIsEven - pWMVDec->m_iStepSize * (1 - (iLevel << 1));
			pWMVDec->m_rgiCoefRecon [grgiZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;
        uiCoefCounter++;
    } while (!bIsLastRun);

    return ICERR_OK;
}

#endif
