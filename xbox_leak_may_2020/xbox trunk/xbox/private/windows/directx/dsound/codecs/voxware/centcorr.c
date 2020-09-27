/***********************************************************************
 *
 * Filename: CentCorr.c
 *
 * Purpose:  centerize the pitch by calculating the autocorrelation value.
 *
 * Author:   Wei Wang
 *
 * Date:     Jan. 22, 1996
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/CentCorr.c_v   1.6   12 May 1998 12:55:02   weiwang  $
 ***********************************************************************/

#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "VoxMem.h"
#include "vLibSpch.h"  /* QuadraticPeak() */
#include "vLibVec.h"   /* DotProd()       */
#include "vLibSgPr.h"  /* WindowKernel() */

#include "PitchDef.h"
#include "CorrPDA.h"
#include "CentCorr.h"

#include "xvocver.h"


float calcCenterCorr(STACK_R CorrPDA_mblk *myCorrPDA_mblk, float *lpfBuf1, 
                     int pitch, float *maxCost )
{
  float *lpfbuf = myCorrPDA_mblk->lpfbuf;
  STACK_INIT
  float psigBuf[2*LFRAME_LEN];           /* psigBuf[2*LFRAME_LEN] */
  float pCorrCosts[9];
  STACK_INIT_END
  STACK_ATTACH(float*,psigBuf)
  STACK_ATTACH(float*,pCorrCosts)
  int delta, Left_lag, Right_lag;
  int win_length;
  int half_win_length;
  int jlag, i, bestI;
  float eng1, eng2;
  float *biasTab = myCorrPDA_mblk->biasTab;
  float maxCorr, totalCorr;
  int bestLag;
  float refine_pitch;

  STACK_START
  /*------------------------------------------------------------
    Find out the pitch searching range.
    ------------------------------------------------------------*/
  delta = (int)(0.1F*(float)pitch+0.5F);
  if (delta > 4)
    delta = 4;
  Left_lag = pitch-delta;
  if (Left_lag < LPITCH_MIN)
    Left_lag = LPITCH_MIN;
  
  Right_lag = pitch+delta;
  if (Right_lag > LPITCH_MAX)
    Right_lag = LPITCH_MAX;

  
  /*------------------------------------------------------------
    Calculate the length of window and copy the data to temporary 
    buffer.
    ------------------------------------------------------------*/
  win_length = 3*Right_lag;
  if (win_length < 65)     /* smallest window size */
    win_length = 65;
  if (win_length > (2*LFRAME_LEN))    /* largest window size */
    win_length = 2*LFRAME_LEN;

  half_win_length = win_length >> 1;
  if (half_win_length > LFRAME_LEN_2) {
    /* copy the signal of previous frame and current frame */
    memcpy(psigBuf, &(lpfbuf[LFRAME_LEN-half_win_length]), 
	   (half_win_length+LFRAME_LEN_2)*sizeof(float));
    /* copy the signal of future frame */
    memcpy(&(psigBuf[half_win_length+LFRAME_LEN_2]),lpfBuf1,(win_length-(half_win_length+LFRAME_LEN_2))*sizeof(float));
  }
  else {
    /* only need the signal of current frame */
    memcpy(psigBuf, &(lpfbuf[LFRAME_LEN-half_win_length]), 
	   win_length*sizeof(float));
  }

  /*------------------------------------------------------------
    Windowing the signal.
    ------------------------------------------------------------*/
  WindowKernel(psigBuf, win_length, CENT_WIN_A, CENT_WIN_B);


  /*--------------------------------------------------
    calculate the autocorrelation function.
    --------------------------------------------------*/
  jlag = Left_lag;
  maxCorr = -MAXFLT;
  bestI = 0;
  i=0;
  do {
    totalCorr = (float)DotProd(psigBuf, psigBuf+jlag, win_length-jlag);

    pCorrCosts[i] = totalCorr;
    if (totalCorr > maxCorr) {
      maxCorr = totalCorr;
      bestI = i;
    }
    i++;
    jlag++;
  } while (jlag <= Right_lag);
  

  /*-------------------------------------------------------
    use quadratic interpolation to get refined pitch.
    -------------------------------------------------------*/
  bestLag = bestI+Left_lag;

  if ((bestLag > Left_lag) && (bestLag < Right_lag)) 
    refine_pitch = (float)bestLag + QuadraticPeak(pCorrCosts[bestI-1], 
                       maxCorr, pCorrCosts[bestI+1], maxCost);
  else {
    refine_pitch = (float)bestLag;
    *maxCost = maxCorr;
  }
  
  /*------------------------------------------------------------
    now normalized the cost value.
    ------------------------------------------------------------*/
  /* calculate energy of psigBuf[0:win_length-bestLag-1] and psigBuf[bestLag:win_length-1] */
  eng1 = (float)DotProd(psigBuf, psigBuf, win_length-bestLag);
  eng2 = (float)DotProd(psigBuf+bestLag, psigBuf+bestLag, win_length-bestLag);

  *maxCost *= (biasTab[bestLag-LPITCH_MIN]/((float)sqrt((double)eng1*eng2+MINCON)));


  STACK_END
  return refine_pitch;
}


