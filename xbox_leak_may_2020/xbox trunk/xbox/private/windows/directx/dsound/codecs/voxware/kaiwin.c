/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1997, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
*
* Filename:    KaiWin.c
*
* Purpose:    Generate Kaiser window.
*
* Functions: KaiserWindow(), InitKaiserWindow()
*            FreeKaiserWindow()
*
* Author/Date: Wei Wang, Jan, 1998
*
*******************************************************************************
*
* Modifications:  KaiserWindow() only compute the left half size of window 
*    instead of the full size of window. InitKaiserWindow() does not contain 
*    normalization. All the normalization should call another function : 
*    NormSymWindowByPower() or NormSymWindowByArea().
*
* Comments: 
*
* Concerns: 
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/kaiwin.c_v   1.14   06 Mar 1998 16:57:48   weiwang  $
******************************************************************************/

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vLibMacr.h"
#include "vLibSgPr.h"

#include "xvocver.h"

static float Bessel(const float x);

#define BESSEL_ITERATIONS 20  /* number of iterations in Bessel function */

/******************************************************************************
*
* Function:  float Bessel(const float x)
*
* Action: Compute zero_order Bessel function.
*
* Input:     x -- input index
*
* Output:    
*
* Globals:   none.
*
* Return:    output value from Bessle function.
*******************************************************************************/

static float Bessel(const float x)
{
   int i;
   float y,z,xk,z0;
   float halfx;
   float ftmp;
   const int MAXiteration = BESSEL_ITERATIONS;

   halfx = 0.5F*x;
   z = 1.0F;
   y = halfx;
   xk = 1.0F;

   for (i=1; i<=MAXiteration; i++)    /* iterate BESSEL_ITERATIONS times */
   {  
      z0 = y * xk;
      ftmp = z0 * z0;   /* ftmp added to improve cross-platform effects */
      z += ftmp;
      xk = xk /(float)(i+1);
      y *= halfx;
   }
   return((float)z);
}


/******************************************************************************
*
* Function:  KaiserWindow()
*
* Action:    Compute the Kaiser window coefficients. Only return left side
*            of window.
*
* Input:     fWindow -- pointer for Kaiser window
*            iWindowSize -- the length of full size window
*            fBeta -- the beta factor for Kaiser window.
*
* Output:    fWindow -- the left half side of Kaiser window.
*
* Globals:   none.
*
* Return:    none.
*
*******************************************************************************
*
* Modifications:
*
* Comments: All the normalization should call another function : 
*           NormSymWindowByPower() or NormSymWindowByArea().
*
* Concerns/TBD:
******************************************************************************/

void KaiserWindow(float *fWindow, int iWindowSize, float fBeta)
{
  float fI0B;
  float fInvI0B;
  float fInvN;
  int iN;
  int iHalfSize;
  int i;
  float x, y, z;
  
  assert(fWindow != NULL);
  assert( (iWindowSize-1) > 0 );

  /**** initialization ****/
  iN = iWindowSize - 1;
  fI0B = Bessel(fBeta);
  fInvI0B=1.0F/fI0B;
  fInvN=1.0F/(float)iN;
  iHalfSize = iWindowSize >> 1;

  /**** compute the window value of the most left point ****/
  fWindow[0] = fInvI0B;

  /**** compute window value at [1...iHalfSize] ****/
  for (i=1 ; i<iHalfSize; i++) 
    { 
      z = 2.0F*((float)i*fInvN);
      z -= 1.0F;  /*** 1/N to prevent sensitivity calculation ***/
      x = -(z * z);
      x += 1.0F;
      y = fBeta*(float)sqrt(x);
      fWindow[i] = Bessel(y)*fInvI0B;
    }
  
  /**** Compute the center point ****/
  if ( iWindowSize & 1 )
     fWindow[i] = 1.0F;
}


/******************************************************************************
*
* Function:  InitKaiserWindow()
*
* Action:    Allocate memory and compute the Kaiser window coefficients.
*            !!! The array *pfWindow only has the half of the window length.
*
* Input:     pfWindow -- pointer to Kaiser window buffer.
*            iWindowSize -- the length of full size window
*            fBeta -- the beta factor for Kaiser window.
*
* Output:    pfWindow -- the return address of the left side of Kaiser window.
*
* Globals:   none.
*
* Return:    1 : fail to allocate the memory.
*            0:  succeed.
*
*******************************************************************************
* Modifications:
*
* Comments: All the normalization should call another function : 
*           NormSymWindowByPower() or NormSymWindowByArea().
*
* Concerns/TBD:
******************************************************************************/

unsigned short InitKaiserWindow(float **pfWindow, int iWindowSize, float fBeta)
{
  unsigned short retFlag = 0;

  /***** allocate the memory *****/
  *pfWindow = (float *)malloc(((iWindowSize+1)>>1) * sizeof(float));
  if (*pfWindow == NULL)
    retFlag = 1;
  else 
    {
      /***** compute the Kaiser window ******/
      KaiserWindow(*pfWindow, iWindowSize, fBeta);
    }
  
  return retFlag;
}

/******************************************************************************
*
* Function:  FreeKaiserWindow()
*
* Action:    Free the Kaiser window memory.
*
* Input:     pfWindow -- pointer to Kaiser window buffer.
*
* Output:    
*
* Globals:   none.
*
* Return:    none
*******************************************************************************/

void FreeKaiserWindow(float **pfWindow)
{
  SafeFree(*pfWindow);
}


