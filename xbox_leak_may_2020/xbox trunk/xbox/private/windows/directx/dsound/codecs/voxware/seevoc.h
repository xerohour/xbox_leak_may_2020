/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/******************************************************************************
*
* Filename:      Seevoc.h
*
* Purpose:
*
* Functions:
*
* Author/Date:   Bob McAulay
*
*******************************************************************************
*
* Modifications:
*                - Reorganized Bob Dunn (12/31/96)
*                - Modified VoxSeevocPeaksFast() so that there will always
*                    be peaks returned.  Bob Dunn (7/6/98)
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/Seevoc.h_v   1.1   20 Jan 1999 15:30:16   weiwang  $
******************************************************************************/
#ifndef SEEVOC_H
#define SEEVOC_H

#define NO_PEAK  -1        /* impossible index to represent no peak selected */
#define ALPHA_SEEVOC  0.999F   /** extrapolation decay rate **/
#define DC_SLOPE     -0.050F   /** -log-base_2(.707)...DC level is -3dB down from 1st peak...was -0.500 **/
 
/******************************************************************************
*
* Function:    VoxSeevocFlattop()
*
* Action:      Compute the flat-top SeeVoc envelope from the measured
*                and then compute the spline envelope.
*
* Input: 
*
* Output:
*
* Globals:     none
*
* Return:      none
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:
*
*   The inputs peaks are measured as squared magnitude.
*     The peaks are then converted to log-base-2, hence
*     the seevoc envelope is also log-base-2.  For missing peaks
*     we fill in with samples of the STFT magnitude-squared.
*
* Concerns/TBD:
******************************************************************************/

void VoxSeevocFlattop( STACK_R  
		       float *pfMagSq, 
		       float *pfPwrIn, 
		       float *pfFrqIn, 
		       int iInCount, 
		       float *pfLogSeeEnv,
		       float fPitchDFT
		     );
#endif /* SEEVOC_H */

