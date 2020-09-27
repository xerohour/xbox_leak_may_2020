/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/******************************************************************************
*
* Filename:      Seevoc.c
*
* Purpose:
*
* Functions:
*
* Author/Date:   Bob McAulay
*
*******************************************************************************
*
* Modifications:
*                - Reorganized Bob Dunn (12/31/96)
*                - Modified VoxSeevocPeaksFast() so that there will always
*                    be peaks returned.  Bob Dunn (7/6/98)
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/Seevoc.c_v   1.6   20 Jan 1999 17:47:56   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include "vLibMath.h"

#include "VoxMem.h"
#include "codec.h"

#include "Seevoc.h"

#include "xvocver.h"

static int VoxSeevocPeaksFast(float *pfMagSq, float *pfPwrIn, 
			      float *pfFrqIn, int iInCount, 
			      float fPitchDFT, float *pfPwrOut, 
			      float *pfFrqOut);



/******************************************************************************
*
* Function:    VoxSeevocFlattop()
*
* Action:      Compute the flat-top SeeVoc envelope from the measured
*                and then compute the spline envelope.
*
* Input: 
*
* Output:
*
* Globals:     none
*
* Return:      none
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
*   The inputs peaks are measured as squared magnitude.
*     The peaks are then converted to log-base-2, hence
*     the seevoc envelope is also log-base-2.  For missing peaks
*     we fill in with samples of the STFT magnitude-squared.
*
* Concerns/TBD:
******************************************************************************/

void VoxSeevocFlattop( STACK_R  
		       float *pfMagSq, 
		       float *pfPwrIn, 
		       float *pfFrqIn, 
		       int iInCount, 
		       float *pfLogSeeEnv,
		       float fPitchDFT
		     )
{
  int   i, j, jlo, jhi;
  int   iSeePeakCount;
  float ampjm1, frqjm1, frqj;
  float ftmp;

  STACK_INIT
    float pfSeePwr[MAXHARM];
    float pfSeeFrq[NFFT_2];
    STACK_INIT_END
    STACK_ATTACH(float *, pfSeePwr)
    STACK_ATTACH(float *, pfSeeFrq)

   STACK_START

   memset(pfSeePwr, 0, MAXHARM*sizeof(float));
   memset(pfSeeFrq, 0, MAXHARM*sizeof(float));

   /*----------------------------------------------
     If the pitch is too high we will not get 
       enough Seevoc peaks.  The pitch estimator
       currently (7/6/98) limits the pitch to 1000 
       Hz so this should not be a problem.
   ----------------------------------------------*/
   assert( fPitchDFT < (1500.0F*HZ2DFT) );

   /*----------------------------------------------
     Select the largest peak in each pitch adaptive frequency band.
   ----------------------------------------------*/
   iSeePeakCount = VoxSeevocPeaksFast(pfMagSq, pfPwrIn, pfFrqIn, 
				      iInCount, fPitchDFT,
				      pfSeePwr, pfSeeFrq);

   /*----------------------------------------------
     There will always be Seevoc peaks!  
   ----------------------------------------------*/
   assert( iSeePeakCount > 0 );


   /*----------------------------------------------
     Compute the flat-top envelope
   ----------------------------------------------*/
   i = 0;
   jhi = 0;
   ampjm1 = fLog2(pfSeePwr[0]) + DC_SLOPE;
   ampjm1 = fInvLog2(ampjm1);
   frqjm1 = 0.0F;
   for (j = 0; j < iSeePeakCount; j++)
   {
      frqj = pfSeeFrq[j];
      jlo = jhi;
      ftmp=0.5F*(frqjm1 + frqj);
      VoxROUND2pos(ftmp, jhi);
      for (i = jlo; i < jhi; i++)
         pfLogSeeEnv[i] = ampjm1;

      ampjm1 = pfSeePwr[j];
      frqjm1 = frqj;
   }

   jhi += VoxFLOORpos(fPitchDFT);
   if (jhi > NFFT_2)
      jhi = NFFT_2;

   for (j = i; j < jhi; j++)
       pfLogSeeEnv[j] = ampjm1;   /** log_seeamp[iSeePeakCount-1] **/


   /*----------------------------------------------
     Extrapolate last peak to the folding frequency
   ----------------------------------------------*/
   for (j = jhi; j < NFFT_2; j++)
       pfLogSeeEnv[j] = ALPHA_SEEVOC*pfLogSeeEnv[j-1];
   
   
   STACK_END
}
 

/******************************************************************************
*
* Function:    VoxSeevocPeaksFast()
*
* Action:     
*
* Input: 
*
* Output:
*
* Globals:     none
*
* Return:      number of peaks
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
static int VoxSeevocPeaksFast( float *pfMagSq,
                                float *pfPwrIn,
                                float *pfFrqIn,
                                int iInCount,
                                float fPitchDFT,
                                float *pfPwrOut,
                                float *pfFrqOut
                              )
{
   int   j;
   int   iOutCount;
   int   iPeakIndex;
   float fStepSize;
   float fFreqLow;
   float fFreqHigh;
   float fMaxPwr;
   float ftmp;


   /*-------------------------------------------
     Initialize some things
   -------------------------------------------*/
   iOutCount  = 0;
   iPeakIndex = 0;
   fStepSize  = 0.5F*fPitchDFT;
   fFreqLow   = fStepSize;
   fFreqHigh  = fFreqLow+fPitchDFT;


   /*-------------------------------------------
     Skip peaks that are out of the harmonic band.
       Be careful here, the test of the right
       hand side of the expression must be done 
       first to avoid an index out of range!
   -------------------------------------------*/
   while ( (pfFrqIn[iPeakIndex]<fFreqLow) && (iPeakIndex<iInCount) )
      iPeakIndex ++;


   /*-------------------------------------------
     Now find the largest peak in each pitch
       adaptive band.
   -------------------------------------------*/
   while ( (fFreqLow<((float)NFFT_2-fStepSize)) && (iOutCount<MAXHARM) )
   {
      fMaxPwr = 0.0F;

     /*-------------------------------------------
       If (iPeakIndex<iInCount) then search for
         a peak in the pitch bin.  Otherwise
         don't search and just use the spectral
         magnitude at the middle of the bin.
     -------------------------------------------*/
      if (iPeakIndex<iInCount)
      {
         j = iPeakIndex;

         /*-------------------------------------------
             Be careful here, the test of the right
             hand side of the expression must be done
             first to avoid an index out of range!
         -------------------------------------------*/
         while ((pfFrqIn[j]<fFreqHigh) && (j<iInCount))
         {
            if (fMaxPwr < pfPwrIn[j])
            {
               fMaxPwr    = pfPwrIn[j];
               iPeakIndex = j;
            }
            j ++;
         }
      }


     /*-------------------------------------------
       If no peak was found use STFT magnitude at
       the mid-point of the pitch bin
     -------------------------------------------*/
      if (fMaxPwr > 0.0F)
      {
         pfPwrOut[iOutCount] = fMaxPwr;
         pfFrqOut[iOutCount] = pfFrqIn[iPeakIndex];
      }
      else
      {
         ftmp=0.5F*(fFreqLow+fFreqHigh);
         VoxROUND2pos(ftmp, j);
         pfPwrOut[iOutCount] = pfMagSq[j] + 1.E-6F;
         pfFrqOut[iOutCount] = (float) j;
      }


     /*-------------------------------------------
       Prepare limits for the next iteration.
     -------------------------------------------*/
      fFreqLow  = pfFrqOut[iOutCount] + fStepSize;
      fFreqHigh = fFreqLow+fPitchDFT;
      if (fFreqHigh > (float) NFFT_2)
         fFreqHigh = (float) NFFT_2;

      iOutCount ++;


     /*-------------------------------------------
       Skip peaks that are out of the harmonic band.
         Be careful here, the test of the right
         hand side of the expression must be done 
         first to avoid an index out of range!
     -------------------------------------------*/
      while ( (pfFrqIn[iPeakIndex]<fFreqLow) && (iPeakIndex<iInCount) )
         iPeakIndex ++;
   }

   return iOutCount;
}


