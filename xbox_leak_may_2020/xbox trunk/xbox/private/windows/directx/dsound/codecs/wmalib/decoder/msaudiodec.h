    /*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudioDec.h

Abstract:

    Header file for CAudioObjectDecoder.

Author:

    Wei-ge Chen (wchen) 14-July-1998

Revision History:


*************************************************************************/

#ifndef __MSAUDIODEC_H_
#define __MSAUDIODEC_H_


#define audecNew audecNew_XDS
#define audecDelete audecDelete_XDS
#define audecInit audecInit_XDS
#define audecReset audecReset_XDS
#define audecFlush audecFlush_XDS
#define audecOutputBufferSize audecOutputBufferSize_XDS
#define audecDecodeInfo audecDecodeInfo_XDS
#define audecDecodeData audecDecodeData_XDS
#define audecGetPCM audecGetPCM_XDS
#define prvWMAGetMoreData prvWMAGetMoreData_XDS
#define audecDecode audecDecode_XDS
#define audecSetEqualizer audecSetEqualizer_XDS
#define audecResetEqualizer audecResetEqualizer_XDS
#define audecStartComputeBandPower audecStartComputeBandPower_XDS
#define audecStopComputeBandPower audecStopComputeBandPower_XDS
#define audecGetBandPower audecGetBandPower_XDS
#define prvDecodePacket prvDecodePacket_XDS
#define prvDecodeFrame prvDecodeFrame_XDS
#define prvDecodeSubFrameTRANSCODE prvDecodeSubFrameTRANSCODE_XDS
#define prvDecodeSubFrame prvDecodeSubFrame_XDS
#define prvDecodeSubFrameHighRate prvDecodeSubFrameHighRate_XDS
#define prvDecodeFrameHeader prvDecodeFrameHeader_XDS
#define prvSetDetTable prvSetDetTable_XDS
#define prvDecodeFrameHeaderLpc prvDecodeFrameHeaderLpc_XDS
#define prvDecodeFrameHeaderNoiseSub prvDecodeFrameHeaderNoiseSub_XDS
#define prvGetNextRunDEC prvGetNextRunDEC_XDS
#define prvGetBandWeightLowRate prvGetBandWeightLowRate_XDS
#define prvUpdateSubFrameConfig prvUpdateSubFrameConfig_XDS
#define prvDecodeCoefficientMono prvDecodeCoefficientMono_XDS
#define prvDecodeCoefficientStereo prvDecodeCoefficientStereo_XDS
#define prvDecodeRunLevel prvDecodeRunLevel_XDS
#define prvAdaptEqToSubFrame prvAdaptEqToSubFrame_XDS
#define prvEqualize prvEqualize_XDS
#define prvComputeBandPower prvComputeBandPower_XDS




#include <math.h>
#include <limits.h>
#include "stdio.h"

#include "msaudio.h"
#include "strmdec_wma.h"

typedef enum {BEGIN_PACKET, BEGIN_FRAME, BEGIN_SUBFRAME, DECODE_SUBFRAME, END_SUBFRAME1, END_SUBFRAME2} DecodeStatus;
typedef enum {SUBFRM_HDR, SUBFRM_COEF, SUBFRM_DONE} SubFrmDecodeStatus;
typedef enum {HDR_SIZE, HDR_QUANT, HDR_NOISE1, HDR_NOISE2, HDR_MSKUPD, HDR_BARK, HDR_DONE} HdrDecodeStatus; //and a lot more
typedef enum {VLC, FRAME_END, NORMAL, ESCAPE} RunLevelStatus;

typedef struct CAudioObjectDecoder 
{
    CAudioObject* pau;
    //packetization stuff
    Bool  m_fPacketLoss;
    I16   m_cFrmInPacket;
    U8*   m_pbSrcCurr;  //ptr to src of curr packet, only for non-strm mode
    U16   m_cbSrcCurrLength; //only for non-strm mode
    DecodeStatus       m_decsts;
    SubFrmDecodeStatus m_subfrmdecsts;
    HdrDecodeStatus    m_hdrdecsts;
    RunLevelStatus     m_rlsts;
    I16                m_iChannel;  //current channel we are doing
    I16                m_iBand;     //current band we are doing
    Bool  m_fNoMoreData;

    Bool        m_fLastSubFrame;
    CWMAInputBitStream	m_ibstrm;

    //I don't think we need any of these now
    I16*        m_rgiRunEntry44ssQb;
    I16*        m_rgiLevelEntry44ssQb;
    I16*        m_rgiRunEntry44smQb;
    I16*        m_rgiLevelEntry44smQb;
    I16*        m_rgiRunEntry44ssOb;
    I16*        m_rgiLevelEntry44ssOb;
    I16*        m_rgiRunEntry44smOb;
    I16*        m_rgiLevelEntry44smOb;

    I16*        m_rgiRunEntry16ssOb;
    I16*        m_rgiLevelEntry16ssOb;
    I16*        m_rgiRunEntry16smOb;
    I16*        m_rgiLevelEntry16smOb;

#if 0
    UInt        m_rgiQuantStepFract[QUANTSTEP_DENOMINATOR - 1]; // Exclude numerator of 0
#endif	// !defined(SILSMOD)

#if defined(UNDER_CE) && defined(_ARM_)
    U32         lPrivate1;  // Used by ARMStub to keep track of ARM structures
    U32         lPrivate2;  // Used by ARMStub to keep track of current buffer pos
#endif

#ifdef ENABLE_EQUALIZER
#define MAX_NO_EQBAND 10
    Float       m_rgfltEqBandWeight[MAX_NO_EQBAND];
    U16         m_rgwEqBandBoundary[MAX_NO_EQBAND + 1];
    I8          m_cEqBand;
    Bool        m_fNoEq; 
    I8          m_rgbBandPower[MAX_NO_EQBAND];
    Bool        m_fComputeBandPower;
    Int         m_iEqFrameSize;
#endif // ENABLE_EQUALIZER

    WMARESULT (*m_pfnDecodeSubFrame) (struct CAudioObjectDecoder* paudec, Int* piBitCnt);
    WMARESULT (*m_pfnDecodeCoefficient) (struct CAudioObjectDecoder* paudec, PerChannelInfo* ppcinfo, Int* piBitCnt);	

} CAudioObjectDecoder;

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif
//public:
CAudioObjectDecoder* audecNew(void *pMemBuf, const I32 iMemBufSize);
Void audecDelete (CAudioObjectDecoder* paudec);

WMARESULT audecInit (
    CAudioObjectDecoder* paudec, 
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
    I32 *piMemBufUsed);
WMARESULT audecReset (CAudioObjectDecoder* paudec);
WMARESULT audecFlush (
    CAudioObjectDecoder* paudec, 
    U8   *pbDst, 
    Int   cbDstLength,
    Int*  pcbDstUsed,
    U8*   pbEqu,
    Int   cbEquLength,
    Int*  pcbEquUsed,
    U32*  pcSamplesFromBefore);

U32 audecOutputBufferSize(Int iVersion, Int iSamplingRate, Int cChannels, Int cBytePerSec, U16 wEncodeOpt);

//streaming decoding interface
WMARESULT audecDecodeInfo (CAudioObjectDecoder* paudec);
WMARESULT audecDecodeData (CAudioObjectDecoder* paudec, U16* pcSampleReady);
WMARESULT audecGetPCM (CAudioObjectDecoder* paudec, U16* pcSampleReady, U8* pbDst, U32 cbDstLength);

// This function is optionally implemented by the user, but if the user opts for
// non-streaming mode, we supply a standard implementation. Streaming-mode callers
// MUST supply their own version of this function.
WMARESULT prvWMAGetMoreData(U8 **ppBuffer, U32 *pcbBuffer,
                            U32 dwUserData, U8* pDummy);

//alternative decoding interface
WMARESULT audecDecode (CAudioObjectDecoder* paudec, 
                     const U8*   pbSrc,
                     Int         cbSrcLength,
                     Int*        pcbSrcUsed,
                     U8*         pbDst,
                     Int         cbDstLength,
                     Int*        pcbDstUsed,
                     U8*         pbEqu,
                     Int         cbEqu,
                     Int*        pcbEquUsed,
                     I32*        pcSamplesFromBefore);

#ifdef ENABLE_EQUALIZER        	
WMARESULT audecSetEqualizer (CAudioObjectDecoder* paudec, Int iBand, Int iGainIndB);
WMARESULT audecResetEqualizer (CAudioObjectDecoder* paudec);  
Void    audecStartComputeBandPower (CAudioObjectDecoder* paudec);
Void    audecStopComputeBandPower (CAudioObjectDecoder* paudec);
WMARESULT audecGetBandPower (CAudioObjectDecoder* paudec, I8* rgbBandPower);
#endif //ENABLE_EQUALIZER
#ifdef __cplusplus
}
#endif

//private:
WMARESULT prvDecodePacket (CAudioObjectDecoder* paudec,
                           const U8*    pbSrc,
                           Int          cbSrcLength,
                           Int*         pcbSrcUsed,
                           U8*          pbDst,
                           Int          cbDstLength,
                           Int*         pcbDstUsed,
                           U8*          pbEqu,
                           Int          cbEquLength,
                           Int*         pcbEquUsed,
                           I32*         pcSamplesPrevPacket);
WMARESULT prvDecodeFrame (CAudioObjectDecoder* paudec,
                        const U8*   pbSrc, 
                        Int         cbSrcLength, 
                        Int*        pcbSrcUsed, 
                        I16*        piDst, 
                        Int         cbDstLength,
                        Int*        pcbDstUsed,
                        U8*         pbEqu,
                        Int         cbEquLength,
                        Int*        pcbEquUsed);

#ifdef TRANSCODER
WMARESULT prvDecodeSubFrameTRANSCODE(CAudioObjectDecoder *paudec, Int *piBitsUsed);
#endif  // TRANSCODER

WMARESULT prvDecodeSubFrame (CAudioObjectDecoder* paudec, Int* piBitCnt);
WMARESULT prvDecodeSubFrameHighRate (CAudioObjectDecoder* paudec, Int* piBitCnt);

WMARESULT prvDecodeFrameHeader (CAudioObjectDecoder* paudec, Int* piBitCnt);
Void    prvSetDetTable  (CAudioObjectDecoder* paudec,  PerChannelInfo* ppcinfo);
WMARESULT prvDecodeFrameHeaderLpc (CAudioObjectDecoder* paudec, Int* piBitCnt);
WMARESULT prvDecodeFrameHeaderNoiseSub (CAudioObjectDecoder* paudec, Int* piBitCnt);
WMARESULT prvGetNextRunDEC (Void* pvoid, PerChannelInfo* ppcinfo, Int *piBitCnt);

// Void    prvGetBandWeightMidRate (CAudioObjectDecoder* paudec);
Void    prvGetBandWeightLowRate (CAudioObjectDecoder* paudec);
WMARESULT prvUpdateSubFrameConfig (CAudioObjectDecoder* paudec, Int iSizeNext, Int iSizePrev, Int iSizeCurr);

WMARESULT prvDecodeCoefficientMono	
                (CAudioObjectDecoder* paudec, PerChannelInfo* ppcinfo, Int* piBitCnt);
WMARESULT prvDecodeCoefficientStereo 
                (CAudioObjectDecoder* paudec, PerChannelInfo* ppcinfo, Int* piBitCnt);
WMARESULT prvDecodeRunLevel  (CAudioObjectDecoder* paudec, PerChannelInfo* ppcinfo,
						      Int iCoefFirst, 
						      Int iCoefLim, 
                              Int* piBitCnt);

//equalize
Void    prvAdaptEqToSubFrame(CAudioObjectDecoder* paudec);
WMARESULT prvEqualize (CAudioObjectDecoder* paudec, PerChannelInfo* ppcinfo);
Void    prvComputeBandPower (CAudioObjectDecoder* paudec);

INLINE Int WMA_get_fNoMoreData(U32 hWMA)
{
    return ((CAudioObjectDecoder *)hWMA)->m_fNoMoreData;
}

INLINE Void WMA_set_fNoMoreData(U32 hWMA, Int iVal)
{
    ((CAudioObjectDecoder *)hWMA)->m_fNoMoreData = iVal;
}

INLINE U8 WMA_get_nHdrBits(U32 hWMA)
{
    const CAudioObject *pau = ((CAudioObjectDecoder *)hWMA)->pau;

    if (pau->m_fAllowSuperFrame)
        return (U8)(pau->m_cBitPackedFrameSize + NBITS_FRM_CNT + NBITS_PACKET_CNT + 3);
    else
        return 0;
}

#endif  //__MSAUDIODEC_H_
