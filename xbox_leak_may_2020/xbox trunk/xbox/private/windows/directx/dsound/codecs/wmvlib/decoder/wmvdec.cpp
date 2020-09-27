#include "bldsetup.h"

#include "xplatform.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"

#include "stdlib.h"
#include "malloc.h"

#ifdef UNDER_CE
#undef assert
#define assert ASSERT
#else
#include "assert.h"
#endif

#include "memory.h"

#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#include "math.h"

// ===========================================================================
// private functions
// ===========================================================================
#include "strmdec_wmv.hpp"
#include "repeatpad_wmv.h"

Void_WMV (*g_pRepeatRef0Y) (RepeatRef0YArgs);
Void_WMV (*g_pRepeatRef0UV) (RepeatRef0UVArgs);

// mingcl: don't delete these two.  These are for hacking the display of the video info in the player.
int g_iIFrame = 0;
int g_iQP = 0;

tWMVDecodeStatus WMVideoDecInit (
    HWMVDecoder* phWMVDecoder,
    U32_WMV uiUserData,
    U32_WMV uiFOURCCCompressed,
    Float_WMV fltFrameRate,
    Float_WMV fltBitRate,
    I32_WMV iWidthSource,
    I32_WMV iHeightSource,
    I32_WMV iPostFilterLevel
) 
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 


    pWMVDec = (tWMVDecInternalMember*) wmvalloc (sizeof (tWMVDecInternalMember));
//    pWMVDec = (tWMVDecInternalMember*) new tWMVDecInternalMember;
    if (pWMVDec == NULL_WMV) {
        *phWMVDecoder = NULL_WMV;
        return WMV_BadMemory;
    }

//    pWMVDec -> m_pInputBitstream = new CInputBitStream_WMV (uiUserData);
    pWMVDec -> m_pInputBitstream = (CInputBitStream_WMV *)wmvalloc(sizeof( CInputBitStream_WMV));
    if (!pWMVDec -> m_pInputBitstream ) {
        return WMV_BadMemory;
    }
    DEBUG_HEAP_ADD( 0, sizeof(CInputBitStream_WMV) ); 

    BS_construct(pWMVDec -> m_pInputBitstream, uiUserData);

    //memset (pWMVDec, 0, sizeof (tWMVDecInternalMember));
    tWMVStatus = WMVInternalMemberInit (
        pWMVDec, 
        uiUserData,
        uiFOURCCCompressed,
        fltFrameRate,
        fltBitRate,
        iWidthSource,
        iHeightSource,
        iPostFilterLevel
    );

    if (tWMVStatus == WMV_Succeeded)
        *phWMVDecoder = (HWMVDecoder) pWMVDec;
    else 
        *phWMVDecoder = NULL_WMV;

    return tWMVStatus;
}


tWMVDecodeStatus WMVideoDecClose (HWMVDecoder phWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

    if (phWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) phWMVDecoder;
    tWMVStatus = WMVInternalMemberDestroy (pWMVDec);
    // NOTE (JHarding, 1/14/02): This pointer was allocated w/ "wmvalloc"
    // above, but released with "delete" right here.
    wmvfree( pWMVDec );
    // delete pWMVDec;

    if (tWMVStatus == WMV_Succeeded) {
        phWMVDecoder = NULL_WMV;
    }

    return tWMVStatus;
}


tWMVDecodeStatus WMVideoDecDecodeSequenceHeader (HWMVDecoder hWMVDecoder)
{

    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec;
//    I32_WMV iTmp;
    U32_WMV uiBufferLengthFromCaller;
    U8_WMV* pucBufferBits;
    Bool_WMV bNotEndOfFrame = TRUE;

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
	
	WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame){
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL){
            return WMV_BrokenFrame;
        }
    }
    BS_reset(pWMVDec->m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame);
    if (pWMVDec->m_cvCodecVersion == WMV2)
        decodeVOLHead (pWMVDec);
    else // MP4S
        decodeVOLHeadMP4S (pWMVDec);

/*
    assert (uiBufferLengthFromCaller == 4); // expect to get 4 bytes since this is small.
    StreamParserAttach (pucBufferBits, uiBufferLengthFromCaller);
    iTmp = StreamParse (pWMVDec->m_pStrmParse, 5);
    if (pWMVDec->m_fltFrameRate == 0.0F)
        pWMVDec->m_fltFrameRate = (Float_WMV) iTmp;
    iTmp = StreamParse (pWMVDec->m_pStrmParse, 11);
    if (pWMVDec->m_fltBitRate == 0.0F)
        pWMVDec->m_fltBitRate = (Float_WMV) iTmp;

    pWMVDec->m_bMixedPel = StreamParse (pWMVDec->m_pStrmParse, 1);
    pWMVDec->m_bLoopFilter = StreamParse (pWMVDec->m_pStrmParse, 1);
    pWMVDec->m_bXformSwitch = StreamParse (pWMVDec->m_pStrmParse, 1);
    pWMVDec->m_bXintra8Switch = StreamParse (pWMVDec->m_pStrmParse, 1);
    pWMVDec->m_bFrmHybridMVOn = StreamParse (pWMVDec->m_pStrmParse, 1);

    // DCTTABLE S/W at MB level for WMV2.
    pWMVDec->m_bDCTTable_MB_ENABLED = StreamParse (pWMVDec->m_pStrmParse, 1);
    pWMVDec->m_iSliceCode = StreamParse (pWMVDec->m_pStrmParse, NUMBITS_SLICE_SIZE_WMV2);
*/
    return tWMVStatus;
}

tWMVDecodeStatus WMVideoDecReset (HWMVDecoder hWMVDecoder)
{
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    return WMV_Succeeded;
}

#ifdef _COUNT_CYCLES_
__int64 DecodeAndLockTmr = 0;
__int64 HrTmrGet (void)
{
  __asm rdtsc;
}
#endif

tWMVDecodeStatus WMVideoDecDecodeData (HWMVDecoder hWMVDecoder, U16_WMV* puiNumDecodedFrames)
{
#ifdef _COUNT_CYCLES_
    DecodeAndLockTmr -= HrTmrGet ();
#endif

    *puiNumDecodedFrames = 0;
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 
    U32_WMV uiBufferLengthFromCaller;
    U8_WMV* pucBufferBits;
	Bool_WMV bNotEndOfFrame = TRUE;

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);

    FUNCTION_PROFILE_DECL_START(fp,WMVDECODEDATA_PROFILE);

    WMVDecCBGetData (pWMVDec->m_uiUserData, 0, (U8_WMV**) &pucBufferBits, 4, &uiBufferLengthFromCaller, &bNotEndOfFrame);
    if (bNotEndOfFrame){
        if (uiBufferLengthFromCaller == 0 || pucBufferBits == NULL){
            return WMV_BrokenFrame;
        }
    }
    BS_reset(pWMVDec -> m_pInputBitstream, (U8_WMV*)pucBufferBits, uiBufferLengthFromCaller, bNotEndOfFrame);

    tWMVStatus = WMVideoDecDecodeFrameHead (pWMVDec);
    if (tWMVStatus != WMV_Succeeded){
        FUNCTION_PROFILE_STOP(&fp);
        return tWMVStatus;
    }
    pWMVDec->m_bFirstEscCodeInFrame = TRUE_WMV;
    // Can't do anything unless we've seen an I-Picture go by
    if (pWMVDec->m_tFrmType == IVOP)
        pWMVDec->m_fDecodedI = TRUE_WMV;
    else if (!pWMVDec->m_fDecodedI) {
        FUNCTION_PROFILE_STOP(&fp);
        return WMV_NoKeyFrameDecoded;
    }
g_iQP = pWMVDec->m_iStepSize;

    if (pWMVDec->m_tFrmType == IVOP) {
g_iIFrame = 1;

#ifdef _SUPPORT_POST_FILTERS_ 
#ifdef _WMV_TARGET_X86_
        WMVDecideDeblokLevel (pWMVDec, pWMVDec->m_iWidthY, pWMVDec->m_iHeightY);
#else
        if (pWMVDec->m_iPostFilterLevel < 0 || pWMVDec->m_iPostFilterLevel >= 5)
            pWMVDec->m_iPostFilterLevel = 0;
        pWMVDec->m_iPostProcessMode = pWMVDec->m_iPostFilterLevel;
#endif
        AssignDeblokRoutines (pWMVDec);
#endif

        if (pWMVDec->m_bXintra8)
            tWMVStatus = WMVideoDecDecodeI_X8 (pWMVDec);
        else
            tWMVStatus = pWMVDec->m_pDecodeI (pWMVDec); //WMVideoDecDecodeI (pWMVDec);
        if (tWMVStatus != WMV_Succeeded) {
            FUNCTION_PROFILE_STOP(&fp);
            return tWMVStatus;
        }
        if (pWMVDec->m_cvCodecVersion != MP4S) { //m_uiFOURCCCompressed 
            if (pWMVDec->m_cvCodecVersion == MP42 || pWMVDec->m_cvCodecVersion == MP43) 
                tWMVStatus = WMVideoDecDecodeClipInfo (pWMVDec);
            if (tWMVStatus != WMV_Succeeded) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }
        }
    }
    else { // P-VOP
g_iIFrame = 0;
        WMVideoDecAssignMotionCompRoutines (pWMVDec);
        WMVideoDecSwapCurrAndRef (pWMVDec);
        if(EXPANDY_REFVOP!=0)
        {
            (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
#ifndef OUTPUT_GRAY4
            (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE_WMV,TRUE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#endif
        }

        tWMVStatus = pWMVDec->m_pDecodeP (pWMVDec); //WMVideoDecDecodeP (pWMVDec);
        if (tWMVStatus != WMV_Succeeded) {
            FUNCTION_PROFILE_STOP(&fp);
            return tWMVStatus;
        }
    }
    pWMVDec->m_uiNumFramesLeftForOutput = 1; // 1 frame ready to output after decoding.
    *puiNumDecodedFrames = 1;
#ifdef _DUMPFRAME_
        pWMVDec->m_pfrmCurrQ->dump_minuspadding(fpDump);
#endif

#ifdef _COUNT_CYCLES_
    DecodeAndLockTmr += HrTmrGet ();
#endif

    FUNCTION_PROFILE_STOP(&fp);

#ifdef _WMV_TARGET_X86_
    if (g_bSupportMMX_WMV)
        _asm emms; //Make sure floating point calc works for building the table
#endif
    return WMV_Succeeded;
}


tWMVDecodeStatus WMVideoDecGetOutput (
    HWMVDecoder     hWMVDecoder,
    U32_WMV         uiFOURCCOutput,
    U16_WMV         uiBitsPerPixelOutput,
    U8_WMV*         pucDecodedOutput
){
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVDecInternalMember *pWMVDec; 

    if (hWMVDecoder == NULL_WMV)
        return WMV_InValidArguments;
    pWMVDec = (tWMVDecInternalMember*) (hWMVDecoder);
	
	if (pucDecodedOutput == NULL )
		pWMVDec->m_uiNumFramesLeftForOutput =0;

    if (pWMVDec->m_uiNumFramesLeftForOutput == 0)
        return WMV_NoMoreOutput;
    if (pucDecodedOutput == NULL_WMV) {
        pWMVDec->m_bRefreshBMP = TRUE_WMV; // repaint next frame if this frame is NULL
        return WMV_Succeeded;
    }

    FUNCTION_PROFILE_DECL_START(fp,WMVDECGETOUTPUT);

    // initializing BMP info
    if (1){//!pWMVDec->m_bBMPInitialized) {
        pWMVDec->m_uiFOURCCOutput = uiFOURCCOutput;
        pWMVDec->m_uiBitsPerPixelOutput = uiBitsPerPixelOutput;
        WMVideoDecInitBMPInfo (pWMVDec);
        pWMVDec->m_bBMPInitialized = TRUE_WMV;
    }

#ifdef COMBINE_LOOPFILTER_RENDERING
    if(pWMVDec->m_bLoopFilter==TRUE_WMV)
    {
        tWMVStatus=combine_loopfilter_rendering(pWMVDec, pucDecodedOutput);
    }
    else
        tWMVStatus = WMVideoDecColorConvert (pWMVDec, pucDecodedOutput);

    if (tWMVStatus != WMV_Succeeded) {
        FUNCTION_PROFILE_STOP(&fp);
        return tWMVStatus;
    }
#else
    tWMVStatus = WMVideoDecColorConvert (pWMVDec, pucDecodedOutput);
    if (tWMVStatus != WMV_Succeeded) {
        FUNCTION_PROFILE_STOP(&fp);
        return tWMVStatus;
    }
#endif
    pWMVDec->m_uiNumFramesLeftForOutput--;

    FUNCTION_PROFILE_STOP(&fp);
#ifdef _WMV_TARGET_X86_
    if (g_bSupportMMX_WMV)
        _asm emms; //Make sure floating point calc works for building the table
#endif
    return WMV_Succeeded;
}

// ===========================================================================
// private functions, inits
// ===========================================================================

tWMVDecodeStatus WMVInternalMemberInit (
    tWMVDecInternalMember *pWMVDec,
    U32_WMV uiUserData,
    U32_WMV uiFOURCCCompressed,
    Float_WMV fltFrameRate,
    Float_WMV fltBitRate,
    I32_WMV iWidthSource,
    I32_WMV iHeightSource,
    I32_WMV iPostFilterLevel
) {
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;

    pWMVDec->m_uiUserData = uiUserData;
    tWMVStatus = VodecConstruct (pWMVDec, uiFOURCCCompressed, (I32_WMV) fltFrameRate, iWidthSource, iHeightSource, pWMVDec -> m_pInputBitstream );
    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus;
    //g_InitDstBMP_WMV ();
#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_GRAY4) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB12) || defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)
    tWMVStatus = g_InitDstTables_WMV (pWMVDec);
    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus;
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
    g_InitDstDitherMap_WMV ();
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)

    pWMVDec->m_uiNumFramesLeftForOutput = 0;
    pWMVDec->m_uiFOURCCCompressed = uiFOURCCCompressed;
    pWMVDec->m_fltFrameRate = fltFrameRate;
    pWMVDec->m_fltBitRate = fltBitRate;
    pWMVDec->m_iPostFilterLevel = iPostFilterLevel;

    // size info
    pWMVDec->m_iWidthSource = pWMVDec->m_iFrmWidthSrc;
    pWMVDec->m_iHeightSource = pWMVDec->m_iFrmHeightSrc;

    pWMVDec->m_iWidthInternal = pWMVDec->m_iWidthPrevY;
    pWMVDec->m_iWidthInternalUV = pWMVDec->m_iWidthPrevUV; 
    pWMVDec->m_iHeightInternal = pWMVDec->m_iHeightPrevY;
    pWMVDec->m_iHeightInternalUV = pWMVDec->m_iHeightPrevUV;
    pWMVDec->m_iWidthInternalTimesMB = pWMVDec->m_iWidthInternal * MB_SIZE;
    pWMVDec->m_iWidthInternalUVTimesBlk = pWMVDec->m_iWidthInternalUV * BLOCK_SIZE;
    pWMVDec->m_bSizeMBAligned = (pWMVDec->m_iWidthSource == pWMVDec->m_iWidthInternal && pWMVDec->m_iHeightSource == pWMVDec->m_iHeightInternal);

    pWMVDec->m_uiNumMBX = pWMVDec->m_iWidthY >> 4; // divided by 16, MB size
    pWMVDec->m_uiNumMBY = pWMVDec->m_iHeightY >> 4;
    pWMVDec->m_uiNumMB = pWMVDec->m_uiNumMBX * pWMVDec->m_uiNumMBY;

    // tables
    pWMVDec->m_rgiClapTabDec = &(g_rgiClapTabDecWMV [384]); // 384 + 256 + 384 = 1024 (256 is the valid region).

#ifdef DYNAMIC_EDGEPAD
    edgePadInit(pWMVDec, &pWMVDec->m_tEdgePad );
#endif

    // picture/frame init
//    pWMVDec->m_pfrCurr = (tYUV420Frame_WMV*) wmvalloc (sizeof (tYUV420Frame_WMV));
//    if (pWMVDec->m_pfrCurr == NULL_WMV)
//        return WMV_BadMemory;
//    memset (pWMVDec->m_pfrCurr, 0, sizeof (tYUV420Frame_WMV));
//    pWMVDec->m_pfrCurr->m_pucYPlane = (U8_WMV*) wmvalloc (pWMVDec->m_iWidthInternal * pWMVDec->m_iHeightInternal);
//    if (pWMVDec->m_pfrCurr->m_pucYPlane == NULL_WMV)
//        return WMV_BadMemory;
//    pWMVDec->m_pfrCurr->m_pucUPlane = (U8_WMV*) wmvalloc (pWMVDec->m_iWidthInternalUV * pWMVDec->m_iHeightInternalUV);
//    if (pWMVDec->m_pfrCurr->m_pucUPlane == NULL_WMV)
//        return WMV_BadMemory;
//    pWMVDec->m_pfrCurr->m_pucVPlane = (U8_WMV*) wmvalloc (pWMVDec->m_iWidthInternalUV * pWMVDec->m_iHeightInternalUV);
//    if (pWMVDec->m_pfrCurr->m_pucVPlane == NULL_WMV)
//        return WMV_BadMemory;
//
//    pWMVDec->m_pfrPrev = (tYUV420Frame_WMV*) wmvalloc (sizeof (tYUV420Frame_WMV));
//    if (pWMVDec->m_pfrPrev == NULL_WMV)
//        return WMV_BadMemory;
//    memset (pWMVDec->m_pfrPrev, 0, sizeof (tYUV420Frame_WMV));
//    pWMVDec->m_pfrPrev->m_pucYPlane = (U8_WMV*) wmvalloc (pWMVDec->m_iWidthInternal * pWMVDec->m_iHeightInternal);
//    if (pWMVDec->m_pfrPrev->m_pucYPlane == NULL_WMV)
//        return WMV_BadMemory;
//    pWMVDec->m_pfrPrev->m_pucUPlane = (U8_WMV*) wmvalloc (pWMVDec->m_iWidthInternalUV * pWMVDec->m_iHeightInternalUV);
//    if (pWMVDec->m_pfrPrev->m_pucUPlane == NULL_WMV)
//        return WMV_BadMemory;
//    pWMVDec->m_pfrPrev->m_pucUPlane = (U8_WMV*) wmvalloc (pWMVDec->m_iWidthInternalUV * pWMVDec->m_iHeightInternalUV);
//    if (pWMVDec->m_pfrPrev->m_pucUPlane == NULL_WMV)
//        return WMV_BadMemory;
/*
    // Huffmane decoder init
    pWMVDec->m_pHuffDec = (tHuffmanDecoder_WMV*) wmvalloc (sizeof (tHuffmanDecoder_WMV));
    if (pWMVDec->m_pHuffDec == NULL_WMV)
        return WMV_BadMemory;
    memset (pWMVDec->m_pHuffDec, 0, sizeof (tHuffmanDecoder_WMV));
    tWMVStatus = HuffmanDecodeInit (pWMVDec->m_pHuffDec);
    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus; 

    // Stream parser init
    pWMVDec->m_pStrmParse = (tStreamParser_WMV*) wmvalloc (sizeof (tStreamParser_WMV));
    if (pWMVDec->m_pStrmParse == NULL_WMV)
        return WMV_BadMemory;
    memset (pWMVDec->m_pStrmParse, 0, sizeof (tStreamParser_WMV));
    tWMVStatus = StreamParserInit (pWMVDec->m_pStrmParse);
    if (tWMVStatus != WMV_Succeeded)
        return tWMVStatus; 
*/
    // picture encoding mode
//    pWMVDec->m_bRndCtrlOn = TRUE_WMV;

    // MB data
    
    return tWMVStatus;
}

tWMVDecodeStatus WMVInternalMemberDestroy (tWMVDecInternalMember *pWMVDec)
{
#if defined (_6BIT_COLORCONV_OPT_) || defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB12) || defined (OUTPUT_GRAY4)
    g_EndDstTables_WMV (pWMVDec);
#endif
#ifdef DYNAMIC_EDGEPAD
    edgePadDestroy(&pWMVDec->m_tEdgePad );
#endif
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    tWMVStatus = VodecDestruct(pWMVDec);
    return tWMVStatus;
}

