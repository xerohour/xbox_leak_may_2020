/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename:      PostFilt.c
*
* Purpose:       Postfilter the log-base-2 warped allpole envelope
*
* Functions:     PostFilter()
*
* Author/Date:   R.J.McAulay/6/17/97
*
*******************************************************************************
*
* Modifications: Extensively re-written by Bob Dunn 4/8/98
*
* Comments:  
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/PostFilt.c_v   1.5   14 Apr 1998 10:14:16   weiwang  $   
*
******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "vLibMath.h"
#include "vLibVec.h"

#include "codec.h"

#include "trigSC.h"
#include "PostFilt.h"

#include "xvocver.h"


/*-----------------------------------------------------------------------
  Set the number of linear segments to use for approximation of the 
   tilt.
-----------------------------------------------------------------------*/
#define LOGNSEGMENTS  2
#define NSEGMENTS     (1<<LOGNSEGMENTS)
#define TILT_MASK     ((1<<(CSENV_FFT_ORDER-LOGNSEGMENTS-1))-1)

/*-----------------------------------------------------------------------
  Compression factor for the postfilter
-----------------------------------------------------------------------*/
#define BETA      0.50F

/*-----------------------------------------------------------------------
  Clipping thresholds for the postfilter.
    fLogWOver  = Log2( 1.20 )
    fLogWUnder = Log2( 0.50 )
-----------------------------------------------------------------------*/
static const float fLogWOver  =  0.26303441F;
static const float fLogWUnder = -1.0F;

/*-----------------------------------------------------------------------
  Set a transition region (0<omega<LO_EDGE*PI) where the amount
    of postfiltering is gradually increased.  The increase is 
    logarithmic with respect to frequency. (i.e. a linear increase
    in the log domain)
-----------------------------------------------------------------------*/
#define LO_EDGE   0.250F
static const int iK0 = (int)((float)CSENV_NFFT_2*LO_EDGE);
static const float fInverseK0 = 1.0F / (float)
                                  ((int)((float)CSENV_NFFT_2*LO_EDGE));



/*-----------------------------------------------------------------------
  Use a table for cosine.
-----------------------------------------------------------------------*/
extern const float *cosTBL_32;
static const int iFund = (1<<(LOG2_TRIG_SIZE-LOGNSEGMENTS-1));

/*******************************************************************************
*
* Function:  PostFilter()
*
* Action:    Use the LPC parameters to compute the spectral tilt and the
*            energy, flatten the allpole envelope and compute the post-filter,
*            apply the post-filter and re-normalize to equalize the energy.
*            The log-base-2 envelope is stored in place of the log-amplitudes.
*
* Input:     float *pfLogEnv  --> log (base 2) magnitude of all-pole envelope
*                                   with length CSENV_NFFT_2
*            float  fLog2Gain --> log (base 2) gain of the all-pole envelope
*            float *pfKS      --> reflection coefficients for all-pole envelope
*                                   with length LPC_ORDER
*
* Output:    float *pfLogEnv  --> postfiltered envelope
*                                   with length CSENV_NFFT_2
*
* Globals:   extern const float *cosTBL_32 --> cosine table
*
* Return:    none
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:   hard-wire coded for envelope length equal to CSENV_NFFT_2
*******************************************************************************/

void PostFilter(float *pfLogEnv, float fLog2Gain, float *pfKS)
{
   int   i, k;
   int   iIndex;
   int   iInterp;
   int   iMax;
   int   iMaxNew;
   int   iArg;
   float fMax;
   float fMaxNew;
   float fLogWeight;
   float fTemp1, fTemp2, fTemp3;
   float fProd;
   float fGain;
   float fInterp;

   float fTilt;
   float fTiltSamples[NSEGMENTS+1];
   
   assert((pfLogEnv != NULL) && (pfKS != NULL));

   /*-------------------------------------------------------------------------
     Check cosine table size...must re-work code if this fails
   -------------------------------------------------------------------------*/
   assert( iFund > 0 );

   /*-------------------------------------------------------------------------
     Find the peak of the spectral log magnitude.  This will be used to 
       normalize the gain of the postfiltered speech.  It is a poor man's 
       estimate of R0.  It is a reasonably good approximation when 
       the tilt is at least (15 dB / 4000 Hz)
   -------------------------------------------------------------------------*/
   FindMaxValueFlt( pfLogEnv, CSENV_NFFT_2, &iMax, &fMax );

   /*-------------------------------------------------------------------------
     A 1st order all pole model of the spectrum is used to normalize out
       the spectral tilt.  The model is:

         Tilt(w) = | G1/(1.0+a_1*exp(-j*w)) |

     where a_1 is the predictor coefficient for a 1st order all pole model,
     G1 is the gain of that model, and w is the frequency in radians.  
     The value of a_1 is determined from:

         a_1 = - k_1 

     where k_1 is the first reflection coefficient.  The value of G1 can 
     found from:

         G1^2 = R0 * (1.0 - k_1^2)

     where R0 is the 0th autocorrelation coefficient.  Finally, R0 is 
     determined from the equation:

         G12^2 = R0 * PROD( i=1...12 ) (1.0 - k_i^2)

     where G12 is the gain of the 12th order all pole model and k_i
     are the reflection coefficients.

     First compute 1st order system gain from the original gain and the 
     reflection coefficients.
   -------------------------------------------------------------------------*/
   fProd = 1.0F;
   for (i=1; i<LPC_ORDER; i++)
   {
      fTemp1 = pfKS[i]*pfKS[i];
      fProd *= (1.0F-fTemp1);
   }
   fGain = fLog2Gain - 0.5F*fLog2(fProd);
 
   /*-------------------------------------------------------------------------
     Compute samples of the log of the tilt curve using:

         Tilt(w) = | G1/(1.0+a_1*exp(-j*w)) |
                 = G1/sqrt(1.0+a_1*a_1-2.0*A_1*cos(w))

         Log2(Tilt(w)) = Log2(G1) - 0.5*Log2(1.0+a_1*a_1-2.0*A_1*cos(w))
   -------------------------------------------------------------------------*/
   fTemp2 = 1.0F + pfKS[0]*pfKS[0];
   fTemp3 = -2.0F*pfKS[0];
   iArg   = 0;
   for (i=0; i<=NSEGMENTS; i++)
   {
      fTemp1           = fTemp2 + fTemp3*cosTBL_32[iArg];
      iArg            += iFund;
      fTiltSamples[i]  = fGain - 0.5F*fLog2(fTemp1);
   }

   /*------------------------------------------------------------------------
     Postfilter using approximation to 1st order all-pole envelope
   ------------------------------------------------------------------------*/
   iIndex  = 0;
   iInterp = 0;
   for (k = 1; k < CSENV_NFFT_2; k++)
   {
      iInterp = k&TILT_MASK;
      iIndex  = (k>>(CSENV_FFT_ORDER-LOGNSEGMENTS-1));
      fInterp = (float) iInterp*((float)NSEGMENTS/(float)CSENV_NFFT_2);
      fTilt = VoxINTERP( fTiltSamples[iIndex], fTiltSamples[iIndex+1], fInterp);

      /*-----------------------------------------------------------------
        Compute postfilter weight
      -----------------------------------------------------------------*/
      fLogWeight = BETA*(pfLogEnv[k] - fTilt);
 
      /*-----------------------------------------------------------------
        Clip the postfilter weight
      -----------------------------------------------------------------*/
      fLogWeight = VoxMIN( fLogWeight, fLogWOver );
      fLogWeight = VoxMAX( fLogWeight, fLogWUnder );
 
      /*-----------------------------------------------------------------
        Fade in the postfilter weight in the region (0<omega<LO_EDGE*PI)
      -----------------------------------------------------------------*/
      if(k < iK0)
         fLogWeight *= (k*fInverseK0);
 
      /*-----------------------------------------------------------------
        Apply the weight
      -----------------------------------------------------------------*/
      pfLogEnv[k] += fLogWeight;
   }

   /*-------------------------------------------------------------------------
     Re-scale the post-filtered log-envelope to achieve the measured energy
   -------------------------------------------------------------------------*/
   FindMaxValueFlt( pfLogEnv, CSENV_NFFT_2, &iMaxNew, &fMaxNew );

   fLogWeight = fMax - fMaxNew;
   for (k = 0; k < CSENV_NFFT_2; k++)
      pfLogEnv[k] += fLogWeight;

} /* PostFilter() */
   
