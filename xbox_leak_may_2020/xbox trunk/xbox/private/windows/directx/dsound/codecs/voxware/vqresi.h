/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:        VqResi.h
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/VqResi.h_v   1.0   06 Feb 1998 10:11:42   johnp  $
*
******************************************************************************/
#ifndef VQRESI_H
#define VQRESI_H

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

void VoxASKResiEnc(const float *pfASKq, const float *pfASKorig, int *piIndex);

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

void VoxASKResiDec(float *pfASKq, const int *piIndex);

#endif /* VQRESI_H */

