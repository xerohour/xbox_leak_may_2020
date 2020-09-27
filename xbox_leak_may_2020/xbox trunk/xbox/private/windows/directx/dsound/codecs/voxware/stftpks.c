/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:     STFTpks.c                        
*                                                                              
* Purpose:      Compute the STFT magnitude-squared envelope from the
*               real and imaginary parts of the FFT of the windowed
*               speech. Then pick the peaks and use the amplitudes and
*               frequencies as the basis for the sinusoidal analysis/
*               synthesis system. RJM has added quadratic interpolation
*               to produce sine-wave frequencies having factional DFT
*               values. Can't say that its important but it feels good
*               particularly for low-pitched speakers
*                                                                              
* Functions:    VoxSTFTpeaks()
*                                                                              
* Author/Date:  Bob McAulay     11/96
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/STFTpks.c_v   1.5   11 Apr 1998 15:18:58   bobd  $
*******************************************************************************/
#include <assert.h>
#include <math.h>

#include "VoxMem.h"
#include "codec.h"
#include "vLibSpch.h"
#include "vLibMath.h"
#include "vLibVec.h"

#include "STFTpks.h"

#include "xvocver.h"

/*------------------------------------------------------------
  If there are fewer than MIN_PEAKS peaks, the arbitrarily 
   sample the spectrum with a pitch of DEFAULT_PITCH.
------------------------------------------------------------*/
#define MIN_PEAKS     3
#define DEFAULT_PITCH 7
#define DEFAULT_PEAKS 36

/******************************************************************************
*
* Function:  VoxSTFTpeaks()
*
* Action:    Compute the sine-wave amplitudes and frequencies.
*
* Input:     float *pfSWR       --> real part of the spectrum
*            float *pfSWI       --> imaginary part of the spectrum
*
* Output:    float *pfMagSq     --> power spectrum (magnitude squared)
*            float *pfPkPwr     --> peaks of the power spectrum
*            float *pfPkFrq     --> frequencies (in DFT samples) of the peaks 
*                                     of the power spectrum
*            int   *piPeakCount --> number of peaks
*
* Globals:
*
* Return: 
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
void VoxSTFTpeaks ( STACK_R
                    float *pfSWR, 
                    float *pfSWI, 
                    float *pfMagSq, 
                    float *pfPkPwr,
                    float *pfPkFrq, 
                    int   *piPeakCount
                  )
{
   int   i, n;
   int   iFreq;
   float fDeltaFreq;
   float fPitch;
   float fFreq;

STACK_INIT
   int   iPeakFreqs[MAXHARM];
STACK_INIT_END

   STACK_ATTACH(int *, iPeakFreqs)

   /*-------------------------------------------------------
     Compute magintude squared of the STFT
   -------------------------------------------------------*/
   MagSq( pfSWR, pfSWI, NFFT_2, pfMagSq);

   STACK_START

   /*-------------------------------------------------------
     Find the locations of the peaks
   -------------------------------------------------------*/
   PeakPick( pfMagSq, 0, NFFT_2, MAXHARM, iPeakFreqs, piPeakCount );

   /*-------------------------------------------------------
     Do quadratic interpolation to improve the peak 
      locations and magnitudes.
   -------------------------------------------------------*/
   for (i=0; i<*piPeakCount; i++)
   {
     n = iPeakFreqs[i];

     fDeltaFreq = QuadraticPeak( pfMagSq[n-1], pfMagSq[n], pfMagSq[n+1], 
                                 &(pfPkPwr[i]));
     pfPkFrq[i] = n + fDeltaFreq;
 
   }      

   STACK_END

   /*----------------------------------------------------------------
     If there are not enough peaks, then arbitrarily sample the 
       pfMagSq[] using a pitch of DEFAULT_PITCH.
   ----------------------------------------------------------------*/
   if (*piPeakCount <= MIN_PEAKS)
   {
      fPitch = (float) DEFAULT_PITCH;
      fFreq = fPitch;
      iFreq = DEFAULT_PITCH;
      for (n=0; n<DEFAULT_PEAKS; n++)
      {
         pfPkFrq[n] = fFreq;
         fFreq += fPitch;
         pfPkPwr[n] = pfMagSq[iFreq];
         iFreq += DEFAULT_PITCH;
      }
      *piPeakCount = DEFAULT_PEAKS;
   }

} /*VoxSTFTpeaks()*/
 
