/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       ResidAmp.h                                                         
*                                                                              
* Purpose:        Compute the harmonic amplitudes of the excitation                                                             
*                                                                              
* Author/Date:    Original developped 1995
*                 Merged by Rob Zopf, Sept. 12/95   
*                 2/9/96: modified by Wei Wang for new structures                                                          
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/encode/ResidAmp.h_v   1.0   26 Jan 1998 10:54:56   johnp  $                                                                     
*******************************************************************************/


#ifndef RESIDAMP_H
#define RESIDAMP_H

#define FFT_RES_ORDER             8
#define NFFT_RES                  (1<<FFT_RES_ORDER)    /* use 256 points FFT for inverse LPC spectrum */
#define F0_UV                     ((float)NFFT/(float)PITCH_UV)
#define F0_UV_RES                 ((float)NFFT_RES/(float)PITCH_UV)

#define FIXED_POINT_INDEX_RES      1
#define RES_SHIFT                  22


unsigned short VoxResidueAmp(STACK_R void *hEncMemBlk, float *BarkWtAmp);


#endif /* RESIDAMP_H */


