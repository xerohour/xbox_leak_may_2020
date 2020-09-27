/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* Filename:     STFTpks.h
*
* Purpose:      Compute the STFT magnitude-squared envelope from the
*               real and imaginary parts of the FFT of the windowed
*               speech. Then pick the peaks and use the amplitudes and
*               frequencies as the basis for the sinusoidal analysis/
*               synthesis system. RJM has added quadratic interpolation
*               to produce sine-wave frequencies having factional DFT
*               values. Can't say that its important but it feels good
*               particularly for low-pitched speakers
*
* Functions:    VoxSTFTpeaks()
*
* Author/Date:  Bob McAulay     11/96
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/STFTpks.h_v   1.2   11 Apr 1998 15:19:02   bobd  $
*******************************************************************************/
#ifndef STFTPEAKS_H
#define STFTPEAKS_H


void VoxSTFTpeaks ( STACK_R
                    float *pfSWR,
                    float *pfSWI,
                    float *pfMagSq,
                    float *pfPkPwr,
                    float *pfPkFrq,
                    int   *piPeakCount
                  );

#endif /* STFTPEAKS_H */

