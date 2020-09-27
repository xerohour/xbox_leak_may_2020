/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/SqLawPks.h_v   1.1   11 Apr 1998 15:18:36   bobd  $
*******************************************************************************/
#ifndef _SQUARELAWPEAKS_H
#define _SQUARELAWPEAKS_H

/******************************************************************************
*
* Function:  VoxSquareLawPeaks()
*
* Action:    This module computes the sine-wave amplitudes and frequencies
*              for the sine-wave representation of the square-law output
*
* Input:     float *pfFltAmp       -> peak amplitudes
*            float *pfPkFrq        -> peak frequencies (in DFT pts)
*            int    iBasePeakCount -> number of peaks
*            float  fLoBandEdge    -> maximum difference frequency (in DFT pts)
*
* Output:    float *pfNuAmp        -> new peak amplitudes
*            float *pfNuFrq        -> new peak frequencies (in DFT pts)
*            int   *piNuPeaks      -> number of new peaks
*
* Globals:   none
*
* Return:    none
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
* Concerns/TBD:
******************************************************************************/
void VoxSquareLawPeaks ( float *pfFltAmp,
                         float *pfPkFrq,
                         int    iBasePeakCount,
                         float  fLoBandEdge,
                         float *pfNuAmp,
                         float *pfNuFrq,
                         int   *piNuPeaks
                       );

#define SQUARE_LAW_BIAS     0.99F    /* to weight the 1st order harmonics...was .99 */

#define MAX_SPAN  2  /** difference frequencies no more that 1 apart **/

#define MAX_NU_THRESHOLD    0.50F     /* eliminates low-level square-law peaks...was .500 */
#define MIN_NU_THRESHOLD    0.05F     /* eliminates low-level square-law peaks...was .05 */
#define MIN_NU_PEAKCOUNT    10        /*  delete peaks if there are enough */

#endif  /*_SQUARELAWPEAKS_H*/

