/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:       dft.c
* Purpose:        Compute the dft at harmonic frequencies.
* Functions:      VoxDFT()
* Author/Date:    Bob Dunn 4/8/97
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/dft.c_v   1.1   10 Feb 1998 13:43:10   weiwang  $
*
******************************************************************************/
#include <math.h>

#include "vLibMath.h"
#include "trigSC.h"
#include "dft.h"

#include "xvocver.h"


extern const float *sinTBL_32;
extern const float *cosTBL_32;
extern const float fRadiansToIndex;


/******************************************************************************
*
* Function:  VoxDFT()
*
* Action:    Compute the dft at harmonic frequencies.
*
* Input:     *pfx:    windowed input signal
*            iLength: length of windowed input signal
*            fOmega:  fundamental frequency for DFT in radians
*            iNFreq:  number of spectral samples to compute
*
* Output:    pfXr:    real part of spectrum at harmonic frequencies
*            pfXi:    imaginary part of spectrum at harmonic frequencies
*
* Globals:   Sine and Cosine tables.
*
* Return:    none.
*******************************************************************************
*
* Implementation/Detailed Description:
*
*    The real and imaginary parts of the spectral samples are 
*      computed at:
*
*        Omega, 2*Omega, 3*Omega, ..., iNFreq*Omega
*
*    If the length of the analysis window is odd, the center point of
*      the window is placed at the origin.  If the length of the
*      analysis window is even, the point at pfx[iLength/2] is placed at
*      the origin.
*
*    There are two segments of code below.  The first code segment
*      is a straight forward implementation of the DFT.  The second
*      code segment is an optimized implementation which uses tables
*      for the sine and cosine and takes advantage of the symmetry
*      of the sine and cosine.
*
* References: 
******************************************************************************/

void VoxDFT( float *pfx, 
             int    iLength, 
             float  fOmega, 
             int    iNFreq, 
             float *pfXr, 
             float *pfXi
           )
{
   int   i, j;
   int   iArg0;
   int   iArg;
   float fArg0;
   float fCosArg;
   float fSinArg;
   float fReal;
   float fImag;
   float *pfx1;
   float *pfx2;
   float ftmp;

   fArg0 = 0.0F;
   for (i=iNFreq; i; i--) 
   {
      /*-------------------------------------------
        Set the data pointers to the center of the
          analysis window.
      -------------------------------------------*/
      pfx1 = pfx2 = pfx + (iLength>>1);

      /*-------------------------------------------
        Set the frequency
      -------------------------------------------*/
      fArg0 += fOmega;

      /*-------------------------------------------
        Quantize the frequency and set up indexing
          constants.
      -------------------------------------------*/
      ftmp = fRadiansToIndex*fArg0;
      VoxROUND2pos(ftmp, iArg0);
      iArg = iArg0 & TRIG_MASK;

      /*-------------------------------------------
        Start with the point at the center of the
          analysis window.
      -------------------------------------------*/
      fReal = *pfx1;           /* cos(0.0) = 1.0 */
      fImag = 0.0F;            /* sin(0.0) = 0.0 */

      /*-------------------------------------------
        Increment/decrement pointers
      -------------------------------------------*/
      pfx1++;
      pfx2--;

      /*-------------------------------------------
        Process most of the analysis window.
      -------------------------------------------*/
      for (j=(iLength>>1)-1; j; j--) 
      {
         fCosArg = cosTBL_32[iArg];        /* get cos(w)            */
         fSinArg = sinTBL_32[iArg];        /* get sin(w)            */
         fReal += *pfx1 * fCosArg;         /* cos(-w) = cos(w)      */
         fReal += *pfx2 * fCosArg;
         fImag -= *pfx1++ * fSinArg;       /* sin(-w) = -sin(-w)    */
         fImag += *pfx2-- * fSinArg;
         iArg += iArg0;                    /* increment phase index */
         iArg &= TRIG_MASK;                /* mask phase index      */
      }

      /*-------------------------------------------
        Add the first point in the of the analysis
          window.
      -------------------------------------------*/
      fCosArg = cosTBL_32[iArg];
      fSinArg = sinTBL_32[iArg];
      fReal += *pfx2 * fCosArg;
      fImag += *pfx2 * fSinArg;

      /*-------------------------------------------
        An odd length window has one more point
          at the end.
      -------------------------------------------*/
      if (iLength&1) 
      {
         fReal += *pfx1 * fCosArg;
         fImag -= *pfx1 * fSinArg;
      }

      /*-------------------------------------------
        Store the spectral sample.
      -------------------------------------------*/
      *pfXr++ = fReal;
      *pfXi++ = fImag;
   }


}

