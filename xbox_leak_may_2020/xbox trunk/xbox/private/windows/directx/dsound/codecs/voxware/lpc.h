
#ifndef LPC_H_
#define LPC_H_
/***********************************************************************
 *
 * Filename:  calcLPC.h
 *
 * Purpose:   calculate the LPC coefficients using autocorrelation method.
 *
 * Reference: any digital speech processing book.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 07, 1996.
 *
 ***********************************************************************/

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
void VoxCalcLPC(STACK_R float *pIn, int length, float *pLPCoef, float *pResidueEng,
	     float expandCoef, float *autocorr_buf);

#endif /* end of #ifndef LPC_H_ */


