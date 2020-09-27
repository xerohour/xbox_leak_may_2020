/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:       dft.h
* Purpose:        Compute the dft at harmonic frequencies.
* Functions:      VoxDFT()
* Author/Date:    Bob Dunn 4/8/97
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/dft.h_v   1.0   06 Feb 1998 10:14:10   johnp  $
*
******************************************************************************/

/******************************************************************************
*
* Function:  VoxDFT()
*
* Action:    Compute the dft at harmonic frequencies.
*
* Input:     *pfx:    windowed input signal
*            iLength: length of windowed input signal
*            fOmega:  fundamental frequency for DFT in radians
*            iNFreq:  number of spectral samples to compute
*
* Output:    pfXr:    real part of spectrum at harmonic frequencies
*            pfXi:    imaginary part of spectrum at harmonic frequencies
*
* Globals:   Sine and Cosine tables.
*
* Return:    none.
******************************************************************************/
#ifndef DFT_H
#define DFT_H

void VoxDFT( float *pfx, int iLength, float fOmega, int iNFreq, 
             float *pfXr, float *pfXi);

#endif /* DFT_H */


