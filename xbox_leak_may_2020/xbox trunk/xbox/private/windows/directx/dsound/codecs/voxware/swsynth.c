/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       SWSynth.c
*                                                                              
* Purpose:        Sine-wave synthesis for one frame of data.
*                                                                              
* Functions:      VoxSWSynth()
*
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications:                                                               
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/SWSynth.c_v   1.7   13 Apr 1998 18:26:20   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "vLibMath.h"

#include "VoxMem.h"
#include "codec.h"
#include "rate.h"
#include "KLPitch.h"
#include "SetAmps.h"
#include "HFRegen.h"
#include "ExPhase.h"
#include "Jitter.h"
#include "UVPhase.h"
#include "OverAdd.h"
#include "SWSynth.h"

#include "xvocver.h"

/*******************************************************************************
*
* Function:  VoxSWSynth()
*
* Action:    Synthesize 10 ms of speech from parameters.
*
* Input:  void   *hHFRegenMblk           -> memory block for HFRegen
*         int     iSCRateSyn             -> current synthesis rate
*         int     iSynSubFrameLength     -> number of samples to synthesize
*         float  *pfVEVphase             -> measured phases
*         float  *pfWaveform_1           -> previous synthesized waveform
*         float   fSynPitch              -> pitch for synthesis
*         float   fPv                    -> voicing probability for synthesis
*         float  *pfEnv                  -> magnitude envelope
*         float  *pfEnv_1                -> previous magnitude envelope
*         float  *pfPhase                -> phase envelope
*         float  *pfPhase_1              -> previous phase envelope
*         float   fInterpFactor          -> envelope interpolation factor
*         int     iUpdatePhaseOffsetFlag -> flag for HFRegen
*         long    *plUVPhaseRandSeed     -> address for random seed for 
*                                            unvoiced phases
*
* Output: float  *pfSynSpeech            -> output speech
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
void VoxSWSynth(STACK_R 
                void   *hHFRegenMblk,
                int     iSCRateSyn,
                float  *pfSynSpeech,
                float  *pfVEVphase,
                float  *pfWaveform_1,
                float   fSynPitch,
                float   fPv,
                float  *pfEnv,
                float  *pfEnv_1,
                float  *pfPhase,
                float  *pfPhase_1,
                float   fInterpFactor, 
                int     iUpdatePhaseOffsetFlag,
                long   *plUVPhaseRandSeed
               )
{
   int i;
   int iIndex;
   int iHarmonics;     /* number of sine waves                                */
   int iSubHarmonics;  /* number of sine waves including sub-harmonics        */
   int iRatio;         /* ratio of iSubHarmonics to iHarmonics                */
   int iBasePeaks;     /* number of peaks with measured phases                */
   int iVoicHarm;      /* number of voiced sine waves                         */
   int iVoicSubHarm;   /* number of voiced sine waves including sub-harmonics */

   float fFHT_F0;       /* F0 to use in FHT synthesis                    */
   float fVoicingCutoff;  /* the frequency cutoff for voicing                 */
   float fBasebandCutoff; /* the frequency cutoff for measured phases         */
   float fFinalCutoff;    /* the frequency cutoff for randomizing phases      */
   float fNoiseScale;     /* scale factor for unvoiced harmonics              */
   float fOnsetPhase;     /* the onset phase                                  */
   float fBeta;           /* the beta factor                                  */
   float fTemp;
   float fDFT_F0;

STACK_INIT
   float pfMinPhase[MAXHARM];
   float pfAmp[MAXHARM];
STACK_INIT_END

   STACK_ATTACH(float *, pfMinPhase)
   STACK_ATTACH(float *, pfAmp)
  
   STACK_START

   assert(hHFRegenMblk != NULL && pfSynSpeech != NULL);
   assert(pfVEVphase != NULL && pfWaveform_1 != NULL);
   assert(pfEnv != NULL && pfEnv_1 != NULL);
   assert(pfPhase != NULL && pfPhase_1 != NULL);
   assert(plUVPhaseRandSeed != NULL);

   fDFT_F0 = (float)NFFT/fSynPitch;
 
   /*--------------------------------------------------------------------
     Determine the number of harmonics
   --------------------------------------------------------------------*/
   iHarmonics = VoxFLOORpos(0.5F*fSynPitch);

   fTemp = fDFT_F0*(float)iHarmonics;
   VoxROUND2pos( fTemp, iIndex );
 
   if (iIndex >= NFFT_2)
         iHarmonics --;
 
   iHarmonics = VoxMIN( iHarmonics, MAXHARM );

   /*--------------------------------------------------------------------
     Determine the voicing cutoff and the number of baseband peaks
   --------------------------------------------------------------------*/
   fVoicingCutoff=fPv*(float)NFFT_2;
   if (iSCRateSyn==SC_RATE_3K)
   {
      iBasePeaks = 0;
      fFinalCutoff = fVoicingCutoff;
   } else {
      iBasePeaks = VoxMIN( SC6_PHASES, iHarmonics );
      fBasebandCutoff = ((float)iBasePeaks+0.5F)*fDFT_F0;
      fFinalCutoff = VoxMAX( fBasebandCutoff, fVoicingCutoff );
   }
 
   /*--------------------------------------------------------------------
     Determin iRatio and number of harmonics
   --------------------------------------------------------------------*/
   iRatio = (fVoicingCutoff<NFFT_2) ? (MAXHARM/iHarmonics) : 1;

   iSubHarmonics = iHarmonics*iRatio;

   fFHT_F0 = fDFT_F0/(float)iRatio;
 
   iVoicHarm = VoxFLOORpos( fFinalCutoff*INV_NFFT*fSynPitch);

   iVoicSubHarm = iVoicHarm*iRatio;


   /*--------------------------------------------------------------------
     Compute the Karhunen-Loeve scale factor to account for the analysis 
       window energy and the synthesis pitch for unvoiced harmonics
   --------------------------------------------------------------------*/
   fNoiseScale = fSetKLFactor( fSynPitch, fFHT_F0);

   /*--------------------------------------------------------------------
     Generate samples of the amplitude and phase envelopes at the
       pitch harmonics by interpolating in both time and frequency.
   --------------------------------------------------------------------*/
   SetAmpAndPhase ( fNoiseScale, fDFT_F0, pfAmp, pfMinPhase, 
                    iHarmonics, iVoicHarm, pfEnv, pfEnv_1, pfPhase, 
                    pfPhase_1, fInterpFactor);

   /*--------------------------------------------------------------------
     Estimate the rate-dependent excitation phase parameters
   --------------------------------------------------------------------*/
   VoxHFRegeneration(STACK_S hHFRegenMblk, pfVEVphase, 
                     fDFT_F0, pfAmp, pfMinPhase, iHarmonics,
                     &fOnsetPhase, &fBeta, iSCRateSyn, 
                     HALF_FRAME_LENGTH_SC, iUpdatePhaseOffsetFlag);

   /*--------------------------------------------------------------------
     Add the linear phase to the minimum phase and the residual phase
   --------------------------------------------------------------------*/
   ExcitationPhase(fOnsetPhase, fBeta, pfVEVphase,
                   pfMinPhase, iHarmonics, iBasePeaks );

   /*--------------------------------------------------------------------
     Add glottal pulse jitter to the uncoded odd harmonics 
   --------------------------------------------------------------------*/
   PhaseJitter( fSynPitch, pfMinPhase, iBasePeaks, iHarmonics );


   /*--------------------------------------------------------------------
     Convert log_base_2 amplitudes to linear
   --------------------------------------------------------------------*/
   for(i = 0; i < iHarmonics; i++)
      pfAmp[i] = fInvLog2(pfAmp[i]);

   /*--------------------------------------------------------------------
     Increase the number of unvoiced harmonics by adding sub-harmonics.
       This is done now rather the previously in SetAmps() to save on the
       maximum number of fInvLog2()'s required.  The maximum number of
       pitch harmonics is 80 because the minimum pitch is 50 Hz.  The
       maximum number of synthesis harmonics is 100, so we can save
       20 fInvLog2()'s from the peak complexity by increasing the
       number of harmonics after taking fInvLog2().
   --------------------------------------------------------------------*/
   AddSubHarmonics ( pfAmp, pfMinPhase, iHarmonics, iSubHarmonics, 
                     iRatio, iVoicHarm);
 
   /*--------------------------------------------------------------------
     For uncoded phases add on the random phase above cutoff
   --------------------------------------------------------------------*/
   UnvoicedPhase( pfMinPhase, iSubHarmonics, iVoicSubHarm, plUVPhaseRandSeed);

   /*--------------------------------------------------------------------
     Compute the sine-wave parameters and perform sine-wave
     synthesis using FHT and overlap-add
   --------------------------------------------------------------------*/
   FHTOverlapAdd (STACK_S pfAmp, fFHT_F0, pfMinPhase, iSubHarmonics, 
                  HALF_FRAME_LENGTH_SC, pfSynSpeech, pfWaveform_1);

   STACK_END
}

