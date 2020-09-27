/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          corrfunc.h
*
* Purpose:           calculate correlation function for pitch refinement
*
* Functions:
*
* Author/Date:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/corrfunc.h_v   1.0   06 Feb 1998 10:11:00   johnp  $
*******************************************************************************/
#ifndef _CORRFUNC_H_
#define _CORRFUNC_H_


void corrfunc( float dpitch, float fpitch, float fHiEdge, float *fRefinedMax,
	       int iBpeaks, float *pfFltPwr, float fFltEnergy, 
               unsigned long *iPeakFrq, float *fRefinedPeriod);

#endif /* _CORRFUNC_H_ */

