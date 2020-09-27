/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/
 
/******************************************************************************
* Filename:       SetAmps.h
*
* Purpose:        Interpolate the amplitude and phase envelopes at the
*                 sine-wave harmonics. Apply KL amplitude scale factor to 
*                 the unvoiced harmonics.  Also, increase the number of 
*                 unvoiced harmonics by adding sub-harmonics.
*
* Functions:      SetAmpAndPhase()
*                 AddSubHarmonics()
*
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns: Requires the length of the envelopes to be 65!  The 65th sample
*             is accessed, but multiplied by zero insetad of putting an
*             if statement inside the loop.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/SetAmps.h_v   1.0   06 Feb 1998 09:38:34   johnp  $
******************************************************************************/
#ifndef SETAMPS_H
#define SETAMPS_H

/*******************************************************************************
*
* Function:  SetAmpAndPhase()
*
* Action:    Set the log amplitude and minimum-phase phase of the 
*              harmonics.
*
* Input:     float  fNoiseScale   -> scale factor for unvoiced harmonics
*                                      (log base 2 value to be added)
*            float  fPitchDFT     -> pitch in DFT samples
*            int    iHarmonics    -> number of harmonics to generate
*            int    iVoicHarm     -> number of harmonics which are voiced
*            float *pfEnv         -> current envelope (warped, log 2 magnitude)
*            float *pfEnv_1       -> previous envelope (warped, log 2 magnitude)
*            float *pfPhase       -> current unwrapped minimum-phase envelope
*            float *pfPhase_1     -> previous unwrapped minimum-phase envelope
*            float  fInterpFactor -> envelope interpolation factor
*
* Output:    float *pfLogAmps     -> harmonic amplitudes (log base 2)
*            float *pfMinFaz      -> minimum-phase harmonic phases
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:  IMPORTANT NOTE: The code is currently written such that
*                              pfEnv[], pfEnv_1[], pfPhase[], pfPhase_1[]
*                              must have a valid numeric value at PI.
*                              This value is multiplied by zero, but it
*                              must exist.  This problem could be avoided
*                              by placing an if statement inside the loop,
*                              but this would increase code size and 
*                              operate more slowly.
*
* Concerns/TBD:
*******************************************************************************/

void SetAmpAndPhase ( float  fNoiseScale,
                      float  fPitchDFT,
                      float *pfLogAmps,
                      float *pfMinFaz,
                      int    iHarmonics,
                      int    iVoicHarm,
                      float *pfEnv,
                      float *pfEnv_1,
                      float *pfPhase,
                      float *pfPhase_1,
                      float  fInterpFactor
                     );

/*******************************************************************************
*
* Function:  AddSubHarmonics()
*
* Action:    This routine adds sub-harmonics in the unvoiced region.
*              Since we will use a harmonic synthesizer, 
*              voiced sub-harmonics are also added but their
*              magnitudes are set to zero.
*
* Input:     float *pfLogAmps     -> harmonics amplitudes (log base 2)
*            float *pfMinFaz      -> harmonics phases
*            int    iHarmonics    -> total number of (input) harmonics
*            int    iSubHarmonics -> total number of (output) harmonics
*                                      plus sub-harmonics
*            int    iRatio        -> the ratio: iSubHarmonics/iHarmonics
*            int    iVoicHarm     -> number of (input) voiced harmonics
*
*
* Output:    float *pfLogAmps     -> harmonics amplitudes (log base 2)
*            float *pfMinFaz      -> harmonics phases
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

void AddSubHarmonics ( float *pfLogAmps,
                       float *pfMinFaz,
                       int    iHarmonics,
                       int    iSubHarmonics,
                       int    iRatio,
                       int    iVoicHarm
                     );

#endif /* SETAMPS_H */

