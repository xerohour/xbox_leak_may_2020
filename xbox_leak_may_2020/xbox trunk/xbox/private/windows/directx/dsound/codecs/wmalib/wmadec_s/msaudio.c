/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudio.cpp

Abstract:

    Implementation of public member functions for CAudioObject.

Author:

    Wei-ge Chen (wchen) 11-March-1998

Revision History:


*************************************************************************/

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

// Print out the target we're building for
#define COMMONMACROS_OUTPUT_TARGET

#if !defined (_WIN32_WCE) && !defined (HITACHI)
#include <time.h>
#endif  // _WIN32_WCE

#include <math.h>
#include <limits.h>
#include <stdio.h>
#include "msaudio.h"
#include "AutoProfile.h"
#include "macros.h"
#include "float.h"
#include "cpudetect.h"
#include "wmamath.h"
#include "..\..\..\dsound\dsndver.h"

//*****************************************************************************************
// Forward Function Declarations
//*****************************************************************************************

void prvInitDiscardSilence(CAudioObject *pau);
void prvDiscardSilence(CAudioObject *pau, U16 *pcSampleDecoded,
                       U16 iChannels, U8 *pbBuf);
void prvSetAdjustedValues(CAudioObject *pau, Int fQToo);
void prvFFT4DCT(CoefType data[], Int nLog2np);


//*****************************************************************************************
//
// msaudioGetSamplePerFrame
//
//*****************************************************************************************
I32 msaudioGetSamplePerFrame (Int   cSamplePerSec, 
                              U32   dwBitPerSec, 
                              Int   cNumChannels,
                              Int   iVersion) 
{
    //return NEW samples coming into a frame; actual samples in a frame
    //should be * 2 due to 50% overlapping window
    I32 cSamplePerFrame;
    U32 dwBytesPerFrame;

        //don't know what to do
    if (dwBitPerSec == 0 || iVersion > 2)
        return 0;

    if (cSamplePerSec <= 8000)
        cSamplePerFrame = 512;
    else if (cSamplePerSec <= 11025)
        cSamplePerFrame = 512;
    else if (cSamplePerSec <= 16000) {
        cSamplePerFrame = 512;
    }
    else if (cSamplePerSec <= 22050) {
        cSamplePerFrame = 1024;
    }
    else if (cSamplePerSec <= 32000) {
        if(iVersion == 1)
            cSamplePerFrame = 1024;
        else
            cSamplePerFrame = 2048;
    }
    else if (cSamplePerSec <= 44100)
        cSamplePerFrame = 2048;
    else if (cSamplePerSec <= 48000)
        cSamplePerFrame = 2048;
    else
        return 0;
    dwBytesPerFrame = (U32) (((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
    if ( dwBytesPerFrame==0 && (cSamplePerFrame*dwBitPerSec) == 0 ) {
        // this can happen when garbage data sets dwBitsPerSec to a very large number
        // avoid an infinite loop below
        dwBitPerSec = cSamplePerSec;
        dwBytesPerFrame = (U32) (((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
    }
    if (dwBytesPerFrame <= 1) {   //silence mode
        while (dwBytesPerFrame == 0) {
            cSamplePerFrame *= 2;           //save more bits; quartz can't take too big a value
            dwBytesPerFrame = (U32) (((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
        }
    }
    return cSamplePerFrame;
}

//*****************************************************************************************
//
// MaxSamplesPerPacket
//   Returns the largest possible number of PCM samples that the decoder can produce
//   from a single compressed packet.  
//
//*****************************************************************************************
U32 MaxSamplesPerPacket(U32 ulVersion, U32 ulSamplingRate, U32 ulChannels, U32 ulBitrate) {
   return (16 + 1) * msaudioGetSamplePerFrame(ulSamplingRate,ulBitrate,ulChannels,ulVersion);
}

//*****************************************************************************************
//
// mallocAligned & freeAligned
// allocates a buffer of size bytes aligned to iAlignToBytes bytes.
//
//*****************************************************************************************
void *mallocAligned(size_t size,Int iAlignToBytes){
    Int mask = -1;                                    //Initally set mask to 0xFFFFFFFF
    void *retBuffer;
    void *buffer = auMalloc(size+iAlignToBytes);        //allocate buffer + alignment bytes
    if(buffer == NULL)
    {
        TraceResult(WMA_E_OUTOFMEMORY);
        return NULL;
    }
    assert(iAlignToBytes > 0);
    assert(iAlignToBytes < 256);
    mask <<= LOG2(iAlignToBytes);                     //Generate mask to clear lsb's
    retBuffer = (void*)(((Int)((U8*)buffer+iAlignToBytes))&mask);//Generate aligned pointer
    ((U8*)retBuffer)[-1] = (U8)((U8*)retBuffer-(U8*)buffer);//Write offset to newPtr-1
    return retBuffer;
}

void freeAligned(void *ptr){
    U8* realBuffer = (U8*)ptr;
    U8 bytesBack;
    if (realBuffer == NULL) return; 
    bytesBack = ((U8*)ptr)[-1];      //Get offset to real pointer from -1 possition
    realBuffer -= bytesBack;    //Get original pointer address
    free(realBuffer);
}



// ----- Memory Allocation Functions -----
#ifdef STATIC_MEMALLOC
// Global vars
static MEMALLOCSTATE    g_maState = MAS_DELETE;
static U8              *g_pBuffer = NULL;
static I32              g_iBufRemaining = 0;
static I32              g_iAllocCount = 0;

I32 auMallocGetCount(void)
{
    return g_iAllocCount;
}


void auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const I32 iBufSize)
{
    switch (maState)
    {
        case MAS_ALLOCATE:
            assert(MAS_DELETE == g_maState);
            g_pBuffer = (U8 *) pBuf;
            g_iBufRemaining = iBufSize;
            g_maState = MAS_ALLOCATE;
            break;

        case MAS_LOCKED:
            assert(MAS_ALLOCATE == g_maState);
            g_maState = MAS_LOCKED;
            break;

        case MAS_DELETE:
            g_maState = MAS_DELETE;
            break;

        default:
            assert(WMAB_FALSE);
            break;
    }
}


void *auMalloc(const size_t iSize)
{
    WMARESULT   wmaResult = WMA_OK;
    void       *pResult = NULL;

    switch (g_maState)
    {
        case MAS_ALLOCATE:

            if (NULL == g_pBuffer)
            {
                pResult = malloc(iSize);
                if (NULL == pResult)
                    wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
            }
            else
            {
                if (iSize <= (U32) g_iBufRemaining)
                {
                    pResult = g_pBuffer;
                    g_pBuffer += iSize;
                    g_iBufRemaining -= iSize;
                }
                else
                {
                    wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
                    assert(WMAB_FALSE);
                }
            }

            g_iAllocCount += iSize;
            break;

        default:
            wmaResult = TraceResult(WMA_E_FAIL);
            assert(WMAB_FALSE); // We should only be called during allocation
            break;
    }

    return pResult;
}

void auFree(void *pFree)
{
    assert(MAS_DELETE == g_maState);
    if (NULL == g_pBuffer)
        free(pFree);
}
#else   // STATIC_MEMALLOC

I32 auMallocGetCount(void)
{
    return 0;
}

void auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const I32 iBufSize)
{
}

void *auMalloc(const size_t iSize)
{
    return malloc(iSize);
}

void auFree(void *pFree)
{
    free(pFree);
}

#endif  // STATIC_MEMALLOC


//*****************************************************************************************
//
// auNew
// create and initialize a CAudioObject
//
//*****************************************************************************************
CAudioObject* auNew ()
{
    CAudioObject* pau = (CAudioObject*) auMalloc (sizeof (CAudioObject));
    if ( pau == NULL )
    {
        TraceResult(WMA_E_OUTOFMEMORY);
        return pau;
    }

    memset(&pau->m_qstQuantStep, 0, sizeof(pau->m_qstQuantStep)); // May be struct or float/double
    // memset(pau->m_rgiQuantStepFract, 0, sizeof(pau->m_rgiQuantStepFract));
    pau->m_iPacketCurr = 0;
    pau->m_cBitPackedFrameSize = 0;
    pau->m_cBitPacketHeader = 0;
    pau->m_cdwPacketHeader = 0;
    pau->m_cBitPacketHeaderFractionDw = 0;
    pau->m_cBitPacketLength = 0;
    pau->m_cRunOfZeros = 0;
    pau->m_iLevel = 0; 
    pau->m_iSign = 0;
    pau->m_iHighCutOffCurr = 0;
    pau->m_iNextBarkIndex = 0;

    pau->m_fNoiseSub = WMAB_FALSE; 
    pau->m_fltBitsPerSample = 0;
    pau->m_fltWeightedBitsPerSample = 0;

    pau->m_iMaxEscSize = 9;
    pau->m_iMaxEscLevel = (1 << pau->m_iMaxEscSize) - 1;

    pau->m_iVersion = 0;
    pau->m_codecStatus = CODEC_NULL;
    pau->m_fSeekAdjustment = WMAB_FALSE;
    pau->m_fPacketLossAdj = WMAB_FALSE;
    pau->m_iSamplingRate = 0;
    pau->m_cChannel = 0;
    pau->m_cSubband = 0;

    pau->m_fAllowSuperFrame = WMAB_FALSE;
    pau->m_fAllowSubFrame = WMAB_FALSE;
    pau->m_iCurrSubFrame = 0;
    pau->m_fHeaderReset = WMAB_TRUE;
    pau->m_iSubFrameSizeWithUpdate = 0;
    pau->m_iMaxSubFrameDiv = 1;
    pau->m_cMinSubFrameSample = 0;
    pau->m_cMinSubFrameSampleHalf = 0;
    pau->m_cMinSubFrameSampleQuad = 0;
    pau->m_cPossibleWinSize = 0;
    pau->m_iIncr = 0;
    pau->m_cSubFrameSample = 0;
    pau->m_cSubFrameSampleHalf = 0;
    pau->m_cSubFrameSampleQuad = 0;
    memset(&pau->m_subfrmconfigPrev, 0, sizeof(pau->m_subfrmconfigPrev));
    memset(&pau->m_subfrmconfigCurr, 0, sizeof(pau->m_subfrmconfigCurr));
    memset(&pau->m_subfrmconfigNext, 0, sizeof(pau->m_subfrmconfigNext));

    pau->m_cBitsSubbandMax = 0;
    pau->m_cFrameSample = 0;
    pau->m_cFrameSampleHalf = 0;
    pau->m_cFrameSampleQuad = 0;

    pau->m_cLowCutOff = 0;
    pau->m_cHighCutOff = 0;
    pau->m_cLowCutOffLong = 0;
    pau->m_cHighCutOffLong = 0;

    pau->m_iWeightingMode = LPC_MODE;
    pau->m_stereoMode = STEREO_LEFTRIGHT;
    pau->m_iEntropyMode = 0;
    pau->m_fltDitherLevel = 0.04F;
    pau->m_iQuantStepSize = (MIN_QUANT + MAX_QUANT - 1) / 2;
    pau->m_fltFlatenFactor = 0.5F;
    pau->m_fltDctScale = 0;
    pau->m_cValidBarkBand = 0;
    pau->m_rgiBarkIndex = NULL;

#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
    pau->m_iSinRampUpStart = 0;
    pau->m_iCosRampUpStart = 0;
    pau->m_iSinRampUpPrior = 0;
    pau->m_iCosRampUpPrior = 0;
    pau->m_iSinRampUpStep = 0;
    pau->m_iSinRampDownStart = 0;
    pau->m_iCosRampDownStart = 0;
    pau->m_iSinRampDownPrior = 0;
    pau->m_iCosRampDownPrior = 0;
    pau->m_iSinRampDownStep = 0;
    memset(pau->m_iSinInit, 0, sizeof(pau->m_iSinInit));
    memset(pau->m_iCosInit, 0, sizeof(pau->m_iCosInit));
#endif

#if !defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
    pau->m_fltSinRampUpStart = 0;
    pau->m_fltCosRampUpStart = 0;
    pau->m_fltSinRampUpPrior = 0;
    pau->m_fltCosRampUpPrior = 0;
    pau->m_fltSinRampUpStep = 0;
    pau->m_fltSinRampDownStart = 0;
    pau->m_fltCosRampDownStart = 0;
    pau->m_fltSinRampDownPrior = 0;
    pau->m_fltCosRampDownPrior = 0;
    pau->m_fltSinRampDownStep = 0;
#endif
#ifdef USE_SIN_COS_TABLES
    //MMX tables
    pau->m_piSinForRecon2048 = NULL;
    pau->m_piSinForRecon1024 = NULL;
    pau->m_piSinForRecon512 = NULL;
    pau->m_piSinForRecon256 = NULL;
    pau->m_piSinForRecon128 = NULL;
    pau->m_piSinForRecon64 = NULL;
    pau->m_piSinForRecon = NULL;
    pau->m_piSinForSaveHistory = NULL;
#endif

    pau->m_iSizePrev = 0;
    pau->m_iSizeCurr = 0;
    pau->m_iSizeNext = 0;

    pau->m_iCoefRecurQ1 = 0;
    pau->m_iCoefRecurQ2 = 0;
    pau->m_iCoefRecurQ3 = 0;
    pau->m_iCoefRecurQ4 = 0;

    pau->m_rgiCoefQ = NULL;

    pau->m_rgpcinfo = NULL;
    pau->m_rgiCoefReconOrig = NULL;
    pau->m_rgiMaskQ = NULL;
    pau->m_rgcValidBarkBand = NULL;
    pau->m_rgiBarkIndexOrig = NULL;


    pau->m_piPrevOutput = NULL;
    pau->m_iDiscardSilence = 0;

#ifdef TRANSCODER
    pau->m_ttTranscodeType = TT_NONE;
#endif  // TRANSCODER

#ifdef ENABLE_ALL_ENCOPT
    pau->m_fltFirstNoiseFreq = 0;
    pau->m_iFirstNoiseBand = 0;
    pau->m_iFirstNoiseIndex = 0;
    pau->m_iNoisePeakIgnoreBand = 1;

    pau->m_rgiFirstNoiseBand = NULL;
    pau->m_rgbBandNotCoded = NULL;
    pau->m_rgffltSqrtBWRatio = NULL;
    pau->m_rgiNoisePower = NULL;
    pau->m_rgfltBandWeight = NULL;

#if defined(_DEBUG) && defined(LPC_COMPARE)
    memset(pau->m_rgfltLpsSpecCos, 0, sizeof(pau->m_rgfltLpsSpecCos));
    memset(pau->m_rgfltLpsSpecSin, 0, sizeof(pau->m_rgfltLpsSpecSin));
#endif  // _DEBUG
#endif  // ENABLE_ALL_ENCOPT

    pau->m_rgfltWeightFactor = NULL;
    pau->m_rguiWeightFactor = NULL;

    pau->m_iFrameNumber = 0;

#if defined(INTERPOLATED_DOWNSAMPLE)
    pau->m_fLowPass = WMAB_FALSE;
    pau->m_fDownsample = WMAB_FALSE;
    pau->m_iInterpolSrcBlkSize = 0;
    pau->m_iInterpolDstBlkSize = 0;
    pau->m_iInterpolIncr = 0;
    pau->m_iInterpolCarry = 0;
    pau->m_iInterpolWeightIncr = 0;
    pau->m_iInterpolAddInterval = 0;
    pau->m_iInterpolNextAdd = 0;
#endif  // defined(INTERPOLATED_DOWNSAMPLE)

#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
    pau->m_fHalfTransform = WMAB_FALSE;
    pau->m_fUpsample = WMAB_FALSE;
    pau->m_fPad2XTransform = WMAB_FALSE;
    memset(pau->m_iPriorSample, 0, sizeof(pau->m_iPriorSample));
    //pau->m_htiHalfPrevState.m_fSaved = WMAB_FALSE;
    //pau->m_htiPad2XPrevState.m_fSaved = WMAB_FALSE;
    pau->m_cSubbandAdjusted = 0;
    pau->m_cFrameSampleAdjusted = 0; 
    pau->m_cFrameSampleHalfAdjusted = 0;
    pau->m_cSubFrameSampleAdjusted = 0;
    pau->m_cSubFrameSampleHalfAdjusted = 0;
    pau->m_cSubFrameSampleQuadAdjusted = 0;
    pau->m_cHighCutOffAdjusted = 0;
#endif  // defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)

	pau->m_fIntelFFT = WMAB_FALSE;

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)
    pau->m_fIntelFFT = WMAB_FALSE;
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)

#ifdef ENABLE_ALL_ENCOPT
    pau->aupfnInverseQuantize = prvInverseQuantizeMidRate;
#else
    pau->aupfnInverseQuantize = NULL;
#endif
    pau->prvpfnInverseTransformMono = NULL; // Not currently used
    pau->aupfnGetNextRun = NULL;

	RandStateClear(&(pau->m_tRandState));

    // OK, now verify the sizes of all fundamental type definitions
#ifndef __QNX__
    assert(sizeof(U64) == 8);
    assert(sizeof(I64) == 8);
#endif
    assert(sizeof(U32) == 4);
    assert(sizeof(I32) == 4);
    assert(sizeof(U16) == 2);
    assert(sizeof(I16) == 2);
    assert(sizeof(U8) == 1);
    assert(sizeof(I8) == 1);

    return pau;
}


//*****************************************************************************************
//
// auDelete
// delete a CAudioObject
//
//*****************************************************************************************
Void    auDelete (CAudioObject* pau)
{
    DELETE_ARRAY (pau->m_rgpcinfo);

    DELETE_ARRAY (pau->m_piPrevOutput);

    {
        freeAligned(pau->m_rgiCoefReconOrig);
        DELETE_ARRAY (pau->m_rguiWeightFactor);
        pau->m_rgfltWeightFactor = NULL;
    }
    DELETE_ARRAY (pau->m_rgcValidBarkBand);
    DELETE_ARRAY (pau->m_rgiBarkIndexOrig);
    DELETE_ARRAY (pau->m_rgiMaskQ);

#ifdef ENABLE_ALL_ENCOPT
    {
#if defined(_DEBUG) && defined(LPC_COMPARE)
        Int i;
        for (i = 0; i < LPCORDER; i++)  {
            if ( pau->m_rgfltLpsSpecCos[i] != NULL )
                DELETE_ARRAY (pau->m_rgfltLpsSpecCos[i]);
            if ( pau->m_rgfltLpsSpecSin[i] != NULL )
                DELETE_ARRAY (pau->m_rgfltLpsSpecSin[i]);
        }
#endif
        DELETE_ARRAY (pau->m_rgiFirstNoiseBand);
        DELETE_ARRAY (pau->m_rgbBandNotCoded);
        DELETE_ARRAY (pau->m_rgffltSqrtBWRatio);
        DELETE_ARRAY (pau->m_rgiNoisePower);
        DELETE_ARRAY (pau->m_rgfltBandWeight);
    }
#endif //ENABLE_ALL_ENCOPT

    DELETE_ARRAY (pau->m_rgiCoefQ);

#if !defined(_Embedded_x86)
#if defined(WMA_TARGET_X86) && !(defined(BUILD_INTEGER) || defined(UNDER_CE))
    if (pau->m_fIntelFFT)
    {
        WMARESULT wmaResult;

        wmaResult = auFreeIntelFFT(pau, INTELFFT_FREE_PROCESS);
        TraceError(wmaResult);
    }
#endif  // WMA_TARGET_X86 && !defined(BUILD_INTEGER)
#endif

    auFree (pau);

    DUMP_MONITOR_RANGES(0);
}


//*****************************************************************************************
//
// auInit
// initialize a CAudioObject based on information from input file or stream
//
//*****************************************************************************************
WMARESULT auInit (CAudioObject* pau, 
                Int iVersionNumber,
                Int cSubband, 
                Int cSamplePerSec, 
                U16 cChannel, 
#ifdef WMA_V9
                U32 nBytePerSample,
                U16 nValidBitsPerSample,
                U32 nChannelMask,
#endif // WMA_V9
                Int cBytePerSec, 
                Int cbPacketLength,
                U16 iEncodeOpt,
                U16 iPlayerOpt)
{
    U16 iNonSupportedPlayerOpt;
    WMARESULT   wmaResult = WMA_OK;

    //set up default pcinfo for noise sub

    static const char fOKOptions[16] = {
        // WinCE Player Option Combinations
        WMAB_TRUE,  // 0: normal
        WMAB_TRUE,  // 1: Device that does not support 32kHz sampling -> interpolated downsample to 22kHz
        WMAB_TRUE,  // 2: Background HalfTransform mode to save CPU cycles
        WMAB_TRUE,  // 3: Background HalfTransform mode on device that does not support 32kHz sampling, ends up at 11kHz
        WMAB_TRUE,  // 4: A slow CPU which does not support F0 but does support 2*F0
        WMAB_TRUE,  // 5: Device that does not support 32kHz sample nor 22kHz playback, plays 32kHz data via 22kHz quality at 44kHz
        WMAB_TRUE,  // 6: Background HalfTransform mode for device that does not support half sampling rate
        WMAB_TRUE,  // 7: Background with downsampling - why?
        WMAB_TRUE,  // 8: Hide HP-430's lack of a low-pass filter for 22kHz output
        WMAB_FALSE, // 9: not appropriate - would need to interpolate to 44kHz if appropriate
        WMAB_TRUE,  // A: Background HalfTransform mode on a HP-430 at 22kHz, decode as normal (neither half nor doubled)
        WMAB_TRUE,  // B: why but why not allow?? 
        WMAB_FALSE, // C: not appropriate
        WMAB_FALSE, // D: not appropriate
        WMAB_FALSE, // E: not appropriate
        WMAB_FALSE  // F: not appropriate
    };
    assert( PLAYOPT_DOWNSAMPLE32TO22==1 && PLAYOPT_HALFTRANSFORM==2 && PLAYOPT_UPSAMPLE==4 && PLAYOPT_PAD2XTRANSFORM==8 );

    pau->m_iVersion = iVersionNumber;

#   if defined(WMA_TARGET_MIPS) && defined(BUILD_INTEGER)
        if (pau->m_iVersion < 2 && cSamplePerSec >= 44100 ) {
            // bug 813 - mips integer code gets an integer overflow exception on V1 content 11/21/2000
            wmaResult = TraceResult(WMA_E_NOTSUPPORTED);
            goto exit;
        }
#   endif

    pau->m_iWeightingMode    = (iEncodeOpt & ENCOPT_BARK) ? BARK_MODE : LPC_MODE;
    pau->m_fV5Lpc = (iEncodeOpt & ENCOPT_V5LPC);
#ifndef ENABLE_LPC
    if (LPC_MODE == pau->m_iWeightingMode)
    {
        // This build of the codec does not support LPC!
        wmaResult = TraceResult(WMA_E_NOTSUPPORTED);
        goto exit;
    }
#endif  // ENABLE_LPC

#if !defined(WMA_V9)
    if ( cChannel>2 ) {
        // when we add 5.1 support, we don't want existing decoders to try to decode them
        wmaResult = TraceResult(WMA_E_NOTSUPPORTED);
        goto exit;
    }
#endif

    pau->m_fAllowSuperFrame  = !!(iEncodeOpt & ENCOPT_SUPERFRAME);
    pau->m_fAllowSubFrame    = pau->m_fAllowSuperFrame && !!(iEncodeOpt & ENCOPT_SUBFRAME);

    if (pau->m_fAllowSubFrame) {
        pau->m_iMaxSubFrameDiv = ((iEncodeOpt & ENCOPT_SUBFRAMEDIVMASK) >>
            ENCOPT_SUBFRAMEDIVSHR);
        if (cBytePerSec / cChannel >= 4000)
            pau->m_iMaxSubFrameDiv = (8 << pau->m_iMaxSubFrameDiv);
        else 
            pau->m_iMaxSubFrameDiv = (2 << pau->m_iMaxSubFrameDiv);
    }
    else 
        pau->m_iMaxSubFrameDiv = 1;
    if (pau->m_iMaxSubFrameDiv > cSubband / MIN_FRAME_SIZE)
        pau->m_iMaxSubFrameDiv = cSubband / MIN_FRAME_SIZE;

    assert(TRANSCODER_AB(WMAB_TRUE, 0 == (iEncodeOpt & ENCOPT_TRANSCODE)));
#ifdef TRANSCODER
    if (iEncodeOpt & ENCOPT_TRANSCODE)
        pau->m_ttTranscodeType = TT_SIMPLE;
#endif  // TRANSCODER

    // When using noise substitution for uncoded bark or frequency bands, 
    // m_fltDitherLevel sets an overall "gain" of the substitution noise
    if (pau->m_iWeightingMode == LPC_MODE)
        pau->m_fltDitherLevel = 0.04F;
    else // BARK_MODE
        pau->m_fltDitherLevel = 0.02F;

    // --- Handle player options ---
    // First, figure out if the player has asked us for an option that we
    // didn't build
    iNonSupportedPlayerOpt = ~0; // Turn everything on
    DOWNSAMPLE_ONLY(iNonSupportedPlayerOpt &= ~(PLAYOPT_DOWNSAMPLE32TO22));
    HALFTRANSFORM_ONLY(iNonSupportedPlayerOpt &= ~(PLAYOPT_HALFTRANSFORM));
    UPSAMPLE_ONLY(iNonSupportedPlayerOpt &= ~(PLAYOPT_UPSAMPLE));
    PAD2XTRANSFORM_ONLY(iNonSupportedPlayerOpt &= ~(PLAYOPT_PAD2XTRANSFORM));

    if ( (iNonSupportedPlayerOpt & iPlayerOpt) || !fOKOptions[iPlayerOpt&0xF] )
    {
        // Player has requested something that we didn't build 
        // or wants an unsupported combination of options
        wmaResult = TraceResult(WMA_E_NOTSUPPORTED);
        goto exit;
    }

    if ( (iPlayerOpt&(PLAYOPT_HALFTRANSFORM|PLAYOPT_PAD2XTRANSFORM))==(PLAYOPT_HALFTRANSFORM|PLAYOPT_PAD2XTRANSFORM) )
    {
        // if in pad2X mode and a shift into the background sets half transform mode, do neither
        iPlayerOpt &= ~(PLAYOPT_HALFTRANSFORM|PLAYOPT_PAD2XTRANSFORM);
    }

#if defined(INTERPOLATED_DOWNSAMPLE)
    pau->m_fLowPass = WMAB_FALSE;
    pau->m_fDownsample = WMAB_FALSE;
    // 16000->11025 produces ticking sounds in tough_16m_16.  32000->22050 also produces ticking, but is less pronounced.
    // if ((iPlayerOpt & PLAYOPT_DOWNSAMPLE32TO22) && (32000 == cSamplePerSec || 16000 == cSamplePerSec) )
    if ((iPlayerOpt & PLAYOPT_DOWNSAMPLE32TO22) && (32000 == cSamplePerSec) )
    {
        pau->m_fLowPass = WMAB_TRUE;
        pau->m_fDownsample = WMAB_TRUE;
        // This works out to 32000=>22050 or 16000=>11025
        prvInterpolateInit(pau, cSamplePerSec, 32000 == cSamplePerSec ? 22000 : 11000, 441); 
    }
#endif  // INTERPOLATED_DOWNSAMPLE

#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
    pau->m_fHalfTransform = WMAB_FALSE;
    if (iPlayerOpt & PLAYOPT_HALFTRANSFORM)
        pau->m_fHalfTransform = WMAB_TRUE;

    pau->m_fUpsample = WMAB_FALSE;
    if (iPlayerOpt & PLAYOPT_UPSAMPLE)
        pau->m_fUpsample = WMAB_TRUE;

    pau->m_fPad2XTransform = WMAB_FALSE;
    if ((iPlayerOpt & PLAYOPT_PAD2XTRANSFORM)  && 22050 == cSamplePerSec)
        pau->m_fPad2XTransform = WMAB_TRUE;
#endif  // defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)

    //assign some basic parameters/mode
    pau->m_iSamplingRate    = cSamplePerSec;
    pau->m_cChannel         = cChannel;
    pau->m_nBytePerSample   = sizeof (I16);
#ifdef WMA_V9
    pau->m_nBytePerSample   = nBytePerSample;
    pau->m_nValidBitsPerSample = nValidBitsPerSample;
    pau->m_nChannelMask     = nChannelMask;
#endif 
    pau->m_cSubband         = cSubband;
    pau->m_fltBitsPerSample  = (Float)(cBytePerSec*8.0f/(pau->m_iSamplingRate*pau->m_cChannel));    
    pau->m_fltWeightedBitsPerSample = pau->m_fltBitsPerSample;
    // With the same QuantStep, the stereo is equivant to the mono with 1.6 times the bitrate/ch.
    if (pau->m_cChannel > 1) 
        pau->m_fltWeightedBitsPerSample *= (Float) MSA_STEREO_WEIGHT;

    //decide secondary parameters
    //first the frame sizes
    pau->m_cFrameSample      = 2 * pau->m_cSubband;
    pau->m_cFrameSampleHalf  = pau->m_cSubband;
    pau->m_cFrameSampleQuad  = pau->m_cSubband / 2; 

    pau->m_cSubFrameSample      = pau->m_cFrameSample;
    pau->m_cSubFrameSampleHalf  = pau->m_cFrameSampleHalf;
    pau->m_cSubFrameSampleQuad  = pau->m_cFrameSampleQuad; 

    pau->m_iCoefRecurQ1 = 0;
    pau->m_iCoefRecurQ2 = pau->m_iCoefRecurQ3 = pau->m_cSubFrameSampleHalf;
    pau->m_iCoefRecurQ4 = pau->m_cSubFrameSample;

    //init now; but can be reassigned in each frame; if so, be careful with tran. det.
    pau->m_cMinSubFrameSample    = pau->m_cSubFrameSample / pau->m_iMaxSubFrameDiv;
    pau->m_cMinSubFrameSampleHalf= pau->m_cSubFrameSampleHalf / pau->m_iMaxSubFrameDiv;
    pau->m_cMinSubFrameSampleQuad= pau->m_cSubFrameSampleQuad / pau->m_iMaxSubFrameDiv; 
    pau->m_cPossibleWinSize = LOG2 ((U32)pau->m_iMaxSubFrameDiv) + 1;
    pau->m_cBitsSubbandMax = LOG2 ((U32)pau->m_cSubband);

    if(pau->m_iVersion == 1)
        pau->m_cLowCutOffLong    = LOW_CUTOFF_V1;                     //need investigation
    else
        pau->m_cLowCutOffLong    = LOW_CUTOFF;                        //need investigation
    pau->m_cHighCutOffLong       = pau->m_cSubband - 9 * pau->m_cSubband / 100; //need investigation

    //default
    pau->m_cLowCutOff            = pau->m_cLowCutOffLong;
    pau->m_cHighCutOff           = pau->m_cHighCutOffLong;

    //set up some global coding condtions based on bitrate
    pau->m_iEntropyMode = SIXTEENS_OB;                     //default

    if (pau->m_fltWeightedBitsPerSample < 0.72f) {
        if (pau->m_iSamplingRate >= 32000)
            pau->m_iEntropyMode = FOURTYFOURS_QB;
    }
    else if (pau->m_fltWeightedBitsPerSample < 1.16f) {
        if (pau->m_iSamplingRate >= 32000)   {
            pau->m_iEntropyMode = FOURTYFOURS_OB;
        }
    }

#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
    prvSetAdjustedValues(pau, WMAB_TRUE);
#endif

    TRACEWMA_EXIT(wmaResult, prvAllocate (pau));

    //initialize constants for packetization
    pau->m_cBitPacketLength  = cbPacketLength * BITS_PER_BYTE;
    pau->m_cBitPackedFrameSize = LOG2 ((U32)ROUNDF (pau->m_fltBitsPerSample * pau->m_cFrameSampleHalf / 8.0F)) + 2;
//pau->m_cBitPackedFrameSize += 1;
    pau->m_cBitPacketHeader   = NBITS_PACKET_CNT + NBITS_FRM_CNT + pau->m_cBitPackedFrameSize + 3;
    pau->m_cdwPacketHeader     = pau->m_cBitPacketHeader / BITS_PER_DWORD;
    pau->m_cBitPacketHeaderFractionDw = pau->m_cBitPacketHeader % BITS_PER_DWORD;
    pau->m_iPacketCurr = 0;

#ifdef ENABLE_ALL_ENCOPT
#ifdef ENABLE_LPC
    prvInitInverseQuadRootTable(pau);    
#endif  // ENABLE_LPC

    if (pau->m_iWeightingMode == BARK_MODE) 
    {
        if (!pau->m_fNoiseSub)   
            pau->aupfnInverseQuantize = NULL;
        else
            pau->aupfnInverseQuantize = prvInverseQuantizeMidRate;
    }
    else
        pau->aupfnInverseQuantize = prvInverseQuantizeLowRate;
#endif  // ENABLE_ALL_ENCOPT

    pau->m_iFrameNumber = 0;

    pau->aupfnReconstruct = auReconstruct;

#if !(defined( WMA_TARGET_MIPS ) && defined( BUILD_INTEGER ))
    pau->aupfnDctIV = auDctIV;
    pau->aupfnFFT = prvFFT4DCT;
#endif

    
#if !defined(_Embedded_x86)
#if defined(WMA_TARGET_X86) && !(defined(BUILD_INTEGER) || defined(UNDER_CE))
    // auInitIntelFFT call checks CPU abilities for us (MMX, SIMD, etc)
    //wmaResult = auInitIntelFFT(pau, INTELFFT_INIT_PROCESS);
    if (WMA_SUCCEEDED(wmaResult))
    {
        pau->aupfnFFT = prvFFT4DCT_INTEL;
        pau->m_fIntelFFT = WMAB_FALSE;
    }

    // Failure means no applicable optimizations. This isn't a fatal error, so suppress it.
    wmaResult = WMA_OK;

#endif  // WMA_TARGET_X86 && !BUILD_INTEGER
#endif

#ifdef USE_SIN_COS_TABLES
#ifdef WMA_TARGET_X86
#if 0 //def BUILD_INTEGER
      //This function has been removed because of a streaming mode bug that is can be reproduced
      //by setting the streaming mode buffer to 2003
    if (g_SupportMMX()){
        pau->aupfnReconstruct = auReconstruct_MMX;
    }
#endif //BUILD_INTEGER

#ifdef BUILD_INT_FLOAT

    pau->aupfnReconstruct = auReconstruct_X86;
    if (pau->m_cChannel==2) {
        if (g_SupportMMX()) 
            pau->aupfnReconstruct = auReconStereo_MMX;

        if (g_SupportSIMD_FP()) 
            pau->aupfnReconstruct = auReconStereo_SIMDFP;
    }

#endif //BUILD_INT_FLOAT

#else  
#pragma message( "Warning! USE_SIN_COS_TABLES defined but no functions are using them. See X86.c for prototype functions." ) 
#endif //WMA_TARGET_X86
#endif //USE_SIN_COS_TABLES

#ifdef WMA_TARGET_MIPS
#   ifdef BUILD_INTEGER
        auInitMIPS(pau);
#   endif //BUILD_INTEGER
#endif //WMA_TARGET_MIPS

exit:
    return wmaResult;
}
                                  


//*****************************************************************************************
//
// prvAllocate
// allocate arrays used by a CAudioObject
// part of auInit
//
//*****************************************************************************************
WMARESULT prvAllocate (CAudioObject* pau)
{
    WMARESULT   wmaResult = WMA_OK;

    const int c_iSizeOfPrevOutput = pau->m_nBytePerSample * DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf) * pau->m_cChannel;
#ifndef PREVOUTPUT_16BITS
    const int c_iSizeOfPrevOutputSign = DOUBLE(pau->m_fPad2XTransform,pau->m_cFrameSampleHalf) * pau->m_cChannel / BITS_PER_BYTE 
                                        + pau->m_cChannel * pau->m_nBytePerSample; //wchen: not sure what this is for; but needed
#else 
    const int c_iSizeOfPrevOutputSign = 0;
#endif

    pau->m_rgpcinfo = (PerChannelInfo*) auMalloc (sizeof (PerChannelInfo) * pau->m_cChannel);
    if(pau->m_rgpcinfo == NULL)
    {
        wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
        goto exit;
    }

    pau->m_piPrevOutput = (I16*) auMalloc (c_iSizeOfPrevOutput + c_iSizeOfPrevOutputSign);
    if (pau->m_piPrevOutput == NULL)
    {
        wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
        goto exit;
    }
    memset (pau->m_piPrevOutput, 0, c_iSizeOfPrevOutput + c_iSizeOfPrevOutputSign);

    pau->m_rgiCoefReconOrig     = (Int*) mallocAligned (sizeof (Int) *
        DOUBLE(pau->m_fPad2XTransform,pau->m_cSubband) * pau->m_cChannel, 32);
    if (pau->m_rgiCoefReconOrig == NULL)
    {
        wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
        goto exit;
    }
    memset (pau->m_rgiCoefReconOrig, 0, 
            sizeof (Int) * DOUBLE(pau->m_fPad2XTransform,pau->m_cSubband) * pau->m_cChannel);    

    pau->m_rgcValidBarkBand  = (Int*) auMalloc (sizeof (Int) * pau->m_cPossibleWinSize);
    if (pau->m_rgcValidBarkBand == NULL)
    {
        wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
        goto exit;
    }

    pau->m_rgiBarkIndexOrig = (Int*) auMalloc (sizeof (Int) * (NUM_BARK_BAND + 1) * pau->m_cPossibleWinSize); //+1 : including bottom and top end
    if (pau->m_rgiBarkIndexOrig == NULL)
    {
        wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
        goto exit;
    }
    
    prvSetBarkIndex (pau);
    pau->m_rgiMaskQ         = (Int*) auMalloc (sizeof (Int) * pau->m_cValidBarkBand * pau->m_cChannel);
    if (pau->m_rgiMaskQ == NULL)
    {
        wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
        goto exit;
    }

    //for v2 it has to be dynamically decided
    if (pau->m_iVersion == 1) 
        pau->m_fltDctScale = (Float) sqrt (2.0 / pau->m_cSubbandAdjusted);


#ifdef ENABLE_ALL_ENCOPT
    // find the region to apply noise subsitution, in the frequencies...
    TRACEWMA_EXIT(wmaResult, prvInitNoiseSub (pau));

#if defined(_DEBUG) && defined(LPC_COMPARE)
    if (pau->m_iWeightingMode == LPC_MODE) {
        Int i;
        for (i = 0; i < LPCORDER; i++)  {
            pau->m_rgfltLpsSpecCos [i] = NULL;
            pau->m_rgfltLpsSpecSin [i] = NULL;
        }
    }
#endif

    if (pau->m_iWeightingMode == LPC_MODE && pau->m_rguiWeightFactor==NULL)
    {
        pau->m_rguiWeightFactor = (UInt*) auMalloc (sizeof (UInt) * DOUBLE(pau->m_fPad2XTransform,pau->m_cSubband) * pau->m_cChannel);
        if (pau->m_rguiWeightFactor == NULL)
        {
            wmaResult = TraceResult(WMA_E_OUTOFMEMORY);
            goto exit;
        }
    }

#endif // ENABLE_ALL_ENCOPT

#ifdef USE_SIN_COS_TABLES          
        pau->m_piSinForRecon2048 = (BP2Type*)g_SinTableForReconstruction;
        pau->m_piSinForRecon1024 = pau->m_piSinForRecon2048+2048;
        pau->m_piSinForRecon512  = pau->m_piSinForRecon1024+1024;
        pau->m_piSinForRecon256  = pau->m_piSinForRecon512+512;
        pau->m_piSinForRecon128  = pau->m_piSinForRecon256+256;
        pau->m_piSinForRecon64   = pau->m_piSinForRecon128+128;

        pau->m_piSinForSaveHistory = pau->m_piSinForRecon2048+2048;
#endif
exit:
    return wmaResult;
}


//*****************************************************************************************
//
// prvSetBarkIndex
// part of prvAllocate which is part of auInit
//
//*****************************************************************************************
Void prvSetBarkIndex (CAudioObject* pau)
{
    Int i, iWin;
    Float fltSamplingPeriod;

    Int *piBarkIndex;
    Int cFrameSample;
    Bool bCombined;

    //calculate index of each bark freq
    fltSamplingPeriod = 1.0F / pau->m_iSamplingRate;
    // wchen: we need to think what to do with the cut off frequencies: not include at all or include zeros.
    //for long window
    piBarkIndex = pau->m_rgiBarkIndexOrig;

    // for the v1 compatibility
    if(pau->m_iVersion == 1)
    {
        assert(pau->m_cPossibleWinSize == 1);

        // set up the bark index
        piBarkIndex[0] = 0;
        for (i = 0; i < NUM_BARK_BAND; i++)
        {
            piBarkIndex [i + 1] = (Int) ftoi(g_rgiBarkFreq [i] * pau->m_cFrameSample * 
                                         fltSamplingPeriod + 0.5f); //open end
            assert (piBarkIndex [i + 1]);
            if (piBarkIndex [i + 1] > pau->m_cFrameSample / 2)
            {
                piBarkIndex [i + 1] = pau->m_cFrameSample / 2;
                pau->m_rgcValidBarkBand [0] = i + 1;
                break;
            }
        }
    }
    else
    {
        for (iWin = 0; iWin < pau->m_cPossibleWinSize; iWin++)    {
            piBarkIndex  [0] = 0;
            cFrameSample = pau->m_cFrameSample / (1 << iWin);
            bCombined = WMAB_FALSE;
            if (pau->m_iSamplingRate >= 44100) {
                if(cFrameSample == 1024) { // winsize = 512
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand[iWin] = 17;
                    piBarkIndex[1]  = 5;    piBarkIndex[2]  = 12;
                    piBarkIndex[3]  = 18;   piBarkIndex[4]  = 25;
                    piBarkIndex[5]  = 34;   piBarkIndex[6]  = 46;
                    piBarkIndex[7]  = 54;   piBarkIndex[8]  = 63;
                    piBarkIndex[9]  = 86;   piBarkIndex[10] = 102;
                    piBarkIndex[11] = 123;  piBarkIndex[12] = 149;
                    piBarkIndex[13] = 179;  piBarkIndex[14] = 221;
                    piBarkIndex[15] = 279;  piBarkIndex[16] = 360;
                    piBarkIndex[17] = 512;
                }
                else if(cFrameSample == 512) { // winsize = 256
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand[iWin] = 15;
                    piBarkIndex[1]  = 5;    piBarkIndex[2]  = 11;
                    piBarkIndex[3]  = 17;   piBarkIndex[4]  = 23;
                    piBarkIndex[5]  = 31;   piBarkIndex[6]  = 37;
                    piBarkIndex[7]  = 43;   piBarkIndex[8]  = 51;
                    piBarkIndex[9]  = 62;   piBarkIndex[10] = 74;
                    piBarkIndex[11] = 89;   piBarkIndex[12] = 110;
                    piBarkIndex[13] = 139;  piBarkIndex[14] = 180;
                    piBarkIndex[15] = 256;
                }
                else if(cFrameSample == 256)  { // winsize = 128
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand [iWin] = 12;
                    piBarkIndex[1]  = 4;   piBarkIndex[2]  = 9;
                    piBarkIndex[3]  = 12;  piBarkIndex[4]  = 16;
                    piBarkIndex[5]  = 21;  piBarkIndex[6]  = 26;
                    piBarkIndex[7]  = 37;  piBarkIndex[8]  = 45;
                    piBarkIndex[9]  = 55;  piBarkIndex[10] = 70;
                    piBarkIndex[11] = 90;  piBarkIndex[12] = 128;
                }
            }
            else if(pau->m_iSamplingRate >= 32000) {
                if(cFrameSample == 1024) { // winsize = 512
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand[iWin] = 16;
                    piBarkIndex[1]  = 6;   piBarkIndex[2]  = 13;
                    piBarkIndex[3]  = 20;  piBarkIndex[4]  = 29;
                    piBarkIndex[5]  = 41;  piBarkIndex[6]  = 55;
                    piBarkIndex[7]  = 74;  piBarkIndex[8]  = 101;
                    piBarkIndex[9]  = 141; piBarkIndex[10] = 170;
                    piBarkIndex[11] = 205; piBarkIndex[12] = 246;
                    piBarkIndex[13] = 304; piBarkIndex[14] = 384;
                    piBarkIndex[15] = 496; piBarkIndex[16] = 512;
                }
                else if(cFrameSample == 512) { // winsize = 256
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand[iWin] = 15;
                    piBarkIndex[1]  = 5;   piBarkIndex[2]  = 10;
                    piBarkIndex[3]  = 15;  piBarkIndex[4]  = 20;
                    piBarkIndex[5]  = 28;  piBarkIndex[6]  = 37;
                    piBarkIndex[7]  = 50;  piBarkIndex[8]  = 70;
                    piBarkIndex[9]  = 85;  piBarkIndex[10] = 102;
                    piBarkIndex[11] = 123; piBarkIndex[12] = 152;
                    piBarkIndex[13] = 192; piBarkIndex[14] = 248;
                    piBarkIndex[15] = 256;
                }
                else if(cFrameSample == 256)  { // winsize = 128
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand [iWin] = 11;
                    piBarkIndex[1]  = 4;   piBarkIndex[2]  = 9;
                    piBarkIndex[3]  = 14;  piBarkIndex[4]  = 19;
                    piBarkIndex[5]  = 25;  piBarkIndex[6]  = 35;
                    piBarkIndex[7]  = 51;  piBarkIndex[8]  = 76;
                    piBarkIndex[9]  = 96;  piBarkIndex[10] = 124;
                    piBarkIndex[11] = 128;
                }
            }
            else if(pau->m_iSamplingRate >= 22050) {
                if(cFrameSample == 512) { // winsize = 256
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand [iWin] = 14;
                    piBarkIndex[1]  = 5;   piBarkIndex[2]  = 12;
                    piBarkIndex[3]  = 18;  piBarkIndex[4]  = 25;
                    piBarkIndex[5]  = 34;  piBarkIndex[6]  = 46;
                    piBarkIndex[7]  = 63;  piBarkIndex[8]  = 86;
                    piBarkIndex[9]  = 102; piBarkIndex[10] = 123;
                    piBarkIndex[11] = 149; piBarkIndex[12] = 179;
                    piBarkIndex[13] = 221; piBarkIndex[14] = 256;
                }
                else if(cFrameSample == 256) { // winsize = 128
                    bCombined = WMAB_TRUE;
                    pau->m_rgcValidBarkBand [iWin] = 10;
                    piBarkIndex[1]  = 5;   piBarkIndex[2]  = 11;
                    piBarkIndex[3]  = 17;  piBarkIndex[4]  = 23;
                    piBarkIndex[5]  = 31;  piBarkIndex[6]  = 43;
                    piBarkIndex[7]  = 62;  piBarkIndex[8]  = 89;
                    piBarkIndex[9]  = 110; piBarkIndex[10] = 128;
                }
            }

            if(!bCombined)
            {
                Float fltTemp = cFrameSample*fltSamplingPeriod;
                Int iIndex;
                Int iFreqCurr = 0;
                Int iCurr = 1;
                while(WMAB_TRUE)
                {
                    iIndex = ((Int) ftoi((g_rgiBarkFreq [iFreqCurr++]*fltTemp + 2.0f)/4.0f))*4; // open end

                    if(iIndex > piBarkIndex[iCurr - 1])
                        piBarkIndex[iCurr++] = iIndex;

                    if(iFreqCurr >= NUM_BARK_BAND ||
                       piBarkIndex[iCurr - 1] > cFrameSample/2)
                    {
                        piBarkIndex[iCurr - 1] = cFrameSample/2;
                        pau->m_rgcValidBarkBand[iWin] = iCurr - 1;
                        break;
                    }
                }
            }
            else
            {
                for (i = 0; i < pau->m_rgcValidBarkBand [iWin]; i++) {
                    piBarkIndex [i + 1] = ((piBarkIndex [i + 1] + 2) / 4) * 4;  //rounding
                    assert (piBarkIndex [i + 1] > piBarkIndex [i]);
                }
            }

            piBarkIndex +=  NUM_BARK_BAND + 1;
        }
    }
    //default
    pau->m_rgiBarkIndex = pau->m_rgiBarkIndexOrig;
    pau->m_cValidBarkBand = pau->m_rgcValidBarkBand [0];

#ifdef DEBUG
    // checking for multiples of 4!!!
    if(pau->m_iVersion != 1)
    {
        Int iWin, iBand;
        for(iWin = 0; iWin < pau->m_cPossibleWinSize; iWin++)
        {
            for(iBand = 0; iBand <= pau->m_rgcValidBarkBand[iWin]; iBand++)
            {
                i = pau->m_rgiBarkIndexOrig[iWin*(NUM_BARK_BAND + 1) + iBand];
                assert(i%4 == 0);
            }
        }
    }
#endif // DEBUG

}


#ifdef BUILD_INTEGER
#define COEF m_rgiCoefRecon
#else   // BUILD_INTEGER
#define COEF m_rgfltCoefRecon
#endif  // BUILD_INTEGER

Void auPreGetPCM (CAudioObject* pau, U16* pcSampleDecoded)
{
    I16 iChannel;
    Int iWinDiff;

    //set up for getPCM
    for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) 
    {
        PerChannelInfo* ppcinfo = pau->m_rgpcinfo + iChannel;
        ppcinfo->m_rgiCoefReconCurr = ((I32*) ppcinfo->COEF) + pau->m_cSubbandAdjusted /2;
        ppcinfo->m_iCurrGetPCM_SubFrame = 0;       //current position in a subframe
        ppcinfo->m_piPrevOutputCurr = ppcinfo->m_piPrevOutput;
    }

    *pcSampleDecoded = (U16) pau->m_cSubFrameSampleHalfAdjusted;
    iWinDiff = pau->m_iSizeCurr - pau->m_iSizePrev;
    if (iWinDiff > 0)   {
        *pcSampleDecoded -= (U16)(iWinDiff >> 1);
        for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++) 
        {
            PerChannelInfo* ppcinfo = pau->m_rgpcinfo + iChannel;
            ppcinfo->m_rgiCoefReconCurr += iWinDiff >> 1;
            ppcinfo->m_iCurrGetPCM_SubFrame += (I16)(iWinDiff >> 1);
            assert(0<=ppcinfo->m_iCurrGetPCM_SubFrame && ppcinfo->m_iCurrGetPCM_SubFrame<=pau->m_cSubFrameSampleHalfAdjusted);
        }
    }
    iWinDiff = pau->m_iSizeCurr - pau->m_iSizeNext;
    if (iWinDiff > 0)
        *pcSampleDecoded += (U16)(iWinDiff >> 1);

    if (CODEC_BEGIN == pau->m_codecStatus)
        prvInitDiscardSilence(pau);
    else if (pau->m_fPacketLossAdj == WMAB_TRUE) //being seeked
        pau->m_iDiscardSilence = pau->m_cSubFrameSampleHalfAdjusted;
    pau->m_fPacketLossAdj = WMAB_FALSE;

    // Check if the samples we just generated are to be discarded (silence)
    // We want to only return what the user should expect to get back from auGetPCM
    if (pau->m_iDiscardSilence)
    {
        U16         uSilence;
        Bool        fUpdateTrig;

        // Part or all of the samples just generated will be discarded.
        if (pau->m_iDiscardSilence >= *pcSampleDecoded)
        {
            // ALL of the samples just generated will be discarded

            uSilence = *pcSampleDecoded;
            *pcSampleDecoded = 0;
            fUpdateTrig = WMAB_FALSE;
        }
        else
        {
            // We don't want to overreport what user will get back from auGetPCM,
            // so subtract the silence.
            uSilence = (U16) pau->m_iDiscardSilence;
            *pcSampleDecoded -= (U16) pau->m_iDiscardSilence;

            // The trigonometry is going to be used after this to compute PCM
            // Update it now as if we had called auGetPCM
            fUpdateTrig = WMAB_TRUE;
        }

        // SIMULATE call to auGetPCM on behalf of the user to flush silence bits out
        // Avoid calling auGetPCM to discard silence because the temporary buffer
        // needed is objectionable to small devices

        // First, do things that auReconMono would normally do
        for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
        {
            PerChannelInfo *ppcinfo = &pau->m_rgpcinfo[iChannel];
            const U16 uSilenceQ2MAX = (U16)min(pau->m_iCoefRecurQ2 -
                ppcinfo->m_iCurrGetPCM_SubFrame, uSilence);
            Int iCoefCount;
            Int iCoefCountMAX;
            I32 iCoefPtrOffset;
            Int iCoefDelta;


            // Need to simulate movement of coefficient ptr in auGetPCM
            iCoefCount = ppcinfo->m_iCurrGetPCM_SubFrame;
            iCoefCountMAX = iCoefCount + uSilence;
            iCoefPtrOffset = 0;
            if (iCoefCount < pau->m_cSubFrameSampleQuadAdjusted)
            {
                iCoefDelta = min(iCoefCountMAX, pau->m_cSubFrameSampleQuadAdjusted) - iCoefCount;
                iCoefPtrOffset += iCoefDelta;
                iCoefCount += iCoefDelta;
            }

            if (iCoefCount < pau->m_iCoefRecurQ2)
            {
                iCoefDelta = min(iCoefCountMAX, pau->m_iCoefRecurQ2) - iCoefCount;
                iCoefPtrOffset -= iCoefDelta;
                iCoefCount += iCoefDelta;
            }

            if (iCoefCount < pau->m_iCoefRecurQ3)
            {
                iCoefDelta = min(iCoefCountMAX, pau->m_iCoefRecurQ3) - iCoefCount;
                iCoefPtrOffset -= iCoefDelta;
                iCoefCount = iCoefDelta;
            }

            assert(iCoefCount <= uSilence);
            ppcinfo->m_rgiCoefReconCurr += iCoefPtrOffset;
            ppcinfo->m_piPrevOutputCurr += uSilenceQ2MAX * pau->m_cChannel;
            ppcinfo->m_iCurrGetPCM_SubFrame += uSilence;

            if (fUpdateTrig)
            {
                Int         iSize;
                BP2Type     bpSinA;
                BP2Type     bpCosA;
                BP2Type     bpSinB;
                BP2Type     bpCosB;
                Int         iMultiplier;

                //if the adjacent size is bigger; just keep your own shape
                //otherwise a transition window is needed.
                if (pau->m_iSizePrev >= pau->m_iSizeCurr)
                {
                    //just forward copy curr
		            iSize = pau->m_iSizeCurr;
                }
                else
                {
                    //long start
		            iSize = pau->m_iSizePrev;
                }


                // It turns out that uSilenceQ2MAX is either equal to iSize
                // or equal to half of it. Our trig then simplifies to
                // sin(pi/(4*iSize) + pi/X) where X = 2 or 4.
                // Use trig ID's below to calculate these values.
                bpSinA = *((BP2Type*)(&ppcinfo->m_iSin));
                bpCosA = *((BP2Type*)(&ppcinfo->m_iCos));
                assert(uSilenceQ2MAX == iSize || uSilenceQ2MAX * 2 == iSize);
                iMultiplier = iSize / uSilenceQ2MAX;
                switch(iMultiplier)
                {
                    case 1:
                        bpSinB = BP2_FROM_FLOAT(1.0); // sin(pi/2)
                        bpCosB = BP2_FROM_FLOAT(0.0); // cos(pi/2)
                        break;

                    case 2:
                        bpSinB = BP2_FROM_FLOAT(0.70710678118654752440084436210485); // sin(pi/4)
                        bpCosB = BP2_FROM_FLOAT(0.70710678118654752440084436210485); // cos(pi/4)
                        break;

                    default:
                        assert(WMAB_FALSE);
                        break;
                }


                // Let A = start = pi/(4*iSize), B = pi/X where X = 2 or 4.
                // Trig ID's: sin(A+B) = sin(A)cos(B) + cos(A)sin(B)
                //            cos(A+B) = cos(A)cos(B) + sin(A)sin(B)
                // To get the prior step for recursion, note that subtracting
                // a step (step = 2*A) and simplifying gets us to
                // sin(pi/X - pi/(4*iSize)) where X = 2 or 4.
                *(BP2Type*)(&ppcinfo->m_iSin)  = MULT_BP2(bpSinA, bpCosB) +
                    MULT_BP2(bpCosA, bpSinB);
                *(BP2Type*)(&ppcinfo->m_iCos)  = MULT_BP2(bpCosA, bpCosB) -
                    MULT_BP2(bpSinA, bpSinB);
                *(BP2Type*)(&ppcinfo->m_iSin1) = MULT_BP2(bpSinB, bpCosA) -
                    MULT_BP2(bpCosB, bpSinA);
                *(BP2Type*)(&ppcinfo->m_iCos1) = MULT_BP2(bpCosB, bpCosA) +
                    MULT_BP2(bpSinB, bpSinA);

                // Check the accuracy
                assert(fabs(FLOAT_FROM_BP2(*((BP2Type*)(&ppcinfo->m_iSin))) -
                    sin(0.5 * PI / iSize / 2 * (2*uSilenceQ2MAX + 1))) <= 0.001);
                assert(fabs(FLOAT_FROM_BP2(*((BP2Type*)(&ppcinfo->m_iCos))) -
                    cos(0.5 * PI / iSize / 2 * (2*uSilenceQ2MAX + 1))) <= 0.001);
                assert(fabs(FLOAT_FROM_BP2(*((BP2Type*)(&ppcinfo->m_iSin1))) -
                    sin(0.5 * PI / iSize / 2 * (2*uSilenceQ2MAX - 1))) <= 0.001);
                assert(fabs(FLOAT_FROM_BP2(*((BP2Type*)(&ppcinfo->m_iCos1))) -
                    cos(0.5 * PI / iSize / 2 * (2*uSilenceQ2MAX - 1))) <= 0.001);

            }
        }

#ifdef USE_SIN_COS_TABLES
        if (fUpdateTrig && pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame < pau->m_iCoefRecurQ2)
        {
            const U16 uSilenceQ2MAX = (U16)min(pau->m_iCoefRecurQ2 -
                pau->m_rgpcinfo[0].m_iCurrGetPCM_SubFrame, uSilence);

            pau->m_piSinForRecon += uSilenceQ2MAX; // Shared by both channels
        }
#endif  // USE_SIN_COS_TABLES

        // Next, do the things that auGetPCM would normally do
        pau->m_iDiscardSilence -= uSilence;
        assert(pau->m_iDiscardSilence >= 0);
        if (CODEC_BEGIN == pau->m_codecStatus)
            pau->m_codecStatus = CODEC_STEADY;

    }
}


WMARESULT auReconstruct (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll){
    Int iChannel;
    WMARESULT hr;
    for (iChannel = 0; iChannel < pau->m_cChannel; iChannel++)
    {
        hr = (auReconMono) (pau, 
                          pau->m_rgpcinfo + iChannel, 
                          piOutput + iChannel,  
                          (I16*)pcSampleGet,
                          fForceTransformAll);
    }
    return hr;
}


WMARESULT auGetPCM (CAudioObject* pau, U16* pcSample, U8* pbDst, U32 cbDstLength)
{  
    Int cbDstUsed;
    Int* pcbDstUsed = &cbDstUsed;
    WMARESULT hr;
    I16* piOutput = (I16*) pbDst;

    U32 cSamplesLeft;
    I16 cSamplesREQ;

    cSamplesLeft = *pcSample;
    do
    {
        const U32 cMaxSamplesInDst = (cbDstLength / sizeof (I16) / pau->m_cChannel);

        // Limit requested number of bytes to fit within the buffer
        const U32 u32MaxSamplesReq = min(cSamplesLeft, cMaxSamplesInDst);

        // limit request to fit in I16
        cSamplesREQ = (I16) min(u32MaxSamplesReq, SHRT_MAX);

        hr = (*pau->aupfnReconstruct) (pau,
                          piOutput,
                          &cSamplesREQ,
                          WMAB_FALSE);

        cSamplesLeft -= cSamplesREQ;
        if (0 == cSamplesREQ)
            // No more samples to retrieve, the well is dry. Avoid infinite loop
            break;

        // Check if we just reconstructed silence that we need to toss
        if (pau->m_iDiscardSilence)
            prvDiscardSilence(pau, (unsigned short *)&cSamplesREQ, pau->m_cChannel, pbDst);

    } while (0 == cSamplesREQ && 0 != cSamplesLeft);

    if (CODEC_BEGIN == pau->m_codecStatus)
        pau->m_codecStatus = CODEC_STEADY;

    *pcSample = cSamplesREQ;
    *pcbDstUsed = (*pcSample) * pau->m_cChannel * sizeof (I16); // in bytes

#   ifdef INTERPOLATED_DOWNSAMPLE
        if (pau->m_fDownsample) {
            prvInterpolate(pau, piOutput, *pcbDstUsed, pau->m_cChannel,
                piOutput, pcbDstUsed, pcSample);
        }
#   endif  // INTERPOLATED_DOWNSAMPLE

#if defined(HALF_TRANSFORM)
    // interpolate 2X 
    // This simple linear interpolation is fast but does not remove aliasing 
    // So it should not be used with the HALF_TRANSFORM case if audio quality is important
    if (pau->m_fUpsample && *pcbDstUsed > 0)
    {
        // work from back of buffer to front so we do not overwrite any values we still need to use
        Int iChannel;
        Int iSamplesPerChannel = *pcbDstUsed / (sizeof(I16) * pau->m_cChannel);
        const int c_iDecr = pau->m_cChannel;

        for (iChannel = 0; iChannel < c_iDecr; iChannel++)
        {

            I16 *piInput = piOutput + iChannel + (iSamplesPerChannel - 1) * c_iDecr;
            I16 *piOutputx = piOutput + iChannel + (2 * iSamplesPerChannel - 1) * c_iDecr;
            I16 iNextInput, iPrevInput;

            iNextInput = *piInput;
            while (piOutputx > (piOutput + iChannel + 2))
            {
                assert(piOutputx >= piInput + iChannel);
                *piOutputx = iNextInput;
                piOutputx -= c_iDecr;

                iPrevInput = iNextInput;

                piInput  -= c_iDecr;
                assert(piInput >= (piOutput + iChannel));
                iNextInput = *piInput;

                assert(piOutputx >= piInput + iChannel);
                *piOutputx = (iPrevInput >> 1) + (iNextInput >> 1);
                assert(fabs((double)(*piOutputx) - ((double)iPrevInput +
                    (double)iNextInput)/2.0) <= 1);

                piOutputx -= c_iDecr;
            }

            // handle beginning of buffer case
            assert( piOutputx == piOutput + iChannel + 2);
            *piOutputx = iNextInput;
            piOutputx -= c_iDecr;

            iPrevInput = iNextInput;

            assert(piInput >= (piOutput + iChannel));
            iNextInput = pau->m_iPriorSample[iChannel];

            assert(piOutputx >= piInput);
            *piOutputx = (iPrevInput >> 1) + (iNextInput >> 1);
            assert(fabs((double)(*piOutputx) - ((double)iPrevInput +
                (double)iNextInput)/2.0) <= 1);

            pau->m_iPriorSample[iChannel] = *(piOutput + iChannel + (2 * iSamplesPerChannel - 1) * c_iDecr);

        }

        *pcSample   *= 2;
        *pcbDstUsed *= 2;
    }
#endif  // defined(HALF_TRANSFORM)

    //FUNCTION_PROFILE_STOP(&fp);
    return WMA_OK;

}

//*****************************************************************************************
//
// prvInverseTransformMono
//  transform, window and combine an overlapping subframe of audio data
//  called repeatedly for each subframe in a frame
// 
//  using abbreviated names, the process in outline form is as follows:
//  
//      pSrc points to someplace in current or previous time-domain Output array
//      pDst points to someplace in current or previous time-domain Output array
//      between each "upto" step below, these two points may change
//
//      Coef is an array of frequency-domain coefs
//
//      call DCT and transform Coef to time-domain
//      pCoef set to middle of the now time-domain coeffiecents
//
//      Window and combine the transformed Coef and pSrc into pDst as follows:
//
//      Window
//                  ________________
//                 /                \
//                /                  \
//               /                    \
//      ________/                      \_________
//      |       | | |       |      | | |         |
//      0       Q Q Q       H      Q 3 Q         F
//              1 u 2       a      3 Q 4         u
//                a         l        u           l
//                d         f        a           l
//                                   d
//      
//      upto Q1 (eg 0): 
//          Dst <- Src                              [[++pDst, ++pCoef, ++pSrc]]
//
//      upto cSubFrameSampleQuad (eg 1024):
//          Dst <- ( sin(a) * Coef + Src<<5 ) >> 5  [[++pDst, ++pCoef, ++pSrc, ++a]]
//
//      upto Q2 (eg 2048):
//          Dst <- ( sin(a) * Coef + Src<<5 ) >> 5  [[++pDst, --pCoef, ++pSrc, ++a]]
//
//      upto cSubFrameSampleHalf (eg 2048):
//          Dst <- Coef >> 5                        [[++pDst, --pCoef]]
//
//      upto Q3 (eg 2944):
//          Dst <- Coef >> 5                        [[++pDst, --pCoef]]
//
//      upto 3 * cSubFrameSampleQuad (eg 3072):
//          Dst <- ( sin(a) * Coef ) >> 5;          [[++pDst, --pCoef]]
//      
//      upto Q4 (eg 3200):
//          Dst <- ( sin(a) * Coef ) >> 5;          [[++pDst, ++pCoef]]
//
//
//  
//  Values decoding the first few frames of Tough_44m_64.cmp (9/22/99 12:08PM version)
//  where pDst is relative to piOutput when c. and to piPrevOutput when p.
//
//                          iCurrSubFrame       iSubFrmOffset   pDst prior to loop:
//              FrameSample         cSubFrameSample               Q1     Q2     Q3     Q4
//                              
//  Decode             4096
//    inverseTransform          0       4096           0        c....0 c.1024 p....0 p.1024
//  Decode             4096
///   inverseTransform          0       2048         512        c..512 c.1024 c.1536 p....0
//    inverseTransform          1        256        1984        c.1984 p....0 p...64 p..128
//    inverseTransform          2        256        2112        p...64 p. 128 p..192 p..256
//    inverseTransform          3        256        2240        p. 192 p. 256 p..320 p..384
//    inverseTransform          4        256        2368        p..320 p..320 p..448 p..512
//    inverseTransform          5       1024        2304        p..256 p..512 p..768 p.1024
//  Decode             4096
//    inverseTransform          0       4096           0        c....0 c.1024 p....0 p.1024
//  Decode             4096
//    inverseTransform          0       4096           0        c....0 c.1024 p....0 p.1024
//  Decode             4096
//    inverseTransform          0       4096           0        c....0 c.1024 p....0 p.1024
//  Decode             4096
//    inverseTransform          0       2048         512        c..512 c.1024 c.1536 p....0
//    inverseTransform          1        512        1920        c.1920 p....0 p..128 p..256
//    inverseTransform          2        256        2240        p..192 p..256 p..320 p..384
//    inverseTransform          3        256        2368        p..320 p..384 p..448 p..512
//    inverseTransform          4        256        2496        p..448 p..512 p..576 p..640
//    inverseTransform          5        256        2624        p..576 p..640 p..704 p..768
//    inverseTransform          6        512        2688        p..640 p..768 p..896 p.1024
//
//*****************************************************************************************

#if defined(_DEBUG)
// Debugging Macros to break at a particular output sample or when sample values are out of range or to print sin values
//
// set IT_BREAK_AT_SAMPLE to a non-negative value to break when that sample is about to be processed
//#define IT_BREAK_AT_SAMPLE -2049
//
// set IT_BREAK_OUT_OF_RANGE to a positive value to break when a sample is stored out of the range
//#define IT_BREAK_OUT_OF_RANGE 32767
//#define IT_BREAK_OUT_OF_RANGE_PAST_SAMPLE 475500
//
// set IT_SIN_PRINT_ALL to print out all sin values as they are used
//#define IT_SIN_PRINT_ALL
//
// set IT_SIN_PRINT_INIT to print out initial sin values as they are used
//#define IT_SIN_PRINT_INIT
//
// set IT_SAMPLE_PRINT to print out sample values as they are generated
//#define IT_SAMPLE_PRINT
// 
// set IT_FRAME_PRINT to print out sample values as they are generated
//#define IT_FRAME_PRINT
// 
#   if defined(IT_BREAK_AT_SAMPLE) || defined(IT_BREAK_OUT_OF_RANGE)|| defined(IT_SIN_PRINT_ALL) \
    || defined(IT_SIN_PRINT_INIT) || defined(IT_SAMPLE_PRINT) || defined(IT_FRAME_PRINT)
#       pragma COMPILER_MESSAGE(__FILE__ "(1101) : Warning - Inverse Transform Debug Code Enabled.")
#       if defined(IT_BREAK_AT_SAMPLE)
            static int nSampleBreak = IT_BREAK_AT_SAMPLE;
#       else
            static int nSampleBreak = -2049;
#       endif
        static int nSampleBreakIdx = -2048;             // correction factor for startup
        static int nSampleBreakFrameSample = 0;         // prior frame's sample size
        static int iSubFrmOffset = 0;                   // Needed to add after merging with wchen's 12/17/99 checkin ???
#   endif
#   define IT_NO_SIN 0x7FEDCBA9
    static double dIorF2F = INTEGER_OR_INT_FLOAT(1.0/NF2BP2,1.0);
#   if defined(IT_SIN_PRINT_ALL)
#       define IT_DO_SIN_PRINT(idx1,idx2,v)                                     \
            if (v != IT_NO_SIN)                                                 \
                printf("%8d %4d %+.10f\n", idx1, idx2, v*dIorF2F )
#   else
#       define IT_DO_SIN_PRINT(a,b,c)
#   endif
#   if defined(IT_SAMPLE_PRINT)
#       define IT_DO_SAMPLE_PRINT(idx1,idx2,v) \
                printf("%8d %4d %+10d\n", idx1, idx2, (I32)v )
#   else
#       define IT_DO_SAMPLE_PRINT(a,b,c)
#   endif
#   if defined(IT_FRAME_PRINT)
#       define IT_DO_FRAME_PRINT                                                \
            if ( pau->m_iFrameNumber==0 )                                       \
                printf("\n");                                                   \
            printf("Frame %4d  SubFrm %2d  SFSamples %4d\n", pau->m_iFrameNumber, pau->m_iCurrSubFrame, pau->m_cSubFrameSampleAdjusted )
#   else
#       define IT_DO_FRAME_PRINT
#   endif
#   if defined(IT_BREAK_AT_SAMPLE) && defined(IT_BREAK_OUT_OF_RANGE)
#       define DEBUG_IT_SAMPLE_BREAK(idx,Dst,iSin)                              \
            if ( (nSampleBreakIdx+idx+iSubFrmOffset) == nSampleBreak || Dst < -IT_BREAK_OUT_OF_RANGE || Dst > IT_BREAK_OUT_OF_RANGE ) \
                DEBUG_BREAK();                                                  \
            IT_DO_SIN_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,iSin);        \
            IT_DO_SAMPLE_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,Dst)
#   elif defined(IT_BREAK_AT_SAMPLE)
#       define DEBUG_IT_SAMPLE_BREAK(idx,Dst,iSin)                              \
            if ( (nSampleBreakIdx+idx+iSubFrmOffset) == nSampleBreak )          \
                DEBUG_BREAK();                                                  \
            IT_DO_SIN_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,iSin);        \
            IT_DO_SAMPLE_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,Dst)
#   elif defined(IT_BREAK_OUT_OF_RANGE)
#       define DEBUG_IT_SAMPLE_BREAK(idx,Dst,iSin)                              \
            if ( ( Dst < -IT_BREAK_OUT_OF_RANGE || Dst > IT_BREAK_OUT_OF_RANGE )\
                 && (nSampleBreakIdx+idx+iSubFrmOffset) > IT_BREAK_OUT_OF_RANGE_PAST_SAMPLE )  \
                DEBUG_BREAK();                                                  \
            IT_DO_SIN_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,iSin);        \
            IT_DO_SAMPLE_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,Dst)
#   else
#       define DEBUG_IT_SAMPLE_BREAK(idx,Dst,iSin)                              \
            IT_DO_SIN_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,iSin);        \
            IT_DO_SAMPLE_PRINT(nSampleBreakIdx+idx+iSubFrmOffset,idx,Dst)
#   endif
#   if defined(IT_SIN_PRINT_INIT)
#       define DEBUG_IT_SIN_INIT(n,a,b,c,d,e)   \
                printf("%d  %+.10f  %+.10f  %+.10f  %+.10f  %+.10f\n", n, a*dIorF2F, b*dIorF2F, c*dIorF2F, d*dIorF2F, e*dIorF2F )
#   else
#       define DEBUG_IT_SIN_INIT(n,a,b,c,d,e)
#   endif
#   if defined(IT_BREAK_AT_SAMPLE) || defined(IT_BREAK_OUT_OF_RANGE) || defined(IT_SIN_PRINT_ALL) || defined(IT_SIN_PRINT_INIT)
        // break when nSampleBreak is within subframe
#       define DEBUG_IT_FRAME_BREAK \
            if ( pau->m_iCurrSubFrame == 0 && ppcinfo == pau->m_rgpcinfo )      \
            {   /* on first subframe of left channel */                         \
                nSampleBreakIdx += nSampleBreakFrameSample;                     \
                nSampleBreakFrameSample = pau->m_cFrameSampleHalfAdjusted;      \
            }                                                                   \
            IT_DO_FRAME_PRINT;                                                  \
            if ( nSampleBreakIdx+iSubFrmOffset <= nSampleBreak && nSampleBreak < (nSampleBreakIdx+iSubFrmOffset+pau->m_cSubFrameSampleAdjusted) )   \
                DEBUG_BREAK();
#   else
#       define DEBUG_IT_FRAME_BREAK
#   endif
#else
#   define DEBUG_IT_FRAME_BREAK
#   define DEBUG_IT_SAMPLE_BREAK(a,b,c)
#   define IT_NO_SIN 0x40000000
#   define DEBUG_IT_SIN_INIT(n,a,b,c,d,e)
#endif

#if defined(WMA_TARGET_SH4) && defined(BUILD_INT_FLOAT) && !defined(PREVOUTPUT_16BITS) && !defined(V4V5_COMPARE_MODE)

// for this build config find auReconMono and auSaveHistoryMono in sh4\transform_sh4.c 

#elif defined(WMA_TARGET_SH3) && defined(BUILD_INTEGER) && !defined(PREVOUTPUT_16BITS) && !defined(V4V5_COMPARE_MODE)

// for this build config find auReconMono and auSaveHistoryMono in sh3\transform_sh3.c

#elif defined(WMA_TARGET_MIPS) && defined(BUILD_INTEGER) && !defined(PREVOUTPUT_16BITS) && !defined(V4V5_COMPARE_MODE)

// for this build config find auReconMono and auSaveHistoryMono in mips\transform_mips_*.c

#else


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// auReconMono and auSaveHistoryMono for either Integer and Float

WMARESULT auReconMono ( CAudioObject* pau, 
                        PerChannelInfo* ppcinfo, 
                        I16* piOutput, 
                        I16* pcSampleGet,
                        Bool fForceTransformAll)
{
    //all starting from the begining of this frame
    //official starting of a subfrm (including leading zeros)
    //case 1 (Large -> Small):  starts later than what's been reconstructed w/o leading zeros
    //                          in this case dst should be skiped (this may mean memcpy from prev)
    //                          src and coef are fine
    //                          
    //case 2 (Small -> Large):  starts earler than what's been reconstructed but all leading zeros
    //                          in this case dst 

    BP2Type bp2SinT;     //temp sin value within recurrsion
    BP2Type bp2CosT;     //temp cos value within recurrsion
    CoefType *piCoef;   //floating vlaue = (float)*piCoef/(1<<5)  assuming TRANSFORM_FRACT_BITS = 5
    Int i, iEnd, iSegEnd;
    I16 *piSrc, *piDst; 
    I32 iResult;
    CoefType cfResult;
    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
    Int cSubFrameSampleHalf = pau->m_cSubFrameSampleHalfAdjusted;
    Int cSubFrameSampleQuad = pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ2 = pau->m_iCoefRecurQ2;
    Int iCoefRecurQ3 = pau->m_iCoefRecurQ3;
    BP2Type bp2Sin  = *((BP2Type*)(&ppcinfo->m_iSin));
    BP2Type bp2Cos  = *((BP2Type*)(&ppcinfo->m_iCos));
    BP2Type bp2Sin1 = *((BP2Type*)(&ppcinfo->m_iSin1));
    BP2Type bp2Cos1 = *((BP2Type*)(&ppcinfo->m_iCos1));
    BP2Type bp2Step = *((BP2Type*)(&ppcinfo->m_iStep));
    int fTrigUpdated = 0;

#ifndef PREVOUTPUT_16BITS
    I16    *piSrcSign;
    U32     uiSignbitData;
    U32     uiSignbitCount;
#endif  // PREVOUTPUT_16BITS
#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,RECON_MONO_PROFILE);
#endif
    DEBUG_IT_FRAME_BREAK;

    //persistent states
    i = ppcinfo->m_iCurrGetPCM_SubFrame;       //current position in a frame
    piCoef = (CoefType*)ppcinfo->m_rgiCoefReconCurr; 
    piSrc = ppcinfo->m_piPrevOutputCurr;

    //eventually we'll use piOutput directly
    piDst = piOutput;

    iEnd = i + *pcSampleGet;
    *pcSampleGet = 0;

    CALC_SIGN_OFFSETS(ppcinfo->m_piPrevOutput, piSrc, ppcinfo->m_piPrevOutputSign, piSrcSign, 
        uiSignbitData, uiSignbitCount, cChan)
    ASSERT_SIGNBIT_POINTER(piSrcSign);

#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
    iSegEnd = 0;
    if (i < cSubFrameSampleQuad) iSegEnd = min (cSubFrameSampleQuad, iEnd);
    if (iSegEnd < iCoefRecurQ2)  iSegEnd = min (iCoefRecurQ2, iEnd);
    if (iSegEnd < iCoefRecurQ3)  iSegEnd = min (iCoefRecurQ3, iEnd);
    if ( i < iSegEnd )
        prvWmaShowFrames(pau, "RecMono", " %4d 0x%08x 0x%08x 0x%08x %4d %4d %4d %4d", 
            i, piCoef, piSrc, piDst, cSubFrameSampleQuad, iCoefRecurQ2, iCoefRecurQ3, iSegEnd);
#endif
    if (i < cSubFrameSampleQuad) 
    {
        DEBUG_IT_SIN_INIT(0,bp2Sin,bp2Cos,bp2Sin1,bp2Cos1,bp2Step);
        iSegEnd = min (cSubFrameSampleQuad, iEnd);
        if ( i < iSegEnd )
        {
            fTrigUpdated = 1;
            for (; i < iSegEnd; i++, (*pcSampleGet)++)    {

                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData,
                    uiSignbitCount);

                cfResult = INT_FROM_COEF( MULT_BP2(-bp2Sin,*piCoef++) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );

                DEBUG_IT_SAMPLE_BREAK(i,  iResult, bp2Sin);

                piSrc += cChan;
                piDst += cChan;
                // sin(a+b) = sin(a-b) + 2*sin(b)*cos(a)
                // cos(a+b) = cos(a-b) - 2*sin(b)*sin(a)
                bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
                bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
                bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
                bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
            }
        }
    }
    if (i < iCoefRecurQ2)  
    {
        DEBUG_IT_SIN_INIT(1,bp2Sin,bp2Cos,bp2Sin1,bp2Cos1,bp2Step);
        iSegEnd = min (iCoefRecurQ2, iEnd);
        if ( i < iSegEnd )
        {
            fTrigUpdated = 1;
            for (; i < iSegEnd; i++,(*pcSampleGet)++)    {

                COMBINE_SIGNBIT(iResult, *piSrc, piSrcSign, uiSignbitData,
                    uiSignbitCount);

                cfResult = INT_FROM_COEF( MULT_BP2(bp2Sin,*--piCoef) + COEF_FROM_INT(iResult) );
                ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );

                DEBUG_IT_SAMPLE_BREAK(i, iResult, bp2Sin);

                piSrc += cChan;
                piDst += cChan;

                bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
                bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
                bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
                bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
            }
        }
    }
    if (i < iCoefRecurQ3) 
    {
        iSegEnd = min (iCoefRecurQ3, iEnd);
        for (; i < iSegEnd; i++,(*pcSampleGet)++)    {
            cfResult = INT_FROM_COEF( *--piCoef );
            ROUND_SATURATE_STORE( piDst, cfResult, SHRT_MIN, SHRT_MAX, iResult );
            DEBUG_IT_SAMPLE_BREAK(i, iResult, IT_NO_SIN);
            piDst += cChan;
        }
    }

    SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData);
    if ( fTrigUpdated )
    {
        *(BP2Type*)(&ppcinfo->m_iSin)  = bp2Sin;
        *(BP2Type*)(&ppcinfo->m_iCos)  = bp2Cos;
        *(BP2Type*)(&ppcinfo->m_iSin1) = bp2Sin1;
        *(BP2Type*)(&ppcinfo->m_iCos1) = bp2Cos1;
    }
    ppcinfo->m_rgiCoefReconCurr = (Int*)piCoef;
    ppcinfo->m_iCurrGetPCM_SubFrame = (I16)i;
    ppcinfo->m_piPrevOutputCurr = piSrc;
#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
}

#if !defined(WMA_TARGET_X86) || !defined(USE_SIN_COS_TABLES) || defined(PREVOUTPUT_16BITS) || defined (BUILD_INTEGER) || defined (DISABLE_OPT)
WMARESULT auSaveHistoryMono (CAudioObject* pau, 
                            PerChannelInfo* ppcinfo, 
                            Bool fForceTransformAll)
{
    BP2Type bp2SinT;     //temp sin value within recurrsion
    BP2Type bp2CosT;     //temp cos value within recurrsion
    CoefType *piCoef;   //floating vlaue = (float)*piCoef/(1<<5)  assuming TRANSFORM_FRACT_BITS = 5
    
    Int i;      // , iEnd, iSegEnd;
    I16 *piDst; 
    I32 iResult;
    CoefType cfResult;

    // get local copies of these values which are used frequently or repeatedly (helps optimization)
    Int cChan = pau->m_cChannel;
    // HALF_TRANSFORM: the following member variables are not restored so continue to hold their halved values
    Int cSubFrameSample3Quad = 3 * pau->m_cSubFrameSampleQuadAdjusted;
    Int iCoefRecurQ4 = pau->m_iCoefRecurQ4;
    BP2Type bp2Sin  = INTEGER_OR_INT_FLOAT( pau->m_iSinRampDownStart, pau->m_fltSinRampDownStart );
    BP2Type bp2Cos  = INTEGER_OR_INT_FLOAT( pau->m_iCosRampDownStart, pau->m_fltCosRampDownStart );
    BP2Type bp2Sin1 = INTEGER_OR_INT_FLOAT( pau->m_iSinRampDownPrior, pau->m_fltSinRampDownPrior );
    BP2Type bp2Cos1 = INTEGER_OR_INT_FLOAT( pau->m_iCosRampDownPrior, pau->m_fltCosRampDownPrior );
    BP2Type bp2Step = INTEGER_OR_INT_FLOAT( pau->m_iSinRampDownStep,  pau->m_fltSinRampDownStep  );


#ifndef PREVOUTPUT_16BITS
    I16    *piDstSign = ppcinfo->m_piPrevOutputSign;
    U32     uiSignbitData = 0;
    U32     uiSignbitCount = 0;
#endif  // PREVOUTPUT_16BITS

#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,SAVE_HISTORY_MONO_PROFILE);
#endif

    //all starting from the begining of this frame
    //official starting of a subfrm (including leading zeros)
    //case 1 (Large -> Small):  starts later than what's been reconstructed w/o leading zeros
    //                          in this case dst should be skiped (this may mean memcpy from prev)
    //                          src and coef are fine
    //                          
    //case 2 (Small -> Large):  starts earler than what's been reconstructed but all leading zeros
    //                          in this case dst 

    //eventually we'll use piOutput directly
    piDst = ppcinfo->m_piPrevOutput;

    DEBUG_IT_FRAME_BREAK;

    //persistent states
    i = ppcinfo->m_iCurrGetPCM_SubFrame;       //current position in a frame
    assert( (0<=i && i<=iCoefRecurQ4) || (i==CURRGETPCM_INVALID));
    piCoef = (CoefType*)ppcinfo->m_rgiCoefReconCurr; 
    ASSERT_SIGNBIT_POINTER(piDstSign);


#if defined(WMA_SHOW_FRAMES)  && defined(_DEBUG)
    prvWmaShowFrames(pau, "SavHist", " %4d 0x%08x 0x%08x %4d %4d", 
        i, piCoef, piDst, cSubFrameSample3Quad, iCoefRecurQ4);
#endif
    DEBUG_IT_SIN_INIT(3,bp2Sin,bp2Cos,bp2Sin1,bp2Cos1,bp2Step);
    for (; i < cSubFrameSample3Quad; i++)    {

        cfResult = INT_FROM_COEF( MULT_BP2(bp2Sin,*--piCoef) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );

        DEBUG_IT_SAMPLE_BREAK(i, iResult, bp2Sin);
        piDst += cChan;

        RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData,
            uiSignbitCount, 1);

        bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
        bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
        bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
        bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
    }

    for (; i < iCoefRecurQ4; i++)    {

        cfResult = INT_FROM_COEF( MULT_BP2(bp2Sin,*piCoef++) );
        ROUND_SATURATE_STORE( piDst, cfResult, I17_MIN, I17_MAX, iResult );

        DEBUG_IT_SAMPLE_BREAK(i, iResult, bp2Sin);
        piDst += cChan;

        RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData,
            uiSignbitCount, 1);

        bp2SinT = bp2Sin1 + MULT_BP2(bp2Step,bp2Cos);
        bp2CosT = bp2Cos1 - MULT_BP2(bp2Step,bp2Sin);
        bp2Sin1 = bp2Sin;  bp2Sin = bp2SinT;
        bp2Cos1 = bp2Cos;  bp2Cos = bp2CosT;
    }
    DEBUG_IT_SIN_INIT(5,bp2Sin,bp2Cos,bp2Sin1,bp2Cos1,bp2Step);

    ppcinfo->m_iCurrGetPCM_SubFrame = (I16)i;       //protect ourself from multiple calls

#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return WMA_OK;
}
#endif  // !defined(WMA_TARGET_X86) || defined(PREVOUTPUT_16BITS) || defined (BUILD_INTEGER) || defined (DISABLE_OPT)
#endif  // first SH4, then anything else



#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
void prvSetAdjustedValues(CAudioObject *pau, Int fQToo)
{
#if defined(HALF_TRANSFORM)
    if (pau->m_fHalfTransform)
    {
        pau->m_cSubbandAdjusted            = pau->m_cSubband>>1;
        pau->m_cFrameSampleAdjusted        = pau->m_cFrameSample>>1;
        pau->m_cFrameSampleHalfAdjusted    = pau->m_cFrameSampleHalf>>1;
        pau->m_cSubFrameSampleHalfAdjusted = pau->m_cSubFrameSampleHalf>>1;
        pau->m_cSubFrameSampleAdjusted     = pau->m_cSubFrameSample>>1;
        pau->m_cSubFrameSampleQuadAdjusted = pau->m_cSubFrameSampleQuad>>1;
        pau->m_cHighCutOffAdjusted         = pau->m_cHighCutOff>>1;
        pau->m_iSizePrev >>= 1;
        pau->m_iSizeCurr >>= 1;
        pau->m_iSizeNext >>= 1;
        if (fQToo)
        {   // normally only needed at initialization time
            pau->m_iCoefRecurQ1 >>= 1;
            pau->m_iCoefRecurQ2 >>= 1;
            pau->m_iCoefRecurQ3 >>= 1;
            pau->m_iCoefRecurQ4 >>= 1;
        }
    }
    else 
#endif // defined(HALF_TRANSFORM)
#if defined(PAD2X_TRANSFORM)
    if (pau->m_fPad2XTransform)
    {
        pau->m_cSubbandAdjusted            = pau->m_cSubband<<1;
        pau->m_cFrameSampleAdjusted        = pau->m_cFrameSample<<1;
        pau->m_cFrameSampleHalfAdjusted    = pau->m_cFrameSampleHalf<<1;
        pau->m_cSubFrameSampleHalfAdjusted = pau->m_cSubFrameSampleHalf<<1;
        pau->m_cSubFrameSampleAdjusted     = pau->m_cSubFrameSample<<1;
        pau->m_cSubFrameSampleQuadAdjusted = pau->m_cSubFrameSampleQuad<<1;
        pau->m_cHighCutOffAdjusted         = pau->m_cHighCutOff;    // unchanged
        pau->m_iSizePrev <<= 1;
        pau->m_iSizeCurr <<= 1;
        pau->m_iSizeNext <<= 1;
        if (fQToo)
        {   // normally only needed at initialization time
            pau->m_iCoefRecurQ1 <<= 1;
            pau->m_iCoefRecurQ2 <<= 1;
            pau->m_iCoefRecurQ3 <<= 1;
            pau->m_iCoefRecurQ4 <<= 1;
        }
    }
    else 
#endif // defined(PAD2X_TRANSFORM)
    {
        pau->m_cSubbandAdjusted            = pau->m_cSubband;
        pau->m_cFrameSampleAdjusted        = pau->m_cFrameSample; 
        pau->m_cFrameSampleHalfAdjusted    = pau->m_cFrameSampleHalf;
        pau->m_cSubFrameSampleAdjusted     = pau->m_cSubFrameSample;
        pau->m_cSubFrameSampleHalfAdjusted = pau->m_cSubFrameSampleHalf;
        pau->m_cSubFrameSampleQuadAdjusted = pau->m_cSubFrameSampleQuad;
        pau->m_cHighCutOffAdjusted         = pau->m_cHighCutOff;
    }
}
#endif // defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)


//*****************************************************************************************
//
// auAdaptToSubFrameConfig
// setup paramters for handling and transisting between varying size subframes
//
//*****************************************************************************************
WMARESULT auAdaptToSubFrameConfig (CAudioObject* pau)
{
    Int i, iTotal, iSizeCurr;
    PerChannelInfo* ppcinfo;
    WMARESULT   wmaResult = WMA_OK;

#ifdef PROFILE
    //FunctionProfile fp;
    //FunctionProfileStart(&fp,ADAPT_TO_SUB_FRAME_CONFIG_PROFILE);
#endif

    assert (pau->m_subfrmconfigCurr.m_cSubFrame <= 16);
    pau->m_iSizeCurr = iSizeCurr = pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];

    if (pau->m_subfrmconfigCurr.m_cSubFrame > 1) {
        pau->m_iIncr = LOG2 ((U32)(pau->m_cFrameSampleHalf / iSizeCurr));
        if (pau->m_iIncr >= pau->m_cPossibleWinSize)   {
            assert (WMAB_FALSE);
            wmaResult = TraceResult(WMA_E_BROKEN_FRAME);
            goto exit;
        }
        i = 0;
        iTotal = 0;
        while (i < pau->m_iIncr) {iTotal += (pau->m_cFrameSampleQuad >> i); i++;}

        pau->m_cValidBarkBand = pau->m_rgcValidBarkBand [pau->m_iIncr];
        pau->m_rgiBarkIndex   = pau->m_rgiBarkIndexOrig + pau->m_iIncr * (NUM_BARK_BAND + 1);
    }
    else {
        pau->m_iIncr = 0;
        pau->m_cValidBarkBand       = pau->m_rgcValidBarkBand [0];
        pau->m_rgiBarkIndex         = pau->m_rgiBarkIndexOrig;
    }
    pau->m_cLowCutOff  = pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame]
                    * pau->m_cLowCutOffLong / pau->m_cFrameSampleHalf;    //proportional
    pau->m_cHighCutOff = pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame]
                    * pau->m_cHighCutOffLong / pau->m_cFrameSampleHalf;    //proportional

    pau->m_cSubFrameSampleHalf   = pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];
    pau->m_cSubFrameSample       = pau->m_cSubFrameSampleHalf * 2;
    pau->m_cSubFrameSampleQuad   = pau->m_cSubFrameSampleHalf / 2; 
    pau->m_cSubband              = pau->m_cSubFrameSampleHalf;    //50% frame overlapping

    //init; could be modified by noise sub
    ppcinfo = pau->m_rgpcinfo;
    ppcinfo->m_cSubbandActual = pau->m_cHighCutOff - pau->m_cLowCutOff;
#ifndef ENCODER
    ppcinfo->m_rgiCoefRecon   = pau->m_rgiCoefReconOrig 
                              + DOUBLE(pau->m_fPad2XTransform,
                                    pau->m_cFrameSampleHalf - pau->m_cSubFrameSampleHalf);
    ppcinfo->m_rgfltCoefRecon = (Float *)(ppcinfo->m_rgiCoefRecon); 
#endif // ENCODER
    if (pau->m_cChannel == 2)   {
        ppcinfo = pau->m_rgpcinfo + 1;
        ppcinfo->m_cSubbandActual = pau->m_rgpcinfo [0].m_cSubbandActual;
#ifndef ENCODER
        ppcinfo->m_rgiCoefRecon   = pau->m_rgiCoefReconOrig 
                                  + DOUBLE(pau->m_fPad2XTransform,
                                        2 * pau->m_cFrameSampleHalf - pau->m_cSubFrameSampleHalf);
        ppcinfo->m_rgfltCoefRecon = (Float *)(ppcinfo->m_rgiCoefRecon); 
#endif // ENCODER
    }


#ifdef ENABLE_ALL_ENCOPT
    //update first noise index
    if (pau->m_fNoiseSub == WMAB_TRUE)    {
        pau->m_iFirstNoiseIndex = (Int) ftoi(0.5F + pau->m_fltFirstNoiseFreq * pau->m_cSubFrameSample 
            / ((Float) pau->m_iSamplingRate));  //open end
        if (pau->m_iFirstNoiseIndex > pau->m_cSubband) 
            pau->m_iFirstNoiseIndex = pau->m_cSubband;

        // use precalculated values
        pau->m_iFirstNoiseBand = pau->m_rgiFirstNoiseBand[pau->m_iIncr];
    }
#endif //ENABLE_ALL_ENCOPT

    //to decide the current window shape; look at sizes on the left and right

    pau->m_iSizePrev = (pau->m_iCurrSubFrame > 0) ? 
                        pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame - 1] :
                        pau->m_subfrmconfigPrev.m_rgiSubFrameSize [pau->m_subfrmconfigPrev.m_cSubFrame - 1];

    pau->m_iSizeNext = (pau->m_iCurrSubFrame < pau->m_subfrmconfigCurr.m_cSubFrame - 1) ? 
                        pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame + 1] :
                        pau->m_subfrmconfigNext.m_rgiSubFrameSize [0];


#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
    prvSetAdjustedValues(pau, WMAB_FALSE);
#endif

    //if the adjacent size is bigger; just keep your own shape
    //otherwise a transition window is needed.
    if (pau->m_iSizePrev >= pau->m_iSizeCurr) {
        //just forward copy curr
        pau->m_iCoefRecurQ1 = 0;
        pau->m_iCoefRecurQ2 = pau->m_iSizeCurr;
    }
    else  {
        //long start
        pau->m_iCoefRecurQ1 = (pau->m_iSizeCurr - pau->m_iSizePrev) / 2;
        pau->m_iCoefRecurQ2 = (pau->m_iSizeCurr + pau->m_iSizePrev) / 2;
    }

    if (pau->m_iSizeNext >= pau->m_iSizeCurr) {
        pau->m_iCoefRecurQ3 = pau->m_cSubFrameSampleHalfAdjusted;
        pau->m_iCoefRecurQ4 = pau->m_cSubFrameSampleAdjusted;
    }
    else    {
        //just backward copy curr
        pau->m_iCoefRecurQ3 = pau->m_cSubFrameSampleHalfAdjusted + (pau->m_iSizeCurr - pau->m_iSizeNext) / 2;
        pau->m_iCoefRecurQ4 = pau->m_cSubFrameSampleHalfAdjusted + (pau->m_iSizeCurr + pau->m_iSizeNext) / 2;
    }

    prvAdaptTrigToSubframeConfig(pau);

exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
    return wmaResult;
}



#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
WMARESULT prvAdaptTrigToSubframeConfig_INT(CAudioObject *pau)
{
    WMARESULT wmaResult = WMA_OK;
    const SinCosTable*  pSinCosTable;
    Int                 iSize;


    //if the adjacent size is bigger; just keep your own shape
    //otherwise a transition window is needed.
    if (pau->m_iSizePrev >= pau->m_iSizeCurr)
        iSize = pau->m_iSizeCurr;  //just forward copy curr
    else
        iSize = pau->m_iSizePrev;  //long start

#if 0 //def USE_SIN_COS_TABLES 
//marchold:I removed this code as I do not bieleve it to be necessary
    switch (iSize){
        case 2048:
            pau->m_piSinForRecon = pau->m_piSinForRecon2048;
            break;
        case 1024:
            pau->m_piSinForRecon = pau->m_piSinForRecon1024;
            break;
        case 512 :
            pau->m_piSinForRecon = pau->m_piSinForRecon512;
            break;
        case 256 :
            pau->m_piSinForRecon = pau->m_piSinForRecon256;
            break;
        case 128 :
            pau->m_piSinForRecon = pau->m_piSinForRecon128;
            break;
        case 64  :
            pau->m_piSinForRecon = pau->m_piSinForRecon64;
            break;
        default: 
            assert (0);
            return (TraceResult (WMA_E_BROKEN_FRAME));
    }
#endif  //maby else out next section

    // Use lookup-tables if we can
    if (iSize >= 64 && iSize <= 2048)
    {
        pSinCosTable = rgSinCosTables[iSize>>7];

        // START = sin( PI/(4*cSB) ) and cos( PI/(4*cSB) )
        pau->m_iSinRampUpStart =  pSinCosTable->sin_PIby4cSB>>1;    //(Int) (sin (0.5 * PI / iSizeCurr / 2) * 0x3FFFFFFF);
        pau->m_iCosRampUpStart =  pSinCosTable->cos_PIby4cSB>>1;    //(Int) (cos (0.5 * PI / iSizeCurr / 2) * 0x3FFFFFFF);

        // PRIOR should be sin(PI/(4*cSB) - PI/(2*cSB) ) = sin( -PI/(4*cSB) )
        pau->m_iSinRampUpPrior = -pSinCosTable->sin_PIby4cSB>>1;
        pau->m_iCosRampUpPrior =  pSinCosTable->cos_PIby4cSB>>1;
        pau->m_iSinRampUpStep  =  pSinCosTable->sin_PIby2cSB;   // STEP = 2 * sin (PI / 2 / iSizeCurr) * 0x3FFFFFFF;
    }
    else
    {
        pau->m_iSinRampUpStart =  (I32) (sin (0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iCosRampUpStart =  (I32) (cos (0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iSinRampUpPrior =  (I32) -(sin(0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iCosRampUpPrior =  (I32) (cos (0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iSinRampUpStep  =  (I32) (sin (PI / 2 / iSize) * NF2BP1);
    }

    // Include the next code-block to verify changes to the lookup-table
#ifdef _DEBUG
    assert(abs(pau->m_iSinRampUpStart - ((I32) (sin (0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iCosRampUpStart - ((I32) (cos (0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iSinRampUpPrior - ((I32) -(sin(0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iCosRampUpPrior - ((I32) (cos (0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iSinRampUpStep  - ((I32) (sin (PI / 2 / iSize) * NF2BP1))) <= 10000); // STEP = 2 * 
#endif


    
    if (pau->m_iSizeNext >= pau->m_iSizeCurr)
        iSize = pau->m_iSizeCurr;
    else
        iSize = pau->m_iSizeNext;  //just backward copy curr


    // Use lookup-tables if we can
    if (iSize >= 64 && iSize <= 2048)
    {
        pSinCosTable = rgSinCosTables[iSize>>7];

        // START = sin ((0.5 + cSB) * PI / (2*cSB) )
        //       = sin ( PI/(4*cSB) + PI/2 ) =  cos( PI/(4*cSB) )
        //         cos ((0.5 + cSB) * PI / (2*cSB) )
        //       = cos ( PI/(4*cSB) + PI/2 ) = -sin( PI/(4*cSB) )
        pau->m_iSinRampDownStart =  pSinCosTable->cos_PIby4cSB>>1;  //(Int) sin ((0.5 + iSizeNext) * PI / iSizeNext / 2);
        pau->m_iCosRampDownStart = -pSinCosTable->sin_PIby4cSB>>1;  //(Int) cos ((0.5 + iSizeNext) * PI / iSizeNext / 2);
        // PRIOR = sin ((0.5 + cSB) * PI / (2*cSB) -PI/(2*cSB) )
        //       = sin ( PI/(4*cSB) + PI/2 - PI/(2*cSB) ) = cos( -PI/(4*cSB) ) = cos( PI/(4*cSB) )
        //         cos ((0.5 + cSB) * PI / (2*cSB) -PI/(2*cSB) )
        //       = cos ( PI/(4*cSB) + PI/2 - PI/(2*cSB) ) =  -sin( -PI/(4*cSB) ) = sin( PI/(4*cSB) )
        pau->m_iSinRampDownPrior =  pSinCosTable->cos_PIby4cSB>>1;
        pau->m_iCosRampDownPrior =  pSinCosTable->sin_PIby4cSB>>1;
        pau->m_iSinRampDownStep  =  pSinCosTable->sin_PIby2cSB;     // STEP = 2 * sin (PI / 2 / iSizeCurr) * 0x3FFFFFFF;
    }
    else
    {
        pau->m_iSinRampDownStart =  (I32) (cos (0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iCosRampDownStart =  (I32) (-sin(0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iSinRampDownPrior =  (I32) (cos(0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iCosRampDownPrior =  (I32) (sin(0.5 * PI / iSize / 2) * NF2BP2);
        pau->m_iSinRampDownStep  =  (I32) (sin (PI / (2 * iSize)) * NF2BP1);
    }

    // Include the next code-block to verify changes to the lookup-table
#ifdef _DEBUG
    assert(abs(pau->m_iSinRampDownStart - ((I32) (cos (0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iCosRampDownStart - ((I32) (-sin(0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iSinRampDownPrior - ((I32) (cos(0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iCosRampDownPrior - ((I32) (sin(0.5 * PI / iSize / 2) * NF2BP2))) <= 10000);
    assert(abs(pau->m_iSinRampDownStep  - ((I32) (sin (PI / (2 * iSize)) * NF2BP1))) <= 10000);
#endif
    return wmaResult;
}
#endif  // defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)


#ifdef BUILD_INTEGER
WMARESULT prvAdaptTrigToSubframeConfig(CAudioObject *pau)
{
    WMARESULT wmaResult;
    ASSERTWMA_EXIT (wmaResult, prvAdaptTrigToSubframeConfig_INT(pau));
exit:
    return wmaResult;
}
#endif  // BUILD_INTEGER

//*****************************************************************************************
//
// auUpdateMaxEsc
//
//*****************************************************************************************
Void    auUpdateMaxEsc(CAudioObject* pau, Int iQuantStepSize)
{
    // HongCho: Adjust the max Tunstall level according to the quantization step...
    //          Matching if's in the decoder...
    //          Too many?  Maybe...
    // HongCho: Note...  For 22.05kHz, even with all 15bits, the bells don't ring...
    if(iQuantStepSize < 5)       pau->m_iMaxEscSize = 13;
    else if(iQuantStepSize < 15) pau->m_iMaxEscSize = 13;
    else if(iQuantStepSize < 32) pau->m_iMaxEscSize = 12;
    else if(iQuantStepSize < 40) pau->m_iMaxEscSize = 11;
    else if(iQuantStepSize < 45) pau->m_iMaxEscSize = 10;
    else if(iQuantStepSize < 55) pau->m_iMaxEscSize =  9;
    else                         pau->m_iMaxEscSize =  9;
    pau->m_iMaxEscLevel = (1<<pau->m_iMaxEscSize) - 1;
}


#if defined(INTERPOLATED_DOWNSAMPLE)

// Stolen from Algorithms in C, Sedgewick
Int GCD (Int u, Int v)
{
    Int t;

    while (u > 0)
    {
        if (u < v)
        {
            // Swap 'em
            t = u;
            u = v;
            v = t;
        }

        u = u - v;
    }

    return v;
}


void prvInterpolateInit(CAudioObject *pau, Int iSrcSampleRate,
                        Int iDstSampleRate, Int iAddInterval)
{
    Int iGCD;

    pau->m_iInterpolSrcBlkSize = iSrcSampleRate;
    pau->m_iInterpolDstBlkSize = iDstSampleRate;

    // Calculate the greatest common divisor between iSrcSampleRate
    // and iDstSampleRate and use to reduce iSrcBlkSize, iDstBlkSize
    iGCD = GCD(iSrcSampleRate, iDstSampleRate);
    if (0 != iGCD)
    {
        pau->m_iInterpolSrcBlkSize /= iGCD;
        pau->m_iInterpolDstBlkSize /= iGCD;
    }

    // Calculate pointer and difference increments to avoid div/mod ops
    pau->m_iInterpolIncr = pau->m_iInterpolSrcBlkSize / pau->m_iInterpolDstBlkSize;
    pau->m_iInterpolCarry = pau->m_iInterpolSrcBlkSize % pau->m_iInterpolDstBlkSize;

    // Calculate fixed-point increment to interpolation weight (x2 - x')
    pau->m_iInterpolWeightIncr = (I32) (-((float)pau->m_iInterpolSrcBlkSize /
        (float)pau->m_iInterpolDstBlkSize) * (1 << INTERPOL_FRACTBITS));

    pau->m_iInterpolAddInterval = iAddInterval;
    pau->m_iInterpolNextAdd = iAddInterval;
}


#ifdef BUILD_INTEGER
void auLowPass(CAudioObject *pau, Int *rgCoef, Int iNumCoefs)
{
    Int    *pEnd = rgCoef + iNumCoefs * 2; // Re and Im coefs (so times 2)
    Int    *pCurr;
#else   // BUILD_INTEGER
void auLowPass(CAudioObject *pau, Float *rgCoef, Int iNumCoefs)
{
    Float  *pEnd = rgCoef + iNumCoefs * 2; // Re and Im coefs (so times 2)
    Float  *pCurr;
#endif  // BUILD_INTEGER

    Int     iPassThruCoefs;

    // Figure out how many coefficients will pass through untouched
    iPassThruCoefs = (pau->m_iInterpolDstBlkSize * iNumCoefs) / 
        pau->m_iInterpolSrcBlkSize;
    pCurr = rgCoef + (2 * iPassThruCoefs);   // Re and Im coefs (so times 2)
    iNumCoefs -= iPassThruCoefs;

    while (iNumCoefs > 0)
    {
        *pCurr++ = 0;       // Re coef
        *pCurr++ = 0;       // Im coef
        iNumCoefs -= 1;
    }

    assert(pCurr == pEnd);
}
#endif  // defined(INTERPOLATED_DOWNSAMPLE)


#ifdef INTERPOLATED_DOWNSAMPLE
// We assume piSrc and piDst point to I16, channel-interleaved buffers
void prvInterpolate(CAudioObject *pau, I16 *piSourceBuffer, Int iNumSrcBytes,
                    Int iChannels, I16 *piDestBuffer, Int *piNumDstBytes, U16* pcSamples)
{
    Int iCurrChannel;
    Int iNewNextAdd = pau->m_iInterpolNextAdd;

    // Check if source buffer is a multiple of indivisible src blk size
    assert(0 == (iNumSrcBytes % pau->m_iInterpolSrcBlkSize));

    *piNumDstBytes = 0;
    *pcSamples = 0;
    for (iCurrChannel = 0; iCurrChannel < iChannels; iCurrChannel += 1)
    {
        const Int c_iInterpolationBlks = iNumSrcBytes /
            (iChannels * pau->m_iInterpolSrcBlkSize * sizeof(I16));

        I16    *piPrevSrc = piSourceBuffer + iCurrChannel;
        I16    *piSrc = piSourceBuffer + iCurrChannel + iChannels;
        I16    *piDst = piDestBuffer + iCurrChannel;
        Int     iNextAdd = pau->m_iInterpolNextAdd;
        Int     i;
        Bool    fAddExtraSample;

        // If we are not to add extra samples, set us up so iNextAdd never hits 0
        if (0 == iNextAdd)
        {
            iNextAdd = (c_iInterpolationBlks * pau->m_iInterpolDstBlkSize) + 1;
            fAddExtraSample = WMAB_FALSE;
        }
        else
            fAddExtraSample = WMAB_TRUE;

        for (i = 0; i < c_iInterpolationBlks; i++)
        {
            I32     fiWeight = (1 << INTERPOL_FRACTBITS);
            Int     iPtrIncrCarry = 0;
            Int     j;

#ifdef _DEBUG
            const Float c_fltWeightIncr = -((float)pau->m_iInterpolSrcBlkSize /
                (float)pau->m_iInterpolDstBlkSize);
            Float   fltWeight = 1.0f;
            Float   fltResult;
            Float   fltDiff;
            Float   fltRelErr;

            fltDiff = c_fltWeightIncr - ((float) pau->m_iInterpolWeightIncr /
                (float) (1 << INTERPOL_FRACTBITS));
            fltRelErr = fltDiff / c_fltWeightIncr;
            assert(0 == fltDiff || fltRelErr < 0.1f);
#endif  // _DEBUG

            for (j = 0; j < pau->m_iInterpolDstBlkSize; j++)
            {
                Int iSrcIncr;

                // Pre-read prev and next src to help compiler optimization
                // and to allow debug code to verify, since interpolation is in-place
                const I16   iPrevSrc = *piPrevSrc;
                const I16   iNextSrc = *piSrc;

                // Interpolate output sample (No rounding. I tried rounding but
                // it makes little difference and it adds 2% to computation time)
                *piDst = (I16) (MULT_HI_DWORD(fiWeight, (iPrevSrc -
                    iNextSrc) << (32 - INTERPOL_FRACTBITS)) + iNextSrc);

                // Add an extra sample if it's time to do so
                iNextAdd -= 1;
                if (iNextAdd <= 0)
                {
                    assert(iNextAdd == 0);
                    assert(fAddExtraSample);
                    *(piDst + iChannels) = *piDst;
                    piDst += iChannels;
                    iNextAdd = pau->m_iInterpolAddInterval;
                }

#ifdef _DEBUG
                fltResult = (I16) ROUNDF(fltWeight * (iPrevSrc - iNextSrc) + iNextSrc);
                fltDiff = fltResult - *piDst;
                fltRelErr = fltDiff / fltResult;
                assert(fabs(fltDiff) <= 1.0f || fabs(fltRelErr) < 0.1f);
#endif  // _DEBUG

                // Check if we need to carry
                iSrcIncr = pau->m_iInterpolIncr;
                iPtrIncrCarry += pau->m_iInterpolCarry;
                if (iPtrIncrCarry >= pau->m_iInterpolDstBlkSize)
                {
                    // Perform carry
                    iSrcIncr += 1;
                    iPtrIncrCarry -= pau->m_iInterpolDstBlkSize;
                }

                // Update pointers and vars
                piPrevSrc += iSrcIncr * iChannels;
                piSrc += iSrcIncr * iChannels;
                piDst += iChannels;

                // Now calculate the next weighting
                fiWeight += pau->m_iInterpolWeightIncr + (iSrcIncr << INTERPOL_FRACTBITS);

#ifdef _DEBUG
                fltWeight += c_fltWeightIncr + iSrcIncr;
                fltDiff = fltWeight - ((float)fiWeight / (float)(1 << INTERPOL_FRACTBITS));
                fltRelErr = fltDiff / fltWeight;
                assert(0 == fltDiff || fabs(fltRelErr) < 0.1f);
#endif  // _DEBUG

            } // for (interpolation block)
        } // for (entire source block)

        // Output number of samples outputted after interpolation
        *piNumDstBytes += sizeof(I16) * (piDst - iCurrChannel - piDestBuffer) / iChannels;

        // Record new countdown to next extra sample, unless we're not to add new samples
        if (0 == iCurrChannel && fAddExtraSample)
            iNewNextAdd = iNextAdd;

        assert(WMAB_FALSE == fAddExtraSample || iNextAdd == iNewNextAdd);

    } // for (all channels)

    *pcSamples = (*piNumDstBytes) / ( sizeof(I16) * iChannels );
    assert(*piNumDstBytes % (iChannels * sizeof(I16)) == 0);
    assert( *piNumDstBytes == (Int)(sizeof(I16) * (*pcSamples) * iChannels) );
    pau->m_iInterpolNextAdd = iNewNextAdd;

} // prvInterpolate

#endif  // INTERPOLATED_DOWNSAMPLE



//*****************************************************************************************
//
// qstCalcQuantStep
// like auCalcQuantStep but returns a QuantStepType which is either:
//   a FastFloat (for integer builds), a Float (Decoder) or a Double(Encoder)
//
//*****************************************************************************************
QuantStepType qstCalcQuantStep(Int iQSS) 
{
    QuantStepType qstQuantStep;

#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
    if ( iQSS < DBPOWER_TABLE_OFFSET )
    {   // This happens when iPower==0, not an important case, but return 10^(1/20) with 28 FractBits
        // It can also happen with a small NoisePower (-13 has been seen)
        if ( iQSS < 0 )
        {   // negative values of iQSS are being generated in the V5 encoder (LowRate in particular)
            qstQuantStep.iFraction = (I32)(0.382943866392*(1<<QUANTSTEP_FRACT_BITS)),      // Average Fraction
            qstQuantStep.iFracBits = QUANTSTEP_FRACT_BITS - ((-iQSS>>3)+1);                // Approximate Exponent
        }
        else
        {
            qstQuantStep.iFraction = (I32)(0.869439785679*(1<<QUANTSTEP_FRACT_BITS));     // Average Fraction
            qstQuantStep.iFracBits = QUANTSTEP_FRACT_BITS - ((iQSS>>3)+1);                // Approximate Exponent
        }
    }
    else if ( iQSS < (DBPOWER_TABLE_OFFSET+DBPOWER_TABLE_SIZE) )
    {   
        // *** normal case ***
        qstQuantStep.iFraction = rgDBPower10[ iQSS - DBPOWER_TABLE_OFFSET ];
        qstQuantStep.iFracBits = QUANTSTEP_FRACT_BITS - ((iQSS>>3)+4);    // implied FractBit scale for rgiDBPower table
        NormUInt( (UInt*)(&qstQuantStep.iFraction), &qstQuantStep.iFracBits, 0x3FFFFFFF );
    }
    else
    {   // This branch can handle out-of-range cases. 
        // rare - but used in some cases by encoder - e.g. Tough_16m_16, Tough_22m_22(?).
        const double    c_dblTenExpToTwo = 3.3219280948873623478703194294894; // ln(10)/ln(2);
        const double    c_dblPointNineNine = (1.0 - DBL_EPSILON);
        double  dblQuantStep,  dblExponent;
        Int     iNumWholeBits,  iNumFractBits;
        dblExponent = (double)(iQSS/(Float)QUANTSTEP_DENOMINATOR);
        dblQuantStep = pow (10, dblExponent);
        iNumWholeBits = (Int)(dblExponent * c_dblTenExpToTwo + c_dblPointNineNine); // Round UP
        iNumWholeBits = max(iNumWholeBits, 5);
        iNumFractBits = max(31 - iNumWholeBits, TRANSFORM_FRACT_BITS);
        if (dblQuantStep * (1 << iNumFractBits) <= UINT_MAX)
            qstQuantStep.iFraction = (I32)(dblQuantStep * (1 << iNumFractBits));
        else
            qstQuantStep.iFraction = UINT_MAX;      // iQSS of 218 seen in encoding Tough 16kHz Stereo 16kbps
        qstQuantStep.iFracBits = iNumFractBits;
        NormUInt( (UInt*)(&qstQuantStep.iFraction), &qstQuantStep.iFracBits, 0x3FFFFFFF );
    }
    MONITOR_RANGE( gMR_qstQuantStep, (qstQuantStep.iFraction)/(Double)(1 << qstQuantStep.iFracBits) );    

#else   // must be BUILD_INT_FLOAT or float encoder

    if ( iQSS < DBPOWER_TABLE_OFFSET )
    {   // This happens when iPower==0, not an important case, but return 10^(1/20) with 28 FractBits
        // It can also happen with a small NoisePower (-13 has been seen)
        if ( iQSS < 0 )
        {   // negative values of iQSS are being generated in the V5 encoder (LowRate in particular)
            qstQuantStep = 0.382943866392f,      // Average Fraction
            qstQuantStep /= (Float)(1<<((-iQSS>>3)+1));                // Approximate Exponent
        }
        else
        {
            qstQuantStep = 0.869439785679f;             // Average Fraction
            qstQuantStep *= (Float)(1<<((iQSS>>3)+1));      // Approximate Exponent
        }
    }
    else if ( iQSS < (DBPOWER_TABLE_OFFSET+DBPOWER_TABLE_SIZE) )
    {   // *** normal case ***
        // implied FractBit scale for rgiDBPower table
        qstQuantStep = rgDBPower10[ iQSS - DBPOWER_TABLE_OFFSET ] * rgfltDBPowerExponentScale[iQSS>>3];
    }
    else
    {   // This branch can handle out-of-range cases. 
        // rare - but used in some cases by encoder - e.g. Tough_16m_16.
        // iQSS of 218 seen in encoding Tough 16kHz Stereo 16kbps
        qstQuantStep = (QuantStepType)pow (10, (double)(iQSS/(Float)QUANTSTEP_DENOMINATOR) );
    }
    MONITOR_RANGE( gMR_qstQuantStep, qstQuantStep );

#endif

    return( qstQuantStep );
}


//*****************************************************************************************
//
// prvWeightedQuantization
// calculate 10^( (MaskQ-MaxMaskQ)*2.5*0.5/20 ) * 10^( QuantStepSize/20 )
//         = (10^(1/16))^( MaskQ-MaxMaskQ )     * (10^(1/20)^QuantStepSize
//*****************************************************************************************
QuantFloat prvWeightedQuantization(CAudioObject *pau, PerChannelInfo *ppcinfo, Int iBark)
{
    Int iIndex = ppcinfo->m_iMaxMaskQ - ppcinfo->m_rgiMaskQ[iBark];
    QuantFloat qfltRMS;
    assert( 0 <= iIndex );

    if (iIndex >= MASK_MINUS_POWER_TABLE_SIZE)
        iIndex = MASK_MINUS_POWER_TABLE_SIZE-1;
#if defined(BUILD_INTEGER) ||defined(INTEGER_ENCODER)
    {
        Int uiFraction, iFracBits;
        uiFraction = rgiMaskMinusPower10[ iIndex ];     // with MASK_POWER_FRAC_BITS==28 fractional bits
        iFracBits = MASK_POWER_FRAC_BITS+(iIndex>>2);
        MONITOR_RANGE(gMR_WeightRatio,(float)uiFraction/pow(2,iFracBits));
#       ifdef _DEBUG
        {
            Float realRMS = (Float)pow(10,(float)(ppcinfo->m_rgiMaskQ[iBark] - ppcinfo->m_iMaxMaskQ)/(1<<4));
            Float Calculated = (float)uiFraction/(float)pow(2,iFracBits); 
            Float diff = (Float)fabs(realRMS-Calculated);
            Float rel = (Float)fabs(diff/realRMS);
            if (rel > .00002)
                DEBUG_BREAK();
        }
#       endif
        qfltRMS.iFraction = MULT_HI(pau->m_qstQuantStep.iFraction,uiFraction);  
        qfltRMS.iFracBits = pau->m_qstQuantStep.iFracBits + iFracBits - 31;
        Norm4FastFloat( &qfltRMS );
    }

#else

    qfltRMS = rgiMaskMinusPower10[ iIndex ] / (Float)(1<<(iIndex>>2));
    MONITOR_RANGE(gMR_WeightRatio,qfltRMS);
    qfltRMS *= FLOAT_FROM_QUANTSTEPTYPE( pau->m_qstQuantStep );  

#endif

    return qfltRMS;
}

//*****************************************************************************************
//
// VERIFY_DECODED_COEFFICENT
//
// define VERIFY_DECODED_COEFS and set fltCoefIntFloatThreshold
//
//*****************************************************************************************
//#define VERIFY_DECODED_COEFS
#if defined(_DEBUG) && defined(VERIFY_DECODED_COEFS)
#   pragma COMPILER_MESSAGE(__FILE__ "(1235) : Warning - Decode Coef HighRate Debug Code Enabled.")
// define threshold ((it can be changed from debugger))
static Float fltCoefIntFloatThreshold = 0.00075F;
static void VerifyDecodedCoefficentHR(CAudioObject* pau, PerChannelInfo* ppcinfo, 
                                          Int iBark, CoefType iRecon, I16 iLevel, CoefType ctCoefRecon) {
    Float ff, f3, dif, rel, fltRecon;
    static Float fltReconMin = 1.0e30F;
    static Float fltReconMax = -1;
    Double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);
    Float fltWeightFactor = ((float)ppcinfo->m_rgiWeightFactor[iBark])  / (1<<WEIGHTFACTOR_FRACT_BITS);     // divide by 1024.
    Float fltInvMaxWeight = 1.0F / ((float)ppcinfo->m_iMaxWeight / (1<<WEIGHTFACTOR_FRACT_BITS));           // didive by 1024.
    fltRecon = FLOAT_FROM_COEF(ctCoefRecon);                                                                // divide by 32.
    // This is the statement in float.c we are to match
    // rgfltCoefRecon [iRecon] = ((Float) rgiCoefQ [iCoefQ]) * rgfltWeightFactor [iBark] * ((Float) dblQuantStep) * fltInvMaxWeight;
    ff = fltWeightFactor * ((Float) dblQuantStep) * fltInvMaxWeight;
    f3 = ((Float) iLevel) * ff;
    dif = (float) fabs(f3 - fltRecon);  
    rel = (float) fabs(dif/f3); 
    // was ((iRecon < pau->m_cHighCutOff) && (dif > 2) && rel > 0.2)
    // fltRecon seems to range from 1e7 down to about 1.0, small values seem to be less accurate
    // consider using the same comparison as used with MidRate (See LowRate.c)
   if ( rel > (f3 > 100 ? fltCoefIntFloatThreshold : (fltCoefIntFloatThreshold*100/f3)) )   
        DEBUG_BREAK();
}
#define VERIFY_DECODED_COEFFICENT(idxBark) VerifyDecodedCoefficentHR(pau, ppcinfo, idxBark, iRecon, iLevel, ctCoefRecon)
#else
#define VERIFY_DECODED_COEFFICENT(idxBark)
#endif


// *************************************************************************************
// DEBUG_BREAK at a particular Frame in auReconCoefficentsHighRate
//
//#define DECODE_COEF_AT_FRAME 435
//#define DECODE_COEF_AT_SUBFRAME 0
//
#if defined(_DEBUG) && ( defined(DECODE_COEF_AT_FRAME) || defined(DECODE_COEF_AT_SUBFRAME) )
#   pragma COMPILER_MESSAGE(__FILE__ "(1154) : Warning - Decode Coefficient Debug Code Enabled.")
#   if defined(DECODE_COEF_AT_FRAME) && defined(DECODE_COEF_AT_SUBFRAME)
#       define DEBUG_BREAK_AT_FRAME_DECODE                                                          \
            if (  ( pau->m_iFrameNumber == DECODE_COEF_AT_FRAME || DECODE_COEF_AT_FRAME < 0 )       \
                &&( pau->m_iCurrSubFrame == DECODE_COEF_AT_SUBFRAME || DECODE_COEF_AT_SUBFRAME < 0 ) ) \
                DEBUG_BREAK();              
#   elif defined(DECODE_COEF_AT_FRAME)
#       define DEBUG_BREAK_AT_FRAME_DECODE                                                          \
            if ( pau->m_iFrameNumber == DECODE_COEF_AT_FRAME || DECODE_COEF_AT_FRAME < 0 )          \
                DEBUG_BREAK();              
#   else
#       define DEBUG_BREAK_AT_FRAME_DECODE                                                          \
            if ( pau->m_iCurrSubFrame == DECODE_COEF_AT_SUBFRAME || DECODE_COEF_AT_SUBFRAME < 0 )   \
                DEBUG_BREAK();
#   endif               
#else   // defined(_DEBUG) && defined(DECODE_COEF_AT_FRAME)
#   define DEBUG_BREAK_AT_FRAME_DECODE
#endif  // defined(_DEBUG) && defined(DECODE_COEF_AT_FRAME)


//#define RL_PRINT_SRC
#if defined(_DEBUG) && defined(RL_PRINT_SRC) && !defined(WMA_MONITOR)
#   define DBG_RUNLEVEL(a,b,c,d,e)                          \
        if(e) {                                             \
            printf("%d %d %d %d\n", a, b, c, d+1);          \
            fflush (stdout);                                \
        }
#elif defined(_DEBUG) && defined(RL_PRINT_SRC) && defined(WMA_MONITOR)
#   define DBG_RUNLEVEL(a,cRunOfZeros,iLevel,iSign,e)                       \
        if(e) {                                                             \
            printf("%d %d %d %d\n", a, b, c, d+1);                          \
            fflush (stdout);                                                \
        }                                                                   \
        MONITOR_RANGE(gMR_iCoefQ,(iLevel^iSign)-iSign);                     \
        MONITOR_COUNT_CONDITIONAL(cRunOfZeros>0,gMC_0CoefQ,cRunOfZeros);    \
        {   int ii;                                                         \
            for( ii = 0; ii < cRunOfZeros; ii++ )                           \
                MONITOR_RANGE(gMR_CoefRecon,0);                             \
            MONITOR_COUNT(gMC_zeroCoefRecon,cRunOfZeros);                   \
        }
#elif defined(_DEBUG) && !defined(RL_PRINT_SRC) && defined(WMA_MONITOR)
#   define DBG_RUNLEVEL(a,cRunOfZeros,iLevel,iSign,e)                       \
        MONITOR_RANGE(gMR_iCoefQ,(iLevel^iSign)-iSign);                     \
        MONITOR_COUNT_CONDITIONAL(cRunOfZeros>0,gMC_0CoefQ,cRunOfZeros);    \
        {   int ii;                                                         \
            for( ii = 0; ii < cRunOfZeros; ii++ )                           \
                MONITOR_RANGE(gMR_CoefRecon,0);                             \
            MONITOR_COUNT(gMC_zeroCoefRecon,cRunOfZeros);                   \
        }
#else
#   define DBG_RUNLEVEL(a,b,c,d,e)
#endif 


// DecodeCoefsHighRate with combined INTEGER and INT_FLOAT code

#ifdef BUILD_INTEGER

#define MULT_QUANT(iLevel,ffltQuantizer) MULT_HI(((iLevel)<<16),ffltQuantizer.iFraction)

// SCALE_COEF_RECON shifts CoefRecon to give it TRANSFORM_FRACT_BITS==5 fractional bits
#if defined(PLATFORM_OPTIMIZATION_MINIMIZE_BRANCHING)
    // This SCALE_COEF_RECON macro requires 6 ops and no branches
    // This MAKE_MASK_FOR_SCALING requires 5 ops plus 1 branch.
    // SCALE_COEFFICENT gets executed 25x as often as MAKE_MASK_FOR_SCALING, so this method requires 6.2 ops plus 0.04 branches per SCALE_COEFFICENT
#   define SCALE_COEF_RECON(iCR) (((iCR>>iShift) & iMask2) | ((iCR<<-iShift) & iMask1))
#   define MAKE_MASK_FOR_SCALING(iFB) iShift=iFB-(TRANSFORM_FRACT_BITS+31-16);                      \
                                      iMask2 = 0xFFFFFFFF ^ (iMask1 = (iShift<0) ? 0xFFFFFFFF : 0);
#else
    // When branching is not a high penaty activity, do it the simplier way
    //   iCoefRecon = (t=iFracBits-5)<0 ? iCoefRecon>>-t : iCoefRecon<<t
    // This SCALE_COEF_RECON requires 3 ops plus 1 branch or 2 ops plus 1 branch.  
    // This MAKE_MASK_FOR_SCALING requires 2 ops
    // SCALE_COEFFICENT gets executed 25x as often as MAKE_MASK_FOR_SCALING, so this method requires 2.58 ops plus 0.04 branches per SCALE_COEFFICENT
    // On one test on a 500 MHz Pentium 686, This way saves 1% execution time over masking.
#   define SCALE_COEF_RECON(iCR) (iShift<0) ? (iCR)<<-iShift : (iCR)>>iShift
#   define MAKE_MASK_FOR_SCALING(iFB) iShift=iFB+(16-31-TRANSFORM_FRACT_BITS)
#endif

#else

#define MULT_QUANT(iLevel,ffltQuantizer) (iLevel*ffltQuantizer)
#define SCALE_COEF_RECON(iCR) (iCR)
#define MAKE_MASK_FOR_SCALING(iFB)

#endif


///*****************************************************************************************
//
// auReconCoefficentsHighRate  - Integer or IntFloat Version
//
//*****************************************************************************************
//wchen: this function is not threadsafe!!
WMARESULT auReconCoefficentsHighRate (CAudioObject* pau, Void* pcaller, PerChannelInfo* ppcinfo, Int* piBitCnt)
{
    WMARESULT   wmaResult = WMA_OK;

    CoefType* rgiCoefRecon   = (CoefType*) ppcinfo->m_rgiCoefRecon;
    CoefType ctCoefRecon;
    Int iMaskResampleRatio, cValidBarkBandLatestUpdate;
    Int iBark = 0;
    I16* piRecon = &pau->m_iCurrReconCoef;
#if defined(PLATFORM_OPTIMIZATION_MINIMIZE_BRANCHING)
    Int iMask1,iMask2;
#endif
    INTEGER_ONLY( Int iShift; )
    Int iMaskResampleRatioPow;
    const Int *rgiBarkIndex;
    Int *piHighCutOff = &pau->m_iHighCutOffCurr;
    Int iHighToBeZeroed;
    QuantFloat qfltQuantizer;   // eithe a FastFloat or a Float, as appropriate to the build.
    Bool fPrint = WMAB_FALSE;
#ifdef PROFILE
    FunctionProfile fp;
    FunctionProfileStart(&fp,DECODE_COEFFICENTS_HIGH_RATE_PROFILE);
#endif

    assert (!pau->m_fNoiseSub &&  pau->m_iWeightingMode == BARK_MODE);
    assert (pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame] != 0);
    assert (pau->m_cFrameSampleHalf <= (1<<12));                                            
    assert (pau->m_iSubFrameSizeWithUpdate != 0);
    DEBUG_BREAK_AT_FRAME_DECODE;

    if (pau->m_iSubFrameSizeWithUpdate <= 0 || pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame] <= 0)
        wmaResult = WMA_E_BROKEN_FRAME;
    CHECKWMA_EXIT (wmaResult);

    iMaskResampleRatio = (pau->m_iSubFrameSizeWithUpdate << 12) /                           
                          pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];    
    iMaskResampleRatioPow = LOG2(iMaskResampleRatio);
    rgiBarkIndex       = pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) *                    
                         LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate);   
    cValidBarkBandLatestUpdate = pau->m_rgcValidBarkBand [LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate)];

    //// DEBUG NOTES below are preceeded by four slashes and typically allow cut and paste so you can view scaled integers as floats
    //// They are on the line below where the calculation occurs - of course, with BUILD_INT_FLOAT, they are unnecessary

    // zero all coefs so we can just skip the many zero ones as we detect them below
    // note 70% of coefficents are zero in High Rate
    // wchen: moved to outside
    //memset (rgiCoefRecon , 0, sizeof (CoefType) * pau->m_cHighCutOff);//(iRun+pau->m_cLowCutOff));                     

    iBark = 0;
    pau->m_iLevel = 0;
    *piHighCutOff = ppcinfo->m_cSubbandActual-1;//values need to be offset by -1 too 
    pau->m_iNextBarkIndex = -1; 

    //Scan for the first bark index = note iRecon is 0 and rgiBarkIndex[1] ==0 for 16000 Hz and 11025 Hz frames with 128 oir 64 samples
    while ((((*piRecon) * iMaskResampleRatio) >> 12) >= rgiBarkIndex [iBark+1]) 
        ++iBark;
        
    TRACEWMA_EXIT (wmaResult, (*pau->aupfnGetNextRun) (pcaller, ppcinfo, piBitCnt));
    DBG_RUNLEVEL(g_cBitGet-21,pau->m_cRunOfZeros,pau->m_iLevel,pau->m_iSign,fPrint);
    *piRecon += (I16)pau->m_cRunOfZeros+1;

    while (*piRecon < *piHighCutOff && iBark < cValidBarkBandLatestUpdate) {

        //Search for the next bark index
        while ((((*piRecon) * iMaskResampleRatio) >> 12) >= rgiBarkIndex [iBark+1]) 
            ++iBark;

        if ( iBark >= cValidBarkBandLatestUpdate )
        {
            assert( iBark < cValidBarkBandLatestUpdate );
            break;
        }

        // Get the quantStep * (10^(1/16))^(MaxMaskQ-MaskQ[iRecon])
        // as qfltQuantizer.fraction/(1<<(23-qfltQuantizer.exponent))
        // then scale coefficent to give it five FracBits
        if (*piRecon == pau->m_iNextBarkIndex) {                               
            //Reconstruct the coefficent before getting the next weighting factor if it lies at the end of a bark band        
            ctCoefRecon = MULT_QUANT(pau->m_iLevel,qfltQuantizer);
            //// Unsigned Float CoefRecon = ctCoefRecon/(1.0F*(1<<(qfltQuantizer.iFracBits+16-31)))
            INTEGER_ONLY( ctCoefRecon = SCALE_COEF_RECON(ctCoefRecon) );
            //// Unsigned Float CoefRecon = ctCoefRecon/32.0F
            VERIFY_DECODED_COEFFICENT(iBark-1);
            qfltQuantizer = prvWeightedQuantization(pau,ppcinfo,iBark);
            //// Float Quantizer = qfltQuantizer.iFraction/(1024.0F*(1<<(qfltQuantizer-10)))
            MAKE_MASK_FOR_SCALING(qfltQuantizer.iFracBits);
        } else {
            //Otherwize get the next weighting factor first
            assert( *piRecon > pau->m_iNextBarkIndex );
            qfltQuantizer = prvWeightedQuantization(pau,ppcinfo,iBark);         
            //// Float Quantizer = qfltQuantizer.iFraction/(1024.0F*(1<<(qfltQuantizer-10)))
            MAKE_MASK_FOR_SCALING(qfltQuantizer.iFracBits);
            ctCoefRecon = MULT_QUANT(pau->m_iLevel,qfltQuantizer);
            //// Unsigned Float CoefRecon = ctCoefRecon/(1.0F*(1<<(qfltQuantizer.iFracBits+16-31)))
            INTEGER_ONLY( ctCoefRecon = SCALE_COEF_RECON(ctCoefRecon) );
            //// Unsigned Float CoefRecon = ctCoefRecon/32.0F
            VERIFY_DECODED_COEFFICENT(iBark);
        }

        //Calculate the index of the end of this bark band
        if (iMaskResampleRatioPow > 12){
            pau->m_iNextBarkIndex = (rgiBarkIndex [iBark + 1] + (1<<(iMaskResampleRatioPow-13))) >> (iMaskResampleRatioPow-12);
        } else {
            pau->m_iNextBarkIndex = (rgiBarkIndex [iBark + 1] << (12-iMaskResampleRatioPow));
        }
        pau->m_iNextBarkIndex--; //correct by -1
        if (pau->m_iNextBarkIndex > *piHighCutOff) 
            pau->m_iNextBarkIndex = *piHighCutOff;

        do {
            rgiCoefRecon [*piRecon] = INTEGER_OR_INT_FLOAT( (ctCoefRecon^pau->m_iSign)-pau->m_iSign, 
                                    pau->m_iSign ? -ctCoefRecon : ctCoefRecon );
            //// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F
            MONITOR_RANGE(gMR_CoefRecon,FLOAT_FROM_COEF(rgiCoefRecon[*piRecon]));
            MONITOR_COUNT_CONDITIONAL(rgiCoefRecon[*piRecon]==0,gMC_zeroCoefRecon,pau->m_cRunOfZeros);
            TRACEWMA_EXIT (wmaResult, (*pau->aupfnGetNextRun) (pcaller, ppcinfo, piBitCnt));
            DBG_RUNLEVEL(g_cBitGet-21,pau->m_cRunOfZeros,pau->m_iLevel,pau->m_iSign,fPrint);
            *piRecon += (I16)pau->m_cRunOfZeros+1;
            if (*piRecon >= pau->m_iNextBarkIndex) 
                break;
            ctCoefRecon = MULT_QUANT(pau->m_iLevel,qfltQuantizer);
            INTEGER_ONLY( ctCoefRecon = SCALE_COEF_RECON(ctCoefRecon) );
            VERIFY_DECODED_COEFFICENT(iBark);
        } while (WMAB_TRUE);   
        iBark++;
    }
    if (*piRecon == *piHighCutOff) {
        if ( *piRecon >= pau->m_iNextBarkIndex  )
        {   // skipped here via a cRunOfZeros past one or more iBark increments
            while ( ((iBark-1) < cValidBarkBandLatestUpdate) && (((*piRecon) * iMaskResampleRatio) >> 12) >= rgiBarkIndex [iBark] ) 
                ++iBark;
            if ( (iBark-1) <= cValidBarkBandLatestUpdate )
            {   
                qfltQuantizer = prvWeightedQuantization(pau,ppcinfo,iBark-1);         
                //// Float Quantizer = qfltQuantizer.iFraction/(1024.0F*(1<<(qfltQuantizer-10)))
                MAKE_MASK_FOR_SCALING(qfltQuantizer.iFracBits);
            }
        }
        else
        {
            assert(WMAB_FALSE);
        }
        ctCoefRecon = MULT_QUANT(pau->m_iLevel,qfltQuantizer);
        //// Unsigned Float CoefRecon = ctCoefRecon/(1.0F*(1<<(qfltQuantizer.iFracBits+16-31)))
        INTEGER_ONLY( ctCoefRecon = SCALE_COEF_RECON(ctCoefRecon) );
        //// Unsigned Float CoefRecon = ctCoefRecon/32.0F
        VERIFY_DECODED_COEFFICENT(iBark-1);
        rgiCoefRecon [*piRecon] = INTEGER_OR_INT_FLOAT( (ctCoefRecon^pau->m_iSign)-pau->m_iSign, 
                                pau->m_iSign ? -ctCoefRecon : ctCoefRecon );
        //// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F
        MONITOR_RANGE(gMR_CoefRecon,FLOAT_FROM_COEF(rgiCoefRecon[*piRecon]));
        MONITOR_COUNT_CONDITIONAL(rgiCoefRecon[*piRecon]==0,gMC_zeroCoefRecon,pau->m_cRunOfZeros);
    }
    assert (*piRecon <= pau->m_cSubband); 
    assert (iBark <=  cValidBarkBandLatestUpdate );

    // do low cutoff here so there is less branching in the above loop
    if ( pau->m_cLowCutOff > 0 )
    {
        memset (rgiCoefRecon, 0, sizeof (Int) * pau->m_cLowCutOff);
#       if defined(_DEBUG) && defined(WMA_MONITOR)
        {   int ii;
            for( ii = 0; ii < pau->m_cLowCutOff; ii++ )
                MONITOR_RANGE(gMR_CoefRecon,0);
        }
#       endif
    }

    //do high cutoff here 
    iHighToBeZeroed = sizeof(CoefType) * (pau->m_cSubbandAdjusted - pau->m_cHighCutOffAdjusted);
    memset (rgiCoefRecon + pau->m_cHighCutOffAdjusted, 0, iHighToBeZeroed);    
#   if defined(_DEBUG) && defined(WMA_MONITOR)
    {   
    int ii;
    for( ii = 0; ii < iHighToBeZeroed; ii += sizeof(Int) )
        MONITOR_RANGE(gMR_CoefRecon,0);
    }
#   endif

exit:
#ifdef PROFILE
    FunctionProfileStop(&fp);
#endif
    return wmaResult;
}

#undef SCALE_COEFFICENT 
#undef MAKE_MASK_FOR_SCALING
#undef MULT_QUANT


void prvInitDiscardSilence(CAudioObject *pau)
{
    // If this is the very first frame, we need to determine if we are at the
    // start-of-file, rather than seeking. If so, we need to discard the silence
    // frames. If not, we need to only discard half a subframe. NOTE that for
    // V4 encoded streams, WE WILL GUESS INCORRECTLY. Our justification for accepting
    // this is that V4 never had timestamps and so sync was never guaranteed anyway.
    Bool fStartOfStream = WMAB_TRUE;
    int i;

    assert(CODEC_BEGIN == pau->m_codecStatus);

    // Due to SCRUNCH bug #32, v5 encoder forces fMaskUpdate to TRUE and
    // all channels' m_iPower to 1 in the very first frame only. If we find that
    // fMaskUpdate, m_iPower are all TRUE but in fact there is no power here,
    // then we know this is a v5-encoded file and this is start-of-file
    if (WMAB_FALSE == pau->m_subfrmconfigCurr.m_rgfMaskUpdate[0])
        fStartOfStream = WMAB_FALSE;

    for (i = 0; i < pau->m_cChannel; i++)
    {
        if (0 == pau->m_rgpcinfo[i].m_iPower)
            fStartOfStream = WMAB_FALSE;
    }

    if (fStartOfStream)
    {
        int iChan;
            
        // Bitstream has forced update, claims non-zero power for all channels.
        // Verify that claim.
        for (iChan = 0; iChan < pau->m_cChannel; iChan++)
        {
            if (0 != pau->m_rgpcinfo[iChan].m_iActualPower)
                fStartOfStream = WMAB_FALSE;
        } // for (channels)
    }

    // Finally the moment of truth: set m_iDiscardSilence
    pau->m_fSeekAdjustment = WMAB_FALSE;
    if (fStartOfStream)
        pau->m_iDiscardSilence = pau->m_cFrameSampleAdjusted;
    else
    {
        pau->m_iDiscardSilence = pau->m_cSubFrameSampleHalfAdjusted;
        pau->m_fSeekAdjustment = WMAB_TRUE;
    }
}


void prvDiscardSilence(CAudioObject *pau, U16 *pcSampleDecoded,
                       U16 iChannels, U8 *pbBuf)
{
    if (*pcSampleDecoded > pau->m_iDiscardSilence)
    {
        // We decoded more than we intend to discard.
        // Discard samples and collapse remaining samples to start of buffer
        memmove(pbBuf, pbBuf + (pau->m_iDiscardSilence * iChannels),
            (*pcSampleDecoded - pau->m_iDiscardSilence) * iChannels);
        *pcSampleDecoded -= (U16) pau->m_iDiscardSilence;
        pau->m_iDiscardSilence = 0;
    }
    else
    {
        // We intend to discard the entire output
        pau->m_iDiscardSilence -= *pcSampleDecoded;
        *pcSampleDecoded = 0;
    }
}



void SetActualPower(const I16 *piCoefQ, const int iCount,
                    PerChannelInfo *ppcinfo, const Status codecStatus)
{
    int i;

    ppcinfo->m_iActualPower = 0;
    if (CODEC_BEGIN != codecStatus || 0 == ppcinfo->m_iPower || NULL == piCoefQ)
        return;

    for (i = 0; i < iCount; i++)
    {
        if (0 != piCoefQ[i])
        {
            ppcinfo->m_iActualPower = 1;
            break;
        }
    }
}



void SetActualPowerHighRate(const I32 *piCoefRecon, const int iCount,
                            PerChannelInfo *ppcinfo, const Status codecStatus)
{
    int i;

    ppcinfo->m_iActualPower = 0;
    if (CODEC_BEGIN != codecStatus || 0 == ppcinfo->m_iPower || NULL == piCoefRecon)
        return;

    for (i = 0; i < iCount; i++)
    {
        if (0 != piCoefRecon[i])
        {
            ppcinfo->m_iActualPower = 1;
            break;
        }
    }
}



#ifdef TRANSCODER

#include "coefStream.h"

WMARESULT prvInverseQuantizeTRANSCODE(CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor)
{
    WMARESULT       wmaResult;
    Float           fltQuantStep;
    I16            *piCoefQ = ppcinfo->m_rgiCoefQ;
    int             i;
    
    // *** TODO: Eliminate this buffer
    CoefType    ctCoefInvQ[2048];

    // Now just inverse quantize the coefficients and we're done
    fltQuantStep = FLOAT_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);

    // *** TODO: Just loop between m_cLowCutoff and m_cHighCutOff (m_cSubbandActual?)
    piCoefQ = ppcinfo->m_rgiCoefQ;
    for (i = 0; i < pau->m_cSubbandAdjusted; i++)
    {
        ctCoefInvQ[i] = (*piCoefQ * fltQuantStep);
        piCoefQ += 1;
    }

    if (0 == ppcinfo->m_iPower)
        memset(ctCoefInvQ, 0, pau->m_cSubbandAdjusted * sizeof(ctCoefInvQ[0]));
    
    TRACEWMA_EXIT(wmaResult, coefstrmRecordCoefs(ppcinfo->m_pcstrmCoefSaved,
        ctCoefInvQ, pau->m_cSubbandAdjusted));
    TRACEWMA_EXIT(wmaResult, coefstrmRecordDone(ppcinfo->m_pcstrmCoefSaved));

exit:
    return wmaResult;
} // prvDecodeSubFrameTRANSCODE

#endif  // TRANSCODER

