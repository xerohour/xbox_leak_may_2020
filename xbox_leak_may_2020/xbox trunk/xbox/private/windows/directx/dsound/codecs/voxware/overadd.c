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
* Filename:      OverAdd.c
*
* Purpose:       Overlap/Add harmonics synthesizer using Fast Harmonic
*                  Transform and triangular window.
*
* Functions:     FHTOverlapAdd()
*
* Author/Date:   Bob Dunn 3/97
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/OverAdd.c_v   1.16   23 Apr 1998 12:50:40   bobd  $
*
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "vLibFilt.h"
#include "vLibTran.h"
#include "vLibVec.h"
#include "vLibMath.h"

#include "VoxMem.h"
#include "codec.h"
#include "trigSC.h"
#include "SumCos.h"
#include "FftSmCos.h"

#include "OverAdd.h"

#include "xvocver.h"

/*-------------------------------------------------------------
  Define synthesis types
-------------------------------------------------------------*/
#define FHT_SYNTHESIS 0
#define FFT_SYNTHESIS 1
#define COS_SYNTHESIS 2

/*-------------------------------------------------------------
  Select method of sine wave synthesis
-------------------------------------------------------------*/
#define SYNTHESIS_TYPE FHT_SYNTHESIS

/*-------------------------------------------------------------
  constants 
-------------------------------------------------------------*/
#define CLIP_LEVEL  (float)SHRT_MAX
 
/*-------------------------------------------------------------
  static functions
-------------------------------------------------------------*/
static void ClipOverflows( float *pfAmp, int iHarm);

#if (SYNTHESIS_TYPE==FHT_SYNTHESIS)

static void GetPhaseIndices( float *pfPhase, int *piPhaseIndex, int iHarm );

static void FHTSumCos( STACK_R
                float   fPitchDFT,
                float  *pfAmps,
                float  *pfPhase,
                int     iHarm,
                int     iLength,
                float  *pfWaveform
              );

/*-------------------------------------------------------------
  FHT weights
-------------------------------------------------------------*/
const static float fFHTWeights[] = {
#include "FHTW128.h"     
};

/*-------------------------------------------------------------
  external trig tables
-------------------------------------------------------------*/
extern const float *sinTBL_32;
extern const float *cosTBL_32;
extern const float fRadiansToIndex;

#endif /* (SYNTHESIS_TYPE==FHT_SYNTHESIS) */
 
/*******************************************************************************
*
* Function:  FHTOverlapAdd()
*
* Action:    Overlap/Add harmonics synthesizer using Fast Harmonic
*                  Transform and triangular window.
*
* Input:     float *pfAmps             -> harmonics amplitudes
*            float *pfPhase            -> harmonics phases
*            float  fPitchDFT          -> pitch in DFT samples
*            int    iHarm              -> number of harmonics
*            int    iSynSubFrameLength -> number of samples to synthesize
*            float *pfWaveform_1       -> previous waverform for overlap
*
* Output:    float *pfOutputBuffer     -> output waveform
*            float *pfWaveform_1        -> waverform for next overlap
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

void FHTOverlapAdd( STACK_R 
                    float *pfAmps, 
                    float  fPitchDFT, 
                    float *pfPhase, 
                    int    iHarm, 
                    int    iSynSubFrameLength, 
                    float *pfOutputBuffer,
                    float *pfWaveform_1
                  )
{
   int   iTotalSynLength;        /* overlap/add window length                 */
   STACK_INIT
   float pfWaveform[FRAME_LENGTH_SC]; 
   STACK_INIT_END

   STACK_ATTACH(float*,pfWaveform)

   STACK_START

   assert((pfAmps!=NULL) && (pfPhase!=NULL));
   assert((pfOutputBuffer!=NULL) && (pfWaveform_1!=NULL));

   iTotalSynLength = iSynSubFrameLength<<1;

   /*--------------------------------------------------
     Clear waveform array
   --------------------------------------------------*/
   memset(pfWaveform, 0, iTotalSynLength*sizeof(float) );

   /*---------------------------------------------------
     clip overflows by rescaling the harmonic amplitudes
   ---------------------------------------------------*/
   ClipOverflows(pfAmps, iHarm);


#if (SYNTHESIS_TYPE==FHT_SYNTHESIS)
   /*---------------------------------------------------
     Sum of cosines using Fast Harmonic Transform
   ---------------------------------------------------*/
   FHTSumCos(STACK_S fPitchDFT, pfAmps, pfPhase, iHarm, iTotalSynLength, 
                     pfWaveform);

#endif

#if (SYNTHESIS_TYPE==FFT_SYNTHESIS)
   /*---------------------------------------------------
     Sum of cosines using FFT
   ---------------------------------------------------*/
   FftSumCos(STACK_S fPitchDFT, pfAmps, pfPhase, iHarm, iTotalSynLength, 
                     pfWaveform);

#endif

#if (SYNTHESIS_TYPE==COS_SYNTHESIS)
   /*---------------------------------------------------
     Sum of cosines
   ---------------------------------------------------*/
   SumCos(fPitchDFT, pfAmps, pfPhase, iHarm, iTotalSynLength, pfWaveform);

#endif

   /*---------------------------------------------------
     clear temporary output buffer
   ---------------------------------------------------*/
   memset( pfOutputBuffer, 0, iSynSubFrameLength*sizeof(float) );
 
   /*---------------------------------------------------
     overlap and add waveforms
   ---------------------------------------------------*/
   OlAdd ( pfWaveform_1, pfWaveform, (short)iSynSubFrameLength,
           (short) iSynSubFrameLength, pfOutputBuffer);
 
   /*---------------------------------------------------
     Save half of the waveform for next time
   ---------------------------------------------------*/
   memcpy( pfWaveform_1, pfWaveform+iSynSubFrameLength,
           iSynSubFrameLength*sizeof(float) );

   STACK_END

   return;
}

#if (SYNTHESIS_TYPE==FHT_SYNTHESIS)
/*******************************************************************************
*
* Function:  FHTSumCos()
*
* Action:    Sum of cosines using Fast Harmonic Transform
*
* Input:     fPitchDFT  --> pitch in DFT samples
*            pfAmps     --> cosine amplitudes
*            pfPhase    --> cosine phase (in radians)
*            iHarm      --> number of harmonics
*            iLength    --> number of samples to synthesize
*
* Output:    pfOutBuf   --> sum of cosines (with length iLength)
*
* Globals:   none
*
* Return:    None
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void FHTSumCos( STACK_R 
                float   fPitchDFT, 
                float  *pfAmps, 
                float  *pfPhase, 
                int     iHarm, 
                int     iLength,
                float  *pfWaveform
              )
{
   float fNormFreq;              /* normalized pitch frequency                */
   float fNormPhase;
   float fResampRate;
   STACK_INIT
   float pfPitchCycle[FHT_EXP_SIZE+PP_NUM_SAMPLES];
   int   piPhaseIndex[MAXHARM];
   STACK_INIT_END
 
   STACK_ATTACH(float*,pfPitchCycle)
   STACK_ATTACH(int*,piPhaseIndex)
 
   STACK_START

   /*--------------------------------------------------
     Get phase indices
   --------------------------------------------------*/
   GetPhaseIndices( pfPhase, piPhaseIndex, iHarm );
 
   /*--------------------------------------------------
      Create the pitch epoch
   --------------------------------------------------*/
   VoxFHTkrnl( pfAmps, piPhaseIndex, iHarm, pfPitchCycle+PP_DELAY,
               FHT_EXP_SHIFT, fFHTWeights, sinTBL_32, cosTBL_32);
 
   /*--------------------------------------------------
     Interpolate the pitch epoch to generate the
       synthesized sum of sine waves.
   --------------------------------------------------*/
   fNormFreq   = fPitchDFT*INV_NFFT;
   fResampRate = (float)FHT_EXP_SIZE*fNormFreq;
   fNormPhase  = (float)(-(iLength>>1))*fNormFreq;
 
   PolyCycleIntp(pfWaveform, iLength, pfPitchCycle+PP_DELAY,
                  FHT_EXP_SHIFT, fResampRate, fNormPhase);

   STACK_END
}

/*******************************************************************************
*
* Function:  GetPhaseIndices()
*
* Action:    Get indices into trig tables for FHT phases
*
* Input:     float *pfPhase      -> pointer to array of phases (in radians)
*            int    iHarm        -> number of harmonics
*
* Output:    int   *piPhaseIndex -> pointer to array of indices
*
* Globals:   none
*
* Return:    None
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
static void GetPhaseIndices( float *pfPhase, 
                             int   *piPhaseIndex, 
                             int    iHarm 
                           )
{
   int   i;
   int   iTemp;
   float fTemp;

   for (i=0; i<iHarm; i++)
   {
      /*-------------------------------------------------
        Compute trig table index.
 
        NOTE: This assumes a 2's complement integer
              representation.  Using the trig mask 
              (iResPhase&TRIG_MASK) yeilds the correct
              positive index, even if iResPhase is
              negative.  This would not be true for  
              signed magnitude integer representation.
      -------------------------------------------------*/
      fTemp = (pfPhase[i]+PIO2)*fRadiansToIndex;
      VoxROUND2( fTemp, iTemp );
      piPhaseIndex[i] = iTemp & TRIG_MASK;
   }
}

#endif /* (SYNTHESIS_TYPE==FHT_SYNTHESIS) */

/*******************************************************************************
*
* Function:  ClipOverflows()
*
* Action:    Prevent overflows by rescaling the entire harmonic amplitudes
*              
* Input:     float *pfAmps       -> input harmonic amplitudes
*            int    iHarm        -> number of harmonics
*
* Output:    float *pfAmps       -> scaled harmonic amplitudes
*
* Globals:   none
*
* Return:    None
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

static void ClipOverflows( float *pfAmps,
                           int    iHarm
                         )
{
   int   i;
   float fSumAmp;
   float fScale = 1.0F;

   assert(pfAmps != NULL);
   assert(iHarm > 0);

   /*--------------------------------------------------
     Prevent clipping by scaling the output to have
       a maximum value of fClipLevel.
   --------------------------------------------------*/
   fSumAmp = 0.0F;
   for (i = 0; i < iHarm; i++)
     fSumAmp += pfAmps[i];
 
   if (fSumAmp > CLIP_LEVEL) 
     fScale = CLIP_LEVEL/fSumAmp;

   ScaleVector( pfAmps, iHarm, fScale, pfAmps);
}



