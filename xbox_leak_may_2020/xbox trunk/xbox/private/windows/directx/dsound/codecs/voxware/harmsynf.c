/*****************************************************************************
 *                        Voxware Proprietary Material                       *
 *                        Copyright 1996, Voxware Inc.                       *
 *                        All Rights Reserved                                *
 *                                                                           *
 *                        DISTRIBUTION PROHIBITED without                    *
 *                        written authorization from Voxware.                *
 *****************************************************************************/

/*****************************************************************************
 * FILE      : HarmSynFloat.c
 *
 * CONTAINS  : Synthesis of input harmonics
 *
 * CREATION  : 11/7/96
 *
 * AUTHOR    : Bob Dunn (derived from Rob Zopf's code for fixed point FHT) 
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/HarmSynF.c_v   1.5   20 Apr 1998 14:40:16   weiwang  $
 *****************************************************************************/

#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "vLibTran.h" /* VoxFHTkrnl()      */
#include "vLibFilt.h" /* PolyCycleIntp()       */
#include "vLibVec.h"  /* OlAdd()           */
#include "vLibMath.h"

#include "model.h"
#include "VoxMem.h"

#include "HarmSynF.h"

#include "xvocver.h"

#if USE_FHT_SYN==1
const static float fFHTWeights64[] = {
#include "FHTW64.h"      
};

const static float fFHTWeights128[] = {
#include "FHTW128.h"      
};

extern const float *sinTBL;
extern const float *cosTBL;

/*****************************************************************************
 * FUNCTION    : HarmSyn()
 *
 * PURPOSE     : Synthesis of speech harmonics and phases
 *
 * INPUT
 *     pfAmps     - harmonic amplitudes
 *     piPhases   - harmonic phases at the current frame boundary (0-2PI)
 *     fPitch     - time-domain pitch lag
 *     fVPitch    - the original voiced pitch (before sub-multiple change)
 *     fNormPhase - the pitch-dependent linear phase for FHT for fPitch
 *     fVoicePhase - the linear-phase for FHT for fVPitch
 *     fResampRate - pitch-dependent resampling rate for pitch cycle interpolation
 *     nHarm      - number of harmonics to synthesize
 *     pfPrevSyn  - previous synthesis buffer for use in current frame
 *     pfOutBuf   - output buffer for synthesized harmonics
 *     frameLEN   - synthesis frame length
 *     olLEN      - overlap length 
 *
 * OUTPUT
 *     pfOutBuf   - synthesized harmonics
 *     pfPrevSyn  - updated for use next time
 *     fNormPhase   - updated FHT linear phase
 *     fVoicePhase - updated FHT linear phase for fVPitch
 *     fResampRate - updated resampling ratio
 *
 *
 * AUTHOR      : Rob Zopf
 *
 * DATE        : 11/7/96
 *
 * SPECIAL NOTES : This routine ACCUMULATES into pfOutBuf.  If needed, pfOutBuf
 *                 should be set to zero prior to calling this routine.
 *
 * WARNING     :   frameLEN+olLEN MUST BE EVEN !!!!
 *
 * MODIFICATION HISTORY :
 *
 *
 ******************************************************************************/
void HarmSynFHTFloat ( STACK_R
                       float    *pfAmps,
                       int    *piPhases,
                       float     fPitch,
                       float     fVPitch,
                       float     fVPitch_1,
                       float    *fNormPhase,
                       float    *fVoicePhase,
                       float    *fResampRate,
                       short     nHarm,
                       float    *fPitchCycle,
                       float    *pfOutBuf,
                       short     frameLEN,
                       short     olLEN
                )
{
   STACK_INIT
   float  fSynBuf[MAXSYNLEN];  /* scratch */
   float  pfPrevSyn[MAXSYNLEN];  /* scratch */
   STACK_INIT_END
   STACK_ATTACH(float*,fSynBuf)
   STACK_ATTACH(float*,pfPrevSyn)
   short  synLEN;
   float  *fFHTWeights;
   short  exp_size;
   short  exp_shift;
   short  maxHarm;

   STACK_START

   maxHarm = MAXHARM;


   if (maxHarm < 64 )
   {
      fFHTWeights = (float *)fFHTWeights64;
      exp_shift = 7;
      exp_size = 1<<exp_shift;
   }
   else
   {
      fFHTWeights = (float *)fFHTWeights128;
      exp_shift = 8;
      exp_size = 1<<exp_shift;
   }

 /*---------------------------------------------
    Make sure that frameLEN+olLEN is even
   ---------------------------------------------*/
   if (((olLEN+frameLEN)&1)==1)
      olLEN--;

 /*---------------------------------------------
    Make sure that frameLEN >= olLEN 
   ---------------------------------------------*/
   if (olLEN>frameLEN)
      olLEN=frameLEN;

   synLEN   = olLEN + ((frameLEN-olLEN)>>1);

 /*----------------------------------------------------
    Perform the synthesis (for now, use either 
    sum of sinusoids or FHT )
   ----------------------------------------------------*/ 
   *fNormPhase = PolyCycleIntp(pfPrevSyn, (int)synLEN, fPitchCycle+PP_DELAY,
                               (int)exp_shift, *fResampRate, *fNormPhase);

   VoxFHTkrnl(pfAmps, piPhases, (int)nHarm, fPitchCycle+1, 
              (int) exp_shift, fFHTWeights, sinTBL, cosTBL);

/*--------------------------------------------------
   Now sample the pitch epoch using a polyphase 
   filter to get our waveform ...
  --------------------------------------------------*/
   *fResampRate = (float)(exp_size)/fPitch;

   *fNormPhase = *fVoicePhase + (float)(synLEN-olLEN)/fVPitch_1;
   *fNormPhase *= (fVPitch/fPitch);

   *fNormPhase = PolyCycleIntp(fSynBuf, (int)synLEN, fPitchCycle+PP_DELAY,
                               (int)exp_shift, *fResampRate, *fNormPhase);

   *fVoicePhase += (float)frameLEN/((fVPitch+fVPitch_1)*0.5F);
   *fVoicePhase -= (float)(VoxFLOOR(*fVoicePhase)); /* mod phase to get fractional part */

 /*----------------------------------------------------
     Do overlap-add of buffers ...
   ----------------------------------------------------*/
   OlAdd(pfPrevSyn, fSynBuf, (const int) olLEN, (const int) frameLEN, pfOutBuf);

   STACK_END
}


#endif


