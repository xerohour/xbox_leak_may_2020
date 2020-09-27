/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       UVPhase.h
*
* Purpose:        Add random phase to unvoiced harmonics.
*
* Functions:      UnvoicedPhase()
*
* Author/Date:    Bob Dunn 12/18/97
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/UVPhase.h_v   1.1   18 Feb 1998 16:55:06   weiwang  $
******************************************************************************/
#ifndef UVPHASE_H
#define UVPHASE_H

/*******************************************************************************
*
* Function:  UnvoicedPhase()
*
* Action:    Add random phase to unvoiced harmonics.
*
* Input:     float *pfPhase     -> harmonic phases
*            int    iHarmonics  -> total number of harmonics
*            int    iVoicHarm   -> number of voiced harmonics
*            long  *plRandSeed --> address to the random seed
*
* Output:    float *pfPhase     -> harmonic phases
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

void UnvoicedPhase( float *pfPhase,
                    int    iHarmonics,
                    int    iVoicHarm,
                    long  *plRandSeed
                  );

#endif /* UVPHASE_H */


