/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*                                                                              *
*******************************************************************************/
 
/*******************************************************************************
*
* File:         vLibSpch.h
*
* Purpose:
*
* Functions:
*
* Author/Date:
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibspch.h_v   1.32   02 Mar 1998 17:41:14   weiwang  $
******************************************************************************/
 
#ifndef _VLIB_VLIBSPCH
#define _VLIB_VLIBSPCH

#ifdef __cplusplus
extern "C" {
#endif

/* BandExpd.h */
/*******************************************************************************
*
* Function: BandExpand()
*
* Input:  pIn:            pointer of the input LPC coefficients.
*         iLPCOrder:      order of the LPC coefficients.
*         fExpandCoeff:   expandion coefficient.
*
* Output: pOut:           pointer of the output LPC coefficients.
*
* We define A(z) as:
*
*               A(z) = 1 + SUM(k=1...p) a_k * z^(-k)
* output is, 1, a[1], a[2]....
*******************************************************************************/
void BandExpand( const float *pIn, float *pOut, int iLPCOrder,
                 float fExpandCoeff );


/* VoxDurb.h */
/*****************************************************************************
 * Function: VoxCalcDurbin()
 *
 * Input:  pAutoCorr:     auto-correlation buffer.
 *         iLpcOrder:      LPC order.
 *
 * Output: pLPCoef:       LPC coefficients.
 *         pfPARCOR:      Partial Correlation (reflection) coefficients.
 *         pResidueEng:   residue energy.
 * Return:  None.
 * We define A(z) as:
 *
 *               A(z) = 1 + SUM(k=1...p) a_k * z^(-k)
 * output is, 1, a[1], a[2]....
 ****************************************************************************/
void VoxDurbin( const float *pAutoCorr, int iLpcOrder, float *pfLPCoef,
                float *pfPARCOR, float *pfResidEnrg );

 
/* PeakPick.h */
/*******************************************************************************
* FUNCTION    : PeakPick()
*
* PURPOSE     : Pick peaks from a segment of an array of floats
*
* INPUT
*      pfMag      - segment of an array of floating point magintudes
*      iStart     - starting index of the segment
*      iStop      - ending index of the segment
*      iMaxPeaks  - maximum number of peaks to be picked
*
* OUTPUT
*      piPeakFreq - indices of peaks
*      piNPeaks   - number of peaks
*
* RETURN          None
*******************************************************************************/
void PeakPick( const float *pfMag, int iStart, int iStop, int iMaxPeaks,
               int *piPeakFreq, int *piNPeaks );


/* quadPeak.h */
/******************************************************************************
*
* Function:  QuadraticPeak()
*
* Action:    Compute the interpolated peak value using quandratic interpolation.
*
* Input:     fY_1, fY0, fY1 -- the output of three continuous indices 
*            pfYpeak -- the pointer to the output peak value
*
* Output:    pfYpeak -- output of the interpolated peak
*
* Globals:   none
*
* Return:    the index for the interpolated peak which is between -1 to 1.
*******************************************************************************
*
* Modifications:
*
* Comments:     (1) Need fY0 >= fY_1 and fY0 >= fY1 to get meaningful value.
*               (2) The output fXpeak is relative value to X0.
*
* Concerns/TBD: 
*
******************************************************************************/
float QuadraticPeak(float  fY_1, float fY0, float fY1, float *pfYpeak);
#ifdef __cplusplus
}
#endif

#endif /* _VLIB_VLIBSPCH */


