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
* Filename:      PostFilt.h
*
* Purpose:       Postfilter the log-base-2 warped allpole envelope
*
* Functions:     void PostFilter(float *pfLogEnv)
*
* Author/Date:   R.J.McAulay/6/17/97
*
*******************************************************************************
*
* Modifications:
*
* Comments:  Could save some time using the natural log in place of log-base-2.
*            Originally used the synthesizer predictor coefficients and gain
*            to determine the energy and the spectral tilt, but when the
*            parameters were quantized, this let to instabilties in the 
*            As[] to Ks[] routine, and "pops" in the synthetic speech. So it is
*            prudent to compute these parameters directly, even though it
*            increases the complexity.
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/PostFilt.h_v   1.2   10 Apr 1998 16:47:10   bobd  $   
*
******************************************************************************/


#ifndef _POST_FILT_H_
#define _POST_FILT_H_

/*******************************************************************************
*
* Function:  PostFilter()
*
* Action:    Use the LPC parameters to compute the spectral tilt and the
*            energy, flatten the allpole envelope and compute the post-filter,
*            apply the post-filter and re-normalize to equalize the energy.
*            The log-base-2 envelope is stored in place of the log-amplitudes.
*
* Input:     float *pfLogEnv  --> log (base 2) magnitude of all-pole envelope
*                                   with length CSENV_NFFT_2
*            float  fLog2Gain --> log (base 2) gain of the all-pole envelope
*            float *pfKS      --> reflection coefficients for all-pole envelope
*                                   with length LPC_ORDER
*
* Output:    float *pfLogEnv  --> postfiltered envelope
*                                   with length CSENV_NFFT_2
*
* Globals:   extern const float *cosTBL_32 --> cosine table
*
* Return:    none
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:   hard-wire coded for envelope length equal to CSENV_NFFT_2
*******************************************************************************/

void PostFilter(float *pfLogEnv, float fLog2Gain, float *pfKS);

#endif /* _POST_FILT_H_ */

