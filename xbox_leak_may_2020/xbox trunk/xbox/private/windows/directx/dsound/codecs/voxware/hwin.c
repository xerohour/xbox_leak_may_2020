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
* Filename:       HWin.c
*
* Purpose:        Window signal using Hamming or Hanning window. The window 
*                 is calculated recursively.
*
* Functions:
*
* Author/Date:    Wei Wang 2/18/97
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/HWin.c_v   1.7   02 Mar 1998 18:24:26   bobd  $
*******************************************************************************
*
* Modifications:  Added option for Hanning window, and window normalization.
*                 Bob Dunn 5/1/97
*
* Comments:
*
* This code is based on Gerard's Magical 
* 2nd Order Difference Equation for Sinusoids.
* 
* Let
* 
*     x0 = theta                y0 = A * cos(x0)
*     x1 = theta + k            y1 = A * cos(x1)
*     x2 = theta + 2 * k        y2 = A * cos(x2)
* 
* then,
* 
*    y2 = diffCoef * y1 - y0
* 
* where,
* 
*    diffCoef = 2 * cos(k)
* 
* The code is modified from Dave's RecSine.c
*
*
* Concerns:
*
******************************************************************************/
#include <stdlib.h> 
#include <assert.h>
#include <math.h>
#include "vLibDef.h"
#include "vLibSgPr.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:   HammingWindowNormSC()
*
* Action:     Window a signal with a Hamming window.  This window was suppose
*              to have unit area, but the area is really 1.08.  The function
*              is kept around because SC36 codebooks are currently trained 
*              on it.
*
* Input:      float *pfX      the input signal
*             int   iLength   the length of the input signal
*
* Output:     float *pfX      the output signal
*
* Globals:    none
*
* Return:     none
******************************************************************************/
void HammingWindowNormSC( float *pfX, int iLength )
{
   float fScale;

   assert(pfX != NULL);
   assert(iLength > 0);

   fScale = 2.0F/(float) iLength;
   WindowKernel( pfX, iLength, 0.54F*fScale, 0.46F*fScale);
}

/******************************************************************************
*
* Function:   HammingWindowNorm()
*
* Action:     Window a signal with a Hamming window with unit area.
*
* Input:      float *pfX      the input signal
*             int   iLength   the length of the input signal
*
* Output:     float *pfX      the output signal
*
* Globals:    none
*
* Return:     none
******************************************************************************/

void HammingWindowNorm( float *pfX, int iLength )
{
   float fScale;
 
   assert(pfX != NULL);
   assert(iLength > 0);
 
   fScale = 1.0F/(0.54F*iLength-0.46F);
   WindowKernel( pfX, iLength, 0.54F*fScale, 0.46F*fScale);
}


/******************************************************************************
*
* Function:   HanningWindowNorm()
*
* Action:     Window a signal with a Hanning window that has unit area.
*
* Input:      float *pfX      the input signal
*             int   iLength   the length of the input signal
*
* Output:     float *pfX      the output signal
*
* Globals:    none
*
* Return:     none
******************************************************************************
*
* Concerns:   The first and the last samples are zeros
*
******************************************************************************/

void HanningWindowNorm( float *pfX, int iLength )
{
   float fScale;

   assert(pfX != NULL);
   assert(iLength > 0);

   fScale = 1.0F/(float) iLength;
   WindowKernel( pfX, iLength, fScale, fScale);
}

/******************************************************************************
*
* Function:   WindowKernel()
*
* Action:     Window a signal (Hamming or Hanning) 
*
* Input:      float *x        input signal
*             int   length    length of signal
*             float a         window constant A
*             float b         window constant B
*
* Output:     float *x        output windowed signal
*
* Globals:    none
*
* Return:     none
*******************************************************************************
*
* Implementation/Detailed Description:
*
*    Window a signal with the window:
*
*           w[n] = A - B * cos( 2*pi*n/(length-1) )
*
*    For a Hamming window A = 0.540 and B = 0.460
*    For a Hanning window A = 0.500 and B = 0.500
*
*    For unit area windows, multiply A and B by 2.0/length
*
* References:
*******************************************************************************
*
* Modifications:  multiply b before the recursion. So it causes non-bit exactness
*                 but the error is every small, in the order of 10^(-5).
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
void WindowKernel(float *x, int length, float a, float b)
{
  int i;
  float r2 = b;
  float r3;
  float r1;
  float rwin;
  float *y = &(x[length-1]);
  int half_length = length>>1;
  float diffCoef = (float)cos(DB_PI/(float)(length-1));

  assert(x != NULL);

  r3 = diffCoef * b;
  diffCoef *= 2.0F;

  assert(half_length > 2);
  
  /* first windowing the first two points */
  rwin = (a-r2); 
  *x++ *= rwin;    *y-- *= rwin;

  rwin = (a-r3);
  *x++ *= rwin;    *y-- *= rwin;

  /* then recursively calculate other points */
  for (i = 2; i+2 < half_length; i+=3) {
    r1 = diffCoef * r3;     /* diffCoef * y1              */
    r1 -= r2;               /* y2 = diffCoef * y1 - y0    */
    rwin = (a-r1);       /* outBuf = y2                */
    *x++ *= rwin;  *y-- *= rwin;
 
    r2 = diffCoef * r1;     /* diffCoef * y1              */
    r2 -= r3;               /* y2 = diffCoef * y1 - y0    */
    rwin = (a-r2);       /* outBuf = y2                */
    *x++ *= rwin;  *y-- *= rwin;
 
    r3 = diffCoef * r2;     /* diffCoef * y1              */
    r3 -= r1;               /* y2 = diffCoef * y1 - y0    */
    rwin = (a-r3);       /* outBuf = y2                */
    *x++ *= rwin;  *y-- *= rwin;
  }

  if (i < half_length) {
    r1 = diffCoef * r3;     /* diffCoef * y1              */
    r1 -= r2;               /* y2 = diffCoef * y1 - y0    */
    rwin = (a-r1);       /* outBuf = y2                */
    *x++ *= rwin;  *y-- *= rwin;
    i++;

    if (i < half_length) {
      r2 = diffCoef * r1;     /* diffCoef * y1              */
      r2 -= r3;               /* y2 = diffCoef * y1 - y0    */
      rwin = (a-r2);       /* outBuf = y2                */
      *x++ *= rwin;  *y-- *= rwin;
    }
  }

  if (length & 1)
    *x++ *= (a+b);
}

