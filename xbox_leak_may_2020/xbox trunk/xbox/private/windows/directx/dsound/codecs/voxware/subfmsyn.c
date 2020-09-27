/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1998, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       SubFmSyn.c
*                                                                              
* Purpose:        synthesis speech of each sub-frame
*                                                                              
* Functions:      SubFrameSynth()
*                                                                              
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications: Moved out from DecSC.c by Wei Wang, 4/98
*                Remove frameEnergy calculation to VciPlsSC.c.
*                                         
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/SubFmSyn.c_v   1.7   30 Apr 1998 17:28:06   bobd  $
******************************************************************************/


#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "vLibTran.h"
#include "vLibVec.h"
#include "vLibMath.h"
#include "VoxMem.h"

#include "paramSC.h"
#include "codec.h"
#include "quansc36.h"
#include "SWSynth.h"
#include "rate.h"
#include "AskToEnv.h"
#include "PostFilt.h"
#include "HFRegen.h"
#include "OutBufSC.h"
#include "DecPriSC.h"
#include "SubFmSyn.h"

#include "xvocver.h"
 
#define PV_SWITCH_THRESH 0.125F
#define INTERP_FACTOR_SMALL  0.25F
#define INTERP_FACTOR_MED    0.75F

static void OffsetMeasuresPhases(float *pfVEVphase, 
                                 float *pfCumPhaseOffset,
                                 int iWarpSamples, 
                                 float fPitchPeriod,
                                 float *pfVEVphaseSyn,
                                 int iRepeatSamples);

 
/*******************************************************************************
*
* Function:  SubFrameSynth()
*
* Action:   synthesis the speech of each sub-frame.
*
* Input:    hDecMemBlk -- decoder structure
*           hParamMblk -- parameter structure
*           pfSpeechBuf -- output speech buffer
*           pfEnv -- envelope spectrum
*           pfPhase -- minimum phases of current frame
*           piSynSamples -- pointer to the number of output speech samples
*
* Output:   pfSpeechBuf -- output speech
*           piSynSamples -- number of output speech samples
*
* Globals:   none
*
* Return:    none
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void SubFrameSynth( STACK_R 
                    void           *hDecMemBlk, 
                    void           *hSubParamMblk,
                    float          *pfSpeechBuf,
                    float          *pfEnv,
                    float          *pfPhase,
                    int            *piSynSamples
                  )
{
   DEC_MEM_BLK *pDecMemBlk = (DEC_MEM_BLK *)hDecMemBlk;
   SUBFRAME_PARAM_SC *pSubParamMblk = (SUBFRAME_PARAM_SC *)hSubParamMblk;

   int     iSubFrame;
   int     iUpdatePhaseOffsetFlag = 0;
   int     iRepeatSamples;
   int     iSCRateSyn = pDecMemBlk->iSCRate_1;
   int     OffsetFlag = 0;
   int     iOffsetSamples;
   float  *pfSynSpeech;
   float   fInterpFactor;
   float   fPitchPeriod_1;
   float   fOffsetPitchPeriod;
   float   fFrameTime;

   SUBFRAME_PARAM_SC *pSubParamSyn;
   int     *piCountSyn;

STACK_INIT
   float pfVEVphaseSyn[SC6_PHASES];
STACK_INIT_END

   STACK_ATTACH(float *, pfVEVphaseSyn)

   STACK_START

   pfSynSpeech = pfSpeechBuf;

   /*-------------------------------------------------------------------------
     fFrameTime is the time (in samples) corresponding to the parameters
       in pParamMblk after the time scale has been warped.  Time has been
       normalized so the parameters in pParamMblk_1 are located at time
       sample 0.  (The mid-frame parameters in pParamMblk are at time
       0.5*fFrameTime).
   -------------------------------------------------------------------------*/
   fFrameTime = pDecMemBlk->fActualWarp * FRAME_LENGTH_SC;

   /*-------------------------------------------------------------------
   | Synthesize up to two subframes of speech.
   --------------------------------------------------------------------*/
   for (iSubFrame = 2; iSubFrame > 0; iSubFrame--) {
      /*-------------------------------------------------------------------
      | If the location of the center of the current synthesis frame
      |   is between pParamMblk_1 and pParamMblk (between 0 and fFrameTime)
      |   then synthesize the frame.  When warping is not used, 
      |   fSynTimeOffset is equal to fFrameTime/2 for the first subframe,
      |   and it is equal to fFrameTime for the second subframe.
      -------------------------------------------------------------------*/
      iUpdatePhaseOffsetFlag = 0;
      if (pDecMemBlk->fSynTimeOffset<=fFrameTime)
      {
         /*-------------------------------------------------------------------
         | Set the interpolation factor.  When warping is not used, 
         |   fInterpFactor is 0.5 for the first subframe, and 1.0 
         |   for the second sub-frame.
         -------------------------------------------------------------------*/
         fInterpFactor = pDecMemBlk->fSynTimeOffset/fFrameTime;
         iRepeatSamples = 0;
   
         /*-------------------------------------------------------------------
         | Set the parameters for a 10ms sub-frame
         -------------------------------------------------------------------*/
         if (fInterpFactor < INTERP_FACTOR_SMALL)
         {  
            /*----------------------------------------------------------------
            | Use parameters from previous frame (previous 20 ms analysis)
            ----------------------------------------------------------------*/
           pSubParamSyn =  &(pDecMemBlk->PrevSubParam);
           fPitchPeriod_1 = pDecMemBlk->Pitch_1_mid;
           piCountSyn = &(pDecMemBlk->piFrameCount[0]);
           if (pDecMemBlk->iFramesRepeated)
               iRepeatSamples = (pDecMemBlk->iFramesRepeated-1)*FRAME_LENGTH_SC;
         } 
         else {
           if (fInterpFactor < INTERP_FACTOR_MED)
           {
              /*----------------------------------------------------------------
              | Use parameters from mid-frame frame (current 10 ms analysis)
              ----------------------------------------------------------------*/
              pSubParamSyn = &(pSubParamMblk[PARAM_MIDFRAME]);
              fPitchPeriod_1 = pDecMemBlk->PrevSubParam.Pitch;           
              piCountSyn = &(pDecMemBlk->piFrameCount[1]);
           } else { 
             /*----------------------------------------------------------------
             | Use parameters from outer frame (current 20 ms analysis)
             ----------------------------------------------------------------*/
             pSubParamSyn = &(pSubParamMblk[PARAM_ENDFRAME]);
             fPitchPeriod_1 = pSubParamMblk[PARAM_MIDFRAME].Pitch;
             piCountSyn = &(pDecMemBlk->piFrameCount[2]);
           }

           OffsetFlag     = pDecMemBlk->iSetOffsetFlag;
           iRepeatSamples = pDecMemBlk->iFramesRepeated*FRAME_LENGTH_SC;
         } 

         /*----------------------------------------------------------------
         | Count points to the counter for the number of times the 
         |   current sub-frame parameters have been synthesized. iCount_1 
         |   is the number of times the previous sub-frame parameters 
         |   were synthesized.  
         |
         | Increment the count.
         ----------------------------------------------------------------*/
         piCountSyn[1] ++;
         fOffsetPitchPeriod = pSubParamSyn->Pitch;

         /*----------------------------------------------------------------
         | If the count is more than 1 (i.e. the sub-frame parameters have 
         |   been used before) then the linear phase offset must be INCREASED
         |   by an amount corresponding to HALF_FRAME_LENGTH_SC samples. The 
         |   pitch period for this is the pitch period of the current sub-frame.
         |   NOTE: The sub-frame can be repeated as many times as 
         |   desired since the phase offset is cumulative.
         ----------------------------------------------------------------*/
         if (piCountSyn[1] > 1)  
            iOffsetSamples = HALF_FRAME_LENGTH_SC;
         /*----------------------------------------------------------------
         | If the count is 1 (i.e. the first time these sub-frame parameters 
         |   are used) but iCount_1 is zero (i.e. the previous sub-frame 
         |   parameters were skipped) then the linear phase offset must be 
         |   DECREASED by an amount corresponding to HALF_FRAME_LENGTH_SC 
         |   samples. The pitch period for this is the pitch period of the 
         |   sub-frame that was skipped.
         |   NOTE: This assumes that a maximum of 1 consecutive sub-frame 
         |   will be skipped.  If more than 1 consecutive sub-frame is
         |   skipped, the phase offset will not be accounted for.
         ----------------------------------------------------------------*/
         else if ((piCountSyn[0]==0) && (piCountSyn[1]==1))
         {
            iOffsetSamples = -HALF_FRAME_LENGTH_SC;
            fOffsetPitchPeriod = fPitchPeriod_1;
         }
         /*----------------------------------------------------------------
         | Count is 1 (i.e. the first time these sub-frame parameters
         |   are used) and iCount_1 is not zero (i.e. the previous sub-frame
         |   parameters were not skipped) so there is no need to change
         |   the phase offset.
         ----------------------------------------------------------------*/
         else
            iOffsetSamples = 0;

         /*--------------------------------------------------------------------
         | Possibly update the phase offset in VoxHFRegeneration(). This is
         |   done when entire frames have been skipped, or when frames have
         |   been repeated.  In either of these cases the measured phases
         |   have a discontinuity in their embeded linear phases.
         --------------------------------------------------------------------*/
         if (OffsetFlag > 0) 
         {
            iUpdatePhaseOffsetFlag = 1;
            pDecMemBlk->iSetOffsetFlag = 0;
         }
     
         /*--------------------------------------------------------------------
         | Set the voicing-depdendent bit rate for the synthesizer
         --------------------------------------------------------------------*/
         if ((pDecMemBlk->iSCTargetRate==SC_RATE_6K) && 
             (iSCRateSyn==SC_RATE_3K) &&
             (pSubParamSyn->Pv>PV_SWITCH_THRESH))
            iSCRateSyn = SC_RATE_3K;
         else 
            iSCRateSyn = pDecMemBlk->iSCTargetRate;
      
         /*--------------------------------------------------------------------
         | Add a linear phase offset to the measured phases to account for
         |   time warping.  (If iOffsetSamples==0 there is no additional
         |   linear phase offset added)
         --------------------------------------------------------------------*/
         if (pDecMemBlk->iSCTargetRate==SC_RATE_6K)
            OffsetMeasuresPhases( pSubParamSyn->pfVEVphase,
                                  &(pDecMemBlk->fPhaseOffset), 
                                  iOffsetSamples, fOffsetPitchPeriod, 
                                  pfVEVphaseSyn,
                                  iRepeatSamples);

         /*--------------------------------------------------------------------
         | Synthesize 10 ms of Speech
         --------------------------------------------------------------------*/
         VoxSWSynth(STACK_S pDecMemBlk->hHFRegenMblk,
                    iSCRateSyn, pfSynSpeech, pfVEVphaseSyn,
                    pDecMemBlk->pWaveform_1, 
                    pSubParamSyn->Pitch, pSubParamSyn->Pv,
                    pfEnv, pDecMemBlk->pfEnv_1, 
                    pfPhase, pDecMemBlk->pfPhase_1, 
                    fInterpFactor, iUpdatePhaseOffsetFlag,
                    &(pDecMemBlk->lUVPhaseRandSeed));
      
         /*--------------------------------------------------------------------
         | increment speech pointer, time offset, and sample count
         --------------------------------------------------------------------*/
         pfSynSpeech                += HALF_FRAME_LENGTH_SC;
         *piSynSamples              += HALF_FRAME_LENGTH_SC;
         pDecMemBlk->fSynTimeOffset += HALF_FRAME_LENGTH_SC;
 
         /*--------------------------------------------------------------------
         | save synthesis rate flag
         --------------------------------------------------------------------*/
         pDecMemBlk->iSCRate_1 = iSCRateSyn;
      }
   }


   /*--------------------------------------------------------------------
     If (fSynTimeOffset>fFrameTime) then set the flag for a new frame
       next time
   --------------------------------------------------------------------*/
   if (pDecMemBlk->fSynTimeOffset>fFrameTime)
   {
      pDecMemBlk->iNewFrame  = 1;

      /*----------------------------------------------------------------------
        Advance the time reference. (i.e. pParamMblk becomes pParamMblk_1
          so the current value of fFrameTime is moved to 0)
      ----------------------------------------------------------------------*/
      pDecMemBlk->fSynTimeOffset -= fFrameTime;
   }

  STACK_END
}


/*******************************************************************************
*
* Function:  OffsetMeasuresPhases()
*
*******************************************************************************/

static void  OffsetMeasuresPhases( float *pfVEVphase, 
                                   float *pfCumPhaseOffset,
                                   int iWarpSamples,
                                   float fPitchPeriod,
                                   float *pfVEVphaseSyn,
                                   int iRepeatSamples
                                 )
{
   int   i;
   float fPhi;
   float fPhaseOffset;
   float fRatio;
 
   fRatio = TWOPI/fPitchPeriod;
   *pfCumPhaseOffset += ((float)iWarpSamples)*fRatio;

   fPhaseOffset = *pfCumPhaseOffset + ((float)iRepeatSamples)*fRatio;

   fPhi = 0.0F;
   for (i=SC6_PHASES; i > 0; i--)
   {
      fPhi += fPhaseOffset;
      *pfVEVphaseSyn++ = *pfVEVphase++ + fPhi;
   }
}
