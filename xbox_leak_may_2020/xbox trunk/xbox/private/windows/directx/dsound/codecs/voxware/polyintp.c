/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1997, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/*****************************************************************************
* Filename:   PolyIntp.c
*
* Purpose:    Floating Point Poly-Phase Cycle Interpolation.
*
* Functions:  PolyCycleIntp()
*
* Author/Date : Wei Wang, 2/26/98
*
*******************************************************************************
*
* Modifications:  (converted Rob Zopf's fixed point code)
*                      (of course, based on JGA's :-) )
*                 Based on Bob Dunn's code 10/25/96
* Comments: 
*
* Concerns: 
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/PolyIntp.c_v   1.8   20 Mar 1998 10:24:32   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vLibFilt.h"

#include "xvocver.h"

#define PP_RESAMP_ORDER                   6
#define PP_RESAMP_RATE                    (1<<PP_RESAMP_ORDER)

#define PP_SHIFT                          16
#define PP_SHIFT_SCALE                    (float)((long)1<<PP_SHIFT)
#define PP_SHIFT_MASK                     (((long)1<<PP_SHIFT)-1)
#define INV_PP_SHIFT_SCALE                (1.0F/PP_SHIFT_SCALE)

#define PP_WEIGHTS_INDEX_SHIFT            (PP_SHIFT-PP_RESAMP_ORDER)

static const float PPWts[PP_RESAMP_RATE*PP_NUM_SAMPLES] = {
#include "PPWeight.h"
};

/******************************************************************************
*
* Function:  PolyCycleIntp()
*
* Action:    Poly-Phase Cycle Interpolation.
*
* Input:    pfOut       - address to the resampling signal.
*           iOutLength  - the length of the resampling signal.
*           pfCycle     - input proto-type signal, usually refer to a single
*                         pitch epoch with 0 phase.
*           iLog2CycleLength 
*                       - log2 of the input signal length. 
*           fResampRate - the resampling ratio of the input cycle with
*                         the output cycle (calculated by input cycle length 
*                         over the pitch period).
*           fNormPhase  - the starting phase of the output signal 
*                         (calculated by starting time over the pitch period).
*
* Output:   pfOut       - the output data
*
* Globals:  none
*
* Return:   fNormPhase  - the ending phase of the resampled signal. If use 
*                         it as input for next PolyCycleIntp() call, the phases
*                         can be guaranteed to be continous.
*
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:  
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*     (1) In order to use fix point technique to speech up the function, the 
*       cycle length has to be in the order of 2 (2^iLog2CycleLength).
*     (2) To use LONG operation (32 bits), iOutLength, iCycleLength and fIndex
*       can not exceed 2^15.
*     (3) In order to speech up FIR operation, the real length of pfCycle has 
*       to be (2^iLog2CycleLength + PP_NUM_SAMPLES). The input pfCycle has 
*       already been shifted by PP_DELAY so that the address of 
*       pfCycle[-PP_DELAY] is valid.
*
* Concerns/TBD:
******************************************************************************/
float PolyCycleIntp( float       *pfOut,
                     int         iOutLength,
                     float       *pfCycle,
                     int         iLog2CycleLength,  
                     float       fResampRate,
                     float       fNormPhase
                   )
{  
  float fSum;
  float *pfW, *pfC;
  float fIndex;
  long lIndex, lResampFactor;
  int iCycleLength;
  long lScaledCycleMask;
  int i;

  /*-----------------------------------------------------
    Error checking :
    -----------------------------------------------------*/
   assert(pfOut != NULL && pfCycle != NULL);
   assert(iLog2CycleLength < 15);  /* to make sure no overflow */
   assert(PP_NUM_SAMPLES == 4);  /* the code is hardwired only for 4 */

   /*--------------------------------------------------
     Get the prototype cycle length:
    --------------------------------------------------*/
   iCycleLength        = 1<<iLog2CycleLength;
   lScaledCycleMask    = ((long)1<<(iLog2CycleLength+PP_SHIFT))-1;


   /*-------------------------------------------------        
     Extend Cycle in both forward & Backward  
     direction for extrapolation in upsampling 
    -------------------------------------------------*/
    pfCycle[iCycleLength]   = pfCycle[0];     
    pfCycle[iCycleLength+1] = pfCycle[1];    
    pfCycle[iCycleLength+2] = pfCycle[2];     
    pfCycle[-1]             = pfCycle[iCycleLength-1]; 
    pfCycle                -=PP_DELAY;   

    /*--------------------------------------------------
      Convert to fix point for speeding up:
      (1) Resample factor
      (2) start index at proto-type cycle.
      --------------------------------------------------*/
    lResampFactor   = (long)(PP_SHIFT_SCALE*fResampRate);

    fIndex          = fNormPhase * (float)iCycleLength;
    assert(fabs(fIndex)<(PP_SHIFT_SCALE*0.5F)); 

    lIndex          = (long)(PP_SHIFT_SCALE*fIndex);
    lIndex         &= lScaledCycleMask;
    
    /*--------------------------------------------------
      To interpolate the prototype cycle and get the
      output data.
      --------------------------------------------------*/
    while (iOutLength-- > 0)
      { 
        /*-------------------------------------------------------
          The length of prototype cycle is in order of 2. And
          the fractional part of lIndex is in the last 16 bits.
          So the weights' index can be calculated by:
          fract(lIndex) * PP_RESAMP_RATE / PP_SHIFT_SCALE
          ------------------------------------------------------*/
         i      = (int)((lIndex&PP_SHIFT_MASK)>>PP_WEIGHTS_INDEX_SHIFT);
         pfW    = (float *)&(PPWts[i<<2]);

        /*-----------------------------------------------------
          The integer part of lIndex is in the first 16 bits.
          To make sure the operation won't overflow, need to 
          make sure the integer index is between 
          0 to (iCycleLength - 1).
          -----------------------------------------------------*/
        i      = (int)(lIndex >> PP_SHIFT);
        pfC    = &(pfCycle[i]);
  
        /*--------------------------------------------------
          Convolve samples in the prototype cycle with 
          weights (hardwired to use 4 samples).
          --------------------------------------------------*/
        fSum    = (*pfC++**pfW++);
        fSum   += (*pfC++**pfW++);
        fSum   += (*pfC++**pfW++);
        fSum   += (*pfC++**pfW++);
        *pfOut++ = fSum;

        /*-------------------
          Update lIndex:
         -------------------*/
        lIndex += lResampFactor;
        lIndex &= lScaledCycleMask;
   }


    /*------------------------
      Get the ending phase:
      ------------------------*/
    lIndex     = lIndex >> iLog2CycleLength;  /* remove the cycle length */
    fNormPhase = (float)(lIndex * INV_PP_SHIFT_SCALE);

    return fNormPhase;
}


