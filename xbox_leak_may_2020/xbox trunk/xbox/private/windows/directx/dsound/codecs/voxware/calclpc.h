#ifndef _CALCLPC_H_
#define _CALCLPC_H_
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
 * Modification: Add LSF interpolation  and perceptual weighting, Jan.22, 1997
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/calcLPC.h_v   1.2   12 May 1998 12:55:12   weiwang  $
 ***********************************************************************/
void VoxLPCAnaly4PDA(STACK_R float *sigin, float *sigout, int length, void *myCorrPDA_mblk);

#endif
