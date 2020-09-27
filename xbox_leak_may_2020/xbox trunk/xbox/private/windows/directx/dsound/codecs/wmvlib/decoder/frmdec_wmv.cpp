#include "bldsetup.h"

#include "xplatform.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "strmdec_wmv.hpp"
#include "codehead.h"

// mingcl: don't delete these two.  These are for hacking the display of the video info in the player.
int g_iBitRate = 0;
int g_iFrameRate = 0;

Void_WMV WMVideoDecAssignMotionCompRoutines (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_iMvResolution == 1) {
        pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompMixed;
        pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompMixedAndAddError;
    } else {
        if (pWMVDec->m_bRndCtrlOn){
            if (pWMVDec->m_iRndCtrl){
                pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompRndCtrlOn;
                pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOn;
#ifdef _EMB_WMV2_
                pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_ON;
#endif
            }
            else{
                pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompRndCtrlOff;
                pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff;
#ifdef _EMB_WMV2_
                
                pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_OFF;
                
#endif
            }
#ifdef DYNAMIC_EDGEPAD
            pWMVDec->m_tEdgePad.m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;
#endif
        }
    }

    if (pWMVDec->m_bRndCtrlOn){
        if (pWMVDec->m_iRndCtrl){
            pWMVDec->m_pMotionCompUV = pWMVDec->m_pMotionCompRndCtrlOn;
            pWMVDec->m_pMotionCompAndAddErrorUV = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOn;
#ifdef _EMB_WMV2_
            
            pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_ON;


            
#endif
        }
        else{
            pWMVDec->m_pMotionCompUV = pWMVDec->m_pMotionCompRndCtrlOff;
            pWMVDec->m_pMotionCompAndAddErrorUV = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff;
#ifdef _EMB_WMV2_
            
            pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_OFF;
            
#endif
        }

#ifdef DYNAMIC_EDGEPAD
        pWMVDec->m_tEdgePad.m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;
#endif
    }
}

Void_WMV WMVideoDecSwapCurrAndRef (tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV*  pTmp = pWMVDec->m_pfrmCurrQ;
    pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmPrev;
    pWMVDec->m_pfrmPrev = pTmp;
    
    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmPrev->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmPrev->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmPrev->m_pucVPlane;
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

    pWMVDec->m_ppxliCurrQPlusExpY = (U8_WMV*) pWMVDec->m_pfrmCurrQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpU = (U8_WMV*) pWMVDec->m_pfrmCurrQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpV = (U8_WMV*) pWMVDec->m_pfrmCurrQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
}

#if DBG
extern DWORD g_TotalBits;

extern "C"
ULONG
_cdecl
DbgPrint(
    PCH Format,
    ...
    );

DWORD g_IFrameCount;

#endif

tWMVDecodeStatus WMVideoDecDecodeI (tWMVDecInternalMember *pWMVDec)
{
    FUNCTION_PROFILE_DECL_START(fp,DECODEI_PROFILE);
    tWMVDecodeStatus tWMVStatus;

    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQPlusExpY;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQPlusExpU;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQPlusExpV;

    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    I16_WMV* piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    I16_WMV** piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;
    U32_WMV imbY;
    
#if DBG
    g_IFrameCount++;
#endif DBG

    if (pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACInterTableIndx];
	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACIntraTableIndx];
        pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
        pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
    }
    else{
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[2];
	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2];
    }

    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);

    HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeI",1);

    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
        U8_WMV* ppxliCodedY = ppxliCurrQY;
        U8_WMV* ppxliCodedU = ppxliCurrQU;
        U8_WMV* ppxliCodedV = ppxliCurrQV;
        Bool_WMV bNot1stRowInSlice;
        U32_WMV imbX;

        if (!(imbY & 0x01)){ 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
        }
        if (pWMVDec->m_cvCodecVersion >= WMV1 || pWMVDec->m_cvCodecVersion == MP4S) // THREE or STANDARD
            bNot1stRowInSlice = (imbY != 0);
        else 
            bNot1stRowInSlice = (imbY % pWMVDec->m_uintNumMBYSlice != 0);

#ifdef _MPG4_
        pWMVDec->m_rgiDCRef [0] = pWMVDec->m_rgiDCRef [1] = pWMVDec->m_rgiDCRef [2] = 128;
#endif
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {

#if DBG
            g_TotalBits = 0;
#endif DBG

#if DBG && 0

if (g_IFrameCount == 4 && imbX < 8 && imbY == 18)
{
    _asm int 3;
}

#endif DBG

            if(pWMVDec->m_cvCodecVersion == MP4S) {
                if(checkResyncMarker(pWMVDec)) {
                    decodeVideoPacketHeader(pWMVDec, pWMVDec->m_iStepSize);
                    uiNumMBFromSliceBdry = 0;
                }
            }

            uiNumMBFromSliceBdry++;
            tWMVStatus = (*pWMVDec->m_pDecodeMBOverheadOfIVOP) (pWMVDec, pmbmd, imbX, imbY);
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }

            {
                Bool_WMV bLeft = (imbX != 0) && (uiNumMBFromSliceBdry > 1);
                Bool_WMV bTop = bNot1stRowInSlice && (uiNumMBFromSliceBdry > pWMVDec->m_uintNumMBX);
                Bool_WMV bLeftTop = (imbX != 0) && bNot1stRowInSlice && (uiNumMBFromSliceBdry > (pWMVDec->m_uintNumMBX+1));
                tWMVStatus = (*pWMVDec->m_pDecodeIMBAcPred) (
                    pWMVDec,
                    pmbmd,
                    ppxliCodedY,
                    ppxliCodedU,
                    ppxliCodedV, 
                    piQuanCoefACPred, 
                    piQuanCoefACPredTable, 
                    bLeft,
                    bTop,
                    bLeftTop);
            }
//              (imbX != 0), 
//              bNot1stRowInSlice,
//              (imbX != 0 && bNot1stRowInSlice));

#if DBG && 0

if (g_IFrameCount == 4 && imbX < 8 && imbY == 18)
{
    BYTE *pBlock;
    DWORD block;
    DWORD YPitch = pWMVDec->m_iWidthPrevY;
    DWORD UVPitch = pWMVDec->m_iWidthPrevUV;

    DWORD i, j;

    for (block = 0; block < 4; block++)
    {
        pBlock = ppxliCodedY + BLOCK_SIZE * (block & 1) + BLOCK_SIZE * YPitch * (block >> 1);

        DbgPrint("----- Y block # %d for Macroblock %d-%d\n\n", block, imbX, imbY);

        for (j = 0; j < BLOCK_SIZE; j++)
        {
            for (i = 0; i < BLOCK_SIZE; i++)
            {
                DbgPrint("%02X ", *(pBlock + i + j * YPitch));
            }

            DbgPrint("\n");
        }

        DbgPrint("\n");
    }

    DbgPrint("----- U block for Macroblock %d-%d\n\n", imbX, imbY);

    for (j = 0; j < BLOCK_SIZE; j++)
    {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            DbgPrint("%02X ", *(ppxliCodedU + i + j * UVPitch));
        }

        DbgPrint("\n");
    }

    DbgPrint("\n");

    DbgPrint("----- V block for Macroblock %d-%d\n\n", imbX, imbY);

    for (j = 0; j < BLOCK_SIZE; j++)
    {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            DbgPrint("%02X ", *(ppxliCodedV + i + j * UVPitch));
        }

        DbgPrint("\n");
    }

    DbgPrint("\n");
}

#endif DBG


            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }
            ppxliCodedY += MB_SIZE;
            ppxliCodedU += BLOCK_SIZE;
            ppxliCodedV += BLOCK_SIZE;
            pmbmd++;
            piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable += 36;
        }

        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
    }
    
    // Loop filter is mandatory
    if (pWMVDec->m_bLoopFilter)
#ifdef _MultiThread_Decode_ 
        DecodeMultiThreads (LOOPFILTER);
#else

#       ifndef COMBINE_LOOPFILTER_RENDERING
        DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQPlusExpY, 
                         		    pWMVDec->m_ppxliCurrQPlusExpU,
                          		    pWMVDec->m_ppxliCurrQPlusExpV, 
                          		    FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#       endif //COMBINE_LOOPFILTER_RENDERING
#endif

    // render
    if (!pWMVDec->m_bLoopFilter)
        memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);

    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}

//***************************************************************************************************
tWMVDecodeStatus WMVideoDecDecodeP (tWMVDecInternalMember *pWMVDec)
{
    FUNCTION_PROFILE(fpDecP[2]);
    tWMVDecodeStatus tWMVStatus;

    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQPlusExpY;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQPlusExpU;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQPlusExpV;

    U8_WMV* ppxliRefY = pWMVDec->m_ppxliRef0YPlusExp; //pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    U8_WMV* rgchSkipPrevFrame = pWMVDec->m_rgchSkipPrevFrame;

    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;

    CVector* pmv = pWMVDec->m_rgmv;

    FUNCTION_PROFILE_START(&fpDecP[1],DECODEP_PROFILE);
    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;
    CoordI x, y; // mvXUV, mvYUV; // mvXUV and mvYUV are zoomed UV MV
    U32_WMV imbx, imbY;
    I16_WMV* piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    I16_WMV** piQuanCoefACPredTable = pWMVDec->m_rgiQuanCoefACPredTable;
    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;

    if (pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACInterTableIndx];

	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACInterTableIndx];
        pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
        pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];


	    pWMVDec->m_pHufMVDec = pWMVDec->m_pHufMVDec_Set[pWMVDec->m_iMVTable];
	    pWMVDec->m_puXMvFromIndex = pWMVDec->m_puMvFromIndex_Set[pWMVDec->m_iMVTable<<1];
	    pWMVDec->m_puYMvFromIndex = pWMVDec->m_puMvFromIndex_Set[(pWMVDec->m_iMVTable<<1) + 1];
        HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeP",5);

    }
    else{
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[2];

	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2];
    }

    // update all stepsize
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);
    pWMVDec->m_i2DoublePlusStepSize = pWMVDec->m_iDoubleStepSize + pWMVDec->m_iStepMinusStepIsEven;
    pWMVDec->m_i2DoublePlusStepSizeNeg = -1 * pWMVDec->m_i2DoublePlusStepSize;


#ifdef _EMB_WMV2_
    InitEMB_DecodePShortcut(pWMVDec);
#endif

#ifdef DYNAMIC_EDGEPAD
    edgePadSetup(pWMVDec, &pWMVDec->m_tEdgePad);
#endif

    for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {
        U8_WMV* ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV* ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV* ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV* ppxliRefYMB = ppxliRefY;
        U8_WMV* ppxliRefUMB = ppxliRefU;
        U8_WMV* ppxliRefVMB = ppxliRefV;
        Bool_WMV bNot1stRowInPict = (imbY != 0);
        Bool_WMV bNot1stRowInSlice = (imbY % pWMVDec->m_uintNumMBYSlice != 0);

        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;
        if (!(imbY & 0x01)){ 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
            pmv = pWMVDec->m_rgmv;
        }

        if (pWMVDec->m_cvCodecVersion == MP4S && imbY) 
            bNot1stRowInSlice = TRUE_WMV;

#ifdef _MPG4_
        pWMVDec->m_rgiDCRef [0] = pWMVDec->m_rgiDCRef [1] = pWMVDec->m_rgiDCRef [2] = 128;
#endif
        for (x = 0, imbx = 0; imbx < pWMVDec->m_uintNumMBX; imbx++, x += MB_SIZE) {
            if (pWMVDec->m_cvCodecVersion == MP4S) {
                if (checkResyncMarker(pWMVDec)) {
                    decodeVideoPacketHeader(pWMVDec, pWMVDec->m_iStepSize);
                    uiNumMBFromSliceBdry = 0;
                }
            }
            uiNumMBFromSliceBdry ++;
//#ifdef _HYBRID_MV_
            pWMVDec->m_bMBHybridMV = decideHybridMVOn (pWMVDec, x, bNot1stRowInSlice, pmv);
//#endif
            tWMVStatus = (*pWMVDec->m_pDecodeMBOverheadOfPVOP) (pWMVDec, pmbmd);
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecP[1]);
                return tWMVStatus;
            }

            if (pmbmd->m_bSkip) {
SKIP_MB:
                pmv->x = pmv->y = 0;

                if (pWMVDec->m_bLoopFilter || *rgchSkipPrevFrame == 0) { // MB in the previous frame is not skipped
                    FUNCTION_PROFILE_START(&fpDecP[0],DECODEMBSKIP_PROFILE);
                    (*pWMVDec->m_pMotionCompZero) (
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                        ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
						pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                }
                *rgchSkipPrevFrame = 1;
                goto NEXT_MB;
            }
            // no skip
            *rgchSkipPrevFrame = 0;
            if (pmbmd->m_dctMd == INTER) {
                FUNCTION_PROFILE_START(&fpDecP[0],DECODEMBMOTION_PROFILE);

                if (pWMVDec->m_cvCodecVersion >= WMV1 || pWMVDec->m_cvCodecVersion == MP43) 
                    //result = decodeMV (pmv, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice);
                    tWMVStatus = decodeMVMSV (pWMVDec, pmv, pmbmd, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice, pWMVDec->m_pHufMVDec, pWMVDec->m_puXMvFromIndex, pWMVDec->m_puYMvFromIndex);
                else if (pWMVDec->m_cvCodecVersion == MP42 || pWMVDec->m_cvCodecVersion == MPG4)
                    tWMVStatus = decodeMV (pWMVDec, pmv, pmbmd, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice);
                else if (pWMVDec->m_cvCodecVersion == MP4S)
                    tWMVStatus = decodeMV (pWMVDec, pmv, pmbmd, imbx == 0 || uiNumMBFromSliceBdry==1 , imbx == pWMVDec->m_uiRightestMB, (!bNot1stRowInSlice) || uiNumMBFromSliceBdry < (pWMVDec->m_uintNumMBX + 1));
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
                pmbmd->m_bZeroMV = (pmv->x == 0 && pmv->y == 0);
                // no COD, so skip is decided by MV and CBP
                if (pmbmd->m_bCBPAllZero && pmbmd->m_bZeroMV) {
                    pmbmd->m_bSkip = TRUE_WMV;
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                    goto SKIP_MB;
                }
                I32_WMV iMV_X = x * 2 + pmv->x;
                I32_WMV iMV_Y = y * 2 + pmv->y;
                I32_WMV iMV_X_UV = x + gmvUVtable4 [pmv->x];
                I32_WMV iMV_Y_UV = y + gmvUVtable4 [pmv->y];


                tWMVStatus = (*pWMVDec->m_pDecodePMB) (
                    pWMVDec, 
                    pmbmd,
                    ppxliCurrQYMB, 
                    ppxliCurrQUMB,
                    ppxliCurrQVMB, 
                    iMV_X, // Y MV 
                    iMV_Y,
                    iMV_X_UV, 
                    iMV_Y_UV);
                FUNCTION_PROFILE_STOP(&fpDecP[0]);
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
            }
            else {
                Bool_WMV bLeft, bTop, bLeftTop; 
                FUNCTION_PROFILE_START(&fpDecP[0],DECODEIMBINPFRAME_PROFILE);
                pmv->x = pmv->y = 0;                
                if (pWMVDec->m_cvCodecVersion >= WMV1){ 
                    bLeft = (imbx != 0);
                    bTop = bNot1stRowInPict;
                    bLeftTop = (imbx !=0 ) && bNot1stRowInPict;
                    if (pWMVDec->m_bDCPred_IMBInPFrame)
                        tWMVStatus = DecodeIMBAcPred_PFrame (
                            pWMVDec, 
                            pmbmd,
                            ppxliCurrQYMB,
                            ppxliCurrQUMB,
                            ppxliCurrQVMB,
                            piQuanCoefACPred, 
                            piQuanCoefACPredTable, 
                            bLeft,
                            bTop,
                            bLeftTop);
                    else
                        tWMVStatus = DecodeIMBAcPred (
                            pWMVDec, 
                            pmbmd,
                            ppxliCurrQYMB,
                            ppxliCurrQUMB,
                            ppxliCurrQVMB,
                            piQuanCoefACPred, 
                            piQuanCoefACPredTable, 
                            bLeft && ((pmbmd - 1)->m_dctMd == INTRA),
                            bTop && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA),
                            bLeftTop && ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA)
                            );
                }
                else{
                    bLeft = (imbx != 0) && ((pmbmd - 1)->m_dctMd == INTRA) &&
                        (uiNumMBFromSliceBdry > 1);
                    bTop = bNot1stRowInSlice && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA)  &&
                        (uiNumMBFromSliceBdry > pWMVDec->m_uintNumMBX);
                    bLeftTop = (imbx!=0 ) && bNot1stRowInSlice && ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA)  &&
                        (uiNumMBFromSliceBdry > (pWMVDec->m_uintNumMBX+1));
                    tWMVStatus = DecodeIMBAcPred_MP4X (
                        pWMVDec, 
                        pmbmd,
                        ppxliCurrQYMB,
                        ppxliCurrQUMB,
                        ppxliCurrQVMB,
                        piQuanCoefACPred, 
                        piQuanCoefACPredTable, 
                        bLeft,
                        bTop,
                        bLeftTop);
                }

                FUNCTION_PROFILE_STOP(&fpDecP[0]);
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
            }
NEXT_MB:
            if (pmbmd->m_dctMd == INTER) {
                piQuanCoefACPred[0] = piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES4] =piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES5] = 0;
            }
            rgchSkipPrevFrame++;
            pmbmd++;
            pmv++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            ppxliRefYMB += MB_SIZE;
            ppxliRefUMB += BLOCK_SIZE;
            ppxliRefVMB += BLOCK_SIZE;
            piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable += 36;
        }



        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliRefU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRefV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
    }

#ifndef COMBINE_LOOPFILTER_RENDERING
    if (pWMVDec->m_bLoopFilter)
#ifdef _MultiThread_Decode_ 
        DecodeMultiThreads (LOOPFILTER);
#else

        DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQPlusExpY, 
                     		        pWMVDec->m_ppxliCurrQPlusExpU,
                      		        pWMVDec->m_ppxliCurrQPlusExpV, 
                      		        FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#endif
#endif
    FUNCTION_PROFILE_STOP(&fpDecP[1]);
    return WMV_Succeeded;
}

#ifdef _SUPPORT_POST_FILTERS_
tWMVDecodeStatus WMVideoDecDecodeIDeblock (tWMVDecInternalMember *pWMVDec)
{
    FUNCTION_PROFILE_DECL_START(fp,DECODEI_PROFILE);
    tWMVDecodeStatus tWMVStatus;

    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQPlusExpY;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQPlusExpU;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQPlusExpV;
    U8_WMV* ppxliPostQY = pWMVDec->m_ppxliPostQPlusExpY;
    U8_WMV* ppxliPostQU = pWMVDec->m_ppxliPostQPlusExpU;
    U8_WMV* ppxliPostQV = pWMVDec->m_ppxliPostQPlusExpV;

    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    I16_WMV* piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    I16_WMV** piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;
    U32_WMV imbY;
    
    if (pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACInterTableIndx];
	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACIntraTableIndx];
        pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
        pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
    }
    else{
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[2];
	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2];
    }

    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);

    HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeI",1);

    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
        U8_WMV* ppxliCodedY = ppxliCurrQY;
        U8_WMV* ppxliCodedU = ppxliCurrQU;
        U8_WMV* ppxliCodedV = ppxliCurrQV;
        Bool_WMV bNot1stRowInSlice;
        U32_WMV imbX;

        if (!(imbY & 0x01)){ 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
        }
        if (pWMVDec->m_cvCodecVersion >= WMV1 || pWMVDec->m_cvCodecVersion == MP4S) // THREE or STANDARD
            bNot1stRowInSlice = (imbY != 0);
        else 
            bNot1stRowInSlice = (imbY % pWMVDec->m_uintNumMBYSlice != 0);

#ifdef _MPG4_
        pWMVDec->m_rgiDCRef [0] = pWMVDec->m_rgiDCRef [1] = pWMVDec->m_rgiDCRef [2] = 128;
#endif
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            if(pWMVDec->m_cvCodecVersion == MP4S) {
                if(checkResyncMarker(pWMVDec)) {
                    decodeVideoPacketHeader(pWMVDec, pWMVDec->m_iStepSize);
                    uiNumMBFromSliceBdry = 0;
                }
            }

            uiNumMBFromSliceBdry++;
            tWMVStatus = (*pWMVDec->m_pDecodeMBOverheadOfIVOP) (pWMVDec, pmbmd, imbX, imbY);
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }

            {
                Bool_WMV bLeft = (imbX != 0) && (uiNumMBFromSliceBdry > 1);
                Bool_WMV bTop = bNot1stRowInSlice && (uiNumMBFromSliceBdry > pWMVDec->m_uintNumMBX);
                Bfool_WMV bLeftTop = (imbX != 0) && bNot1stRowInSlice && (uiNumMBFromSliceBdry > (pWMVDec->m_uintNumMBX+1));
                tWMVStatus = (*pWMVDec->m_pDecodeIMBAcPred) (
                    pWMVDec,
                    pmbmd,
                    ppxliCodedY,
                    ppxliCodedU,
                    ppxliCodedV, 
                    piQuanCoefACPred, 
                    piQuanCoefACPredTable, 
                    bLeft,
                    bTop,
                    bLeftTop);
            }
//              (imbX != 0), 
//              bNot1stRowInSlice,
//              (imbX != 0 && bNot1stRowInSlice));

            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }
            ppxliCodedY += MB_SIZE;
            ppxliCodedU += BLOCK_SIZE;
            ppxliCodedV += BLOCK_SIZE;
            pmbmd++;
            piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable += 36;
        }

        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
    }
    
    // Loop filter is mandatory
    if (pWMVDec->m_bLoopFilter)
#ifdef _MultiThread_Decode_ 
        DecodeMultiThreads (LOOPFILTER);
#else
        DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQPlusExpY, 
                         		    pWMVDec->m_ppxliCurrQPlusExpU,
                          		    pWMVDec->m_ppxliCurrQPlusExpV, 
                          		    FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#endif

    // Post-processing
    ppxliCurrQY = pWMVDec->m_ppxliCurrQPlusExpY;
    ppxliCurrQU = pWMVDec->m_ppxliCurrQPlusExpU;
    ppxliCurrQV = pWMVDec->m_ppxliCurrQPlusExpV; 
    ppxliPostQY = pWMVDec->m_ppxliPostQPlusExpY;
    ppxliPostQU = pWMVDec->m_ppxliPostQPlusExpU;
    ppxliPostQV = pWMVDec->m_ppxliPostQPlusExpV;
	for (I32_WMV i = 0; i < pWMVDec->m_iHeightUV; i++) {
        memcpy(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(PixelC));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        memcpy(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(PixelC));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        memcpy(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(PixelC));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        memcpy(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(PixelC));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }
    DeblockIFrame (pWMVDec, 
                    pWMVDec->m_ppxliPostQPlusExpY,
                    pWMVDec->m_ppxliPostQPlusExpU,
                    pWMVDec->m_ppxliPostQPlusExpV,
                    0, pWMVDec->m_uintNumMBY);

    // render
    if (!pWMVDec->m_bLoopFilter)
        memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);

    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}

tWMVDecodeStatus WMVideoDecDecodePDeblock (tWMVDecInternalMember *pWMVDec)
{
    FUNCTION_PROFILE(fpDecP[2]);
    tWMVDecodeStatus tWMVStatus;

    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQPlusExpY;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQPlusExpU;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQPlusExpV;

    U8_WMV* ppxliRefY = pWMVDec->m_ppxliRef0YPlusExp; //pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    
    U8_WMV* ppxliPostQY = pWMVDec->m_ppxliPostQPlusExpY;
    U8_WMV* ppxliPostQU = pWMVDec->m_ppxliPostQPlusExpU;
    U8_WMV* ppxliPostQV = pWMVDec->m_ppxliPostQPlusExpV;

    U8_WMV* rgchSkipPrevFrame = pWMVDec->m_rgchSkipPrevFrame;

    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;

    CVector* pmv = pWMVDec->m_rgmv;

    FUNCTION_PROFILE_START(&fpDecP[1],DECODEP_PROFILE);
    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;
    CoordI x, y; // mvXUV, mvYUV; // mvXUV and mvYUV are zoomed UV MV
    U32_WMV imbx, imbY;
    I16_WMV* piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    I16_WMV** piQuanCoefACPredTable = pWMVDec->m_rgiQuanCoefACPredTable;
    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;

    if (pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACInterTableIndx];

	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_iDCTACInterTableIndx];
        pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
        pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];


	    pWMVDec->m_pHufMVDec = pWMVDec->m_pHufMVDec_Set[pWMVDec->m_iMVTable];
	    pWMVDec->m_puXMvFromIndex = pWMVDec->m_puMvFromIndex_Set[pWMVDec->m_iMVTable<<1];
	    pWMVDec->m_puYMvFromIndex = pWMVDec->m_puMvFromIndex_Set[(pWMVDec->m_iMVTable<<1) + 1];
        HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeP",5);

    }
    else{
        pWMVDec->m_pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[2];

	    pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2];
    }

    // update all stepsize
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);
    pWMVDec->m_i2DoublePlusStepSize = pWMVDec->m_iDoubleStepSize + pWMVDec->m_iStepMinusStepIsEven;
    pWMVDec->m_i2DoublePlusStepSizeNeg = -1 * pWMVDec->m_i2DoublePlusStepSize;


#ifdef _EMB_WMV2_
    InitEMB_DecodePShortcut(pWMVDec);
#endif

#ifdef DYNAMIC_EDGEPAD
    edgePadSetup(pWMVDec, &pWMVDec->m_tEdgePad);
#endif

    for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {
        U8_WMV* ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV* ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV* ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV* ppxliRefYMB = ppxliRefY;
        U8_WMV* ppxliRefUMB = ppxliRefU;
        U8_WMV* ppxliRefVMB = ppxliRefV;
        Bool_WMV bNot1stRowInPict = (imbY != 0);
        Bool_WMV bNot1stRowInSlice = (imbY % pWMVDec->m_uintNumMBYSlice != 0);

        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;
        if (!(imbY & 0x01)){ 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
            pmv = pWMVDec->m_rgmv;
        }

        if (pWMVDec->m_cvCodecVersion == MP4S && imbY) 
            bNot1stRowInSlice = TRUE_WMV;

#ifdef _MPG4_
        pWMVDec->m_rgiDCRef [0] = pWMVDec->m_rgiDCRef [1] = pWMVDec->m_rgiDCRef [2] = 128;
#endif
        for (x = 0, imbx = 0; imbx < pWMVDec->m_uintNumMBX; imbx++, x += MB_SIZE) {
            if (pWMVDec->m_cvCodecVersion == MP4S) {
                if (checkResyncMarker(pWMVDec)) {
                    decodeVideoPacketHeader(pWMVDec, pWMVDec->m_iStepSize);
                    uiNumMBFromSliceBdry = 0;
                }
            }
            uiNumMBFromSliceBdry ++;
//#ifdef _HYBRID_MV_
            pWMVDec->m_bMBHybridMV = decideHybridMVOn (pWMVDec, x, bNot1stRowInSlice, pmv);
//#endif
            tWMVStatus = (*pWMVDec->m_pDecodeMBOverheadOfPVOP) (pWMVDec, pmbmd);
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecP[1]);
                return tWMVStatus;
            }

            if (pmbmd->m_bSkip) {
SKIP_MB:
                pmv->x = pmv->y = 0;

                if (pWMVDec->m_bLoopFilter || *rgchSkipPrevFrame == 0) { // MB in the previous frame is not skipped
                    FUNCTION_PROFILE_START(&fpDecP[0],DECODEMBSKIP_PROFILE);
                    (*pWMVDec->m_pMotionCompZero) (
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                        ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
						pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                }
                *rgchSkipPrevFrame = 1;
                goto NEXT_MB;
            }
            // no skip
            *rgchSkipPrevFrame = 0;
            if (pmbmd->m_dctMd == INTER) {
                FUNCTION_PROFILE_START(&fpDecP[0],DECODEMBMOTION_PROFILE);

                if (pWMVDec->m_cvCodecVersion >= WMV1 || pWMVDec->m_cvCodecVersion == MP43) 
                    //result = decodeMV (pmv, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice);
                    tWMVStatus = decodeMVMSV (pWMVDec, pmv, pmbmd, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice, pWMVDec->m_pHufMVDec, pWMVDec->m_puXMvFromIndex, pWMVDec->m_puYMvFromIndex);
                else if (pWMVDec->m_cvCodecVersion == MP42 || pWMVDec->m_cvCodecVersion == MPG4)
                    tWMVStatus = decodeMV (pWMVDec, pmv, pmbmd, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice);
                else if (pWMVDec->m_cvCodecVersion == MP4S)
                    tWMVStatus = decodeMV (pWMVDec, pmv, pmbmd, imbx == 0 || uiNumMBFromSliceBdry==1 , imbx == pWMVDec->m_uiRightestMB, (!bNot1stRowInSlice) || uiNumMBFromSliceBdry < (pWMVDec->m_uintNumMBX + 1));
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
                pmbmd->m_bZeroMV = (pmv->x == 0 && pmv->y == 0);
                // no COD, so skip is decided by MV and CBP
                if (pmbmd->m_bCBPAllZero && pmbmd->m_bZeroMV) {
                    pmbmd->m_bSkip = TRUE_WMV;
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                    goto SKIP_MB;
                }
                I32_WMV iMV_X = x * 2 + pmv->x;
                I32_WMV iMV_Y = y * 2 + pmv->y;
                I32_WMV iMV_X_UV = x + gmvUVtable4 [pmv->x];
                I32_WMV iMV_Y_UV = y + gmvUVtable4 [pmv->y];


                tWMVStatus = (*pWMVDec->m_pDecodePMB) (
                    pWMVDec, 
                    pmbmd,
                    ppxliCurrQYMB, 
                    ppxliCurrQUMB,
                    ppxliCurrQVMB, 
                    iMV_X, // Y MV 
                    iMV_Y,
                    iMV_X_UV, 
                    iMV_Y_UV);
                FUNCTION_PROFILE_STOP(&fpDecP[0]);
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
            }
            else {
                Bool_WMV bLeft, bTop, bLeftTop; 
                FUNCTION_PROFILE_START(&fpDecP[0],DECODEIMBINPFRAME_PROFILE);
                pmv->x = pmv->y = 0;                
                if (pWMVDec->m_cvCodecVersion >= WMV1){ 
                    bLeft = (imbx != 0);
                    bTop = bNot1stRowInPict;
                    bLeftTop = (imbx !=0 ) && bNot1stRowInPict;
                    if (pWMVDec->m_bDCPred_IMBInPFrame)
                        tWMVStatus = DecodeIMBAcPred_PFrame (
                            pWMVDec, 
                            pmbmd,
                            ppxliCurrQYMB,
                            ppxliCurrQUMB,
                            ppxliCurrQVMB,
                            piQuanCoefACPred, 
                            piQuanCoefACPredTable, 
                            bLeft,
                            bTop,
                            bLeftTop);
                    else
                        tWMVStatus = DecodeIMBAcPred (
                            pWMVDec, 
                            pmbmd,
                            ppxliCurrQYMB,
                            ppxliCurrQUMB,
                            ppxliCurrQVMB,
                            piQuanCoefACPred, 
                            piQuanCoefACPredTable, 
                            bLeft && ((pmbmd - 1)->m_dctMd == INTRA),
                            bTop && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA),
                            bLeftTop && ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA)
                            );
                }
                else{
                    bLeft = (imbx != 0) && ((pmbmd - 1)->m_dctMd == INTRA) &&
                        (uiNumMBFromSliceBdry > 1);
                    bTop = bNot1stRowInSlice && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA)  &&
                        (uiNumMBFromSliceBdry > pWMVDec->m_uintNumMBX);
                    bLeftTop = (imbx!=0 ) && bNot1stRowInSlice && ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA)  &&
                        (uiNumMBFromSliceBdry > (pWMVDec->m_uintNumMBX+1));
                    tWMVStatus = DecodeIMBAcPred_MP4X (
                        pWMVDec, 
                        pmbmd,
                        ppxliCurrQYMB,
                        ppxliCurrQUMB,
                        ppxliCurrQVMB,
                        piQuanCoefACPred, 
                        piQuanCoefACPredTable, 
                        bLeft,
                        bTop,
                        bLeftTop);
                }

                FUNCTION_PROFILE_STOP(&fpDecP[0]);
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
            }
NEXT_MB:
            if (pmbmd->m_dctMd == INTER) {
                piQuanCoefACPred[0] = piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES4] =piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES5] = 0;
            }
            rgchSkipPrevFrame++;
            pmbmd++;
            pmv++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            ppxliRefYMB += MB_SIZE;
            ppxliRefUMB += BLOCK_SIZE;
            ppxliRefVMB += BLOCK_SIZE;
            piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable += 36;
        }
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliRefU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRefV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
    }

#ifndef COMBINE_LOOPFILTER_RENDERING
    if (pWMVDec->m_bLoopFilter)
#ifdef _MultiThread_Decode_ 
        DecodeMultiThreads (LOOPFILTER);
#else

        DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQPlusExpY, 
                     		        pWMVDec->m_ppxliCurrQPlusExpU,
                      		        pWMVDec->m_ppxliCurrQPlusExpV, 
                      		        FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#endif
#endif

    //Post-procesing
    ppxliCurrQY = pWMVDec->m_ppxliCurrQPlusExpY;
    ppxliCurrQU = pWMVDec->m_ppxliCurrQPlusExpU;
    ppxliCurrQV = pWMVDec->m_ppxliCurrQPlusExpV; 
    ppxliPostQY = pWMVDec->m_ppxliPostQPlusExpY;
    ppxliPostQU = pWMVDec->m_ppxliPostQPlusExpU;
    ppxliPostQV = pWMVDec->m_ppxliPostQPlusExpV;
	for (I32_WMV i = 0; i < pWMVDec->m_iHeightUV; i++) {
        memcpy(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(PixelC));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        memcpy(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(PixelC));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        memcpy(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(PixelC));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        memcpy(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(PixelC));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }
    DeblockPFrame (pWMVDec, 
                pWMVDec->m_ppxliPostQPlusExpY,
                pWMVDec->m_ppxliPostQPlusExpU,
                pWMVDec->m_ppxliPostQPlusExpV,
                0, pWMVDec->m_uintNumMBY);

    FUNCTION_PROFILE_STOP(&fpDecP[1]);
    return WMV_Succeeded;
}
#endif //_SUPPORT_POST_FILTERS_

// 
Void_WMV UpdateDCStepSize(tWMVDecInternalMember *pWMVDec, I32_WMV iStepSize) 
{
    pWMVDec->m_iStepSize = iStepSize;
    pWMVDec->m_bStepSizeIsEven = ((pWMVDec->m_iStepSize % 2) == 0);
    pWMVDec->m_iStepMinusStepIsEven = pWMVDec->m_iStepSize - pWMVDec->m_bStepSizeIsEven;
    pWMVDec->m_iDoubleStepSize = pWMVDec->m_iStepSize << 1;
    pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = 8;

    if (pWMVDec->m_cvCodecVersion >= MP43 || pWMVDec->m_cvCodecVersion == MP4S) {
        if (pWMVDec->m_iStepSize <= 4)   {
            pWMVDec->m_iDCStepSize = 8;
            pWMVDec->m_iDCStepSizeC = 8;
        }
        else if (pWMVDec->m_cvCodecVersion >= WMV1){
            pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = pWMVDec->m_iStepSize / 2 + 6;
        }
        else if (pWMVDec->m_iStepSize <= 8)  {
            pWMVDec->m_iDCStepSize = 2 * pWMVDec->m_iStepSize;
            pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize + 13) / 2;
        }
        else if (pWMVDec->m_iStepSize <= 24) {
            pWMVDec->m_iDCStepSize = pWMVDec->m_iStepSize + 8;
            pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize + 13) / 2;
        }
        else {
            pWMVDec->m_iDCStepSize = 2 * pWMVDec->m_iStepSize - 16;
            pWMVDec->m_iDCStepSizeC = pWMVDec->m_iStepSize - 6;
        }
        if (pWMVDec->m_cvCodecVersion >= MP43) {
            pWMVDec->m_pAvgQuanDctCoefDec [0] = pWMVDec->m_pAvgQuanDctCoefDec [8] = (1024 + (pWMVDec->m_iDCStepSize >> 1)) / pWMVDec->m_iDCStepSize;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[8] = (1024 + (pWMVDec->m_iDCStepSizeC >> 1)) / pWMVDec->m_iDCStepSizeC;
        }
    }
}


tWMVDecodeStatus WMVideoDecDecodeClipInfo (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_cvCodecVersion == WMV2) {
        if (pWMVDec->m_bXintra8Switch)
            pWMVDec->m_bXintra8 = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    } else { // WMV1
        I32_WMV iFrameRate = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            pWMVDec->m_iFrameRate = 30;
            pWMVDec->m_iBitRate = 500; // to be conservative for the deblocking/deringing choice
            pWMVDec->m_bRndCtrlOn = FALSE;
            return WMV_Succeeded;
        }
        if (pWMVDec->m_iFrameRate == 0) // if the info is available from system (app), use it.
            pWMVDec->m_iFrameRate = iFrameRate;
        pWMVDec->m_iBitRate = BS_getBits (pWMVDec->m_pbitstrmIn, 11);
g_iBitRate = pWMVDec->m_iBitRate;
g_iFrameRate = pWMVDec->m_iFrameRate;
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            pWMVDec->m_iFrameRate = 30;
            pWMVDec->m_iBitRate = 500; // to be conservative for the deblocking/deringing choice
            pWMVDec->m_bRndCtrlOn = FALSE;
            return WMV_Succeeded;
        }
    	if (pWMVDec->m_cvCodecVersion != MP42) 
            pWMVDec->m_bRndCtrlOn = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    }
    return WMV_Succeeded;
}

tWMVDecodeStatus setSliceCode (tWMVDecInternalMember *pWMVDec, I32_WMV iSliceCode)
{
    // setSliceCode 
    if (!pWMVDec->m_fPrepared) {
        if (pWMVDec->m_cvCodecVersion == WMV2) {
			setRefreshPeriod(pWMVDec);
            pWMVDec->m_uintNumMBYSlice = pWMVDec->m_uintNumMBY / iSliceCode;
        }
        else if (pWMVDec->m_cvCodecVersion != MP4S) {
			if (pWMVDec->m_cvCodecVersion == WMV1) 
                setRefreshPeriod(pWMVDec);
            if (iSliceCode <= 22)
                return WMV_Failed;
            else {
                pWMVDec->m_uintNumMBYSlice = pWMVDec->m_uintNumMBY / (iSliceCode - 22);
            }
        }
        else // MP4S  Needs to be fixed
            pWMVDec->m_uintNumMBYSlice = pWMVDec->m_uintNumMBY;

        pWMVDec->m_bMainProfileOn = (!pWMVDec->m_bDeblockOn && pWMVDec->m_bMBAligned);
        pWMVDec->m_fPrepared = TRUE_WMV;
        return WMV_Succeeded;
    }
    else
        return WMV_Succeeded;
}

tWMVDecodeStatus decodeVOLHead (tWMVDecInternalMember *pWMVDec)
{
    assert (pWMVDec->m_cvCodecVersion == WMV2);
    pWMVDec->m_iFrameRate = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);
g_iFrameRate = pWMVDec->m_iFrameRate;
    pWMVDec->m_iBitRate = BS_getBits  ( pWMVDec->m_pbitstrmIn, 11);
g_iBitRate = pWMVDec->m_iBitRate;
    pWMVDec->m_bRndCtrlOn = TRUE_WMV;
    pWMVDec->m_bMixedPel = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bLoopFilter = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bXformSwitch = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bXintra8Switch = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);

//#ifdef _HYBRID_MV_
    pWMVDec->m_bFrmHybridMVOn = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
//#else
//    pWMVDec->m_bFrmHybridMVOn = FALSE;
//#endif
    // DCTTABLE S/W at MB level for WMV2.
    pWMVDec->m_bDCTTable_MB_ENABLED = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_iSliceCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE_WMV2);

    //pWMVDec->m_pbitstrmIn->flushMPEG4 ();
    return WMV_Succeeded;
}

Void_WMV DecodeSkipBit(tWMVDecInternalMember *pWMVDec)
{
	CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
	U32_WMV imbY, imbX;

    switch(pWMVDec->m_SkipBitCodingMode) {
	case Normal:
		for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
			for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
				pmbmd [imbX].m_bSkip = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
				//pmbmd++;
			}
            pmbmd += pWMVDec->m_uintNumMBX;
		}
		break;
	case RowPredict:
		for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
			if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { 
				// skip row
				for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
					pmbmd->m_bSkip = 1;
					pmbmd++;
				}
			} else { 
				for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
					pmbmd->m_bSkip = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
					pmbmd++;
				}
			}
		}
		break;
	case ColPredict:
        {
		    CWMVMBMode* pmbmdcol = pWMVDec->m_rgmbmd;
		    for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
			    pmbmd = pmbmdcol;
			    if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { // skip column
				    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
					    pmbmd->m_bSkip = 1;
					    pmbmd += pWMVDec->m_uintNumMBX;
				    }
			    } else {
				    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
					    pmbmd->m_bSkip = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
					    pmbmd += pWMVDec->m_uintNumMBX;
				    }
			    }
			    pmbmdcol++;
		    }
        }
		break;
	}
}

tWMVDecodeStatus WMVideoDecDecodeFrameHead (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_cvCodecVersion != MP4S) {
        I32_WMV iNumBitsFrameType;
#ifdef _MPG4_
        if (pWMVDec->m_cvCodecVersion == MPG4) {
            U32_WMV uiVopStartCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_START_CODE_PREFIX+NUMBITS_VOP_START_CODE);
            uiVopStartCode = uiVopStartCode << NUMBITS_START_CODE_PREFIX;
            U32_WMV start = START_CODE_PREFIX << NUMBITS_START_CODE_PREFIX;
            BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_TIME);
        }
#endif
        iNumBitsFrameType = (pWMVDec->m_cvCodecVersion == WMV2) ? NUMBITS_VOP_PRED_TYPE_WMV2 :  NUMBITS_VOP_PRED_TYPE;
        pWMVDec->m_tFrmType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, iNumBitsFrameType);
        if (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != PVOP)
            return WMV_CorruptedBits;

        if (pWMVDec->m_cvCodecVersion == WMV2 && pWMVDec->m_tFrmType == IVOP) {
            I32_WMV iBufferFullPercent = BS_getBits  ( pWMVDec->m_pbitstrmIn, 7);
            if (BS_invalid ( pWMVDec->m_pbitstrmIn))
	            return WMV_CorruptedBits;
        }

        pWMVDec->m_iStepSize = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_QUANTIZER);
        //pWMVDec->m_iStepSize = pWMVDec->m_iStepSize = stepDecoded;
        if (BS_invalid ( pWMVDec->m_pbitstrmIn) || pWMVDec->m_iStepSize <= 0 || pWMVDec->m_iStepSize > 31) {
	        return WMV_CorruptedBits;
        }


        if (pWMVDec->m_tFrmType == IVOP) {
            tWMVDecodeStatus tWMVStatus;
            if (pWMVDec->m_cvCodecVersion != WMV2)        
                pWMVDec->m_iSliceCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE);
            tWMVStatus = setSliceCode (pWMVDec, pWMVDec->m_iSliceCode);
            //I32_WMV iSliceCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE);
            if (tWMVStatus != WMV_Succeeded) {
			    return tWMVStatus;
		    }
            if (pWMVDec->m_cvCodecVersion >= WMV1){
                tWMVStatus = WMVideoDecDecodeClipInfo (pWMVDec);
                if(BS_invalid ( pWMVDec->m_pbitstrmIn) || tWMVStatus != WMV_Succeeded) {
                    return tWMVStatus;
                }
                if (pWMVDec->m_cvCodecVersion == WMV2) {
                    pWMVDec->m_bDCPred_IMBInPFrame = FALSE;
                } else {    // WMV1
                    pWMVDec->m_bDCTTable_MB_ENABLED = (pWMVDec->m_iBitRate > MIN_BITRATE_MB_TABLE);
                    pWMVDec->m_bDCPred_IMBInPFrame = (pWMVDec->m_iBitRate <= MAX_BITRATE_DCPred_IMBInPFrame && (pWMVDec->m_iFrmWidthSrc * pWMVDec->m_iFrmHeightSrc < 320 * 240));
                }
            }
            if (!pWMVDec->m_bXintra8 && pWMVDec->m_cvCodecVersion >= MP43) {
                //pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion == WMV1(2)
                // If pWMVDec->m_bDCTTable_MB_ENABLED is on
                if (pWMVDec->m_bDCTTable_MB_ENABLED){
                    pWMVDec->m_bDCTTable_MB = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                }
                if (!pWMVDec->m_bDCTTable_MB){
		            // DCT Table swtiching, I and P index are coded separately.
		            // Can be jointly coded using the following table. 
		            // IP Index : Code
		            // 00		: 00, 
		            // 11		: 01, 
		            // 01		: 100,
		            // 10		: 101,
		            // 02		: 1100,
		            // 12		: 1101,
		            // 20		: 1110, 
		            // 21		: 11110
		            // 22		: 11111
                    pWMVDec->m_iDCTACInterTableIndx = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		            if (pWMVDec->m_iDCTACInterTableIndx){
			            pWMVDec->m_iDCTACInterTableIndx += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		            }
                    pWMVDec->m_iDCTACIntraTableIndx = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		            if (pWMVDec->m_iDCTACIntraTableIndx){
			            pWMVDec->m_iDCTACIntraTableIndx += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		            }
                }
		        pWMVDec->m_iIntraDCTDCTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
            }
        
            pWMVDec->m_iRndCtrl = 1;
        }
        else {
            //if (pWMVDec->m_cvCodecVersion == WMV2)
            decodeVOPHead_WMV2(pWMVDec);
            //else
            //    pWMVDec->m_bCODFlagOn = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);

            if (pWMVDec->m_cvCodecVersion >= MP43) {
		        // MP43 || WMV1 (2) 
                if (pWMVDec->m_bDCTTable_MB_ENABLED){
                    pWMVDec->m_bDCTTable_MB = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                }
                if (!pWMVDec->m_bDCTTable_MB){
                    pWMVDec->m_iDCTACInterTableIndx = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		            if (pWMVDec->m_iDCTACInterTableIndx){
			            pWMVDec->m_iDCTACInterTableIndx += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		            }
                }
		        pWMVDec->m_iIntraDCTDCTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
		        pWMVDec->m_iMVTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
            }

            pWMVDec->m_iRndCtrl ^= 0x01;
        }
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            return WMV_CorruptedBits;
        }
        return WMV_Succeeded;
    }// MP4S
    else{
        U32_WMV start, uiVopStartCode, iModuloInc, uiMarker;
        Time tCurrSec, tVopIncr;
        I32_WMV	iIntraDcSwitchThr;

        start = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_START_CODE_PREFIX);
        if (start != START_CODE_PREFIX) {
            return WMV_CorruptedBits;
        }

        uiVopStartCode = BS_getBits  (pWMVDec->m_pbitstrmIn , NUMBITS_VOP_START_CODE);
        if (uiVopStartCode != VOP_START_CODE) {
            return WMV_CorruptedBits;
        }
        pWMVDec->m_tFrmType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_PRED_TYPE);
        // Time reference and VOP_pred_type
        iModuloInc = 0;
        while (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1) != 0)
            iModuloInc++;
        tCurrSec = iModuloInc + pWMVDec->m_tModuloBaseDecd;
        uiMarker = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
//        assert(uiMarker == 1);

        tVopIncr = BS_getBits  ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNumBitsTimeIncr);
        uiMarker = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // marker bit
//        assert(uiMarker == 1);

        pWMVDec->m_tOldModuloBaseDecd = pWMVDec->m_tModuloBaseDecd;
        pWMVDec->m_tOldModuloBaseDisp = pWMVDec->m_tModuloBaseDisp;
        //(pWMVDec->m_tFrmType != BVOP)
        pWMVDec->m_tModuloBaseDisp = pWMVDec->m_tModuloBaseDecd;      //update most recently displayed time base
        pWMVDec->m_tModuloBaseDecd = tCurrSec;

        pWMVDec->m_t = tCurrSec * pWMVDec->m_iClockRate + tVopIncr;

//  if ((pWMVDec->m_coded = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) == 0)         //vop_coded == false
        if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1) == 0)     {       //vop_coded == false
            Bool_WMV bInterlace = FALSE; //wchen: temporary solution
            return WMV_CorruptedBits;
        }


        if (pWMVDec->m_tFrmType == PVOP)
            pWMVDec->m_iRndCtrl = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); //"VOP_Rounding_Type"
        else
            pWMVDec->m_iRndCtrl = 0;

        iIntraDcSwitchThr = BS_getBits  ( pWMVDec->m_pbitstrmIn, 3);

// INTERLACE
//        bInterlace = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
//        I32_WMV UIQUANTPRECISION = 5;
        if (pWMVDec->m_tFrmType == IVOP)    {
            pWMVDec->m_iStepSize = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);    //also assign intStep to be safe
            pWMVDec->uiFCode = 1;
        }
        else if (pWMVDec->m_tFrmType == PVOP) {
            pWMVDec->m_iStepSize = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);
            pWMVDec->uiFCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_FCODE);
            pWMVDec->iScaleFactor = 1 << (pWMVDec->uiFCode - 1);
            pWMVDec->iRange = 16 << pWMVDec->uiFCode;
        }
        return WMV_Succeeded;
    }
}
Void_WMV decodeVOPHead_WMV2 (tWMVDecInternalMember *pWMVDec)
{
    if (!pWMVDec->m_bSKIPBIT_CODING_){
#ifdef _MPG4_
        if (pWMVDec->m_cvCodecVersion != MPG4)
#endif
        pWMVDec->m_bCODFlagOn = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    }
    else{
		I32_WMV iSkipBitCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, 2);
		pWMVDec->m_bCODFlagOn = 1;
		if (iSkipBitCode == 0) {
			pWMVDec->m_bCODFlagOn = 0;
		} else {
			if (iSkipBitCode == 1) {
				pWMVDec->m_SkipBitCodingMode = Normal;
			} else if (iSkipBitCode == 2) {
				pWMVDec->m_SkipBitCodingMode = RowPredict;
			} else {
				pWMVDec->m_SkipBitCodingMode = ColPredict;
			}
			DecodeSkipBit(pWMVDec);
		}
    }

    // NEW_PCBPCY_TABLE
    if (pWMVDec->m_bNEW_PCBPCY_TABLE){
		if (pWMVDec->m_iStepSize <= 10) {
			if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //0 High
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_HighRate;
			} else if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //10 Low
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_LowRate;
			} else { //11 Mid
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_MidRate;
			}
		} else if (pWMVDec->m_iStepSize <= 20) {
			if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //0 Mid
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_MidRate;
			} else if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //10 High
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_HighRate;
			} else { //11 Low
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_LowRate;
			}
		} else {
			if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //0 Low
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_LowRate;
			} else if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //10 Mid
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_MidRate;
			} else { //11 High
				pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_HighRate;
			}
		}
    }

    //_MIXEDPEL_
    if (pWMVDec->m_bMixedPel)
        pWMVDec->m_iMvResolution = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    
    
    if (pWMVDec->m_bXformSwitch) {
        if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
        {
            pWMVDec->m_bMBXformSwitching = FALSE;
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                pWMVDec->m_iFrameXformMode = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                pWMVDec->m_iFrameXformMode = XFORMMODE_8x4;
            else
                pWMVDec->m_iFrameXformMode = XFORMMODE_4x8;
        }
        else
            pWMVDec->m_bMBXformSwitching = TRUE_WMV;
    }

}

Void_WMV setRefreshPeriod(tWMVDecInternalMember *pWMVDec) 
{
#ifdef _SUPPORT_POST_FILTERS_
	I32_WMV iWidthTimesHeight  = pWMVDec->m_iWidthY * pWMVDec->m_iHeightY;
    if (pWMVDec->m_iBitRate > 300 || iWidthTimesHeight > 320 * 240)
        pWMVDec->m_bRefreshDisplay_AllMB_Enable = FALSE;
    else if (iWidthTimesHeight > 240 * 176){
#ifdef  _WMV_TARGET_X86_
        if (g_bSupportMMX_WMV)
            //pWMVDec->m_bRefreshDisplay_AllMB_Enable = FALSE;
            pWMVDec->m_iRefreshDisplay_AllMB_Period = 2;
        else
#endif //  _WMV_TARGET_X86_
            pWMVDec->m_iRefreshDisplay_AllMB_Period = 4;
            //pWMVDec->m_iRefreshDisplay_AllMB_Period = 3;
    }
    else {
        pWMVDec->m_iRefreshDisplay_AllMB_Period = 1;
    }
#endif
}
