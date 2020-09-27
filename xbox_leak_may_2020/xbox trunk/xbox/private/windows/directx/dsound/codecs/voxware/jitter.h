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
* Filename:     Jitter.c
*
* Purpose:      Add a pitch-dependent phase perturbation to account for
*               irregular glottal pulse timing.
*
* Functions:    PhaseJitter()
*
* Author/Date:  Bob Dunn 1/98 (original Bob McAulay)
*
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/Jitter.h_v   1.1   13 Apr 1998 18:26:16   weiwang  $
*
*******************************************************************************/
#ifndef JITTER_H
#define JITTER_H

/*******************************************************************************
*
* Function:  PhaseJitter()
*
* Action:    Add a pitch-dependent phase perturbation to account for
*               irregular glottal pulse timing.
*
* Input:     float     fPitchDFT   -> pitch period
*            float    *pfPhase     -> harmonic phases (in)
*            int       iBasePeaks  -> number of baseband measure phases
*            int       iHarmonics  -> number of harmonics
*
* Output:    float    *pfPhase     -> harmonic phases (out)
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


void PhaseJitter ( float     fPitchDFT,
                   float    *pfPhase,
                   int       iBasePeaks,
                   int       iHarmonics
                 );

#endif /* JITTER_H */


