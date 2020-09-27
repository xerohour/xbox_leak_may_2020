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
* Filename:   fft_cent.c
*
* Purpose:    Compute the FFT of a REAL signal with the signal centered at
*               FFT sample 0.
*
* Functions:  fftCenter()
*
* Author/Date:  Wei Wang, 95-97
*
*******************************************************************************
*
* Modifications: Support the output with the inter-leave format spectrum.
*
* Comments: 
*
* Concerns:      If pfSig and pfSW share the same buffer, their lengths must 
*                  satisfy the condition:  iNfft - iLength > (iLength>>1)
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/fft_cent.c_v   1.8   07 Apr 1998 17:55:14   weiwang  $
******************************************************************************/
#include <string.h>
#include <assert.h>
#include "vLibTran.h"

#include "xvocver.h"


/******************************************************************************
*
* Function:  VoxFftCenter()
*
* Action:    Compute the FFT of a REAL signal with the signal centered at
*              FFT sample 0.  Calculate the real FFT using inter-leaved 
*              formated FFT. 
* 
* Input:     pfSig     -- input signal buffer
*            iLength   -- the length of the input signal
*            pfSW      -- pointer to the output spectrum (need length N+2)
*            iFFTorder -- the order of FFT
*
* Output:    pfSW      -- output spectrum (inter-leaved)
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*      Change the signal from __/====\__ to ==\____/==,  
*          so the spectrum is smoothed but not centered.
*
* References: 
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns: If pfSig and pfSW share the same buffer, their lengths must
*           satisfy the condition:  iNfft - iLength >= (iLength>>1)
* 
******************************************************************************/

void VoxFftCenter( const float *pfSig, 
                   int          iLength,
                   float *       pfSW, 
                   int           iFFTorder
                 )
{
  int iNfft = 1 << iFFTorder; 
  int iHalfLen = iLength >> 1;

  /*** do error checking ***/ 
  assert(pfSig != NULL && pfSW != NULL);
  assert(((pfSig == pfSW)&&(iNfft>=(iLength+iHalfLen))) 
         ||((pfSig != pfSW)&&(iNfft>=iLength)));

  /*** Put the beginning part of pfSig to the end of pfSWR ******/
  memcpy(&(pfSW[iNfft-iHalfLen]), &(pfSig[0]), iHalfLen*sizeof(float));

  /*** Put the end part of pfSig to the beginning of pfSWR ***/
  memcpy(&(pfSW[0]), &(pfSig[iHalfLen]), (iLength-iHalfLen)*sizeof(float));
     
  /*** Zero the center of pfSig ***/
  memset(&(pfSW[iLength-iHalfLen]), 0, (iNfft-iLength)*sizeof(float));

  /*** FFT computation ***/
  VoxRealFft(pfSW, iFFTorder);
} /* VoxFftCenter() */

