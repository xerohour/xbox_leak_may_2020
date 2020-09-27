/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
/*******************************************************************************
* Filename:     CalcCost.c
*                                                                              
* Purpose:      calculate likelyhood function for LDPDA.
*                                                                              
* Functions:    CalcCostFunc
*                                                                              
* Author/Date:  Gerard Aguilar 01/97
*               Wei Wang       04/97
********************************************************************************
* Modifications: To reduce the complexity, all float to int conversions are 
*                eliminated by using fix-point indices. And time-consuming 
*                floating point comparisons are speed-up using long integer
*                comparisons by assigning numbers to float-int union (assuming
*                IEEE floating point). 
*
*              1. Use fixed-point code to prevent float-int conversion 
*              2. Use pitch table instead of F0 table 
*              3. Fewer input arrays (lower ROM and RAM). 
*           
*               -- Wei Wang 12/97
*
* Comments:                                                                    
* 
* Concerns:  
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/CalcCost.h_v   1.1   02 Apr 1998 14:46:52   weiwang  $
*******************************************************************************/

#ifndef CALC_COST_H
#define CALC_COST_H

/******************************************************************************
* Function:  CalcCostFunc()
*
* Action:    Compute the frequency domain pitch likelyhood function.
*
* Input:     fPitch     --  pitch value
*            piPeakFreq --  peak indices of spectrum
*            pfMag      --  compressed magnitude spectrum
*            iNBand     --  length of spectrum for 
*            fEngScale  --  energy scale factor
*
* Output:    none
*
* Globals:   none
*
* Return:    cost function (or likelyhood function) of input pitch
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

float CalcCostFunc( float         fPitch, 
                    const int     *piPeakFreq, 
                    const float   *pfMag, 
                    int           iNBand, 
                    float         fEngScale
                  );

#endif /* CALC_COST_H */


