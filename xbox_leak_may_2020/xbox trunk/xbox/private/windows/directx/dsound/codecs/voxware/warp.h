/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:        warp.h
*
* Purpose:         Spectral warping
*
* Functions:       VoxWarpSC36(), CalcWarpedFreq()
*
* Author/Date:     Bob Dunn 10/7/97
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/warp.h_v   1.2   10 Apr 1998 13:57:48   weiwang  $
*
******************************************************************************/
 
#ifndef WARP_H
#define WARP_H

/*******************************************************************************
*
* Function:  VoxWarpSC36()
*
* Action:    Perceptually warp a spectral magnitude or phase envelope. The
*              warping function streatches low frequency regions and
*              compresses high frequency regions.  The warping function
*              is linear in the low end an quadratic in the high end.
*
* Input:     float *pfIn  --> 256 point envelope (magintude or phase)
*                               on a linear scale.
*
* Output:    float *pfOut --> 256 point envelope (magintude or phase)
*                               on a non-linear perceptually warped scale.
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:  1) The warping may be done in place (i.e. pfIn=pfOut).
*            2) The fixed point index computation is valid for 16 bit
*                 as well as 32 bit word lengths.  This was written in
*                 fixed point for speed.
*            3) If speed is more important that memory size, this code
*                 should be implemented with a look-up table using
*                 256 words of ROM.
*
* Concerns/TBD:
*******************************************************************************/

void VoxWarpSC36( float *pfIn, float *pfOut);

/*******************************************************************************
*
* Function:  CalcWarpedFreq()
*
* Action:    Given a frequency in DFT samples on a linear scale, compute
*              the corresponding warped frequency.
*
* Input:     float --> frequency in DFT samples on a linear scale
*
* Output:    float --> frequency in DFT samples on the warped scale
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

float CalcWarpedFreq( float fLinFreq );


#endif /* WARP_H */

