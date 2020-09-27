/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
*
* Filename: MidPitch.c
*
* Purpose:  centerize the pitch by calculating the autocorrelation value.
*
* Author:   Wei Wang
*
* Date:     Jan. 22, 1996
*
* Modified : Xiaoqin Sun for middle frame pitch estimation
* 
* Date:    May   30  1997 
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/MidPitch.c_v   1.3   20 Mar 1998 10:26:22   weiwang  $
*******************************************************************************/

#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "vLibVec.h"
#include "vLibMath.h"
#include "vLibSpch.h"
#include "codec.h"

#include "MidPitch.h"

#include "xvocver.h"

/* MidEstPitch:  estimiate the possible pitch range. */
float MidEstPitch(float Pitchm1, float Pitchp1, float Pvm1, float Pvp1)
{
  float pitch, Pv;

  /*----------------------------------------------------------------------------
    Determine the initial pitch value at the middle frame from pitch values 
    and voicing probability values at k-1 and k+1 frames.
    ----------------------------------------------------------------------------*/
  if( (Pitchp1 <= 1.25F*Pitchm1)  && (Pitchp1 >= Pitchm1*0.8F))
    pitch = (Pitchm1 + Pitchp1) *0.5F;
  else 
    {
      if(Pvm1>Pvp1) {       /* prevent quantization rounding error */
        pitch=Pitchm1;
        Pv = Pvm1;
      }
      else {
        pitch=Pitchp1;
        Pv = Pvp1;
      }

      if (pitch < 30 && (Pv*pitch*0.5F) < 1.0F)   /* to prevent pulsive sound */
        {
         if (Pitchp1 > Pitchm1)
            pitch = Pitchp1;
         else
            pitch = Pitchm1;
        }
    }

  return pitch;
}


/* MidPitchRefine: refine the estimated pitch using time-domain auto-correlation
   calculation. */
float TDPitchRefine(float *Spbuf, short length, float pitch, float *maxCost, 
                    float deltaP0)
{
  int delta;
  int Left_lag, Right_lag;
  float maxCorr;
  float refine_pitch;
  int bestLag, bestI;
  int jlag, i;
  float totalCorr;
  float pCorrCosts[3];
  float eng1, eng2;
  float ftmp;

  /*------------------------------------------------------------
    Find out the pitch searching range.
    ------------------------------------------------------------*/
  ftmp = deltaP0*pitch;
  VoxROUND2pos(ftmp, delta);
  if (delta > MAX_DELTA)
    delta = MAX_DELTA;
  
  ftmp = pitch-delta;
  VoxROUND2pos(ftmp, Left_lag);

  ftmp = pitch+delta;
  VoxROUND2pos(ftmp, Right_lag);

  if (Left_lag < MIN_PITCH)
  Left_lag = MIN_PITCH;
    
  if (Right_lag > MAX_PITCH)
  Right_lag = MAX_PITCH;


  /*--------------------------------------------------
    calculate the autocorrelation function.
    --------------------------------------------------*/
  jlag = Left_lag;
  maxCorr = -MAXFLT;
  bestI = 0;
  pCorrCosts[0] = pCorrCosts[1] = pCorrCosts[2] = 0.0F;
  i=0;
  do {
    totalCorr = DotProd(Spbuf, Spbuf+jlag, length-jlag);
    if (totalCorr > maxCorr) {
      maxCorr = totalCorr;
      bestI = i;
      pCorrCosts[0] = pCorrCosts[1];
    }
    else {
      if (bestI == (i-1)) {
        pCorrCosts[2] = totalCorr;
      }
    }

    pCorrCosts[1] = totalCorr;

    i++;
    jlag++;
  } while (jlag <= Right_lag);
  

  /*-------------------------------------------------------
    use quadratic interpolation to get refined pitch.
    -------------------------------------------------------*/
  bestLag = bestI+Left_lag;

  if ((bestLag > Left_lag) && (bestLag < Right_lag)) 
    refine_pitch = (float)bestLag + QuadraticPeak(pCorrCosts[0], maxCorr, 
                             pCorrCosts[2], maxCost);
  else {
    refine_pitch = (float)bestLag;
    *maxCost = maxCorr;
  }
     
  /*------------------------------------------------------------
    now normalized the cost value.
    ------------------------------------------------------------*/
  /* calculate energy of psigBuf[0:win_length-bestLag-1] and psigBuf[bestLag:win_length-1] */
  eng1 = DotProd(Spbuf, Spbuf, length-bestLag);
  eng2 = DotProd(Spbuf+bestLag, Spbuf+bestLag, length-bestLag);

  *maxCost /= ((float)sqrt(eng1*eng2+MINCON));

 
  return refine_pitch;
}
