/***********************************************************************
 * 
 * Filename:  CorrPDA.c
 *
 * Purpose:   Calculate cross-correlation related pitch esti-
 *            mator. Then backward / forward track the pitch.
 *           
 * References:(1) "A 5.85kb/s CELP Algorithm For Cellular Application."
 *                 W. Kleijn etc, ICASSP 1993, p.596.
 *            (2) Email from W. Kleijn to B. McAulay, 1995.
 *            (3) ITU-T Draft G.723, p.13.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 04, 1996
 *
 *
 * Modification:  May 5, 1997, W.W. Add higher level function:
 *                VoxTDPDA().
 *
 *                Jan. 20, 1997, W.W. Change to lower complexity, 
 *                  lower memory version.
 *                (1) Asymmetric window for LPC calculation.
 *                (2) Interpolate LSPs instead of calculating LPC every 
 *                     subframe.
 *                (3) Decimate low-passed signal by 2. 
 *                (4) Calculate only the half pitch candidates.
 *                (5) Use auto-correlation for pitch centralization.
 *                (6) Limit memory usage and delay only one frame.
 *                (7) Incorporate Aguilars VAD + make LPC order dependent on PitchDef.h
 *
 * Comments:      reuse speechIn buffer inside of VoxCorrPitchEst() 
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/CorrPDA.c_v   1.12   04 Dec 1998 16:24:40   weiwang  $
 ***********************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "VoxMem.h"
#include "vLibSpch.h"
#include "vLibSnd.h"   /* GetFrameSpCBM() */
#include "vLibVec.h"   /* GetFrameSpCBM() */

#include "CorrPDA.h"
#include "PitchDef.h"
#include "calcLPC.h"
#include "CorrCost.h"
#include "trackPth.h"
#include "CentCorr.h"
#include "NspIIR.h"
#include "hamming.h"
#include "VAD2.h"

#include "xvocver.h"

/*************************************************************
 *
 * Function: VoxCorrPDA()
 *
 * Action:   Calculate subframe based cross-correlation of 
 *           all pitch lags. Do pitch tracking based on two 
 *           previous frames and one future frame.
 *           
 * Input:    speechIn:  preprocessed speech buffer (in short format).
 *           framelen: length of speech.
 *           myCorrPDA_mblk: pitch estimator structure.
 *           pda_delay_size: delay size for encoder.
 *
 * Output:   pitch value of the previous frame.
 *
 *************************************************************/
float VoxCorrPDA(STACK_R float *speechIn, short framelen, 
                 void *pvCorrPDA_mblk, float *pda_delay_size,
                 short *VAD, short *UV)
{
  CorrPDA_mblk *myCorrPDA_mblk = (CorrPDA_mblk *)pvCorrPDA_mblk;
  int i;
  float *tmpSigBuf = speechIn;
  float *ptmpSigBuf = tmpSigBuf;    
  float *pSigLPF;
  float *corr_cost1;
  float bias_floor;
  int coarse_pitch;
  float refine_pitch;
  float maxCost, lowBandEng;
  float CentCorrValue;


  assert(framelen == FRAME_LEN);

#if USE_HPF  == 1
  /*------------------------------------------------------------
    Add a little offset to prevent underflow 
    ------------------------------------------------------------*/
  for (i = 0; i < framelen; i++)
    speechIn[i] += 0.1F;

  /*------------------------------------------------------------
    (1) Preprocessing signal by using high-pass filter.
    ------------------------------------------------------------*/
  VoxNspIIRFlt(ptmpSigBuf, ptmpSigBuf, FRAME_LEN, myCorrPDA_mblk->hpfCoefs,
	       HPF_ORDER, myCorrPDA_mblk->hpf_mem);
#endif


  /* Calculate lowBandEng; needed for VAD estimation.                         */
  lowBandEng = DotProd(ptmpSigBuf, ptmpSigBuf, FRAME_LEN)/(float)FRAME_LEN;

  /*------------------------------------------------------------
    (2) Do LPC analysis and perceptually weight the high-pass 
    filtered signal.
    ------------------------------------------------------------*/
  VoxLPCAnaly4PDA(STACK_S ptmpSigBuf, ptmpSigBuf, FRAME_LEN, myCorrPDA_mblk);


  /*------------------------------------------------------------
    (3) Low pass filter the perceptual weighting signal.
    ------------------------------------------------------------*/
  VoxNspIIRFlt(ptmpSigBuf, ptmpSigBuf, FRAME_LEN, myCorrPDA_mblk->lpfCoefs, 
	       LPF_ORDER, myCorrPDA_mblk->lpf_mem);


  /*------------------------------------------------------------
    (4) Calculate cross correlation and reture the cost function
    of frame (1).
    ------------------------------------------------------------*/
  /* The signal of previous frame has been put to tmpBuf[0:89] and current 
     frame signal is at tmpBuf[90:179].
     So the decimated signal is put to [90:179]. */
  pSigLPF = tmpSigBuf + FRAME_LEN-1;
  ptmpSigBuf = tmpSigBuf + FRAME_LEN-2;
  for (i = LFRAME_LEN; i > 0; i--) {
    *pSigLPF-- = *ptmpSigBuf;
    ptmpSigBuf -= 2;
  }

  /* lpbuf has kept the previous 135 samples signal. 
     The previous frame signal is put to tmpBuf[0:89].*/
  pSigLPF = tmpSigBuf;
  memcpy(pSigLPF, &(myCorrPDA_mblk->lpfbuf[LFRAME_LEN_2]), sizeof(float)*LFRAME_LEN);



  /* Since tmpBuf[0:89] is identical to lpbuf[45:134],
     we borrow tmpBuf[0:89] for returned normalized costs. */
  corr_cost1 = tmpSigBuf;
  VoxCalcCrossCorrCost(STACK_S pSigLPF, corr_cost1, myCorrPDA_mblk, &bias_floor);
  

  /*------------------------------------------------------------
    (5) Backward and forward tracking the cost function to 
    get the coarse pitch value of frame (0).
    ------------------------------------------------------------*/
  coarse_pitch = VoxTrackPitch(STACK_S corr_cost1, myCorrPDA_mblk);


  /*------------------------------------------------------------
    (6) centerize the pitch.
    ------------------------------------------------------------*/
  /* the previous frame: lpfbuf[0:44],
     current frame: lpfbuf[45:134],
     future frame: ptmpSigBuf[90:179] */
  pSigLPF = &(tmpSigBuf[LFRAME_LEN]);
  refine_pitch = calcCenterCorr(STACK_S myCorrPDA_mblk,	pSigLPF, coarse_pitch, 
				&maxCost );


  /* update the costs value of current frame */
  if (maxCost > myCorrPDA_mblk->cost_1)
    myCorrPDA_mblk->cost_1 = maxCost;

  /* check if maxCost is too small */
  if (maxCost <= 0.01F) {
    myCorrPDA_mblk->cost_1 = INIT_COST;
    myCorrPDA_mblk->pitch_1 = myCorrPDA_mblk->pitch_2;
    refine_pitch = (float)myCorrPDA_mblk->pitch_2;
  }
   
 
  /* update the signal buffer */
  memcpy(myCorrPDA_mblk->lpfbuf, &(myCorrPDA_mblk->lpfbuf[LFRAME_LEN]), LFRAME_LEN_2*sizeof(float));
  memcpy(&(myCorrPDA_mblk->lpfbuf[LFRAME_LEN_2]), pSigLPF, LFRAME_LEN*sizeof(float));


  /* get the pitch delay size */
  *pda_delay_size = (float)(FRAME_LEN+LFRAME_LEN);

  /* Calculate Center Correlation Value for voiceActivity Function */
  CentCorrValue=maxCost/myCorrPDA_mblk->biasTab[(int)(refine_pitch+0.5F)-LPITCH_MIN];

  VoiceActivity(myCorrPDA_mblk->VADMemBlk, lowBandEng, CentCorrValue, VAD, UV);

  return (2.0F*refine_pitch);
}



/***********************************************************************
 * Function: initCorrPitchEst()
 *
 * Input:    myCorrPDA_mblk:  pointer for the structure.
 *
 * Output:   error code:   error with initialization.
 *           0:            no error
 ***********************************************************************/

static const float LpfCoefs2k[2*LPF_ORDER+1] = {
0.13209632758491F, 0.16441597660485F, 0.16441597660485F, 0.13209632758491F,
-1.00457576781968F, 0.75391007092870F,  -0.15630969472949F
};

#if USE_HPF == 1
static const float HpfCoefs100[2*HPF_ORDER+1] = {
0.89669841138219F, -3.58456196150319F,
5.37572784022113F, -3.58456196150319F, 0.89669841138219F,
-3.79077226063959F, 5.39682474557460F, -3.41970282566249F,
0.81372436653670F
};
#endif

unsigned short VoxInitCorrPitchEst(void **hCorrPDA_mblk)
{
  CorrPDA_mblk *myCorrPDA_mblk;
  float *pbias;
  float bias0;
  int i;
  int *pTabStart, *pTabLength;
  int delta;
  float x1;

  /* initialize the structure */
  if(VOX_MEM_INIT(myCorrPDA_mblk=(CorrPDA_mblk *)*hCorrPDA_mblk,1,
     sizeof(CorrPDA_mblk)) != 0)
	  return 1;

  /*------------------------------------------------------------
    initialize the inter-frame memory 
    ------------------------------------------------------------*/
#if USE_HPF == 1
  if(VOX_MEM_INIT(myCorrPDA_mblk->hpf_mem, HPF_ORDER, sizeof(float))!=0) 
    return 1;
#endif

  if(VOX_MEM_INIT(myCorrPDA_mblk->lpf_mem, LPF_ORDER, sizeof(float))!=0) 
    return 1;
 
  if(VOX_MEM_INIT(myCorrPDA_mblk->lpfbuf, (LFRAME_LEN+LFRAME_LEN_2), sizeof(float))!=0)
    return 1;

  if(VOX_MEM_INIT(myCorrPDA_mblk->costs0, (PITCH_CAND), sizeof(float))!=0)
    return 1;

  for (i = 0; i < PITCH_CAND; i++)
    myCorrPDA_mblk->costs0[i] = INIT_COST;

  if(VOX_MEM_INIT(myCorrPDA_mblk->Alsf0, PDA_LPC_ORDER, sizeof(float))!=0) 
    return 1;
  /* initialize Alsf0 */
  for (i = 0, x1=0.0F; i < PDA_LPC_ORDER; i++) 
    myCorrPDA_mblk->Alsf0[i] = x1 = (float)(ZERO_LSF + x1);
  

  if (VOX_MEM_INIT(myCorrPDA_mblk->percWeight_mem, PDA_LPC_ORDER, sizeof(float))!=0) 
    return 1;  

  myCorrPDA_mblk->pitch_1 = INIT_PITCH;
  myCorrPDA_mblk->pitch_2 = INIT_PITCH;

  myCorrPDA_mblk->cost_1 = INIT_COST;
  myCorrPDA_mblk->cost_2 = INIT_COST;


  /*------------------------------------------------------------
    Tables:  pre-caculated for general application 
    or load-in to ROM for DSP application 
    ------------------------------------------------------------*/
  myCorrPDA_mblk->lpfCoefs = (float *)LpfCoefs2k;

#if USE_HPF == 1
  myCorrPDA_mblk->hpfCoefs = (float *)HpfCoefs100;
#endif

  /* calculate the asymmetric Hamming window */
  if (VOX_MEM_INIT(myCorrPDA_mblk->LpcWindow, FRAME_LEN, sizeof(float))!=0) 
    return 1;

  hamming_half(myCorrPDA_mblk->LpcWindow, 2*LEFT_ASYM_WIN);
  hamming_right(myCorrPDA_mblk->LpcWindow+LEFT_ASYM_WIN, 2*RIGHT_ASYM_WIN);

  /* calculate the bias table */
  if(VOX_MEM_INIT(myCorrPDA_mblk->biasTab,PITCH_CAND,sizeof(float))!=0) 
     return 1;

  pbias = myCorrPDA_mblk->biasTab;
  bias0 = (float)pow((double)LPITCH_MIN, (double)BIAS_C0);   /*   *pbias++ = 1.0 - (float)i * BIAS_LAG;  */
  *pbias++ = 1.0F;
  for (i = LPITCH_MIN+1; i <= LPITCH_MAX; i++)
    *pbias++ = (float)(bias0/pow((double)i, (double)BIAS_C0));   /*   *pbias++ = 1.0 - (float)i * BIAS_LAG;  */


  /* calculate the tables for intra-frame tracking */
  if(VOX_MEM_INIT(myCorrPDA_mblk->dIntraLagStart, PITCH_CAND, sizeof(int))!=0) 
     return 1;
  
  if(VOX_MEM_INIT(myCorrPDA_mblk->dIntraLagLength, PITCH_CAND, sizeof(int))!=0) 
     return 1;
  pTabStart =  myCorrPDA_mblk->dIntraLagStart;
  pTabLength = myCorrPDA_mblk->dIntraLagLength;

  for (i = 0; i < PITCH_CAND; i++) {
    delta = (short)((float)(i+LPITCH_MIN)*INTRA_FRAME_TRACK_RATIO+0.5);

    *pTabStart = i-delta;
    if (*pTabStart < 0)
      *pTabStart = 0;

    *pTabLength = i+delta;
    if (*pTabLength >= PITCH_CAND)
      *pTabLength = PITCH_CAND-1;
    *pTabLength++ -= (*pTabStart++ - 1);
  }


  /* calculate the tables for inter-frame tracking */
  if(VOX_MEM_INIT(myCorrPDA_mblk->dInterLagStart, PITCH_CAND, sizeof(int))!=0) 
     return 1;
  
  if(VOX_MEM_INIT(myCorrPDA_mblk->dInterLagLength, PITCH_CAND, sizeof(int))!=0) 
     return 1;
  pTabStart = myCorrPDA_mblk->dInterLagStart;
  pTabLength = myCorrPDA_mblk->dInterLagLength;

  for (i = 0; i < PITCH_CAND; i++) {
    delta = (short)((float)(i+LPITCH_MIN)*INTER_FRAME_TRACK_RATIO+0.5);

    *pTabStart = i-delta;
    if (*pTabStart < 0)
      *pTabStart = 0;

    *pTabLength = i+delta;
    if (*pTabLength >= PITCH_CAND)
      *pTabLength = PITCH_CAND-1;
    *pTabLength++ -= (*pTabStart++ - 1);
  }
  if(initVAD(&(myCorrPDA_mblk->VADMemBlk)))
     return 1;

  return 0;
}


/***********************************************************************
 * Function: freeCorrPitchEst()
 *
 * Input:    myCorrPDA_mblk:   pointer for the structure.
 *
 * Output:   None
 ***********************************************************************/

unsigned short VoxFreeCorrPitchEst(void **hCorrPDA_mblk)
{
  CorrPDA_mblk *myCorrPDA_mblk=(CorrPDA_mblk *)*hCorrPDA_mblk;
  freeVAD(&(myCorrPDA_mblk->VADMemBlk));

  if(*hCorrPDA_mblk) {
#if USE_HPF == 1
    VOX_MEM_FREE(myCorrPDA_mblk->hpf_mem);
#endif

    VOX_MEM_FREE(myCorrPDA_mblk->lpf_mem);

    VOX_MEM_FREE(myCorrPDA_mblk->lpfbuf);

    VOX_MEM_FREE(myCorrPDA_mblk->costs0);

    VOX_MEM_FREE(myCorrPDA_mblk->Alsf0);

    VOX_MEM_FREE(myCorrPDA_mblk->percWeight_mem);

    VOX_MEM_FREE(myCorrPDA_mblk->LpcWindow);
    
    VOX_MEM_FREE(myCorrPDA_mblk->biasTab);

    VOX_MEM_FREE(myCorrPDA_mblk->dInterLagStart);

    VOX_MEM_FREE(myCorrPDA_mblk->dInterLagLength);

    VOX_MEM_FREE(myCorrPDA_mblk->dIntraLagStart);

    VOX_MEM_FREE(myCorrPDA_mblk->dIntraLagLength);
  
    VOX_MEM_FREE(*hCorrPDA_mblk);
  }

  return 0;
}


#define  MAGIC1 0.65F
#define  MAGIC2 0.6F
#define  MAGIC3 0.1F
#define  MAGIC4 0.16F
#define  MAGIC5 0.4F
#define  MAGIC6 0.4F
#define  MAGIC7 0.4F
#define  MAGIC8 0.60F

/*************************************************************
 *
 * Function: PvRefine()
 *
 * Action:   Refines Pv according to some black magic.
 *           
 * Input:    Pv           - Voicing for the current frame
 * Input:    Pv_1         - Refined Voicing for the last frame
 *           MaxCost      - biased pitch correlation
 *           MaxCostShift - shift of
 * Output:   Return refined Pv.
 *
 *************************************************************/


float PvRefine( float Pv, float Pv_1, float Pitch, float MaxCost, float *biasTab)
{
   float CentCorrValue;
   float RefinedPv;

   CentCorrValue = MaxCost/biasTab[(int)(Pitch+0.5F)-LPITCH_MIN];
   if (CentCorrValue < 0.0F)
      CentCorrValue = 0.0F;

   if (CentCorrValue > 1.0F)
      CentCorrValue = 1.0F;


   RefinedPv = Pv;
   if ((CentCorrValue>MAGIC2)&&(Pv>MAGIC3))
   {
      RefinedPv = (float) (Pv+CentCorrValue)* ((float) 0.5);
   }
   else if ((Pv*CentCorrValue < MAGIC4)&&(CentCorrValue<MAGIC8))
   {
      if (Pv_1 < MAGIC5)
         RefinedPv =0;
   }
   else if ((Pv > MAGIC6)&&(CentCorrValue>MAGIC7))
   {     
      RefinedPv = (float) sqrt(Pv);
   }
   
   return(RefinedPv);

}


