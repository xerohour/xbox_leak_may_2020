/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       Refine.c                                                             
*                                                                              
* Purpose:        Performs spectrum reconstruction                                                             
*                                                                              
* Functions:      VOXErr VoxPitchRefine(float *const pitch, void *PDA_mblk,
*                             const float *const SWR, const float *const SWI,
*                             const unsigned int uiNfft, short *pLL);                                                                 
*                                                                              
* Autohor/Date    Original developped by Suat Yeldener 
*                 Written by Rob Zopf  Sept. 12/95                                                                 
********************************************************************************
* Modifications:Ilan Berci/ 10/96 Removed global dependencies
*                                 Modularized code segments (Encapsulation)
*                                 Removed scope changes
*                                 Changed error code procedure
*                                 Removed useless code segments
*                                 General optimization         
*
* Modifications:Wei Wang  1/97    Rewrite all code. Change to L1 norm for pitch 
*                                 refinement. Use resampling technique to best-fit
*                                 window spectrum. -- need optimization.
*                         5/2/97: put macro FABS().
*
* Comments:                                                               
* 
* Concerns:
*
* $Header:   G:/r_and_d/archives/realtime/encode/Refine.c_v   1.1   11 Mar 1998 13:33:48   weiwang  $
*******************************************************************************/

#include  <math.h>
#include  <assert.h>
#include  <stdlib.h>

#include  "VoxMem.h"
#include  "model.h"
#include "research.h"

#include "VoxRound.h"
#include  "GetnHarm.h"

#include "SynSpec.h"

#include "Refine.h"

#include "xvocver.h"



/*******************************************************************************
* Function:       VOXErr VoxPitchRefine(float *const pitch, void *PDA_mblk,
*                         const float *const SWR, const float *const SWI,
*                         const unsigned int uiNfft)
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:Wei Wang  1/97    Rewrite all code. Change to L1 norm for pitch 
*                                 refinement. Use resampling technique to best-fit
*                                 window spectrum. -- need optimization.
*
* Comments:       use only half the window spectrum
*
* Concerns/TBD:
*******************************************************************************/
const float CRefinePitchTab1[NREFINEP01] = {-1.5F, -0.75F, 0.0F, 0.75F, 1.5F};
const float CRefinePitchTab2[NREFINEP02] = {-0.5F, -0.25F, 0.25F, 0.5F};


unsigned short VoxPitchRefine_TIME(float *const pitch, const float *const SWR, const float *const SWI,
		      float *pPower, const unsigned int uiWinLen, float *BandError)
{
  float *pSWR = (float *)SWR;
  float *pSWI = (float *)SWI;
  int iSpecInd1, iSpecInd2;
  float minErr, totalErr;
  int i;
  float fBasePitch;
  float fTryPitch;
  float fRefinePitch;


  if (*pitch < (float)MIN_REFINE_PITCH) *pitch = (float)MIN_REFINE_PITCH;


  /*************************************************************
    Find the errors from 0.75 resolution 
    The spectrum error ranged from 50Hz to 3700 Hz
    ************************************************************/
  fBasePitch = *pitch;
  iSpecInd1 = SPEC_START;  iSpecInd2 = SPEC_END;
  i = 0;
  minErr = MAXCON;    
  fRefinePitch = fBasePitch;
  while (i < NREFINEP01) {
    fTryPitch = *pitch + CRefinePitchTab1[i];

    if (fTryPitch >= MIN_REFINE_PITCH)
      totalErr = CalcCmpxSpecErr(fTryPitch, pSWR, pSWI, uiWinLen, iSpecInd1, iSpecInd2);
    else
      totalErr = MAXCON;

    if (minErr > totalErr) {
      minErr = totalErr;
      fRefinePitch = fTryPitch;
    }
    i++;
  } 


  /*************************************************************
    second run: test from  0.25 resultion 
    ************************************************************/
  i = 0;
  fBasePitch = fRefinePitch;
  while (i < NREFINEP02) {
    fTryPitch = fBasePitch + CRefinePitchTab2[i];

    if (fTryPitch >= MIN_REFINE_PITCH)
      totalErr = CalcCmpxSpecErr(fTryPitch, pSWR, pSWI, uiWinLen, iSpecInd1, iSpecInd2);
    else
      totalErr = MAXCON;

    if (minErr > totalErr) {
      minErr = totalErr;
      fRefinePitch = fTryPitch;
    }
    i++;
  } 


  /**** make sure the number of harmonics won't go beyond 90 ****/
  /**** the maximum pitch after the refinement is 180+1.5+0.5 ****/
  /**** and the corresponding number of harmonics should be 
        (floor)(182.0*255.0/512.0) ****/
  if(fRefinePitch > (float)(MAX_PITCH+2))
     fRefinePitch = (float)(MAX_PITCH+2);

  /*************************************************************
    calculate the harmonics error and energy 
    ************************************************************/
  CalcBandErr(fRefinePitch, pSWR, pSWI, pPower, uiWinLen, BandError);


  *pitch = fRefinePitch;

  return 0;
  
}
