/*******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
*******************************************************************************/

/*******************************************************************************
*
* Filename:     ExPhase.c
*
* Purpose:      Create voiced and measured phase components of the 
*                 excitation phase.
*
* Functions:    ExcitationPhase()
*
* Author/Date:  Bob McAulay, Spring 1997
*
********************************************************************************
*
* Modifications:  Code cleaning and such. Bob Dunn 12/97.
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/ExPhase.h_v   1.1   16 Mar 1998 10:33:36   weiwang  $
*
*******************************************************************************/

/*******************************************************************************
*
* Function:  ExcitationPhase()
*
* Action:    Create voiced and measured phase components of the
*                 excitation phase.
*
* Input:     float  fOnsetPhase -> the onset phase
*            float  fBeta       -> the beta factor
*            float *pfVEVPhase  -> measured phase residuals
*            float *pfMinPhase  -> minimum-phase phases
*            int    iNpeaks     -> total number of harmonics
*            int    iBasePeaks  -> number of measured phase residuals
*
* Output:    float *pfMinPhase  -> output phases with voiced and measured
*                                    phase components
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
#ifndef EXPHASE_H
#define EXPHASE_H

void ExcitationPhase( float  fOnsetPhase,
                      float  fBeta,
                      const float *pfVEVPhase,
                      float *pfMinPhase,
                      int    iNpeaks,
                      int    iBasePeaks
                    );

#endif /* EXPHASE_H */

