#ifndef _CENTCORR_H_
#define _CENTCORR_H_

/***********************************************************************
 *
 * Filename: CentCorr.c
 *
 * Purpose:  centerize the pitch by calculating cross-correlation.
 *
 * Reference: "Super Resolution Pitch Determination of Speech Signals".
 *             By Y. Medan, etc. IEEE ASSP, 1991.
 *
 * Author:   Wei Wang
 *
 * Date:     Oct. 09, 1996.
 *
 ***********************************************************************/


float calcCenterCorr(STACK_R CorrPDA_mblk *myCorrPDA_mblk, float *lpfBuf1, int pitch, 
		     float *maxCost );

#endif /* end of #ifndef _CENTCORR_H_ */

