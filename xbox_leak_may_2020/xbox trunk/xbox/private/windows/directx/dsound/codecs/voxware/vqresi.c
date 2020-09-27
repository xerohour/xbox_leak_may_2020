/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:        VqResi.c
*
* Purpose:         quantize residual ASKs
*
* Functions:       VoxASKResiEnc and VoxASKResiDec
*
* Author/Date:     Bob Dunn 9/11/97
*******************************************************************************
*
* Modifications: Cleaned up the code by Wei Wang, 2/9/98
*
* Comments: 
*
* Concerns: The numbers are hard wired for 12th order. 
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/VqResi.c_v   1.4   08 Apr 1998 11:03:08   weiwang  $
*
******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "vLibQuan.h"

#include "codec.h"
#include "quansc36.h"
#include "VqResi.h"

#include "xvocver.h"

#define RES_ASKS_8_9_SIZE    (1<<RESIDUAL_BITS_ASKS_8_9_SC6)
static const float tabCB_8_9[RES_ASKS_8_9_SIZE*2] = {
 0.003142F,  0.002211F,
-0.052339F, -0.000010F,
-0.024635F, -0.048779F,
-0.015182F, -0.011979F,
 0.042617F,  0.002844F,
-0.020211F,  0.031112F,
 0.021640F,  0.042246F,
 0.020281F, -0.032090F
};

#define RES_ASKS_10_11_SIZE    (1<<RESIDUAL_BITS_ASKS_10_11_SC6)
static const float tabCB_10_11[RES_ASKS_10_11_SIZE*2] = {
-0.003104F, -0.002303F,
 0.016099F,  0.051439F,
 0.057398F,  0.008813F,
 0.017065F,  0.011489F,
 0.026144F, -0.028814F,
-0.044614F, -0.007490F,
-0.014048F, -0.042063F,
-0.022506F,  0.028976F
};


#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  VoxASKResiEnc
*
* Action:   quantize the residual for last two pairs of ASKs
*
* Input:  pfASKq -- quantized ASKs input vector
*         pfASKorig -- unquantized ASKs
*         piIndex -- the pointer to the output indices
*
* Output: piIndex -- output quantization indices
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

void VoxASKResiEnc(const float *pfASKq, const float *pfASKorig, int *piIndex)
{
   float fResi[2];

   assert((pfASKq != NULL) && (pfASKorig != NULL) && (piIndex != NULL));

   /* quantize the residual of ASK[8] and ASK[9] */
   fResi[0] = pfASKorig[8] - pfASKq[8];
   fResi[1] = pfASKorig[9] - pfASKq[9];
   piIndex[0] = Dim2VectorQuantize(fResi, tabCB_8_9, RES_ASKS_8_9_SIZE);

   /* quantize the residual of ASK[10] and ASK[11] */
   fResi[0] = pfASKorig[10] - pfASKq[10];
   fResi[1] = pfASKorig[11] - pfASKq[11];
   piIndex[1] = Dim2VectorQuantize( fResi, tabCB_10_11, RES_ASKS_10_11_SIZE);
}

#endif

/******************************************************************************
*
* Function:  VoxASKResiDec
*
* Action:   unquantize the residual for last two pairs of ASKs
*
* Input:  pfASKq -- quantized ASKs input vector
*         piIndex -- the quantization indices
*
* Output: pfASKq -- quantized ASKs (input vector + residual)
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

void VoxASKResiDec(float *pfASKq, const int *piIndex)
{
  float pfVector[2];

  assert((pfASKq != NULL) && (piIndex != NULL));

  Dim2VectorLUT(piIndex[0], tabCB_8_9, RES_ASKS_8_9_SIZE, pfVector);
  pfASKq[8] += pfVector[0];
  pfASKq[9] += pfVector[1];


  Dim2VectorLUT(piIndex[1], tabCB_10_11, RES_ASKS_10_11_SIZE, pfVector);
  pfASKq[10] += pfVector[0];
  pfASKq[11] += pfVector[1];
}
 
