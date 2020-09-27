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
 * $Header:   G:/r_and_d/archives/realtime/encode/LPC.c_v   1.5   11 Mar 1998 13:33:46   weiwang  $
 ***********************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "model.h"
#include "VoxMem.h"

#include "vLibSpch.h" /* BandExpand(), VoxDurbin() */
#include "vLibVec.h"  /* AutoCor(), ScaleVector()  */

#include "LPC.h"

#include "xvocver.h"

/***************************************************
 * Function: calcLPC()
 *
 * Input:  pIn:  input data pointer.
 *         length: input data length.
 *         pLPCoef: LPC coefficients pointer (output).
 *         pResidueEng:  residue energy (output).
 *         pWinFrameEng: windowed signal energy (output).
 *         lpcOrder: LPC order.
 *         expandCoef: expansion coefficients.
 *         myLPC_mblk: pointer of the LPC structure.
 *
 * Output:  None.
 ***************************************************/

void VoxCalcLPC( STACK_R 
                 float *pIn, 
					  int   length,
                 float *pLPCoef, 
                 float *pResidueEng, 
                 float  expandCoef, 
                 float *autocorr_buf
                )
{
  STACK_INIT
  float fPARCOR[LPC_ORDER];
  STACK_INIT_END
  STACK_ATTACH(float *, fPARCOR)

  float invLength = 1.0F/(float)length;

  STACK_START

  /*--------------------------------------------------
    calcualte auto-correlation.
  --------------------------------------------------*/
  AutoCor(LPC_ORDER+1, pIn, length, autocorr_buf);

  
  /*--------------------------------------------------
    calculate LPC coefficients by using Levinson-Durbin
    algorithm.
  --------------------------------------------------*/
  VoxDurbin(autocorr_buf, LPC_ORDER, pLPCoef, fPARCOR, pResidueEng);

  *pResidueEng *= invLength;
  ScaleVector( autocorr_buf, LPC_ORDER+1, invLength, autocorr_buf );


  /*--------------------------------------------------
    expand the LPC pole. pLPCoef is overwritten.
  --------------------------------------------------*/
  BandExpand(pLPCoef, pLPCoef, LPC_ORDER, expandCoef);

  STACK_END
}


