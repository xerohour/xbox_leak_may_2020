/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
/*******************************************************************************
* Filename:     CalcCost.c
*                                                                              
* Purpose:      calculate likelyhood function for LDPDA.
*                                                                              
* Functions:    CalcCostFunc
*                                                                              
* Author/Date:  Gerard Aguilar 01/97
*               Wei Wang       04/97
********************************************************************************
* Modifications: To reduce the complexity, all float to int conversions are 
*                eliminated by using fix-point indices. And time-consuming 
*                floating point comparisons are speed-up using long integer
*                comparisons by assigning numbers to float-int union (assuming
*                IEEE floating point). 
*
*              1. Use fixed-point code to prevent float-int conversion 
*              2. Use pitch table instead of F0 table 
*              3. Fewer input arrays (lower ROM and RAM). 
*           
*               -- Wei Wang 12/97
*
* Comments:                                                                    
* 
* Concerns:  
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/CalcCost.c_v   1.5   02 Apr 1998 14:46:52   weiwang  $
*******************************************************************************/

/*** system include files ***/
#include <stdlib.h>
#include <assert.h>

/*** Research lib include files ***/
#include "vLibType.h"

/*** codec related include files ***/
#include "codec.h"
#include "CalcCost.h"
#include "trigSC.h"

#include "xvocver.h"

/*** Table ***/
extern const float *cosTBL_32;

/*** local definitions ***/
#define PITCH_SHIFT_BITS           (15-LOG2_TRIG_SIZE)
#define PITCH_SHIFT_SCALE          (float)(1<<PITCH_SHIFT_BITS)
#define SCALED_TABLE_SIZE          ((long)TRIG_SIZE << PITCH_SHIFT_BITS)
#define SCALED_TABLE_SIZE_D_NFFT   (SCALED_TABLE_SIZE >> FFT_ORDER)
#define SCALED_HALF_TBL_SIZE       ((long)SCALED_TABLE_SIZE>>1)
#define SCALED_TABLE_MASK          ((long)SCALED_TABLE_SIZE-1)
#define LDF0_SHIFT_BITS            12
#define LDF0_SCALED_05             ((long)1<<(LDF0_SHIFT_BITS-1))
#define F0_FFT_PITCH_SHIFT_BITS    (LDF0_SHIFT_BITS+FFT_ORDER+PITCH_SHIFT_BITS)

#define CALC_TAB_INDEX(Fk, TP0)  ((long)Fk*(long)TP0)


/******************************************************************************
* Function:  CalcCostFunc()
*
* Action:    Compute the frequency domain pitch likelyhood function.
*
* Input:     fPitch     --  pitch value
*            piPeakFreq --  peak indices of spectrum
*            pfMag      --  compressed magnitude spectrum
*            iNBand     --  length of spectrum for 
*            fEngScale  --  energy scale factor
*
* Output:    none
*
* Globals:   none
*
* Return:    cost function (or likelyhood function) of input pitch
*******************************************************************************
*
* Implementation/Detailed Description:
*
* Reference: Pitch Estimation and Vocing Detection Based on a Sinusoidal
*            Speech Model. R. McAulay and T. Quatieri, ICASSP 1990.
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

float CalcCostFunc( float         fPitch, 
                    const int     *piPeakFreq, 
                    const float   *pfMag, 
                    int           iNBand, 
                    float         fEngScale
                  )
{
  long lPitch, lScaledPitch;
  long lF0, lHarmF0;
  float fCrossCost, fHarmPower;
  float fHarmMag;
  long lRightBound, lTableIndex;
  int iHarmF0;
  int iPeaks;
  int i;
  UnionFltLng uMaxMag, uPeakMag;

  assert(piPeakFreq != NULL);
  assert(pfMag != NULL);

  /*------------------------------------------------------------
    (1) Initializations.
    ------------------------------------------------------------*/
  /* initialize lPitch: shift fPitch by PITCH_SHIFT_BITS */
  lPitch = (long)(fPitch*PITCH_SHIFT_SCALE);

  /* initialize lScaledPitch: scaled lPitch by TRIG_SIZE over NFFT */
  lScaledPitch = (long)(fPitch*SCALED_TABLE_SIZE_D_NFFT); 

  /* initialize lF0: shift F0=NFFT/fPitch by LDF0_SHIFT_BITS */
  assert(F0_FFT_PITCH_SHIFT_BITS < 31);
  lF0 = ((long)1<<F0_FFT_PITCH_SHIFT_BITS)/lPitch;

  /* lHarmF0: iHarm * lF0 */
  lHarmF0 = lF0;
  
  /* initialize the accumulators: 
     fCrossCosts -- sum of cross correlation term.
     fHarmPower  -- sum of harmonics' power */
  fCrossCost = EPS;
  fHarmPower = EPS;

  /*---------------------------------------------------------------
    (2) Skip peak indices which are smaller than 0.5F0.
    ---------------------------------------------------------------*/
  /* lRightBound: mapped from harmonic's boundary to scaled 
     Cosine table index. */
  lRightBound = SCALED_HALF_TBL_SIZE;
  iPeaks = 0;

  /* lTableIndex: mapped to unwarped scaled Cosine table. 
     This index should related to fPitch and peaks in frequency domain. */
  lTableIndex = CALC_TAB_INDEX(piPeakFreq[iPeaks], lScaledPitch);

  /* skip first few peaks ... */
  while (lTableIndex < lRightBound)
    lTableIndex = CALC_TAB_INDEX(piPeakFreq[++iPeaks], lScaledPitch);

  /*------------------------------------------------------------
    (3) Compute fCrossCost and fHarmPower for all harmonics
    within [0.5F0, iNBand].
    ------------------------------------------------------------*/

  /* iHarmF0:  round(N * F0), N refers to number of harmonics */
  iHarmF0 = (int)((lHarmF0+LDF0_SCALED_05) >> LDF0_SHIFT_BITS);

  /* main loop: search all harmonics within [0.5F0, iNBand]. */
  while (iHarmF0 < iNBand) 
    {
      /* lRightBound: mapped from (N+0.5)*F0 to (N+0.5)*SCALED_TABLE_SIZE */
      lRightBound += SCALED_TABLE_SIZE;

      /* Find out the most confident peak within one harmonic band */
      uMaxMag.f = EPS;

      while (lTableIndex < lRightBound)
        {
          /* peak magnitude is scaled by 1+cos[i], where i can be calculated 
           * by lTableIndex. And i refers to how closely the peak to center
           * harmonic frequency. */
          i = (int)((lTableIndex & SCALED_TABLE_MASK) >> PITCH_SHIFT_BITS);
          uPeakMag.f = pfMag[piPeakFreq[iPeaks]] * (1.0F+cosTBL_32[i]);

          if (uPeakMag.l > uMaxMag.l)
             uMaxMag.l = uPeakMag.l;

          lTableIndex = CALC_TAB_INDEX(piPeakFreq[++iPeaks], lScaledPitch);
        }

      /* accumulate fCrossCost and fHarmPower: */
      fHarmMag = pfMag[iHarmF0];
      fCrossCost += uMaxMag.f * fHarmMag;
      fHarmPower += fHarmMag * fHarmMag;

      /* update lHarmF0 and iHarmF0 */
      lHarmF0 += lF0;
      iHarmF0 = (int)((lHarmF0+LDF0_SCALED_05)>>LDF0_SHIFT_BITS);
    }

  /* return the final likelyhood function. */
  return  (fEngScale*(0.5F*fCrossCost - 0.375F*fHarmPower));
} /* CalcCostFunc() */

