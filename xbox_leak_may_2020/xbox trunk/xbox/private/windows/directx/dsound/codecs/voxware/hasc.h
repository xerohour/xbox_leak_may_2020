/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       HASC.c
*                                                                              
* Purpose:        Compute the harmonic amplitudes.
*                                                                              
* Functions:  void HASC_pwr(float *power, short iStartHarm, short iEndHarm, float pitch,
	      float *pAmp, short nfft, short nwin);
*
* Author:   Wei Wang, 2/24/97 based on Gerard's HASC algorithm.
*
********************************************************************************
* Modification:
*                                                                              
* Comments:                                                                    
* 
* Concerns:   Maximum Bandwidth should depend on FFT points, length of window and
*             window type.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/utils/HASC.h_v   1.0   26 Jan 1998 11:20:14   johnp  $
*******************************************************************************/
#ifndef _HASC_H_
#define _HASC_H_

#define NFFT_BW                512
#define MAX_BW                 4          /* the maximum bandwidth is 4 for 512 points FFT */
#define HASC_SHIFT             22
#define FIXED_POINT_INDEX_HASC      1
/*
 * calculate power of harmonic amplitude using Gerard's HASC algorithm.
 *
 */
void HASC_pwr(float *power, short iStartHarm, short iEndHarm, float pitch,
	      float *pAmp, short nfft, short nwin);

#endif
/*ifndef _HASC_H_ */
