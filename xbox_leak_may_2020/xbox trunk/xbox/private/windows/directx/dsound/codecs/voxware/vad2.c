/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1998, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          VAD.c                                                         
*                                                                              
* Purpose:           Voice Activity Detector                                                          
*                                                                              
* Functions:         VAD(), initVAD(), freeVAD()                                                          
*                                                                              
* Author/Date:       Rob Zopf/10/09/98 - Code based on JGA                                                          
********************************************************************************
* Modifications:
*                   
* Comments:                                                                      
* 
* Concerns:
*
* 
*******************************************************************************/
#define WRITE_OUT  0
#if (WRITE_OUT==1)
#include <stdio.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "vLibMath.h"
#include "VoxMem.h"
#include "VAD2.h"

#include "xvocver.h"

/******************************************************************************
                P R I V A T E   V A D   M E M   B L O C K 
 =============================================================================*/
typedef struct
{
   float dBFrameEnergy;       /* Future frame energy in dB                    */
   short SkippedFrames;       /* Used to skip the first few frames from VAD   */
   float NominalVoiced;       /* Voiced Energy Estimate in dB                 */
   float NominalBckGrnd;      /* Background Energy Estimate in dB             */
   short VADHangoverSoFar;    /* Running Hangover Frame Total                 */
   short VBHangoverSoFar;     /* Running Voiced Background Hangover Total     */
   short PrevVAD;             /* Last VAD Decision                            */
   float sbnr;                /* signal-to-backgroundnoise-ratio              */

} VAD_MemBlk;

/******************************************************************************/


/******************************************************************************
                         V A D   # D E F I N E S 
 =============================================================================*/
#define FRAME_RATE      (1.0F/0.0225F)
                                 /* Frames per Second (needed for decay rates */

#define MAX_PITCH_CORR  (1.0F)   /* can use to tune thresholds if input
                                    correlation is funny                      */

#define VOICED_TH       (MAX_PITCH_CORR*0.7F)
                                 /* frame is assumed to be voiced if corr at  */
                                 /* the pitch lag is greater than this        */

#define BCKGRND_TH      (MAX_PITCH_CORR*0.5F)
                                 /* frame is assumed to be background noise   */
                                 /* or unvoiced if the correlation is less    */

#define VOICED_DEC      (5.0F/FRAME_RATE)
                                 /* Decrement the nominal voiced frame energy */
                                 /* estimate my this much each voiced frame   */

#define BCKGRND_INC     (44.4444F/FRAME_RATE)
                                 /* Increment the nominal background noise    */
                                 /* estimate each frame by this amount        */

#define SKIP_FRAMES     4        /* Skip these frames to avoid setting BckGrnd*/
                                 /* estimate to zero at the start of the file */

#define VAD_TH          0.25F    /* VAD Threshold (will be multiplied by the  */
                                 /* difference between the voiced and bckgrnd */
                                 /* energy estimates                          */

#define VAD_ON_TRIGGER  0.05F

#define VAD_OFF_TRIGGER 0.05F

#define SILENCE_TH      10.0F    /* lower than this in dB is silence for sure */

#define MIN_SNR_TH      15.0F    /* Maintain at least this SNR estimate       */

#define INIT_VOICED_ESTIMATE  70.0F 
                                 /* dB initial setting                        */

#define INIT_BCKGRND_ESTIMATE 50.0F
                                 /* dB initial setting                        */

#define VOICED_BCKGRND_THRES  10.0F
                                 /* dB                                        */

#define VOICED_CORR_THRES     (MAX_PITCH_CORR*0.513F)
                                 /* correlation threshold to determine if     */
                                 /* background noise is background speech     */

#define VOICED_BCKGRND_HANGOVER 4
                                 /* hangover frames for voiced background flag*/

#define SBNR_UPDATE_RATE      0.85F

#define MAX_HANGOVER          40
#define SBNR_AT_MAX           15.0F

#define MIN_HANGOVER          10
#define SBNR_AT_MIN           35.0F

#define SLOPE                 ((MIN_HANGOVER-MAX_HANGOVER)/((float)(SBNR_AT_MIN-SBNR_AT_MAX)))
#define INTERCEPT             (MAX_HANGOVER-(SLOPE*SBNR_AT_MAX))

#define SBNR2HANGOVER         0.666F

/******************************************************************************/


/*******************************************************************************
* Function: VoiceActivity()
*
* Action: Perform Voice Activity Detection
*
* Input: *hVADMemBlk       - the VAD memory block 
*        FutureFrameEnergy - the energy of the next frame (the VAD uses a 
*                            one frame lookahead to avoid misclassifying onsets)
*        PitchCorr         - the normalized autocorrelation at the pitch lag 
*                            for the current frame.
*
* Output:
*        *VAD              - the activity flag : active speech =  ACTIVE_SPEECH
*                                                background    = !ACTIVE_SPEECH
*        *BckgrndSpeech    - a flag that signals the presence of possible 
*                            speech in the background noise.  This flag could
*                            be used to set Pv to zero, or be used to control
*                            the mode of a variable rate codec.
*                                                bckgrnd speech=  ACTIVE_SPEECH
*                                                non-speech    = !ACTIVE_SPEECH 
*
* Globals:      none
*
* Return:	none
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void VoiceActivity(void *hVADMemBlk, float FutureFrameEnergy, float PitchCorr, short *VAD,
         short *BckgrndSpeech)
{
   VAD_MemBlk *VADMemBlk;
   float       dBFutureFrameEnergy;
   float       dBCurrentFrameEnergy;
   float       VADThreshold;
   float       sbnr;                   /* signal-to-background-noise ratio    */
   short       FrameVAD;               /* Instantaneous VAD decision for frame*/

   /*------------------------------------------------
     Initialize Variables
     ------------------------------------------------*/
   VADMemBlk                = (VAD_MemBlk *)hVADMemBlk;
   dBCurrentFrameEnergy     = VADMemBlk->dBFrameEnergy;
   dBFutureFrameEnergy      = 10.0F*(float)log10(FutureFrameEnergy); /* try use RL later */
   VADMemBlk->dBFrameEnergy = dBFutureFrameEnergy;

   /*------------------------------------------------
      Update Nominal Voiced Energy Estimate.  
      ===============================================
      
      The energy estimate is only updated when the 
      pitch correlation is greater than a threshold
      indicating that the frame is most likely voiced.

      The estimate is first decreased slightly, and 
      then compared to the current frame energy to 
      see which is greater.
     ------------------------------------------------*/
   if (PitchCorr > VOICED_TH)
   {
      VADMemBlk->NominalVoiced -= VOICED_DEC;
      if (dBCurrentFrameEnergy > VADMemBlk->NominalVoiced)
         VADMemBlk->NominalVoiced = dBCurrentFrameEnergy;
   }
   /*------------------------------------------------
      Update Nominal Background Noise Energy Estimate.  
      ===============================================
      
      The energy estimate is only updated when the 
      pitch correlation is less than a threshold
      indicating that the frame is most likely 
      background noise or unvoiced.

      The estimate is first increased slightly, and 
      then compared to the current frame energy to 
      see which is less.

      Skip the first few frames for file robustness
      and initial lookahead energy.
     ------------------------------------------------*/
   if (VADMemBlk->SkippedFrames > SKIP_FRAMES)
   {
      if (PitchCorr < BCKGRND_TH)
      {
         VADMemBlk->NominalBckGrnd += BCKGRND_INC;
         if ((dBCurrentFrameEnergy < VADMemBlk->NominalBckGrnd)&&
            (dBCurrentFrameEnergy > SILENCE_TH))
         {
            VADMemBlk->NominalBckGrnd = dBCurrentFrameEnergy;
         }
      }
   }

   /*-------------------------------------------------
      Estimate the signal-to-background-noise energy
      ratio.
   ---------------------------------------------------*/
   sbnr = VADMemBlk->sbnr*SBNR_UPDATE_RATE + 
         (VADMemBlk->NominalVoiced - VADMemBlk->NominalBckGrnd)*
         (1.0F-SBNR_UPDATE_RATE);


   /*-------------------------------------------------
      Calculate the VAD Energy Threshold in dB 
      based on the estimated nominal voiced energy 
      level and the estimated nominal background noise
      energy level.
   ---------------------------------------------------*/
   if (VADMemBlk->PrevVAD==ACTIVE_SPEECH)
      VADThreshold = (sbnr*(VAD_TH-VAD_ON_TRIGGER)) + VADMemBlk->NominalBckGrnd;
   else
      VADThreshold = (sbnr*(VAD_TH+VAD_OFF_TRIGGER))+ VADMemBlk->NominalBckGrnd;


   /*-------------------------------------------------
      Maintain a Minimum SNR for VAD operation
   ---------------------------------------------------*/
   if (VADMemBlk->SkippedFrames > SKIP_FRAMES)
   {
      if (sbnr < MIN_SNR_TH)
      {
         VADMemBlk->NominalBckGrnd = VADMemBlk->NominalVoiced - MIN_SNR_TH;
      }
   }


   /*--------------------------------------------------
      Make VAD decision on current frame based on 
      frame energy for current frame and future frame.
   ----------------------------------------------------*/
   if ((dBCurrentFrameEnergy > VADThreshold)||(dBFutureFrameEnergy > VADThreshold))
   {
      FrameVAD = ACTIVE_SPEECH;
   }
   else
   {
      FrameVAD = !ACTIVE_SPEECH;
   }

   *VAD = FrameVAD;
   if (FrameVAD!=ACTIVE_SPEECH)
   {
      if (VADMemBlk->PrevVAD==ACTIVE_SPEECH)
      {
      /*---------------------------------------------------
         Set the hangover for this frame based on the 
         estimated signal-to-background-noise energy ratio.
         Use a simple 1st order equation to map lower sbnr
         with a greater hangover.
      -----------------------------------------------------*/
         if (sbnr <= SBNR_AT_MAX)
            VADMemBlk->VADHangoverSoFar = MAX_HANGOVER;
         else if (sbnr >= SBNR_AT_MIN)
            VADMemBlk->VADHangoverSoFar = MIN_HANGOVER;
         else
            VADMemBlk->VADHangoverSoFar = (short)(SLOPE*sbnr+INTERCEPT);      
      }
      VADMemBlk->VADHangoverSoFar--;
      if (VADMemBlk->VADHangoverSoFar > 0)
         *VAD = ACTIVE_SPEECH;
      else
         VADMemBlk->VADHangoverSoFar = 0;
   }    

   /*------------------------------------------------------
      Background Speech Signal Flag Logic

      The Background Speech Flag can be used by the codec 
      to improve synthesis of harmonic like background noise.  
      This could include babble or interfering talker.  Here,
      we just basically look at the correlation and if it
      is high, make the assumption that the background 
      noise is some sort of voiced speech, which would 
      sound better if synthesized with a non-zero Pv.

      This really should be part of Pv and should not be
      here.
   ---------------------------------------------------------*/
   *BckgrndSpeech = FrameVAD;
    if ((sbnr > VOICED_BCKGRND_THRES)&&(PitchCorr > VOICED_CORR_THRES))
      *BckgrndSpeech = ACTIVE_SPEECH;

   /* Some hangover for the decision */
   if (*BckgrndSpeech == ACTIVE_SPEECH)
   {
      VADMemBlk->VBHangoverSoFar = 0;
   }
   else
   {
      if (VADMemBlk->VBHangoverSoFar <= VOICED_BCKGRND_HANGOVER)
      {
         *BckgrndSpeech = ACTIVE_SPEECH;
      }

      VADMemBlk->VBHangoverSoFar++;
      if (VADMemBlk->VBHangoverSoFar > VOICED_BCKGRND_HANGOVER+1)
      {
         VADMemBlk->VBHangoverSoFar = VOICED_BCKGRND_HANGOVER+1;
      }
   }

   VADMemBlk->SkippedFrames++;
   if (VADMemBlk->SkippedFrames > SKIP_FRAMES)
   {
      VADMemBlk->SkippedFrames = SKIP_FRAMES+1;
   }

   VADMemBlk->PrevVAD = FrameVAD;
   VADMemBlk->sbnr    = sbnr;

#if (WRITE_OUT==1)
{
static FILE *f1, *f2;
static int init=0;
short tempbuf[180];
int i;

   if (init==0)
   {
      init=1;
      f1 = fopen("VOICED_NEW.out", "w");
      f2 = fopen("VAD_NEW.out", "w");
   }
   for (i=0;i<180;i++)
      tempbuf[i] = *BckgrndSpeech;

   fwrite(tempbuf, sizeof(short), 180, f1);

   for (i=0;i<180;i++)
      tempbuf[i] = *VAD;

   fwrite(tempbuf, sizeof(short), 180, f2);

}

#endif

}


/*******************************************************************************
* Function: initVAD()
*
* Action: initialize the VAD memory block
*
* Input: **hVADMemBlk -  handle to the memory block
*
* Output: *hVADMemBlk -  pointer to the allocated and initialized memory
*
* Globals:        none
*
* Return:	unsigned short : 0 = successful memory allocation
*                                1 = failed memory allocation
*               
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short initVAD(void **hVADMemBlk)
{
   VAD_MemBlk  *VADMemBlk;

   if (VOX_MEM_INIT(VADMemBlk = *hVADMemBlk, 1, sizeof(VAD_MemBlk)) != 0)
   {
      return 1;
   }
   VADMemBlk->NominalVoiced    = INIT_VOICED_ESTIMATE;
   VADMemBlk->NominalBckGrnd   = INIT_BCKGRND_ESTIMATE;
   VADMemBlk->dBFrameEnergy    = INIT_BCKGRND_ESTIMATE;
   VADMemBlk->SkippedFrames    = 0;
   VADMemBlk->VBHangoverSoFar  = 0;
   VADMemBlk->VADHangoverSoFar = 0;
   VADMemBlk->PrevVAD          = ACTIVE_SPEECH;
   VADMemBlk->sbnr             = INIT_VOICED_ESTIMATE-INIT_BCKGRND_ESTIMATE;

   return 0;
}

/*******************************************************************************
* Function: freeVAD()
*
* Action: free the allocated memory used by the VAD
*
* Input: **hVADMemBlk - handle to the VAD memory
*
* Output:
*
* Globals:      none
*
* Return:	void
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
void freeVAD(void **hVADMemBlk)
{
   VOX_MEM_FREE(*hVADMemBlk);
}
