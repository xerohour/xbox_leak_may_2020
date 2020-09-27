/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    LowRate.c

Abstract:

    Implementation of functions only used by low bitrate or mid bitrates
	e.g. MidRate and LowRate specific code

Author:

    Wei-ge Chen (wchen) 14-July-1998

Revision History:
	Sil Sanders (sils)	17-Dec-1999 - Added MidRate specific functions


*************************************************************************/

#pragma code_seg("WMADEC")
#pragma data_seg("WMADEC_RW")
#pragma const_seg("WMADEC_RD")

#ifdef ENABLE_ALL_ENCOPT

#include <math.h>
#include <limits.h>
#include "stdio.h"

#include "AutoProfile.h" 
#include "msaudio.h"
#include "wmamath.h"
#include "..\..\..\dsound\dsndver.h"

#pragma warning (disable:4554)

#if defined(REFERENCE_RAND_24) || defined(REFERENCE_RAND_16)
#	pragma COMPILER_MESSAGE(__FILE__ "(36) : Warning - Non-32-bit random number generator in use.")
#endif

// DEBUG_BREAK at a particular Frame in prvInverseQuantizeHighRate or prvInverseQuantizeLowRate 
//#define INVERSE_QUANTIZE_AT_FRAME 178
// PRINT CoefRecon for all Frames in range (define or undefine both at once)
//#define PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST 177
//#define PRINT_INVERSE_QUANTIZE_AT_FRAME_LAST  177
#if defined(_DEBUG) && ( defined(INVERSE_QUANTIZE_AT_FRAME) || defined(PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST)  )
#	pragma COMPILER_MESSAGE(__FILE__ "(45) : Warning - Inverse Quantize LowRate/MidRate Debug Code Enabled.")
// these help match where we are with V4 which does not have a m_iFrameCount
extern int iInvQuantFrameCount = 0;
extern int iInvQuantSubFrameCount = 0;
#endif
#if defined(_DEBUG) && defined(INVERSE_QUANTIZE_AT_FRAME)
// these help match where we are with V4 which does not have a m_iFrameCount
#	define DEBUG_BREAK_AT_FRAME_INV_QUANT										\
		{ 																		\
			if ( pau->m_iCurrSubFrame == 0 && ppcinfo == pau->m_rgpcinfo )		\
			{	iInvQuantFrameCount++;	iInvQuantSubFrameCount = 0; }			\
			else if ( ppcinfo == pau->m_rgpcinfo ) iInvQuantSubFrameCount++;	\
			if ( iInvQuantFrameCount==INVERSE_QUANTIZE_AT_FRAME )				\
				DEBUG_BREAK();													\
		}
#elif defined(_DEBUG) && ( defined(PRINT_INVERSE_QUANTIZE_AT_FRAME) || defined(PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST) )
// these help match where we are with V4 which does not have a m_iFrameCount
#	define DEBUG_BREAK_AT_FRAME_INV_QUANT										\
		{ 																		\
			if ( pau->m_iCurrSubFrame == 0 && ppcinfo == pau->m_rgpcinfo )		\
			{	iInvQuantFrameCount++;	iInvQuantSubFrameCount = 0; }			\
			else if ( ppcinfo == pau->m_rgpcinfo ) iInvQuantSubFrameCount++;	\
		}
#else	// defined(_DEBUG) && defined(PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST) 
#	define DEBUG_BREAK_AT_FRAME_INV_QUANT
#endif	// ...

#ifdef BUILD_INTEGER
//Integer Version

//*********************************************************************************************************
// Verify CoefRecon
// Debug code which simulates V4 floating point code.
// If decoded inputs are in error, false verifications can occur.
// include this by defining VERIFY_DECODED_COEFS below or in the project file
//*********************************************************************************************************
//#define VERIFY_DECODED_COEFS
#if defined(_DEBUG) && defined(VERIFY_DECODED_COEFS)
#	pragma COMPILER_MESSAGE(__FILE__ "(83) : Warning - Inverse Quantize LowRate Verification Code Enabled.")
static int fIgnoreLRVerification = 1;		// avoid verify checks - set by hand in debugger only
static Float fltLowRateThreshold = 0.1F;	// change from the debugger ((really should be much smaller!!!))
void VerifyCoefReconLR( CAudioObject* pau, PerChannelInfo* ppcinfo, 
					  int iRecon, int iBark, I32 qrand, I32 iCoefRecon, int iCoefQ, int iWeightFactor, 
					  const U8* rgbBandNotCoded, int iNoiseBand, int cNoiseBand, 
					  const Int* rgiNoisePower, const FastFloat* rgffltSqrtBWRatio, const int* m_rgiBarkIndex )
{	// stolen and adapted from V4 
	if ( !fIgnoreLRVerification )
	{
		Float fltiCoefRecon = (Float)iCoefRecon/(1<<TRANSFORM_FRACT_BITS);
		Float fltNoise = ((Float) qrand / (Float) 0x20000000);
		Float fltInvMaxWeight = (Float)(1<<WEIGHTFACTOR_FRACT_BITS)/((Float)ppcinfo->m_iMaxWeight);
		Float fltWeightFactor = ((Float)iWeightFactor)/(1<<WEIGHTFACTOR_FRACT_BITS);
		double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);
		Float dif, rel, fltCoefRecon, fltNoisePower;
		static fInBandNotCoded = 0;
		if (!pau->m_fNoiseSub) {
			if (iRecon < pau->m_cLowCutOff  || iRecon >= pau->m_cHighCutOff) 
				fltCoefRecon = 0;
			else
    			fltCoefRecon = (Float)( (Float)iCoefQ  * fltWeightFactor * dblQuantStep * fltInvMaxWeight );
		}
		else
		{
			if (iRecon < pau->m_cLowCutOff)  {
				fltNoise *= pau->m_fltDitherLevel;
				fltCoefRecon = (Float)(fltNoise * fltWeightFactor * dblQuantStep * fltInvMaxWeight);
				fInBandNotCoded = 0;
			}
			else if ( (fInBandNotCoded && iRecon >= m_rgiBarkIndex [iBark + 1] && rgbBandNotCoded [iBark+1] == 0) 
					|| (fInBandNotCoded==0 && iRecon >= pau->m_cHighCutOff) )  {
				fltNoise *= pau->m_fltDitherLevel;
				fltCoefRecon = (Float)(fltNoise * fltWeightFactor * dblQuantStep * fltInvMaxWeight);
				fInBandNotCoded = 0;
			}
			else if (iRecon >= pau->m_iFirstNoiseIndex && iBark < NUM_BARK_BAND && rgbBandNotCoded [iBark] == 1)
			{
				fltNoisePower = ((Float) pow (10, rgiNoisePower [iNoiseBand] / 20.0F)) * FloatFromFastFloat( rgffltSqrtBWRatio[iNoiseBand] );
				fltCoefRecon = fltNoise * fltNoisePower * fltWeightFactor * fltInvMaxWeight;
				fInBandNotCoded = 1;
			}
			else 
			{
				fltNoise *= pau->m_fltDitherLevel;
				fltCoefRecon = (Float)((iCoefQ + fltNoise) * fltWeightFactor * dblQuantStep * fltInvMaxWeight);
				fInBandNotCoded = 0;
			}
		}
		dif = (float) fabs(fltCoefRecon - fltiCoefRecon);
		rel = (float) fabs( (fabs(fltCoefRecon)>32) ? (dif/fltCoefRecon) : (dif/32));
		if ( rel > fltLowRateThreshold )
			DEBUG_BREAK();
		else 
		{	// when magnitude of integer version is much larger than float, we sometimes see differences
			// expect some false triggers, but often there is a relevant difference
			dif = (float)( fabs(fltiCoefRecon) - fabs(fltCoefRecon) );
			rel = (float) fabs( (fabs(fltCoefRecon)>32) ? (dif/fltCoefRecon) : (dif/32));
			if ( dif > 75.0f && rel > 0.000125f )		// test with all1_22s_22
				DEBUG_BREAK();
		}
	}
	MONITOR_RANGE(gMR_CoefRecon,iCoefRecon/((Float)(1<<TRANSFORM_FRACT_BITS)));
	MONITOR_RANGE(gMR_WeightRatio,(float)iWeightFactor/((float)ppcinfo->m_iMaxWeight));
	MONITOR_COUNT_CONDITIONAL(iCoefRecon==0,gMC_zeroCoefRecon,1);
#	if defined(PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST)
		if ( PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST <= iInvQuantFrameCount && iInvQuantFrameCount <= PRINT_INVERSE_QUANTIZE_AT_FRAME_LAST )
#           if UNDER_CE
			    MyOutputDbgStr(0,"%5d  %2d  %1d  %4d  %+16.6f  %+16.6f\n", 
				    iInvQuantFrameCount, iInvQuantSubFrameCount, ppcinfo-pau->m_rgpcinfo, iRecon, 
                    FLOAT_FROM_WEIGHT(rgiWeightFactor[iRecon]), FLOAT_FROM_COEF(rgiCoefRecon[iRecon]) );
#           else // !UNDER_CE
			    printf("%5d  %2d  %1d  %4d  %+16.6f  %+16.6f\n", 
				    iInvQuantFrameCount, iInvQuantSubFrameCount, ppcinfo-pau->m_rgpcinfo, iRecon, 
                    FLOAT_FROM_WEIGHT(rgiWeightFactor[iRecon]), FLOAT_FROM_COEF(rgiCoefRecon[iRecon]) );
#           endif   // UNDER_CE
#	endif
}
#define VERIFY_COEF_RECON_LR(idx,qRand,cNBand)																			\
	 VerifyCoefReconLR(	pau, ppcinfo,																					\
						iRecon, iBark, qRand, rgiCoefRecon[iRecon], rgiCoefQ[iCoefQ], rgiWeightFactor[idx],				\
						rgbBandNotCoded, iNoiseBand, cNBand, rgiNoisePower, rgffltSqrtBWRatio, pau->m_rgiBarkIndex )
#elif defined(_DEBUG) && defined(WMA_MONITOR)
#define VERIFY_COEF_RECON_LR(idx,qRand,cNBand)																			\
	MONITOR_RANGE(gMR_CoefRecon,rgiCoefRecon[iRecon]/((Float)(1<<TRANSFORM_FRACT_BITS)));                               \
	MONITOR_RANGE(gMR_WeightRatio,(float)rgiWeightFactor[iRecon]/((float)ppcinfo->m_wtMaxWeight));                      \
	MONITOR_COUNT_CONDITIONAL(rgiCoefRecon[iRecon]==0,gMC_zeroCoefRecon,1);
#elif defined(_DEBUG) && defined(PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST)
#   if UNDER_CE
#       define VERIFY_COEF_RECON_LR(idx,qRand,cNBand)																	\
            if ( PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST <= iInvQuantFrameCount                                           \
                 && iInvQuantFrameCount <= PRINT_INVERSE_QUANTIZE_AT_FRAME_LAST )                                       \
		        MyOutputDbgStr(0,"%5d  %2d  %1d  %4d  %+16.6f  %+16.6f\n",                                              \
				    iInvQuantFrameCount, iInvQuantSubFrameCount, ppcinfo-pau->m_rgpcinfo, iRecon,                       \
                    FLOAT_FROM_WEIGHT(rgiWeightFactor[iRecon]), FLOAT_FROM_COEF(rgiCoefRecon[iRecon]) );
#   else // !UNDER_CE
#       define VERIFY_COEF_RECON_LR(idx,qRand,cNBand)																	\
            if ( PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST <= iInvQuantFrameCount                                           \
                 && iInvQuantFrameCount <= PRINT_INVERSE_QUANTIZE_AT_FRAME_LAST )                                       \
		        printf("%5d  %2d  %1d  %4d  %+16.6f  %+16.6f\n",                                                        \
				    iInvQuantFrameCount, iInvQuantSubFrameCount, ppcinfo-pau->m_rgpcinfo, iRecon,                       \
                    FLOAT_FROM_WEIGHT(rgiWeightFactor[iRecon]), FLOAT_FROM_COEF(rgiCoefRecon[iRecon]) );
#   endif   // UNDER_CE
#else	// so not ( defined(_DEBUG) && defined(VERIFY_DECODED_COEFS) )
#define VERIFY_COEF_RECON_LR(idx,qRand,cNBand)
#endif	// defined(_DEBUG) && defined(VERIFY_DECODED_COEFS)




#define INV_MAX_WEIGHT_FRAC_BITS 30
#define MORE_WF_FRAC_BITS 0

#if (defined(BUILD_INTEGER) || defined(INTEGER_ENCODER))

    // WinCE and embedded compilers have lousy performance for I64 multiplication and division.
    // On the SH3, this saves .35% of playtime decoding 16m_16 by speeding up InverseQuadRoot 14%.

    INLINE UInt uiInverseMaxWeight(WeightType wtMaxWeight)
    {   
        U32 uiMSF = wtMaxWeight;
        Int iExp = (32-INV_MAX_WEIGHT_FRAC_BITS)+(32-WEIGHTFACTOR_FRACT_BITS);  // (32-30)+(32-21)=13
        int iMSF8;
        U32 uiFrac1;
        if ( uiMSF < ((U32)(1<<((32-INV_MAX_WEIGHT_FRAC_BITS)+(32-WEIGHTFACTOR_FRACT_BITS)))) ) 
            return 0xFFFFFFFF ;
        // normalize the fractional part
        while( (uiMSF & 0xF0000000)==0 ) {
            iExp -= 4;
            uiMSF <<= 4;
        }
        while( (uiMSF & 0x80000000)==0 ) {
            iExp--;
            uiMSF <<= 1;
        }
        // discard the most significant one bit (it's presence is built into g_InverseFraction)
        iExp--;
        uiMSF <<= 1;
        assert( iExp >= 0 );
        if ( iExp < 0 )
            return 0xFFFFFFFF ;
        // split into top INVERSE_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
        iMSF8 = uiMSF>>(32-INVERSE_FRACTION_TABLE_LOG2_SIZE);
        uiMSF <<= INVERSE_FRACTION_TABLE_LOG2_SIZE;
        // lookup and interpolate - tables are set up to return correct binary point for WeightType (we hope)
        uiFrac1  = g_InverseFraction[iMSF8++];
        uiFrac1 -= MULT_HI_UDWORD( uiMSF, uiFrac1 - g_InverseFraction[iMSF8] );
        return uiFrac1 >> iExp;
    }

#   define INVERSE_MAX_WEIGHT(mw) uiInverseMaxWeight(mw)

#else

#   define INVERSE_MAX_WEIGHT(mw) ((UInt)((((I64)1)<<(INV_MAX_WEIGHT_FRAC_BITS+WEIGHTFACTOR_FRACT_BITS))/(mw)))

#endif

//**************************************************************************************************
//
// prvInverseQuantizeLowRate handles LowRate inverse quantization
//
//**************************************************************************************************
WMARESULT prvInverseQuantizeLowRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor)
{
	Int iBark = 0;
	Int iCoefQ = 0;
	Int iRecon = 0;
    UInt uiWeightFactor, uiMaxWeight, uiQuantStepXInvMaxWeight;
    Int QuantStepXMaxWeightXWeightFactor, qrand, iNoise;
	Int cQSIMWFracBits, cFracBits, cFracBits2, cWFFracBits, cMaxWeightFracBits;
    Float fltCoefRecon;
    Int *rgiCoefRecon = ppcinfo->m_rgiCoefRecon;
    const I16* const rgiCoefQ                = ppcinfo->m_rgiCoefQ;
    const U8* const rgbBandNotCoded          = ppcinfo->m_rgbBandNotCoded;
    const Int* const rgiNoisePower           = ppcinfo->m_rgiNoisePower;
    const FastFloat* const rgffltSqrtBWRatio = ppcinfo->m_rgffltSqrtBWRatio;
    U8  cNoiseBand;
    Int iNoiseBand = 0;
	const Int iDitherFactor = 0x51EB851F;		// LPC  0.04 * 2^35 
	UInt uiInvMaxWeight = INVERSE_MAX_WEIGHT(ppcinfo->m_wtMaxWeight);
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,INVERSE_QUAN_LOW_RATE_PROFILE);
	//assert( uiInvMaxWeight == (UInt)((float)(1<<INV_MAX_WEIGHT_FRAC_BITS)*(float)(1<<WEIGHTFACTOR_FRACT_BITS)/((float)ppcinfo->m_wtMaxWeight) ) );
	//// Float 1/MaxWeight = uiInvMaxWeight/(1.0F*(1<<30))

	DEBUG_BREAK_AT_FRAME_INV_QUANT;
	MONITOR_COUNT(gMC_IQ,1);

    //Calculate QuantStep X invMaxWeight
    cQSIMWFracBits  = pau->m_qstQuantStep.iFracBits;                           
    uiQuantStepXInvMaxWeight = MULT_HI_DWORD(pau->m_qstQuantStep.iFraction,uiInvMaxWeight);
    //uiQuantStepXInvMaxWeight = (MULT_HI_DWORD(pau->m_qstQuantStep.iFraction,uiInvMaxWeight>>1)<<1);
	cQSIMWFracBits += (INV_MAX_WEIGHT_FRAC_BITS-32);
	// Float QSIMWF = uiQuantStepXInvMaxWeight*1.0F/(1<<cQSIMWFracBits)
	NormUInt( &uiQuantStepXInvMaxWeight, &cQSIMWFracBits, 0x3FFFFFFF );
	MONITOR_RANGE(gMR_QuantStepXInvMaxWeight,uiQuantStepXInvMaxWeight/pow(2,cQSIMWFracBits));

	// since all weights are less than MaxWeight, fast scale below by MaxWeight's FracBits
	uiMaxWeight = ppcinfo->m_wtMaxWeight<<MORE_WF_FRAC_BITS;;
	cMaxWeightFracBits = MORE_WF_FRAC_BITS;		// really should be WEIGHTFACTOR_FRACT_BITS+MORE_WF_FRAC_BITS but this way is for shift delta

	NormUInt( &uiMaxWeight, &cMaxWeightFracBits, 0x3FFFFFFF );

    if (!pau->m_fNoiseSub) {
		// Unusual case, but 8kHz Mono 8kpbs gets here
        memset (rgiCoefRecon, 0, pau->m_cLowCutOff * sizeof (Int));
        for (iRecon = pau->m_cLowCutOff; iRecon < pau->m_cHighCutOff; iRecon++, iCoefQ++)
		{	// rgfltCoefRecon [iRecon] = Float ((Float) (rgiCoefQ [iCoefQ]) * rgfltWeightFactor [iRecon] * dblQuantStep * fltInvMaxWeight);
			int iCoef;
			assert( rgiWeightFactor[iRecon] <= (0x7FFFFFFF>>cMaxWeightFracBits) );
			uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
			cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
			//// Float WeightFactor = uiWeightFactor*1.0F/(1<<cWFFracBits)
			assert( uiWeightFactor <= 0x7FFFFFFF );
			NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		
			MONITOR_RANGE(gMR_weightFactor,uiWeightFactor/pow(2,cWFFracBits));

			QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);		
			cFracBits = cQSIMWFracBits+cWFFracBits-31;
			//// Float QuantStep*WeightFactor/InvMaxWeight = QuantStepXMaxWeightXWeightFactor/(1.0F*(1<<cFracBits))
			MONITOR_RANGE(gMR_QuantStepXMaxWeightXWeightFactor,QuantStepXMaxWeightXWeightFactor/pow(2,cFracBits));

			iCoef = MULT_HI(((Int)(rgiCoefQ[iCoefQ])<<20),QuantStepXMaxWeightXWeightFactor);
			cFracBits += (20-31);
			//// Float Coef = iCoef*1.0F/(1<<cFracBits))

			rgiCoefRecon[iRecon] = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );
			//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F

			VERIFY_COEF_RECON_LR(iRecon,0,0);
		}
        memset (rgiCoefRecon + pau->m_cHighCutOff, 0, (pau->m_cSubband - pau->m_cHighCutOff) * sizeof (Int));
		FUNCTION_PROFILE_STOP(&fp);
		return WMA_OK;
    }

    cNoiseBand = rgbBandNotCoded [0];

	if (iRecon < pau->m_cLowCutOff ) {
		// not integerized since cLowCutOff is typically 0, so this is here for compatability with V1 
		Double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);
		while  (iRecon < pau->m_cLowCutOff) 	{
			Float fltNoise = pau->m_fltDitherLevel * ((Float) quickRand (&(pau->m_tRandState)) / (Float) 0x20000000);//rgfltNoise [iRecon];
			Float fltWeightFactor = ((float)rgiWeightFactor [pau->m_cLowCutOff])/(1<<WEIGHTFACTOR_FRACT_BITS);
			// SH4 warning CBE4717 on the next line is ignorable - appraently a compiler mistake
			fltCoefRecon  = (Float) (fltNoise * fltWeightFactor * dblQuantStep * (float)(1<<WEIGHTFACTOR_FRACT_BITS)/((float)ppcinfo->m_wtMaxWeight) );
			rgiCoefRecon [iRecon] = (Int)(fltCoefRecon * (1<<TRANSFORM_FRACT_BITS));
			MONITOR_RANGE(gMR_CoefRecon,rgiCoefRecon[iRecon]/32.0f);
			MONITOR_RANGE(gMR_WeightRatio,(fltWeightFactor*(float)(1<<WEIGHTFACTOR_FRACT_BITS)/((float)ppcinfo->m_wtMaxWeight)));
			MONITOR_COUNT(gMC_IQ_Float,9);
			iRecon++;
		}
	}

	while  (iRecon < pau->m_iFirstNoiseIndex) {
        int iCoef,iCoefScaled,iCoefRecon,iNoiseScaled,iNoiseQuant;         
        assert(TRANSFORM_FRACT_BITS==5);
        qrand = quickRand (&(pau->m_tRandState));

        if (iRecon >= pau->m_rgiBarkIndex [iBark + 1]) 
			iBark++;
        assert(iBark < NUM_BARK_BAND);

	    // Since weight factors became unsigned, the following assert is not
	    // valid. Other wrap-around detection would have to be performed elsewhere.
		assert( rgiWeightFactor[iRecon] <= (0x7FFFFFFF>>cMaxWeightFracBits) );
        uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
        cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
		//// Float WeightFactor = uiWeightFactor/(1.0F*(1<<cWFFracBits))
	    // Since weight factors became unsigned, the following assert is not
	    // valid. Other wrap-around detection would have to be performed elsewhere.
	    assert( uiWeightFactor <= 0x7FFFFFFF );
		NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		// weightFactor with cWFFracBits fractional bits
		MONITOR_RANGE(gMR_weightFactor,uiWeightFactor/pow(2,cWFFracBits));
        
        QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);		
        cFracBits = cQSIMWFracBits+cWFFracBits-31;
        //// Float QuantStep*WeightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1.0F*(1<<cFracBits))
		MONITOR_RANGE(gMR_QuantStepXMaxWeightXWeightFactor,QuantStepXMaxWeightXWeightFactor/pow(2,cFracBits));

        iCoef = MULT_HI(((Int)(rgiCoefQ[iCoefQ])<<20),QuantStepXMaxWeightXWeightFactor);
		cFracBits += (20-31);
		//// Float Coef = iCoef/(1.0F*(1<<cFracBits))

        //Rescale to TRANSFORM_FRACT_BITS for outputing to the inverse transform so that (float)iCoefScaled/(1<<TRANSFORM_FRACT_BITS)
		iCoefScaled = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );
		//// Float CoefScaled = iCoefScaled/32.0F

        iNoise = MULT_HI(iDitherFactor,qrand);
		cFracBits2 = 35+29-31;											// == 33
		//// Float Noise = iNoise/(1024.0F*(1<<(cFracBits2-10)))
        iNoiseQuant = MULT_HI(iNoise,QuantStepXMaxWeightXWeightFactor);
		cFracBits2 += ((cQSIMWFracBits+cWFFracBits-31)-31);
		//// Float NoiseQuant = iNoiseQuant/(1024.0F*(1<<(cFracBits2-10)))
		assert( (cFracBits+(33-20)) == cFracBits2 );

        //rescale iNoiseQuant so that (float)iNoiseScaled/(1<<TRANSFORM_FRACT_BITS)
		iNoiseScaled = Align2FracBits( iNoiseQuant, cFracBits2, TRANSFORM_FRACT_BITS );
		//// Float NoiseScaled = iNoiseScaled/32.0F

        iCoefRecon = iCoefScaled + iNoiseScaled;
        rgiCoefRecon [iRecon] = iCoefRecon;
		//// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F

		VERIFY_COEF_RECON_LR(iRecon,qrand,cNoiseBand);

        iRecon++;
		iCoefQ++;
    }

    while  (iRecon < pau->m_cHighCutOff) {
		if (iRecon >= pau->m_rgiBarkIndex [iBark + 1]) 
			iBark++;
        assert(iBark < NUM_BARK_BAND);
        
		if (rgbBandNotCoded [iBark] == 1)
        {
			FastFloat ffltNoisePower;
            UInt uiNoisePowerXinvMaxWeight;
			Int iUBLimitOniRecon = min(pau->m_rgiBarkIndex [iBark + 1], pau->m_cHighCutOff);

			assert( iNoiseBand < cNoiseBand );

			ffltNoisePower = qstCalcQuantStep( rgiNoisePower[iNoiseBand] );
			//// Float Noise Power = ffltNoisePower.iFraction/(1.0F*(1<<ffltNoisePower.iFracBits))

			ffltNoisePower = ffltMultiply( ffltNoisePower, rgffltSqrtBWRatio[iNoiseBand] );
			//// Float Noise Power = ffltNoisePower.iFraction/(1.0F*(1<<ffltNoisePower.iFracBits))

            uiNoisePowerXinvMaxWeight = MULT_HI(ffltNoisePower.iFraction,uiInvMaxWeight>>1)<<1;
			cFracBits = ffltNoisePower.iFracBits + (INV_MAX_WEIGHT_FRAC_BITS-31);
			//// Float NoisePower/MaxWeight = uiNoisePowerXinvMaxWeight/(1.0F*(1<<cFracBits))
			NormUInt( &uiNoisePowerXinvMaxWeight, &cFracBits, 0x3FFFFFFF );

            while (iRecon < iUBLimitOniRecon)
            {
                Int iNoiseRand,iNoiseWeighted,iCoefRecon;

                qrand = quickRand (&(pau->m_tRandState));										// FB = 29												
                iNoiseRand = MULT_HI(uiNoisePowerXinvMaxWeight,qrand);
                cFracBits2 = cFracBits+29-31;
				//// Float NoiseRand = iNoiseRand/(1.0F*(1<<cFracBits2))

	            // Since weight factors became unsigned, the following assert is not
	            // valid. Other wrap-around detection would have to be performed elsewhere.
				assert( rgiWeightFactor[iRecon] <= (0x7FFFFFFF>>cMaxWeightFracBits) );
				uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
				cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
				//// Float WeightFactor = 	uiWeightFactor/(1024.0F*(1<<(cWFFracBits-10)))
	            // Since weight factors became unsigned, the following assert is not
	            // valid. Other wrap-around detection would have to be performed elsewhere.
				assert( uiWeightFactor <= 0x7FFFFFFF );
				NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		// uiWeightFactor with cWFFracBits fractional bits
				MONITOR_RANGE(gMR_weightFactor,uiWeightFactor/pow(2,cWFFracBits));

                iNoiseWeighted = MULT_HI(iNoiseRand,uiWeightFactor);	
				cFracBits2 += (cWFFracBits-31);
				//// Float NoiseWeighted = iNoiseWeighted/(1024.0F*(1<<(cFracBits2-10)))

				iCoefRecon = Align2FracBits( iNoiseWeighted, cFracBits2, TRANSFORM_FRACT_BITS );  //scale so that (float)iCoefRecon/(1<<TRANSFORM_FRACT_BITS)
                rgiCoefRecon [iRecon] = iCoefRecon;
				//// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F

				VERIFY_COEF_RECON_LR(iRecon,qrand,cNoiseBand);

				iRecon++;	
			}

			iNoiseBand++;
		}
		else 
		{	// This should be the same as the first < FirstNoiseIndex loop
			// Float fltNoise = pau->m_fltDitherLevel * ((Float) quickRand (&(pau->m_tRandState)) / (Float) 0x20000000);
			// rgfltCoefRecon [iRecon] = (Float) ((rgiCoefQ [iCoefQ] + fltNoise) * rgfltWeightFactor[iRecon] * dblQuantStep * fltInvMaxWeight);			

            Int iCoef,iNoiseQuant,iCoefScaled,iCoefRecon;

            if (iRecon >= pau->m_rgiBarkIndex [iBark + 1]) 
				iBark++;
            assert(iBark < NUM_BARK_BAND);

			qrand = quickRand (&(pau->m_tRandState));

			assert( rgiWeightFactor[iRecon] <= (0x7FFFFFFF>>cMaxWeightFracBits) );
			uiWeightFactor = rgiWeightFactor [iRecon] << cMaxWeightFracBits; 
			cWFFracBits = WEIGHTFACTOR_FRACT_BITS+cMaxWeightFracBits;
			//// Float WeightFactor = uiWeightFactor/(1024.0F*(1<<(cWFFracBits-10)))
			assert( uiWeightFactor <= 0x7FFFFFFF );
			NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		// uiWeightFactor with cWFFracBits fractional bits
			MONITOR_RANGE(gMR_weightFactor,uiWeightFactor/pow(2,cWFFracBits));

            QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);
            cFracBits = cQSIMWFracBits+cWFFracBits-31;
			//// Float QuantStep*WightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1.0F*(1<<cFracBits))
			MONITOR_RANGE(gMR_QuantStepXMaxWeightXWeightFactor,QuantStepXMaxWeightXWeightFactor/pow(2,cFracBits));

            iNoise = MULT_HI(iDitherFactor,qrand); 
			cFracBits2 = 35+29-31;							// FP = 33
			//// Float Noise = iNoise/(1024.0F*(1<<(33-10)))

            iNoiseQuant = MULT_HI(iNoise,QuantStepXMaxWeightXWeightFactor);
			cFracBits2 += (cFracBits-31);
			//// Float NoiseQuant = iNoiseQuant/(1024.0F*(1<<(cFracBits2-10)))

            iCoef = MULT_HI( ((Int)rgiCoefQ[iCoefQ])<<20, QuantStepXMaxWeightXWeightFactor );
			cFracBits += (20-31);
			//// Float Coef = iCoef/(1.0F*(1<<cFracBits))

			iCoefScaled = Align2FracBits( iCoef, cFracBits, TRANSFORM_FRACT_BITS );  
			//// Float CoefScaled = iCoefScaled/32.0F

            assert(cFracBits2>=TRANSFORM_FRACT_BITS);
            iCoefRecon = iCoefScaled + (iNoiseQuant>>(cFracBits2-TRANSFORM_FRACT_BITS));
            rgiCoefRecon [iRecon] = iCoefRecon;
			//// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F

			VERIFY_COEF_RECON_LR(iRecon,qrand,cNoiseBand);

            iRecon++;
		    iCoefQ++;
		}
	}

	{	//Calculate from highCutOff to m_cSubband
		UInt QuantStepXMaxWeightXWeightFactorXDither;
		
	    // Since weight factors became unsigned, the following assert is not
	    // valid. Other wrap-around detection would have to be performed elsewhere.
		assert( rgiWeightFactor[pau->m_cHighCutOff - 1] <= (0x7FFFFFFF>>cMaxWeightFracBits) );
		uiWeightFactor = rgiWeightFactor [pau->m_cHighCutOff - 1] << MORE_WF_FRAC_BITS;
		cWFFracBits = WEIGHTFACTOR_FRACT_BITS+MORE_WF_FRAC_BITS;
		//// Float WeightFactor = uiWeightFactor/(1024.0F*(1<<(cWFFracBits-10)))
		NormUInt( &uiWeightFactor, &cWFFracBits, 0x3FFFFFFF );		

		QuantStepXMaxWeightXWeightFactor = MULT_HI(uiQuantStepXInvMaxWeight,uiWeightFactor);
		cFracBits = cQSIMWFracBits+cWFFracBits-31;
		//// Float QuantStep*WeightFactor/MaxWeight = QuantStepXMaxWeightXWeightFactor/(1024.0F*(1<<(cFracBits-10)))
		NormUInt( (unsigned long *)&QuantStepXMaxWeightXWeightFactor, &cFracBits, 0x3FFFFFFF );

		QuantStepXMaxWeightXWeightFactorXDither = MULT_HI(QuantStepXMaxWeightXWeightFactor,iDitherFactor);
		cFracBits += (35-31);
		//// Float QS * WF/MaxWF * Dither = QuantStepXMaxWeightXWeightFactorXDither/(1024.0F*(1<<(cFracBits-10)))
		NormUInt( &QuantStepXMaxWeightXWeightFactorXDither, &cFracBits, 0x3FFFFFFF );

		while (iRecon < pau->m_cSubband) {
			Int iCoefScaled;
			Int qrand = quickRand (&(pau->m_tRandState));
			Int iCoefRecon = MULT_HI(QuantStepXMaxWeightXWeightFactorXDither,qrand);
			cFracBits2 = cFracBits + 29 - 31;
			//// Float CoefRecon = iCoefRecon/(1.0F*(1<<cFracBits2))
			
			iCoefScaled = Align2FracBits( iCoefRecon, cFracBits2, TRANSFORM_FRACT_BITS );
			rgiCoefRecon [iRecon] = iCoefScaled;		        
			//// Float CoefRecon = rgiCoefRecon [iRecon]/32.0F

			MONITOR_RANGE(gMR_QuantStepXMaxWeightXWeightFactor,QuantStepXMaxWeightXWeightFactor/pow(2,cFracBits));
			VERIFY_COEF_RECON_LR(pau->m_cHighCutOff - 1,qrand,cNoiseBand);

			iRecon++;
		}
	}

#   if defined(REFERENCE_RAND_24) || defined(REFERENCE_RAND_16)
        // call the random generator one extra time per subframe to improve subband randomness
        quickRand (&(pau->m_tRandState));
#   endif

	FUNCTION_PROFILE_STOP(&fp);
	return WMA_OK;
}
#pragma warning (default:4554)
#endif // BUILD_INTEGER

#endif //ENABLE_ALL_ENCOPT



//****************************************************************************
//****************************************************************************
//
//  MidRate 
//
//	Functions to implement MidRate
//
//****************************************************************************
//****************************************************************************

#ifdef ENABLE_ALL_ENCOPT
//#ifndef ENCODER


//******************************************************************************************
// Support routines for Resampling indexes to those used when the MaskQ's were last updated
//******************************************************************************************
#define MASKQ_RESAMPLE_OFFSET 6
typedef struct MaskResampleInfo
{
	Int iMaskResampleRatio;
	Int iMaskResampleRatioPow;
	Int cValidBarkBandLatestUpdate;
} MaskResampleInfo;

#ifdef HITACHI
#pragma inline(iResampleIndex)
static Int iResampleIndex( Int iRecon, const MaskResampleInfo MRI )
#else
static INLINE Int iResampleIndex( Int iRecon, const MaskResampleInfo MRI )
#endif
{	// return resampled linear index suitable for frame that was last updated with MaskQ
	// equvilent to iRecon shifted left or right by (MRI.iMaskResampleRatioPow-MASKQ_RESAMPLE_OFFSET)
	return ( (((iRecon) << MRI.iMaskResampleRatioPow) >> MASKQ_RESAMPLE_OFFSET) );
}

#ifdef HITACHI
#pragma inline(iUnResampleIndex)
static Int iUnResampleIndex( Int iResampled, const MaskResampleInfo MRI )
#else
static INLINE Int iUnResampleIndex( Int iResampled, const MaskResampleInfo MRI )
#endif
{	// return the reverse of iResampleIndex()
	// that is, convert from resampled indexes of MaskQ or WeightFactor to natural index for the current frame
    if (MRI.iMaskResampleRatioPow > MASKQ_RESAMPLE_OFFSET){
		// Rounding for down shift 
		// Although V4 did not round when resampling, this inverse funtion needs to round ???
        return (iResampled + (1<<(MRI.iMaskResampleRatioPow-(MASKQ_RESAMPLE_OFFSET+1)))) >> (MRI.iMaskResampleRatioPow-MASKQ_RESAMPLE_OFFSET);
        // return iResampled >> (MRI.iMaskResampleRatioPow-MASKQ_RESAMPLE_OFFSET);
    } else {
        return iResampled << (MASKQ_RESAMPLE_OFFSET-MRI.iMaskResampleRatioPow);
    }
}

// prvScanForNextBarkIndex scans for the next resampled bark index and then returns its unresampled linear index
//   iRecon and iHighLimit are linear indexes in the current frames natural scale.
//   iBarkResampled is the bark index in the resampled bark scale (appropriate for the frame when the MaskQ's were last updated)
//   rgiBarkIndex is the resampled bark index scale from that last updated MaskQ subframe
#ifdef HITACHI
#pragma inline(prvScanForNextBarkIndex)
static Int prvScanForNextBarkIndex( const Int iRecon, Int* piBarkResampled, const int iHighLimit, 
										  const Int* const rgiBarkIndexResampled, const MaskResampleInfo MRI )
#else
static INLINE Int prvScanForNextBarkIndex( const Int iRecon, Int* piBarkResampled, const int iHighLimit, 
										  const Int* const rgiBarkIndexResampled, const MaskResampleInfo MRI )
#endif
{   //Scan for the next resampled bark index
	Int iNextBarkIndex, iBarkResampled, iReconResampled;
	iReconResampled = iResampleIndex(iRecon, MRI);
    while ( iReconResampled >= rgiBarkIndexResampled [*piBarkResampled+1]) 
		++(*piBarkResampled);
	iBarkResampled = *piBarkResampled;
	if ( iBarkResampled >= MRI.cValidBarkBandLatestUpdate )
	{
		assert( iBarkResampled < MRI.cValidBarkBandLatestUpdate );
		return( MRI.cValidBarkBandLatestUpdate );
	}
	iNextBarkIndex = iUnResampleIndex( rgiBarkIndexResampled [iBarkResampled + 1], MRI );
    if (iNextBarkIndex > iHighLimit) 
		iNextBarkIndex = iHighLimit;
	return iNextBarkIndex;
}


//*****************************************************************************************
// Look up (10^(1/16)) ^ iMaskQ
//*****************************************************************************************
#ifdef HITACHI
#pragma inline(ffltMaskPower10)
static FastFloat ffltMaskPower10( Int iMaskQ )
#else
static INLINE FastFloat ffltMaskPower10( Int iMaskQ )
#endif
{	// (10^(1/16)) ^ iMaskQ
	FastFloat fflt;
	
#if !defined (ENCODER)
	assert( -MASK_MINUS_POWER_TABLE_SIZE < iMaskQ && iMaskQ <= MASK_PLUS_POWER_TABLE_SIZE );
#endif
	if ( iMaskQ <= 0 )
	{
		if (iMaskQ < -MASK_MINUS_POWER_TABLE_SIZE)
			iMaskQ = -(MASK_MINUS_POWER_TABLE_SIZE-1);
#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
		fflt.iFraction = rgiMaskMinusPower10[ -iMaskQ ];	// with MASK_POWER_FRAC_BITS==28 fractional bits
		fflt.iFracBits = MASK_POWER_FRAC_BITS+(-iMaskQ>>2);
#else
		fflt = rgiMaskMinusPower10[ -iMaskQ ] / (Float)(1<<(-iMaskQ>>2));
#endif
	}
	else
	{
		if (iMaskQ >= MASK_PLUS_POWER_TABLE_SIZE)
			iMaskQ = MASK_PLUS_POWER_TABLE_SIZE;
#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
		fflt.iFraction = rgiMaskPlusPower10[ iMaskQ-1 ];	// with MASK_POWER_FRAC_BITS==28 fractional bits
		fflt.iFracBits = MASK_POWER_FRAC_BITS-(iMaskQ>>2);
#else
		fflt = rgiMaskPlusPower10[ iMaskQ-1 ] * (Float)(1<<(iMaskQ>>2));
#endif
	}
	return fflt;
}

//function prototype, used to eliminate Mac warnings
Void prvGetBandWeightMidRate (CAudioObject* pau, PerChannelInfo* ppcinfo, const Int* const rgiBarkIndex, const Int* const rgiBarkIndexResampled, MaskResampleInfo MRI);

//*********************************************************************************
// prvGetBandWeightMidRate
// Calculate a band weight for bands which are not coded
// 
// BW = average( 10^(2*MaskQ[iBarkResampled]/16) ) over each linear frequency bin
// For most sampling rates, the bark scale when resampled does not equal the un-resampled bark scale
// But 32000 does have the nice property that the resampled bark scale = un-resampled in the noise substitution region
// So for 32000 Hz to 44099 Hz:  
//		BW = 10^((2/16)*MaskQ[iBarkResampled])
// For all other sampling frequencies:
//		BW = ( N1 * 10^(2*MarkQ[iBarkResampled]) + N2 * 10^(2*MarkQ[iBarkResampled+1]/16) ) / (N1 + N2)
//		where N1 is the number of linear frequency bins in the first resampled bark band
//		and   N2 is the number of linear frequency bins in the second resampled bark band
// BandWeights are only used as the sqrt of the ratio to the last noise BandWeight
// So for 32000 Hz to 44099 Hz
//		SqrtBWRatio = 10^((MaskQ[iBarkResampled] - MaskQ[iLast])/16)
// And for other sampling frequencies
//		SqrtBWRatio = sqrt( BW[i] / BW[last] )
// And for all cases where SqrtBWRatio[last] = 1
// Note that log to the base 10^(1/16) of the MaskQ are integers for 32000 but are not integers for non-32000Hz
// So doing all the inverse quantization in the exponent domain does not have the advantage of using integer exponents
// For this reason, SqrtBWRatio is stored and used as a FastFloat.
// TODO:  rgffltSqrtBWRatio seems to have a narrow range between 1/4 and 4. Consider making it a fixed point int.
// TODO:  Avoid recalculating when MaskQ's have not been updated and the resample ratio is the same as the previous subframe
//*********************************************************************************
Void prvGetBandWeightMidRate (CAudioObject* pau, PerChannelInfo* ppcinfo, 
							  const Int* const rgiBarkIndex, const Int* const rgiBarkIndexResampled, MaskResampleInfo MRI)
{
    U8*	rgbBandNotCoded			 = ppcinfo->m_rgbBandNotCoded;
	FastFloat* rgffltSqrtBWRatio = ppcinfo->m_rgffltSqrtBWRatio;
	Int* rgiMaskQ				 = ppcinfo->m_rgiMaskQ;
	U8 cNoiseBand	= 0;
	FastFloat fflt;
	Float flt;
	Int iCurrStart, iCurrBand, iCurrEnd;	// indexes for the current subframe
	Int iMaskStart, iMaskBand, iMaskEnd;	// indexes in the subframe where the MaskQ's were last updated
	Int iRsmpStart, iRsmpBand, iRsmpEnd;	// indexes for the current subframe resampled to the subframe where the MaskQ's were last updated
	int fAllBandsSynced = MRI.iMaskResampleRatioPow==MASKQ_RESAMPLE_OFFSET 
		|| ( pau->m_iVersion!=1 && 32000 <= pau->m_iSamplingRate && pau->m_iSamplingRate < 44100 );
	Int rgiMaskQ4BandNotCoded[10];			// used if bands Syncronized
	Float fltBandWeight[10];				// used if bands are no Syncronized
	Int cMaskHighCutOff;
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,GET_BAND_WEIGHTS_PROFILE);

	iCurrBand = pau->m_iFirstNoiseBand;
	iMaskBand = iRsmpBand = 0;
	if ( !fAllBandsSynced )
		cMaskHighCutOff = iResampleIndex( pau->m_cSubband, MRI ) * pau->m_cHighCutOffLong / pau->m_cFrameSampleHalf;

    while (WMAB_TRUE)    {
        assert (iCurrBand <= pau->m_cValidBarkBand);
        iCurrStart = max( rgiBarkIndex [iCurrBand], pau->m_iFirstNoiseIndex );
        if (iCurrStart >= pau->m_cHighCutOff)
            break;

		if (rgbBandNotCoded [iCurrBand] == 1)	{
			assert( cNoiseBand < 10 );		// probably (cNoiseBand < 5) would be OK

			// Determine the band for accessing the possibly resampled MaskQ's
			iMaskStart = iResampleIndex( iCurrStart, MRI );
			while ( rgiBarkIndexResampled [iMaskBand+1] <= iMaskStart ) 
				++iMaskBand;

			if ( fAllBandsSynced )
			{	// Current subFrame bands synced to band were MaskQ's were last updated
				rgiMaskQ4BandNotCoded[cNoiseBand] = rgiMaskQ[iMaskBand];
			}
			else
			{	// Have to check in detail about whether one or two MaskQ's are used by this BandNotCoded
				iCurrEnd   = min ( pau->m_cHighCutOff, rgiBarkIndex [iCurrBand + 1]);
				assert (iCurrEnd >= iCurrStart);
				iRsmpStart = iMaskStart;
				iMaskEnd = min( rgiBarkIndexResampled [ iMaskBand+1 ], cMaskHighCutOff );
				iRsmpEnd = min( iResampleIndex( iCurrEnd, MRI ), cMaskHighCutOff );
				while ( rgiBarkIndexResampled [iRsmpBand+1] <= (iRsmpEnd-1) ) 
					++iRsmpBand;
				assert( iMaskBand == iRsmpBand || (iMaskBand+1) == iRsmpBand );
				if ( iRsmpBand == iMaskBand )
				{	// just a constant MaskQ for the whole NoiseBand
					fflt = ffltMaskPower10( rgiMaskQ[iMaskBand] );
					flt = FLOAT_FROM_FASTFLOAT( fflt );
					fltBandWeight[cNoiseBand] = flt*flt;
					MONITOR_COUNT(gMC_GBW_floats,4);
				}
				else
				{	// Two different MaskQ's for this NoiseBand
					// BW = ( N1 * 10^(2*MarkQ[iBarkResampled]) + N2 * 10^(2*MarkQ[iBarkResampled+1]/16) ) / (N1 + N2)
					// This is not suited to FastFloat - leave in float.
					Float fltP1, fltP2;
					Int iUnRsmpEnd = iUnResampleIndex( iMaskEnd, MRI );
					fflt  = ffltMaskPower10( rgiMaskQ[iMaskBand] );
					fltP1 = FLOAT_FROM_FASTFLOAT( fflt );
					fflt  = ffltMaskPower10( rgiMaskQ[iRsmpBand] );
					fltP2 = FLOAT_FROM_FASTFLOAT( fflt );
					flt   = ( (iUnRsmpEnd-iCurrStart)*fltP1*fltP1 + (iCurrEnd-iUnRsmpEnd)*fltP2*fltP2 ) / (iCurrEnd - iCurrStart);
					fltBandWeight[cNoiseBand] = flt;
					MONITOR_COUNT(gMC_GBW_floats,13);
				}
			}
			cNoiseBand++;
		}
        iCurrBand++;
	}
	for( iCurrBand = 0; iCurrBand < (cNoiseBand-1); iCurrBand++ )
	{	
		if ( fAllBandsSynced )
		{
			fflt = ffltMaskPower10( rgiMaskQ4BandNotCoded[iCurrBand] - rgiMaskQ4BandNotCoded[cNoiseBand-1] );
			flt  = FLOAT_FROM_FASTFLOAT( fflt );
			rgffltSqrtBWRatio[iCurrBand] = fflt;
			//// float SqrtBWRatio = fflt.iFraction*1.0F/(1<<fflt.iFracBits)
		}
		else
		{
			// This calculation is not ideally suited to FastFloat, leave in float for time being
			// But note that limited range of the result probably means it can be done more quickly
			flt = (Float)sqrt( fltBandWeight[iCurrBand] / fltBandWeight[cNoiseBand-1] );
			fflt = FASTFLOAT_FROM_FLOAT( flt );
			rgffltSqrtBWRatio[iCurrBand] = fflt;
			//// float SqrtBWRatio = fflt.iFraction*1.0F/(1<<fflt.iFracBits)
		}
		MONITOR_RANGE(gMR_rgffltSqrtBWRatio,FLOAT_FROM_FASTFLOAT(fflt));
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

	assert (cNoiseBand < pau->m_cValidBarkBand);
	assert (pau->m_iFirstNoiseBand > 0);
	rgbBandNotCoded [0] = cNoiseBand;				// used as cNoiseBand
    FUNCTION_PROFILE_STOP(&fp);
}


#pragma warning (disable:4554)

//*********************************************************************************************************
// Verify CoefRecon MidRate
// Debug code which simulates V4 floating point code.
// If decoded inputs are in error, false verifications can occur.
// include this by defining VERIFY_DECODED_COEFS below or in the project file
//*********************************************************************************************************
//#define VERIFY_DECODED_COEFS
#if defined(_DEBUG) && defined(VERIFY_DECODED_COEFS)
#	pragma COMPILER_MESSAGE(__FILE__ "(837) : Warning - Inverse Quantize MidRate Verification Code Enabled.")
static int fIgnoreMRVerification = 0;		// avoid verify checks - set by hand in debugger only
static Float fltMidRateThreshold = 0.025F;		// change from the debugger
void VerifyCoefReconMR( CAudioObject* pau, PerChannelInfo* ppcinfo, 
					  int iRecon, int iBark, int iBarkResampled, I32 qrand, I32 iCoefRecon, int iCoefQ,
					  const U8* const rgbBandNotCoded, int iNoiseBand, int cNoiseBand, 
					  const Int* const rgiNoisePower, const FastFloat* const rgffltSqrtBWRatio, const int* const m_rgiBarkIndex )
{	// stolen and adapted from V4 
	if ( !fIgnoreMRVerification  )
	{
		Float fltiCoefRecon = (Float)iCoefRecon/(1<<TRANSFORM_FRACT_BITS);
		Float fltNoise = ((Float) qrand / (Float) 0x20000000);
		Float fltWeightRatio = (float)pow(10,(ppcinfo->m_rgiMaskQ[iBarkResampled] - ppcinfo->m_iMaxMaskQ)/16.0);
		double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);
		Float dif, rel, fltCoefRecon;
		Float fltNoisePower;
		double dblWeightedQuant;
		if (!pau->m_fNoiseSub) {
			if (iRecon < pau->m_cLowCutOff  || iRecon >= pau->m_cHighCutOff) 
				fltCoefRecon = 0;
			else
    			fltCoefRecon = (Float)( (Float)iCoefQ  * ( dblWeightedQuant = fltWeightRatio * dblQuantStep ) );
		}
		else
		{
			if (iRecon < pau->m_cLowCutOff)  
			{
				fltNoise *= pau->m_fltDitherLevel;
				fltCoefRecon = (Float)(fltNoise * (dblWeightedQuant = fltWeightRatio * dblQuantStep ) );
			}
			else if (iRecon >= pau->m_iFirstNoiseIndex && iBark < NUM_BARK_BAND && rgbBandNotCoded [iBark] == 1 )
			{
				fltNoisePower = ((Float) pow (10, rgiNoisePower [iNoiseBand] / 20.0F)) * 
									FloatFromFastFloat( rgffltSqrtBWRatio[iNoiseBand] );
				fltCoefRecon = fltNoise * (Float)(dblWeightedQuant = fltNoisePower * fltWeightRatio);
			}
			else if ( iRecon >= pau->m_cHighCutOff )  
			{
				fltNoise *= pau->m_fltDitherLevel;
				fltCoefRecon = (Float)(fltNoise * (dblWeightedQuant = fltWeightRatio * dblQuantStep) );
			}
			else 
			{
				fltNoise *= pau->m_fltDitherLevel;
				fltCoefRecon = (Float)((iCoefQ + fltNoise) * (dblWeightedQuant = fltWeightRatio * dblQuantStep) );
			}
		}
		dif = (float) fabs(fltCoefRecon - fltiCoefRecon);
		rel = (float) fabs( (fabs(fltCoefRecon)>32) ? (dif/fltCoefRecon) : (dif/32) );
		if (rel > ((dblWeightedQuant>131072.0) ? 10.0f*fltMidRateThreshold : fltMidRateThreshold) )
			DEBUG_BREAK();
		else 
		{	// when magnitude of integer version is much larger than float, we sometimes see differences
			// expect some false triggers, but often there is a relevant difference
			dif = (float)( fabs(fltiCoefRecon) - fabs(fltCoefRecon) );
			rel = (float) fabs( (fabs(fltCoefRecon)>32) ? (dif/fltCoefRecon) : (dif/32) );
			if ( dif > 75.0f && rel > 0.000125f )		// verify with all1_22s_22
				DEBUG_BREAK();
			else if ( fabs(fltCoefRecon) > (2.0F*(1<<30)) )
				DEBUG_BREAK();	// Overflow that should have been caught above
		}
	}
	MONITOR_RANGE(gMR_CoefRecon,iCoefRecon/((Float)(1<<TRANSFORM_FRACT_BITS)));
	MONITOR_RANGE(gMR_WeightRatio, (float)pow(10,(ppcinfo->m_rgiMaskQ[iBarkResampled] - ppcinfo->m_iMaxMaskQ)/16.0));
	MONITOR_COUNT_CONDITIONAL(iCoefRecon==0,gMC_zeroCoefRecon,1);
#	if defined(PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST)
		if ( PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST <= iInvQuantFrameCount && iInvQuantFrameCount <= PRINT_INVERSE_QUANTIZE_AT_FRAME_LAST )
			MyOutputDbgStr(0,"%5d  %2d  %1d  %4d  %+16.6f\n", 
				iInvQuantFrameCount, iInvQuantSubFrameCount, ppcinfo-pau->m_rgpcinfo, iRecon, iCoefRecon/((Float)(1<<TRANSFORM_FRACT_BITS)) );
#	endif
}
#define VERIFY_COEF_RECON_MR(idx,qRand,cNBand,iBarkRsmp)																			\
	 VerifyCoefReconMR(	pau, ppcinfo, iRecon, iBark, iBarkRsmp, qRand, rgiCoefRecon[iRecon], rgiCoefQ[iCoefQ],						\
						rgbBandNotCoded, iNoiseBand, cNBand, rgiNoisePower, ppcinfo->m_rgffltSqrtBWRatio, pau->m_rgiBarkIndex )
#elif defined(_DEBUG) && defined(WMA_MONITOR)
#define VERIFY_COEF_RECON_MR(idx,qRand,cNBand,iBarkRsmp)																			\
	MONITOR_RANGE(gMR_CoefRecon,FLOAT_FROM_COEF(rgiCoefRecon[iRecon]));											\
	MONITOR_RANGE(gMR_WeightRatio, (float)pow(10,(ppcinfo->m_rgiMaskQ[iBarkRsmp] - ppcinfo->m_iMaxMaskQ)/16.0F));					\
	MONITOR_COUNT_CONDITIONAL(rgiCoefRecon[iRecon]==0,gMC_zeroCoefRecon,1);
#elif defined(PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST)
#define VERIFY_COEF_RECON_MR(idx,qRand,cNBand,iBarkRsmp)                                                                                \
	if ( PRINT_INVERSE_QUANTIZE_AT_FRAME_FIRST <= iInvQuantFrameCount && iInvQuantFrameCount <= PRINT_INVERSE_QUANTIZE_AT_FRAME_LAST )  \
		MyOutputDbgStr(0,"%5d  %2d  %1d  %4d  %+16.6f\n",                                                                                         \
			iInvQuantFrameCount, iInvQuantSubFrameCount, ppcinfo-pau->m_rgpcinfo, iRecon, FLOAT_FROM_COEF(rgiCoefRecon[iRecon]) )
#else	// so not ( defined(_DEBUG) && defined(VERIFY_DECODED_COEFS) )
#define VERIFY_COEF_RECON_MR(idx,qRand,cNBand,iBarkRsmp)
#endif	// defined(_DEBUG) && defined(VERIFY_DECODED_COEFS)


// **********************************************************************
// Macros for DecodeCoefsMidRate with combined INTEGER and INT_FLOAT code

#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)

#define MASK_X_QUANT(iLevel,ffltQuantizer) MULT_HI(((iLevel)<<16),ffltQuantizer.iFraction)
#define RAND_X_QUANT(rnd,ffltQuantizer) MULT_HI((rnd),ffltQuantizer.iFraction)
#define UNNORMED_MULT(fflt,c,shift) \
	fflt.iFraction = MULT_HI( fflt.iFraction, c );	\
	fflt.iFracBits += shift;

// SCALE_COEF_RECON shifts CoefRecon to give it TRANSFORM_FRACT_BITS==5 fractional bits
#if defined(PLATFORM_OPTIMIZE_MINIMIZE_BRANCHING)
	// This SCALE_COEF_RECON macro requires 6 ops and no branches
	// This SETUP_FOR_SCALE_COEF_RECON requires 5 ops plus 1 branch.
	// SCALE_COEFFICENT gets executed 25x as often as SETUP_FOR_SCALE_COEF_RECON, 
	// so this method requires 6.2 ops plus 0.04 branches per SCALE_COEFFICENT
#	define SCALE_COEF_RECON(iCR) ((((iCR)>>-iShift) & iMask2) | (((iCR)<<iShift) & iMask1))
//#	define SETUP_FOR_SCALE_COEF_RECON(iFB) iShift = iFB-TRANSFORM_FRACT_BITS;  \
//								      iMask2 = 0xFFFFFFFF ^ (iMask1 = (iShift>=0) ? 0xFFFFFFFF : 0)
// See comment below
#	define SETUP_FOR_SCALE_COEF_RECON(fftQ) iShift = fftQ.iFracBits-TRANSFORM_FRACT_BITS;  \
								      iMask2 = 0xFFFFFFFF ^ (iMask1 = (iShift>=0) ? 0xFFFFFFFF : 0)

#else
	// When branching is not a high penaty activity, do it the simplier way
	//   iCoefRecon = (t=iFracBits-5)<0 ? iCoefRecon>>-t : iCoefRecon<<t
	// This SCALE_COEF_RECON requires 3 ops plus 1 branch or 2 ops plus 1 branch.  
	// This SETUP_FOR_SCALE_COEF_RECON requires 2 ops
	// SCALE_COEFFICENT gets executed 25x as often as SETUP_FOR_SCALE_COEF_RECON, 
	// so this method requires 2.58 ops plus 0.04 branches per SCALE_COEFFICENT
	// On one test on a 500 MHz Pentium 686, this way saves 1% execution time over masking.
#	define SCALE_COEF_RECON(iCR) (iShift<0) ? (iCR)<<-iShift : (iCR)>>iShift
//#	define SETUP_FOR_SCALE_COEF_RECON(iFB) iShift=iFB-TRANSFORM_FRACT_BITS
// This more complex setup (with pre-normalization) is required to deal with 56_WMAv2.wma which ends
// with 1.5 seconds of DC bias at -890.  This results in a single large coef at 0 and the rest 0.
// Then in the noise band, iShift tries to be == 33...
#	define SETUP_FOR_SCALE_COEF_RECON(fftQ) \
    while( fftQ.iFracBits > (30+TRANSFORM_FRACT_BITS) ) { fftQ.iFracBits--; fftQ.iFraction>>=1; } \
    iShift=fftQ.iFracBits-TRANSFORM_FRACT_BITS;
#endif

#define COEF_PLUS_NOISE_FRAC_BITS 22
#define DITHER_FRAC_BITS 35
#define RAND_FRAC_BITS 29
#define MORE_WF_FRAC_BITS 0

const I32 cDitherFactorMR = 0x28F5C28F;		// BARK 0.02 * 2^35 (but I am not sure how to derive the 35)
#define RAND_TO_NOISE(qr) qr

// Inverse Quantize for "normal" case of CoefQ and Noise Dithering
static INLINE void CoefPlusNoiseInvQuant( const Int iRecon, const Int iCoefQ, I32 qrand, 
										  const I16* rgiCoefQ, I32* rgiCoefRecon, FastFloat ffltQuantizer, Int iShift )
{
	Int iNoise, iCoefPlusNoise, iCoefRecon;
	
	// Multiply by Dither and align iNoise fractional bits to be COEF_PLUS_NOISE_FRAC_BITS == 22
	iNoise = MULT_HI(cDitherFactorMR,qrand)>>((DITHER_FRAC_BITS+RAND_FRAC_BITS-31)-COEF_PLUS_NOISE_FRAC_BITS);
	//// Float Noise = iNoise/(1.0F*(1<<22))
	if ( abs(rgiCoefQ[iCoefQ]) >= (1<<(31-COEF_PLUS_NOISE_FRAC_BITS))  )
	{	// rare, but it does happen occasionally (e.g. tough_32m_32)
		Int iFB = 0;
		UInt uiCoefQ = abs(rgiCoefQ[iCoefQ]);
		while( uiCoefQ >= (1<<(31-COEF_PLUS_NOISE_FRAC_BITS)) )
		{
			uiCoefQ >>= 1;
			iFB++;
		}
		iCoefPlusNoise = (rgiCoefQ[iCoefQ]<<(COEF_PLUS_NOISE_FRAC_BITS-iFB)) + (iNoise>>iFB);
		//// Float Coef+Noise = iCoefPlusNoise/(1.0F*(1<<(22-iFB)))
		iCoefRecon = MULT_HI( iCoefPlusNoise, ffltQuantizer.iFraction );
		//// Float qrand = qrand/(1024.0F*(1<<25))
#if defined(BUILD_INTEGER) || defined(INTEGER_ENCODER)
        ffltQuantizer.iFracBits -= iFB;
		SETUP_FOR_SCALE_COEF_RECON(ffltQuantizer);
        ffltQuantizer.iFracBits += iFB;
#endif
		rgiCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
		//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
		SETUP_FOR_SCALE_COEF_RECON(ffltQuantizer);
	}
	else
	{
		iCoefPlusNoise = (rgiCoefQ[iCoefQ]<<COEF_PLUS_NOISE_FRAC_BITS) + iNoise;	
		//// Float Coef+Noise = iCoefPlusNoise/(1.0F*(1<<22))
		iCoefRecon = MULT_HI( iCoefPlusNoise, ffltQuantizer.iFraction );
		//// Float qrand = qrand/(1024.0F*(1<<25))
		rgiCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
		//// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
	}
}

#else   // so must be BUILD_INT_FLOAT

#define MASK_X_QUANT(iLevel,ffltQuantizer) ((iLevel)*(ffltQuantizer))
#define RAND_X_QUANT(rnd,ffltQuantizer) ((rnd)*(ffltQuantizer))
#define UNNORMED_MULT(flt,c,shift) flt *= c

#define SCALE_COEF_RECON(iCR) (iCR)
#define SETUP_FOR_SCALE_COEF_RECON(ffltQ)

const Float cDitherFactorMR = 0.02F;		            // BARK_MODE: 0.02
const Float cfltInverseRand2Float = 1.0f/0x20000000;		// 2^-29

#define RAND_TO_NOISE(qr) (qr * cfltInverseRand2Float)

// Inverse Quantize for "normal" case of CoefQ and Noise Dithering
#ifdef HITACHI
#pragma inline(CoefPlusNoiseInvQuant)
static  void CoefPlusNoiseInvQuant( const Int iRecon, const Int iCoefQ, I32 qrand, 
										  const I16* rgiCoefQ, CoefType* rgiCoefRecon, QuantFloat qfltQuantizer, Int iShift )
#else
static INLINE void CoefPlusNoiseInvQuant( const Int iRecon, const Int iCoefQ, I32 qrand, 
										  const I16* rgiCoefQ, CoefType* rgiCoefRecon, QuantFloat qfltQuantizer, Int iShift )
#endif
{
	Float fltNoise =  qrand * (cDitherFactorMR * cfltInverseRand2Float);
	rgiCoefRecon [iRecon] = (rgiCoefQ[iCoefQ] + fltNoise) * qfltQuantizer;
}

#endif


WMARESULT prvInitNoiseSub (CAudioObject* pau)
{
    Int iWin, iBand;
    Float fltSamplingPeriod;

    Int *piBarkIndex;
    Int cFrameSample;
    Int iNoiseIndex;

    pau->m_fNoiseSub = WMAB_TRUE;
    pau->m_fltFirstNoiseFreq = (Float)(pau->m_iSamplingRate*0.5f);
    // adjust...
    // HongCho: This is related to Bark bands (re-adjust when Bark bands change)
    if(pau->m_iVersion == 1) {

        // version 1 was incorrectly using the inequalities...
        // do not change!!!

        if(pau->m_iSamplingRate == 22050) {
            // somewhat different parameters...
            if (pau->m_fltWeightedBitsPerSample >= 1.16f)
                pau->m_fNoiseSub = WMAB_FALSE;
            else if(pau->m_fltWeightedBitsPerSample >= 0.72f)
                pau->m_fltFirstNoiseFreq *= (Float)0.70;
            else
                pau->m_fltFirstNoiseFreq *= (Float)0.60;
        }
        else if (pau->m_iSamplingRate == 44100) {
            if (pau->m_fltWeightedBitsPerSample >= 0.61f)
                pau->m_fNoiseSub = WMAB_FALSE;
            else
                pau->m_fltFirstNoiseFreq *= (Float) 0.4;
        }
        else if (pau->m_iSamplingRate == 16000) {
            if (pau->m_fltBitsPerSample <= 0.5f)
                pau->m_fltFirstNoiseFreq *= (Float) 0.30;
            else {
                pau->m_fltFirstNoiseFreq *= (Float) 0.50;
                pau->m_iNoisePeakIgnoreBand = 3;
            }
        }
        else if (pau->m_iSamplingRate == 11025) {
            assert(1 == pau->m_iNoisePeakIgnoreBand);
            pau->m_fltFirstNoiseFreq *= (Float) 0.70;
            if (pau->m_fltBitsPerSample >= 0.9f)
                pau->m_iNoisePeakIgnoreBand = 3;
        }
        else if (pau->m_iSamplingRate == 8000) {
            assert(1 == pau->m_iNoisePeakIgnoreBand);
            if (pau->m_fltBitsPerSample <=0.625f)
                pau->m_fltFirstNoiseFreq *= (Float) 0.50;
            else if (pau->m_fltBitsPerSample <= 0.75f)
                pau->m_fltFirstNoiseFreq *= (Float) 0.65;
            else
                pau->m_fNoiseSub = WMAB_FALSE;
        }
        else {
            if(pau->m_fltBitsPerSample >= 0.8f)
                pau->m_fltFirstNoiseFreq *= (Float)0.75;
            else if(pau->m_fltBitsPerSample >= 0.6f)
                pau->m_fltFirstNoiseFreq *= (Float)0.60;
            else
                pau->m_fltFirstNoiseFreq *= (Float)0.5;
        }

    }
    else {

        // for newer versions...  more correct using inequality...

        if (pau->m_iSamplingRate >= 44100) {
            if (pau->m_fltWeightedBitsPerSample >= 0.61f)
                pau->m_fNoiseSub = WMAB_FALSE;
            else
                pau->m_fltFirstNoiseFreq *= (Float) 0.4;
        }
        else if(pau->m_iSamplingRate >= 22050) {
            // somewhat different parameters...
            if (pau->m_fltWeightedBitsPerSample >= 1.16f)
                pau->m_fNoiseSub = WMAB_FALSE;
            else if(pau->m_fltWeightedBitsPerSample >= 0.72f)
                pau->m_fltFirstNoiseFreq *= (Float)0.70;
            else
                pau->m_fltFirstNoiseFreq *= (Float)0.60;
        }
        else if (pau->m_iSamplingRate >= 16000) {
            if (pau->m_fltBitsPerSample <= 0.5f)
                pau->m_fltFirstNoiseFreq *= (Float) 0.30;
            else {
                pau->m_fltFirstNoiseFreq *= (Float) 0.50;
                pau->m_iNoisePeakIgnoreBand = 3;
            }
        }
        else if (pau->m_iSamplingRate >= 11025) {
            assert(1 == pau->m_iNoisePeakIgnoreBand);
            pau->m_fltFirstNoiseFreq *= (Float) 0.70;
            if (pau->m_fltBitsPerSample >= 0.9f)
                pau->m_iNoisePeakIgnoreBand = 3;
        }
        else if (pau->m_iSamplingRate >= 8000) {
            assert(1 == pau->m_iNoisePeakIgnoreBand);
            if (pau->m_fltBitsPerSample <=0.625f)
                pau->m_fltFirstNoiseFreq *= (Float) 0.50;
            else if (pau->m_fltBitsPerSample <= 0.75f)
                pau->m_fltFirstNoiseFreq *= (Float) 0.65;
            else
                pau->m_fNoiseSub = WMAB_FALSE;
        }
        else {
            if(pau->m_fltBitsPerSample >= 0.8f)
                pau->m_fltFirstNoiseFreq *= (Float)0.75;
            else if(pau->m_fltBitsPerSample >= 0.6f)
                pau->m_fltFirstNoiseFreq *= (Float)0.60;
            else
                pau->m_fltFirstNoiseFreq *= (Float)0.5;
        }
    }

	if (!pau->m_fNoiseSub)
        return WMA_OK;

    pau->m_rgiFirstNoiseBand = (Int*) auMalloc (sizeof (Int) * pau->m_cPossibleWinSize);
    if(pau->m_rgiFirstNoiseBand == NULL)
        return TraceResult(WMA_E_OUTOFMEMORY);
    pau->m_rgbBandNotCoded	= (U8*) auMalloc (pau->m_cValidBarkBand * pau->m_cChannel);	
    if (pau->m_rgbBandNotCoded == NULL)
        return TraceResult(WMA_E_OUTOFMEMORY);
    pau->m_rgiNoisePower	= (Int*) auMalloc (sizeof (Int) * pau->m_cValidBarkBand * pau->m_cChannel);
    if (pau->m_rgiNoisePower == NULL)
        return TraceResult(WMA_E_OUTOFMEMORY);
#ifndef BUILD_INTEGER
    pau->m_rgfltBandWeight	= (Float*) auMalloc (sizeof (Float) * pau->m_cValidBarkBand * pau->m_cChannel);
    if (pau->m_rgfltBandWeight == NULL)
        return TraceResult(WMA_E_OUTOFMEMORY);
#endif
    pau->m_rgffltSqrtBWRatio	= (FastFloat*) auMalloc (sizeof (FastFloat) * pau->m_cValidBarkBand * pau->m_cChannel);
    if (pau->m_rgffltSqrtBWRatio == NULL)
        return TraceResult(WMA_E_OUTOFMEMORY);

    //calculate index of each bark freq
    fltSamplingPeriod = 1.0F / pau->m_iSamplingRate;
    // wchen: we need to think what to do with the cut off frequencies: not include at all or include zeros.
    //for long window
    piBarkIndex = pau->m_rgiBarkIndexOrig;

    // for the v1 compatibility
    if(pau->m_iVersion == 1)
    {
        // precalculate the first noise bands
        // wchen: who put these two lines here? No use!
        //pau->m_rgiFirstNoiseBand[0] = (Int)(pau->m_fltFirstNoiseFreq*pau->m_cFrameSample*fltSamplingPeriod + 0.5);
        //if(pau->m_rgiFirstNoiseBand[0] > pau->m_cSubband) pau->m_rgiFirstNoiseBand[0] = pau->m_cSubband;
        for(iBand = 0; iBand < pau->m_rgcValidBarkBand[0]; iBand++) {
            if(g_rgiBarkFreq[iBand] > (Int)ftoi(pau->m_fltFirstNoiseFreq)) {
                pau->m_rgiFirstNoiseBand[0] = iBand;
                break;
            }
        }
        // if not there, turn it off...
        if(iBand == pau->m_rgcValidBarkBand[0])
            pau->m_fNoiseSub = WMAB_FALSE;
    }
    else
    {
        for (iWin = 0; iWin < pau->m_cPossibleWinSize; iWin++)    {
            // precalculate the first noise bands
            pau->m_rgiFirstNoiseBand[iWin] = pau->m_rgcValidBarkBand[iWin] - 1;// init to max...
            cFrameSample = pau->m_cFrameSample / (1 << iWin);
            iNoiseIndex = (Int)ftoi(pau->m_fltFirstNoiseFreq*cFrameSample*fltSamplingPeriod + 0.5f);
            for(iBand = 1; iBand < pau->m_rgcValidBarkBand[iWin]; iBand++) {
                if(piBarkIndex[iBand] > iNoiseIndex) {
                    pau->m_rgiFirstNoiseBand[iWin] = iBand - 1;
                    break;
                }
            }
            piBarkIndex +=  NUM_BARK_BAND + 1;
        }
    }
    pau->m_iFirstNoiseBand = pau->m_rgiFirstNoiseBand[0];    
    return WMA_OK;
}

//#define PRINT_RESAMPLE_WEIGHT_FACTOR 100000000
#	ifdef PRINT_RESAMPLE_WEIGHT_FACTOR
		// this function must be defined in fft.c
		extern void DebugDctPrintCoefs(CAudioObject* pau, int def, int id, float fac, CoefType* pfCoef);
#		define WFR_PRINT(id,pfCoef)  \
		DebugDctPrintCoefs(pau,PRINT_RESAMPLE_WEIGHT_FACTOR,id,1.0f*(1<<WEIGHTFACTOR_FRACT_BITS),(CoefType*)pfCoef)
#   else
#       define WFR_PRINT(id,pfCoef)
#   endif

Void auResampleWeightFactorLPC (CAudioObject* pau, PerChannelInfo* ppcinfo)
{
    Int iRatio;
    Int i, j;

    UInt*  rguiWeightFactor = ppcinfo->m_rguiWeightFactor;
    Int iSizeSrc = pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame - 1];
    Int iSizeDst = pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];
    assert (pau->m_iCurrSubFrame > 0);
#ifndef BUILD_INTEGER
	assert( sizeof(Int) == sizeof(Float) );		// rgfltWegihtFactor == rgiWeightFactor
#endif

    // This should only be called for LPC mode. Bark mode resamples weight factor
    // from the original weight factor array to avoid losing information.
    assert(LPC_MODE == pau->m_iWeightingMode);

    if (iSizeSrc > iSizeDst) {
        //downsample
        iRatio = iSizeSrc / iSizeDst;
        i = 0;
        while (i < pau->m_cSubband) {
            rguiWeightFactor [i] = rguiWeightFactor [i * iRatio];
            i++;
        }
    }
    else if (iSizeSrc < iSizeDst) {
        //upsample
        iRatio = iSizeDst / iSizeSrc;
        i = pau->m_cSubband / iRatio - 1;
        while (i >= 0) {
            for (j = 0; j < iRatio; j++)    {
                rguiWeightFactor [i * iRatio + j] = rguiWeightFactor [i];
            }
            i--;
        }                
    }
    //if == don't need to do anything
	WFR_PRINT(iSizeSrc > iSizeDst ? 5 :(iSizeSrc < iSizeDst ? 6 : 7),rguiWeightFactor);
}


#ifdef ENCODER
Void auResampleWeightFactorBark (CAudioObject* pau, PerChannelInfo* ppcinfo)
{
    const Int *rgiBarkIndexLastUpdate = pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) *
        LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate);
    const Int cValidBarkBandLastUpdate = pau->m_rgcValidBarkBand [LOG2 (pau->m_cFrameSampleHalf /
        pau->m_iSubFrameSizeWithUpdate)];

    const Int c_iMaskResampleFB = 12;
    const Int c_iRoundUp = (1 << c_iMaskResampleFB) - 1;
    const Int iMaskResampleRatio = (pau->m_iSubFrameSizeWithUpdate << 12) /                           
                          pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];
    const Int iIndexResampleRatio =
        (pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame] << 12) /
        pau->m_iSubFrameSizeWithUpdate;

    const Int cSubbandActual  = ppcinfo->m_cSubbandActual;

    U32*    rguiWeightFactor = ppcinfo->m_rguiWeightFactor;
    U32*    rguiWeightFactorPerBark = (U32 *) ppcinfo->m_rgfltWeightFactorPerBark;
    Int     i;
    Int     iNextBoundary;
    Int     iCurrBark;

    // Some assumptions made above
    assert(sizeof(U32) == sizeof(ppcinfo->m_rgfltWeightFactorPerBark[0]));
    assert(sizeof(U32) == sizeof(ppcinfo->m_rguiWeightFactor[0]));

    // This should only be called for bark mode
    assert(BARK_MODE == pau->m_iWeightingMode);

    iCurrBark = 0;
    i = 0;
    do
    {
        const Int   c_iResampledIdx = ((i * iMaskResampleRatio) >> c_iMaskResampleFB);
        U32         uiWeightFactor;

        // Advance bark index until we find one that maps past current i
        while (c_iResampledIdx >= rgiBarkIndexLastUpdate[iCurrBark])
            iCurrBark++;

        // iCurrBark currently points to the NEXT bark band
        assert(iCurrBark <= cValidBarkBandLastUpdate);
        assert(rgiBarkIndexLastUpdate[iCurrBark] >= 0 &&
            rgiBarkIndexLastUpdate[iCurrBark] <= 2048);
        iNextBoundary = ((rgiBarkIndexLastUpdate[iCurrBark] * iIndexResampleRatio) +
            c_iRoundUp) >> c_iMaskResampleFB;

        if (iNextBoundary > cSubbandActual)
            iNextBoundary = cSubbandActual; // Any work done past cSubbandActual is wasted

        // Make iCurrBark point to current bark band again
        iCurrBark -= 1;

        // Propagate new weight factor. We never touch rgfltWeightFactorPerBark
        // and so using it is equivalent to resampling from last updated weight factors.
        uiWeightFactor = rguiWeightFactorPerBark[iCurrBark];

        assert(iNextBoundary > i);
        assert(iNextBoundary <= cSubbandActual);
        for ( ; i < iNextBoundary; i++)
            rguiWeightFactor[i] = uiWeightFactor;

    } while (i < cSubbandActual);

#ifdef _DEBUG
    memset(&rguiWeightFactor[cSubbandActual], 0xCD, (pau->m_cSubband - cSubbandActual) *
        sizeof(rguiWeightFactor[0]));
#endif  // _DEBUG

}

#endif  // ENCODER


//**************************************************************************************************
//
// prvInverseQuantizeMidRate handles only MidRate inverse quantization
//
// rgiWeightFactor is not used.
//
//**************************************************************************************************
WMARESULT prvInverseQuantizeMidRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor)
{
	Int iBark = 0;					// index barks using natural scale for this cSubband (at this sampling frequency)
	Int iBarkResampled = 0;			// index barks using resampled scale from cSubbands when the MaskQ's were last updated
	Int iCoefQ = 0;
	Int iRecon = 0;
    const I16* rgiCoefQ				= ppcinfo->m_rgiCoefQ;
    const U8*  rgbBandNotCoded		= ppcinfo->m_rgbBandNotCoded;
    const Int* rgiNoisePower		= ppcinfo->m_rgiNoisePower;
	const FastFloat* rgffltSqrtBWRatio	= ppcinfo->m_rgffltSqrtBWRatio;
	const Int* rgiMaskQ				= ppcinfo->m_rgiMaskQ;
	const Int  iMaxMaskQ			= ppcinfo->m_iMaxMaskQ;
    const Int *rgiBarkIndex;
	const Int *rgiBarkIndexResampled;
    CoefType* rgiCoefRecon			= (CoefType*)ppcinfo->m_rgiCoefRecon;
	CoefType iCoefRecon; 
    Int qrand, iReconTarget;
    U8  cNoiseBand = rgbBandNotCoded [0];
    Int iNoiseBand = 0;
    QuantFloat qfltQuantizer;
	MaskResampleInfo MRI;
#if defined(PLATFORM_OPTIMIZATION_MINIMIZE_BRANCHING) && defined(BUILD_INTEGER)
    Int iMask1, iMask2;
#endif
    Int iShift = 0;
#if defined (SMOOTH_NMR)
    Bool *rgfMaskNeededForBark = ppcinfo->m_rgfMaskNeededForBark;
	// A mask value of a bark should be preserved if that bark:
	// (1) is needed for mask resampling,
	// (2) not coded (noise substituted),
	// (3) has a coded coeff with non-zero value,
	// (4) or has the maximum mask value used in normalization.
#endif // SMOOTH_NMR
    
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,INVERSE_QUAN_MID_RATE_PROFILE);

	DEBUG_BREAK_AT_FRAME_INV_QUANT;
	MONITOR_COUNT(gMC_IQ,1);
    assert (pau->m_fNoiseSub &&  pau->m_iWeightingMode == BARK_MODE);
    assert(TRANSFORM_FRACT_BITS==5);
    assert (pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame] != 0);         
    // assert (pau->m_cFrameSampleHalf <= (1<<MASKQ_RESAMPLE_OFFSET));       // why???                                  
    assert (pau->m_iSubFrameSizeWithUpdate != 0);                                           
	assert( pau->m_cPossibleWinSize <= MASKQ_RESAMPLE_OFFSET );
    
#if defined (SMOOTH_NMR)
    // Initially, no bark-mask needs to be preserved.
    memset(rgfMaskNeededForBark, 0, 25 * sizeof(Bool));
#endif //  SMOOTH_NMR
    
	// handle changing subFrame window sizes by resampling the indexes
    MRI.iMaskResampleRatio = (pau->m_iSubFrameSizeWithUpdate << MASKQ_RESAMPLE_OFFSET) /                           
                          pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];    
    
	MRI.iMaskResampleRatioPow = LOG2(MRI.iMaskResampleRatio);
    rgiBarkIndexResampled     = pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) *                    
                                  LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate);   
    MRI.cValidBarkBandLatestUpdate = pau->m_rgcValidBarkBand [LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate)];

	rgiBarkIndex  = pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) * pau->m_iIncr;  

	prvGetBandWeightMidRate (pau, ppcinfo, rgiBarkIndex, rgiBarkIndexResampled, MRI);

    //Scan for the first bark index = note iRecon is 0 and rgiBarkIndex[1] ==0 for 16000 Hz and 11025 Hz frames with 128 or 64 samples
	prvScanForNextBarkIndex( iRecon, &iBarkResampled, pau->m_cLowCutOff, rgiBarkIndexResampled, MRI );
	while( iRecon >= rgiBarkIndex[iBark+1] )
		iBark++;

	if (iRecon < pau->m_cLowCutOff ) {
		// not integerized since cLowCutOff is typically 0, so this is here for compatability with V1 
		Double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);
		Float fltCoefRecon;
		while  (iRecon < pau->m_cLowCutOff) 	{
			Float fltNoise = pau->m_fltDitherLevel * ((Float) quickRand (&(pau->m_tRandState)) / (Float) 0x20000000);//rgfltNoise [iRecon];
			Float fltWeightRatio = (float)pow(10.0f,(rgiMaskQ[iBarkResampled] - iMaxMaskQ)/16.0f);
			fltCoefRecon  = (Float) (fltNoise * fltWeightRatio * dblQuantStep);
			rgiCoefRecon [iRecon] = COEF_FROM_FLOAT(fltCoefRecon);
			MONITOR_COUNT(gMC_IQ_Float,9);
			iRecon++;
		}
	}

    while  (iRecon < pau->m_iFirstNoiseIndex) 
    {
        iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, pau->m_iFirstNoiseIndex, rgiBarkIndexResampled, MRI );
        
        qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
        INTEGER_ONLY( qfltQuantizer.iFracBits += COEF_PLUS_NOISE_FRAC_BITS - 31 );      // Account for MULT_HI in loop below
        //// Float Quantizer = qfltQuantizer.iFraction/(512.0F*(1<<qfltQuantizer.iFracBits)
        SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);              
        
        while ( iRecon < iReconTarget )
        {   
            //  CoefRecon[iR] = (CoefQ[iQ] + rand()*DitherLevel) * (10^(1/16)) ^ (MaskQ[iB]-Max(MaskQ[])) * (10^(1/20)) ^ QuantStepSize
            qrand = quickRand (&(pau->m_tRandState));       
            //// Float qrand = qrand/(1024.0F*(1<<25))
            CoefPlusNoiseInvQuant( iRecon, iCoefQ, qrand, rgiCoefQ, rgiCoefRecon, qfltQuantizer, iShift );
            //// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
            
            VERIFY_COEF_RECON_MR(iRecon,qrand,-1,iBarkResampled);
#if defined (SMOOTH_NMR)
            if (rgiCoefQ[iCoefQ]) rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif // SMOOTH_NMR
            
            iRecon++;
            iCoefQ++;
        }
        if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
            iBarkResampled++;       // normal to increment except when hitting FirstNoiseIndex
        while( iRecon >= rgiBarkIndex[iBark+1] )
            iBark++;
    }
    
    while  (iRecon < pau->m_cHighCutOff) 
    {   // These Bands may or may not be coded, treat as appropriate
        
        if ( (rgbBandNotCoded [iBark] == 1))
        {   
            Int iLoopMax;
            
            
            // CoefRecon[iR] = rand() * ((10^(1/16)) ^ (MaskQ[iB]-Max(MaskQ[])) * sqrt( BandWeight[iN]/BandWeight[cN-1] ) * ((10^(1/20)) ^ NoisePower[iN])
            // Note BandsNotCoded span whole "natural" bark bands, which are not resampled and are not limited by m_cHighCutOff
            FastFloat ffltMaskPower, ffltNoisePower;
            
#if defined (SMOOTH_NMR)
            rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif // SMOOTH_NMR

            assert( iNoiseBand < cNoiseBand );
            
            // auCalcQuantStep( rgiNoisePower[iNoiseBand], &ffltNoisePower.iFraction, &ffltNoisePower.iFracBits );
            ffltNoisePower = FASTFLOAT_FROM_QUANTSTEPTYPE( qstCalcQuantStep( rgiNoisePower[iNoiseBand] ) );
            ffltNoisePower = FASTFLOAT_MULT( ffltNoisePower, rgffltSqrtBWRatio[iNoiseBand] );
            //// Float Noise Power = ffltNoisePower.iFraction/(4.0F*(1<<ffltNoisePower.iFracBits))
            
            iLoopMax = min(rgiBarkIndex[iBark+1], pau->m_cHighCutOff);
            while(iRecon < iLoopMax)
            {
                ffltMaskPower = ffltMaskPower10( rgiMaskQ[iBarkResampled] - iMaxMaskQ );
                //// Float Mask Power = ffltMaskPower.iFraction/(1.0F*(1<<ffltMaskPower.iFracBits))
                
                qfltQuantizer = FASTFLOAT_MULT( ffltMaskPower, ffltNoisePower );
                INTEGER_ONLY( qfltQuantizer.iFracBits += (RAND_FRAC_BITS - 31) );       // Account for MULT_HI in loop below
                //// Float Quantizer = qfltQuantizer.iFraction/(4.0F*(1<<qfltQuantizer.iFracBits))
                SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);
                
                iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, iLoopMax, rgiBarkIndexResampled, MRI );
                while ( iRecon < iReconTarget )
                {   
                    qrand = quickRand (&(pau->m_tRandState));
                    //// Float qrand = qrand/(1024.0F*(1<<25))
                    iCoefRecon = RAND_X_QUANT( RAND_TO_NOISE(qrand), qfltQuantizer );
                    //// Float CoefRecon = iCoefRecon/(1.0F*(1<<qfltQuantizer.iFracBits))
                    rgiCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
                    //// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
                    
                    VERIFY_COEF_RECON_MR(iRecon,qrand,cNoiseBand,iBarkResampled);
                    
                    iRecon++;
                }
                if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
                    iBarkResampled++;       // normal to increment except when hitting end of Bark Band
            }
            iNoiseBand++;
        }
        else 
        {   // This Band is Coded (just like those before FirstNoiseIndex)
            // CoefRecon[iR] = (CoefQ[iQ] + rand()*DitherLevel) * 10^(MaskQ[iB]-Max(MaskQ[]))*2.5*0.5/20 * 10^(QuantStepSize/20)
            iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, pau->m_cHighCutOff, rgiBarkIndexResampled, MRI );
            if ( iReconTarget > rgiBarkIndex [iBark + 1] )
                iReconTarget = rgiBarkIndex [iBark + 1];        
            qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
            //// Float Quantizer = qfltQuantizer.iFraction/(1.0F*(1<<qfltQuantizer.iFracBits))
            // account for MULT_HI in loop below
            INTEGER_ONLY( qfltQuantizer.iFracBits += COEF_PLUS_NOISE_FRAC_BITS - 31 );
            //// Float Quantizer = qfltQuantizer.iFraction/(512.0F*(1<<qfltQuantizer.iFracBits))
            SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);
            
            while ( iRecon < iReconTarget )
            {   
                //  CoefRecon[iR] = (CoefQ[iQ] + rand()*DitherLevel) * (10^(1/16)) ^ (MaskQ[iB]-Max(MaskQ[])) * (10^(1/20)) ^ QuantStepSize
                qrand = quickRand (&(pau->m_tRandState));
                //// Float qrand = qrand/(1024.0F*(1<<25))
                CoefPlusNoiseInvQuant( iRecon, iCoefQ, qrand, rgiCoefQ, rgiCoefRecon, qfltQuantizer, iShift );
                //// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
                
#if defined (SMOOTH_NMR)
                if (rgiCoefQ[iCoefQ]) rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif // SMOOTH_NMR

                VERIFY_COEF_RECON_MR(iRecon,qrand,-1,iBarkResampled);
                
                iRecon++;
                iCoefQ++;
            }
        }
        if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
            iBarkResampled++;       // normal to increment except when hitting HighCutOff
        while( iRecon >= rgiBarkIndex[iBark+1] )
            iBark++;
    }
    
    iReconTarget = pau->m_cSubband;
    if  (iRecon < iReconTarget) 
    {   //  CoefRecon[iR] = rand() * DitherLevel * (10^(1/16)) ^ (MaskQ[iB for HighCutOff-1]-Max(MaskQ[])) * (10^(1/20)) ^ QuantStepSize
        // We may have scaned past m_cHighCutOff doing a BandNotCoded, so search back to find it.
        while ( (((pau->m_cHighCutOff-1) << MRI.iMaskResampleRatioPow) >> MASKQ_RESAMPLE_OFFSET) < rgiBarkIndexResampled [iBarkResampled] ) 
            --iBarkResampled;
        
        qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
        //// Float Quantizer = qfltQuantizer.iFraction/(1.0F*(1<<qfltQuantizer.iFracBits))
        UNNORMED_MULT( qfltQuantizer, cDitherFactorMR, (DITHER_FRAC_BITS - 31) + (RAND_FRAC_BITS - 31) );
        //// Float Quantizer = qfltQuantizer.iFraction/(4.0F*(1<<qfltQuantizer.iFracBits))
        SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);
        
        while ( iRecon < iReconTarget )
        {   
            qrand = quickRand (&(pau->m_tRandState));
            //// Float qrand = qrand/(1024.0F*(1<<25))
            iCoefRecon = RAND_X_QUANT( RAND_TO_NOISE(qrand), qfltQuantizer );
            //// Float CoefRecon = iCoefRecon/(1.0F*(1<<qfltQuantizer.iFracBits))
            rgiCoefRecon [iRecon] = SCALE_COEF_RECON(iCoefRecon);
            //// Float CoefRecon = rgiCoefRecon[iRecon]/32.0F
            
            VERIFY_COEF_RECON_MR(iRecon,qrand,cNoiseBand,iBarkResampled);
            
            iRecon++;
        }
    }

#   if defined(REFERENCE_RAND_24) || defined(REFERENCE_RAND_16)
        // call the random generator one extra time per subframe to improve subband randomness
        quickRand (&(pau->m_tRandState));
#   endif

#   if defined (SMOOTH_NMR)
        // rgfMaskNeededForBark[bark corresponding to HighCutOff-1 should be made true. 
        for (iBark = pau->m_rgcValidBarkBand [pau->m_iIncr]-1; iBark >= 0; iBark--) {
            if ((pau->m_cHighCutOff >= rgiBarkIndex[iBark]) && (pau->m_cHighCutOff < rgiBarkIndex[iBark+1])) {
                rgfMaskNeededForBark[iBark] = WMAB_TRUE;
                break; 
            }
        }
#   endif // SMOOTH_NMR
    
    FUNCTION_PROFILE_STOP(&fp);
	return WMA_OK;
}


#if defined(ENCODER) && defined(SMOOTH_NMR) && !defined(INTEGER_ENCODER) && !defined(BUILD_INTEGER)
WMARESULT prvInverseQuantizeMidRateSkipNoise (CAudioObject* pau, PerChannelInfo* ppcinfo,
                                              Int* rgiWeightFactor, Bool fSkipNoise)
{
	Int iBark = 0;					// index barks using natural scale for this cSubband (at this sampling frequency)
	Int iBarkResampled = 0;			// index barks using resampled scale from cSubbands when the MaskQ's were last updated
	Int iCoefQ = 0;
	Int iRecon = 0;
    const I16* rgiCoefQ				= ppcinfo->m_rgiCoefQ;
    const U8*  rgbBandNotCoded		= ppcinfo->m_rgbBandNotCoded;
    const Int* rgiNoisePower		= ppcinfo->m_rgiNoisePower;
	const FastFloat* rgffltSqrtBWRatio	= ppcinfo->m_rgffltSqrtBWRatio;
	const Int* rgiMaskQ				= ppcinfo->m_rgiMaskQ;
	const Int  iMaxMaskQ			= ppcinfo->m_iMaxMaskQ;
    const Int *rgiBarkIndex;
	const Int *rgiBarkIndexResampled;
    CoefType* rgiCoefRecon			= (CoefType*)ppcinfo->m_rgiCoefRecon;
    Int iReconTarget;
    U8  cNoiseBand = rgbBandNotCoded [0];
    Int iNoiseBand = 0;
    QuantFloat qfltQuantizer;
	MaskResampleInfo MRI;
#if defined(PLATFORM_OPTIMIZATION_MINIMIZE_BRANCHING) && defined(BUILD_INTEGER)
    Int iMask1, iMask2;
#endif
    Int iShift = 0;
#if defined (SMOOTH_NMR)
    Bool *rgfMaskNeededForBark = ppcinfo->m_rgfMaskNeededForBark;
    // A mask value of a bark should be preserved if that bark:
    // (1) is needed for mask resampling,
    // (2) not coded (noise substituted),
    // (3) has a coded coeff with non-zero value,
    // (4) or has the maximum mask value used in normalization.
#endif // SMOOTH_NMR    
    
    FUNCTION_PROFILE(fp);
    FUNCTION_PROFILE_START(&fp,INVERSE_QUAN_MID_RATE_PROFILE);

	DEBUG_BREAK_AT_FRAME_INV_QUANT;
	MONITOR_COUNT(gMC_IQ,1);
    assert (pau->m_fNoiseSub &&  pau->m_iWeightingMode == BARK_MODE);
    assert(TRANSFORM_FRACT_BITS==5);
    assert (pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame] != 0);         
    // assert (pau->m_cFrameSampleHalf <= (1<<MASKQ_RESAMPLE_OFFSET));       // why???                                  
    assert (pau->m_iSubFrameSizeWithUpdate != 0);                                           
	assert( pau->m_cPossibleWinSize <= MASKQ_RESAMPLE_OFFSET );
    
    // Initially, no bark-mask needs to be preserved.
#if defined (SMOOTH_NMR)
    memset(rgfMaskNeededForBark, 0, 25 * sizeof(Bool));
#endif // SMOOTH_NMR
    
	// handle changing subFrame window sizes by resampling the indexes
    MRI.iMaskResampleRatio = (pau->m_iSubFrameSizeWithUpdate << MASKQ_RESAMPLE_OFFSET) /                           
                          pau->m_subfrmconfigCurr.m_rgiSubFrameSize [pau->m_iCurrSubFrame];    
    
	MRI.iMaskResampleRatioPow = LOG2(MRI.iMaskResampleRatio);
    rgiBarkIndexResampled     = pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) *                    
                                  LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate);   
    MRI.cValidBarkBandLatestUpdate = pau->m_rgcValidBarkBand [LOG2 (pau->m_cFrameSampleHalf / pau->m_iSubFrameSizeWithUpdate)];

	rgiBarkIndex  = pau->m_rgiBarkIndexOrig + (NUM_BARK_BAND + 1) * pau->m_iIncr;  

	prvGetBandWeightMidRate (pau, ppcinfo, rgiBarkIndex, rgiBarkIndexResampled, MRI);

    //Scan for the first bark index = note iRecon is 0 and rgiBarkIndex[1] ==0 for 16000 Hz and 11025 Hz frames with 128 or 64 samples
	prvScanForNextBarkIndex( iRecon, &iBarkResampled, pau->m_cLowCutOff, rgiBarkIndexResampled, MRI );
    while( iRecon >= rgiBarkIndex[iBark+1] )
        iBark++;
    
    if (iRecon < pau->m_cLowCutOff ) {
        // not integerized since cLowCutOff is typically 0, so this is here for compatability with V1 
        Double dblQuantStep = DOUBLE_FROM_QUANTSTEPTYPE(pau->m_qstQuantStep);
        Float fltCoefRecon;
        
        while  (iRecon < pau->m_cLowCutOff)     {
            Float fltNoise = pau->m_fltDitherLevel * ((Float) quickRand (&(pau->m_tRandState)) / (Float) 0x20000000);//rgfltNoise [iRecon];
            Float fltWeightRatio = (float)pow(10.0f,(rgiMaskQ[iBarkResampled] - iMaxMaskQ)/16.0f);
            fltCoefRecon  = (Float) (fltNoise * fltWeightRatio * dblQuantStep);
            rgiCoefRecon [iRecon] = COEF_FROM_FLOAT(fltCoefRecon);
            MONITOR_COUNT(gMC_IQ_Float,9);
            iRecon++;
        }
    }
    
    while  (iRecon < pau->m_iFirstNoiseIndex) 
    {
        iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, pau->m_iFirstNoiseIndex, rgiBarkIndexResampled, MRI );
        
        qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
        INTEGER_ONLY( qfltQuantizer.iFracBits += COEF_PLUS_NOISE_FRAC_BITS - 31 );      // Account for MULT_HI in loop below
        //// Float Quantizer = qfltQuantizer.iFraction/(512.0F*(1<<qfltQuantizer.iFracBits)
        SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);              
        
        while ( iRecon < iReconTarget )
        {   
            rgiCoefRecon[iRecon] = (rgiCoefQ[iCoefQ] + ppcinfo->m_rgfltDitherNoise[iRecon]) *
                qfltQuantizer;
            
#if defined (SMOOTH_NMR)
            if (rgiCoefQ[iCoefQ]) rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif // SMOOTH_NMR
            
            iRecon++;
            iCoefQ++;
        }
        if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
            iBarkResampled++;       // normal to increment except when hitting FirstNoiseIndex
        while( iRecon >= rgiBarkIndex[iBark+1] )
            iBark++;
    }
    
    while  (iRecon < pau->m_cHighCutOff) 
    {   // These Bands may or may not be coded, treat as appropriate
        
        if ( (rgbBandNotCoded [iBark] == 1))
        {   
            Int iLoopMax;
            
            iLoopMax = min(rgiBarkIndex[iBark+1], pau->m_cHighCutOff);
            
#if defined (SMOOTH_NMR)
            rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif // SMOOTH_NMR
            
            if (WMAB_FALSE == fSkipNoise)
            {
                // CoefRecon[iR] = rand() * ((10^(1/16)) ^ (MaskQ[iB]-Max(MaskQ[])) * sqrt( BandWeight[iN]/BandWeight[cN-1] ) * ((10^(1/20)) ^ NoisePower[iN])
                // Note BandsNotCoded span whole "natural" bark bands, which are not resampled and are not limited by m_cHighCutOff
                FastFloat ffltMaskPower, ffltNoisePower;
                assert( iNoiseBand < cNoiseBand );
                
                // auCalcQuantStep( rgiNoisePower[iNoiseBand], &ffltNoisePower.iFraction, &ffltNoisePower.iFracBits );
                ffltNoisePower = FASTFLOAT_FROM_QUANTSTEPTYPE( qstCalcQuantStep( rgiNoisePower[iNoiseBand] ) );
                ffltNoisePower = FASTFLOAT_MULT( ffltNoisePower, rgffltSqrtBWRatio[iNoiseBand] );
                //// Float Noise Power = ffltNoisePower.iFraction/(4.0F*(1<<ffltNoisePower.iFracBits))
                
                while(iRecon < iLoopMax)
                {
                    ffltMaskPower = ffltMaskPower10( rgiMaskQ[iBarkResampled] - iMaxMaskQ );
                    //// Float Mask Power = ffltMaskPower.iFraction/(1.0F*(1<<ffltMaskPower.iFracBits))
                    
                    qfltQuantizer = FASTFLOAT_MULT( ffltMaskPower, ffltNoisePower );
                    INTEGER_ONLY( qfltQuantizer.iFracBits += (RAND_FRAC_BITS - 31) );       // Account for MULT_HI in loop below
                    //// Float Quantizer = qfltQuantizer.iFraction/(4.0F*(1<<qfltQuantizer.iFracBits))
                    SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);
                    
                    iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, iLoopMax, rgiBarkIndexResampled, MRI );
                    while ( iRecon < iReconTarget )
                    {   
                        rgiCoefRecon [iRecon] = ppcinfo->m_rgfltDitherNoise[iRecon] * 
                            qfltQuantizer;
                        iRecon++;
                    }
                    if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
                        iBarkResampled++;       // normal to increment except when hitting end of Bark Band
                }
            }
            else
                iRecon = iLoopMax;
            
            iNoiseBand++;
        }
        else 
        {   // This Band is Coded (just like those before FirstNoiseIndex)
            // CoefRecon[iR] = (CoefQ[iQ] + rand()*DitherLevel) * 10^(MaskQ[iB]-Max(MaskQ[]))*2.5*0.5/20 * 10^(QuantStepSize/20)
            iReconTarget = prvScanForNextBarkIndex( iRecon, &iBarkResampled, pau->m_cHighCutOff, rgiBarkIndexResampled, MRI );
            if ( iReconTarget > rgiBarkIndex [iBark + 1] )
                iReconTarget = rgiBarkIndex [iBark + 1];        
            qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
            //// Float Quantizer = qfltQuantizer.iFraction/(1.0F*(1<<qfltQuantizer.iFracBits))
            // account for MULT_HI in loop below
            INTEGER_ONLY( qfltQuantizer.iFracBits += COEF_PLUS_NOISE_FRAC_BITS - 31 );
            //// Float Quantizer = qfltQuantizer.iFraction/(512.0F*(1<<qfltQuantizer.iFracBits))
            SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);
            
            while ( iRecon < iReconTarget )
            {   
                rgiCoefRecon[iRecon] = (rgiCoefQ[iCoefQ] + ppcinfo->m_rgfltDitherNoise[iRecon]) *
                    qfltQuantizer;
                
#if defined (SMOOTH_NMR)
                if (rgiCoefQ[iCoefQ]) rgfMaskNeededForBark[iBark] = WMAB_TRUE;
#endif // SMOOTH_NMR

                iRecon++;
                iCoefQ++;
            }
        }
        if (iResampleIndex (iRecon+1, MRI) >= rgiBarkIndexResampled [iBarkResampled+1]) 
            iBarkResampled++;       // normal to increment except when hitting HighCutOff
        while( iRecon >= rgiBarkIndex[iBark+1] )
            iBark++;
    }
    
    iReconTarget = pau->m_cSubband;
    if  (iRecon < iReconTarget) 
    {   //  CoefRecon[iR] = rand() * DitherLevel * (10^(1/16)) ^ (MaskQ[iB for HighCutOff-1]-Max(MaskQ[])) * (10^(1/20)) ^ QuantStepSize
        // We may have scaned past m_cHighCutOff doing a BandNotCoded, so search back to find it.
        while ( (((pau->m_cHighCutOff-1) << MRI.iMaskResampleRatioPow) >> MASKQ_RESAMPLE_OFFSET) < rgiBarkIndexResampled [iBarkResampled] ) 
            --iBarkResampled;
        
        qfltQuantizer = prvWeightedQuantization( pau, ppcinfo, iBarkResampled );
        //// Float Quantizer = qfltQuantizer.iFraction/(1.0F*(1<<qfltQuantizer.iFracBits))
        UNNORMED_MULT( qfltQuantizer, cDitherFactorMR, (DITHER_FRAC_BITS - 31) + (RAND_FRAC_BITS - 31) );
        //// Float Quantizer = qfltQuantizer.iFraction/(4.0F*(1<<qfltQuantizer.iFracBits))
        SETUP_FOR_SCALE_COEF_RECON(qfltQuantizer);
        
        while ( iRecon < iReconTarget )
        {   
            rgiCoefRecon[iRecon] = qfltQuantizer * ppcinfo->m_rgfltDitherNoise[iRecon];
            iRecon++;
        }
    }
    
#if defined (SMOOTH_NMR)
    // rgfMaskNeededForBark[bark corresponding to HighCutOff-1 should be made true. 
    for (iBark = pau->m_rgcValidBarkBand [pau->m_iIncr]-1; iBark >= 0; iBark--) {
        if ((pau->m_cHighCutOff >= rgiBarkIndex[iBark]) && (pau->m_cHighCutOff < rgiBarkIndex[iBark+1])) {
            rgfMaskNeededForBark[iBark] = WMAB_TRUE;
            break; 
        }
    }
#endif // SMOOTH_NMR
    
    FUNCTION_PROFILE_STOP(&fp);
	return WMA_OK;
} // prvInverseQuantizeMidRateSkipNoise
#endif  // defined(ENCODER) && defined(SMOOTH_NMR) && !defined(INTEGER_ENCODER) && !defined(BUILD_INTEGER)


#pragma warning (default:4554)


#endif
