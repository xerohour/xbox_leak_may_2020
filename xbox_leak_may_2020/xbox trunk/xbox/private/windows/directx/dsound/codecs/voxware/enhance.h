/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:    enhance.h                                                                  
*                                                                              
* Purpose:     Frequency-domain based perceptual weighting of the 
*		         harmonic amplitudes based upon the spectrum of the LPC
*		         coefficients.                                                                  
*                                                                              
* Author/Date: Rob Zopf 02/06/96                                                                
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/enhance.h_v   1.3   20 Apr 1998 14:41:36   weiwang  $                                                                     
*******************************************************************************/

#ifndef ENHANCE_H
#define ENHANCE_H

#define ORIGINAL		1
#define AGRESSIVE  		2
#define CONSERVATIVE  	3

#define FILTER_TYPE CONSERVATIVE 

#define ENHANCE_FFT           (1<<ENHANCE_FFT_REAL_ORDER)
#define ENHANCE_FFT2          (ENHANCE_FFT>>1)

/* Distance Supression between formants and nulls */
#define SUPPRESSION_FACTOR    0.3/* 0.3F needs to be pitch adaptive [.3-.8] Pfw=0.3 for F0>=200 && Pfw=0.8  F0<=120 && Pfw=0.5  F0<=120 */
#define SUPPRESSION_FACTOR2   0.25/* 0.25F if Pv <=0.1 */

#if FILTER_TYPE == ORIGINAL 
	#define SF_0   0.1F   /*        Pv<=0.1 */
	#define SF_1    .1F   /*  150<F0                P0 <53.33 */
	#define SF_2   .3F    /*  100<F0<=150   53.3 >= P0 <80 */
	#define SF_3   .5F    /*  F0<=100    	 80  >= P0 */
#endif

#if FILTER_TYPE == CONSERVATIVE /*  Conservative  */
		#define SF_0    0.35F   /* 0.7F                 Pv<=0.1 */
		#define SF_1    0.2F    /* 0.4F  150<F0                P0 <53.33 */ 
		#define SF_2    0.35F   /* 0.7F  100<F0<=150   53.3 >= P0 <80 */ 
		#define SF_3    0.5F    /* 0.9F  F0<=100    	 80  >= P0 */
#endif 

#if FILTER_TYPE == AGRESSIVE   /*  Aggresive */
	#define SF_0    0.7F        /*                 Pv<=0.1 */
	#define SF_1    0.4F        /*  150<F0                P0 <53.33 */
	#define SF_2    0.7F        /*  100<F0<=150   53.3 >= P0 <80 */
	#define SF_3    0.9F        /*  F0<=100    	 80  >= P0 */
#endif
#define SF_P1 53.33F
#define SF_P2 80.0F






/*------------------------------------------------------------------------------
  In order to eliminate casting from a float to an int for indexing, the index
  is now a long.  The definitions below are used for quick shifting and 
  normalization of the indexing, eliminating the cast.  This is seen as a 
  significant speed-up, especially on the PC.

  RZ - 01/17/97
  ------------------------------------------------------------------------------*/
#define F_QFACTOR                        (31-(ENHANCE_FFT_REAL_ORDER+1))
#define F_TO_FLOAT                       (1.0F/(((long)1)<<F_QFACTOR))
#define F_FRACTION_MASK                  ((((long)1)<<F_QFACTOR)-1) 

/*------------------------------------------------------------------------------
  During enhancement, it is better to interpolate between fft bins to obtain
  a better estimate of the lpc inverse spectrum at the given harmonic freq.
  Previously, we just selected the nearest fft bin, which meant that a high
  order fft was necessary to maintain a small fft bin width, and minimize
  error from selecting the nearest fft value.  By interpolating, the fft
  size can be reduced, hence reducing both complexity and memory requirements.

  RZ - 01/17/97
  ------------------------------------------------------------------------------*/
#define INTERPOLATE_BETWEEN_FFT_BINS      1

/*------------------------------------------------------------------------------
   A 5th order polynomial can be used to estimate the power function for a 
   significant complexity reduction.  There are 4 different polynomials
   corresponding to a certain range of input.  The BOUNDARY definitions below
   specify the cutoff points for validity of each polynomial.  There are 
   separate sets of polynomials for x^(-0.125) and x^(-0.150).  The polynomial
   estimates are very accurate and the speech quality is identical to using
   the power function.

   RZ - 01/17/97
  ------------------------------------------------------------------------------*/         
#define USE_POWER_ESTIMATE                0
#define POLYORDER                         5
#define BOUNDARY1                         8.0F
#define BOUNDARY2                         50.0F
#define BOUNDARY3                         1000.0F
#define BOUNDARYMAX                       15000.0F

     

/*---------------------------------------------------------------------
  Enhancement Decoder Structure ...
  ---------------------------------------------------------------------*/
typedef struct tagEnhance_mblk
{
   float        *RE;          /* 256 -- only inside enhance_29 */
   float        *IM;          /* 256 -- only inside enhance_29 */
   float        *TT;          /* 128 -- only inside enhance_29 */
   float        *amp;         /* 60  -- only inside enhance_29 */
   short        *freq;        /* 60  -- only inside enhance_29 */
   float        *lpc;
} ENHANCE;


unsigned short VoxEnhance(STACK_R float *lpc, float Pv, float PitchLag, int nHarm, float *HarmAmp);


#endif /* ENHANCE_H */

