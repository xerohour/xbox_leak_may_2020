/***********************************************************************
 *
 * File: CorrCost.h
 *
 * Purpose: Calculate cross correlator based pitch cost function. Use
 *          subframes instead of full frame. Then dynamicly align the
 *          correlation cost values and normalize them.
 *
 * Reference: (1) "A 5.85kb/s CELP Algorithm For Cellular Application."
 *                 W. Kleijn etc, ICASSP 1993, p.596.
 *            (2) Email from W. Kleijn to B. McAulay, 1995.
 *
 * Author:    Wei Wang
 *
 * Date:      Oct. 08, 1996
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/CorrCost.h_v   1.2   12 May 1998 12:55:06   weiwang  $
 ***********************************************************************/

#ifndef _CORRCOST_H_
#define _CORRCOST_H_


void VoxCalcCrossCorrCost(STACK_R float *sigBuf, float *normcost, 
		       CorrPDA_mblk *myCorrPDA_mblk, float *pbias_floor);


#endif
/* end of #ifndef _CORRCOST_H_ */


