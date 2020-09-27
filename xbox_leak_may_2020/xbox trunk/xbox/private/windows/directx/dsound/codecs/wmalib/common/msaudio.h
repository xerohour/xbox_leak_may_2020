/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudio.h

Abstract:

    Implementation of public member functions for CAudioObject.

Author:

    Wei-ge Chen (wchen) 11-March-1998

Revision History:


*************************************************************************/


// Compile Time Flags normally defined in project seetings
// these vary from specific project to project
// BUILD_INTEGER - build integer version
// BUILD_INT_FLOAT - build float version of integer tree = !BUILD_INTEGER
// ENCODER - build encoder
// DECODER - build decoder = !ENCODER
// ENABLE_ALL_ENCOPT - build support for all encoder options (otherwise just HighRate)
// ENABLE_LPC - build LPC spectrogram weighting for LowRate configurations (only defined if ENABLE_ALL_ENCOPT)
// ENABLE_EQUALIZER - build support for equalizer
// Known Useful Combinations
//   ENCODER && !BUILD_INTEGER &&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC	= Normal Encoder
//   ENCODER && INTEGER_ENCODER&&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC	= Partially Integerized Encoder
//   DECODER &&  BUILD_INTEGER &&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC	= Normal Integer Do-It-All Decoder
//   DECODER &&  BUILD_INTEGER &&  ENABLE_ALL_ENCOPT && !ENABLE_LPC	= Normal Integer HighRate & MidRate Decoder
//   DECODER &&  BUILD_INTEGER && !ENABLE_ALL_ENCOPT && !ENABLE_LPC	= Normal Integer HighRate Only Decoder
//   DECODER && !BUILD_INTEGER &&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC	= IntFloat Do-It-All Decoder
//   DECODER && !BUILD_INTEGER &&  ENABLE_ALL_ENCOPT && !ENABLE_LPC	= IntFloat HighRate & MidRate Decoder
//   DECODER && !BUILD_INTEGER && !ENABLE_ALL_ENCOPT && !ENABLE_LPC	= IntFloat HighRate Only Decoder
#ifndef __MSAUDIO_H_
#define __MSAUDIO_H_

#define  MaxSamplesPerPacket MaxSamplesPerPacket_XDS
#define  msaudioGetSamplePerFrame msaudioGetSamplePerFrame_XDS
#define  auNew auNew_XDS
#define  auDelete auDelete_XDS
#define  auInit auInit_XDS
#define  auPreGetPCM auPreGetPCM_XDS
#define  auGetPCM auGetPCM_XDS
#define  auReconstruct auReconstruct_XDS
#define  auAdaptToSubFrameConfig auAdaptToSubFrameConfig_XDS
#define  prvAdaptTrigToSubframeConfig prvAdaptTrigToSubframeConfig_XDS
#define  prvAdaptTrigToSubframeConfig_INT prvAdaptTrigToSubframeConfig_INT_XDS
#define  auPowerToRMS auPowerToRMS_XDS
#define  auUpdateMaxEsc auUpdateMaxEsc_XDS
#define  auDctIV auDctIV_XDS
#define  auResampleWeightFactorLPC auResampleWeightFactorLPC_XDS
#define  auLsp2lpc auLsp2lpc_XDS
#define  auResampleWeightFactorBark auResampleWeightFactorBark_XDS
#define  prvAllocate prvAllocate_XDS
#define  prvSetBarkIndex prvSetBarkIndex_XDS
#define  prvInverseQuantizeHighRate prvInverseQuantizeHighRate_XDS
#define  prvInverseTransformMonoauReconMono prvInverseTransformMonoauReconMono_XDS
#define  auDctIV_KNIprvFFT4DCT_INTEL auDctIV_KNIprvFFT4DCT_INTEL_XDS
#define  auInitIntelFFT auInitIntelFFT_XDS
#define  auFreeIntelFFT auFreeIntelFFT_XDS
#define  prvInvFftComplexConjugate_INTEL prvInvFftComplexConjugate_INTEL_XDS
#define  auReconstruct_X86 auReconstruct_X86_XDS
#define  auReconMono_X86 auReconMono_X86_XDS
#define  auReconStereo_MMX auReconStereo_MMX_XDS
#define  auReconStereo_SIMDFP auReconStereo_SIMDFP_XDS
#define  auReconstruct_MMX auReconstruct_MMX_XDS
#define  auReconMono_MMX auReconMono_MMX_XDS
#define  prvInverseTransformMono_MMX prvInverseTransformMono_MMX_XDS
#define  prvFFT_Int prvFFT_Int_XDS
#define  prvFFT4DCT prvFFT4DCT_XDS
#define  prvInvFftComplexConjugate prvInvFftComplexConjugate_XDS
#define  prvInitNoiseSub prvInitNoiseSub_XDS
#define  prvInverseQuantizeMidRate prvInverseQuantizeMidRate_XDS
#define  prvInverseQuantizeLowRate prvInverseQuantizeLowRate_XDS
#define  prvInverseQuantizeMidRateSkipNoise prvInverseQuantizeMidRateSkipNoise_XDS
#define  prvInverseQuantizeTRANSCODE prvInverseQuantizeTRANSCODE_XDS
#define  prvLpcToSpectrumprvInitInverseQuadRootTable prvLpcToSpectrumprvInitInverseQuadRootTable_XDS
#define  prvInterpolateInit prvInterpolateInit_XDS
#define  auLowPass auLowPass_XDS
#define  prvInterpolate prvInterpolate_XDS
#define  qstCalcQuantStep qstCalcQuantStep_XDS
#define  prvWeightedQuantization prvWeightedQuantization_XDS
#define  SetActualPower SetActualPower_XDS
#define  SetActualPowerHighRate SetActualPowerHighRate_XDS
#define  prvWmaShowFrames prvWmaShowFrames_XDS
#define  HeapDebugCheck HeapDebugCheck_XDS
#define  mallocAligned mallocAligned_XDS
#define  freeAligned freeAligned_XDS
#define  auMallocGetCount auMallocGetCount_XDS
#define  auMallocSetState auMallocSetState_XDS
#define  auMalloc auMalloc_XDS
#define  auFree auFree_XDS
#define  auReconMono auReconMono_XDS
#define  auReconCoefficentsHighRate auReconCoefficentsHighRate_XDS
#define  prvInitDiscardSilence prvInitDiscardSilence_XDS
#define  prvDiscardSilence prvDiscardSilence_XDS
#define  g_SupportMMX g_SupportMMX_XDS
#define  g_SupportKNI g_SupportKNI_XDS
#define  g_SupportSIMD_FP g_SupportSIMD_FP_XDS
#define  g_SupportCMOV g_SupportCMOV_XDS
#define  auSaveHistoryMono auSaveHistoryMono_XDS
#define  prvGetBandWeightMidRate prvGetBandWeightMidRate_XDS
#define  cfltInverseRand2Float cfltInverseRand2Float_XDS
#define  prvPq2lpc prvPq2lpc_XDS
#define  prvConvolve_odd prvConvolve_odd_XDS
#define  prvConvolve prvConvolve_XDS
#define  prvLsp2pq prvLsp2pq_XDS
#define  prvInitInverseQuadRootTable prvInitInverseQuadRootTable_XDS
#define  prvDoLpc4 prvDoLpc4_XDS
#define  prvLpcJLoopBottom prvLpcJLoopBottom_XDS
#define  prvLpcToSpectrum prvLpcToSpectrum_XDS
#define  g_rgiLsfReconLevel g_rgiLsfReconLevel_XDS
#define  lpc_trig2048 lpc_trig2048_XDS
#define  CLSID_CAsfHeaderObjectV0 CLSID_CAsfHeaderObjectV0_XDS
#define  CLSID_AsfXAcmAudioErrorMaskingStrategy CLSID_AsfXAcmAudioErrorMaskingStrategy_XDS
#define  CLSID_AsfXSignatureAudioErrorMaskingStrategy CLSID_AsfXSignatureAudioErrorMaskingStrategy_XDS
#define  CLSID_AsfXStreamTypeAcmAudio CLSID_AsfXStreamTypeAcmAudio_XDS
#define  CLSID_CAsfMarkerObjectV0 CLSID_CAsfMarkerObjectV0_XDS
#define  CLSID_CAsfLicenseStoreObject CLSID_CAsfLicenseStoreObject_XDS
#define  CLSID_CAsfExtendedContentDescObject CLSID_CAsfExtendedContentDescObject_XDS
#define  CLSID_CAsfContentDescriptionObjectV0 CLSID_CAsfContentDescriptionObjectV0_XDS
#define  CLSID_CAsfContentEncryptionObject CLSID_CAsfContentEncryptionObject_XDS
#define  CLSID_CAsfStreamPropertiesObjectV2 CLSID_CAsfStreamPropertiesObjectV2_XDS
#define  CLSID_CAsfStreamPropertiesObjectV1 CLSID_CAsfStreamPropertiesObjectV1_XDS
#define  CLSID_CAsfPropertiesObjectV2 CLSID_CAsfPropertiesObjectV2_XDS
#define  CLSID_CAsfDataObjectV0 CLSID_CAsfDataObjectV0_XDS
#define  CLSID_AsfXNullErrorMaskingStrategy CLSID_AsfXNullErrorMaskingStrategy_XDS


#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#include <stdlib.h>

#include "macros.h"
#include "DecTables.h"


#define MAX_SUBFRAMES       16

// Forward data type decl
typedef struct _CoefStream CoefStream;

#ifdef TRANSCODER
#define OLD_QUANT_LOOP
#define TRANSCODER_ONLY(x)  (x)
#define TRANSCODER_AB(x,y)  (x)
#else   // TRANSCODER
#define TRANSCODER_ONLY(x)
#define TRANSCODER_AB(x,y)  (y)
#endif  // TRANSCODER

typedef enum {TT_NONE = 0, TT_SIMPLE} TRANSCODETYPE;
typedef enum {STEREO_LEFTRIGHT = 0, STEREO_SUMDIFF} StereoMode;
typedef enum {CODEC_NULL, CODEC_STEADY, CODEC_DONE, CODEC_BEGIN, CODEC_LAST, CODEC_ONHOLD} Status;
typedef enum {FMU_DONE = 0, FMU_QUANTIZE, FMU_PROCESS} FMU_STATUS;
typedef struct PerChannelInfo 
{
//whenever the memory gest moved to buckets, update is needed
    // Decoder and Global Members
    I16*           m_rgiCoefQ;						//quantized coef
    Int*           m_rgiMaskQ;                      //quantized maskq ((could be I16))
    U8*            m_rgbBandNotCoded;               //band not coded
	FastFloat*	   m_rgffltSqrtBWRatio;				//MidRate/LowRate for Bands not coded
	Int*		   m_rgiNoisePower;
    const U16*     m_rgiHuffDecTbl;                 //decoder only
    const U16*     m_rgiRunEntry;                   //decoder only        
    const U16*     m_rgiLevelEntry;                 //decoder only
    Int            m_cSubbandActual;
    Int            m_iPower;                        //channel power
    Int            m_iActualPower;                  // Power as suggested by CoefQ/CoefRecon
    UInt*          m_rguiWeightFactor;				//weighting factor
    Int*           m_rgiCoefRecon;                  //reconstructed coef
	Int			   m_iMaxMaskQ;	

#ifdef TRANSCODER
    // Transcoder members
    CoefStream    *m_pcstrmCoefSaved;
    CoefStream    *m_pcstrmPower;
    CoefStream    *m_pcstrmMaskQ;
    Int            m_rgiLastMaskQUpdate[NUM_BARK_BAND];
    Int            m_iLastMaskQUpdBarkBands;
    I32            m_rgiThinningRands[2048];
    I32            m_rgiRands[2048];
#endif  // TRANSCODER

#define CURRGETPCM_INVALID  0x7FFF
    I16            m_iCurrGetPCM_SubFrame;          // reconstruction index
    Int*           m_rgiCoefReconCurr;  //current position in m_rgiCoefRecon
    I16*           m_piPrevOutput;      //prev history per channel
    I16*           m_piPrevOutputSign;  //sign of m_piPrevOutput
    I16*           m_piPrevOutputCurr;  //current position in m_piPrevOutput

	// These next five must be kept in this order for SH4 optimizations
    Int            m_iSin;    //floating value = (float)iSin/0x3FFFFFFF
    Int            m_iCos;    //floating value = (float)iCos/0x3FFFFFFF
	Int            m_iSin1;	 //previous sin value for recurrsion
	Int            m_iCos1;	 //previous cos value for recurrsion
	Int            m_iStep;	 // 2*sin(step increment)

    // Float-only members
#ifndef BUILD_INTEGER
#endif
    // The following is NOT YET INTEGERIZED but after it is,
    // it should be moved into the #ifndef BUILD_INTEGER above
    Float*         m_rgfltCoefRecon;                //reconstructed coef soon to be removed
    Float*         m_rgfltBandWeight;    

#ifdef BUILD_INTEGER
#else
#endif
    // The following are NOT YET INTEGERIZED but after they are,
    // they should be moved into the #ifndef BUILD_INTEGER above
    Float*         m_rgfltWeightFactor;
    WeightType     m_wtMaxWeight;
  

#ifdef ENCODER
#if !defined (DISABLE_PREPROCESSING)
    // For pre-processing
    I16*             m_rgnPPBuffer;            // Temporary buffer   
#endif 
    
    Float*           m_rgfltMask;
#if defined (PEAQ_MASK)
    // The excitation values for mask computation etc.
    Float*           m_rgfltBarkNmrWts;        // somewhat based on the excitation pattern
    Float*           m_rgfltBarkExc;           // enough for all subframes
    Bool*            m_rgfNoisyBark;           // barks with any kind of noise substition
    Float*           m_rgfltBarkNmr;           // NMR measured, per bark basis
    Float*           m_rgfltBarkExcPrev;       // just pointer for v8
    Float*           m_rgfltBarkExcCurr;       // just pointer for v8
    Float*           m_rgfltBarkExcRef;        // just pointer for v8
    
    Float*           m_rgfltBarkExcRecon;      // enough for all subframes
    Float*           m_rgfltBarkExcReconPrev;  // just pointer for v8
    Float*           m_rgfltBarkExcReconCurr;  // just pointer for v8
    
    Float*           m_rgfltTargetSpectrum;    // for generating excitation=>mask=>lpc parameters.

#endif // PEAQ_MASK
	
#if defined (SMOOTH_NMR)     
    // Noise detection, suppression etc
    Float*           m_rgfltBandEnergy;        // to determine stationarity
    Float*           m_rgfltNoiseSpectrum;     // to perform noise filtering
    Float*           m_rgfltSpectrumCurr;      // to perform noise filtering
    
    Bool*            m_rgfMaskNeededForBark;   // for header squeezing
    Float*           m_rgfltBarkEnergyOrig;    // in computing nrmr
#endif // SMOOTH_NMR

    // Encoder-only members
    FLOATORI32*     m_pfltInputW;                    //weighted data: encoder only

    Float*          m_rgfltCoefOrig;                 // original coef: encoder only
    Float*          m_rgfltCoef;                     //original coef: encoder only, weighted
    
#if defined (PEAQ_MASK)
    Float*          m_rgfltCoefLR;                   // original coef, straight through (without stereo matrixing)
    Float*          m_rgfltCoefReconLR;              // reconstructed coef, straight through (without stereo matrixing)
#endif // PEAQ_MASK
    
    I32*            m_rgiCoef;                       // Integer version of m_rgfltCoef, not really used by anybody now
    U32             m_iCoefMagnitude;                // Magnitude (OR) of all coefs in m_rgiCoef; integer encoder
    Float*          m_rgfltWeightFactorPerBark;
    U8*             m_rgbLsfQ;                       //quantized lsf; encoder only
    I16*            m_rgiCoefQRLC;                   //Run length coded quantized coef
    I16*            m_rgiCoefQRLC_DIFF;              //Previous QRLC to be used for requant purposes
    U8*             m_rgbIsTone;                     //encoder only
    const U32*      m_rgiHffmanTblRunLevel;          //could be different for each stereo channel
    const I16*      m_rgiRunLevelVLCAtLevel;
    Int             m_iMaxLevelInRunLevelVLC;
    Int             m_cBitsUsed;                     //encoder only   
    Bool            m_fAnyNonZeroCoefQ;              //is coefQ all zero?
    Int             m_iFramePowerPrev;
    Int             m_iFramePower;
    WMARESULT       m_hr;                            // return code for multi-threaded operations
    FMU_STATUS      m_fNeedFullMaskUpdate;  // Scrunch bug #32: v4 decoder can resample
                                            // uninitialized weight factors
    float           m_fltLastFreq[LPCORDER];

    // transient detection (encoder only)
    I16           *m_piInputCurr;   //other preprocessing as well
    I16           *m_piInputNext;   //other preprocessing as well

    I16           *m_rgiTransient;
    FLOATORU32    *m_rgfltTransientStrength;
    FLOATORU32    *m_rgfltSlicePower;
    FLOATORU32    m_fltPowerPrevL;
    FLOATORU32    m_fltPowerPrevM;
    FLOATORU32    m_fltPowerPrevH;

    I16           *m_rgiTransientSrcLow;
    Int           m_iTransientSrcLowCurr;
    FLOATORI32    *m_rgfltTransientDstLow;
    Int           m_iTransientDstLowCurr;

    I16           *m_rgiTransientSrcMed;
    Int           m_iTransientSrcMedCurr;
    FLOATORI32    *m_rgfltTransientDstMed;
    Int           m_iTransientDstMedCurr;

    I16           *m_rgiTransientSrcHi;
    Int           m_iTransientSrcHiCurr;
    FLOATORI32    *m_rgfltTransientDstHi;
    Int           m_iTransientDstHiCurr;

#if defined (SMOOTH_NMR) && !defined(INTEGER_ENCODER) && !defined(BUILD_INTEGER)
    Float        *m_rgfltDitherNoise;
    Float         m_fltAchievedNmr;
#endif  // defined (SMOOTH_NMR) && !defined(INTEGER_ENCODER) && !defined(BUILD_INTEGER)
    
#endif // ENCODER

} PerChannelInfo;

typedef struct SubFrameConfigInfo
{ 
    Int m_cSubFrame;
    Int m_rgiSubFrameSize [MAX_SUBFRAMES];
    Int m_rgiSubFrameStart [MAX_SUBFRAMES];
    Int m_iFirstTransientSubFrm;        //for enc only
    Int m_iLastTransientSubFrm;         //for enc only
    Int m_cMaskUpdate;                  //for enc only
    Int m_rgfMaskUpdate [MAX_SUBFRAMES];
} SubFrameConfigInfo;


typedef enum {LPC_MODE=0,BARK_MODE} WeightMode;

typedef struct CAudioObject
{
    //Step Size
    QuantStepType  m_qstQuantStep;
#if 0
	// no longer needed
    UInt    m_rgiQuantStepFract[QUANTSTEP_DENOMINATOR - 1]; // Exclude numerator of 0
#endif

    // packetization (has nothing to do with the rest)
    Int m_iPacketCurr;
    Int m_cBitPackedFrameSize;
    Int m_cBitPacketHeader;
    Int m_cdwPacketHeader;
    Int m_cBitPacketHeaderFractionDw;
    Int m_cBitPacketLength;

    //utility vars for entropy decoding for high rate only
    Int m_cRunOfZeros;
    I16 m_iLevel; 
    Int m_iSign;
    Int m_iHighCutOffCurr;
    Int m_iNextBarkIndex;

    Bool  m_fNoiseSub; 
    Float m_fltBitsPerSample;
    Float m_fltWeightedBitsPerSample;

    Int m_iMaxEscSize;          // in bits
    Int m_iMaxEscLevel;         // (1<<m_iMaxEscSize)

    
    Int     m_iVersion;
    Status	m_codecStatus;          //current status of the codec
    Bool    m_fSeekAdjustment;
    Bool    m_fPacketLossAdj;       //skip some samples when packetloss
    Int		m_iSamplingRate;        //audio sampling rate in Hz
    U16		m_cChannel;
    U32     m_nBytePerSample;
#ifdef WMA_V9
    U16     m_nValidBitsPerSample;
    U32     m_nChannelMask;
#endif 
    Int		m_cSubband;				//no. of subbands in the MLT transform

    Bool    m_fAllowSuperFrame;
    Bool    m_fAllowSubFrame;
    Bool    m_fV5Lpc;
//!! reset
    Int     m_iCurrSubFrame;        //which subframe we are doing
    I16     m_iCurrReconCoef;       //which coef we are doing
    Bool    m_fHeaderReset;
//!! dupe? needed for inv-quantization
    Int     m_iSubFrameSizeWithUpdate;
    Int     m_iMaxSubFrameDiv;      //maximum no. division into subwindows
    Int     m_cMinSubFrameSample;   //min size of subframes in current frame; many change from frame to frame
    Int     m_cMinSubFrameSampleHalf;   //min size of subframes in current frame; many change from frame to frame
    Int     m_cMinSubFrameSampleQuad;   //min size of subframes in current frame; many change from frame to frame
    Int     m_cPossibleWinSize; 
    Int     m_iIncr;                //log2(longest winsize / curr winsize)
    Int     m_cSubFrameSample;      //for long win, same as m_cFrameSample
    Int     m_cSubFrameSampleHalf;
    Int     m_cSubFrameSampleQuad;
//!! yes!
    SubFrameConfigInfo  m_subfrmconfigPrev;
    SubFrameConfigInfo  m_subfrmconfigCurr;
    SubFrameConfigInfo  m_subfrmconfigNext;

    //should be reset every subfrm in next version
    Int		m_cBitsSubbandMax;	    //no. of bits for m_cSubband; 
    Int		m_cFrameSample;         //no. of input/output samples = 2 * cSubband
    Int		m_cFrameSampleHalf;     //no. of input/output samples = cSubband
    Int		m_cFrameSampleQuad;     //no. of input/output samples = cSubband / 2

    Int		m_cLowCutOff;           //How many coeffs to zero at the low end of spectrum
    Int		m_cHighCutOff;			//How many coeffs to zero at the high end of spectrum
    Int		m_cLowCutOffLong;
    Int		m_cHighCutOffLong;		

    Int		m_iWeightingMode;
//!! yes!
    StereoMode  m_stereoMode;       // left-right or sum-difference coding 
    Int     m_iEntropyMode;         //entropy coding mode
    Float	m_fltDitherLevel;	    //=.12; Relative intensity of Roberts' pseudo-random noise quantization
    Int		m_iQuantStepSize;
    Float	m_fltFlatenFactor;	    //Excess spectral noise power factor. Typical values are
    							    //in the range 0.4 -- 0.9; best choice is 0.5
    Float   m_fltDctScale;
    Int		m_cValidBarkBand;       //valid bark band in range; some are out of range in the subband decompositio
    const Int*	m_rgiBarkIndex;     //corresponding coef index for each valid bark


#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
	I32     m_iSinRampUpStart;
	I32     m_iCosRampUpStart;
	I32     m_iSinRampUpPrior;
	I32     m_iCosRampUpPrior;
	I32     m_iSinRampUpStep;
	I32     m_iSinRampDownStart;
	I32     m_iCosRampDownStart;
	I32     m_iSinRampDownPrior;
	I32     m_iCosRampDownPrior;
	I32     m_iSinRampDownStep;
	I32     m_iSinInit[4];			// still need these for encoder (MsAudioEnc) until its recurrsion is fixed
	I32     m_iCosInit[4];			// still need these for encoder (MsAudioEnc) until its recurrsion is fixed
#endif

#if !defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
	V4V5COMPARE    m_fltSinRampUpStart;
	V4V5COMPARE    m_fltCosRampUpStart;
	V4V5COMPARE    m_fltSinRampUpPrior;
	V4V5COMPARE    m_fltCosRampUpPrior;
	V4V5COMPARE    m_fltSinRampUpStep;
	// These next five must be kept in this order for SH4 optimizations
	V4V5COMPARE    m_fltSinRampDownStart;
	V4V5COMPARE    m_fltCosRampDownStart;
	V4V5COMPARE    m_fltSinRampDownPrior;
	V4V5COMPARE    m_fltCosRampDownPrior;
	V4V5COMPARE    m_fltSinRampDownStep;
#endif

    Int     m_iSizePrev;
    Int     m_iSizeCurr;
    Int     m_iSizeNext;

    Int     m_iCoefRecurQ1;
    Int     m_iCoefRecurQ2;
    Int     m_iCoefRecurQ3;
    Int     m_iCoefRecurQ4;

    I16*	m_rgiCoefQ;             // just a pointer

    //random accessed memory
    //overall
    own PerChannelInfo* m_rgpcinfo;
    //transform
	own Int*	m_rgiCoefReconOrig;
    //weighting
//!!yes? invQ needs it
    own Int*	m_rgiMaskQ;
    own Int*	m_rgcValidBarkBand;
    own Int*	m_rgiBarkIndexOrig;  

    //output
//!!yes? invQ needs it
    own I16*	m_piPrevOutput;        // previous encoded output; bottom half += top half of Curr; then gone
//!!yes? invQ needs it
    I32         m_iDiscardSilence;

#ifdef TRANSCODER
    // Transcoder member variables
    TRANSCODETYPE   m_ttTranscodeType;
    CoefStream     *m_pcstrmSubFrameSizes;
    CoefStream     *m_pcstrmStereoMode;
    CoefStream     *m_pcstrmMaskUpdate;
    CoefStream     *m_pcstrmBitAllocs;
    CoefStream     *m_pcstrmQSteps;
    CoefStream     *m_pcstrmSubFrmConfig;
    CoefStream     *m_pcstrmFrameBitAlloc;
    Int             m_rgfOrigMaskUpdate[MAX_SUBFRAMES];
#endif  // TRANSCODER

#ifdef ENABLE_ALL_ENCOPT
    Float   m_fltFirstNoiseFreq;
    Int		m_iFirstNoiseBand;
    Int     m_iFirstNoiseIndex;
    Int     m_iNoisePeakIgnoreBand; // Num of coefficients to ignore when detecting peaks during noise substitution

    //only used for NoiseSub mode
    own Int*        m_rgiFirstNoiseBand;
//!!yes
    own U8*         m_rgbBandNotCoded;
	own FastFloat*	m_rgffltSqrtBWRatio;
//!!yes
    own Int*        m_rgiNoisePower;
//!!for invQ
    own Float*      m_rgfltBandWeight;

#if defined(_DEBUG) && defined(LPC_COMPARE)
    Float* m_rgfltLpsSpecCos [LPCORDER];
    Float* m_rgfltLpsSpecSin [LPCORDER];
#endif  // _DEBUG
#else   // ENABLE_ALL_ENCOPT
#endif  // ENABLE_ALL_ENCOPT

    Float*  m_rgfltWeightFactor; 
    UInt*	m_rguiWeightFactor;   //share with m_rgiMaskQ when bark

    U32 m_iFrameNumber; // frame counter to the common code

#if defined(INTERPOLATED_DOWNSAMPLE)
    Bool    m_fLowPass;             // Specify independently of downsampling
    Bool    m_fDownsample;          // Eg, Downsample 32kHz output to 22kHz for some devices
    Int     m_iInterpolSrcBlkSize;  // Size of 1 indivisible source block
    Int     m_iInterpolDstBlkSize;  // Size of 1 indivisible destination block
    Int     m_iInterpolIncr;        // Pointer increment
    Int     m_iInterpolCarry;       // Pointer increment "carry", +1 from time to time
    I32     m_iInterpolWeightIncr;  // FixedPt increment to interpolation weight
    Int     m_iInterpolAddInterval; // Add an extra sample at this interval of output samples
    Int     m_iInterpolNextAdd;     // Countdown to extra sample
#endif  // defined(INTERPOLATED_DOWNSAMPLE)

#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
    Bool    m_fHalfTransform;
    Bool    m_fUpsample;
    Bool    m_fPad2XTransform;
	I16		m_iPriorSample[2];		// warning: maximum number of channels is currently 2

    // the following values are adjusted to reflect HalfTransform or PAd2XTransform
    // They are used for the DCT and for Reconstruction.
    Int		m_cSubbandAdjusted;
    Int     m_cFrameSampleAdjusted; 
    Int     m_cFrameSampleHalfAdjusted;
    Int     m_cSubFrameSampleAdjusted;
    Int     m_cSubFrameSampleHalfAdjusted;
    Int     m_cSubFrameSampleQuadAdjusted;
    Int		m_cHighCutOffAdjusted;
    // m_iCoefRecurQ1..4 and Trig values only exist in their adjusted states
#else   // defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
    // Without these features, no need for adjusted values, 
    // so make the code use the regular values
#   define  m_cSubbandAdjusted            m_cSubband
#   define  m_cFrameSampleAdjusted        m_cFrameSample
#   define  m_cFrameSampleHalfAdjusted    m_cFrameSampleHalf
#   define  m_cSubFrameSampleAdjusted     m_cSubFrameSample
#   define  m_cSubFrameSampleHalfAdjusted m_cSubFrameSampleHalf
#   define  m_cSubFrameSampleQuadAdjusted m_cSubFrameSampleQuad
#   define	m_cHighCutOffAdjusted         m_cHighCutOff
#endif  // defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)


    // Function Pointers
    WMARESULT (*aupfnInverseQuantize) (struct CAudioObject* pau,
        PerChannelInfo* ppcinfo, Int* rgiWeightFactor);
    WMARESULT (*prvpfnInverseTransformMono) (struct CAudioObject* pau,
        PerChannelInfo* ppcinfo, I16* piOutput, I16* piPrevOutput,
        I16* piPrevOutputSign, Bool fForceTransformAll);
    WMARESULT (*aupfnGetNextRun) (Void* paudec, PerChannelInfo* ppcinfo, Int *piBitCnt);

    WMARESULT (*aupfnReconstruct) (struct CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll);

    WMARESULT (*aupfnDctIV) (struct CAudioObject* pau, CoefType* rgiCoef, Float fltAfterScaleFactor, U32 *piMagnitude);
    void (*aupfnFFT) (CoefType data[], Int nLog2np);

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)
    Bool    m_fIntelFFT;    // Intel FFT needs setup/takedown, so we need to know this
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE) && defined(WMA_TARGET_X86)

	tRandState m_tRandState;

#ifdef USE_SIN_COS_TABLES
    //Tables for reconstruction sin values
    BP2Type *m_piSinForRecon2048;
    BP2Type *m_piSinForRecon1024;
    BP2Type *m_piSinForRecon512;
    BP2Type *m_piSinForRecon256;
    BP2Type *m_piSinForRecon128;
    BP2Type *m_piSinForRecon64;
    BP2Type *m_piSinForRecon;

    BP2Type *m_piSinForSaveHistory;
#endif

} CAudioObject;


//public and protected
#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif


#if defined(HALF_TRANSFORM)
#   define HALFTRANSFORM_ONLY(x)   (x)
#   define UPSAMPLE_ONLY(x)        (x)
#   define HALF(fHalfTransform, x)  ((fHalfTransform) ? ((x)/2) : (x))
#else   // defined(HALF_TRANSFORM)
#   define HALFTRANSFORM_ONLY(x)
#   define UPSAMPLE_ONLY(x)
#   define HALF(fUpsample, x)  (x)
#endif  // defined(HALF_TRANSFORM)

#if defined(PAD2X_TRANSFORM)
#   define PAD2XTRANSFORM_ONLY(x)	(x)
#   define DOUBLE(fPad2X, x)  ((fPad2X) ? ((x)*2) : (x))
#else   // defined(PAD2X_TRANSFORM)
#   define PAD2XTRANSFORM_ONLY(x)
#   define DOUBLE(fUpsample, x)    (x)
#endif  // defined(PAD2X_TRANSFORM)

//#if defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
//#   define HALF_OR_DOUBLE(fHalfTransform, fPad2X, x)  ((fHalfTransform) ? ((x)/2) : ((fPad2X) ? ((x)*2) : (x)))
//#else   // defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)
//#   define HALF_OR_DOUBLE(fUpsample, fPad2X, x)      (x)
//#endif  // defined(HALF_TRANSFORM) || defined(PAD2X_TRANSFORM)


U32 MaxSamplesPerPacket(U32 ulVersion, U32 ulSamplingRate, U32 ulChannels, U32 ulBitrate);
I32 msaudioGetSamplePerFrame (Int   cSamplePerSec, 
                              U32   dwBitPerSec, 
                              Int   cChannel,
                              Int   iVersion);

CAudioObject* auNew ();
Void    auDelete (CAudioObject* pau);        //destructor
WMARESULT auInit (CAudioObject* pau, Int iVersionNumber, Int cSubband, 
                  Int iSamplingRate, U16 cChannel, 
#ifdef WMA_V9
                  U32 nBlockAlign,
                  U16   nValidBitsPerSample,
                  U32   nChannelMask,
#endif // WMA_V9
                  Int cBytePerSec, Int cbPacketLength,
                  U16 iEncodeOpt, U16 iPlayerOpt);

Void    auPreGetPCM (CAudioObject* pau, U16* pcSampleDecoded);
WMARESULT auGetPCM (CAudioObject* pau, U16* pcSample, U8* pbDst, U32 cbDstLength);
WMARESULT auReconstruct (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll);
WMARESULT auSaveHistoryMono (CAudioObject* pau, PerChannelInfo* ppcinfo, 
                            Bool fForceTransformAll);

WMARESULT auAdaptToSubFrameConfig (CAudioObject* pau);
WMARESULT prvAdaptTrigToSubframeConfig(CAudioObject *pau);
WMARESULT prvAdaptTrigToSubframeConfig_INT(CAudioObject *pau);


Int     auPowerToRMS(CAudioObject* pau, Int iCurrMaskQ, Int iPrevMaskQ, Int iPrevResult);
Void    auUpdateMaxEsc(CAudioObject* pau, Int iQuantStepSize);

WMARESULT auDctIV(CAudioObject* pau, CoefType* rgiCoef, 
				   Float fltAfterScaleFactor, U32 *piMagnitude);  // DCT type 4

// LPC functions
#ifdef ENABLE_ALL_ENCOPT
Void    auResampleWeightFactorLPC (CAudioObject* pau, PerChannelInfo* ppcinfo);
Void    auLsp2lpc(CAudioObject* pau, U8 *lsfQ, LpType *lpc, Int order);
#endif //ENABLE_ALL_ENCOPT

Void    auResampleWeightFactorBark (CAudioObject* pau, PerChannelInfo* ppcinfo);

WMARESULT prvAllocate (CAudioObject* pau);

#ifdef __cplusplus
}
#endif

//private:

// *** TEMPORARY: prvAllocate moved from here to extern "C" block above ***

Void    prvSetBarkIndex (CAudioObject* pau);
WMARESULT prvInverseQuantizeHighRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor);
WMARESULT auReconCoefficentsHighRate (CAudioObject* pau, Void* pcaller, PerChannelInfo* ppcinfo, Int* piBitCnt);

WMARESULT prvInverseTransformMono (CAudioObject* pau, PerChannelInfo* ppcinfo, I16* piOutput,
                                 I16* piPrevOutput, I16* piPrevOutputSign,
                                 Bool fForceTransformAll);

WMARESULT auReconMono ( CAudioObject* pau,PerChannelInfo* ppcinfo,I16* piOutput,I16* pcSampleGet,Bool fForceTransformAll);


#if defined(WMA_TARGET_MIPS) && defined(BUILD_INTEGER)
//This function initalizes the function pointers for mips transforms
WMARESULT auInitMIPS(CAudioObject* pau);
#endif


#if defined(WMA_TARGET_X86)
WMARESULT auDctIV_KNI(CAudioObject* pau, CoefType* rgiCoef, Float fltAfterScaleFactor, U32 *piMagnitude);
WMARESULT auDctIV_INTEL(CAudioObject* pau, CoefType* rgiCoef, Float fltAfterScaleFactor, U32 *piMagnitude);

#if !defined(BUILD_INTEGER) && !defined(UNDER_CE)
typedef enum {INTELFFT_INIT_PROCESS, INTELFFT_INIT_THREAD,
    INTELFFT_FREE_THREAD, INTELFFT_FREE_PROCESS} INTELFFT_INITCODE;

void prvFFT4DCT_INTEL(CoefType data[], Int nLog2np);
WMARESULT auInitIntelFFT(const CAudioObject *pau, const INTELFFT_INITCODE iCode);
WMARESULT auFreeIntelFFT(CAudioObject *pau, const INTELFFT_INITCODE iCode);

#if defined (PEAQ_MASK) && defined (ENCODER)
Void prvInvFftComplexConjugate_INTEL(Float rgfltData[], Int nLog2np);
#endif // PEAQ_MASK && ENCODER
#endif  // !defined(BUILD_INTEGER) && !defined(UNDER_CE)


#ifdef USE_SIN_COS_TABLES
#ifndef BUILD_INTEGER
WMARESULT auReconstruct_X86 (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll);
WMARESULT auReconMono_X86 ( CAudioObject* pau,PerChannelInfo* ppcinfo,I16* piOutput,I16* pcSampleGet,Bool fForceTransformAll);
WMARESULT auReconStereo_MMX (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll);
WMARESULT auReconStereo_SIMDFP (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll);
#else
WMARESULT auReconstruct_MMX (CAudioObject* pau, I16* piOutput, I16* pcSampleGet, Bool fForceTransformAll);
WMARESULT auReconMono_MMX ( CAudioObject* pau,PerChannelInfo* ppcinfo,I16* piOutput,I16* pcSampleGet,Bool fForceTransformAll);
#endif //#ifndef BUILD_INTEGER
#endif //#ifdef USE_SIN_COS_TABLES
WMARESULT prvInverseTransformMono_MMX (CAudioObject* pau, Float* rgfltCoefRecon,
                                     I32* piCurrOutput, Bool fForceTransformAll);
#endif // defined(WMA_TARGET_X86)

#if !(defined(WMA_TARGET_MIPS)&&defined(BUILD_INTEGER))
Void    prvFFT_Int(Int data[], Int np);
Void    prvFFT4DCT(CoefType data[], Int nLog2np);

#if defined (PEAQ_MASK) && defined (ENCODER)
// Implemented in x86.c, fft.c
Void prvInvFftComplexConjugate(Float *rgfltData, Int nOrder);
#endif // PEAQ_MASK && ENCODER
#endif // !WMA_TARGET_MIPS

#ifdef ENABLE_ALL_ENCOPT
//for noise sub and low rates only
WMARESULT prvInitNoiseSub (CAudioObject* pau);
WMARESULT prvInverseQuantizeMidRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor);
WMARESULT prvInverseQuantizeLowRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor);

#if defined(ENCODER) && defined (SMOOTH_NMR) && !defined(INTEGER_ENCODER) && !defined(BUILD_INTEGER)
WMARESULT prvInverseQuantizeMidRateSkipNoise (CAudioObject* pau, PerChannelInfo* ppcinfo,
                                              Int* rgiWeightFactor, Bool fSkipNoise);
#endif // defined(ENCODER) && defined (SMOOTH_NMR) && !defined(INTEGER_ENCODER) && !defined(BUILD_INTEGER)

#ifdef TRANSCODER
WMARESULT prvInverseQuantizeTRANSCODE (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor);
#endif  // TRANSCODER

//private lpc functions
WMARESULT prvLpcToSpectrum (CAudioObject* pau, const LpType* rgfltLpcCoef, PerChannelInfo* ppcinfo);
Void prvInitInverseQuadRootTable (CAudioObject* pau);
#endif //ENABLE_ALL_ENCOPT

#ifdef __cplusplus
}
#endif

#ifndef PREVOUTPUT_16BITS
// **************************************************************************
// Signbit Macros
// I've put them here because they're used in both msaudio.c and float.c
// **************************************************************************
#define I17_MIN ((signed) 0xFFFF0000)
#define I17_MAX ((signed) 0x0000FFFF)
#define CALC_SIGN_OFFSETS(piSrcBase, piSrc, piSrcSignBase, piSrcSign,           \
                           uiSignbitData, uiSignbitCount, cChan)                \
    {                                                                           \
    const int c_iOffset = (piSrc - piSrcBase)>>(cChan-1);                       \
    const int c_iSignOffset = (c_iOffset+((BITS_PER_BYTE * sizeof(I16))-1))		\
							/ (BITS_PER_BYTE * sizeof(I16));					\
    piSrcSign = piSrcSignBase + c_iSignOffset;                                  \
    uiSignbitCount = 16 - (c_iOffset % (BITS_PER_BYTE * sizeof(I16)));          \
	if (uiSignbitCount == 16)													\
		uiSignbitData = *piSrcSign++;                                           \
	else																		\
		uiSignbitData = *(piSrcSign-1);											\
    }

#define RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData, uiSignbitCount, incr) \
    uiSignbitData <<= 1;                                                        \
    uiSignbitData |= ((iResult >> 31) & 0x00000001);                            \
    uiSignbitCount += 1;                                                        \
    if (uiSignbitCount >= 16)                                                   \
    {   /* Signbit register is full! Write it out */                            \
        *piDstSign = (I16) uiSignbitData;                                       \
        piDstSign += incr;                                                      \
        DEBUG_ONLY( uiSignbitData = 0 );                                        \
        uiSignbitCount = 0;                                                     \
    }

#define COMBINE_SIGNBIT(iResult, iSrc, piSrcSign, uiSignbitData, uiSignbitCount) \
    if (0 == uiSignbitCount)                                                     \
    {   /* Signbit register is empty! Read new one in */                         \
        uiSignbitData = *piSrcSign;                                              \
        piSrcSign += 1;                                                          \
        uiSignbitCount = 16;                                                     \
    }                                                                            \
    if (uiSignbitData & 0x00008000)                                              \
        iResult = (-1) & 0xFFFF0000;                                             \
    else                                                                         \
        iResult = 0;                                                             \
    iResult |= (iSrc & 0x0000FFFF);                                              \
    uiSignbitData <<= 1;                                                         \
    uiSignbitCount -= 1;

#define SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData)								 \
	if (0 != uiSignbitCount)													 \
		*(piSrcSign-1) = (I16)uiSignbitData;	/* must save partial shift */

#define ASSERT_SIGNBIT_POINTER(piSrcOrDstSign)								     \
	assert( piSrcOrDstSign != NULL );

#else   // PREVOUTPUT_16BITS

// Use I16 min/max in place of I17
#define I17_MIN SHRT_MIN
#define I17_MAX SHRT_MAX

#define CALC_SIGN_OFFSETS(piSrcBase, piSrc, piSrcSignBase, piSrcSign,            \
                           uiSignData, uiSignCount, cChan)                             
#define RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData, uiSignbitCount, incr)
#define COMBINE_SIGNBIT(iResult, iSrc, piSrcSign, uiSignbitData, uiSignbitCount) \
    iResult = iSrc
#define SAVE_SIGNBIT_STATE(piSrcSign,uiSignData)
#define ASSERT_SIGNBIT_POINTER(piSrcOrDstSign)

#endif  // PREVOUTPUT_16BITS


#if defined(INTERPOLATED_DOWNSAMPLE)

#define INTERPOL_FRACTBITS  30

void prvInterpolateInit(CAudioObject *pau, Int iSrcSampleRate,
                        Int iDstSampleRate, Int iAddInterval);

#ifdef BUILD_INTEGER
void auLowPass(CAudioObject *pau, Int *rgiCoef, Int iNumCoefs);
#else
void auLowPass(CAudioObject *pau, Float *rgfltCoef, Int iNumCoefs);
#endif  // BUILD_INTEGER
#endif  // defined(INTERPOLATED_DOWNSAMPLE)

#ifdef INTERPOLATED_DOWNSAMPLE
#define DOWNSAMPLE_ONLY(x)  (x)
void prvInterpolate(CAudioObject *pau, I16 *piSourceBuffer, Int iNumSrcSamples, Int iChannels,
                    I16 *piDestBuffer, Int *piNumDstSamples, U16* pcSamples);
#else   // INTERPOLATED_DOWNSAMPLE
#define DOWNSAMPLE_ONLY(x)
#endif  // INTERPOLATED_DOWNSAMPLE

#ifndef BUILD_INTEGER
// do not scale WeightFactor when using float
#undef WEIGHTFACTOR_FRACT_BITS
#define WEIGHTFACTOR_FRACT_BITS 0
#endif

//Void auCalcQuantStep(Int iQuantStepSize, U32 *piQuantStep, U32 *piFractBits);
QuantStepType qstCalcQuantStep(Int iQuantStepSize);
QuantFloat prvWeightedQuantization(CAudioObject *pau, PerChannelInfo *ppcinfo, Int iBark);


void SetActualPower(const I16 *piCoefQ, const int iCount,
                    PerChannelInfo *ppcinfo, const Status codecStatus);
void SetActualPowerHighRate(const I32 *piCoefRecon, const int iCount,
                    PerChannelInfo *ppcinfo, const Status codecStatus);


// **************** Monitor Range of Values for Debugging ******************************
// Place MONITOR_RANGE(name,variable) in the code where you want to monitor a variable,
// and use DUMP_MONITOR_RANGES(0) in the exit code where you want to print out the results
// If you want to dump on a frame-by-frame basis, use REINIT_MONITOR_RANGE after the dump
// Finally, add an appropriate DECL_MONITOR_RANGE(name) in wmaOS.c 
// and add DUMP_MONITOR_RANGE(name) to the DumpMonitorRanges() function there.
// **************************************************************************************
#if defined(_DEBUG) && defined(WMA_MONITOR)
// some MONITOR_RANGE variations can be defined.
// First variation is to print the variable to stdout
//#	define WMA_MONITOR_PRINT "gMR_rgfltWeightFactor"
// Second variation allows you to break when the fabs(variable) exceeds a threshold
//#	define WMA_MONITOR_BREAK "gMR_rgfltWeightFactor"
//#	define WMA_MONITOR_BREAK_THRESHOLD 50
// both of these variations are slow because strcmp is done at runtime instead of compile time - but this is debug code.
#	ifdef WMA_MONITOR_PRINT
#		define MONITOR_RANGE(a,x) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; \
									if (strcmp(#a,WMA_MONITOR_PRINT)==0) printf("%14.6g\n",(double)(x));  }
#	elif defined(WMA_MONITOR_BREAK)
#		define MONITOR_RANGE(a,x) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; \
									if (strcmp(#a,WMA_MONITOR_BREAK)==0 && fabs(x)>WMA_MONITOR_BREAK_THRESHOLD) DEBUG_BREAK();  }
#	else
#		define MONITOR_RANGE(a,x) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; }
#	endif
#	define MONITOR_RANGE_CONDITIONAL(cond,a,x) if (cond) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; }
#	define MONITOR_COUNT(a,c) { extern long lc##a; lc##a+=c; }
#	define MONITOR_COUNT_CONDITIONAL(cond,a,c) if (cond) { extern long lc##a; lc##a+=c; }
#	define DUMP_MONITOR_RANGES(fAppend) { void DumpMonitorRanges(int f); DumpMonitorRanges(fAppend); }
#	define REINIT_MONITOR_RANGE(a) { extern double a[]; extern long lc##a; a[1] = -( a[0] = 1.0e30 );  a[2] = a[3] = 0; lc##a = 0; }
#	define DUMP_MONITOR_RANGE(a) { extern double a[]; extern long lc##a; if ( a[0] == 1.0e30 && a[3]==0 ) { a[0] = a[1] = 0; } if (lc##a>0) printf("%14.6g %14.6g %14.6g %14.6g %8ld " #a "\n", a[0], a[1], a[2]/lc##a, sqrt((lc##a*a[3]-a[2]*a[2])/((double)(lc##a)*(lc##a-1))), lc##a ); }
#else // not (defined(_DEBUG) && defined(WMA_MONITOR))
#	define MONITOR_RANGE(a,x)
#	define MONITOR_RANGE_CONDITIONAL(cond,a,x)
#	define MONITOR_COUNT(a,c)
#	define MONITOR_COUNT_CONDITIONAL(cond,a,c)
#	define DUMP_MONITOR_RANGES(fAppend)
#	define REINIT_MONITOR_RANGE(a)
#	define DUMP_MONITOR_RANGE(a) 
#endif

#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
    void prvWmaShowFrames(CAudioObject* pau, const char* pszID, const char* pszFmt, ...);
#endif

#if defined(HEAP_DEBUG_TEST) && defined(_DEBUG)
void HeapDebugCheck();
#define HEAP_DEBUG_CHECK HeapDebugCheck()
#else
#define HEAP_DEBUG_CHECK
#endif

// **************************************************************************
// Encoding Options
// **************************************************************************
#define ENCOPT_BARK             0x0001
#define ENCOPT_V5LPC            0x0020
#define ENCOPT_SUPERFRAME       0x0002
#define ENCOPT_SUBFRAME         0x0004

#define ENCOPT_SUBFRAMEDIVMASK  0x0018
#define ENCOPT_SUBFRAMEDIVSHR   3

#define ENCOPT_TRANSCODE        0x0040


// **************************************************************************
// Player Options
// **************************************************************************
#define PLAYOPT_DOWNSAMPLE32TO22 0x0001
#define PLAYOPT_HALFTRANSFORM    0x0002
#define PLAYOPT_UPSAMPLE         0x0004
#define PLAYOPT_PAD2XTRANSFORM   0x0008


//mallocAligned allocates a buffer of size (+iAlignToBytes for allignment padding) bytes 
//which is alligned on an iAllignToBytes byte boundary
//the buffer must be deallocated using freeAligned
//and it is only safe to use size bytes as the padding could be at the begining or ending
void *mallocAligned(size_t size,Int iAlignToBytes);
void freeAligned(void *ptr);

// ----- Memory Allocation Functions -----
typedef enum {MAS_ALLOCATE, MAS_LOCKED, MAS_DELETE} MEMALLOCSTATE;

I32 auMallocGetCount(void);
void auMallocSetState(const MEMALLOCSTATE maState, void *pBuf, const I32 iBufSize);
void *auMalloc(const size_t iSize);
void auFree(void *pFree);


#endif //__MSAUDIO_H_
