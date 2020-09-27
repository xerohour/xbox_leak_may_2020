#include "bldsetup.h"

#include <stdlib.h>
#include <stdio.h>
#include "xplatform.h"
#include "limits.h"
#include "typedef.hpp"
#include "wmvdec_member.h"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#include "tables_wmv.h"

#include "strmdec_wmv.hpp"
#include "motioncomp_wmv.h"
#include "idctdec_wmv2.h"
#include "repeatpad_wmv.h"
#include "postfilter_wmv.hpp"

#include "spatialpredictor_wmv.hpp"
#include "localhuffman_wmv.h"
#include "tables_wmv.h"
#include "wmvdec_function.h"

#define setTable(																\
		vlcTableInfo,															\
		hufDec_DCTAC,															\
		iTCOEF_ESCAPE,															\
		iNUMOFCODESINNOTLASTMinus1,												\
        igrgLevelAtIndx_HghMt,													\
        igrgRunAtIndx_HghMt,													\
		igrgIfNotLastNumOfLevelAtRun,											\
		igrgIfLastNumOfLevelAtRun,												\
		igrgIfNotLastNumOfRunAtLevel,											\
		igrgIfLastNumOfRunAtLevel												\
		)																		\
		vlcTableInfo.hufDCTACDec = hufDec_DCTAC;								\
		vlcTableInfo.iTcoef_ESCAPE = iTCOEF_ESCAPE;								\
		vlcTableInfo.iStartIndxOfLastRunMinus1 = iNUMOFCODESINNOTLASTMinus1;	\
		vlcTableInfo.pcLevelAtIndx = igrgLevelAtIndx_HghMt;						\
		vlcTableInfo.puiRunAtIndx = igrgRunAtIndx_HghMt;						\
		vlcTableInfo.puiNotLastNumOfLevelAtRun = igrgIfNotLastNumOfLevelAtRun;	\
		vlcTableInfo.puiLastNumOfLevelAtRun = igrgIfLastNumOfLevelAtRun;		\
		vlcTableInfo.puiNotLastNumOfRunAtLevel = igrgIfNotLastNumOfRunAtLevel;	\
		vlcTableInfo.puiLastNumOfRunAtLevel = igrgIfLastNumOfRunAtLevel;		\


#define UVTCOEF_ESCAPE_HghMt 168							
#define NUMOFCODESINNOTLASTINTERMinus1_HghMt 98
#define TCOEF_ESCAPEINTRAY_HghMt 185							
#define NUMOFCODESINNOTLASTINTRAMinus1_HghMt 118

#define UVTCOEF_ESCAPE_Talking 148							
#define NUMOFCODESINNOTLASTINTERMinus1_Talking 80
#define TCOEF_ESCAPEINTRAY_Talking 132							
#define NUMOFCODESINNOTLASTINTRAMinus1_Talking 84

#define UVTCOEF_ESCAPE_MPEG4 102							
#define NUMOFCODESINNOTLASTINTERMinus1_MPEG4 57
#define TCOEF_ESCAPEINTRAY_MPEG4 102
#define NUMOFCODESINNOTLASTINTRAMinus1_MPEG4 66

//I32_WMV sm_iIDCTDecCount = 0;


Void_WMV setVlcTableInfo(tWMVDecInternalMember *pWMVDec)
{
	/// Set tables for High Motion
    setTable(
		pWMVDec->InterDCTTableInfo_Dec_HghMt,
		&pWMVDec->m_hufDCTACInterDec_HghMt,
		UVTCOEF_ESCAPE_HghMt,
		NUMOFCODESINNOTLASTINTERMinus1_HghMt,
		//STARTINDEXOFLASTRUN_HghMt, 
        sm_rgLevelAtIndx_HghMt, 
        sm_rgRunAtIndx_HghMt, 
		sm_rgIfNotLastNumOfLevelAtRun_HghMt,
		sm_rgIfLastNumOfLevelAtRun_HghMt,
		sm_rgIfNotLastNumOfRunAtLevel_HghMt,
		sm_rgIfLastNumOfRunAtLevel_HghMt
		);

    setTable(
		pWMVDec->IntraDCTTableInfo_Dec_HghMt,
		&pWMVDec->m_hufDCTACIntraDec_HghMt,
		TCOEF_ESCAPEINTRAY_HghMt,
		NUMOFCODESINNOTLASTINTRAMinus1_HghMt,
		//STARTINDEXOFLASTRUN_HghMt, 
        sm_rgLevelAtIndxOfIntraY_HghMt, 
        sm_rgRunAtIndxOfIntraY_HghMt, 
		sm_rgIfNotLastNumOfLevelAtRunIntraY_HghMt,
		sm_rgIfLastNumOfLevelAtRunIntraY_HghMt,
		sm_rgIfNotLastNumOfRunAtLevelIntraY_HghMt,
		sm_rgIfLastNumOfRunAtLevelIntraY_HghMt
		);

	/// Set tables for Talking head
    setTable(
		pWMVDec->InterDCTTableInfo_Dec_Talking,
		&pWMVDec->m_hufDCTACInterDec_Talking,
		UVTCOEF_ESCAPE_Talking,
		NUMOFCODESINNOTLASTINTERMinus1_Talking,
		//STARTINDEXOFLASTRUN_Talking, 
        sm_rgLevelAtIndx_Talking, 
        sm_rgRunAtIndx_Talking, 
		sm_rgIfNotLastNumOfLevelAtRun_Talking,
		sm_rgIfLastNumOfLevelAtRun_Talking,
		sm_rgIfNotLastNumOfRunAtLevel_Talking,
		sm_rgIfLastNumOfRunAtLevel_Talking
		);

    setTable(
		pWMVDec->IntraDCTTableInfo_Dec_Talking,
		&pWMVDec->m_hufDCTACIntraDec_Talking,
		TCOEF_ESCAPEINTRAY_Talking,
		NUMOFCODESINNOTLASTINTRAMinus1_Talking,
		//STARTINDEXOFLASTRUN_Talking, 
        sm_rgLevelAtIndxOfIntraY_Talking, 
        sm_rgRunAtIndxOfIntraY_Talking, 
		sm_rgIfNotLastNumOfLevelAtRunIntraY_Talking,
		sm_rgIfLastNumOfLevelAtRunIntraY_Talking,
		sm_rgIfNotLastNumOfRunAtLevelIntraY_Talking,
		sm_rgIfLastNumOfRunAtLevelIntraY_Talking
		);

	/// Set tables for MPEG4
    setTable(
		pWMVDec->InterDCTTableInfo_Dec_MPEG4,
		&pWMVDec->m_hufDCTACInterDec_MPEG4,
		UVTCOEF_ESCAPE_MPEG4,
		NUMOFCODESINNOTLASTINTERMinus1_MPEG4,
		//STARTINDEXOFLASTRUN_MPEG4, 
        sm_rgLevelAtIndx_MPEG4, 
        sm_rgRunAtIndx_MPEG4, 
		sm_rgIfNotLastNumOfLevelAtRun_MPEG4,
		sm_rgIfLastNumOfLevelAtRun_MPEG4,
		sm_rgIfNotLastNumOfRunAtLevel_MPEG4,
		sm_rgIfLastNumOfRunAtLevel_MPEG4
		);

    setTable(
		pWMVDec->IntraDCTTableInfo_Dec_MPEG4,
		&pWMVDec->m_hufDCTACIntraDec_MPEG4,
		TCOEF_ESCAPEINTRAY_MPEG4,
		NUMOFCODESINNOTLASTINTRAMinus1_MPEG4,
		//STARTINDEXOFLASTRUN_MPEG4, 
        sm_rgLevelAtIndxOfIntraY_MPEG4, 
        sm_rgRunAtIndxOfIntraY_MPEG4, 
		sm_rgIfNotLastNumOfLevelAtRunIntraY_MPEG4,
		sm_rgIfLastNumOfLevelAtRunIntraY_MPEG4,
		sm_rgIfNotLastNumOfRunAtLevelIntraY_MPEG4,
		sm_rgIfLastNumOfRunAtLevelIntraY_MPEG4
		);

	pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_Talking;
	pWMVDec->m_pInterDCTTableInfo_Dec_Set[1] = &pWMVDec->InterDCTTableInfo_Dec_HghMt;
	pWMVDec->m_pInterDCTTableInfo_Dec_Set[2] = &pWMVDec->InterDCTTableInfo_Dec_MPEG4;



	pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_Talking;
	pWMVDec->m_pIntraDCTTableInfo_Dec_Set[1] = &pWMVDec->IntraDCTTableInfo_Dec_HghMt;
	pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2] = &pWMVDec->IntraDCTTableInfo_Dec_MPEG4;

    pWMVDec->m_pHufDCTDCDec_Set[0] = &pWMVDec->m_hufDCTDCyDec_Talking;
    pWMVDec->m_pHufDCTDCDec_Set[1] = &pWMVDec->m_hufDCTDCcDec_Talking;
    pWMVDec->m_pHufDCTDCDec_Set[2] = &pWMVDec->m_hufDCTDCyDec_HghMt;
    pWMVDec->m_pHufDCTDCDec_Set[3] = &pWMVDec->m_hufDCTDCcDec_HghMt;


	pWMVDec->m_pHufMVDec_Set[0] = &pWMVDec->m_hufMVDec_Talking;
	pWMVDec->m_pHufMVDec_Set[1] = &pWMVDec->m_hufMVDec_HghMt;
	pWMVDec->m_puMvFromIndex_Set[0] = sm_uXMvFromIndex_Talking;
	pWMVDec->m_puMvFromIndex_Set[1] = sm_uYMvFromIndex_Talking;
	pWMVDec->m_puMvFromIndex_Set[2] = sm_uXMvFromIndex_HghMt;
	pWMVDec->m_puMvFromIndex_Set[3] = sm_uYMvFromIndex_HghMt;

}

Void_WMV freeFrame(tYUV420Frame_WMV* pFrm)
{
    FREE_PTR(pFrm->m_pucYPlane_Unaligned);
    FREE_PTR(pFrm->m_pucUPlane_Unaligned);
    FREE_PTR(pFrm->m_pucVPlane_Unaligned);
    FREE_PTR(pFrm);
}

#if defined(WANT_HEAP_MEASURE)
#   define DEBUG_HEAP_SUB_FRAME(a)                                          \
    {                                                                       \
        I32_WMV iWidthMBAligned = (pWMVDec->m_iFrmWidthSrc + 15) & ~15;     \
        I32_WMV iHeightMBAligned = (pWMVDec->m_iFrmHeightSrc + 15) & ~15;   \
        I32_WMV iSizeYplane = (iWidthMBAligned + EXPANDY_REFVOP * 2) * (iHeightMBAligned + EXPANDY_REFVOP * 2);                     \
        I32_WMV iSizeUVplane = ((iWidthMBAligned >> 1) + EXPANDUV_REFVOP * 2) * ((iHeightMBAligned  >> 1)+ EXPANDUV_REFVOP * 2);    \
        DEBUG_HEAP_SUB( 0, a * sizeof(tYUV420Frame_WMV) );                                                                             \
        DEBUG_HEAP_SUB( 1, a * (((iSizeYplane + 32)*sizeof(U8_WMV)) + 2*((iSizeUVplane + 32) * sizeof (U8_WMV)) ) );                \
    }
#else
#   define DEBUG_HEAP_SUB_FRAME(a)
#endif


tWMVDecodeStatus  VodecDestruct (tWMVDecInternalMember *pWMVDec)
{
    FREE_PTR(pWMVDec -> m_pInputBitstream);
    DEBUG_HEAP_SUB( 0, sizeof(CInputBitStream_WMV));
    freeFrame (pWMVDec->m_pfrmCurrQ);
    freeFrame (pWMVDec->m_pfrmPrev);
    DEBUG_HEAP_SUB_FRAME(2);
#ifdef _SUPPORT_POST_FILTERS_
    freeFrame (pWMVDec->m_pfrmPostQ);
    DEBUG_HEAP_SUB_FRAME(1);
#endif
    DELETE_ARRAY (pWMVDec->m_rgmv);
    DELETE_ARRAY (pWMVDec->m_rgmbmd);
    DELETE_PTR (pWMVDec->m_pmbmdZeroCBPCY);
    DELETE_ARRAY (pWMVDec->m_pAvgQuanDctCoefDec);
    DELETE_ARRAY (pWMVDec->m_pAvgQuanDctCoefDecC);
    DELETE_ARRAY (pWMVDec->m_pAvgQuanDctCoefDecLeft);
    DELETE_ARRAY (pWMVDec->m_pAvgQuanDctCoefDecTop);
    DEBUG_HEAP_SUB( 0, (pWMVDec->m_uintNumMBX * 2 * sizeof(CVector)) 
                  + (pWMVDec->m_uintNumMB * sizeof(CWMVMBMode)) 
                  + sizeof(CWMVMBMode) 
                  + (4 * BLOCK_SIZE_TIMES2 * sizeof(I16_WMV)) );

    DELETE_ARRAY (pWMVDec->m_rgiQuanCoefACPred);
    DELETE_ARRAY (pWMVDec->m_rgiQuanCoefACPredTable);
    DELETE_ARRAY (pWMVDec->m_rgchSkipPrevFrame);
    pWMVDec->m_pBMPBits = NULL_WMV;
    DEBUG_HEAP_SUB( 0, (pWMVDec->m_uintNumMBX * 2 * BLOCK_SIZE_TIMES2_TIMES6 * sizeof(I16_WMV))
                  + (pWMVDec->m_uintNumMBX * 2  * 36 * sizeof(I16_WMV*)) 
                  + (pWMVDec->m_uintNumMB*sizeof(U8_WMV)) );

    DEBUG_HEAP_SUB(g_iHeapLastClass,0);  // indicate the following huffman tables are localHuffman

    t_AltTablesDecoderDestruct (pWMVDec->m_pAltTables);
    t_SpatialPredictorDestruct (pWMVDec->m_pSp);
    t_ContextWMVDestruct (pWMVDec->m_pContext);

    DEBUG_HEAP_SUB(DHEAP_HUFFMAN,0);  // indicate the following huffman tables are regular tables (non-localHuffman)

    Huffman_WMV_destruct(&pWMVDec->m_hufMVDec_Talking);
    Huffman_WMV_destruct(&pWMVDec->m_hufMVDec_HghMt);

    Huffman_WMV_destruct(&pWMVDec->m_hufDCTDCyDec_Talking);
    Huffman_WMV_destruct(&pWMVDec->m_hufDCTDCcDec_Talking);

    Huffman_WMV_destruct(&pWMVDec->m_hufDCTDCyDec_HghMt);
    Huffman_WMV_destruct(&pWMVDec->m_hufDCTDCcDec_HghMt);

    Huffman_WMV_destruct(&pWMVDec->m_hufICBPCYDec);
    Huffman_WMV_destruct(&pWMVDec->m_hufPCBPCYDec);

    Huffman_WMV_destruct(&pWMVDec->m_hufPCBPCYDec_HighRate);
    Huffman_WMV_destruct(&pWMVDec->m_hufPCBPCYDec_MidRate);
    Huffman_WMV_destruct(&pWMVDec->m_hufPCBPCYDec_LowRate);

    Huffman_WMV_destruct(&pWMVDec->m_hufDCTACInterDec_HghMt);
    Huffman_WMV_destruct(&pWMVDec->m_hufDCTACIntraDec_HghMt);
    Huffman_WMV_destruct(&pWMVDec->m_hufDCTACInterDec_Talking);
    Huffman_WMV_destruct(&pWMVDec->m_hufDCTACIntraDec_Talking);
    Huffman_WMV_destruct(&pWMVDec->m_hufDCTACInterDec_MPEG4);
    Huffman_WMV_destruct(&pWMVDec->m_hufDCTACIntraDec_MPEG4);

    return WMV_Succeeded;

}

Void_WMV InitVars_Tmp(tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_t = 0; 
// for _MPEG4_BITSTRM
    pWMVDec->m_tModuloBaseDecd = 0;
    pWMVDec->m_tModuloBaseDisp = 0;
    pWMVDec->m_iNumBitsTimeIncr = 0;
// _MPEG4_BITSTRM
	pWMVDec->m_iMvResolution = 0; 
    pWMVDec->m_bDCTTable_MB_ENABLED= FALSE_WMV; 
	pWMVDec->m_bDCTTable_MB= FALSE_WMV; 
	pWMVDec->m_bDCPred_IMBInPFrame= FALSE_WMV;
    pWMVDec->m_bMixedPel= FALSE_WMV; 
	pWMVDec->m_bLoopFilter= FALSE_WMV; 
	pWMVDec->m_bXformSwitch = FALSE_WMV;
    pWMVDec->m_bSKIPBIT_CODING_= FALSE_WMV; 
	pWMVDec->m_bNEW_PCBPCY_TABLE= FALSE_WMV; 
	pWMVDec->m_bXintra8Switch = FALSE_WMV;
    pWMVDec->m_bXintra8 = FALSE_WMV; 
	pWMVDec->m_bFrmHybridMVOn = FALSE_WMV;

    pWMVDec->m_bRndCtrlOn = FALSE_WMV;
    pWMVDec->m_bDeblockOn = FALSE_WMV; // no deblocking at the beginning
    pWMVDec->m_bDeringOn = FALSE_WMV;
    pWMVDec->m_fPrepared = FALSE_WMV;
    pWMVDec->m_fDecodedI = FALSE_WMV;    // Haven't seen an I-Picture yet.
    pWMVDec->m_rgchSkipPrevFrame = NULL_WMV; pWMVDec->m_rgmv = NULL_WMV;
    pWMVDec->m_pBMPBits = NULL_WMV;
    pWMVDec->m_rgiQuanCoefACPred = NULL_WMV;
    pWMVDec->m_rgiQuanCoefACPredTable= NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDec = NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDecC = NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDecLeft = NULL_WMV;
    pWMVDec->m_pAvgQuanDctCoefDecTop = NULL_WMV;
    pWMVDec->m_rgmbmd = NULL_WMV;
    pWMVDec->m_pmbmdZeroCBPCY = NULL_WMV;
    pWMVDec->m_iPostProcessMode= 0;
    pWMVDec->m_bUseOldSetting= FALSE_WMV;
    pWMVDec->m_iPostFilterLevel= -1;
    pWMVDec->m_iDCTACInterTableIndx= 0;
    pWMVDec->m_iDCTACIntraTableIndx= 0;
    pWMVDec->m_bRefreshDisplay_AllMB_Enable= FALSE_WMV;
    pWMVDec->m_bRefreshDisplay_AllMB= FALSE_WMV;
    pWMVDec->m_iRefreshDisplay_AllMB_Cnt= 0;
    pWMVDec->m_iRefreshDisplay_AllMB_Period= 1000;
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE_WMV;
}

#ifndef _XBOX
Bool_WMV g_bSupportMMX_WMV = 0;
#endif //  _XBOX

tWMVDecodeStatus VodecConstruct (
    tWMVDecInternalMember *pWMVDec,
    U32_WMV               dwFOURCC,
    I32_WMV                 iFrameRate,
    I32_WMV                 iWidthSource,
    I32_WMV                 iHeightSource,
    CInputBitStream_WMV     *pInputBitstream//,
)
{
    VResult vr;
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    I32_WMV maxBits[3];

#ifdef  _WMV_TARGET_X86_
#ifndef _XBOX
    g_bSupportMMX_WMV = g_SupportMMX ();
#endif //  _XBOX
#endif //  _WMV_TARGET_X86_

    //VOConstruct();
	pWMVDec->m_uintNumMBYSlice = 1;
    pWMVDec->m_bCODFlagOn = TRUE_WMV; 
    pWMVDec->m_bFirstEscCodeInFrame= TRUE_WMV; 
	pWMVDec->m_uiNumProcessors= 1;
    //VODecConstruct
    pWMVDec->m_iFrameRate = iFrameRate;

    pWMVDec->m_iRefreshDisplay_AllMB_Period= 1000;
    pWMVDec->m_iPostProcessMode= 0;
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE_WMV;

    Huffman_WMV_construct(&pWMVDec->m_hufMVDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufMVDec_HghMt);

    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCyDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCcDec_Talking);

    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCyDec_HghMt);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTDCcDec_HghMt);

    Huffman_WMV_construct(&pWMVDec->m_hufICBPCYDec);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec);

    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_HighRate);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_MidRate);
    Huffman_WMV_construct(&pWMVDec->m_hufPCBPCYDec_LowRate);

    Huffman_WMV_construct(&pWMVDec->m_hufDCTACInterDec_HghMt);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACIntraDec_HghMt);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACInterDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACIntraDec_Talking);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACInterDec_MPEG4);
    Huffman_WMV_construct(&pWMVDec->m_hufDCTACIntraDec_MPEG4);

    InitVars_Tmp(pWMVDec);



    if (dwFOURCC == FOURCC_WMV2_WMV || dwFOURCC == FOURCC_wmv2_WMV)
        pWMVDec->m_cvCodecVersion = WMV2;
    else if (dwFOURCC == FOURCC_WMV1_WMV || dwFOURCC == FOURCC_wmv1_WMV)
        pWMVDec->m_cvCodecVersion = WMV1;
    else if (dwFOURCC == FOURCC_MP43_WMV || dwFOURCC == FOURCC_mp43_WMV)
        pWMVDec->m_cvCodecVersion = MP43;
    else if (dwFOURCC == FOURCC_MP42_WMV || dwFOURCC == FOURCC_mp42_WMV)
        pWMVDec->m_cvCodecVersion = MP42;
    else if (dwFOURCC == FOURCC_MP4S_WMV || dwFOURCC == FOURCC_mp4s_WMV)
        pWMVDec->m_cvCodecVersion = MP4S;
//    else if (dwFOURCC == FOURCC_MPG4 || dwFOURCC == FOURCC_mpg4)
//        pWMVDec->m_cvCodecVersion = WMV2;
    {
#if !defined(NO_WINDOWS) && defined(_MultiThread_Decode_)  
    SYSTEM_INFO sysinfo;
    GetSystemInfo (&sysinfo);
    pWMVDec->m_uiNumProcessors = sysinfo.dwNumberOfProcessors;
#else
	pWMVDec->m_uiNumProcessors = 1;
#endif
    }

#if defined (CALC_SNR)
    pWMVDec->m_uiNumProcessors = 1;  // implemented for only one processor
#endif

    if (pWMVDec->m_uiNumProcessors == 3)
        pWMVDec->m_uiNumProcessors = 2;
    if (pWMVDec->m_uiNumProcessors > 4)
        pWMVDec->m_uiNumProcessors = 4;
pWMVDec->m_uiNumProcessors = 1;


    g_InitDecGlobalVars ();
    decideMMXRoutines (pWMVDec);
    m_InitFncPtrAndZigzag(pWMVDec);
    tWMVStatus = InitDataMembers (pWMVDec, iWidthSource, iHeightSource);

	if (tWMVStatus != WMV_Succeeded) {
		return tWMVStatus;
	}

    pWMVDec->m_pbitstrmIn = pInputBitstream;

    // Set 32-byte aligned pointers to IDCT recon coeff and pixel error buffers
    pWMVDec->m_ppxliErrorQ = (Buffer *)(((I32_WMV)pWMVDec->m_riPixelError + 31) & ~31);
    pWMVDec->m_rgiCoefReconBuf = (Buffer *)(((I32_WMV)pWMVDec->m_riReconBuf + 31) & ~31);
    pWMVDec->m_rgiCoefRecon = (PixelI32 *)pWMVDec->m_rgiCoefReconBuf;

    pWMVDec->m_rgiCoefReconPlus1 = &pWMVDec->m_rgiCoefRecon[1];
    pWMVDec->m_iStepSize = 0;

    pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompUV = pWMVDec->m_pMotionCompRndCtrlOff;
    pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompAndAddErrorUV = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff;
	
#ifdef DYNAMIC_EDGEPAD                
    pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_OFF;
    pWMVDec->m_tEdgePad.m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;
#endif
    
    //_MV

    DEBUG_HEAP_ADD(DHEAP_HUFFMAN,0);  // indicate these huffman tables are regular tables (non-localHuffman)

    maxBits[0] = 6,  maxBits[1] = 5,  maxBits[2] = 6;
    vr = Huffman_WMV_init(&pWMVDec->m_hufMVDec_Talking, pWMVDec->m_uiUserData, sm_HufMVTable_Talking, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 5,  maxBits[1] = 5,  maxBits[2] = 5;
    vr = Huffman_WMV_init(&pWMVDec->m_hufMVDec_HghMt, pWMVDec->m_uiUserData, sm_HufMVTable_HghMt, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 6,  maxBits[1] = 3,  maxBits[2] = 4;
    vr = Huffman_WMV_init(&pWMVDec->m_hufICBPCYDec,pWMVDec->m_uiUserData, sm_HufICBPCYTable, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 7; maxBits[1] = 6; maxBits[2] = 8;
    vr = Huffman_WMV_init(&pWMVDec->m_hufPCBPCYDec, pWMVDec->m_uiUserData, sm_HufPCBPCYTable, maxBits, 3);
    if (vr.failed()) goto lerror;

    //NEW_PCBPCY_TABLE
    maxBits[0] = 9; maxBits[1] = 7; maxBits[2] = 8;
    vr = Huffman_WMV_init(&pWMVDec->m_hufPCBPCYDec_HighRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_HighRate, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 7; maxBits[1] = 6; maxBits[2] = 7;
    vr = Huffman_WMV_init(&pWMVDec->m_hufPCBPCYDec_MidRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_MidRate, maxBits, 3);
    if (vr.failed()) goto lerror;
    
    maxBits[0] = 6; maxBits[1] = 6; maxBits[2] = 5;
    vr = Huffman_WMV_init(&pWMVDec->m_hufPCBPCYDec_LowRate, pWMVDec->m_uiUserData, sm_HufPCBPCYTable_LowRate, maxBits, 3);
    if (vr.failed()) goto lerror;

	//_DCTDC
    maxBits[0] = 8; maxBits[1] = 8; maxBits[2] = 8;
    vr = Huffman_WMV_init(&pWMVDec->m_hufDCTDCyDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTDCyTable_Talking, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 8; maxBits[1] = 7; maxBits[2] = 8;
    vr = Huffman_WMV_init(&pWMVDec->m_hufDCTDCcDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTDCcTable_Talking, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 9; maxBits[1] = 9; maxBits[2] = 8;    
    vr = Huffman_WMV_init(&pWMVDec->m_hufDCTDCyDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTDCyTable_HghMt, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 9; maxBits[1] = 8; maxBits[2] = 8;    
    vr = Huffman_WMV_init(&pWMVDec->m_hufDCTDCcDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTDCcTable_HghMt, maxBits, 3);
    if (vr.failed()) goto lerror;
	// _DCTAC
    maxBits[0] = 6,  maxBits[1] = 5,  maxBits[2] = 4;
	vr = Huffman_WMV_init(&pWMVDec->m_hufDCTACInterDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_HghMt, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 7; maxBits[1] = 4; maxBits[2] = 4;
	vr = Huffman_WMV_init(&pWMVDec->m_hufDCTACIntraDec_HghMt, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_HghMt, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 7; maxBits[1] = 4; maxBits[2] = 4;
	vr = Huffman_WMV_init(&pWMVDec->m_hufDCTACInterDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_Talking, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 5; maxBits[1] = 4; maxBits[2] = 4;
	vr = Huffman_WMV_init(&pWMVDec->m_hufDCTACIntraDec_Talking, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_Talking, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 3; maxBits[1] = 6; maxBits[2] = 3;
	vr = Huffman_WMV_init(&pWMVDec->m_hufDCTACInterDec_MPEG4, pWMVDec->m_uiUserData, sm_HufDCTACInterTable_MPEG4, maxBits, 3);
    if (vr.failed()) goto lerror;

    maxBits[0] = 3; maxBits[1] = 6; maxBits[2] = 3;
	vr = Huffman_WMV_init(&pWMVDec->m_hufDCTACIntraDec_MPEG4, pWMVDec->m_uiUserData, sm_HufDCTACIntraTable_MPEG4, maxBits, 3);
    if (vr.failed()) goto lerror;


    pWMVDec->m_pAltTables = t_AltTablesDecoderConstruct (pWMVDec->m_uiUserData);
    if (pWMVDec->m_pAltTables == NULL_WMV)
        goto lerror;
    // Construct spatial predictor
    pWMVDec->m_pSp = t_SpatialPredictorConstruct ();
    if (!pWMVDec->m_pSp)
        goto lerror;

    // Construct contexts, and DCT array
    pWMVDec->m_pContext = t_ContextWMVConstruct (pWMVDec->m_uintNumMBX*2, 2);  // 2 rows enough
    if (!pWMVDec->m_pContext)
        goto lerror;

	setVlcTableInfo(pWMVDec);
    return WMV_Succeeded;

lerror:
    return WMV_BadMemory;
}


Void_WMV computePars (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_cvCodecVersion != MP4S) {
        pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iWidthY;
        pWMVDec->m_iWidthUVRepeatPad = pWMVDec->m_iWidthUV;
        pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iHeightY;
        pWMVDec->m_iHeightUVRepeatPad = pWMVDec->m_iHeightUV;
    }
    else {
        pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iFrmWidthSrc;
        pWMVDec->m_iWidthUVRepeatPad = pWMVDec->m_iFrmWidthSrc/2;
        pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iFrmHeightSrc;
        pWMVDec->m_iHeightUVRepeatPad = pWMVDec->m_iFrmHeightSrc/2;
    }
    pWMVDec->m_iWidthYPlusExp = pWMVDec->m_iWidthYRepeatPad + EXPANDY_REFVOP;
    pWMVDec->m_iWidthUVPlusExp = pWMVDec->m_iWidthUVRepeatPad + EXPANDUV_REFVOP;

    pWMVDec->m_bMBAligned = (pWMVDec->m_iWidthY == pWMVDec->m_iFrmWidthSrc && pWMVDec->m_iHeightY == pWMVDec->m_iFrmHeightSrc);
    // see if the size is multiples of MB_SIZE

    pWMVDec->m_uintNumMBX = pWMVDec->m_iWidthY >> 4; // divided by 16, MB size
    pWMVDec->m_uintNumMBY = pWMVDec->m_iHeightY >> 4;
    pWMVDec->m_uintNumMB = pWMVDec->m_uintNumMBX * pWMVDec->m_uintNumMBY;
    pWMVDec->m_uiRightestMB = pWMVDec->m_uintNumMBX - 1;

    // for motion compensation
    pWMVDec->m_iWidthPrevY = pWMVDec->m_iWidthY + (EXPANDY_REFVOP << 1);
    pWMVDec->m_iWidthPrevUV = pWMVDec->m_iWidthUV + (EXPANDUV_REFVOP << 1);
    pWMVDec->m_iHeightPrevY = pWMVDec->m_iHeightY + (EXPANDY_REFVOP << 1);
    pWMVDec->m_iHeightPrevUV = pWMVDec->m_iHeightUV + (EXPANDUV_REFVOP << 1);
    
    // for update
    pWMVDec->m_iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevY * EXPANDY_REFVOP + EXPANDY_REFVOP;
    pWMVDec->m_iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUV * EXPANDUV_REFVOP + EXPANDUV_REFVOP;

    // for motion estimation
	pWMVDec->m_iMBSizeXWidthPrevY = pWMVDec->m_iWidthPrevY << 4; // * MB_SIZE
	pWMVDec->m_iBlkSizeXWidthPrevUV = pWMVDec->m_iWidthPrevUV << 3; // * BLOCK_SIZE
}

#ifdef IPAQ_HACK
    I32_WMV ipaq_hack(U8_WMV * & pData, I32_WMV size);

#   define CHECK_MALLOC2(x,y)               \
    {                                       \
       I32_WMV reslt=ipaq_hack(ppxlTmp,y);  \
           x = ppxlTmp;                     \
        if (ppxlTmp == NULL_WMV)            \
            return WMV_BadMemory;           \
       DEBUG_HEAP_ADD(1,y);                 \
    }
#else
#   define CHECK_MALLOC2(x,y) DEBUG_HEAP_ADD(1,y);
#endif

tWMVDecodeStatus InitDataMembers (    
    tWMVDecInternalMember *pWMVDec, 
    I32_WMV                iWidthSource, 
    I32_WMV                iHeightSource
)
{
    I32_WMV iWidthMBAligned = (iWidthSource + 15) & ~15;
    I32_WMV iHeightMBAligned = (iHeightSource + 15) & ~15;
    I32_WMV iSizeYplane, iSizeUVplane;
    U8_WMV *ppxlTmp;

    pWMVDec->m_iFrmWidthSrc = iWidthSource;
    pWMVDec->m_iFrmHeightSrc = iHeightSource;
    pWMVDec->m_bMBAligned = (
        pWMVDec->m_iFrmWidthSrc == iWidthMBAligned && 
        pWMVDec->m_iFrmHeightSrc == iHeightMBAligned);

    pWMVDec->m_iMVLeftBound = -63; // in half pel
    pWMVDec->m_iMVRightBound = 63; // in half pel

    pWMVDec->m_iWidthY = iWidthMBAligned;
    pWMVDec->m_iWidthUV = pWMVDec->m_iWidthY >> 1;
    pWMVDec->m_iHeightY = iHeightMBAligned;
    pWMVDec->m_iHeightUV = pWMVDec->m_iHeightY >> 1;

    //I32_WMV result = assignVODataMembers ();
    computePars (pWMVDec);

    // Default transform-switching settings
    pWMVDec->m_iFrameXformMode = XFORMMODE_8x8;
    pWMVDec->m_bMBXformSwitching = FALSE_WMV;

    // Allocate Frame memory
    pWMVDec->m_pfrmCurrQ = NULL_WMV; 
    pWMVDec->m_pfrmPrev = NULL_WMV; 
    pWMVDec->m_pfrmPostQ = NULL_WMV;

    iSizeYplane = (iWidthMBAligned + EXPANDY_REFVOP * 2) * (iHeightMBAligned + EXPANDY_REFVOP * 2);
    iSizeUVplane = ((iWidthMBAligned >> 1) + EXPANDUV_REFVOP * 2) * ((iHeightMBAligned  >> 1)+ EXPANDUV_REFVOP * 2);

    pWMVDec->m_pfrmCurrQ = (tYUV420Frame_WMV*) wmvalloc (sizeof (tYUV420Frame_WMV));
    if (pWMVDec->m_pfrmCurrQ == NULL_WMV)
        return WMV_BadMemory;

    pWMVDec->m_pfrmPrev = (tYUV420Frame_WMV*) wmvalloc (sizeof (tYUV420Frame_WMV));
    if (pWMVDec->m_pfrmPrev == NULL_WMV)
        return WMV_BadMemory;

    DEBUG_HEAP_ADD( 0, 2 * sizeof (tYUV420Frame_WMV));

    pWMVDec->m_pfrmCurrQ->m_pucYPlane_Unaligned = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeYplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;

    CHECK_MALLOC2(pWMVDec->m_pfrmCurrQ->m_pucYPlane_Unaligned,(iSizeYplane + 32)*sizeof(U8_WMV));

    pWMVDec->m_pfrmCurrQ->m_pucYPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);

    pWMVDec->m_pfrmCurrQ->m_pucUPlane_Unaligned = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeUVplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;
    
    CHECK_MALLOC2(pWMVDec->m_pfrmCurrQ->m_pucUPlane_Unaligned,(iSizeUVplane + 32) * sizeof (U8_WMV));

    pWMVDec->m_pfrmCurrQ->m_pucUPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);
    pWMVDec->m_pfrmCurrQ->m_pucVPlane_Unaligned = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeUVplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;
    
    CHECK_MALLOC2(pWMVDec->m_pfrmCurrQ->m_pucVPlane_Unaligned,(iSizeUVplane + 32) * sizeof (U8_WMV));
    

    pWMVDec->m_pfrmCurrQ->m_pucVPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);

    pWMVDec->m_pfrmPrev->m_pucYPlane_Unaligned  = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeYplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;

    CHECK_MALLOC2(pWMVDec->m_pfrmPrev->m_pucYPlane_Unaligned,(iSizeYplane + 32) * sizeof (U8_WMV));
   

    pWMVDec->m_pfrmPrev->m_pucYPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);
    pWMVDec->m_pfrmPrev->m_pucUPlane_Unaligned  = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeUVplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;

    CHECK_MALLOC2(pWMVDec->m_pfrmPrev->m_pucUPlane_Unaligned ,(iSizeUVplane + 32) * sizeof (U8_WMV));
   

    pWMVDec->m_pfrmPrev->m_pucUPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);
    pWMVDec->m_pfrmPrev->m_pucVPlane_Unaligned  = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeUVplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;

    CHECK_MALLOC2(pWMVDec->m_pfrmPrev->m_pucVPlane_Unaligned, (iSizeUVplane + 32) * sizeof (U8_WMV));
   

    pWMVDec->m_pfrmPrev->m_pucVPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);

    pWMVDec->m_ppxliCurrQPlusExpY = pWMVDec->m_pfrmCurrQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpU = pWMVDec->m_pfrmCurrQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpV = pWMVDec->m_pfrmCurrQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmPrev->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmPrev->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmPrev->m_pucVPlane;
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

#ifdef _SUPPORT_POST_FILTERS_
    pWMVDec->m_pfrmPostQ = (tYUV420Frame_WMV*) wmvalloc (sizeof (tYUV420Frame_WMV));
    if (pWMVDec->m_pfrmPostQ == NULL_WMV)
        return WMV_BadMemory;
    DEBUG_HEAP_ADD(0, sizeof (tYUV420Frame_WMV));

    pWMVDec->m_pfrmPostQ->m_pucYPlane_Unaligned  = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeYplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;
    pWMVDec->m_pfrmPostQ->m_pucYPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);
    DEBUG_HEAP_ADD(1, (iSizeYplane + 32) * sizeof (U8_WMV));

    pWMVDec->m_pfrmPostQ->m_pucUPlane_Unaligned  = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeUVplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;
    pWMVDec->m_pfrmPostQ->m_pucUPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);
    DEBUG_HEAP_ADD(1, (iSizeUVplane + 32) * sizeof (U8_WMV));

    pWMVDec->m_pfrmPostQ->m_pucVPlane_Unaligned  = ppxlTmp = (U8_WMV *) wmvalloc ((iSizeUVplane + 32) * sizeof (U8_WMV));
    if (ppxlTmp == NULL_WMV)
        return WMV_BadMemory;
    pWMVDec->m_pfrmPostQ->m_pucVPlane  = (U8_WMV *)(((U32_WMV)ppxlTmp + 31) & ~31);
    DEBUG_HEAP_ADD(1, (iSizeUVplane + 32) * sizeof (U8_WMV));

    pWMVDec->m_ppxliPostQPlusExpY = pWMVDec->m_pfrmPostQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliPostQPlusExpU = pWMVDec->m_pfrmPostQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliPostQPlusExpV = pWMVDec->m_pfrmPostQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
#endif // _SUPPORT_POST_FILTERS_

    pWMVDec->m_rgmv = new CVector [pWMVDec->m_uintNumMBX * 2];
    if (!pWMVDec->m_rgmv)
        return WMV_BadMemory;
    DEBUG_HEAP_ADD(0,pWMVDec->m_uintNumMBX * 2 * sizeof(CVector));

    // MB data
    pWMVDec->m_rgmbmd = new CWMVMBMode [pWMVDec->m_uintNumMB];
    if (!pWMVDec->m_rgmbmd)
        return WMV_BadMemory;
    DEBUG_HEAP_ADD(0,pWMVDec->m_uintNumMB * sizeof(CWMVMBMode));

    pWMVDec->m_pmbmdZeroCBPCY = new CWMVMBMode;
    if (!pWMVDec->m_pmbmdZeroCBPCY)
        return WMV_BadMemory;
    DEBUG_HEAP_ADD(0,sizeof(CWMVMBMode));

    pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern = 0; 

    // for block-based rendering
    pWMVDec->m_iWidthPrevYTimes8Minus8 = pWMVDec->m_iWidthPrevY * 8 - 8;
    pWMVDec->m_iWidthPrevUVTimes4Minus4 = pWMVDec->m_iWidthPrevUV * 4 - 4;

    pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow = BLOCK_SIZE_TIMES2_TIMES6 * pWMVDec->m_uintNumMBX; 
    pWMVDec->m_rgiQuanCoefACPred = new I16_WMV [pWMVDec->m_uintNumMBX * 2 * BLOCK_SIZE_TIMES2_TIMES6]; // Allocate 2 rows buffer, 6 blocks in an MB
    if (!pWMVDec->m_rgiQuanCoefACPred)
        return WMV_BadMemory;
    DEBUG_HEAP_ADD(0,(pWMVDec->m_uintNumMBX * 2 * BLOCK_SIZE_TIMES2_TIMES6) * sizeof(I16_WMV));

    pWMVDec->m_rgiQuanCoefACPredTable = new I16_WMV* [pWMVDec->m_uintNumMBX * 2  * 36];
    if (!pWMVDec->m_rgiQuanCoefACPredTable)
        return WMV_BadMemory;
    DEBUG_HEAP_ADD(0,pWMVDec->m_uintNumMBX * 2  * 36 * sizeof(I16_WMV*));

    pWMVDec->m_pAvgQuanDctCoefDec = new I16_WMV [BLOCK_SIZE_TIMES2];
    pWMVDec->m_pAvgQuanDctCoefDecC = new I16_WMV [BLOCK_SIZE_TIMES2];
    pWMVDec->m_pAvgQuanDctCoefDecLeft = new I16_WMV [BLOCK_SIZE_TIMES2];
    pWMVDec->m_pAvgQuanDctCoefDecTop = new I16_WMV [BLOCK_SIZE_TIMES2];
    if (!pWMVDec->m_pAvgQuanDctCoefDec || !pWMVDec->m_pAvgQuanDctCoefDecC || !pWMVDec->m_pAvgQuanDctCoefDecLeft || !pWMVDec->m_pAvgQuanDctCoefDecTop)
        return WMV_BadMemory;
    DEBUG_HEAP_ADD(0, 4 * BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));

    memset(pWMVDec->m_pAvgQuanDctCoefDec, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));
    memset(pWMVDec->m_pAvgQuanDctCoefDecC, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));
    memset(pWMVDec->m_pAvgQuanDctCoefDecLeft, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));
    memset(pWMVDec->m_pAvgQuanDctCoefDecTop, 0, BLOCK_SIZE_TIMES2 * sizeof(I16_WMV));

    if (pWMVDec->m_cvCodecVersion == MP4S) {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 1024;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 1024;
        pWMVDec->m_bRndCtrlOn = TRUE_WMV;
    }
	else{
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;
        pWMVDec->m_bRndCtrlOn = FALSE_WMV;
    }

    {

        U32_WMV ii;
        U32_WMV imby, i;        
        I16_WMV **pDct = pWMVDec->m_rgiQuanCoefACPredTable; 
        I16_WMV *pDct1  = pWMVDec->m_rgiQuanCoefACPred;
        I32_WMV iNumOfQuanDctCoefForACPredPerRow = - pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow;
        
        for (ii = 0; ii < pWMVDec->m_uintNumMB; ii++) 
            pWMVDec->m_rgmbmd[ii].m_bBlkXformSwitchOn = FALSE_WMV;
        
        for (imby = 0, i = 0; imby < 2; imby++) {
            U32_WMV imbx;
            if (imby & 0x01)  // bTopRowBufferInOrder
                iNumOfQuanDctCoefForACPredPerRow = pWMVDec->m_iNumOfQuanDctCoefForACPredPerRow;
            
            for (imbx = 0; imbx < pWMVDec->m_uintNumMBX; i++, imbx++) {
                I32_WMV ib;
                for (ib = 0;ib < 6;ib++) {
                    switch (ib) {
                    case 0://Y_BLOCK1:
                        pDct[0] = pWMVDec->m_pAvgQuanDctCoefDec;
                        pDct[1] = pDct1 - BLOCK_SIZE_TIMES2_TIMES5; 
                        pDct[2] = pWMVDec->m_pAvgQuanDctCoefDec;
                        pDct[3] = pDct1 - iNumOfQuanDctCoefForACPredPerRow+ BLOCK_SIZE_TIMES2_TIMES2;
                        pDct[4] = pWMVDec->m_pAvgQuanDctCoefDec;
                        pDct[5] = pDct1 - iNumOfQuanDctCoefForACPredPerRow - BLOCK_SIZE_TIMES2_TIMES3;
                        break;
                    case 1://Y_BLOCK2:
                        pDct[6]  = pDct[7] =  pDct1 - BLOCK_SIZE_TIMES2;
                        pDct[8] = pWMVDec->m_pAvgQuanDctCoefDec;
                        pDct[9] = pDct1- iNumOfQuanDctCoefForACPredPerRow + BLOCK_SIZE_TIMES2_TIMES2;
                        pDct[10] = pWMVDec->m_pAvgQuanDctCoefDec;	   //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                        pDct[11] = pDct1- iNumOfQuanDctCoefForACPredPerRow + BLOCK_SIZE_TIMES2;
                        break;
                    case 2://Y_BLOCK3:
                        pDct[12] = pWMVDec->m_pAvgQuanDctCoefDec;
                        pDct[13] = pDct1 - BLOCK_SIZE_TIMES2_TIMES5 ;
                        pDct[14] = pDct[15] = pDct1 - BLOCK_SIZE_TIMES2_TIMES2;	   //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                        pDct[16] = pWMVDec->m_pAvgQuanDctCoefDec;
                        pDct[17] = pDct1 - BLOCK_SIZE_TIMES2_TIMES7 ;
                        break;
                    case 3://Y_BLOCK4:
                        pDct[18] = pDct[19] = pDct1 - BLOCK_SIZE_TIMES2;
                        pDct[20] = pDct[21] = pDct1 - BLOCK_SIZE_TIMES2_TIMES2;	   //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                        pDct[22]= pDct[23] = pDct1 - BLOCK_SIZE_TIMES2_TIMES3;
                        break;
                    case 4://U_BLOCK:
                        pDct[24] = pWMVDec->m_pAvgQuanDctCoefDecC;
                        pDct[25] = pDct1- BLOCK_SIZE_TIMES2_TIMES6 ;
                        pDct[26] = pWMVDec->m_pAvgQuanDctCoefDecC;	   //(pWMVDec->m_uintNumMBX * 6 - 2)* 8 
                        pDct[27] = pDct1 - iNumOfQuanDctCoefForACPredPerRow ;
                        pDct[28] = pWMVDec->m_pAvgQuanDctCoefDecC;
                        pDct[29] = pDct1 - iNumOfQuanDctCoefForACPredPerRow - BLOCK_SIZE_TIMES2_TIMES6;
                        break;
                    case 5://V_BLOCK:
                        pDct[30] = pWMVDec->m_pAvgQuanDctCoefDecC;
                        pDct[31] = pDct1 - BLOCK_SIZE_TIMES2_TIMES6 ;
                        pDct[32] = pWMVDec->m_pAvgQuanDctCoefDecC;
                        pDct[33] = pDct1 - iNumOfQuanDctCoefForACPredPerRow ;
                        pDct[34] = pWMVDec->m_pAvgQuanDctCoefDecC;
                        pDct[35] = pDct1 - iNumOfQuanDctCoefForACPredPerRow - BLOCK_SIZE_TIMES2_TIMES6 ;
                        break;
                    }
                    pDct1 += BLOCK_SIZE_TIMES2;
                }
                pDct += 36;
            }
        }
    }


#if !defined(_WIN32)
    assert(pWMVDec->m_uintNumMB * sizeof(U8_WMV) <= UINT_MAX);
#endif

    pWMVDec->m_rgchSkipPrevFrame = new U8_WMV [pWMVDec->m_uintNumMB];
    if (pWMVDec->m_rgchSkipPrevFrame == NULL_WMV) {
        delete [] pWMVDec->m_rgchSkipPrevFrame; pWMVDec->m_rgchSkipPrevFrame = NULL_WMV;
        return WMV_BadMemory;
    }
    DEBUG_HEAP_ADD(0,pWMVDec->m_uintNumMB*sizeof(U8_WMV));

    memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);

    pWMVDec->m_iWidthPrevYxBlkMinusBlk = (pWMVDec->m_iWidthPrevY * BLOCK_SIZE) - BLOCK_SIZE;

    return WMV_Succeeded;
}


Void_WMV decideMMXRoutines (tWMVDecInternalMember *pWMVDec)
{
    // IDCT
    pWMVDec->m_pIntraIDCT_Dec = g_IDCTDec_WMV2;

#ifndef _EMB_WMV2_
    pWMVDec->m_pInterIDCT_Dec = g_IDCTDec_WMV2;
    pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDec_WMV2;
    pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDec_WMV2;
#endif

    // g_InitRepeatRefInfo
    g_pRepeatRef0Y = g_RepeatRef0Y;
    g_pRepeatRef0UV = g_RepeatRef0UV;
    // MC
    pWMVDec->m_pMotionCompAndAddErrorRndCtrlOn = g_MotionCompAndAddErrorRndCtrl;
    pWMVDec->m_pMotionCompRndCtrlOn = g_MotionCompRndCtrl;
    pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff = g_MotionCompAndAddError;
    pWMVDec->m_pMotionCompRndCtrlOff = g_MotionComp;
    pWMVDec->m_pMotionCompZero = g_MotionCompZeroMotion_WMV;

#ifndef _EMB_WMV2_
    pWMVDec->m_pMotionCompMixed = MotionCompMixed;
    pWMVDec->m_pMotionCompMixedAndAddError = MotionCompMixedAndAddError;
#endif //_EMB_WMV2_

    // Loop Filter Selection
    pWMVDec->m_pFilterHorizontalEdge = g_FilterHorizontalEdge_WMV;
    pWMVDec->m_pFilterVerticalEdge = g_FilterVerticalEdge_WMV;

	pWMVDec->m_pBlkAvgX8_MMX = BlkAvgX8;

#if defined( _WMV_TARGET_X86_) || defined(_Embedded_x86)
    if (g_bSupportMMX_WMV) {
        pWMVDec->m_pInter8x4IDCT_Dec = g_8x4IDCTDecMMX_WMV2;
        pWMVDec->m_pInter4x8IDCT_Dec = g_4x8IDCTDecMMX_WMV2;
        pWMVDec->m_pInterIDCT_Dec = g_IDCTDecMMX_WMV2;
        pWMVDec->m_pIntraIDCT_Dec = g_IDCTDecMMX_WMV2;

        g_pRepeatRef0Y = g_RepeatRef0Y_MMX;
        g_pRepeatRef0UV = g_RepeatRef0UV_MMX;

        pWMVDec->m_pMotionCompAndAddErrorRndCtrlOn = g_MotionCompAndAddErrorRndCtrl_MMX;
        pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff = g_MotionCompAndAddError_MMX;
        pWMVDec->m_pMotionCompRndCtrlOn = g_MotionCompRndCtrl_MMX;
        pWMVDec->m_pMotionCompRndCtrlOff = g_MotionComp_MMX;
        pWMVDec->m_pMotionCompZero = g_MotionCompZeroMotion_MMX_WMV;

        pWMVDec->m_pMotionCompMixed = MotionCompMixed_MMX;
        pWMVDec->m_pMotionCompMixedAndAddError = MotionCompMixedAndAddError_MMX;

        pWMVDec->m_pBlkAvgX8_MMX = BlkAvgX8_MMX;

        pWMVDec->m_pFilterHorizontalEdge = g_FilterHorizontalEdge_MMX;
        pWMVDec->m_pFilterVerticalEdge = g_FilterVerticalEdge_MMX;

    }
#ifndef _XBOX
    if (g_SupportSSE1())
#endif // _XBOX
    {
        pWMVDec->m_pMotionCompRndCtrlOff = g_MotionComp_KNI;
        //pWMVDec->m_pMotionCompRndCtrlOn = g_MotionCompRndCtrlOn_KNI;
        pWMVDec->m_pMotionCompRndCtrlOn = g_MotionCompRndCtrl_MMX;

        pWMVDec->m_pMotionCompMixed = MotionCompMixed_KNI;
        pWMVDec->m_pMotionCompMixedAndAddError = MotionCompMixedAndAddError_KNI;
    }
#endif // _M_IX86) || _Embedded_x86

#if defined(macintosh) && defined(_MAC_VEC_OPT)
    if (g_SupportAltiVec ()) {
        pWMVDec->m_pMotionCompRndCtrlOn = g_MotionCompRndCtrl_AltiVec;
        pWMVDec->m_pMotionCompRndCtrlOff = g_MotionComp_AltiVec;
        pWMVDec->m_pMotionCompZero = g_MotionCompZeroMotion_AltiVec;
		pWMVDec->m_bSupportAltiVec = TRUE_WMV;
    }
#endif

}

Void_WMV m_InitFncPtrAndZigzag(tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_bRefreshDisplay_AllMB_Enable = TRUE_WMV;
    // At the decoder side, Both Intra and Inter Frame might use Rotated matrix depending on which idct is called.
    if (pWMVDec->m_cvCodecVersion == WMV2) {
        pWMVDec->m_p8x4ZigzagInv = grgi8x4ZigzagInv;
        pWMVDec->m_p4x8ZigzagInv = grgi4x8ZigzagInv;
        pWMVDec->m_p8x4ZigzagInvRotated = grgi8x4ZigzagInvRotated;
        pWMVDec->m_p4x8ZigzagInvRotated = grgi4x8ZigzagInvRotated;
        pWMVDec->m_bSKIPBIT_CODING_ = TRUE_WMV;
        pWMVDec->m_bNEW_PCBPCY_TABLE = TRUE_WMV;
    }

    if (pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_pZigzagInv_I = grgiZigzagInv_NEW_I;
        pWMVDec->m_pZigzagInvRotated_I = grgiZigzagInvRotated_NEW_I;
        pWMVDec->m_pHorizontalZigzagInv = grgiHorizontalZigzagInv_NEW;
        pWMVDec->m_pVerticalZigzagInv = grgiVerticalZigzagInv_NEW;
        //Decoder Only
        pWMVDec->m_pHorizontalZigzagInvRotated = grgiHorizontalZigzagInvRotated_NEW;
        pWMVDec->m_pVerticalZigzagInvRotated = grgiVerticalZigzagInvRotated_NEW;

        pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated_NEW;
        pWMVDec->m_pZigzagInv = grgiZigzagInv_NEW;

        pWMVDec->m_pDecodeIMBAcPred = DecodeIMBAcPred;    
        setRefreshPeriod(pWMVDec);
    }
    else{
        pWMVDec->m_pZigzagInv_I = grgiZigzagInv;
        pWMVDec->m_pZigzagInvRotated_I = grgiZigzagInvRotated;
        pWMVDec->m_pHorizontalZigzagInv = grgiHorizontalZigzagInv;
        pWMVDec->m_pVerticalZigzagInv = grgiVerticalZigzagInv;
        //Decoder Only
        pWMVDec->m_pHorizontalZigzagInvRotated = grgiVerticalZigzagInv;
        pWMVDec->m_pVerticalZigzagInvRotated = grgiHorizontalZigzagInv;

        pWMVDec->m_pZigzagInvRotated = grgiZigzagInvRotated;
        pWMVDec->m_pZigzagInv = grgiZigzagInv;

        pWMVDec->m_pDecodeIMBAcPred = DecodeIMBAcPred_MP4X;
    }

    pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInv;
    pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInv;
    pWMVDec->m_bRotatedIdct = FALSE_WMV;
    pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInv;
#ifdef  _WMV_TARGET_X86_
    if (g_bSupportMMX_WMV) { 
        pWMVDec->m_bRotatedIdct = TRUE_WMV;
        pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
    }
#endif //  _WMV_TARGET_X86_

    // Func Pointers
    pWMVDec->m_pDecodeI = WMVideoDecDecodeI;
    pWMVDec->m_pDecodeP = WMVideoDecDecodeP;
#ifndef _EMB_WMV2_
    pWMVDec->m_pDecodePMB=DecodePMB;
#else //_EMB_WMV2_
    pWMVDec->m_pDecodePMB=DecodePMB_EMB;
    InitEMB_FuncTable(pWMVDec);
#endif

    if (pWMVDec->m_cvCodecVersion != MP4S) {
        pWMVDec->m_iEscRunDiffV2V3 = (pWMVDec->m_cvCodecVersion >= MP43)? 1 : 0;
        pWMVDec->m_iDCPredCorrect = (pWMVDec->m_cvCodecVersion >= WMV1)? 1 : 0;

        pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode;
        pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode;

#ifdef  _WMV_TARGET_X86_
        if (g_bSupportMMX_WMV) 
            pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode_MMX;
#endif //  _WMV_TARGET_X86_

        if (pWMVDec->m_cvCodecVersion >= MP43) {
            pWMVDec->m_pDecodeMBOverheadOfIVOP = decodeMBOverheadOfIVOP_MSV;
            pWMVDec->m_pDecodeMBOverheadOfPVOP = decodeMBOverheadOfPVOP_MSV; 
        }
        else{ // MP42
            pWMVDec->m_pDecodeMBOverheadOfIVOP = decodeMBOverheadOfIVOP_New;
            pWMVDec->m_pDecodeMBOverheadOfPVOP = decodeMBOverheadOfPVOP_New; 
        }
    }
    else{
        pWMVDec->m_iDCPredCorrect = 1;
        pWMVDec->m_uintNumMBYSlice = pWMVDec->m_uintNumMBY;
        pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode_MPEG4;
#ifdef  _WMV_TARGET_X86_
        if (g_bSupportMMX_WMV) 
            pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantizeEscCode_MMX_MPEG4;
#endif //  _WMV_TARGET_X86_
        pWMVDec->m_pDecodeMBOverheadOfIVOP = decodeMBOverheadOfIVOP_MPEG4;
        pWMVDec->m_pDecodeMBOverheadOfPVOP = decodeMBOverheadOfPVOP_MPEG4; 
    }
}

#if defined(WANT_HEAP_MEASURE)
    extern "C" unsigned int g_cbHeapSize[6] = {0}, g_cbMaxHeapSize[6] = {0}, g_iHeapLastClass = 0;
#endif;

