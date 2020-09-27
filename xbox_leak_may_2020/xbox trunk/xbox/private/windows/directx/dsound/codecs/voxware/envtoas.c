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
* Filename:    EnvToAs.c
*
* Purpose:     Compute predictor coefficients and gain from the log (base 2)
*                of the spine fit to the SeeVoc power peaks.
*
* Functions:
*
* Author/Date: Bob Dunn 12/28/96
*
*******************************************************************************
*
* Modifications:  Removed call to rfft2() and rewritted for readability.
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/EnvToAs.c_v   1.2   11 Apr 1998 15:19:16   bobd  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>
 
#include "vLibMath.h"
#include "vLibVec.h"
#include "vLibTran.h"
#include "vLibSpch.h"

#include "codec.h"
#include "VoxMem.h"
#include "warp.h"

#include "EnvToAs.h"

#include "xvocver.h"

/*-----------------------------------------------------------------
  Constants for downsampling the spectrum to reduce the size of
   the FFT used to compute the correlation coefficients.
-----------------------------------------------------------------*/
#define DWNSMP      2
#define DFFT       (NFFT>>DWNSMP)
#define DFFT_ORDER (FFT_ORDER-DWNSMP)

/*-----------------------------------------------------------------
  White noise scale factor for autocorrelation coefficients.
-----------------------------------------------------------------*/
#define WHITE_NOISE_SCALE 0.999999F

/******************************************************************************
*
* Function:  LogEnvelopeToAs()
*
* Action:    Compute predictor coefficients and gain from the log (base 2)
*              of the spine fit to the SeeVoc power peaks.
*
* Input:     float *pfLogSeeEnv --> spine fit to the SeeVoc power peaks
*            int iOrderAllpole  --> allpole model order
*
* Output:    float *pfLogSeeEnv --> perceptually warped spine fit to the 
*                                     SeeVoc power peaks
*            float *pfGain      --> gain
*            float *pfAS        --> predictor coefficients
*            float *pfKS        --> reflection coefficients
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
*            pfLogSeeEnv[] ranges from 0 to (NFFT_2-1)
*            pfAS[]        ranges from 0 to (iOrderAllpole+1) (pfAS[0] = 1.0F)
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/

void LogEnvelopeToAs( STACK_R  
                      float *pfLogSeeEnv, 
                      int    iOrderAllpole, 
                      float *pfGain, 
                      float *pfAS, 
                      float *pfKS
                     ) 
{
   int   i, j;
   int   iStride;
   float fSigma;
   STACK_INIT
   float fRs[DFFT+2];
   STACK_INIT_END
   STACK_ATTACH(float *,fRs) 

   STACK_START

   /*-----------------------------------------------------------------
      Warp spectrum for better all-pole fit to low frequencies.
   -----------------------------------------------------------------*/
   VoxWarpSC36( pfLogSeeEnv, pfLogSeeEnv );

   /*-----------------------------------------------------------------
      Down sample the log spline power envelope and place it in the
        real part of the FFT buffer. Clear imaginary part of FFT buffer.
   -----------------------------------------------------------------*/
   iStride = 1 << DWNSMP;
   for (i=0, j=0; i<NFFT_2; i+=iStride, j+=2)
   {
       fRs[j] = fInvLog2(pfLogSeeEnv[i]);
       fRs[j+1] = 0.0F;
   }
   fRs[DFFT] = fInvLog2(pfLogSeeEnv[NFFT_2-1]); /* approximate value at PI */
   fRs[DFFT+1] = 0.0F;

   /*-----------------------------------------------------------------
      Compute autocorrelation function with inverse FFT.
   -----------------------------------------------------------------*/
   VoxRealInverseFft( fRs, DFFT_ORDER, IFFT_SCALE_FLAG);

   /*-----------------------------------------------------------------
     Apply the white noise scale factor to all but the first
      autocorrelation coefficient.
   -----------------------------------------------------------------*/
   ScaleVector( &fRs[1], iOrderAllpole, WHITE_NOISE_SCALE, &fRs[1] );

   /*-----------------------------------------------------------------
      Compute the predictor coefficients and the reflection 
       coefficients.
   -----------------------------------------------------------------*/
   VoxDurbin( fRs, iOrderAllpole, pfAS, pfKS, &fSigma);
   
   /*-----------------------------------------------------------------
      Use the prediction gain calculated within Durbin's recursion.
   -----------------------------------------------------------------*/
   *pfGain = 0.5F*fLog2(fSigma);

   STACK_END

} /* LogEnvelopeToAs() */

