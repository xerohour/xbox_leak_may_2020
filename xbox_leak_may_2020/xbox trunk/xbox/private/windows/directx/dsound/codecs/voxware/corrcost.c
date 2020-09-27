/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/***********************************************************************
 *
 * File: CorrCost.c
 *
 * Purpose: Calculate cross correlator based pitch cost function. Use
 *          subframes instead of full frame. Then dynamicly align the
 *          correlation cost values and normalize them.
 *
 * Reference: (1) "A 5.85kb/s CELP Algorithm For Cellular Application."
 *                 W. Kleijn etc, ICASSP 1993, p.596.
 *            (2) Email from W. Kleijn to B. McAulay, 1995.
 *            (3) Bias function from B. McAulay.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 08, 1996.
 *
 * History:  
 *
 *   --   Rewrite the code for decimated signal.  Wei Wang, 01/22/97.
 *
 *   --   Optimized in C level, get rid of warnings, Xiangdong Fu 12/11/96
 *       
 *   --   Add bias function as STC PDA.  Oct. 25, 1996.   Wei Wang.
 *   --   Split to a lot of small functions.  Nov. 11-13, 1996,  Wei Wang.
 *   --   Interpolate the cost value when pitch is above 100 samples.
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/CorrCost.c_v   1.5   12 May 1998 12:55:04   weiwang  $
 ***********************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "VoxMem.h"
#include "vLibSpch.h" /* QuadraticPeak()              */
#include "vLibVec.h"  /* FindMaxValueFlt(), DotProd() */

#include "CorrPDA.h"
#include "PitchDef.h"
#include "CorrCost.h"

#include "xvocver.h"

static void CalcSubCross(float *sigRef, float *psubcosts);
static void CalcEnergy(float *sigRef, float *EngRef);
static int FindRefFrame(float *EnergyRef, float *frameEnergy);
static void CalcFrameCosts(float *normcost, int ref_Frame, float frameEnergy, 
			   int *dLagStart, int *dLagLength, float *EnergyRef, 
			   float *psubcosts);
static void BiasCosts(float *costs, float *biasTab, float *bias_floor);

/***********************************************************************
 * Function: calcCrossCorrCost()
 *
 * Action:   calculate cross-orrelated cost function using decimated signal.
 *
 * Input:    sigBuf:  signal buffer (from 0:LPITCH_MAX+LFRAME_LEN-1)
 *           normcost:  normalization costs (output)         
 *           myCorrPDA_mblk: correlation related structure.
 *
 * Output:   None.
 ***********************************************************************/

void VoxCalcCrossCorrCost(STACK_R float *sigBuf, float *normcost, 
		       CorrPDA_mblk *myCorrPDA_mblk, float *pbias_floor)
{
  STACK_INIT
  float subcosts[3*PITCH_CAND];
  float EnergyRef[LPITCH_MAX+LFRAME_LEN-LSUBFRAME_LEN+1];
  STACK_INIT_END
  STACK_ATTACH(float*,subcosts)
  STACK_ATTACH(float*,EnergyRef)
  float *sigRef;
  float frameEnergy;
  int ref_Frame;
  float *pEnergyRef;
  float fSlope, fDSlope;
  int i;

  STACK_START

  assert(LPITCH_MAX <= LFRAME_LEN);

  /*--------------------------------------------------
    Window the decimated signal: keep the current frame
    signal and decay the previous frame signal.
    --------------------------------------------------*/
  fSlope = PDA_WIN_COEF;
  fDSlope = PDA_WIN_DIFFC;
  for (i = 0; i < LFRAME_LEN; i++) {
    sigBuf[i] *= fSlope;
    fSlope += fDSlope;
  }

  /*------------------------------------------------------------
    reference signal:sigBuf[LFRAME_LEN ... LFRAME_LEN+LFRAME_LEN].
    ------------------------------------------------------------*/
  sigRef = sigBuf+LFRAME_LEN;

  /*--------------------------------------------------
    calculate the cross correlation of subframes.
    --------------------------------------------------*/
  CalcSubCross(sigRef, subcosts); 


  /*--------------------------------------------------
    calculate the energy of subframes and the cross-
    referenced energy.
    I calculate from backward.
    --------------------------------------------------*/
  CalcEnergy(sigRef, EnergyRef);


  /*--------------------------------------------------
    calculate the energy of subframes.
    find out which subframe as the center reference.
    --------------------------------------------------*/
  /***** point EnergyRef[] to the start of the frame ******/
  pEnergyRef = EnergyRef + LFRAME_LEN - LSUBFRAME_LEN;

  ref_Frame = FindRefFrame(pEnergyRef, &frameEnergy);
  

  /*------------------------------------------------------------
    calculate the total cost by aligning three subcosts.
    ------------------------------------------------------------*/
  CalcFrameCosts(normcost, ref_Frame, frameEnergy, 
		 myCorrPDA_mblk->dIntraLagStart,
		 myCorrPDA_mblk->dIntraLagLength,
		 pEnergyRef, subcosts);


  /*------------------------------------------------------------
    Sharp the peaks of costs function.
    ------------------------------------------------------------*/
  for (i = 1; i < (LPITCH_CONF-LPITCH_MIN+1); i++)
    if (normcost[i] > NOISE_FLOOR) {
      if (normcost[i] > normcost[i-1] && normcost[i] > normcost[i+1])  /* find a peak */
	QuadraticPeak(normcost[i-1], normcost[i], normcost[i+1], &normcost[i]);
    }


  /*------------------------------------------------------------
    bias the cost function. 
    ------------------------------------------------------------*/
  BiasCosts(normcost, myCorrPDA_mblk->biasTab, pbias_floor);
  STACK_END
}



/*
 * Calculate cross correlation value of subframes
 */

static void CalcSubCross(float *sigRef, float *psubcosts)
{
  register int jlag;
  int isub;

  for (isub = 0; isub < NUM_SUBFRAME; isub++, sigRef += LSUBFRAME_LEN)
    {
      jlag = LPITCH_MIN;
      do {    
	  *psubcosts++ = (float)DotProd(sigRef, sigRef-jlag,LSUBFRAME_LEN);
	} while(++jlag <= LPITCH_MAX);
    }
}



/*
 * calculate the energy of subframes.
 */

static void CalcEnergy(float *sigRef, float *EngRef)
{
  register float *pS1, *pS2;
  register double dsum;
  register int k;

  pS1 = sigRef + LFRAME_LEN - 1;

  pS2 = pS1-LSUBFRAME_LEN + 1;

  dsum = DotProd(pS2, pS2, LSUBFRAME_LEN);
  *EngRef++ = (float)dsum;

  pS2--;
  for (k = 1; k <= LPITCH_MAX+LFRAME_LEN-LSUBFRAME_LEN; k++) {
    /*** we just need to take one sample out and add one sample in ***/
    dsum += ((double)*pS2 * *pS2); 
    pS2--;
    dsum -= ((double)*pS1 * *pS1);
    pS1--;
    if (dsum < 0) dsum = 0;
    *EngRef ++ = (float)dsum;
  }
}


/*
 * Find out the center subframe for correlation alignment.
 *
 * The simplest version, we only consider the last two subframes.
 */

static int FindRefFrame(float *EnergyRef, float *frameEnergy)
{
  float TotalEnergy = 0.0f;
  int isub;
  float dsum;
  float energy1, energy2;

  /* The first subframe */
  for (isub = 0; isub < NUM_SUBFRAME - 2; isub++) {
    TotalEnergy += EnergyRef[-isub*LSUBFRAME_LEN];
  }

  /* the NUM_SUBFRAME-1th subframe */
  dsum = EnergyRef[-isub*LSUBFRAME_LEN];
  TotalEnergy += dsum;
  energy1 = dsum;

  /* the last subframe */
  isub++;
  dsum = EnergyRef[-isub*LSUBFRAME_LEN];
  TotalEnergy += dsum;
  energy2 = LAST_SUBFRAME_WEIGHT * dsum;

  *frameEnergy = TotalEnergy;

  /* find out the reference frame */
  if (energy1 >= energy2)
    return NUM_SUBFRAME-2;
  else
    return NUM_SUBFRAME-1;
}


/*
 * calculate the total costs value of one frame and normalize them by energy
 */


static void CalcFrameCosts(float *normcost, int ref_Frame, float frameEnergy, 
			   int *dLagStart, int *dLagLength, float *EnergyRef, 
			   float *psubcosts)
{
  float *tempcrossEnergy;
  float *pEnergyRef;
  float *psubcosts_isub;
  int isub;
  int jlag;
  int istart;
  int tmpmaxlag;
  float tmpmaxcost;
  float small_corr_energy;

  /* first find out which subframe will be center one. */
  assert(ref_Frame == NUM_SUBFRAME-2 || ref_Frame == NUM_SUBFRAME-1);

  /* We do not need tracking the reference subframe, so copy its costs to normcost */
  /* then borrow the subcosts array of the reference subframe for tempcrossEnergy array */
  EnergyRef += LPITCH_MIN;
  tempcrossEnergy = &(psubcosts[ref_Frame*PITCH_CAND]);
  memcpy(normcost, tempcrossEnergy, PITCH_CAND*sizeof(float));
  memcpy(tempcrossEnergy, EnergyRef-ref_Frame*LSUBFRAME_LEN, PITCH_CAND*sizeof(float));
    

  /* If the reference subframe is the last subframe, we do not need to track the first
     subframe. */
  if (ref_Frame == NUM_SUBFRAME - 2) {
    isub = 0;
    pEnergyRef = EnergyRef;
    psubcosts_isub = psubcosts;
  }
  else {
    isub = 1;
    pEnergyRef = EnergyRef-LSUBFRAME_LEN;
    psubcosts_isub = &(psubcosts[PITCH_CAND]);
  }    


  /* then process other subframes */
  for(; isub < NUM_SUBFRAME; isub++) {
    if (isub != ref_Frame) {
      {
	int length;
	int iend;
	float tmpmaxcost1;
	int tmpmaxlag1;

	/* faster searching code */
	/* search for the maximum costs within the pitch range */
	iend = 0;
	tmpmaxlag = -1;
	tmpmaxcost = -1.0F;
          
	for (jlag = 0; jlag < PITCH_CAND; jlag++) {
	  istart = dLagStart[jlag];
	  if(tmpmaxlag < istart) {
	    FindMaxValueFlt(&psubcosts_isub[istart], dLagLength[jlag], 
			    &tmpmaxlag, &tmpmaxcost);
	    tmpmaxlag +=istart;
	  }
	  else {
	    length = istart + dLagLength[jlag] - iend;
	    if(length) {
	      assert(iend < PITCH_CAND);
	      tmpmaxcost1 = psubcosts_isub[iend];
	      tmpmaxlag1 = iend;
	      if(length == 2) {
		assert(iend+1 < PITCH_CAND);
		if(psubcosts_isub[iend+1] >tmpmaxcost1) {
		  tmpmaxcost1 = psubcosts_isub[iend+1];
		  tmpmaxlag1 = iend+1;
		}
	      }
	      if(tmpmaxcost1 > tmpmaxcost) {
		tmpmaxcost = tmpmaxcost1;
		tmpmaxlag = tmpmaxlag1;
	      }
	    }
	  }/* end if(tmpmaxlag ...) */
	  iend = istart + dLagLength[jlag];
	  normcost[jlag] += tmpmaxcost;
	  tempcrossEnergy[jlag] += (float)pEnergyRef[tmpmaxlag];
        } /* end jlag */
      }
    } /* end if(iframe == 0) */
    pEnergyRef -= LSUBFRAME_LEN;
    psubcosts_isub += PITCH_CAND;
  }


  FindMaxValueFlt(tempcrossEnergy, PITCH_CAND, &tmpmaxlag, &tmpmaxcost);
  small_corr_energy = LOW_ENERGY_RATIO * tmpmaxcost + MINCON;

  /* truncate low energy to the threshold value */
  for (jlag = 0; jlag < PITCH_CAND; jlag ++)
    if (tempcrossEnergy[jlag] < small_corr_energy)
      tempcrossEnergy[jlag] = small_corr_energy;


  /* now calculate the normalized pitch costs */
  for(jlag=0; jlag+2<PITCH_CAND; jlag+=3) {
    register float temp1; 
    register float temp2; 
    register float temp3; 

    temp1 = (float)sqrt((double)(frameEnergy*tempcrossEnergy[jlag]+MINCON));

    temp2 = (float)sqrt((double)(frameEnergy*tempcrossEnergy[jlag+1]+MINCON));

    temp3 = (float)sqrt((double)(frameEnergy*tempcrossEnergy[jlag+2]+MINCON));

    normcost[jlag] = normcost[jlag]/temp1;
    normcost[jlag+1] = normcost[jlag+1]/temp2;
    normcost[jlag+2] = normcost[jlag+2]/temp3;
  }

  for( ;jlag<PITCH_CAND; jlag++) {
    normcost[jlag] = normcost[jlag]/(float)sqrt((double)(frameEnergy*tempcrossEnergy[jlag]+MINCON));
  } 
}



static void BiasCosts(float *costs, float *biasTab, float *bias_floor)
{
  register int i;
  register float bias_f = 1.0F;

  /*------------------------------------------------------------
    Apply the new bias to the cost function.
    ------------------------------------------------------------*/
  for (i = 0; i+2 < PITCH_CAND; i+=3) {
    costs[i] *= biasTab[i];
    costs[i+1] *= biasTab[i+1];
    costs[i+2] *= biasTab[i+2];
  }

  for (; i < PITCH_CAND; i++)
    costs[i] *= biasTab[i];

  *bias_floor = bias_f;
}

