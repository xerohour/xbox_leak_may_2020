/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename:        SVqPhase.c
*
* Purpose:         Pairwise vector quantization of the 10ms and 20ms phases.
*                  The 10ms and 20ms pitches are use to predict the 20ms phase
*                  from the 10ms phase. The measured 20ms phase and the
*                  predicted 20ms phase then form a pair that are
*                  vector-quantized. 
*
* Functions:     VoxSVQEncPhase, VoxSVQDecPhase, VoxEncPhasePrediction,
*                VoxDecPhasePrediction
*
* Author/Date:     Bob Dunn 4/18/97 and Bob McAulay 4/30/97
*
*******************************************************************************
*
* Modifications:   With Phase prediction to increase correlation
*                  Hard wired for a sub-vector length of 2 for speed
*
* Comments:    
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/SVqPhase.c_v   1.8   13 Apr 1998 16:17:54   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "vLibQuan.h"
#include "vLibMath.h"

#include "codec.h"
#include "quansc36.h"

#include "SVqPhase.h"

#include "xvocver.h"


/*--------------------------------------------------
  Definitions for phase quantization 
  --------------------------------------------------*/
#define PHASE0_LEVELS  32                 /* linear quantizer if phase error is small enough */
#define PHASE0_ERR_THRD   0.217237F       /* threshold of phase error */
#define PHASE0_FACTOR  (32.0F/TWOPI)      /* step for linear quantizer */
#define INV_PHASE0_FACTOR (TWOPI/32.0F)   /* inverse step for linear quantizer */
#define PHASE0_FLOOR      0.1927F         /* floor for end-frame phase */

/*--------------------------------------------
  Assign appropriate coding tables
  --------------------------------------------*/
/* VQ codebook size */
#define PHASE_VQ_SIZE     (((1<<BITS_PHASE_PAIR_SC6)-PHASE0_LEVELS)>>1)

static const float fPhaseCodeBookTable[PHASE_VQ_SIZE*2] = {
#include "PhaseCB.h"
};

/* SQ size + VQ size without sign bit */
#define PHASE_CODEBOOK_SIZE  (1<<(BITS_PHASE_PAIR_SC6-1)) 
static const int sPhaseCOVIndex[PHASE_CODEBOOK_SIZE] = {
#include "PhaseInd.h"
};


#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  VoxSVQEncPhase
*
* Action:    Use this module to VQ pairwise predictive phase coding.
*
* Input:  pfPhaseErr --  phase errors
*         pfPhase    --  end-frame phases
*         piIndices -- pointer to the output indices
*
* Output: piIndices -- output quantization indices
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Implementation/Detailed Description: 
*        If predictive error is smaller than PHASE0_ERR_THRD, quantize the 20ms
*     phase using linear quantizer. Otherwise, do 2-dimensional VQ of the phase
*     pair. 
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

void VoxSVQEncPhase(float *pfPhase_Err, float *pfPhase, int *piIndices)
{
  int   i, j;
  int   iSignBit, iIndex;
  float pfVector[2];

  assert((pfPhase != NULL) && (pfPhase_Err != NULL) && (piIndices != NULL));

  for (i=0; i<SC6_PHASES; i++)
    {
      /*--------------------------------------------
        Get the sign bit and set the phase error 
        larger than 0.
        --------------------------------------------*/
      if (pfPhase_Err[i] < 0.0F)
        {
          pfPhase_Err[i] = -pfPhase_Err[i];
          iSignBit = 1;
        }
      else 
        iSignBit = 0;

      /*--------------------------------------------
        Test for a 10ms phase near zero.
        If phase error is small enough, use
        linear quantizer.
        --------------------------------------------*/
      if (pfPhase_Err[i] < PHASE0_ERR_THRD)
        {
          /*----------------------------------------
            Do linear quantization 
            ----------------------------------------*/
          iIndex = VoxFLOOR((pfPhase[i]-PHASE0_FLOOR)
                            *PHASE0_FACTOR+0.5F);
          if (iIndex < 0)
            iIndex += PHASE0_LEVELS;

          assert(iIndex < PHASE0_LEVELS);

          /*----------------------------------------
            Extract the sign bit.
            ----------------------------------------*/
          iSignBit = iIndex & 1;
          iIndex = iIndex >> 1;
        }
      else
        /*--------------------------------------------------
          Do vector quantization if phase error is above the
          threshold.
          --------------------------------------------------*/
        {
         /*--------------------------------------------
           Create the vector pair for Dim2VectorQuantize()
           --------------------------------------------*/
         pfVector[0] = pfPhase_Err[i];
         pfVector[1] = pfPhase[i];

         /*--------------------------------------------
           Encode subvector using fast search
           --------------------------------------------*/
         iIndex = Dim2VectorQuantize(pfVector, fPhaseCodeBookTable, 
                                     PHASE_VQ_SIZE);
          
         /*--------------------------------------------------
           Normalize the index by considering SQ level without
           sign bits.
           --------------------------------------------------*/
         iIndex += (PHASE0_LEVELS>>1);
        }

      /*--------------------------------------------------
        Now search for the channel optimized index 
        --------------------------------------------------*/
      j = 0;
      while(iIndex != sPhaseCOVIndex[j])
        j++;
      iIndex = j;

      /*--------------------------------------------------
        Combine quantization index and the sign bits
        --------------------------------------------------*/
      piIndices[i] = iIndex + (iSignBit<<(BITS_PHASE_PAIR_SC6-1));

      assert(piIndices[i] < (1<<BITS_PHASE_PAIR_SC6));
   }
}
#endif

/******************************************************************************
*
* Function:  VoxSVQDecPhase
*
* Action:    Use this module to decode the phase pair
*
* Input:  piIndices -- quantization indices
*         pfPhase_Err -- pointers to predictive phase errors
*         pfPhase     -- pointer to the end-frame phases
*
* Output: pfPhase_Err -- predictive phase errors
*         pfPhase     -- the end-frame phases
*
* Globals:   none
*
* Return:    none
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

void VoxSVQDecPhase(const int *piIndices, float *pfPhase_Err, float *pfPhase)
{
  int   i, iIndex, iSignBit;
  float pfVector[2];

  assert((piIndices != NULL) && (pfPhase != NULL) && (pfPhase_Err != NULL));

  for (i=0; i<SC6_PHASES; i++)
    {
      /*--------------------------------------------------
        Get the sign bit and index
        --------------------------------------------------*/
      iSignBit = piIndices[i]>>(BITS_PHASE_PAIR_SC6-1);
      iIndex = piIndices[i] & (PHASE_CODEBOOK_SIZE-1);

      /*--------------------------------------------------
        Map the channel optimized index to regular index 
        --------------------------------------------------*/
      iIndex = sPhaseCOVIndex[iIndex];

      /*--------------------------------------------
        Test for a 10ms phase near zero.
        The first 16 code entries are for the phase errors
        equal to 0. Phases are linearly quantized.
        --------------------------------------------*/
      if (iIndex < (PHASE0_LEVELS>>1))
        {
          /*----------------------------------------
            Combine the sign bit
            ----------------------------------------*/
          iIndex = (iIndex<<1) + iSignBit;

         /*----------------------------------------
           Decode the phase pair
           ----------------------------------------*/
          pfPhase_Err[i] = 0.0F;
          pfPhase[i] = (float)iIndex*INV_PHASE0_FACTOR + PHASE0_FLOOR;
          if (pfPhase[i] > TWOPI)
            pfPhase[i] -= TWOPI;
        }
      else
        /*--------------------------------------------------
          Decode phase pair using VQ.
          --------------------------------------------------*/
        {
          /*--------------------------------------------
            Set pointer to decoded sub vector
            --------------------------------------------*/
          iIndex -= (PHASE0_LEVELS>>1);

          Dim2VectorLUT(iIndex, fPhaseCodeBookTable, PHASE_VQ_SIZE, pfVector);
          if (iSignBit == 1)
            pfPhase_Err[i] = -pfVector[0];
          else
            pfPhase_Err[i] = pfVector[0];

          pfPhase[i] = pfVector[1];
        }
    }
}

/******************************************************************************
*
* Function:  VoxEncPhasePrediction
*
* Action:    Predict the 10ms phases and output the predictive error
*
* Input:  fPitch_Mid -- middle-frame pitch
*         fPitch     -- end-frame pitch
*         pfPhaseMid -- middle-frame phases
*         pfPhase    -- end-frame phases
*
* Output: pfPhaseMid -- predictive phase errors
*         pfPhase    -- end-frame phases
*
* Globals:   none
*
* Return:    none
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

void VoxEncPhasePrediction (float fPitch_Mid, float fPitch, 
                            float *pfPhaseMid, float *pfPhase)
{
   int  i, iMhat;
   float fPitchDFT[2];
   float fDeltaPhase, fPhaseShift;
   float fTemp;

   assert(pfPhase != NULL);
   assert(pfPhaseMid != NULL);

   /*--------------------------------------------------
     Compute pitch in DFT domain 
     --------------------------------------------------*/
   fPitchDFT[0] = (float)NFFT/fPitch_Mid;
   fPitchDFT[1] = (float)NFFT/fPitch;

   /*--------------------------------------------------
     Compute delta-phase and initialize phase-shift
     --------------------------------------------------*/
   fDeltaPhase = (fPitchDFT[0]+fPitchDFT[1])*
                 (0.5F*(float)HALF_FRAME_LENGTH_SC*TWOPIONFFT);
   fPhaseShift = fDeltaPhase;

   for (i = 0; i < SC6_PHASES; i++) {
     /*--------------------------------------------------
       Compute the predictive error for the middle frame 
       phase.
       --------------------------------------------------*/
      pfPhaseMid[i] = pfPhase[i] - fPhaseShift - pfPhaseMid[i];

      /*--------------------------------------------------
        Normalize the phase error so that it's between -Pi
        to Pi
        --------------------------------------------------*/
      fTemp = pfPhaseMid[i]*INVTWOPI;
      VoxROUND2( fTemp, iMhat );
      pfPhaseMid[i] -= iMhat*TWOPI;

      /*--------------------------------------------------
        Update the phase shift 
        --------------------------------------------------*/
      fPhaseShift += fDeltaPhase;
   }

}

/******************************************************************************
*
* Function: VoxDecPhasePrediction
*
* Action:  Estimate the 10ms phases from 20ms phases and 10ms predictive errors.
*
* Input:  fPitchMid -- middle-frame pitch
*         fPitch    -- end-frame pitch
*         pfPhaseMid -- predictive phase errors
*         pfPhase -- end-frame phases
*
* Output: pfPhaseMid -- middle-frame phases
*         pfPhase    -- end-frame phases
*
* Globals:   none
*
* Return:    none
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

void VoxDecPhasePrediction(float fPitchMid, float fPitch, 
                           float *pfPhaseMid, float *pfPhase)
{
   int i;

   VoxEncPhasePrediction(fPitchMid, fPitch, pfPhaseMid, pfPhase);

   /*--------------------------------------------------
     Normalize the mid-frame phase between 0 to 2*Pi
    --------------------------------------------------*/
   for (i = 0; i < SC6_PHASES; i++)
     {
        while(pfPhaseMid[i] < 0.0F)
           pfPhaseMid[i] += TWOPI;
     }
}
