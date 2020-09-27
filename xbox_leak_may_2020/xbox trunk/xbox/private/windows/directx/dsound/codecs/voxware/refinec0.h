/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/RefineC0.h_v   1.0   06 Feb 1998 10:10:52   johnp  $
*******************************************************************************/
#ifndef _REFINEDC0_H
#define _REFINEDC0_H

unsigned short VoxRefineC0 (STACK_R float fCoarsePitchPeriod, float *fpFltPwr, 
			    float *fpPeakFrq, int iBpeaks, float *pfFltEnergy,
			    float *pfRefinedPitchPeriod, float *pfRhoRefined ); 

#endif /* _REFINEDC0_H */

