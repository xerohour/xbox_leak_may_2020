/***********************************************************************
 *
 * Filename:  calcLPC.c
 *
 * Purpose:   calculate the LPC coefficients using autocorrelation method.
 *
 * Reference: any digital speech processing text book.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 07, 1996.
 *
 * Modification: Add LSF interpolation  and perceptual weighting, Jan.22, 1997
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/calcLPC.c_v   1.7   12 May 1998 12:55:12   weiwang  $
 *
 ***********************************************************************/

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vLibTran.h" /* VoxAsToLspEven(), VoxLspToAsEven() */
#include "vLibSpch.h" /* BandExpand(), VoxDurbin()          */
#include "vLibVec.h"  /* AutoCor()                          */

#include "VoxMem.h"
#include "CorrPDA.h"
#include "PitchDef.h"
#include "NspIIR.h"

#include "calcLPC.h"

#include "xvocver.h"

void VoxLPCAnaly4PDA( STACK_R 
                      float *sigin, 
                      float *sigout, 
                      int    length, 
                      void  *pCorrPDA_mblk
                    )
{
  CorrPDA_mblk *myCorrPDA_mblk=(CorrPDA_mblk *)pCorrPDA_mblk;

STACK_INIT
  float psigBuf[FRAME_LEN];   /* psigBuf[FRAME_LEN] */
  float fPARCOR[PDA_LPC_ORDER];
  float pAutoCorrBuf[PDA_LPC_ORDER+1];
  float pAlpc[PDA_LPC_ORDER+1];
  float pAlsf1[PDA_LPC_ORDER];
  float pAlsf[PDA_LPC_ORDER];
  float pWeightCoef[2*PDA_LPC_ORDER+1];
STACK_INIT_END

STACK_ATTACH(float *, psigBuf)
STACK_ATTACH(float *, fPARCOR)
STACK_ATTACH(float *, pAutoCorrBuf)
STACK_ATTACH(float *, pAlpc)
STACK_ATTACH(float *, pAlsf1)
STACK_ATTACH(float *, pAlsf)
STACK_ATTACH(float *, pWeightCoef)

  float ResidueEng;

  int i, j;

  float diffLsf;
  float *pAlsf0;               /* LSF of previous frame */
  float fSlope;
  float weight_den, weight_num;

  float *psigIn, *psigOut;
  float *pwin;

STACK_START

  /*------------------------------------------------------------
    (1). asymmetric windowing the signal. 
    ------------------------------------------------------------*/
  pwin = myCorrPDA_mblk->LpcWindow;
  psigIn = sigin;
  for (i = 0; i < length; i++)
    psigBuf[i] = psigIn[i] * pwin[i]; 


  /*------------------------------------------------------------
    (2). calcualte auto-correlation coefficients.
    ------------------------------------------------------------*/
  AutoCor( (PDA_LPC_ORDER+1), psigBuf, length, pAutoCorrBuf);

  
  /*------------------------------------------------------------
    (3). calculate LPC coefficients by using Levinson-Durbin
    algorithm.
    ------------------------------------------------------------*/
  VoxDurbin(pAutoCorrBuf, PDA_LPC_ORDER, pAlpc, fPARCOR, &ResidueEng);

  /*------------------------------------------------------------
    (4). expand the LPC pole. pAlpc is overwritten.
    ------------------------------------------------------------*/
  BandExpand(pAlpc, pAlpc, PDA_LPC_ORDER, PDA_EXPAND_COEF);


  /*------------------------------------------------------------
    (5). calculate LSFs.
    ------------------------------------------------------------*/
  VoxAsToLspEven(pAlpc, PDA_LPC_ORDER, RATE8K, pAlsf1);

  /*------------------------------------------------------------
    (6). check the difference of LSFs.
    ------------------------------------------------------------*/
  weight_num = PERCEP_WEIGHT_NUM_BIG;
  for (i = 1; i < PDA_LPC_ORDER; i++) {
    diffLsf = pAlsf1[i]-pAlsf1[i-1];
    if (diffLsf < MIN_DIFF_LSF)
      weight_num = PERCEP_WEIGHT_NUM_SMALL;
  }
  
  /*------------------------------------------------------------
    (7). interpolate the LSFs between subframes. 
    do perceptual weighting of signal.
    ------------------------------------------------------------*/
  pAlsf0 = myCorrPDA_mblk->Alsf0;
  fSlope = INTERP_LSF_COEF;
  weight_den = PERCEP_WEIGHT_DEN; 
  psigIn = sigin;
  psigOut = sigout;
  for (i = 0; i < NUM_SUBFRAME; i++) {
    /* interpolate LSFs */
    for (j = 0; j < PDA_LPC_ORDER; j++)
      pAlsf[j] = pAlsf0[j] + fSlope*(pAlsf1[j]-pAlsf0[j]);
    fSlope += INTERP_LSF_COEF;


    /* Lsf converts to Lpc */
    VoxLspToAsEven(pAlsf, PDA_LPC_ORDER, INV_RATE8K, pAlpc);

    /** numerator of weighting filter **/
    BandExpand(pAlpc, &(pWeightCoef[0]), PDA_LPC_ORDER, weight_num);
    /** denominator of weighting filter **/
    BandExpand(pAlpc, pAlpc, PDA_LPC_ORDER, weight_den);
    memcpy( &(pWeightCoef[PDA_LPC_ORDER+1]), pAlpc+1, PDA_LPC_ORDER*sizeof(float));

    /** filter the signal using IIR filter.**/
    VoxNspIIRFlt(psigIn, psigOut, SUBFRAME_LEN, pWeightCoef, PDA_LPC_ORDER,
		 myCorrPDA_mblk->percWeight_mem);
    
    psigIn += SUBFRAME_LEN;
    psigOut += SUBFRAME_LEN;
  }


  /*------------------------------------------------------------
    (8) copy Alsf1[0:9] to Alsf0[0:9] for next frame.
    ------------------------------------------------------------*/
  memcpy(pAlsf0, pAlsf1, PDA_LPC_ORDER*sizeof(float));

STACK_END
}


