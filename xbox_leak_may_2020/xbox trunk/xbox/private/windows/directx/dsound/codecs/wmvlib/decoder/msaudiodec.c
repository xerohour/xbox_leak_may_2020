/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudioDec.cpp

Abstract:

    Implementation of top level functions of CAudioObjectDecoder.

Author:

    Wei-ge Chen (wchen) 14-July-1998

Revision History:


*************************************************************************/

#include "bldsetup.h"

#include "msaudiodec.h"
#include "macros.h"

#include "AutoProfile.h" 

#if defined(_DEBUG) || defined(BITRATE_REPORT)
extern Int g_cBitGet = 0;
#endif  // _DEBUG || BITRATE_REPORT

//***************************************************************************
// Local Function Prototypes
//***************************************************************************
Void auInitPcInfoDEC (CAudioObject* pau);
FastFloat prvCalcSqrtBWRatioDEC(const PerChannelInfo *ppcinfo,
                            const Int iNoiseBand,
                            const Int cNoiseBand);


//*****************************************************************************************
//*****************************************************************************************
//
//  Outline of decoding process - major function call graph (HighRate is >= 32kbps)
//  Decode
//  ..audecDecode
//  ....prvDecodePacket
//  ......audecDecodeData
//  ........auSaveHistoryMono
//  ........prvDecodeSubFrameHighRate
//  ..........prvDecodeFrameHeader
//  ............prvUpdateSubFrameConfig
//  ............auAdaptToSubFrameConfig
//  ............prvAdaptEqToSubFrame
//  ............prvSetDetTable
//  ............prvReconWeightingFactor
//  ..........qstCalcQuantStep                      ; calc sub frame's quantization step size
//  ..........prvDecodeCoefficentsHighRate<float.c> ; fills rgiCoefRecon[] from bitstream with amplitude as a function of frequency
//  ..........InverseQuantizeHighRate<float.c>
//  ..........dctIV
//  ............FFT
//  ......audecGetPCM
//  ......audecDecodeInfo
//*****************************************************************************************
//*****************************************************************************************


//*****************************************************************************************
//
// audecDelete
//   free up and delete the CAudioObjectDecoder
//
//*****************************************************************************************
Void audecDelete (CAudioObjectDecoder* paudec)
{
    auMallocSetState(MAS_DELETE, NULL, 0);

    auDelete (paudec->pau);
    paudec->pau = NULL;

    auFree(paudec);
}


//*****************************************************************************************
//
// audecFlush
//   usually this deos do anything unless a subframe was put on-hold
//   because of the peekBits()'s look ahead. we set it to NonStop mode and finish it off.
//
//*****************************************************************************************

WMARESULT audecFlush (CAudioObjectDecoder* paudec, 
                    U8*  pbDst, 
                    Int  cbDstLength,
                    Int* pcbDstUsed,
                    U8*  pbEqu,
                    Int  cbEquLength,
                    Int* pcbEquUsed,
                    U32* pcSamplesFromBefore)
{
    WMARESULT hr = WMA_OK;
    Int     cbSrcUsed;
    U32     cSamplesPrevPacket = 0;

    assert (pbDst != NULL);
    assert (pcbDstUsed != NULL);

#if defined(WMA_TARGET_X86) && !(defined(BUILD_INTEGER) || defined(UNDER_CE) || defined(_Embedded_x86))
    // This function may use the FFT. Therefore, on every call to this function,
    // we must check if the current thread has been initialized to use the Intel FFT.
    // This is because under the DMO, the thread which decoded may not be the
    // thread that does the flushing.
    if (paudec->pau->m_fIntelFFT)
        auInitIntelFFT(paudec->pau, INTELFFT_INIT_THREAD);
#endif // defined(WMA_TARGET_X86) && !(defined(BUILD_INTEGER) || defined(UNDER_CE))

    if (pcSamplesFromBefore != NULL)
        *pcSamplesFromBefore = cSamplesPrevPacket;
    if (pcbDstUsed != NULL)
        *pcbDstUsed = 0;
    if (pcbEquUsed != NULL)
        *pcbEquUsed = 0;

    paudec->m_ibstrm.ibstrmpfnPeekBits = ibstrmPeekBitsNonStop;

    if (paudec->pau->m_fAllowSuperFrame) {
        if (!paudec->m_fPacketLoss && 0 != paudec->m_cFrmInPacket)  
        {
            hr = prvDecodePacket (paudec, 
                                  NULL,
                                  0,
                                  &cbSrcUsed,
                                  pbDst,
                                  cbDstLength,
                                  pcbDstUsed,
                                  pbEqu,
                                  cbEquLength,
                                  pcbEquUsed,
                                  (I32 *)&cSamplesPrevPacket);

            if (pcSamplesFromBefore != NULL)
                *pcSamplesFromBefore = cSamplesPrevPacket;

            assert (*pcbDstUsed <= cbDstLength);
            if (*pcbDstUsed > cbDstLength)
                hr = TraceResult(WMA_E_BUFFEROVERFLOW);
        }
    }
    else                //don't do anything
        *pcbDstUsed = 0;

    audecReset(paudec);

    return hr;
} // audecFlush


//*****************************************************************************************
//
// audecNew
//   create and initialize a CAudioObjectDecoder object
//
//*****************************************************************************************
CAudioObjectDecoder* audecNew (void *pMemBuf, const I32 iMemBufSize)
{
    CAudioObjectDecoder* paudec;

    auMallocSetState(MAS_ALLOCATE, pMemBuf, iMemBufSize);
    paudec = (CAudioObjectDecoder*) auMalloc (sizeof (CAudioObjectDecoder));
    if(paudec == NULL)
    {
        TraceResult(WMA_E_OUTOFMEMORY);
        return paudec;
    }

    paudec->pau = NULL;
    paudec->m_fPacketLoss = WMAB_FALSE;
    paudec->m_cFrmInPacket = 0;
    paudec->m_pbSrcCurr = NULL;
    paudec->m_cbSrcCurrLength = 0; 
    paudec->m_decsts = BEGIN_PACKET;
    paudec->m_subfrmdecsts = SUBFRM_HDR;
    paudec->m_hdrdecsts = HDR_SIZE;
    paudec->m_rlsts = VLC;
    paudec->m_iChannel = 0;
    paudec->m_iBand = 0;
    paudec->m_fNoMoreData = WMAB_FALSE;
    paudec->m_fLastSubFrame = WMAB_TRUE;
    memset(&paudec->m_ibstrm, 0, sizeof(paudec->m_ibstrm));
    paudec->m_rgiRunEntry44ssQb = NULL;
    paudec->m_rgiLevelEntry44ssQb = NULL;
    paudec->m_rgiRunEntry44smQb = NULL;
    paudec->m_rgiLevelEntry44smQb = NULL;
    paudec->m_rgiRunEntry44ssOb = NULL;
    paudec->m_rgiLevelEntry44ssOb = NULL;
    paudec->m_rgiRunEntry44smOb = NULL;
    paudec->m_rgiLevelEntry44smOb = NULL;
    paudec->m_rgiRunEntry16ssOb = NULL;
    paudec->m_rgiLevelEntry16ssOb = NULL;
    paudec->m_rgiRunEntry16smOb = NULL;
    paudec->m_rgiLevelEntry16smOb = NULL;
#if defined(UNDER_CE) && defined(_ARM_)
    paudec->lPrivate1 = 0;
    paudec->lPrivate2 = 0;
#endif //defined(UNDER_CE) && defined(_ARM_)
#ifdef ENABLE_EQUALIZER
    memset(&paudec->m_rgfltEqBandWeight, 0, sizeof(paudec->m_rgfltEqBandWeight));
    memset(&paudec->m_rgwEqBandBoundary, 0, sizeof(paudec->m_rgwEqBandBoundary));
    paudec->m_cEqBand = 0;
    paudec->m_fNoEq = WMAB_TRUE; 
    memset(&paudec->m_rgbBandPower, 0, sizeof(paudec->m_rgbBandPower));
    paudec->m_fComputeBandPower = WMAB_FALSE;
    paudec->m_iEqFrameSize = 0;
#endif // ENABLE_EQUALIZER
#if defined(SEEK_DECODE_TEST)
    paudec->m_pSaveRandState = NULL;
#endif  // defined(SEEK_DECODE_TEST)
    paudec->m_pfnDecodeSubFrame = NULL;
    paudec->m_pfnDecodeCoefficient = NULL;
    
    return paudec;
}


//*****************************************************************************************
//
// audecInit
//   initialize a CAudioObjectDecoder for parameters from input file or stream
//
//*****************************************************************************************
WMARESULT audecInit (CAudioObjectDecoder* paudec, 
                   Int iVersionNumber,
                   Int cSubband, 
                   Int iSamplingRate, 
                   U16 cChannel, 
                   Int cBytePerSec, 
                   Int cbPacketLength,
                   U16 wEncodeOpt,
                   U16 wPlayerOpt,
                   PFNGETMOREDATA pfnGetMoreData,
                   U32 dwUserData,
                   I32 *piMemUsed)
{
    Float fltBitsPerSample = 0.0F;
    CAudioObject* pau = paudec->pau;
    WMARESULT   wmaResult = WMA_OK;

    if (pau == NULL)
        paudec->pau = pau = auNew ();

    // Fill in function ptrs with decoder fns
    pau->aupfnGetNextRun = prvGetNextRunDEC;

    if (pau->m_codecStatus == CODEC_BEGIN)
        goto exit;

    //wchen: in the future we should use the one directly from the bitstream.
    //but for now the one in the bistream doesn't represent Sample/Frame
    //instead it represents Sample/Raw Packet that is useless for the decoder
    //other than serving the stupidity in the V4RTM decoder. We can't change the format for now.
    //but it should be changed to reprsent Sample/Frame and that should nullify the following function call.
    cSubband = msaudioGetSamplePerFrame (iSamplingRate, cBytePerSec * 8, cChannel, iVersionNumber);

    TRACEWMA_EXIT(wmaResult, auInit (pau, iVersionNumber, cSubband, iSamplingRate, 
        cChannel, cBytePerSec, cbPacketLength, wEncodeOpt, wPlayerOpt));

    auInitPcInfoDEC (pau);

    if (cChannel == 1)
        paudec->m_pfnDecodeCoefficient = &prvDecodeCoefficientMono;
    else
        paudec->m_pfnDecodeCoefficient = &prvDecodeCoefficientStereo;

#ifdef ENABLE_EQUALIZER
    wmaResult = audecResetEqualizer(paudec);
    TraceError(wmaResult);
#endif //ENABLE_EQUALIZER

    ibstrmInit (&paudec->m_ibstrm, pau->m_fAllowSuperFrame);
    ibstrmSetOwner (&paudec->m_ibstrm, (U32)paudec); 

    if (NULL == pfnGetMoreData)
    {
        // Set us up for non-streaming mode
        assert(0 == dwUserData); // We ignore user-supplied data in non-streaming mode
        pfnGetMoreData = prvWMAGetMoreData;
        dwUserData = (U32)paudec;
    }

    ibstrmSetGetMoreData (&paudec->m_ibstrm, pfnGetMoreData);
    ibstrmSetUserData (&paudec->m_ibstrm, (U32)dwUserData); //need to change for real streaming mode

    if (pau->m_fAllowSuperFrame)
        paudec->m_ibstrm.ibstrmpfnPeekBits = ibstrmPeekBits;
    else
        paudec->m_ibstrm.ibstrmpfnPeekBits = ibstrmPeekBitsNonStop;

    if (pau->m_iEntropyMode == SIXTEENS_OB)   {
        pau->m_rgpcinfo [0].m_rgiHuffDecTbl = g_rgiHuffDecTbl16smOb;
        pau->m_rgpcinfo [0].m_rgiRunEntry   = gRun16smOb;
        pau->m_rgpcinfo [0].m_rgiLevelEntry = gLevel16smOb;
    }
#ifdef ENABLE_ALL_ENCOPT
    else if (pau->m_iEntropyMode == FOURTYFOURS_QB)   {
        pau->m_rgpcinfo [0].m_rgiHuffDecTbl = g_rgiHuffDecTbl44smQb; 
        pau->m_rgpcinfo [0].m_rgiRunEntry   = gRun44smQb; 
        pau->m_rgpcinfo [0].m_rgiLevelEntry = gLevel44smQb;
    }
    else if (pau->m_iEntropyMode == FOURTYFOURS_OB)   {
        pau->m_rgpcinfo [0].m_rgiHuffDecTbl = g_rgiHuffDecTbl44smOb; 
        pau->m_rgpcinfo [0].m_rgiRunEntry   = gRun44smOb; 
        pau->m_rgpcinfo [0].m_rgiLevelEntry = gLevel44smOb;
    }
#endif // ENABLE_ALL_ENCOPT
    else {
        assert (WMAB_FALSE);
        wmaResult = TraceResult(WMA_E_INVALIDARG);
        goto exit;
    }

    //init subfrm configration
    pau->m_subfrmconfigPrev.m_cSubFrame = 1;
    pau->m_subfrmconfigPrev.m_rgiSubFrameSize [0]  = pau->m_cFrameSampleHalf;
    pau->m_subfrmconfigPrev.m_rgiSubFrameStart [0] = 0;
    pau->m_subfrmconfigPrev.m_iFirstTransientSubFrm = -1;
    pau->m_subfrmconfigPrev.m_iLastTransientSubFrm  = -1;
    memcpy (&pau->m_subfrmconfigCurr, &pau->m_subfrmconfigPrev, sizeof (SubFrameConfigInfo));
    memcpy (&pau->m_subfrmconfigNext, &pau->m_subfrmconfigPrev, sizeof (SubFrameConfigInfo));

    pau->m_codecStatus = CODEC_BEGIN;
    pau->m_iPacketCurr = -2;                // force a Packet Loss to begin

#if 0
    {   Int i;
        // Initialize quant step table
        for (i = 0; i < ARRAY_SIZE(paudec->m_rgiQuantStepFract); i++)
        {
            paudec->m_rgiQuantStepFract[i] = (UInt)(pow(10, (Float)(i + 1) / QUANTSTEP_DENOMINATOR) *
                (1 << QUANTSTEP_FRACT_BITS));
        }
    }
#endif  // 0

    if (pau->m_iWeightingMode == LPC_MODE || pau->m_fNoiseSub)
    {
        //low rate or mid rate
        paudec->m_pfnDecodeSubFrame = prvDecodeSubFrame;
    }
    else
    {
        //high rate
        paudec->m_pfnDecodeSubFrame = prvDecodeSubFrameHighRate;
    }

#ifdef BITRATE_REPORT
    printf("Fr#\tSubF#\tTime\tPCM\tSF Bits\tSF Bitr\tFr Bits\tFr Bitrate\n");
    printf("---\t-----\t----\t---\t-------\t-------\t-------\t----------\n");
#endif   // BITRATE_REPORT

    if (NULL != piMemUsed)
        *piMemUsed = auMallocGetCount();

    auMallocSetState(MAS_LOCKED, NULL, 0);

    if (pau->m_iWeightingMode != BARK_MODE)
    {
        assert(prvInverseQuantizeLowRate == pau->aupfnInverseQuantize);
        pau->aupfnCalcSqrtBWRatio = prvCalcSqrtBWRatioDEC;
    }

exit:
    return wmaResult;
}


//*****************************************************************************************
//
// auInitPcInfoDEC
//
//*****************************************************************************************
Void auInitPcInfoDEC (CAudioObject* pau)
{
    I16 i;
    const U16 c_iSizeOfPrevOutput = (U16) DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf) ;
    I16* piSignBase = pau->m_piPrevOutput + c_iSizeOfPrevOutput * pau->m_cChannel;
    for (i = 0; i < pau->m_cChannel; i++)
    {
        PerChannelInfo* ppcinfo = pau->m_rgpcinfo + i;

//      memset(ppcinfo->m_rgfltMask, 0, sizeof(ppcinfo->m_rgfltMask));
        ppcinfo->m_rgiCoefQ = NULL;
        ppcinfo->m_rgiMaskQ = pau->m_rgiMaskQ + pau->m_cValidBarkBand * i;
        ppcinfo->m_rgiHuffDecTbl = NULL;
        ppcinfo->m_rgiRunEntry = NULL;
        ppcinfo->m_rgiLevelEntry = NULL;
        ppcinfo->m_cSubbandActual = pau->m_cHighCutOff - pau->m_cLowCutOff;
        ppcinfo->m_iPower = 0;
        ppcinfo->m_iActualPower = 0;

        //ppcinfo->m_rgiWeightFactor: initialized below
        ppcinfo->m_rgiCoefRecon = pau->m_rgiCoefReconOrig + DOUBLE(pau->m_fPad2XTransform,
            pau->m_cSubband) * i;

        ppcinfo->m_iMaxMaskQ = 0;
        ppcinfo->m_iCurrGetPCM_SubFrame = (I16) pau->m_cSubFrameSampleHalfAdjusted;

        ppcinfo->m_rgiCoefReconCurr = ppcinfo->m_rgiCoefRecon + pau->m_cSubbandAdjusted /2;
        ppcinfo->m_piPrevOutput = pau->m_piPrevOutput + i;
        ppcinfo->m_piPrevOutputSign = piSignBase + i * (c_iSizeOfPrevOutput /
            BITS_PER_BYTE / sizeof (I16)); //non 17bit shouldn't use this pointer at all
        ppcinfo->m_piPrevOutputCurr = ppcinfo->m_piPrevOutput;

        ppcinfo->m_iSin = 0;
        ppcinfo->m_iCos = 0;
        ppcinfo->m_iSin1 = 0;
        ppcinfo->m_iCos1 = 0;
        ppcinfo->m_iStep = 0;

        ppcinfo->m_rgfltCoefRecon = (Float *)(ppcinfo->m_rgiCoefRecon); //This will soon be removed
    
        //ppcinfo->m_rgfltWeightFactor: initialized below
        ppcinfo->m_wtMaxWeight = 0; // Currently always used because LPC not integerized at encoder
    
        if (pau->m_iWeightingMode == LPC_MODE)
        { // LPC
            ppcinfo->m_rguiWeightFactor = pau->m_rguiWeightFactor + DOUBLE(pau->m_fPad2XTransform,pau->m_cSubband) * i;
            ppcinfo->m_rgfltWeightFactor = (float*)(pau->m_rguiWeightFactor + DOUBLE(pau->m_fPad2XTransform,pau->m_cSubband) * i);
        } else 
        {
            ppcinfo->m_rguiWeightFactor =(U32*) pau->m_rgpcinfo [i].m_rgiMaskQ;
            ppcinfo->m_rgfltWeightFactor = (float*)(pau->m_rgpcinfo [i].m_rgiMaskQ);
        }

        ppcinfo->m_rgbBandNotCoded = NULL;
        ppcinfo->m_rgffltSqrtBWRatio = NULL;
        ppcinfo->m_rgiNoisePower = NULL;
        ppcinfo->m_rgfltBandWeight = NULL;
#ifdef ENABLE_ALL_ENCOPT
        //set up default pcinfo for noise sub
        pau->m_rgpcinfo [i].m_rgbBandNotCoded   = pau->m_rgbBandNotCoded   + pau->m_cValidBarkBand * i;
        pau->m_rgpcinfo [i].m_rgffltSqrtBWRatio = pau->m_rgffltSqrtBWRatio + pau->m_cValidBarkBand * i;
        pau->m_rgpcinfo [i].m_rgiNoisePower     = pau->m_rgiNoisePower     + pau->m_cValidBarkBand * i;
#ifndef BUILD_INTEGER
        pau->m_rgpcinfo [i].m_rgfltBandWeight   = pau->m_rgfltBandWeight   + pau->m_cValidBarkBand * i;
#endif
#endif

        pau->m_rgpcinfo [i].ppcinfoENC = NULL;
    } // for
}

WMARESULT audecReset (CAudioObjectDecoder *paudec)
{
    CAudioObject* pau;
    if (paudec == NULL)
        return WMA_OK;
    pau = paudec->pau;

    //since a packet loss will be forced; the following are already being done there
#if 0
    for (i = 0; i < pau->m_cChannel; i++)
    {
        PerChannelInfo* ppcinfo = pau->m_rgpcinfo + i;
        ppcinfo->m_iCurrGetPCM_SubFrame = (I16) pau->m_iCoefRecurQ3;
    }

    if ( pau->m_piPrevOutput != NULL )
    {
        const int c_iSizeOfPrevOutput = sizeof (I16) * DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf) * pau->m_cChannel;
#       ifndef PREVOUTPUT_16BITS
            const int c_iSizeOfPrevOutputSign = DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf) * pau->m_cChannel / BITS_PER_BYTE + pau->m_cChannel*sizeof(I16);
#       else 
            const int c_iSizeOfPrevOutputSign = 0;
#       endif
        memset (pau->m_piPrevOutput, 0, c_iSizeOfPrevOutput + c_iSizeOfPrevOutputSign);
    }
#endif

    //this is not needed anymore
    //if we set packetheader to 0, meaning whatever left in the current packet
    //will be discarded by decodeInfo(). We just have to make sure
    //decodeInfo() gets called right after this. But it has to be since we are forcing
    //a packet loss situation.


	//to suppress packet loss check
    ibstrmReset(&paudec->m_ibstrm);
    ibstrmSetPacketHeader(&paudec->m_ibstrm, 0);

    // During a flush (eg, end-of-file), we use ibstrmPeekBitsNonStop. To start playing
    // at the start of a file once more, we need to reset to ibstrmPeekBitsNonStop.
    if (pau->m_fAllowSuperFrame)
        paudec->m_ibstrm.ibstrmpfnPeekBits = ibstrmPeekBits;
    else
        paudec->m_ibstrm.ibstrmpfnPeekBits = ibstrmPeekBitsNonStop;

    //unlock ourself because there is an error already
    //wouldn't continue to decode the frame, making sure decodeInfo will be called
    if (pau->m_codecStatus == CODEC_ONHOLD)
    {
        pau->m_codecStatus = CODEC_STEADY;
    }

    //== to codec begin
    pau->m_iPacketCurr = -2;        //make sure we see a packet loss

    return WMA_OK;
}

U32 prvOutputBufferSize(Bool fAllowSuperFrame, Int cSamplesPerFrame, Int cChannels) {
  return sizeof (I16) * ( fAllowSuperFrame ? 32 : 1 )
                      * cSamplesPerFrame * cChannels;

}

U32 audecOutputBufferSize(Int iVersion, Int iSamplingRate, Int cChannels, Int cBytesPerSec, U16 wEncodeOpt) {
  return prvOutputBufferSize(!!(wEncodeOpt & ENCOPT_SUPERFRAME),
                             msaudioGetSamplePerFrame (iSamplingRate,
                                                       cBytesPerSec * 8,
                                                       cChannels,
                                                       iVersion),
                             cChannels);
}

//*****************************************************************************************
//
// audecDecode
//   decode a frame or subframe
//
//*****************************************************************************************
WMARESULT audecDecode (CAudioObjectDecoder* paudec, 
                     const U8* pbSrc,
                     Int       cbSrcLength,
                     Int*      pcbSrcUsed,
                     U8*       pbDst,
                     Int       cbDstLength,
                     Int*      pcbDstUsed,
                     U8*       pbEqu,
                     Int       cbEqu,
                     Int*      pcbEquUsed,
                     I32*      pcSamplesFromBefore)
{   
    WMARESULT hr;
    Int cbRawPacket;
    Int cbSrcUsed; 
    Int cbDstUsed;
    Int cbEquUsed;
    Int cbEquRemaining;
    Int cSamplesPrevPacket = 0;
    Bool fFirstPacket = WMAB_TRUE;
    CAudioObject* pau = paudec->pau;
#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,DECODE_PROFILE);
#endif

    assert (pbSrc != NULL && pcbSrcUsed != NULL && pbDst != NULL && pcbDstUsed != NULL);
    if (pbSrc == NULL || pcbSrcUsed == NULL || pbDst == NULL || pcbDstUsed == NULL)
        return TraceResult(WMA_E_INVALIDARG);

    if (cbSrcLength <= 0) {  //nothing to decode
        *pcbDstUsed   = 0;
        hr = WMA_OK;
        goto exit;
    }

    cbRawPacket = prvOutputBufferSize(pau->m_fAllowSuperFrame, pau->m_cFrameSampleHalfAdjusted, pau->m_cChannel);
    
    *pcbSrcUsed = 0;
    *pcbDstUsed = 0;
    if (NULL != pcbEquUsed)
        *pcbEquUsed = 0;

    cbEquRemaining = cbEqu;
    hr = WMA_OK;
    while (WMAB_TRUE)    {
        if ((cbSrcLength - *pcbSrcUsed) * BITS_PER_BYTE < pau->m_cBitPacketLength ||
            (cbDstLength - *pcbDstUsed) < cbRawPacket)      //this is to be safe, usually less than cbRawPacket is needed
            break;

        hr = prvDecodePacket  (paudec,
                               pbSrc, 
                               pau->m_cBitPacketLength/BITS_PER_BYTE, 
                               &cbSrcUsed,
                               pbDst, 
                               cbDstLength - *pcbDstUsed, 
                               &cbDstUsed,
                               pbEqu,
                               cbEquRemaining,
                               &cbEquUsed, 
                               &cSamplesPrevPacket);

        //first time
        if (fFirstPacket && pcSamplesFromBefore != NULL)
        {
            *pcSamplesFromBefore = cSamplesPrevPacket;
            fFirstPacket = WMAB_FALSE;
        }

        assert (cbSrcUsed == pau->m_cBitPacketLength/BITS_PER_BYTE);
        pbSrc       += cbSrcUsed;
        *pcbSrcUsed += cbSrcUsed;

        CHECKWMA_EXIT(hr);

        assert (cbDstUsed <= cbRawPacket);
        pbDst       += cbDstUsed;
        *pcbDstUsed += cbDstUsed;

        if (NULL != pbEqu)
        {
            pbEqu       += cbEquUsed;
            cbEquRemaining -= cbEquUsed;
            if (NULL != pcbEquUsed)
                *pcbEquUsed += cbEquUsed;
        }
        
    }
exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif

    return hr;
}

#if defined(_DEBUG)
static int iSampleTotal = 0;
#endif

//*****************************************************************************************
//
// prvDecodePacket
//
//*****************************************************************************************
WMARESULT prvDecodePacket (CAudioObjectDecoder* paudec, 
                         const  U8*           pbSrc,
                         Int                  cbSrcLength,
                         Int*                 pcbSrcUsed,
                         U8*                  pbDst,
                         Int                  cbDstLength,
                         Int*                 pcbDstUsed,
                         U8*                  pbEqu,
                         Int                  cbEquLength,
                         Int*                 pcbEquUsed,
                         I32*                 pcSamplesPrevPacket)
{
    WMARESULT hr;
    Int cbRawFrame;
    U16 cSampleDecoded = 0;
    CAudioObject* pau = paudec->pau;
    Int cIteration = 0;
    Bool fDoDecode = WMAB_TRUE;
#ifdef PROFILE
    //FunctionProfile fp;
#endif

    assert(pcbSrcUsed != NULL && pbDst != NULL && pcbDstUsed != NULL);
    assert(paudec->m_ibstrm.m_pfnGetMoreData == prvWMAGetMoreData); // non-streaming mode MUST use the std fn
    assert(paudec->m_ibstrm.m_dwUser == (U32)paudec);

    if (pcbSrcUsed == NULL || pbDst == NULL || pcbDstUsed == NULL)
        return TraceResult(WMA_E_INVALIDARG);

    *pcbSrcUsed = *pcbDstUsed = 0;
    *pcSamplesPrevPacket = 0;
    cbRawFrame = pau->m_cFrameSampleHalfAdjusted * sizeof (I16) * pau->m_cChannel;

    // Verify that we have enough space in destination for worst-case scenario
    if ((paudec->pau->m_fAllowSuperFrame && cbDstLength < 16 * cbRawFrame) ||
        (WMAB_FALSE == paudec->pau->m_fAllowSuperFrame && cbDstLength < cbRawFrame))
        return TraceResult(WMA_E_BUFFEROVERFLOW);

#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
    if ( pau->m_fUpsample || pau->m_fPad2XTransform )
        cbRawFrame *= 2;    
#endif

#ifdef PROFILE
    //FunctionProfileStart(&fp,DECODE_PACKET_PROFILE);
#endif

    *pcbSrcUsed = 0;
    *pcbDstUsed = 0;
    if (NULL != pcbEquUsed)
        *pcbEquUsed = 0;

    //to be used by getMoreData
    paudec->m_pbSrcCurr = (U8*) pbSrc; 
    paudec->m_cbSrcCurrLength = (U16) min (cbSrcLength, paudec->pau->m_cBitPacketLength/BITS_PER_BYTE); 
   
    while (fDoDecode)
    {
        switch (pau->m_codecStatus)
        {
        case CODEC_BEGIN:
        case CODEC_STEADY:
            //only flush will call with null,but, in which case it goes to on-hold directly
            if (pbSrc == NULL || cbSrcLength <= 0)
            {
                hr = TraceResult(WMA_E_INVALIDARG);
                goto exit;
            }

            TRACEWMA_EXIT (hr, audecDecodeInfo (paudec));
            *pcSamplesPrevPacket = *pcbDstUsed / pau->m_cChannel / sizeof (I16);
            fDoDecode = WMAB_FALSE;

#           if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
            prvWmaShowFrames(paudec->pau, "\nDecPkt2", " %1d ", 
                paudec->m_cFrmInPacket );
#           endif

        case CODEC_ONHOLD:
            // If no superframes, WMA_S_NO_MORE_SRCDATA always happens at the end of
            // each frame and should constitute an exit condition. If superframes,
            // WMA_S_NO_MORE_SRCDATA should not exit this loop or we may not decode
            // last frame.
            while (WMAB_TRUE) 
            {
                HEAP_DEBUG_CHECK;
                hr = audecDecodeData (paudec, &cSampleDecoded);
                HEAP_DEBUG_CHECK;

                if (hr == WMA_E_ONHOLD)
                {
                    //remembers it
                    pau->m_codecStatus = CODEC_ONHOLD; 
                    //map it to okay
                    TraceResult (hr = WMA_OK);
                    goto exit;
                }
                else if (hr == WMA_E_BROKEN_FRAME)
                {
                    audecReset (paudec);
                    TraceResult (hr = WMA_OK);
                    goto exit;  //bits will be advanced
                }
                else if (hr == WMA_E_LOSTPACKET)
                {
                    TraceResult (hr = WMA_OK);
                    break;      //try it again will call decodeInfo next round
                }

                CHECKWMA_EXIT(hr);

#               if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
                prvWmaShowFrames(pau, "GetPCM2", " %4d 0x%08x %6d     @%8d",
                    cSampleDecoded, pbDst, cbDstLength - *pcbDstUsed, iSampleTotal);
#               endif

                HEAP_DEBUG_CHECK;
                audecGetPCM (paudec, &cSampleDecoded, pbDst, cbDstLength - *pcbDstUsed);
                HEAP_DEBUG_CHECK;

#               if defined(_DEBUG)
                assert(pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame >= pau->m_iCoefRecurQ3);
                iSampleTotal += cSampleDecoded;
#               endif
                cSampleDecoded *= sizeof (I16) * paudec->pau->m_cChannel; //in bytes
                //when seeking, we can return the 1st half of the subframe
                //whose begining the packet timestamp points to 
                //we use this negative number as a workaround
                if (pau->m_fSeekAdjustment && pau->m_fAllowSuperFrame)
                {
                    *pcSamplesPrevPacket -= (pau->m_iCoefRecurQ2 - pau->m_iCoefRecurQ1);
                    pau->m_fSeekAdjustment = WMAB_FALSE;
                }

                pbDst           += cSampleDecoded;
                *pcbDstUsed     += cSampleDecoded;
                assert (*pcbDstUsed <= cbDstLength);
                if (*pcbDstUsed > cbDstLength)          //out of space
                {
                    //FUNCTION_PROFILE_STOP(&fp);
                    return TraceResult(WMA_E_BUFFEROVERFLOW);
                }
#ifdef ENABLE_EQUALIZER        
                if (paudec->m_fLastSubFrame)    
                {
                    if (NULL != pbEqu && paudec->m_fComputeBandPower) 
                    {
                        Int cbEquUsedFrame = sizeof (I8) * MAX_NO_EQBAND;
                        memcpy (pbEqu, paudec->m_rgbBandPower, cbEquUsedFrame);
                        pbEqu       += cbEquUsedFrame;
                        cbEquLength -= cbEquUsedFrame;
                        if (NULL != pcbEquUsed)
                            *pcbEquUsed += cbEquUsedFrame;
                    }
                }
#endif

                if (cIteration > pau->m_iMaxSubFrameDiv * 0x0F)
                    return TraceResult(WMA_E_BROKEN_FRAME);              //never overflow the stack
                else 
                {
                    cIteration++;
                    if (hr == WMA_S_NO_MORE_FRAME)
                        break;
                }
            }
            pau->m_codecStatus = CODEC_STEADY;
        }
    }

exit:
    // always tell them to give us the next packet.
    *pcbSrcUsed = pau->m_cBitPacketLength/BITS_PER_BYTE;

    //FUNCTION_PROFILE_STOP(&fp);
    HEAP_DEBUG_CHECK;
    return hr;
}

//*****************************************************************************************
//
// audecDecodeInfo
//
//*****************************************************************************************
WMARESULT audecDecodeInfo (CAudioObjectDecoder* paudec)
{   
    Int iPacketCurr;
    Int cBitLeftOver = 0;
    Int cBitLs, cBitRs;
    WMARESULT   wmaResult = WMA_OK;

    paudec->m_decsts = BEGIN_PACKET;
    paudec->m_fPacketLoss = WMAB_FALSE;
    paudec->m_cFrmInPacket = 1;

    if (paudec->pau->m_fAllowSuperFrame) 
    {
        //no left over
        while(ibstrmGetPacketHeader(&paudec->m_ibstrm) == 0) 
        {
            U32         iBufLen;

            ibstrmReset(&paudec->m_ibstrm);
            TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(&paudec->m_ibstrm, &iBufLen));

            if(paudec->m_fNoMoreData)
            {
                //wmaResult = WMA_S_NO_MORE_SRCDATA;
                //theoretically we can be on-hold but not tested as apps don't do this
                wmaResult = WMA_E_ONHOLD;
                goto exit;
            }
        }

        cBitLs = 0;
        cBitRs = (BITS_PER_DWORD - NBITS_PACKET_CNT);
        iPacketCurr = ibstrmGetPacketHeader(&paudec->m_ibstrm) >> cBitRs;
        if (iPacketCurr - paudec->pau->m_iPacketCurr == 1 || 
            iPacketCurr - paudec->pau->m_iPacketCurr + (1 << NBITS_PACKET_CNT) == 1) {
            paudec->m_fPacketLoss = WMAB_FALSE;
        }
        else 
            paudec->m_fPacketLoss = WMAB_TRUE;
        paudec->pau->m_iPacketCurr = iPacketCurr;

        cBitLs = NBITS_FRM_CNT;
        cBitRs = BITS_PER_DWORD - NBITS_FRM_CNT;
        paudec->m_cFrmInPacket = (U16) ((ibstrmGetPacketHeader(&paudec->m_ibstrm) 
                                 << cBitLs) >> cBitRs);
        assert (paudec->m_cFrmInPacket < (1 << NBITS_FRM_CNT));

        cBitLs = NBITS_PACKET_CNT + NBITS_FRM_CNT;
        cBitRs = BITS_PER_DWORD - (paudec->pau->m_cBitPackedFrameSize + 3); 
        cBitLeftOver = (ibstrmGetPacketHeader(&paudec->m_ibstrm) << cBitLs) >> cBitRs;
        if (cBitLeftOver == 0) 
            ibstrmResetPacket(&paudec->m_ibstrm);
        ibstrmSetPacketHeader(&paudec->m_ibstrm, 0);
    }
    else 
    {
        Int iCurrPrevDiff;

        // We should only hit this on very first frame
        if (paudec->pau->m_iPacketCurr < 0)
        {
            assert(ibstrmGetPacketHeader(&paudec->m_ibstrm) == 0);
            paudec->pau->m_iPacketCurr = ibstrmGetPacketHeader(&paudec->m_ibstrm);
        }

        iCurrPrevDiff = ibstrmGetPacketHeader(&paudec->m_ibstrm) -
            paudec->pau->m_iPacketCurr;

        // Non-superframe mode can't really lose packets
        assert(iCurrPrevDiff >= 0 || iCurrPrevDiff <= 1 - NONSUPER_WRAPAROUND);
        assert(iCurrPrevDiff < 0 || iCurrPrevDiff <= 1);
        assert(iCurrPrevDiff >= 0 || NONSUPER_WRAPAROUND + iCurrPrevDiff <= 1);

        // If, while processing the last frame we loaded in the start of next frame,
        // cue bitstream pointer to start of next frame
        if (iCurrPrevDiff != 0)
            ibstrmResetPacket(&paudec->m_ibstrm);

        // Advance to next payload: discard data until we hit next frame
        while (ibstrmGetPacketHeader(&paudec->m_ibstrm) == (U32)paudec->pau->m_iPacketCurr)
        {
            U32 iBufLen;

            ibstrmReset(&paudec->m_ibstrm);
            TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(&paudec->m_ibstrm, &iBufLen));

            if(paudec->m_fNoMoreData)
            {
                //wmaResult = WMA_S_NO_MORE_SRCDATA;
                wmaResult = WMA_E_ONHOLD;
                goto exit;
            }
        }

        paudec->pau->m_iPacketCurr = ibstrmGetPacketHeader(&paudec->m_ibstrm);

        if(paudec->m_fNoMoreData)
        {
            //wmaResult = WMA_S_NO_MORE_SRCDATA;
            wmaResult = WMA_E_FAIL;
            goto exit;
        }
    }

#if defined(SEEK_DECODE_TEST)
    if (paudec->m_pSaveRandState != NULL ) {
        *paudec->m_pSaveRandState = paudec->pau->m_tRandState;
        paudec->m_pSaveRandState = NULL;
    }
#   endif // SEEK_DECODE_TEST

    if (paudec->m_fPacketLoss == WMAB_FALSE)
    {
        wmaResult = WMA_OK;
        goto exit;
    }
    else    {
        CAudioObject* pau = paudec->pau;
        I16 i;
        const int c_iSizeOfPrevOutput = sizeof (I16) * DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf) * pau->m_cChannel;
#       ifndef PREVOUTPUT_16BITS
            const int c_iSizeOfPrevOutputSign = DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf) * pau->m_cChannel / BITS_PER_BYTE + pau->m_cChannel*sizeof(I16);
#       else 
            const int c_iSizeOfPrevOutputSign = 0;
#       endif

        //packet loss or a broken frame earlier, seek to next complete frm
        while (cBitLeftOver > 24)   {
            TRACEWMA_EXIT(wmaResult, ibstrmFlushBits (&paudec->m_ibstrm, 24));
            cBitLeftOver -= 24;
        }
        TRACEWMA_EXIT(wmaResult, ibstrmFlushBits (&paudec->m_ibstrm, cBitLeftOver));

        for (i = 0; i < pau->m_cChannel; i++)
        {
            PerChannelInfo* ppcinfo = pau->m_rgpcinfo + i;
            ppcinfo->m_iCurrGetPCM_SubFrame = (I16) CURRGETPCM_INVALID;
        }

        if (pau->m_piPrevOutput != NULL)
            memset (pau->m_piPrevOutput, 0, c_iSizeOfPrevOutput + c_iSizeOfPrevOutputSign);

		//so that the 1st frame doesn't output anything
        pau->m_fPacketLossAdj = WMAB_TRUE;
        pau->m_fSeekAdjustment = WMAB_TRUE; //not relavent in streaming mode

        // Ignore packet loss, we're already set to deal with it
        wmaResult = WMA_OK; // WMA_BROKEN_FRAME;
        goto exit;
    }

exit:
    return wmaResult;
}

#ifdef BITRATE_REPORT
#define BITRATE_REPORT_VARS         Int cBitGet
#define BITRATE_REPORT_CHECKPT      cBitGet = g_cBitGet
#define BITRATE_REPORT_PRINT        PrintSubframeStats(pau, cBitGet)

Float g_fltRunningTime = 0;
Int g_cSubframeBits = 0;

void PrintSubframeStats(const CAudioObject *pau, const Int cBitGetCheckPt)
{
    Int iSubframePCM;
    Int iBitsInSubframe;
    Float fltSubframeTime;

    iSubframePCM = pau->m_cSubbandAdjusted;
    iBitsInSubframe = g_cBitGet - cBitGetCheckPt;
    fltSubframeTime = iSubframePCM / (float)pau->m_iSamplingRate;

    // Print frame number only for first subframe (helps with visual formatting)
    if (0 == pau->m_iCurrSubFrame)
        printf("%d\t", pau->m_iFrameNumber);
    else
        printf("\t");

    // Print subframe stats
    printf("%d\t%6.3f\t%4d\t%6d\t%7.0f", pau->m_iCurrSubFrame,
        g_fltRunningTime, iSubframePCM, iBitsInSubframe,
        iBitsInSubframe / fltSubframeTime);

    g_fltRunningTime += fltSubframeTime;
    g_cSubframeBits += iBitsInSubframe;

    // If last subframe in frame, print frame summary
    if (pau->m_iCurrSubFrame + 1 >= pau->m_subfrmconfigCurr.m_cSubFrame)
    {
        const Float fltFrameTime = (float)(2048.0 / pau->m_iSamplingRate);
        printf("\t%6d\t%7.0f\n", g_cSubframeBits, g_cSubframeBits / fltFrameTime);
        g_cSubframeBits = 0;
    }
    else
        printf("\n");
}
#else    // BITRATE_REPORT
#define BITRATE_REPORT_VARS
#define BITRATE_REPORT_CHECKPT
#define BITRATE_REPORT_PRINT
#endif  // BITRATE_REPORT

//*****************************************************************************************
//
// audecDecodeData
//
//*****************************************************************************************
WMARESULT audecDecodeData (CAudioObjectDecoder* paudec, U16* pcSampleReady)
{
    CAudioObject* pau = paudec->pau;
    PerChannelInfo* ppcinfo = pau->m_rgpcinfo;
    WMARESULT hr = WMA_OK;
    I16 iChannel;
    Int cBitUsed = 0;
    U32 iBufLen;

#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,AUDEC_DECODE_DATA_PROFILE);
#endif

#if defined(WMA_TARGET_X86) && !(defined(BUILD_INTEGER) || defined(UNDER_CE) || defined(_Embedded_x86))
    // This function may use the FFT. Therefore, on every call to this function,
    // we must check if the current thread has been initialized to use the Intel FFT.
    // This is because under the DMO, the thread which initialized us may not be the
    // thread that does the decoding.
    if (pau->m_fIntelFFT)
        auInitIntelFFT(pau, INTELFFT_INIT_THREAD);
#endif // defined(WMA_TARGET_X86) && !(defined(BUILD_INTEGER) || defined(UNDER_CE))

    *pcSampleReady = 0;
    //if we are being resumed, we need to get the new data
    if (paudec->m_fNoMoreData)
        TRACEWMA_EXIT(hr, ibstrmGetMoreData(&paudec->m_ibstrm, &iBufLen));

    while(1)
    {
        BITRATE_REPORT_VARS;
        switch (paudec->m_decsts)
        {
            case BEGIN_SUBFRAME :
                if (!paudec->m_fLastSubFrame) 
                {                   
                    for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
                    {
                        ppcinfo = pau->m_rgpcinfo + iChannel;
                        if (!(ppcinfo->m_iCurrGetPCM_SubFrame==pau->m_iCoefRecurQ3 || ppcinfo->m_iCurrGetPCM_SubFrame>=pau->m_iCoefRecurQ4))
                            CHECKWMA_EXIT( hr = WMA_E_BROKEN_FRAME );

                        HEAP_DEBUG_CHECK;
                        hr = auSaveHistoryMono (pau, ppcinfo, WMAB_FALSE);
                        HEAP_DEBUG_CHECK;

                        // Effectively disable GetPCM in case we find broken frame or packet
                        // (stupid caller maps WMA_E_LOSTPACKET to WMA_OK and therefore
                        // will try to retrieve the PCM, in which case we must return nothing)
                        pau->m_rgpcinfo[iChannel].m_iCurrGetPCM_SubFrame = CURRGETPCM_INVALID;
                    }

                    BITRATE_REPORT_CHECKPT;
                    paudec->m_decsts = DECODE_SUBFRAME; 
                    paudec->m_subfrmdecsts = SUBFRM_HDR;
                    paudec->m_hdrdecsts = HDR_SIZE;
                    paudec->m_iChannel = 0;  
                    paudec->m_iBand = 0;     

                }
                else 
                    paudec->m_decsts = END_SUBFRAME2;

            case DECODE_SUBFRAME :
                TRACEWMA_EXIT(hr, (*(paudec->m_pfnDecodeSubFrame))(paudec, &cBitUsed));

                HEAP_DEBUG_CHECK;
                BITRATE_REPORT_PRINT;
                if (paudec->m_subfrmdecsts == SUBFRM_DONE)
                    paudec->m_decsts = END_SUBFRAME1;

            case END_SUBFRAME1 :
                auPreGetPCM (pau, pcSampleReady);
                paudec->pau->m_iCurrSubFrame++;

            case END_SUBFRAME2 :
                if (paudec->m_fLastSubFrame) 
                {
                    //take care the end of a frame
                    if (pau->m_fAllowSuperFrame)
                    {
                        //copy next subfrm config to curr
                        memcpy (&pau->m_subfrmconfigPrev, &pau->m_subfrmconfigCurr, sizeof (SubFrameConfigInfo));
                        memcpy (&pau->m_subfrmconfigCurr, &pau->m_subfrmconfigNext, sizeof (SubFrameConfigInfo));
                    }
                    paudec->m_cFrmInPacket--;
                    //start decoding the next frame
                    paudec->m_decsts = BEGIN_FRAME; 

                    if (paudec->m_cFrmInPacket <= 0)
                    {
                        hr = WMA_S_NO_MORE_FRAME;
                        goto exit;
                    }
                }
                else                    
                    paudec->m_decsts = BEGIN_SUBFRAME; //goto the start of next subfrm
                goto exit;

           case BEGIN_PACKET:

                paudec->pau->m_fHeaderReset = WMAB_TRUE;
                assert (paudec->pau->m_fAllowSuperFrame || (paudec->m_cFrmInPacket == 1));
                paudec->m_decsts = BEGIN_FRAME; 

           case BEGIN_FRAME:
                paudec->pau->m_iFrameNumber++;
                paudec->pau->m_iCurrSubFrame = 0;
                paudec->m_fLastSubFrame = WMAB_FALSE;

                paudec->m_decsts = BEGIN_SUBFRAME;
                if (paudec->m_cFrmInPacket == 0)
                {
                    hr = WMA_S_NO_MORE_FRAME;
                    goto exit;
                }
                break;
         }
    }

exit:
    //FUNCTION_PROFILE_STOP(&fp);
#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
        prvWmaShowFrames(paudec->pau, "RetDecD", hr<0 ? " 0x%08x " : " %d ", hr );
#endif
    HEAP_DEBUG_CHECK;

    return hr;
}

WMARESULT audecGetPCM (CAudioObjectDecoder* paudec, U16* pcSampleReady, U8* pbDst, U32 cbDstLength)
{
    WMARESULT wmaResult = WMA_OK;
    
#if 0
    // *** TODO: Enable this code to enhance efficiency.
    // No PCM required (or available) when doing simple transcode
    if (TT_SIMPLE == paudec->pau->m_ttTranscodeType)
    {
        *pcSampleReady = 0;
        return WMA_OK;
    }
#endif

    //get pcm only when we are in these states
    if (paudec->m_decsts == BEGIN_FRAME || paudec->m_decsts == BEGIN_SUBFRAME)
        wmaResult = auGetPCM (paudec->pau, pcSampleReady, pbDst, cbDstLength);
    else
        *pcSampleReady = 0;         //we are in a state of error; don't return anything

#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
    prvWmaShowFrames(paudec->pau, "RetGetP", wmaResult<0 ? " 0x%08x " : " %d ", wmaResult );
#endif
    return wmaResult;
}



//*****************************************************************************************
//
// prvDecodeSubFrame
//
//*****************************************************************************************
WMARESULT prvDecodeSubFrame (CAudioObjectDecoder* paudec, Int* piBitCnt)
{
    WMARESULT hr = WMA_OK;
    Int     i, iChannel;
    CAudioObject* pau = paudec->pau;
    CoefType     ctTemp0, ctTemp1;
    CoefType*    pctCoefRecon0;
    CoefType*    pctCoefRecon1;

#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,DECODE_SUB_FRAME_PROFILE);
#endif

    switch (paudec->m_subfrmdecsts)
    {
        case SUBFRM_HDR :   
            TRACEWMA_EXIT(hr, prvDecodeFrameHeader (paudec, piBitCnt));
            if (paudec->m_hdrdecsts == HDR_DONE)
            {
                paudec->m_subfrmdecsts = SUBFRM_COEF;
                paudec->pau->m_iCurrReconCoef = 0;             //iRecon is offset by -1 so comarison 
                paudec->m_iChannel = 0;
                paudec->m_rlsts = VLC;

            }
            else
                break;

        case SUBFRM_COEF:
            TRACEWMA_EXIT(hr, (*(paudec->m_pfnDecodeCoefficient))(paudec, pau->m_rgpcinfo, piBitCnt));
            paudec->m_subfrmdecsts = SUBFRM_DONE;

        case SUBFRM_DONE:
            // WMA Timestamps: To detect start-of-stream and discard correct amount of silence,
            // we need to verify claim that m_iPower[*] = 1, ForceMaskUpdate and actual power = 0.
            // If m_rgiCoefQ is all 0, we will accept that as actual power = 0 even though it is
            // theoretically possible for actual power != 0 if all bands inside noise-sub band are
            // substituted.
            if (CODEC_BEGIN == pau->m_codecStatus)
            {
                for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
                {
                    PerChannelInfo *ppcinfo = &(pau->m_rgpcinfo[iChannel]);
                    if (pau->m_rgpcinfo[iChannel].m_iPower != 0)
                        SetActualPower (ppcinfo->m_rgiCoefQ, ppcinfo->m_cSubbandActual,
                                       ppcinfo, pau->m_codecStatus);
                }
            }

            pau->m_qstQuantStep = qstCalcQuantStep(pau->m_iQuantStepSize);
            for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) 
            {
                if (pau->m_rgpcinfo[iChannel].m_iPower != 0)
                {
                    TRACEWMA_EXIT(hr, (*pau->aupfnInverseQuantize)(pau, pau->m_rgpcinfo + iChannel,
                        (I32 *)(pau->m_rgpcinfo + iChannel)->m_rguiWeightFactor));
                }
                else
                {
                    memset (pau->m_rgpcinfo [iChannel].m_rgiCoefRecon, 0, sizeof (Int) * DOUBLE(pau->m_fPad2XTransform,pau->m_cSubband));
                }
            }

#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
            if (pau->m_cChannel==1)
                prvWmaShowFrames(pau, "\nDecSub ", " %2d   0x%08x            [%4d %4d %4d]",
                    pau->m_rgpcinfo[0].m_iPower,
                    pau->m_rgpcinfo[0].m_rgiCoefRecon,
                    pau->m_cFrameSample,
                    pau->m_cSubFrameSample,
                    pau->m_cSubband );
            else
                prvWmaShowFrames(pau, "\nDecSub ", " %2d%2d 0x%08x 0x%08x [%4d %4d %4d]",
                    pau->m_rgpcinfo[0].m_iPower,
                    pau->m_rgpcinfo[1].m_iPower,
                    pau->m_rgpcinfo[0].m_rgiCoefRecon,
                    pau->m_rgpcinfo[1].m_rgiCoefRecon,
                    pau->m_cFrameSample,
                    pau->m_cSubFrameSample,
                    pau->m_cSubband );
#endif

            //convert s/d to l/r
            if (pau->m_cChannel == 2 && pau->m_stereoMode == STEREO_SUMDIFF && 
                            (pau->m_rgpcinfo[0].m_iPower != 0 || pau->m_rgpcinfo[1].m_iPower != 0)) {
                pctCoefRecon0 = (CoefType*)pau->m_rgpcinfo[0].m_rgiCoefRecon;
                pctCoefRecon1 = (CoefType*)pau->m_rgpcinfo[1].m_rgiCoefRecon;
                for (i = pau->m_cSubbandAdjusted; i > 0; i--)   
                {
                    ctTemp0 = *pctCoefRecon0;
                    ctTemp1 = *pctCoefRecon1;
                    *pctCoefRecon0++ = ctTemp0 + ctTemp1;
                    *pctCoefRecon1++ = ctTemp0 - ctTemp1;
                }
                pau->m_rgpcinfo[0].m_iPower = 1;
                pau->m_rgpcinfo[1].m_iPower = 1;
            }

            if ( ! pau->m_fNoiseSub )
            {   // V4 only zeroed above HighCutOff when NoiseSubstitution was not in effect - e.g. it only zeros for HighRate
                Int iHighToBeZeroed = sizeof(CoefType) * (pau->m_cSubbandAdjusted - pau->m_cHighCutOffAdjusted);
                for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
                {
                    memset (pau->m_rgpcinfo [iChannel].m_rgiCoefRecon + pau->m_cHighCutOffAdjusted, 0, iHighToBeZeroed);    
#           if defined(_DEBUG) && defined(WMA_MONITOR)
                    {   int ii;
                        for( ii = 0; ii < iHighToBeZeroed; ii += sizeof(Int) )
                            MONITOR_RANGE(gMR_CoefRecon,0);
                    }
#           endif
                }
            }

#ifdef ENABLE_EQUALIZER
            //equalize
            for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) {
                if (pau->m_rgpcinfo[iChannel].m_iPower != 0) {
                    TRACEWMA_EXIT(hr, prvEqualize(paudec, pau->m_rgpcinfo + iChannel));

                    if (paudec->m_fComputeBandPower == WMAB_TRUE)
                        prvComputeBandPower (paudec);
                }
            }
#endif // ENABLE_EQUALIZER

            {
                Float fltAfterScaleFactor = (pau->m_iVersion == 1) ? pau->m_fltDctScale :
                    (Float)(2.0f/pau->m_cSubband);

                for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) {
                    PerChannelInfo* ppcinfo = pau->m_rgpcinfo + iChannel;
#if defined(INTERPOLATED_DOWNSAMPLE)
                    if (paudec->pau->m_fLowPass)
                        auLowPass(paudec->pau, (CoefType*) ppcinfo->m_rgiCoefRecon, pau->m_cSubband / 2);
#endif  // defined(INTERPOLATED_DOWNSAMPLE)
                    if (ppcinfo->m_iPower != 0) 
                        (*pau->aupfnDctIV) (pau, (CoefType*) ppcinfo->m_rgiCoefRecon, fltAfterScaleFactor, NULL);
                }
            }
        }
exit:
    //FUNCTION_PROFILE_STOP(&fp);
    return hr;
}


//*****************************************************************************************
//
// prvUpdateSubFrameConfig
//
//*****************************************************************************************
WMARESULT prvUpdateSubFrameConfig (CAudioObjectDecoder* paudec, Int iSizeNext, Int iSizePrev,
                                 Int iSizeCurr)
{
    Int iStartCurr;
    CAudioObject* pau = paudec->pau;
/*** less than 0.5%
#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,UPDATE_SUB_FRAME_CONFIG_PROFILE);
#endif
*///
    if (iSizeNext == 0) {
        assert (pau->m_subfrmconfigCurr.m_cSubFrame > 0);
        paudec->m_fLastSubFrame = WMAB_TRUE;
        return WMA_OK;
    }

    //only useful in case of packet loss and prev info lost or first in sequece
    if (iSizePrev != 0) {
        assert (pau->m_iCurrSubFrame == 0);
        pau->m_subfrmconfigPrev.m_cSubFrame = 1;
        pau->m_subfrmconfigPrev.m_rgiSubFrameSize [0] = iSizePrev;
    }

    //first frame in super and first sub frame; init
    if (iSizeCurr != 0) {
        assert (pau->m_iCurrSubFrame == 0);
        pau->m_subfrmconfigCurr.m_cSubFrame = 0;
        pau->m_subfrmconfigCurr.m_rgiSubFrameSize [0] = iSizeCurr;
        pau->m_subfrmconfigCurr.m_rgiSubFrameStart[0] = 0;
        pau->m_subfrmconfigCurr.m_cSubFrame++;
    }

    iStartCurr = pau->m_subfrmconfigCurr.m_rgiSubFrameStart [pau->m_iCurrSubFrame] 
                   + pau->m_subfrmconfigCurr.m_rgiSubFrameSize  [pau->m_iCurrSubFrame];
    if (iStartCurr >= pau->m_cFrameSampleHalf) {
        //init the next frame
        paudec->m_fLastSubFrame = WMAB_TRUE;
        pau->m_subfrmconfigNext.m_cSubFrame = 0;
        pau->m_subfrmconfigNext.m_rgiSubFrameSize [0] = iSizeNext;
        pau->m_subfrmconfigNext.m_rgiSubFrameStart[0] = 0;
        pau->m_subfrmconfigNext.m_cSubFrame++;
    }
    else {
        //must be within one frame; must have received the first one
        Int iCurr = pau->m_iCurrSubFrame;
        assert (pau->m_subfrmconfigCurr.m_cSubFrame == iCurr + 1);
        assert (iSizeNext < pau->m_cFrameSampleHalf);        //> 1 subfrm
        pau->m_subfrmconfigCurr.m_rgiSubFrameSize [iCurr + 1] = iSizeNext;
        pau->m_subfrmconfigCurr.m_rgiSubFrameStart[iCurr + 1] = 
            pau->m_subfrmconfigCurr.m_rgiSubFrameStart[iCurr] + 
            pau->m_subfrmconfigCurr.m_rgiSubFrameSize[iCurr];
        if (pau->m_subfrmconfigCurr.m_rgiSubFrameStart[iCurr + 1] + 
            pau->m_subfrmconfigCurr.m_rgiSubFrameSize [iCurr + 1] > pau->m_cFrameSampleHalf)  {
            assert (WMAB_FALSE);
            return TraceResult(WMA_E_BROKEN_FRAME);
        }
        pau->m_subfrmconfigCurr.m_cSubFrame++;
    }
/***
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
*///
    return WMA_OK;
}

//*****************************************************************************************
//
// prvDecodeSubFrameHighRate
//
//*****************************************************************************************
WMARESULT prvDecodeSubFrameHighRate (CAudioObjectDecoder* paudec, Int* piBitCnt)
{
    WMARESULT hr = WMA_OK;
    Int     iChannel,i;
    PerChannelInfo* ppcinfo;
    CoefType *pctCoefRecon0,*pctCoefRecon1, ctTemp0, ctTemp1;
    CAudioObject* pau = paudec->pau;

#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,DECODE_SUB_FRAME_HIGH_RATE_PROFILE);
#endif  // PROFILE

    assert(0 == *piBitCnt); // Need this assumption, storing bit allocs for transcoding

    switch (paudec->m_subfrmdecsts)
    {
        case SUBFRM_HDR :   
            TRACEWMA_EXIT(hr, prvDecodeFrameHeader(paudec, piBitCnt));    
            pau->m_qstQuantStep = qstCalcQuantStep(pau->m_iQuantStepSize);
            if (paudec->m_hdrdecsts == HDR_DONE)
            {
                I16 iChannel;
                for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
                {
                    ppcinfo = pau->m_rgpcinfo + iChannel;
                    memset (ppcinfo->m_rgiCoefRecon, 0, sizeof (CoefType) * pau->m_cHighCutOff);
                }
                paudec->m_subfrmdecsts = SUBFRM_COEF;

                //iRecon is offset by -1 so comarison 
                paudec->pau->m_iCurrReconCoef = (I16) (pau->m_cLowCutOff-1);

                paudec->m_iChannel = 0;
                paudec->m_rlsts = VLC;
            }
            else
                break;

        case SUBFRM_COEF:
            //Decode coefficents for sum channel or left & right channels 
            for (; paudec->m_iChannel < pau->m_cChannel; paudec->m_iChannel++) 
            {
                ppcinfo = pau->m_rgpcinfo + paudec->m_iChannel;

                if (ppcinfo->m_iPower != 0)
                {
                    TRACEWMA_EXIT(hr, auReconCoefficentsHighRate (paudec->pau,
                        paudec, ppcinfo, piBitCnt));
                }

                if(pau->m_iVersion == 1) 
                {
                    *piBitCnt += ibstrmBitsLeft (&paudec->m_ibstrm) % 8;
                    ibstrmFlush(&paudec->m_ibstrm);
                }
                //usually only go up to highCutoff but certain V1 contents do go up as high as m_cSubband
                assert (paudec->pau->m_iCurrReconCoef <= (I16) pau->m_cSubband);
                paudec->pau->m_iCurrReconCoef = (I16) (pau->m_cLowCutOff-1);             //iRecon is offset by -1 so comarison 
            }
            paudec->m_subfrmdecsts = SUBFRM_DONE;

        case SUBFRM_DONE:
            for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) {
                ppcinfo = pau->m_rgpcinfo + iChannel;
                if (ppcinfo->m_iPower != 0)
                {
                    // WMA Timestamps: To detect start-of-stream and discard correct amount of silence,
                    // we need to verify claim that m_iPower[*] = 1, ForceMaskUpdate and actual power = 0.
                    ppcinfo->m_iActualPower = 0;
                    if (CODEC_BEGIN == pau->m_codecStatus)
                    {
                        SetActualPowerHighRate (ppcinfo->m_rgiCoefRecon, 
                                                pau->m_cSubband, ppcinfo, pau->m_codecStatus);
                    }
                }
                else 
                {
                    memset (ppcinfo->m_rgiCoefRecon, 0, sizeof (CoefType) * pau->m_cSubbandAdjusted);
                    ppcinfo->m_iActualPower = 0;
                }
            }
#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
            if (pau->m_cChannel==1)
                prvWmaShowFrames(pau, "\nDecSubH", " %2d   0x%08x            [%4d %4d %4d]",
                    pau->m_rgpcinfo[0].m_iPower,
                    pau->m_rgpcinfo[0].m_rgiCoefRecon,
                    pau->m_cFrameSample,
                    pau->m_cSubFrameSample,
                    pau->m_cSubband );
            else
                prvWmaShowFrames(pau, "\nDecSubH", " %2d%2d 0x%08x 0x%08x [%4d %4d %4d]",
                    pau->m_rgpcinfo[0].m_iPower,
                    pau->m_rgpcinfo[1].m_iPower,
                    pau->m_rgpcinfo[0].m_rgiCoefRecon,
                    pau->m_rgpcinfo[1].m_rgiCoefRecon,
                    pau->m_cFrameSample,
                    pau->m_cSubFrameSample,
                    pau->m_cSubband );
#endif

            //convert s/d to l/r
            if (pau->m_cChannel == 2 && pau->m_stereoMode == STEREO_SUMDIFF && 
                            (pau->m_rgpcinfo[0].m_iPower != 0 || pau->m_rgpcinfo[1].m_iPower != 0)) {
                pctCoefRecon0 = (CoefType*)pau->m_rgpcinfo[0].m_rgiCoefRecon;
                pctCoefRecon1 = (CoefType*)pau->m_rgpcinfo[1].m_rgiCoefRecon;
                for (i = pau->m_cHighCutOffAdjusted; i >0; i--) {
                    ctTemp0 = *pctCoefRecon0;
                    *pctCoefRecon0++ = ctTemp0 + (ctTemp1 = *pctCoefRecon1);
                    *pctCoefRecon1++ = ctTemp0 - ctTemp1;
                }
                pau->m_rgpcinfo[0].m_iPower = 1;
                pau->m_rgpcinfo[1].m_iPower = 1;
                assert( pctCoefRecon0 == (CoefType*)pau->m_rgpcinfo [0].m_rgiCoefRecon + pau->m_cHighCutOffAdjusted );
                assert( pctCoefRecon1 == (CoefType*)pau->m_rgpcinfo [1].m_rgiCoefRecon + pau->m_cHighCutOffAdjusted );
            }

#ifdef ENABLE_EQUALIZER
            //equalize
            for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) {
                if (pau->m_rgpcinfo[iChannel].m_iPower != 0) {
                    TRACEWMA_EXIT(hr, prvEqualize(paudec, pau->m_rgpcinfo + iChannel));

                    if (paudec->m_fComputeBandPower == WMAB_TRUE)
                        prvComputeBandPower (paudec);
                }
            }
#endif // ENABLE_EQUALIZER
    
            {
                Float fltAfterScaleFactor = (pau->m_iVersion == 1) ? pau->m_fltDctScale :
                    (Float)(2.0f/pau->m_cSubband);

                for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) {
                    PerChannelInfo* ppcinfo = pau->m_rgpcinfo + iChannel;
#if defined(INTERPOLATED_DOWNSAMPLE)
                    if (paudec->pau->m_fLowPass)
                        auLowPass(paudec->pau, (CoefType*) ppcinfo->m_rgiCoefRecon, pau->m_cSubband / 2);
#endif  // defined(INTERPOLATED_DOWNSAMPLE)
                    if (ppcinfo->m_iPower != 0) 
                        (*pau->aupfnDctIV) (pau, (CoefType*) ppcinfo->m_rgiCoefRecon, fltAfterScaleFactor, NULL);
                }
            }
    }

exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif  // PROFILE

    return hr;
}



//*****************************************************************************************
//
// Std WMA_GetMoreData for non-streaming mode
//
//*****************************************************************************************
WMARESULT prvWMAGetMoreData (U8 **ppBuffer, U32 *pcbBuffer,
                             U32 dwUserData)
{
    CAudioObjectDecoder* paudec = (CAudioObjectDecoder*) dwUserData;

    if(paudec == NULL || ppBuffer == NULL || pcbBuffer == NULL)
    {
        if(ppBuffer != NULL)
            *ppBuffer = NULL;
        if(pcbBuffer != NULL)
            *pcbBuffer = 0;

        assert(WMAB_FALSE);
        return TraceResult(WMA_E_INVALIDARG);
    }

    if (paudec->m_pbSrcCurr != NULL)
    {
        *ppBuffer  = paudec->m_pbSrcCurr;
        *pcbBuffer = paudec->m_cbSrcCurrLength;
        //make sure nothing is left
        paudec->m_pbSrcCurr = NULL;
        paudec->m_cbSrcCurrLength = 0;
    }
    return WMA_S_NEWPACKET;
}


//***************************************************************************
// Encoder/Decoder Separation
//***************************************************************************

FastFloat prvCalcSqrtBWRatioDEC(const PerChannelInfo *ppcinfo,
                            const Int iNoiseBand,
                            const Int cNoiseBand)
{
    const FastFloat* const rgffltSqrtBWRatio = ppcinfo->m_rgffltSqrtBWRatio;
    return rgffltSqrtBWRatio[iNoiseBand];
} // prvCalcSqrtBWRatioENC


// Start BLOCK B
// From lpc.c

// Forward decl
void gLZLTableInit(void);

#ifdef BUILD_INTEGER

// INVQUADROOT_FRACTION_TABLE_SIZE defined as 256 and interpolate
//UInt g_InvQuadRootFraction[1+INVQUADROOT_FRACTION_TABLE_SIZE];

//UInt g_InvQuadRootExponent[BITS_LP_SPEC_POWER];  // was [32-PRECESSION_BITS_FOR_INVQUADROOT]

Void prvInitInverseQuadRootTable (CAudioObject* pau)
{
#if 0
    Int i;
    
    for (i = 1; i < 1<<(PRECESSION_BITS_FOR_INVQUADROOT); i++){          
        Float f = (float)i/LP_SPEC_POWER_SCALE;
        Float invQuadRoot = (Float)(1/sqrt(sqrt(f)));                                   
        //// range is 11.3137:0.0525556 or in bits +3.5:-4.25
        // When PRECESSION_BITS_FOR_INVQUADROOT = 14, LP_SPEC_POWER_SCALE = 28, range is 128.000000:11.313881 ==> QR_FRAC = 32-8
        // When PRECESSION_BITS_FOR_INVQUADROOT = 14, LP_SPEC_POWER_SCALE = 30, range is 181.019333:16.000244 ==> QR_FRAC = 32-9
        // When PRECESSION_BITS_FOR_INVQUADROOT = 14, LP_SPEC_POWER_SCALE = 31, range is 215.269482:19.027604 ==> QR_FRAC = 32-8
        g_InvQuadRootFraction[i] = (UInt)(invQuadRoot*(1<<QR_FRACTION_FRAC_BITS));
    }
    g_InvQuadRootFraction[0] = (UInt)((1/(sqrt(sqrt(1.0/LP_SPEC_SCALE/2))))*(1<<QR_FRACTION_FRAC_BITS));
    
    for (i = -PRECESSION_BITS_FOR_INVQUADROOT; i < BITS_LP_SPEC_POWER-PRECESSION_BITS_FOR_INVQUADROOT; i++){
        Float invQuadRoot = (i<0) ? (float)pow(2,i) : (float)(1<<i);
                invQuadRoot = (Float)(1/sqrt(sqrt(invQuadRoot)));                               
                // range is 45.2548:3.36364 or in bits +5.5:1.75 for 32 bit power. 
        g_InvQuadRootExponent[i+PRECESSION_BITS_FOR_INVQUADROOT] = (UInt)(invQuadRoot*(1<<QR_EXPONENT_FRAC_BITS));
    }
        // range of product is 511.999:0.176778 or +9.0:-2.5
        // When PRECESSION_BITS_FOR_INVQUADROOT = 14, BITS_LP_SPEC_POWER = 64, range is 1.000000:0.005524 ==> QR_EXP = 32-2
        // So value should work with a LP_SPEC_FRAC_BITS of 22.
#endif
    SETUP_LPC_SPEC_TRIG(pau);
#ifndef LPCPOWER32

	gLZLTableInit();
#endif
}

//Debugging aid to dealwith LPC calculations separately from InverseQuadRoot
//#define FAKE_INVERSE_QUAD_ROOT


INLINE WeightType InverseQuadRoot(LpSpecPowerType f)
{
    //// 1.0f/(1<<30)
    WeightType uiResult;
    LpSpecPowerType uFrac;
    Int iExp;// can be -ve
    UInt uiExpVal;
    UInt uiTmp;
    
    uFrac = f;
    // Get normalized fractional portion with PRECESSION_BITS_FOR_INVQUADROOT
    // bits and corresponding exponent value
    if (uFrac == 0) {
      iExp = 0;
    } else {
      // Find the most significant bit with value 1.
#if defined (LPCPOWER32)
      // 32-bit input
      iExp = 0;
      uiTmp = uFrac;
#else 
      // 64-bit input: Look at each 32-bit portion
      // We are assuming that PRECESSION_BITS_FOR_INVQUADROOT is < 32.
      if ((uFrac >> 32) == 0) {
        iExp = 32;
        uiTmp = (UInt) uFrac;
      } else {
        iExp = 0;
        uiTmp = (UInt)(uFrac >> 32);
      }
#endif
      while (!(uiTmp & 0x80000000)) { uiTmp <<= 1; iExp++;};
      
      // Perform shifts on the input so that we have 
      // PRECESSION_BITS_FOR_INVQUADROOT bits of normalized 
      // fraction, and corresponding exponent values.
      
      uFrac = f;
      iExp = BITS_LP_SPEC_POWER - iExp - PRECESSION_BITS_FOR_INVQUADROOT;
      if (iExp < 0) 
        uFrac <<= (-iExp);
      if (iExp > 0)
        uFrac >>= iExp;
    }
    
    // QR_*_FRAC_BITS: same as used in prvInitInverseQuadRootTable
    MONITOR_COUNT(gMC_ScaleInverseQuadRoot,iExp);
    uFrac = g_InvQuadRootFraction[(Int)uFrac];
    //// iFrac*1.0f/(1<<23)
    uiExpVal = g_InvQuadRootExponent[iExp+PRECESSION_BITS_FOR_INVQUADROOT];
    //// iExp*1.0f/(1<<29)
    uiResult = MULT_HI_UDWORD(((UInt)uFrac),uiExpVal);  // frac_bits = 23+29-32 = 20
    //// iResult*1.0f/(1<<20)
#if ((QR_FRACTION_FRAC_BITS+QR_EXPONENT_FRAC_BITS-32) > WEIGHTFACTOR_FRACT_BITS)
    uiResult >>= (QR_FRACTION_FRAC_BITS+QR_EXPONENT_FRAC_BITS-31) - WEIGHTFACTOR_FRACT_BITS;
#elif ((QR_FRACTION_FRAC_BITS+QR_EXPONENT_FRAC_BITS-32) < WEIGHTFACTOR_FRACT_BITS)
    uiResult <<= WEIGHTFACTOR_FRACT_BITS - (QR_FRACTION_FRAC_BITS+QR_EXPONENT_FRAC_BITS-31);
#endif //((QR_FRACTION_FRAC_BITS+QR_EXPONENT_FRAC_BITS-32) > WEIGHTFACTOR_FRACT_BITS)
    // uiResult = (2^19/QR(iFrac*2^-19)) * 2^6 * (2^19/QR(2^iExp)) * 2^7 * 2^-32 = 2^19/QR(iFrac*2^-19*2^iExp)
    // uiResult = MULT_HI_DWORD(iFrac<<(BITS_PER_DWORD-FRACT_BITS_LP_SPEC>>1),(iExp<<((BITS_PER_DWORD-FRACT_BITS_LP_SPEC>>1)+1)));
    assert( uiResult >= 0 );
    
    //fprintf(stdout, "%lf\n", (double)uiResult/(1<<WEIGHTFACTOR_FRACT_BITS));
    return uiResult;
}
#endif  // BUILD_INTEGER
// End BLOCK B



// Decoder-only portion of auAdaptToSubFrameConfig
WMARESULT auAdaptToSubFrameConfigDEC (CAudioObject* pau)
{
    PerChannelInfo *ppcinfo = pau->m_rgpcinfo;

    ppcinfo->m_rgiCoefRecon   = pau->m_rgiCoefReconOrig 
                              + DOUBLE(pau->m_fPad2XTransform,
                                    pau->m_cFrameSampleHalf - pau->m_cSubFrameSampleHalf);
    ppcinfo->m_rgfltCoefRecon = (Float *)(ppcinfo->m_rgiCoefRecon);

    if (pau->m_cChannel == 2)   {
        ppcinfo = pau->m_rgpcinfo + 1;

        ppcinfo->m_rgiCoefRecon   = pau->m_rgiCoefReconOrig 
                                  + DOUBLE(pau->m_fPad2XTransform,
                                        2 * pau->m_cFrameSampleHalf - pau->m_cSubFrameSampleHalf);
        ppcinfo->m_rgfltCoefRecon = (Float *)(ppcinfo->m_rgiCoefRecon);
    }

    return WMA_OK;
}
