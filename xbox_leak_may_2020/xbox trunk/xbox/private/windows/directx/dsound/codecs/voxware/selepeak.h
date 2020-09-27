/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       SelePeak.h
*                                                                              
* Purpose:        Choose "good" peaks for frequency-domain pitch estimator.
*                                                                              
* Functions:      SelectPeaks()
*
* Author/Date:    
*
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/SelePeak.h_v   1.1   02 Apr 1998 14:47:02   weiwang  $
******************************************************************************/

#ifndef _SELE_PEAK_H_
#define _SELE_PEAK_H_

/******************************************************************************
* Function:  SelectPeaks()
*
* Action:    Select peaks from the power spectrum and eliminate small peaks
*            due to background-noise.
*
* Input:     pfPower    -- input power spectrum. 
*            iBaseBand  -- the length of input power spectrum .
*            piPeakFreq -- the address of the output peak indecies.
*
* Output:    piPeakFreq -- the output peak indecies.
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/

void SelectPeaks( STACK_R 
                  const float *pfPower, 
                  int          iBaseBand, 
                  int         *piPeakFreq, 
                  float        fMaxPower
                );

#endif /* _SELE_PEAK_H_ */


