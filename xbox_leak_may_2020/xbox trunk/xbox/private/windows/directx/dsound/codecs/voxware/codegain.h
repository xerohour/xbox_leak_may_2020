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
* Filename:        CodeGain.h
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/CodeGain.h_v   1.1   16 Mar 1998 15:25:54   weiwang  $
*
******************************************************************************/

#ifndef _CODEGAIN_H_
#define _CODEGAIN_H_


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
*******************************************************************************/

int QuanGainSC3(float flog2Gain);


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
*******************************************************************************/

float UnQuanGainSC3(int iIndex);

#endif
