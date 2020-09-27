/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:          QPitchSC3.h
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/QP0SC3.h_v   1.2   17 Aug 1998 18:14:18   weiwang  $
* 
******************************************************************************/
#ifndef QPITCHSC_H
#define QPITCHSC_H

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
int VoxQuanPitchSC3( float fPitch);


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
float VoxUnQuanPitchSC3( int iIndex);

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
*******************************************************************************/
float UnQuanMidPitchSC3( float fPitch_1,  float fPitch, 
                         float fPv_1,  float fPv);


#endif /* QPITCHSC_H */
 
