/*******************************************************************************
*                         Voxware Proprietary Material                         *
*                         Copyright 1996, Voxware, Inc                         *
*                             All Rights Reserved                              *
*                                                                              *
*                        DISTRIBUTION PROHIBITED without                       *
*                       written authorization from Voxware                     *
*******************************************************************************/
 
/*******************************************************************************
* Filename:       KLPitch.c
*
* Purpose:        Set the scale factor for unvoiced speech.
*
* Functions:      float fSetKLFactor()
*
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
********************************************************************************
* Modifications:  Change fWinScale[] to fLog2WinScale[] by Wei Wang, 4/98
*
* Comments:
*
* Concerns: 
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/KLPitch.c_v   1.3   13 Apr 1998 18:26:18   weiwang  $
*******************************************************************************/
#include <assert.h>

#include "vLibMath.h"
#include "codec.h"
#include "KLPitch.h"

#include "xvocver.h"

/*---------------------------------------------------------------------
  The number of analysis windows used minus one.
---------------------------------------------------------------------*/
#define NUMBER_OF_WINDOWS_M1 5

/*---------------------------------------------------------------------
  The table fWinScale[] contains the scale factor 
    (window area)/sqrt(window energy) for each possible analysis window.
  Table now convert to fLog2(fWinScale[]).
---------------------------------------------------------------------*/
static const float fLog2WinScale[NUMBER_OF_WINDOWS_M1+1]={
#include "winscale.h"
};

/*---------------------------------------------------------------------
  Define the pitch periods for the analysis windows.
---------------------------------------------------------------------*/
static const float fWinPeriod[NUMBER_OF_WINDOWS_M1] = {
 96.0F, /* period of 32.5ms window */
 88.0F, /* period of 30.0ms window */
 80.0F, /* period of 27.5ms window */
 72.0F, /* period of 25.0ms window */
 64.0F, /* period of 22.0ms window */
};

/*---------------------------------------------------------------------
  Must scale by 1/sqrt(2/3) to adjust for the variance reduction
    due to overlapping and adding the triangular window.
    Also, take log base 2 of this facto.
---------------------------------------------------------------------*/
#define WINDOW_SCALING  0.29248125F  /*0.29248125036057792681F*/
 
/*---------------------------------------------------------------------
  Since the noise spectral density was fitted to the sine-wave peaks
    we can expect the synthesized noise level to be too high. So we
    reduce it to account for the peak-to-rms level.
---------------------------------------------------------------------*/
#define PEAKPICK_SCALING -0.33779991F  /*-0.33779990790610769924F*/

/*---------------------------------------------------------------------
  To get a noise level that RJM "likes" use the fudge factor
---------------------------------------------------------------------*/
#define FUDGEFACTOR_SCALING -0.50F

/*---------------------------------------------------------------------
  MUTLTI_SCALING: sum up the above 3 scaling factor and 0.5*fLog2(1/NFFT)
---------------------------------------------------------------------*/
#define MULTI_SCALING  (WINDOW_SCALING+PEAKPICK_SCALING+FUDGEFACTOR_SCALING \
                        -0.5F*(float)FFT_ORDER)

/*******************************************************************************
*
* Function:  fSetKLFactor()
*
* Action:    Determine scale factor for unvoiced harmonics
*
* Input:     float fAnaPitch  -> the analysis pitch period
*            float fSynF0  -> the synthesis pitch in DFT samples
*
* Output:    none
*
* Globals:   none
*
* Return:    float fLogAlpha  -> the scale factor (to add in log base 2)
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

float fSetKLFactor( float fAnaPitchPeriod, float fSynF0)
{
   int   i;
   float fLogAlpha;

   /*-----------------------------------------------------------------
     Set the Scaling ratio: (window area)/sqrt(window energy).
       This raio is a function of the analysis window used.  The
       window used depends on the coarse pitch estimate at the
       encoder, so the best we can do is guess based on the pitch
       received by the decoder.
   -----------------------------------------------------------------*/
   for (i=0; i<NUMBER_OF_WINDOWS_M1; i++)
      if (fAnaPitchPeriod>fWinPeriod[i])
        break;

   /*-----------------------------------------------------------------
    fLogAlpha = fLog2(fWinScale[i])+0.5*fLog2(fSynPitch/NFFT)
                + WINDOW_SCALING+PEAKPICK_SCALING+FUDGEFACTOR_SCALING

     fLog2(fWinScale[i])         normalizes for the energy in the 
                                   analysis window
     0.5F*fLog2(fSynPitch/NFFT)  normalizes for the synthesis pitch
   -----------------------------------------------------------------*/
   fLogAlpha = fLog2WinScale[i]+0.5F*fLog2(fSynF0)
               + MULTI_SCALING;
   
   return fLogAlpha;
}


