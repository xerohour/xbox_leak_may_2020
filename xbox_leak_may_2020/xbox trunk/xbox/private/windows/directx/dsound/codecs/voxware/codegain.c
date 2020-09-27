/******************************************************************************
*
*                        Voxware Proprietary Material
*
*                        Copyright 1996, Voxware, Inc
*
*                            All Rights Reserved
*
*
*
*                       DISTRIBUTION PROHIBITED without
*
*                      written authorization from Voxware
*
******************************************************************************/
 
/******************************************************************************
*
* Filename:        CodeGain.c
*
* Purpose:         quantize / unquantize gain
*
* Functions:   QuanGainSC3(),  UnQuanGainSC3()
*
* Author/Date:     Wei Wang,   06/10/97
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/CodeGain.c_v   1.3   20 Mar 1998 10:30:12   weiwang  $
*
******************************************************************************/

#include <assert.h>

#include "quansc36.h"
#include "vLibQuan.h"
#include "CodeGain.h"

#include "xvocver.h"


#define LENGTH_Gain_CB              (int)(1<<BITS_GAIN_SC3)


static const float GainCodeBook[LENGTH_Gain_CB] = {
#include "Gain6bit.h"
};

#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  int QuanGainSC3(float flog2Gain)
*
* Action:    Quantize gain for SC36 using scalar quantizer
*
* Input:     flog2Gain -- input log2(Gain) for quantization.
*
* Output:    same as the return
*
* Globals:   none
*
* Return:    index from the gain quantizer
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

int QuanGainSC3(float flog2Gain)
{
  int iIndex;

  iIndex = ScalarQuant(flog2Gain, GainCodeBook, LENGTH_Gain_CB);

  return iIndex;
}
#endif


/******************************************************************************
*
* Function:  float UnQuanGainSC3(int iIndex)
*
* Action:    unquantize gain for SC36
*
* Input:     iIndex -- index from gain quantizer.
*
* Output:    same as the return
*
* Globals:   none
*
* Return:    quantized log2(Gain)
*******************************************************************************
*
* Implementation/Detailed Description: 3-bits scalar quantizer + 1-bit frame-fill
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

float UnQuanGainSC3(int iIndex)
{
  float flog2Gain;

  flog2Gain = ScalarLUT( GainCodeBook, LENGTH_Gain_CB, (unsigned int) iIndex);
  
  return flog2Gain;
}
