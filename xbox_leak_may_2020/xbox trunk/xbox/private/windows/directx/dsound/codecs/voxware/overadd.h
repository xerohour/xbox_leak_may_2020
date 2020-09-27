/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
*
* Filename:      OverAdd.h
*
* Purpose:       Overlap/Add harmonics synthesizer using Fast Harmonic
*                  Transform and triangular window.
*
* Functions:     FHTOverlapAdd() 
*
* Author/Date:   Bob Dunn 3/97
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns: 
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/OverAdd.h_v   1.2   10 Apr 1998 16:10:50   weiwang  $
*
******************************************************************************/
#ifndef OVERADD_H
#define OVERADD_H

/*******************************************************************************
*
* Function:  FHTOverlapAdd()
*
* Action:    Overlap/Add harmonics synthesizer using Fast Harmonic
*                  Transform and triangular window.
*
* Input:     float *pfAmps             -> harmonics amplitudes
*            float *pfPhase            -> harmonics phases
*            float  fPitchDFT          -> pitch in DFT samples
*            int    iHarm              -> number of harmonics
*            int    iSynSubFrameLength -> number of samples to synthesize
*            float *fWaveform_1        -> previous waverform for overlap
*
* Output:    float *pfOutputBuffer     -> output waveform
*            float *pfWaveform_1        -> waverform for next overlap
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

void FHTOverlapAdd( STACK_R 
                    float *pfAmps, 
                    float fPitchDFT, 
                    float *pfPhase, 
                    int iHarm, 
                    int iSynSubFrameLength, 
                    float *pfOutputBuffer,
                    float *pfWaveform_1
                   );

#endif /* OVERADD_H */



