/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    EntropyDec.cpp

Abstract:

    Implementation of entropy decoding functions.

Author:

    Wei-ge Chen (wchen) 14-July-1998

Revision History:


*************************************************************************/

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#include "..\decoder\msaudiodec.h"
#include "..\decoder\huffdec.h"
#include "AutoProfile.h"  
#include "..\..\..\dsound\dsndver.h"

#ifdef TRANSCODER
#include "coefStream.h"
#endif  // TRANSCODER


//*****************************************************************************************
//
// prvDecodeFrameHeader
//
//*****************************************************************************************

//#define DEBUG_DECODE_TRACE
#undef DEBUG_DECODE_TRACE
#if defined (DEBUG_DECODE_TRACE)
extern unsigned int g_ulOutputSamples;
#define DEBUG_TRACE_QUANT(a, b, c)                                              \
                printf("%d %d %d %u\n", a, b, c, g_ulOutputSamples);                                  \
                fflush (stdout);
#pragma COMPILER_MESSAGE(__FILE__ "(43) : Warning - m_iQuantStepSize Debug Code Enabled.")
#else
#define DEBUG_TRACE_QUANT(a, b, c)
#endif

#if defined (_DEBUG)
#define COUNT_BITS(a,b) a += b
#else
#define COUNT_BITS(a,b)
#endif

WMARESULT prvDecodeFrameHeader (CAudioObjectDecoder* paudec, Int* piBitCnt)
{      
    Bool fSkipAll;
    Bool* fUpdateMask;
    I16  *piChannel = &paudec->m_iChannel;
    Int iMaskQPrev;
    Int iSizePrev = 0, iSizeCurr = 0, iSizeNext = 0;
    PerChannelInfo* ppcinfo;
	CAudioObject* pau = paudec->pau;

    WMARESULT   wmaResult = WMA_OK;
    U32         iResult;

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,DECODE_FRAME_HEADER_PROFILE);
#endif

    switch (paudec->m_hdrdecsts)
    {
        case HDR_SIZE:
            if (pau->m_fAllowSubFrame) 
            {
                //peek enough bits for this "case" if not enough will be on hold
                Int cBitsNeed = LOG2 ((U32) LOG2 ((U32)pau->m_iMaxSubFrameDiv)) + 1;
                TRACEWMA_EXIT(wmaResult, ibstrmLookForBits (&paudec->m_ibstrm, 12));
                if (pau->m_fHeaderReset) 
                {
                    TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, cBitsNeed, &iResult));
                    COUNT_BITS(*piBitCnt, cBitsNeed);
                    iSizePrev = pau->m_cFrameSampleHalf / (1 << iResult);

                    //current
                    TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, cBitsNeed, &iResult));
                    COUNT_BITS(*piBitCnt, cBitsNeed);
                    iSizeCurr = pau->m_cFrameSampleHalf / (1 << iResult);

                    assert (iSizePrev != 0 && iSizeCurr != 0);
                    pau->m_fHeaderReset = WMAB_FALSE;
                    if (iSizePrev  < pau->m_cMinSubFrameSampleHalf || iSizePrev > pau->m_cFrameSampleHalf ||
                        iSizeCurr  < pau->m_cMinSubFrameSampleHalf || iSizeCurr > pau->m_cFrameSampleHalf)
                    {
                        assert (WMAB_FALSE);
                        return TraceResult(WMA_E_BROKEN_FRAME);
                    }
                }

                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, cBitsNeed, &iResult));
                COUNT_BITS(*piBitCnt, cBitsNeed);
                iSizeNext = pau->m_cFrameSampleHalf / (1 << iResult);
                if (iSizeNext  < pau->m_cMinSubFrameSampleHalf || iSizeNext > pau->m_cFrameSampleHalf)
                {
                    assert (WMAB_FALSE);
                    return TraceResult(WMA_E_BROKEN_FRAME);
                }
            }
            ASSERTWMA_EXIT(wmaResult, prvUpdateSubFrameConfig(paudec, iSizeNext, iSizePrev, iSizeCurr));
            ASSERTWMA_EXIT(wmaResult, auAdaptToSubFrameConfig (pau));

#ifdef ENABLE_EQUALIZER
            prvAdaptEqToSubFrame(paudec);
#endif    //ENABLE_EQUALIZER

            if (pau->m_cChannel == 1)    {
                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, &iResult));
                pau->m_rgpcinfo [0].m_iPower = iResult;

                COUNT_BITS(*piBitCnt, 1);
                fSkipAll = (pau->m_rgpcinfo [0].m_iPower == 0);

#ifdef TRANSCODER
                if (TT_SIMPLE == pau->m_ttTranscodeType)
                {
                   TRACEWMA_RET(wmaResult, coefstrmRecordCoefs(pau->m_rgpcinfo[0].
                       m_pcstrmPower, &iResult, 1));
                   TRACEWMA_RET(wmaResult, coefstrmRecordDone(pau->m_rgpcinfo[0].
                       m_pcstrmPower));
                }
#endif  // TRANSCODER

            }
	        else if (pau->m_cChannel == 2)	{
                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, &iResult));
		        pau->m_stereoMode = (StereoMode) iResult;

                COUNT_BITS(*piBitCnt, 1);
                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, &iResult));
		        pau->m_rgpcinfo [0].m_iPower = iResult;

                COUNT_BITS(*piBitCnt, 1);
                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, &iResult));
		        pau->m_rgpcinfo [1].m_iPower = iResult;

                COUNT_BITS(*piBitCnt, 1);
                fSkipAll = (pau->m_rgpcinfo [0].m_iPower == 0) & (pau->m_rgpcinfo [1].m_iPower == 0);
                prvSetDetTable  (paudec,  pau->m_rgpcinfo + 1);

#ifdef TRANSCODER
                if (TT_SIMPLE == pau->m_ttTranscodeType)
                {
                    PerChannelInfo *ppcinfo0 = &(pau->m_rgpcinfo[0]);
                    PerChannelInfo *ppcinfo1 = &(pau->m_rgpcinfo[1]);

                    TRACEWMA_RET(wmaResult, coefstrmRecordCoefs(pau->m_pcstrmStereoMode,
                        &pau->m_stereoMode, 1));

                    TRACEWMA_RET(wmaResult, coefstrmRecordCoefs(ppcinfo0->m_pcstrmPower,
                        &ppcinfo0->m_iPower, 1));

                    TRACEWMA_RET(wmaResult, coefstrmRecordCoefs(ppcinfo1->m_pcstrmPower,
                        &ppcinfo1->m_iPower, 1));

                    TRACEWMA_RET(wmaResult, coefstrmRecordDone(pau->m_pcstrmStereoMode));
                    TRACEWMA_RET(wmaResult, coefstrmRecordDone(ppcinfo0->m_pcstrmPower));
                    TRACEWMA_RET(wmaResult, coefstrmRecordDone(ppcinfo1->m_pcstrmPower));
                }
#endif  // TRANSCODER

            }
            pau->m_iQuantStepSize = MIN_QUANT; // shift it back by the minimum for decode or in case of exit
            if (fSkipAll) {
#               if defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)
                    // no bits will be read
                    if (pau->m_iWeightingMode == LPC_MODE) 
                        TRACEWMA_EXIT(wmaResult, prvDecodeFrameHeaderLpc(paudec, (Int *)piBitCnt));
#               endif  // defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)
                paudec->m_hdrdecsts = HDR_DONE;

#ifdef TRANSCODER
                if (TT_SIMPLE == pau->m_ttTranscodeType)
                {
                    // *** TODO: Can probably avoid saving explicitly and implicitly set
                    // after noticing fSkipAll condition on encoder side.
                    TRACEWMA_EXIT(wmaResult, coefstrmRecordCoefs(pau->m_pcstrmQSteps,
                        &pau->m_iQuantStepSize, 1));
                    TRACEWMA_EXIT(wmaResult, coefstrmRecordDone(pau->m_pcstrmQSteps));
                }
#endif  // TRANSCODER

                goto exit;
            }
            paudec->m_hdrdecsts = HDR_QUANT;

        case HDR_QUANT:
            TRACEWMA_EXIT(wmaResult, ibstrmLookForBits (&paudec->m_ibstrm, 3 * NUM_BITS_QUANTIZER));
            while(1) {
                Int iStepSize;

                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm,
                    NUM_BITS_QUANTIZER, &iResult));
                iStepSize = iResult;

                COUNT_BITS(*piBitCnt, NUM_BITS_QUANTIZER);
                assert(iStepSize <= MSA_MAX_QUANT_ESC);
                DEBUG_TRACE_QUANT(pau->m_iFrameNumber, pau->m_iCurrSubFrame, iStepSize);
                if(iStepSize != MSA_MAX_QUANT_ESC) {
                    // the last one...
                    pau->m_iQuantStepSize += iStepSize;
			        MONITOR_RANGE(gMR_iQuantStepSize,pau->m_iQuantStepSize);
                    break;
                }
                // escape code, so on to the next level...
                pau->m_iQuantStepSize += MSA_MAX_QUANT_ESC;
            }
            auUpdateMaxEsc(pau, pau->m_iQuantStepSize);

#ifdef TRANSCODER
            if (TT_SIMPLE == pau->m_ttTranscodeType)
            {
                TRACEWMA_EXIT(wmaResult, coefstrmRecordCoefs(pau->m_pcstrmQSteps,
                    &pau->m_iQuantStepSize, 1));
                TRACEWMA_EXIT(wmaResult, coefstrmRecordDone(pau->m_pcstrmQSteps));
            }
#endif  // TRANSCODER
            
            *piChannel  = 0;
#ifdef ENABLE_ALL_ENCOPT
            paudec->m_iBand     = (I16) pau->m_iFirstNoiseBand;;
#endif // ENABLE_ALL_ENCOPT
            paudec->m_hdrdecsts = HDR_NOISE1;

        case HDR_NOISE1 :
        case HDR_NOISE2 :
#           ifdef ENABLE_ALL_ENCOPT
            if (pau->m_fNoiseSub == WMAB_TRUE)
            {
                TRACEWMA_EXIT(wmaResult, prvDecodeFrameHeaderNoiseSub(paudec, (Int *)piBitCnt));
            }
#           endif //ENABLE_ALL_ENCOPT
            paudec->m_hdrdecsts = HDR_MSKUPD;

        case HDR_MSKUPD :

            TRACEWMA_EXIT(wmaResult, ibstrmLookForBits (&paudec->m_ibstrm, 1));
            //Setup the pointer to the quantized coefficents. This must be done after 
            //prvDecodeFrameHeaderNoiseSub since it changes the value of m_cSubbandActual
            ppcinfo = pau->m_rgpcinfo;
            ppcinfo->m_rgiCoefQ       = ((I16*) (pau->m_rgiCoefReconOrig + DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf)))  
                                      - DOUBLE(pau->m_fPad2XTransform,ppcinfo->m_cSubbandActual);
            memset (ppcinfo->m_rgiCoefQ, 0,  sizeof (I16) * DOUBLE(pau->m_fPad2XTransform,ppcinfo->m_cSubbandActual));
            if (pau->m_cChannel == 2)   {
                ppcinfo = pau->m_rgpcinfo + 1;
                ppcinfo->m_rgiCoefQ       = ((I16*) (pau->m_rgiCoefReconOrig + DOUBLE(pau->m_fPad2XTransform, 2 * pau->m_cFrameSampleHalf)))
                                          - DOUBLE(pau->m_fPad2XTransform,ppcinfo->m_cSubbandActual);
                memset (ppcinfo->m_rgiCoefQ, 0,  sizeof (I16) * DOUBLE(pau->m_fPad2XTransform,ppcinfo->m_cSubbandActual));
            }

            fUpdateMask = &(pau->m_subfrmconfigCurr.m_rgfMaskUpdate [pau->m_iCurrSubFrame]);
            *fUpdateMask = WMAB_TRUE;
            if (pau->m_fAllowSubFrame && pau->m_subfrmconfigCurr.m_cSubFrame > 1) {
                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, &iResult));
                *fUpdateMask = iResult;
                (*piBitCnt)++;
            }
            *piChannel = 0;
            paudec->m_iBand = 0;
            paudec->m_hdrdecsts = HDR_BARK;

        case HDR_BARK :
            fUpdateMask = &(pau->m_subfrmconfigCurr.m_rgfMaskUpdate [pau->m_iCurrSubFrame]);
            if (pau->m_iWeightingMode == BARK_MODE) 
            {
                for (; *piChannel < pau->m_cChannel; (*piChannel)++)
                {
                    PerChannelInfo *ppcinfo = pau->m_rgpcinfo + *piChannel;
                    if (ppcinfo->m_iPower != 0) 
                    {
                        Int* rgiMaskQ = ppcinfo->m_rgiMaskQ;
                        if (*fUpdateMask == WMAB_TRUE)    
                        {
                            if (pau->m_iVersion == 1) {
                                TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm,
                                    NBITS_FIRST_MASKQUANT, &iResult));
	                            rgiMaskQ [0] = iResult + MIN_MASKQ_IN_DB_V1;

#ifdef TRANSCODER
                                if (TT_SIMPLE == pau->m_ttTranscodeType)
                                {
                                    TRACEWMA_EXIT(wmaResult,
                                        coefstrmRecordCoefs(ppcinfo->m_pcstrmMaskQ,
                                        &rgiMaskQ[0], 1));
                                }
#endif  // TRANSCODER

                                *piBitCnt += NBITS_FIRST_MASKQUANT;
                                paudec->m_iBand++;
                            }

                            for (; paudec->m_iBand < pau->m_cValidBarkBand; paudec->m_iBand++)	
                            {
				                Int iDiff;                        
                                TRACEWMA_EXIT(wmaResult, huffDecGet (g_rgiHuffDecTblMsk,
                                    &paudec->m_ibstrm, &iResult, (unsigned long *)&iDiff, (unsigned long *)0));
                                COUNT_BITS(*piBitCnt, iResult);
                                TRACEWMA_EXIT(wmaResult, ibstrmFlushBits(&paudec->m_ibstrm, iResult));   //flush bits used by huffDecGet
                                iDiff -= MAX_MASKQUANT_DIFF;
                                assert (iDiff >= -MAX_MASKQUANT_DIFF);
                                iMaskQPrev = (paudec->m_iBand == 0) ? 
                                              FIRST_V2_MASKQUANT : rgiMaskQ [paudec->m_iBand - 1];
				                rgiMaskQ [paudec->m_iBand] = iDiff + iMaskQPrev;
						        MONITOR_RANGE(gMR_rgiMaskQ, rgiMaskQ[paudec->m_iBand]);

#ifdef TRANSCODER
                                if (TT_SIMPLE == pau->m_ttTranscodeType)
                                {
                                    TRACEWMA_EXIT(wmaResult,
                                        coefstrmRecordCoefs(ppcinfo->m_pcstrmMaskQ,
                                        &rgiMaskQ[paudec->m_iBand], 1));
                                }
#endif  // TRANSCODER

                            }
                            pau->m_iSubFrameSizeWithUpdate = pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];
                            paudec->m_iBand = 0;

#ifdef TRANSCODER
                            if (TT_SIMPLE == pau->m_ttTranscodeType)
                                TRACEWMA_EXIT(wmaResult, coefstrmRecordDone(ppcinfo->m_pcstrmMaskQ));
#endif  // TRANSCODER
                        }

                        //for IntFloat this gets done inside ReconWeightFactor()
                        //Scan For Max Weight
                        if (*fUpdateMask)
                        {
					        Int iMax = rgiMaskQ[0];
                            I16 iBand;
                            for (iBand = 1; iBand < pau->m_cValidBarkBand; iBand++)
                            {
                                if (rgiMaskQ[iBand] > iMax)
							        iMax = rgiMaskQ[iBand];
                            }
					        ppcinfo->m_iMaxMaskQ = iMax;
                        }
                    }
                    else if (*fUpdateMask == WMAB_TRUE)    
                    {	//else artifically set to constants since nothing got sent; see comments in msaudioenc.c
				        //0 db = 1.0 for weightfactor
				        memset( ppcinfo->m_rgiMaskQ, 0, pau->m_cValidBarkBand*sizeof(Int) );
                        ppcinfo->m_iMaxMaskQ = 0;
			        }
                }
            }
#if defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)
            else 
            {
                TRACEWMA_EXIT(wmaResult, prvDecodeFrameHeaderLpc(paudec, (Int *)piBitCnt));
		        if (pau->m_fNoiseSub == WMAB_TRUE)	
			        prvGetBandWeightLowRate (paudec);
            }
#else   // defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)
            else {
                assert(WMAB_FALSE);
            }
#endif  // defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)
    }

    paudec->m_hdrdecsts = HDR_DONE;

exit:

#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif

    return wmaResult;
}


//*****************************************************************************************
//
// prvDecodeCoefficientMono
//
//*****************************************************************************************
WMARESULT prvDecodeCoefficientMono	(CAudioObjectDecoder* paudec, 
                                     PerChannelInfo* ppcinfo, 
                                     Int* piBitCnt)
{
    WMARESULT   wmaResult = WMA_OK;

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,DECODE_COEFFICENT_MONO_PROFILE);
#endif

    if (ppcinfo->m_iPower == 0)
        goto exit;

    TRACEWMA_EXIT(wmaResult, prvDecodeRunLevel(paudec, ppcinfo, 0, ppcinfo->m_cSubbandActual, piBitCnt));

exit:
#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return wmaResult;
}


//*****************************************************************************************
//
// prvDecodeCoefficientStereo
//
//*****************************************************************************************
WMARESULT prvDecodeCoefficientStereo (CAudioObjectDecoder* paudec, 
                                    PerChannelInfo* ppcinfo, Int* piBitCnt)
{
    WMARESULT   wmaResult = WMA_OK;
    CAudioObject* pau = paudec->pau;

#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,DECODE_COEFFICENT_STEREO_PROFILE);
#endif
	for (; paudec->m_iChannel < pau->m_cChannel; paudec->m_iChannel++)	
    {
        TRACEWMA_EXIT(wmaResult, prvDecodeCoefficientMono(paudec, ppcinfo + paudec->m_iChannel, piBitCnt));

        if(pau->m_iVersion == 1) 
        {
            *piBitCnt += ibstrmBitsLeft (&paudec->m_ibstrm) % 8;
		    ibstrmFlush(&paudec->m_ibstrm);         //no need to support pause mode (always has fraction bytes)
        }
        paudec->pau->m_iCurrReconCoef = 0;
	}

exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
	return wmaResult;
}


//*****************************************************************************************
//
// prvGetNextRunDEC
//
//*****************************************************************************************
extern Int g_cBitPut;
WMARESULT prvGetNextRunDEC(Void*			 pobjin, 
					 PerChannelInfo* ppcinfo,
                     Int*			 piBitCnt)
{
	U32 uSign  = 0;		//1 for positive 0 for nonpositive
    I32 iSign  = 0;
	U32 iEntry = 0;		//index to the Vlc table entry
    U32 iResult;
    U16 cBitsTotal;
    WMARESULT     wmaResult = WMA_OK;
    CAudioObjectDecoder* paudec  = (CAudioObjectDecoder*) pobjin;
    CAudioObject*		 pau = paudec->pau;
    CWMAInputBitStream*  pibstrm = &(paudec->m_ibstrm);

    switch (paudec->m_rlsts)
    {
    case VLC:
        TRACEWMA_EXIT(wmaResult, huffDecGet (ppcinfo->m_rgiHuffDecTbl, &paudec->m_ibstrm,
            &iResult, &iEntry, &uSign));
        COUNT_BITS(*piBitCnt, iResult);
        TRACEWMA_EXIT(wmaResult, ibstrmFlushBits(&paudec->m_ibstrm, iResult));   //flush bits used by huffDecGet
	    if (iEntry == 0)
            goto escape;
        if (iEntry == 1)
        {
			/*
		    // [1], end of coefs for this subFrame 
            *pcRunOfZeros = ppcinfo->m_cSubbandActual - iCoef - 1;
            *piCurrLevel = 0;
            wmaResult = WMA_OK;
			*/
            pau->m_iLevel = 0;
			pau->m_iHighCutOffCurr = -1; 
			pau->m_iNextBarkIndex = -1;                                                                    
			pau->m_cRunOfZeros = ppcinfo->m_cSubbandActual - pau->m_iCurrReconCoef - 1;
        }
        else 
        {
			assert (iEntry >= 2);
            COUNT_BITS(*piBitCnt, 1);
            TRACEWMA_EXIT(wmaResult, ibstrmFlushBits(&paudec->m_ibstrm, 1));   //flush the sign bit also
			pau->m_cRunOfZeros = ppcinfo->m_rgiRunEntry [iEntry - 2] & 0x7FFFFFFF;
			assert (pau->m_cRunOfZeros >= 0);
			pau->m_iLevel = ppcinfo->m_rgiLevelEntry [iEntry - 2];
			pau->m_iSign = (I32) (uSign >> 31) - 1;
        }
        break;

    case ESCAPE:
escape:
        paudec->m_rlsts = ESCAPE;
        cBitsTotal = (U16) pau->m_iMaxEscSize + (U16) pau->m_cBitsSubbandMax + 1; //<=25=13+11+1
        assert (cBitsTotal <= 25);
        TRACEWMA_EXIT(wmaResult, ibstrmLookForBits (&paudec->m_ibstrm, cBitsTotal));

        TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, cBitsTotal, &iResult));
        COUNT_BITS(*piBitCnt, cBitsTotal);

        cBitsTotal = (U16) paudec->pau->m_cBitsSubbandMax + 1;
		pau->m_iLevel = (I16) (iResult >> cBitsTotal);
        pau->m_iSign = (I32) (iResult & 0x00000001) - 1;
        //*piCurrLevel = (I16) ((*piCurrLevel ^ iSign) - iSign);

		pau->m_cRunOfZeros = (I16) (iResult & (0xFFFFFFFF >> (32 - cBitsTotal))) >> 1;
		pau->m_cRunOfZeros &= 0x7FFFFFFF;
        paudec->m_rlsts = VLC;
	}

exit:
    return wmaResult;
}

//*****************************************************************************************
//
// prvDecodeRunLevel
//
//*****************************************************************************************
//#define RL_PRINT_SRC
#if defined (RL_PRINT_SRC)
#define DBG_RUNLEVEL(a,b,c,e)     \
                    if(e) {\
	                    printf("%d %d %d\n", a, b, c);\
                        fflush (stdout);\
                    }
#else 
#define DBG_RUNLEVEL(a,b,c,e)
#endif //DBG_RUNLEVEL

WMARESULT prvDecodeRunLevel  (CAudioObjectDecoder* paudec, 
                            PerChannelInfo* ppcinfo,
						    Int iCoefFirst, 
						    Int iCoefLim, 
                            Int* piBitCnt)
{
    I16* rgiCoefQ       = ppcinfo->m_rgiCoefQ;
    const U16* rgiRunEntry    = ppcinfo->m_rgiRunEntry;    
    const U16* rgiLevelEntry  = ppcinfo->m_rgiLevelEntry;    
    CAudioObject* pau = paudec->pau;
	I16* piCoef = &pau->m_iCurrReconCoef;	
#ifdef DEBUG_DECODE_TRACE
    DEBUG_ONLY( Bool fPrint = (g_ulOutputSamples==0x4e00); )
#else
    DEBUG_ONLY( Bool fPrint = (pau->m_iFrameNumber  <= 10 && pau->m_iCurrSubFrame == 0); )
#endif
    WMARESULT   wmaResult = WMA_OK;

    // This function is too frequently called to be profiled
	//FUNCTION_PROFILE(fp);
    //FUNCTION_PROFILE_START(&fp,DECODE_RUN_LEVEL_PROFILE);
    while (*piCoef < iCoefLim) 
    {
        TRACEWMA_EXIT(wmaResult, prvGetNextRunDEC(paudec,ppcinfo,piBitCnt));
        pau->m_iLevel = (I16) ((pau->m_iLevel ^ pau->m_iSign) - pau->m_iSign);
        DBG_RUNLEVEL(g_cBitGet-21,pau->m_cRunOfZeros,pau->m_iLevel,fPrint);
        if ( (*piCoef+ pau->m_cRunOfZeros) >= iCoefLim)  {
            assert (WMAB_FALSE);
            wmaResult = TraceResult(WMA_E_BROKEN_FRAME);
            goto exit;
        }
//        memset (rgiCoefQ + *piCoef, 0, pau->m_cRunOfZeros * sizeof (I16));
    	*piCoef += (I16) pau->m_cRunOfZeros;
		rgiCoefQ [*piCoef] = pau->m_iLevel;
		MONITOR_RANGE(gMR_iCoefQ,rgiCoefQ [*piCoef]);
		MONITOR_COUNT_CONDITIONAL(pau->m_cRunOfZeros>0,gMC_0CoefQ,pau->m_cRunOfZeros);
		(*piCoef)++;
        paudec->m_rlsts = VLC;
	}
    
exit:
    //FUNCTION_PROFILE_STOP(&fp);
	return wmaResult;
}


//*****************************************************************************************
//
// prvSetDetTable
//
//*****************************************************************************************
Void prvSetDetTable  (CAudioObjectDecoder* paudec,  PerChannelInfo* ppcinfo)
{
    CAudioObject* pau = paudec->pau;
/*** less than 0.5%
#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,SET_DET_TABLE_PROFILE);
#endif
*///
    if (pau->m_iEntropyMode == SIXTEENS_OB)   {
        if (pau->m_stereoMode == STEREO_SUMDIFF)  {
            ppcinfo->m_rgiHuffDecTbl = g_rgiHuffDecTbl16ssOb;
            ppcinfo->m_rgiRunEntry = gRun16ssOb;    
            ppcinfo->m_rgiLevelEntry = gLevel16ssOb;    
        }
        else {
            ppcinfo->m_rgiHuffDecTbl = g_rgiHuffDecTbl16smOb; 
            ppcinfo->m_rgiRunEntry = gRun16smOb;    
            ppcinfo->m_rgiLevelEntry = gLevel16smOb;    
        }
    }
#ifdef ENABLE_ALL_ENCOPT
    else if (pau->m_iEntropyMode == FOURTYFOURS_QB)   {
        if (pau->m_stereoMode == STEREO_SUMDIFF)  {
            ppcinfo->m_rgiHuffDecTbl = g_rgiHuffDecTbl44ssQb;
            ppcinfo->m_rgiRunEntry = gRun44ssQb;    
            ppcinfo->m_rgiLevelEntry = gLevel44ssQb;
        }
        else {
            ppcinfo->m_rgiHuffDecTbl = g_rgiHuffDecTbl44smQb; 
            ppcinfo->m_rgiRunEntry = gRun44smQb;    
            ppcinfo->m_rgiLevelEntry = gLevel44smQb;    
        }
    }
    else if (pau->m_iEntropyMode == FOURTYFOURS_OB)   {
        if (pau->m_stereoMode == STEREO_SUMDIFF)  {
            ppcinfo->m_rgiHuffDecTbl = g_rgiHuffDecTbl44ssOb;
            ppcinfo->m_rgiRunEntry = gRun44ssOb;
            ppcinfo->m_rgiLevelEntry = gLevel44ssOb;
        }
        else {
            ppcinfo->m_rgiHuffDecTbl = g_rgiHuffDecTbl44smOb;
            ppcinfo->m_rgiRunEntry = gRun44smOb;
            ppcinfo->m_rgiLevelEntry = gLevel44smOb;
        }
    }
#endif // ENABLE_ALL_ENCOPT
/***
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
*///
}


#if defined(ENABLE_ALL_ENCOPT) && defined(ENABLE_LPC)

// define to break at a particular frame
//#define LPC_WEIGHTS_BREAK_AT_FRAME 27
// define to display overview of weight factor calculations by subframe
//#define SHOW_OVERVIEW_WEIGHT_FACTOR_FIRST 0
//#define SHOW_OVERVIEW_WEIGHT_FACTOR_LAST  60
#if defined(_DEBUG) && ( defined(SHOW_OVERVIEW_WEIGHT_FACTOR_FIRST) || defined(LPC_WEIGHTS_BREAK_AT_FRAME) )
#	pragma COMPILER_MESSAGE(__FILE__ "(644) : Warning - LPC Weight Factor Debug Code Enabled.")
#endif
#if defined(_DEBUG) && defined(LPC_WEIGHTS_BREAK_AT_FRAME)
#   define DEBUG_BREAK_AT_FRAME_LPC_WEIGHTS                 \
    if ( pau->m_iFrameNumber==LPC_WEIGHTS_BREAK_AT_FRAME )  \
        { DEBUG_BREAK(); }
#else   // define(_DEBUG) && defined(SHOW_OVERVIEW_WEIGHT_FACTOR)
#   define DEBUG_BREAK_AT_FRAME_LPC_WEIGHTS
#endif  // define(_DEBUG) && defined(SHOW_OVERVIEW_WEIGHT_FACTOR)
#if defined(_DEBUG) && defined(SHOW_OVERVIEW_WEIGHT_FACTOR_FIRST)
#   if UNDER_CE
#       define SHOW_OWF(iChan,iCase)                                            \
            if (SHOW_OVERVIEW_WEIGHT_FACTOR_FIRST <= pau->m_iFrameNumber        \
                && pau->m_iFrameNumber <= SHOW_OVERVIEW_WEIGHT_FACTOR_LAST )    \
                MyOutputDbgStr(0,"%4d %2d %d %4d %d  %+16.6f %+16.6f %+16.6f %+16.6f\n", \
                    pau->m_iFrameNumber,pau->m_iCurrSubFrame, pau->m_cSubband,  \
                    iChan, iCase,                                               \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[0]),          \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[1]),          \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[2]),          \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[3]) )
#   else // UNDER_CE
#       define SHOW_OWF(iChan,iCase)                                            \
            if (SHOW_OVERVIEW_WEIGHT_FACTOR_FIRST <= pau->m_iFrameNumber        \
                && pau->m_iFrameNumber <= SHOW_OVERVIEW_WEIGHT_FACTOR_LAST )    \
                printf("%4d %2d %d %4d %d  %+16.6f %+16.6f %+16.6f %+16.6f\n",  \
                    pau->m_iFrameNumber,pau->m_iCurrSubFrame, pau->m_cSubband,  \
                    iChan, iCase,                                               \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[0]),          \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[1]),          \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[2]),          \
                    FLOAT_FROM_WEIGHT(ppcinfo->m_rguiWeightFactor[3]) )
#   endif  // UNDER_CE
#else   // define(_DEBUG) && defined(SHOW_OVERVIEW_WEIGHT_FACTOR)
#   define SHOW_OWF(iChan,iCase)
#endif  // define(_DEBUG) && defined(SHOW_OVERVIEW_WEIGHT_FACTOR)

WMARESULT prvDecodeFrameHeaderLpc (CAudioObjectDecoder* paudec, Int* piBitCnt) 
{
    Bool fUpdateMask = paudec->pau->m_subfrmconfigCurr.m_rgfMaskUpdate [paudec->pau->m_iCurrSubFrame];
	CAudioObject* pau = paudec->pau;
    I16* piChannel = &paudec->m_iChannel;
    PerChannelInfo *ppcinfo;
    // U8 rgiLspFreqQ[LPCORDER];  ONHOLD will lose this, use WeightFactor instead
    I16* piFreq = &paudec->m_iBand;  // preserve this across ONHOLD
	Int cBitAlloc;
    WMARESULT   wmaResult = WMA_OK;
    LpType rgLpcCoef [LPCORDER];
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,FRAME_HEADER_LPC_PROFILE);

    DEBUG_BREAK_AT_FRAME_LPC_WEIGHTS;

    for (; *piChannel < pau->m_cChannel; (*piChannel)++)   
	{
		ppcinfo = pau->m_rgpcinfo + *piChannel;
		if (ppcinfo->m_iPower != 0 )   
		{
			if (fUpdateMask == WMAB_TRUE)
			{
                // these 10 bytes are saved in the WeightFactor array in case we go ONHOLD.  
                // WeightFactor will be set by rgiLspFreqQ
                U8* rgiLspFreqQ = (U8*)(INTEGER_OR_INT_FLOAT( ppcinfo->m_rguiWeightFactor, ppcinfo->m_rgfltWeightFactor ));
				for (; *piFreq < LPCORDER; (*piFreq)++)	
				{
					U32         iResult;
					cBitAlloc = (*piFreq == 0 || *piFreq == 8 || *piFreq == 9) ? 3 : 4;
                
					TRACEWMA_EXIT(wmaResult, ibstrmGetBits(&paudec->m_ibstrm,
						          cBitAlloc, &iResult));
					rgiLspFreqQ[*piFreq] = (U8) iResult;

					*piBitCnt += cBitAlloc;
					MONITOR_RANGE(gMR_rgiLspFreqQ, rgiLspFreqQ[*piFreq]);
				}
				auLsp2lpc(pau, rgiLspFreqQ, rgLpcCoef, LPCORDER);
				TRACEWMA_EXIT(wmaResult, prvLpcToSpectrum(pau,
					          rgLpcCoef, ppcinfo));
                SHOW_OWF(*piChannel,0);
			}
			else if (pau->m_iCurrSubFrame > 0) 
			{	
				auResampleWeightFactorLPC (pau, ppcinfo);
                SHOW_OWF(*piChannel,1);
			}
		}
		else if (fUpdateMask == WMAB_TRUE)
		{	//artifically set to constants since nothing got sent for this channel; see comments in msaudioenc.c
			WeightType* pwtWeightFactor = (WeightType*) ppcinfo->m_rguiWeightFactor;
            I16 iFreq;
			ppcinfo->m_wtMaxWeight = WEIGHT_FROM_FLOAT(1.0F);
			for (iFreq = (I16) pau->m_cSubband; iFreq > 0; iFreq-- )
				*pwtWeightFactor++ = WEIGHT_FROM_FLOAT(1.0F);
            SHOW_OWF(*piChannel,2);
		}
        else if (pau->m_iCurrSubFrame > 0) 
		{	// resample even if zero power, these weights might be used in a later subframe with non-zero power
            auResampleWeightFactorLPC (pau, ppcinfo);
            SHOW_OWF(*piChannel,3);
		}
        *piFreq = 0;
	}

exit:
    FUNCTION_PROFILE_STOP(&fp);
    return wmaResult;
}
#endif  // ENABLE_LPC

#if defined (_DEBUG)
#define COUNT_BITS(a,b) a += b
#else
#define COUNT_BITS(a,b)
#endif

#ifdef ENABLE_ALL_ENCOPT
WMARESULT prvDecodeFrameHeaderNoiseSub (CAudioObjectDecoder* paudec, Int* piBitCnt)
{
    U8*	rgbBandNotCoded;
    Int iStart, iEnd;
    Int iCode;
    U32 cBitUsed;
    I16* piChannel = &paudec->m_iChannel;
    I16* piBand = &paudec->m_iBand;
	CAudioObject* pau = paudec->pau;

    WMARESULT   wmaResult = WMA_OK;
    U32         iResult;
	FUNCTION_PROFILE(fp);
	FUNCTION_PROFILE_START(&fp,FRAME_HEADER_NOISE_SUB_PROFILE);

    switch (paudec->m_hdrdecsts)
    {
        case HDR_NOISE1:
            //noise band indicator
            for (; *piChannel < pau->m_cChannel; (*piChannel)++)   {
                PerChannelInfo* ppcinfo = pau->m_rgpcinfo + *piChannel;
                TRACEWMA_EXIT(wmaResult, ibstrmLookForBits (&paudec->m_ibstrm, 
                                pau->m_cValidBarkBand - pau->m_iFirstNoiseBand));                
                if (ppcinfo->m_iPower != 0) {
                    rgbBandNotCoded = ppcinfo->m_rgbBandNotCoded;
                    *rgbBandNotCoded = 0;
                    ppcinfo->m_cSubbandActual = pau->m_iFirstNoiseIndex - pau->m_cLowCutOff;
                    iStart = 0, iEnd = 0;
                    while (WMAB_TRUE) {
                        iStart = max (pau->m_iFirstNoiseIndex, pau->m_rgiBarkIndex [*piBand]);
                        if (iStart >= pau->m_cHighCutOff)
                            break;
                        iEnd   = min (pau->m_cHighCutOff, pau->m_rgiBarkIndex [*piBand + 1]);

                        TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 1, &iResult));
				        rgbBandNotCoded [*piBand] = (U8) iResult;

                        (*piBitCnt)++;
				        if (rgbBandNotCoded [*piBand] != 0)
					        rgbBandNotCoded [0]++;
                        else 
                            ppcinfo->m_cSubbandActual += iEnd - iStart;
                        (*piBand)++;
			        }
			        if (*piBand < pau->m_cValidBarkBand )
				        rgbBandNotCoded [*piBand] = (U8) 0;		// protect from debug verify code looking one band past end  
                }
                else 
                    ppcinfo->m_cSubbandActual = 0;
                assert (ppcinfo->m_cSubbandActual >= 0 &&
                        ppcinfo->m_cSubbandActual <= pau->m_cHighCutOff - pau->m_cLowCutOff);
                *piBand = (I16) pau->m_iFirstNoiseBand;
            }
            paudec->m_hdrdecsts = HDR_NOISE2;
            *piChannel = 0;
            //I guess
            *piBand = 0;

        case HDR_NOISE2:
            //noise power
            for (; *piChannel < pau->m_cChannel; (*piChannel)++)   {
                PerChannelInfo* ppcinfo = pau->m_rgpcinfo + *piChannel;
                if (ppcinfo->m_iPower != 0) {
			        U8*	 rgbBandNotCoded = ppcinfo->m_rgbBandNotCoded;
			        Int* rgiNoisePower   = ppcinfo->m_rgiNoisePower;
			        if (rgbBandNotCoded [0] > 0)	{
                        if (*piBand == 0)
                        {
                            TRACEWMA_EXIT(wmaResult, ibstrmGetBits (&paudec->m_ibstrm, 7, &iResult));
				            rgiNoisePower [0] = iResult - 19;
                            (*piBand)++;
                            *piBitCnt += 7;
                        }

				        for (; *piBand < rgbBandNotCoded [0]; (*piBand)++)	{
                            TRACEWMA_EXIT(wmaResult, huffDecGet (g_rgiHuffDecTblNoisePower,
                                          &paudec->m_ibstrm, &cBitUsed, (unsigned long *)&iCode, (unsigned long *)0));
                            TRACEWMA_EXIT(wmaResult, ibstrmFlushBits(&paudec->m_ibstrm, cBitUsed));     //flush bits used by huffDecGet
                            COUNT_BITS(*piBitCnt, cBitUsed);
					        rgiNoisePower [*piBand] = iCode - 18 + rgiNoisePower [*piBand - 1];

					        MONITOR_RANGE(gMR_rgiNoisePower,rgiNoisePower[*piBand]);
				        }
			        }
                }
                *piBand = 0;
            }
    }

exit:
	FUNCTION_PROFILE_STOP(&fp);
    return wmaResult;
}

//******************************************************************************************
//
//  Helper functions for prvGetBandWeightLowRate
//  These functions can be replaced by processor dependent optimized variants
//

#ifndef FFLT_AVG_SQUARE_WEIGHTS
#   if defined(BUILD_INTEGER)
#       if defined(U64SQUARE32SR)
#           define U64_SQUARE_I32(x) U64SQUARE32SR(x, 0)
#       else
#           define U64_SQUARE_I32(x) (((U64)(x)) * ((U64)(x))) 
#       endif
        INLINE FastFloat ffltAvgSquareWeights(WeightType* pwtWeightFactor, Int cN )
        {
            U64 u64Sum = 0;
            FastFloat fflt;
            Int i = cN;
	        for ( ; i > 0; i--, pwtWeightFactor++) {
                u64Sum += U64_SQUARE_I32(*pwtWeightFactor);
            }
            // convert to a fastfloat and then divide.  This avoids an int64 division which is
            // really slow on the sh 3 and quite slow on other risc/dsp machines
            // cN varies from 1 to 328 so this can lose 8.3 bits of precision.
            // but we are comparing to float implementation which has only 24 bits of mantissa.
            fflt = FASTFLOAT_FROM_U64( u64Sum, (WEIGHTFACTOR_FRACT_BITS<<1) );
            fflt.iFraction /= cN;
            Norm4FastFloatU( &fflt );
            return fflt;
        }
#   else
        INLINE Float ffltAvgSquareWeights(WeightType* pwtWeightFactor, Int cN )
        {
            Float fltSum = 0;
            Int i = cN;
	        for ( ; i > 0; i--, pwtWeightFactor++) {
                fltSum += *pwtWeightFactor * *pwtWeightFactor;
	        }
            return fltSum/cN;
        }
#   endif
#   define FFLT_AVG_SQUARE_WEIGHTS( pwtWeightFactor, cN ) ffltAvgSquareWeights( pwtWeightFactor, cN )
#endif


#ifndef FFLT_SQRT_RATIO
    INLINE FastFloat ffltSqrtRatio( FastFloat fflt1, FastFloat fflt2 )
    {
#       if defined(BUILD_INTEGER)
            U64 u64Ratio;
            FastFloat fflt;
            Int iMSF8;
            U32 uiMSF;
            U32 uiFrac1;
            Int iExp = 0;
            if ( fflt2.iFraction==0 ) {
                assert( fflt2.iFraction != 0 ); // divide by 0
                fflt.iFraction = 0x7FFFFFFF;
                fflt.iFracBits = 0;
                return fflt;
            }
            //// fflt1.iFraction*0.5F/(1<<(fflt1.iFracBits-1))
            //// fflt2.iFraction*0.5F/(1<<(fflt2.iFracBits-1))
            u64Ratio = (((U64)fflt1.iFraction)<<32)/fflt2.iFraction;
            uiMSF = (U32)(u64Ratio>>32);
            if ( uiMSF==0 ) {
                iExp = 32;
                uiMSF = (U32)u64Ratio;
            }
            assert( uiMSF != 0 ) ;
            // normalize the most significant fractional part
            while( (uiMSF & 0xF0000000)==0 ) {
                iExp += 4;
                uiMSF <<= 4;
            }
            while( (uiMSF & 0x80000000)==0 ) {
                iExp++;
                uiMSF <<= 1;
            }
            // discard the most significant one bit (it's presence is built into g_InvQuadRootFraction)
            iExp++;
            // get all 32 bits from source
#           if defined(PLATFORM_OPTIMIZE_MINIMIZE_BRANCHING)
                uiMSF = (U32)((u64Ratio<<iExp)>>32);
#           else
                uiMSF = (iExp>32) ? (U32)(u64Ratio<<(iExp-32)) : (U32)(u64Ratio>>(32-iExp));
#           endif
            // split into top SQRT_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
            iMSF8 = uiMSF>>(32-SQRT_FRACTION_TABLE_LOG2_SIZE);
            uiMSF <<= SQRT_FRACTION_TABLE_LOG2_SIZE;
            // lookup and interpolate
            uiFrac1  = g_SqrtFraction[iMSF8++];   // BP2
            uiFrac1 += MULT_HI_UDWORD( uiMSF,  g_SqrtFraction[iMSF8] - uiFrac1 );
            // adjust by sqrt(1/2) if expoenent is odd
            if ( (iExp+fflt1.iFracBits-fflt2.iFracBits) & 1 ) {
                // multiply by 1/sqrt(2) and adjust fracbits by 1/2
                uiFrac1 = MULT_HI_UDWORD( uiFrac1, UBP0_FROM_FLOAT(0.70710678118654752440084436210485) );
                fflt.iFracBits = ((fflt1.iFracBits-(fflt2.iFracBits+1))>>1)+(iExp-3);
            } else {
                fflt.iFracBits = ((fflt1.iFracBits-fflt2.iFracBits)>>1)+(iExp-3);
            }
            fflt.iFraction = uiFrac1>>1;    // make sure sign is positive
            //// fflt.iFraction*0.5F/(1<<(fflt.iFracBits-1))
			Norm4FastFloatU( &fflt );
#           if defined(_DEBUG) && 0
            {   // old way used float
			    Float flt = (Float)sqrt( FloatFromFastFloat(fflt1) / FloatFromFastFloat(fflt2) );
                if ( fabs( flt - FloatFromFastFloat(fflt) ) > 0.01 ) {
                    DEBUG_BREAK();
                }
            }
#           endif
            return fflt;
#       else
			return (Float)sqrt( fflt1 / fflt2 );
#       endif
    }
#   define FFLT_SQRT_RATIO(fflt1,fflt2) ffltSqrtRatio(fflt1,fflt2)
#endif


// display noise bands used only once
//#define SHOW_NOISE_BANDS
#if defined(_DEBUG) && defined(SHOW_NOISE_BANDS)
    I16 iShown[2049] = { 1, 0 };
#   define SHOW_NOISE_BANDS_ONCE                                    \
        if (iShown[0]==1)                                           \
            memset( iShown, 0, 2049*sizeof(I16) );                  \
        if ((iShown[iEnd-iStart] & paudec->pau->m_cSubband)==0)     \
        {                                                           \
            iShown[iEnd-iStart] |= paudec->pau->m_cSubband;         \
            printf("%4d  %2d  %4d  %4d  %4d  %16.12f\n",            \
                paudec->pau->m_cSubbandAdjusted, iBand,                     \
                iStart, iEnd, iEnd-iStart, 1.0f/(iEnd-iStart) );    \
        }
#else
#   define SHOW_NOISE_BANDS_ONCE
#endif


Void prvGetBandWeightLowRate (CAudioObjectDecoder* paudec)
{
    U8*	rgbBandNotCoded;
    Int iBand;
    Int iStart, iEnd;
    Int iChannel;
	U8 cNoiseBand;
#if 0
    Int i;
    Float fltWeightFactor;
    Float fltInvBandWithd;
#endif
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,GET_BAND_WEIGHTS_PROFILE);

    //calculating band weight
    for (iChannel = 0; iChannel < paudec->pau->m_cChannel; iChannel++)   {
        if (paudec->pau->m_rgpcinfo [iChannel].m_iPower != 0) {
		    FastFloat rgfltBandWeight[25];
			FastFloat* rgffltSqrtBWRatio = paudec->pau->m_rgpcinfo[iChannel].m_rgffltSqrtBWRatio;
            WeightType* rgxWeightFactor  = paudec->pau->m_rgpcinfo[iChannel].INTEGER_OR_INT_FLOAT(m_rguiWeightFactor,m_rgfltWeightFactor);
		    rgbBandNotCoded = paudec->pau->m_rgpcinfo[iChannel].m_rgbBandNotCoded;
		    cNoiseBand = 0;
            iStart = 0, iEnd = 0;
            iBand = paudec->pau->m_iFirstNoiseBand;            
            while (WMAB_TRUE)    {
                iStart = max (paudec->pau->m_iFirstNoiseIndex, paudec->pau->m_rgiBarkIndex [iBand]);
                if (iStart >= paudec->pau->m_cHighCutOff)
                    break;
                assert (iBand < paudec->pau->m_cValidBarkBand);
                iEnd   = min (paudec->pau->m_cHighCutOff, paudec->pau->m_rgiBarkIndex [iBand + 1]);
                assert (iEnd >= iStart);

			    if (rgbBandNotCoded [iBand] == 1)	{
                    // This may be slower than straight divide, but we must do this to
                    // remain consistent with prvGetNoiseLikeBandsMono
                    rgfltBandWeight [cNoiseBand] = FFLT_AVG_SQUARE_WEIGHTS( rgxWeightFactor+iStart, iEnd-iStart );
                    //// rgfltBandWeight[cNoiseBand].iFraction*1.0F/(1<<rgfltBandWeight[cNoiseBand].iFracBits)
					MONITOR_RANGE(gMR_rgfltBandWeight,FLOAT_FROM_FASTFLOAT(rgfltBandWeight[cNoiseBand]));
					MONITOR_COUNT(gMC_GBW_floats,1+(iEnd-iStart)*2 );
				    cNoiseBand++;
			    }
                SHOW_NOISE_BANDS_ONCE;
                iBand++;
		    }
		    assert (cNoiseBand < paudec->pau->m_cValidBarkBand);
		    assert (paudec->pau->m_iFirstNoiseBand > 0);
			for( iBand = 0; iBand < (cNoiseBand-1); iBand++ )
			{	
				// This calculation is not ideally suited to FastFloat, leave in float for time being
				// But note that limited range of the result probably means it can be done more quickly
                rgffltSqrtBWRatio[iBand] = FFLT_SQRT_RATIO( rgfltBandWeight[iBand], rgfltBandWeight[cNoiseBand-1] );
				//// float SqrtBWRatio = fflt.iFraction*1.0F/(1<<fflt.iFracBits)
				MONITOR_RANGE(gMR_rgffltSqrtBWRatio,FLOAT_FROM_FASTFLOAT(rgffltSqrtBWRatio[iBand]));
				MONITOR_COUNT(gMC_GBW_floats,14);		// count sqrt as 10.
			}
			if (cNoiseBand > 0)
			{	// last band has a ratio of 1.0
#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
				rgffltSqrtBWRatio[cNoiseBand-1].iFraction = 0x40000000;
				rgffltSqrtBWRatio[cNoiseBand-1].iFracBits = 30;
#else
				rgffltSqrtBWRatio[cNoiseBand-1] = 1.0F;
#endif
				MONITOR_RANGE(gMR_rgffltSqrtBWRatio,1.0F);
			}
		    rgbBandNotCoded [0] = cNoiseBand; //used as cNoiseBand
        }
    }
    FUNCTION_PROFILE_STOP(&fp);
}
#endif

