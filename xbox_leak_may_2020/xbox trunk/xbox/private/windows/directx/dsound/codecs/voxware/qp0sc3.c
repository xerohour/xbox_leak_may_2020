/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:          QPitchSC3.c
*                                                                             
* Purpose:           Pitch Quantization for SC36
*                                                                             
* Functions:    VoxQuanPitchSC3, VoxUnQuanPitchSC3, UnQuanMidPitchSC3
*
* Author/Date:  
********************************************************************************
* Modifications:  (1) Change from Log10 to faster fLog2(). And simplify the 
*                 implementation.
*                 (2) Change fLog2() back to log() to support old bitstream.
* Comments:
* 
* Concerns: If not use linear-approximated fLog2, LOG2PMAX, LOG2STEP and 
*           INV_LOG2STEP have to be recalculated.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/QP0SC3.c_v   1.6   17 Aug 1998 18:14:16   weiwang  $
* 
******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "vLibMath.h"

#include "quansc36.h"
#include "QP0SC3.h"
#include "MidPitch.h"

#include "xvocver.h"

#define LOGExpPMAX         5.0751738F   /* Log(PMAX) = Log(160) */
#define LOGExpPMIN         2.0794415F         /* Log(PMIN) = Log(8) */
#define LOGExpSTEP         1.1747970e-2F  /* (LOGExpPMAX-LOGExpPMIN)/MaxLevel */
#define INV_LOGExpSTEP     85.121091F    /* MaxLevel/(LOGExpPMAX-LOGExpPMIN) */

#ifndef DECODE_ONLY
/******************************************************************************
* Function: VoxQuanPitchSC3
*
* Action:  Quantize pitch for SC36
*
* Input:   fPitch -- input pitch value
*
* Output:  same as return
*
* Globals:   none
*
* Return: pitch quantization index
*******************************************************************************
* Implementation/Detailed Description:
*   This is exponential pitch quantizer. 
*   Assume: (Pmax/Pitch)^(1/Level) = (Pmax/Pmin)^(1/MaxLevel), where Pmax = 160,
*   Pmin = 8, MaxLevel = 2^7-1=255. For quantization, we need get Level with 
*   known Pitch value.
*
*   Use log function, the equation can change to: 
*    (log(Pmax)-log(Pitch))/Level = (log(Pmax)-log(Pmin))/MaxLevel
*   Then quantization level can be solved by:
*     Level = (log(Pmax)-log(Pitch))/{(log(Pmax)-log(Pmin))/MaxLevel}
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/

int VoxQuanPitchSC3( float fPitch)
{
  float fLevel;
  int iIndex;

  /* truncate pitch value */
  fPitch = VoxMAX(fPitch, MIN_QUAN_PITCH_PERIOD_SC3);
  fPitch = VoxMIN(fPitch, MAX_QUAN_PITCH_PERIOD_SC3);

  /* convert pitch to level */
  fLevel = (float) (LOGExpPMAX - log(fPitch))*INV_LOGExpSTEP;

  /* round to integer level */
  VoxROUND2(fLevel, iIndex);

  return iIndex;
}
#endif

/******************************************************************************
* Function: VoxUnQuanPitchSC3
*
* Action: unquantize pitch for SC36
*
* Input:  iIndex -- pitch quantization index
*
* Output: same as return
*
* Globals: none
*
* Return: quantized pitch 
*******************************************************************************
* Implementation/Detailed Description:
*   Assume: (Pmax/Pitch)^(1/Level) = (Pmax/Pmin)^(1/MaxLevel), where Pmax = 160,
*   Pmin = 8, MaxLevel = 2^7-1=255. For unquantization, we need get Pitch with 
*   known quantization level.
*
*   Use log function, the equantion can change to:
*    (log(Pmax)-log(Pitch))/Level = (log(Pmax)-log(Pmin))/MaxLevel
*   Then log(Pitch) can be obtained by:
*     log(Pitch) = log(Pmax)- Level * (log(Pmax)-log(Pmin))/MaxLevel.
*
*   Use inverse of log -- exp, Pitch = exp(fLog2(Pmax) - Level * LOGExpSTEP).
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/

float VoxUnQuanPitchSC3( int iIndex)
{
  float fPitch;

  assert((iIndex >= 0) && (iIndex < (1<<BITS_PITCH_SC3)));

  fPitch = (float)exp(LOGExpPMAX-(float)iIndex*LOGExpSTEP);

  return fPitch;
}


/******************************************************************************
* Function: UnQuanMidPitchSC3
*
* Action: Use conditional interpolation to get the quantized middle frame pitch.
*
* Input:    fPitch_1 -- the quantized previous frame pitch
*           fPitch -- the quantized current frame pitch
*           fPv_1  -- the quantized previous voicing
*
* Output:   same as return
*
* Globals:        none
*
* Return: quantized middle frame pitch.
*******************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/

float UnQuanMidPitchSC3( float fPitch_1,  float fPitch, 
                         float fPv_1,  float fPv)
{
  return MidEstPitch(fPitch_1, fPitch, fPv_1, fPv);
}
