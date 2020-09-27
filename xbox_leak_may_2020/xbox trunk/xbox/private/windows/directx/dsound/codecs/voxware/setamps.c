/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/
 
/******************************************************************************
* Filename:       SetAmps.c
*
* Purpose:        Interpolate the amplitude and phase envelopes at the
*                 sine-wave harmonics. Apply KL amplitude scale factor to 
*                 the unvoiced harmonics.  Also, increase the number of 
*                 unvoiced harmonics by adding sub-harmonics.
*
* Functions:      SetAmpAndPhase()
*                 AddSubHarmonics()
*
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications: Set pfEnv[] and pfPhase[] at PI as holding previous values
*                rather than 0. And reduce complexity. Wei Wang, 4/98
*
* Comments:
*
* Concerns: Requires the length of the envelopes to be 65!  The 65th sample
*             is accessed, but multiplied by zero insetad of putting an
*             if statement inside the loop.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/SetAmps.c_v   1.7   10 Apr 1998 11:50:56   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "vLibMath.h"

#include "codec.h"
#include "warp.h"
#include "SetAmps.h"

#include "xvocver.h"

/*-----------------------------------------------------------
  To convert frequency to down-sampled scale
-----------------------------------------------------------*/
#define  WARP_FREQ_SCALE    (1.0F/(float)(1<<CSENVDWNSMP))

/*******************************************************************************
*
* Function:  SetAmpAndPhase()
*
* Action:    Set the log amplitude and minimum-phase phase of the 
*              harmonics.
*
* Input:     float  fNoiseScale   -> scale factor for unvoiced harmonics
*                                      (log base 2 value to be added)
*            float  fPitchDFT     -> pitch in DFT samples
*            int    iHarmonics    -> number of harmonics to generate
*            int    iVoicHarm     -> number of harmonics which are voiced
*            float *pfEnv         -> current envelope (warped, log 2 magnitude)
*            float *pfEnv_1       -> previous envelope (warped, log 2 magnitude)
*            float *pfPhase       -> current unwrapped minimum-phase envelope
*            float *pfPhase_1     -> previous unwrapped minimum-phase envelope
*            float  fInterpFactor -> envelope interpolation factor
*
* Output:    float *pfLogAmps     -> harmonic amplitudes (log base 2)
*            float *pfMinFaz      -> minimum-phase harmonic phases
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:  pfEnv[], pfPhase[] at PI must be set to hold the previous
*                 values. Then if fWarpedFreq is between [CSENV_NFFT_2-1,
*                 CSENV_NFFT_2), the interpolated envelope and phase should 
*                 take the value at CSENV_NFFT_2-1. This should be better than
*                 setting pfEnv[] and pfPhase[] at PI to be 0 since no extra
*                 if-statement is required. W.W, 4/98 
*
* Comments:  IMPORTANT NOTE: The code is currently written such that
*                              pfEnv[], pfEnv_1[], pfPhase[], pfPhase_1[]
*                              must have a valid numeric value at PI.
*                              This value is multiplied by zero, but it
*                              must exist.  This problem could be avoided
*                              by placing an if statement inside the loop,
*                              but this would increase code size and 
*                              operate more slowly.
*
* Concerns/TBD:
*******************************************************************************/

void SetAmpAndPhase ( float  fNoiseScale,
                      float  fPitchDFT,
                      float *pfLogAmps,
                      float *pfMinFaz,
                      int    iHarmonics,
                      int    iVoicHarm,
                      float *pfEnv,
                      float *pfEnv_1,
                      float *pfPhase,
                      float *pfPhase_1,
                      float  fInterpFactor
                     )
{
   int   n;
   int   iIndex;
   float fWarpedFreq;
   float fFreqInterp;
   float fFreq;
   float fEnv, fEnv_1;
   float fPhase, fPhase_1;

   assert((pfLogAmps != NULL) && (pfMinFaz != NULL));
   assert((pfEnv != NULL) && (pfEnv_1 != NULL));
   assert((pfPhase != NULL) && (pfPhase_1 != NULL));

   fFreq = fPitchDFT;

   /*-------------------------------------------------------------------
     The points at PI must be set to avoid
     if statements within a for loop in SetAmpAndPhase().
   -------------------------------------------------------------------*/
   pfEnv[CSENV_NFFT_2]   = pfEnv[CSENV_NFFT_2-1];
   pfPhase[CSENV_NFFT_2]   = pfPhase[CSENV_NFFT_2-1];

   for (n = 0; n < iHarmonics; n++) {

      /*------------------------------------------------------------------
        Compute warped frequency on 64 point scale
      ------------------------------------------------------------------*/
      fWarpedFreq = CalcWarpedFreq(fFreq)*WARP_FREQ_SCALE;

      fFreq += fPitchDFT;

      /*------------------------------------------------------------------
        Confine frequency to range of 0.0 <= fWarpedFreq <= fLastEnvelopePoint
          WARNING: This will actually read from the envelopes
                   0<=i<=(fLastEnvelopePoint+1) so be sure the envelopes 
                   are (fLastEnvelopePoint+2) points 
                   long! (One alternative is to put an if statment 
                   in the loop, but this would be slow)
      ------------------------------------------------------------------*/
      assert(fWarpedFreq >= 0.0F);
      iIndex = VoxFLOORpos( fWarpedFreq );

      assert(iIndex < CSENV_NFFT_2);
      fFreqInterp = fWarpedFreq - (float) iIndex;

      /*------------------------------------------------------------------
        Interpolate Log amplitude envelopes, first in frequency, 
          then in time.
      ------------------------------------------------------------------*/
      fEnv_1 = VoxINTERP( pfEnv_1[iIndex], pfEnv_1[iIndex+1], fFreqInterp );
      fEnv = VoxINTERP( pfEnv[iIndex], pfEnv[iIndex+1], fFreqInterp);
      pfLogAmps[n] = VoxINTERP( fEnv_1, fEnv, fInterpFactor);

      /*------------------------------------------------------------------
        Interpolate unwrapped phase envelopes, first in frequency, 
          then in time.
      ------------------------------------------------------------------*/
      fPhase_1 = VoxINTERP(pfPhase_1[iIndex], pfPhase_1[iIndex+1], fFreqInterp);
      fPhase = VoxINTERP(pfPhase[iIndex], pfPhase[iIndex+1], fFreqInterp);
      pfMinFaz[n] = VoxINTERP( fPhase_1, fPhase, fInterpFactor );
   }

   /*------------------------------------------------------------------
     Rescale the unvoiced harmonics to account for the analysis window 
       energy and the synthesis pitch.
   ------------------------------------------------------------------*/
   for (n=iVoicHarm; n<iHarmonics; n++)
      pfLogAmps[n] += fNoiseScale;

}

/*******************************************************************************
*
* Function:  AddSubHarmonics()
*
* Action:    This routine adds sub-harmonics in the unvoiced region.
*              Since we will use a harmonic synthesizer, 
*              voiced sub-harmonics are also added but their
*              magnitudes are set to zero.
*
* Input:     float *pfLogAmps     -> harmonics amplitudes (log base 2)
*            float *pfMinFaz      -> harmonics phases
*            int    iHarmonics    -> total number of (input) harmonics
*            int    iSubHarmonics -> total number of (output) harmonics
*                                      plus sub-harmonics
*            int    iRatio        -> the ratio: iSubHarmonics/iHarmonics
*            int    iVoicHarm     -> number of (input) voiced harmonics
*
*
* Output:    float *pfLogAmps     -> harmonics amplitudes (log base 2)
*            float *pfMinFaz      -> harmonics phases
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void AddSubHarmonics ( float *pfLogAmps,
                       float *pfMinFaz,
                       int    iHarmonics,
                       int    iSubHarmonics,
                       int    iRatio,
                       int    iVoicHarm
                     )
{
 
   int   i, j, k;

   assert(pfLogAmps!=NULL);
   assert(pfMinFaz != NULL);

   k = iSubHarmonics-1;
   /* unvoiced band */
   for (i = iHarmonics-1; i>=iVoicHarm; i--)
     {
       for (j = 0; j < iRatio; j++, k--)
         {
           pfLogAmps[k] = pfLogAmps[i];
           pfMinFaz[k] = pfMinFaz[i];
         }
     }
   
   /* voiced band */
   for (; i >= 0; i--)
     {
       pfLogAmps[k] = pfLogAmps[i];
       pfMinFaz[k] = pfMinFaz[i];
       k--;
       for (j = 1; j < iRatio; j++, k--) 
         {
           pfLogAmps[k] = 0.0F;
           pfMinFaz[k] = 0.0F;
         }
     }
}



