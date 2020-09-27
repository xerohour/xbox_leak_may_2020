/******************************************************************************
*
*                        Voxware Proprietary Material
*
*                        Copyright 1996, Voxware, Inc
*
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*
*                      written authorization from Voxware
*
******************************************************************************/
/******************************************************************************
*
* Filename:    Lineintp.c
*
* Purpose:     Linearly interpolate the harmonic samples  
*
* Author:      Xiaoqin Sun
* Date         11/24/97
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/Lineintp.c_v   1.1   10 Feb 1998 13:43:02   weiwang  $
*******************************************************************************
* Modifications:  
*
* Comments:
*
* Concerns:  
******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "vLibMath.h"

#include "Lineintp.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:  linearinterp()
*
* Action:    Sample the SeeVoc flat-top envelope at the pitch harmonics
*            and linearly interpolate through the peaks to create an
*            up-sampled envelope.
*
* Input:     float  fPitchDFT  --> pitch in DFT samples
*            float  fSeeVoc --> the SeeVoc flat-top (linear, not log)
*            int    iLfft2     --> length of input and output envelopes
*
* Output:    float  fLogSeeVoc --> log (base 2) of the spline envelope
*
* Return:    None
*******************************************************************************
*
* Implementation/Detailed Description:
*
*   Subroutine which samples the flattop seevoc envelope
*   at the pitch harmonics and linearly interpolate them to 
*   create the upsampled envelope. By using the
*   equally-spaced harmonic samples, the computational
*   complexity is reduced significantly. To maintain control
*   on real-time, the number of peaks allowed in the interpolation
*   is limited to NPEAKS_LINEAR. This has the effect of 
*   fitting a linear envelope in at least the baseband region
*   for low-pitched speakers and over the whole range for
*   high-pitch speakers. It is probably the latter case that is
*   more important. In that region for which the linear interpolation was
*   not computed, the flattop envelope is used.
*
* References:
******************************************************************************/

/* Limit number of peaks in spline routine to maintain real-time */
#define NPEAKS_LINEAR   80  
 
void linearinterp (const float fPitchDFT, float *fSeeVoc, const int iLfft2 ) 
{
   int    i, k;
   int    iPeaks;
   int    iTemp;
   int   *piFreqLoc;
   float  fSeeFreq;
   float *pfX;
   float *pfY;
   float  fScratchRam[4*NPEAKS_LINEAR+8];     /* scratch memory */

   /* Compute the number of harmonic peaks to interpolate */
   iPeaks = (int)((iLfft2-1)/fPitchDFT);
   iPeaks ++;                           /* count the extra peak at DC */
   if (iPeaks > NPEAKS_LINEAR)
      iPeaks = NPEAKS_LINEAR;
   
   pfY = fScratchRam;
   pfX = fScratchRam + iPeaks + 4;
   piFreqLoc = (int *) (fScratchRam + 2*(iPeaks+4)) ;

   /* Sample the flattop envelope at the pitch harmonics */
   fSeeFreq = 0.0F;
   for (i = 0; i < iPeaks; i++)
   {
      VoxROUND2pos( fSeeFreq, iTemp );
      pfY[i] = fLog2( fSeeVoc[iTemp] );
      piFreqLoc[i] = iTemp;
      pfX[i] = fSeeFreq;
      fSeeFreq += fPitchDFT;
   }

   pfY[iPeaks]=fLog2(fSeeVoc[iLfft2-1]);
   piFreqLoc[iPeaks] =iLfft2;
   pfX[iPeaks]=(float) (iLfft2-1);

   for(i=1; i<iPeaks+1;i++)
   {
   for(k=piFreqLoc[i-1]; k<piFreqLoc[i]; k++)
   fSeeVoc[k]=pfY[i-1]+(pfY[i]-pfY[i-1])*(k-pfX[i-1])/(pfX[i]-pfX[i-1]);
   }
 }





