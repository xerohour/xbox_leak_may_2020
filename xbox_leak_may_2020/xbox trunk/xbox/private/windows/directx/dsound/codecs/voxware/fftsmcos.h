/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:      FftSmCos.h
* Purpose:       Compute sum of cosines with an FFT
* Functions:     FftSumCos()
* Author/Date:   Bob Dunn    4/22/98
*******************************************************************************
*
* Modifications:  
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/FftSmCos.h_v   1.0   23 Apr 1998 12:01:08   bobd  $
*
******************************************************************************/
#ifndef FFTSMCOS_H
#define FFTSMCOS_H

/******************************************************************************
*
* Function:   FftSumCos()
*
* Action:     Compute sum of cosines with FFT.
*
* Input:      fPitchDFT  --> pitch in DFT samples
*             pfAmps     --> cosine amplitudes
*             pfPhase    --> cosine phase (in radians)
*             iHarm      --> number of harmonics
*             iLength    --> number of samples to synthesize
*
* Output:     pfOutBuf   --> sum of cosines (with length iLength)
*
* Globals:    cosTBL_32[], sinTBL_32[], fRadiansToIndex
*
* Return:     none
******************************************************************************/

void FftSumCos( STACK_R
                float  fPitchDFT, 
                float *pfAmps, 
                float *pfPhase, 
                int    iHarm, 
                int    iLength, 
                float *pfOutBuf
              );

#endif /* FFTSMCOS_H */


