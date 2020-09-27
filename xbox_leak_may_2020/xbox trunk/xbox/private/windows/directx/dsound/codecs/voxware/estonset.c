/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       EstOnset.c
*                                                                              
* Purpose:        Estimate the onset position of current frame. Calculate how 
*                 much to shift for window placement.
*                                                                              
* Functions:   
*                                                                              
* Author/Date: Wei Wang,  Aug. 21, 1997
*
* Reference:  Improvement of the Narrowband Linear Predictive Coder, 
*             NRL Report 8645 Page 8-14.
*          
********************************************************************************
* Modifications:  (1) change the algorithm to avoid the division. (v 1.9)
*                 (2) do 2:1 decimation.   (v 2.0)
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/EstOnset.c_v   1.3   11 Apr 1998 16:09:04   bobd  $
******************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "codec.h"
#include "VoxMem.h"
#include "vLibSnd.h"
#include "vLibMath.h"
#include "EstOnset.h"

#include "xvocver.h"

#define DECIM_R             2
#define AC_WEIGHT1           (0.03100586F)     /***** 1-(63/64)^2 *****/
#define AC_WEIGHT2           (0.96899414F)    /***** (63/64)^2 ****/
#define R                    (4)

#define MAX_KS                1.05F
#define MIN_ONSET_DIFF        10              /**** 10 samples difference ****/
#define ONSET_THRESHOLD       0.16F    /* 1.8/8,  was 1.7F */
#define INV_FRAME_LEN         0.0125F   /* 1/80 */
#define MAX_SHIFT             (HALF_FRAME_LENGTH_SC>>1)

typedef struct {
  float saveAC0[R];
  float saveAC1[R];
  float sumAC0;
  float sumAC1;
  int ptr0;
  int Onset_1;
} ONSET_Mblk;

/* estimate the onset time and return the window position */
int EstOnset(void *hOnsetMblk, void *pCircBufMblk, int codec_delay_size,
               int winLength, float *SpBuf)
{
  ONSET_Mblk *pOnsetMblk = (ONSET_Mblk *)hOnsetMblk;
  int OnsetK=0, OnsetDur = pOnsetMblk->Onset_1+MIN_ONSET_DIFF-FRAME_LENGTH_SC;

  float *psaveAC0 = pOnsetMblk->saveAC0;
  float *psaveAC1 = pOnsetMblk->saveAC1;

  int ptr0 = pOnsetMblk->ptr0;     /**** the pointer for the buffer ****/

  float AC0 = psaveAC0[ptr0];    /**** point to the previous sample ****/
  float AC1 = psaveAC1[ptr0];

  float sumAC0 = pOnsetMblk->sumAC0;
  float sumAC1 = pOnsetMblk->sumAC1;
  float ftmp;
  int WinShift;
  int k;
  register float t0, t1;
  float maxT0 = 1.0F;
  float maxT1 = ONSET_THRESHOLD;
  
#ifdef WIN32
  union {
    float f;
    long l;
  } union_t0, union_t1;
#else
  float s0, s1;
#endif

  /*----------------------------------------------------------------------
    (1) Get the current frame of speech out. 
    ----------------------------------------------------------------------*/
  GetFrameSpCBM(pCircBufMblk, codec_delay_size, FRAME_LENGTH_SC+1, SpBuf);

  for (k = 0; k < FRAME_LENGTH_SC; k+=DECIM_R, SpBuf+=DECIM_R) {

    /*-----------------------------------------------------------------
      (2) increase the pointer: point to k-8 sample.
      -----------------------------------------------------------------*/
    ptr0 = (ptr0+1)%R;

    /*-----------------------------------------------------------------
      (3) Calculate the 1st order forward-autocorrelation coefficients.      
      -----------------------------------------------------------------*/
    t1 = AC_WEIGHT1*SpBuf[0];
    AC0 = t1*SpBuf[0] + AC_WEIGHT2*AC0;
    AC1 = t1*SpBuf[1] + AC_WEIGHT2*AC1;

    /*-----------------------------------------------------------------
      (4) Find the fabs(AC1/AC0-sumAC1/sumAC0) > maxDCep ? 
      -----------------------------------------------------------------*/
    t1 = (float)fabs(AC1*sumAC0-AC0*sumAC1);
    t0 = (sumAC0*AC0);

    /*-----------------------------------------------------------------
      (5) calculate the cumulative Ac: sumAC = sumAC+AC[0]-AC[-8]
      -----------------------------------------------------------------*/
    sumAC0 = sumAC0 + AC0 - psaveAC0[ptr0];
    sumAC1 = sumAC1 + AC1 - psaveAC1[ptr0];
    psaveAC0[ptr0] = AC0;
    psaveAC1[ptr0] = AC1;

    if (k < OnsetDur) 
      continue;

#ifndef WIN32
    s0 = t1*maxT0;
    s1 = t0*maxT1;
    if (s0 > s1) {
      maxT0 = t0;
      maxT1 = t1;
      OnsetK = k;
    }
#else
    union_t0.f = t1*maxT0;
    union_t1.f = t0*maxT1;
    if (union_t0.l > union_t1.l) {
      maxT0 = t0;
      maxT1 = t1;
      OnsetK = k;
    }
#endif
  }

  /*------------------------------------------------------------
    (6) Update the memory 
    ------------------------------------------------------------*/
  pOnsetMblk->Onset_1 = OnsetK;
  pOnsetMblk->sumAC0 = sumAC0;
  pOnsetMblk->sumAC1 = sumAC1;
  pOnsetMblk->ptr0 = ptr0;

  /*-----------------------------------------------------------------
    (7) Compute the window shift based on the onset time.
    -----------------------------------------------------------------*/
  if (OnsetK > HALF_FRAME_LENGTH_SC) {
    ftmp = ((float)(FRAME_LENGTH_SC-OnsetK)*(codec_delay_size-(winLength>>1)))*INV_FRAME_LEN;
    VoxROUND2pos(ftmp, WinShift);
    if (WinShift > MAX_SHIFT)
      WinShift = MAX_SHIFT;
  }
  else {
    ftmp = (float)(OnsetK*(codec_delay_size-(winLength>>1)))*INV_FRAME_LEN;
    VoxROUND2pos(ftmp, WinShift);
    WinShift = -WinShift;
    if (WinShift < -MAX_SHIFT)
      WinShift = -MAX_SHIFT;
  }

  return WinShift;
}

/*
 * Initialize the structure.
 */
short VoxInitEstOnset(void **hOnsetMblk)
{
  ONSET_Mblk *pOnsetMblk;

  /* init the structure */
  if (VOX_MEM_INIT(*hOnsetMblk, 1, sizeof(ONSET_Mblk)))
    return 1;

  pOnsetMblk = (ONSET_Mblk *)*hOnsetMblk;
  
  pOnsetMblk->Onset_1 = 0;
  pOnsetMblk->sumAC0 = 0.0F;
  pOnsetMblk->sumAC1 = 0.0F;
  pOnsetMblk->ptr0 = 0;
  memset(pOnsetMblk->saveAC0, 0, R*sizeof(float));
  memset(pOnsetMblk->saveAC1, 0, R*sizeof(float));

  return 0;
}

/* Free structure */
short VoxFreeEstOnset(void **hOnsetMblk)
{
  if (*hOnsetMblk) {
    VOX_MEM_FREE(*hOnsetMblk);
  }

  return 0;
}


