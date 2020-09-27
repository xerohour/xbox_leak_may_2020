/*****************************************************************************
*                        Voxware Proprietary Material                       *
*                        Copyright 1996, Voxware Inc.                       *
*                        All Rights Reserved                                *
*                                                                           *
*                        DISTRIBUTION PROHIBITED without                    *
*                        written authorization from Voxware.                *
*****************************************************************************/
/*******************************************************************************
*
* File:  NormWin.c
*
* Purpose: Normalize window by either power or area function.
*
* Author: Wei Wang
*
* Date:  Jan., 1998
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/NormWin.c_v   1.2   26 Feb 1998 16:46:14   weiwang  $
*
******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vLibSgPr.h"

#include "xvocver.h"

/*******************************************************************************
* 
* Function: NormSymWindowByPower()
*
* Action: Normalize the symmetric window by power
*
* Input:  pfWindow: non-normalized window data (either left half or whole size).
*         iWindowSize : the length of window.
*
* Output: pfWindow: normalized window data.
*
* return: None.
*
******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD: Since the window is symmetric, the total power is:
*   2.0*sum(Win[i]*Win[i]), i = 0...(N>>1)-1,
*   if N is odd number, add center point.
*   However, since the variable is float, it's not identical to:
*   sum(Win[i]*Win[i]), i = 0...(N-1).
*
******************************************************************************/

void NormSymWindowByPower(float *pfWindow, int iWindowSize)
{
  int iHalfSize;
  float fWindowPower, fNormFactor;
  int i;

  iHalfSize = iWindowSize >> 1;

  assert(pfWindow != NULL);

  /**** compute the left half size of window Power *****/
  fWindowPower = 0.0F;
  for (i = 0; i < iHalfSize; i++)
    fWindowPower += pfWindow[i] * pfWindow[i];

  /**** add the right half size of window Power *****/
  fWindowPower *= 2.0F;

  /**** consider the center point *****/
  if (iWindowSize & 1)
    fWindowPower += pfWindow[iHalfSize] * pfWindow[iHalfSize];

  /***** compute the normalization factor *****/
  fNormFactor = (float)sqrt((float)iWindowSize/fWindowPower);
  
  /***** apply the normalization factor to the window *****/
  for (i = 0; i < iHalfSize; i++)
    pfWindow[i] *= fNormFactor;

  if (iWindowSize & 1)
    pfWindow[i] *= fNormFactor;
}


/*******************************************************************************
* 
* Function: NormSymWindowByArea()
*
* Action: Normalize the symmetric window by area function.
*
* Input:  pfWindow: non-normalized window data (either left half or whole size).
*         iWindowSize : the length of window.
*
* Output: pfWindow: normalized window data.
*
* return: None.
*******************************************************************************/

void NormSymWindowByArea(float *pfWindow, int iWindowSize)
{
  int iHalfSize;
  float fWindowArea, fNormFactor;
  int i;

  iHalfSize = iWindowSize >> 1;

  /**** compute the left half size of window area function ****/
  fWindowArea = 0.0F;
  for (i = 0; i < iHalfSize; i++)
    fWindowArea += pfWindow[i];

  /**** add the right half size of window energy *****/
  fWindowArea *= 2.0F;

  /**** consider the center point *****/
  if (iWindowSize & 1)
    fWindowArea += pfWindow[iHalfSize];

  /***** compute the normalization factor *****/
  fNormFactor = 1.0F/fWindowArea;
  
  /***** apply the normalization factor to the window *****/
  for (i = 0; i < iHalfSize; i++)
    pfWindow[i] *= fNormFactor;
  if (iWindowSize & 1)
    pfWindow[i] *= fNormFactor;
}
