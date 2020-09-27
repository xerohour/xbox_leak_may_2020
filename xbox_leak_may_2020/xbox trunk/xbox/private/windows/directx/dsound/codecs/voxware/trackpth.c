/***********************************************************************
 *
 * Filename:  trackPth.c
 *
 * Purpose:  tracking the pitch based on one future frame and one or two
 *           previous frames. And make the decision.
 *           
 * Reference:  (1) ITU-T Draft G.723, p.13.
 *             (2) Inmarsat-M Voce Coding System Description, p15-17.
 *
 * Author:     Wei Wang
 *
 * Date:       Oct. 08, 1996
 *
 * Modification: modified for decimated signal. When pitch is too small,
 *               the cost values of double pitch and triple pitch are checked.
 *               If they are too small, the second pitch candidate has to be 
 *               selected.  1/21/97, Wei Wang.
 * 
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/trackPth.c_v   1.6   12 May 1998 12:55:16   weiwang  $
 ***********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "vLibVec.h" /* FindMaxValueFlt() */

#include "VoxMem.h"

#include "CorrPDA.h"
#include "PitchDef.h"

#include "trackPth.h"

#include "xvocver.h"

static void LookForward(STACK_R float *pcost1, CorrPDA_mblk *myCorrPDA_mblk, 
                        float *pcost_f, short *pp0_f, short *tooSmall_flag);

static void LookBackward(CorrPDA_mblk *myCorrPDA_mblk, float *pcost_b, short *pp0_b);


/*************************************************************
 * Fucntion:   trackPitch()
 *
 * Action:    do forward and backward pitch tracking using 
 *            biased cost function.
 *
 * Input:     myPDATrack_mblk:  pointer for pitch tracking.
 *
 * Output:    pitch value of frame 0.
 *************************************************************/

short VoxTrackPitch(STACK_R float *costs1, CorrPDA_mblk *myCorrPDA_mblk)
{
  float cost_f, cost_b;
  short p0_f, p0_b;
  short pitch;
  short tooSmall_flag = 0;

  /*--------------------------------------------------
    forward tracking :
    --------------------------------------------------*/
  LookForward(STACK_S costs1, myCorrPDA_mblk, &cost_f, &p0_f, &tooSmall_flag);


  /*-------------------------------------------------- 
    backward tracking :
    find the maximum costs while tracking.
    --------------------------------------------------*/
  LookBackward(myCorrPDA_mblk, &cost_b, &p0_b);

  if (tooSmall_flag == 1) {
    /* need check p0_b to see if pitch candidate is not reliable */
    if (p0_b <= LPITCH_MIN_CONF && cost_b < 0.5F) {
      cost_b = INIT_COST;
    }
  }

  /*-----------------------------------------------------------------
    make the decision to choose from backward pitch or forward pitch.
    -----------------------------------------------------------------*/
  if (((p0_b>p0_f)&&(cost_f<=BIAS_FORWARD*cost_b))
      ||((p0_b <= p0_f) && ( (cost_b >= (BIAS_BACKWARD * cost_f)))))
    pitch = p0_b;
  else 
    pitch = p0_f;


  /*------------------------------------------------------------
    Save the tracking information.
    ------------------------------------------------------------*/
  myCorrPDA_mblk->pitch_2 = myCorrPDA_mblk->pitch_1;
  myCorrPDA_mblk->pitch_1 = pitch;

  myCorrPDA_mblk->cost_2 = myCorrPDA_mblk->cost_1;
  myCorrPDA_mblk->cost_1 = myCorrPDA_mblk->costs0[pitch-LPITCH_MIN];

  memcpy(myCorrPDA_mblk->costs0, costs1, PITCH_CAND*sizeof(float));


  return pitch;
}





/*--------------------------------------------------
  forward tracking :
  --------------------------------------------------*/
static void LookForward(STACK_R float *pcost1, CorrPDA_mblk *myCorrPDA_mblk, 
                        float *pcost_f, short *pp0_f, short *tooSmall_flag)
{
  STACK_INIT
  float pforward_costs[3*PITCH_CAND];
  STACK_INIT_END
  STACK_ATTACH(float*,pforward_costs)
  float *pcost0 = myCorrPDA_mblk->costs0;
  int *dLagStart = myCorrPDA_mblk->dInterLagStart;
  int *dLagLength = myCorrPDA_mblk->dInterLagLength;
  float cost_f;
  int p0_f;
  int jlag;
  float forcosts;

  float tmpmaxcost;
  int tmpmaxlag = -1;
  float tmpcost2, tmpcost3;

  STACK_START

  /*--------------------------------------------------
    calculate the forward cost function.
    --------------------------------------------------*/
  {
    int k, length, kend=0;
    float tmpmaxcost1;
    int tmpmaxlag1;

    cost_f = INIT_COST;
    for (jlag = 0; jlag < PITCH_CAND; jlag++) {
      k = *dLagStart++;
      length = k+ *dLagLength-kend;
      if(tmpmaxlag < k) {
	FindMaxValueFlt(&(pcost1[k]), *dLagLength, &tmpmaxlag, &tmpmaxcost);
	tmpmaxlag += k;
      }
      else if(length) {
	assert(kend < PITCH_CAND);
	tmpmaxcost1 = pcost1[kend];
	tmpmaxlag1 = kend;
	if(length == 2) {
	  assert( kend+1 < PITCH_CAND);
	  if(tmpmaxcost1 < pcost1[kend+1]) {
	    tmpmaxcost1 = pcost1[kend+1];
	    tmpmaxlag1 ++;
	  }
	}
	if(tmpmaxcost1>tmpmaxcost) {
	  tmpmaxlag = tmpmaxlag1;
	  tmpmaxcost = tmpmaxcost1;
	}
      }
      kend = k + *dLagLength++;
      forcosts = 0.5F * (tmpmaxcost + *pcost0++);
      if (forcosts > cost_f) {
	cost_f = forcosts;
	p0_f = jlag;
      }
      pforward_costs[jlag] = forcosts;
    }
  }

  p0_f += LPITCH_MIN;


  /*--------------------------------------------------
    To prevent pick too small pitch as the candidates,
    If p0_f is [PITCH_MIN:PITCH_MIN+1], we have to check
    the costs of 2*p0_f and 3*p0_f. If all of them are 
    very large, then we keep this candidates. Otherwise,
    we have to find the largest cost from 3*p0_f+1:
    PITCH_MAX.
    --------------------------------------------------*/
  if (p0_f <= LPITCH_MIN_CONF) {
    jlag = 2*p0_f-LPITCH_MIN;
    tmpcost2 = pforward_costs[jlag];
    jlag += p0_f;
    tmpcost3 = pforward_costs[jlag];
    if ((tmpcost2 < 0.5F*cost_f) || (tmpcost3 < 0.5F*cost_f)) {
      /* we have to find the new forward tracking candidates */
      jlag++;
      FindMaxValueFlt(&(pforward_costs[jlag]), (PITCH_CAND-jlag),
		      &tmpmaxlag, &cost_f);
      p0_f = tmpmaxlag + jlag + LPITCH_MIN;

      *tooSmall_flag = 1;
    }
  }
  
  *pp0_f = (short) p0_f;
  *pcost_f = cost_f;
  STACK_END
}


/*-------------------------------------------------- 
  backward tracking :
  --------------------------------------------------*/
static void LookBackward(CorrPDA_mblk *myCorrPDA_mblk, float *pcost_b, short *pp0_b)
{
  float *pcost0 = myCorrPDA_mblk->costs0;
  short lag_1, lag_2;
  int *dLagStart = myCorrPDA_mblk->dInterLagStart;
  int *dLagLength = myCorrPDA_mblk->dInterLagLength;
  float tmpcost;
  float tmpmaxcost;
  int tmpmaxlag;
  float cost_b;
  short p0_b;
  short k;

  /*--------------------------------------------------
    find out the backward candidate.
    --------------------------------------------------*/
  lag_1 = (short)(myCorrPDA_mblk->pitch_1 - LPITCH_MIN);
  k = (short) dLagStart[lag_1];
  FindMaxValueFlt(&(pcost0[k]), dLagLength[lag_1], &tmpmaxlag, &tmpmaxcost);

  p0_b = (short)(tmpmaxlag + k + LPITCH_MIN);
  cost_b = (tmpmaxcost + myCorrPDA_mblk->cost_1) * 0.5F; 

  
  /*--------------------------------------------------
    lookback to see if the previous previous frame is under
    tracking.
    --------------------------------------------------*/
  lag_2 = (short)(myCorrPDA_mblk->pitch_2 - LPITCH_MIN);
  k = (short) dLagStart[lag_2];

  if (lag_1>=k && lag_1< (k+dLagLength[lag_2])) {
    tmpcost = (float)(myCorrPDA_mblk->cost_2*0.25F + myCorrPDA_mblk->cost_1*0.35F
      + tmpmaxcost*0.4F);
    if (tmpcost > cost_b)
      cost_b = tmpcost;
  } /* end of "if (lag_1 ... " */

  *pcost_b = cost_b;
  *pp0_b = p0_b;
}


