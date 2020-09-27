/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware Inc.                          *
*                        All Rights Reserved                                   *
*                                                                              *
*                        DISTRIBUTION PROHIBITED without                       *
*                        written authorization from Voxware.                   *
*******************************************************************************/

/*******************************************************************************
*
* File:         quadPeak.c
*
* Purpose:      Using quadratic interpolation to get the interpolated peak
*               value.
*
* Functions:    QuadraticPeak()
*
* Author/Date:  Wei Wang, Oct. 09, 1996
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/quadPeak.c_v   1.5   02 Mar 1998 17:41:08   weiwang  $
******************************************************************************/

#include <assert.h>
#include "vLibSpch.h"

#include "xvocver.h"

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
* Implementation/Detailed Description:
*    Assume the function Y(x) satisfy the quandratic polynomial equation:
*         Y(x) = a*x^2 + b*x + c.
*    Then the peak value can be obtain by:
*         fYpeak = a*(fXpeak)^2 + b*fXpeak + c.
*    Since at peak, the derivative of Y(x) at fXpeak is 0:
*         2*a*fXpeak + b = 0.
*    So fXpeak = - (b / 2*a).
*
*    In order to get a, b and c, we shift the x-axis so that 3 indices are
*    at -1, 0, 1, and the integer peak is at x = 0. Then:
*           fY_1 = Y(-1) = a - b + c              (1)
*           fY0  = Y(0)  = c                      (2)
*           fY1  = Y(1)  = a + b + c              (3)
*    By solving the equation (1), (2) and (3), we can easily get:
*           a = (fY_1+fY1)*0.5-fY0,
*           b = (fY1-fY_1)*0.5
*       and c = fY0. 
*
* References:  Bob McAulay
*
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
float QuadraticPeak(float  fY_1, float fY0, float fY1, float *pfYpeak)
{
   float a, b, c;   /* the coefficients for quadratic equation */
   float fXpeak;

   /* do error checking */
   assert((fY0 >= fY_1) && (fY0 >= fY1));

   /* compute the coefficients */
   a = (fY_1 + fY1 ) * 0.5F - fY0;
   b = (fY1  - fY_1) * 0.5F;
   c = fY0;

   /* compute the peak index */
   if (a != 0.0F)
      fXpeak = - b / (2.0F*a);
   else
      fXpeak = 0.0F;
  
   /* peak value */
   *pfYpeak = (a*fXpeak + b)*fXpeak + c;

   return fXpeak;
}


