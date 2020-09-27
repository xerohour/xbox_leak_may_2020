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
* Filename:    FHTkrnl.c
*
* Purpose:     A generalized version of the Fast Harmonic Transform.
*
* Functions:   FHTkrnl()
*
* Author/Date: Bob Dunn (converted from Rob Zopf's fixed point code)
*                       (of course, based on JGA's FHT :-) )
*
*
*******************************************************************************
*
* Modifications: 
*
*  2/24/97 - RZ : Uses a conjugate symmetric FFT or inverse Real FFT.
*                 The phase input should be have the range of the Sin/Cos
*                 table used in the codec.
*                 
*                 NOTE : the phase is not checked to see if it is in the range
*                        of the sin/cos tables.  The calling function must 
*                        ensure that 0 <= fPhase[i] < N, where N is the size 
*                        of the sin and cos tables.
*
*  3/21/97 - WW : Use inverse Real FFT. The input data have to arrange to:
*                 re(0), re(1), re(2)...re(N/2), Im(1)...Im(N>>1-1) and output
*                 data is :  re(0), re(1), re(2)....re(N-1)
*
*  6/18/97 - WW: put exp_shift and fFHTWeights as input so that RT and SC can 
*                share the same code.
*
*  12/97   - WW: clean up code for VOX library.
*
*  1/98    - BD: more code cleaning and removing external trig tables
*
*  2/98    - WW: remove the scaling factor
*
* Comments: 
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/FHTkrnl.c_v   1.2   02 Mar 1998 16:19:50   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "vLibTran.h"

#include "xvocver.h"

/******************************************************************************
*
* Function:  VoxFHTkrnl()
*
* Action:    A fast harmonic sum of sinusoids algorithm (the fastest 
*            known to mankind as of 10/22/96 :-) ).
*            Returns a single pitch cycle of speech
*
* Input:     pfAmps       - array of harmonic amplitudes
*            piPhaseIndex - array of harmonic phase indexes (indexes into
*                             pfSinTbl[] and pfCosTbl[])
*            iHarm        - number of harmonics for synthesis
*            pfSig        - pointer to buffer for speech accumulation
*            iFFTorder    - the FFT order for FHT
*            pfFHTWeights - table of FHT weights
*            pfSinTbl     - pointer to sine table
*            pfCosTbl     - pointer to cosine table
*
* Output:    pfSig        - single pitch cycle of speech.
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:  
*
*******************************************************************************
*
* Modifications: remove scaling factor at end
*
* Comments:
*
* Concerns/TBD: For speed, this routine currently uses table 
*               lookup for the sine and cosine of the phase.
*               The table size is variable and the phase indexes
*               are assumed to be within the table's limits.
*
*               For inter-leaved formatted inverse FFT, the size for pfSig has 
*               to be (1<<iFFTorder)+2.
*
******************************************************************************/

void VoxFHTkrnl( const float *pfAmps, 
                 const int   *piPhaseIndex, 
                 int   iHarm,    
                 float   *pfSig, 
                 int   iFFTorder, 
                 const float *pfFHTWeights,
                 const float *pfSinTbl,
                 const float *pfCosTbl)
{
  float fMag;
  int   i, j;
  int   iNfft = 1<<iFFTorder;

  assert(pfAmps != NULL && piPhaseIndex != NULL);
  assert(pfSig != NULL);
  assert(pfFHTWeights != NULL);
  assert(pfSinTbl != NULL && pfCosTbl != NULL);

  /*---------------------------------------------------------
    Zero the FFT buffer
  ---------------------------------------------------------*/
  memset(pfSig, 0, sizeof(float)*(iNfft+2));

  /*---------------------------------------------------------
    Fill the interleaved FFT buffer with real and imaginary
      parts corresponding to the harmonic amplitudes and 
      phases.  Multiply the harmonic amplitudes by the FHT 
      weight (pfFHTWeights[]).  
  ---------------------------------------------------------*/
  for (i=0, j=2; i < iHarm; i++, j+=2)
  {
     fMag       =  pfAmps[i] * pfFHTWeights[i];
     pfSig[j]   =  fMag * pfSinTbl[piPhaseIndex[i]];
     pfSig[j+1] = -fMag * pfCosTbl[piPhaseIndex[i]];
  }

  /*---------------------------------------------------------
    Take the inverse REAL FFT without scaling.
  ---------------------------------------------------------*/
  VoxRealInverseFft(pfSig, iFFTorder, IFFT_NO_SCALE_FLAG);

}
