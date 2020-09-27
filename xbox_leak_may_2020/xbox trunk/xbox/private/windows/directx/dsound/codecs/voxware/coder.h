/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:        Coder.h
*
* Purpose:         Code parameters for both SC3 and SC6
*
* Functions:   VoxQuantSC3, VoxQuantSC6, VoxUnQuantSC3 and VoxUnQuantSC6,
*              VoxInitQuantizer and VoxFreeQuantizer
*
* Author/Date:     Bob Dunn 1/22/97
*******************************************************************************
*
* Modifications:  Clean up the code by Wei Wang, 1/27/98
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/Coder.h_v   1.3   06 Apr 1998 17:32:50   weiwang  $
******************************************************************************/

#ifndef _CODER_H
#define _CODER_H

/******************************************************************************
*
* Function:  VoxQuantSC3
*
* Action:    Quantize parameters for SC3
*
* Input:     STACK_R -- the pointer to the Voxware stack 
*            piQuanIndex -- quantization indices
*            pvParamMblk_1 -- parameter structure of previous frame
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    piQuanIndex -- the updated quantization indices
*            pvParamMblk -- quantized parameters (except gain)
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void VoxQuantSC3(STACK_R int *piQuanIndex, void *pvEncMblk,
                 void *pvParamMblk);


/******************************************************************************
*
* Function:  VoxQuantSC6
*
* Action:    Quantize parameters for SC6
*
* Input:     piQuanIndex -- pointer to quantization indices
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    piQuanIndex -- the updated quantization indices
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void VoxQuantSC6(int *piQuanIndex, void *pvParamMblk);


/******************************************************************************
*
* Function:  VoxUnQuantSC3
*
* Action:    Unquantize parameters for SC3
*
* Input:     STACK_R -- stack memory pointer
*            piQuanIndex -- quantization indices
*            pvParamMblk_1 -- parameter structure of previous frame
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    pvParamMblk -- quantized parameters 
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void VoxUnQuantSC3(STACK_R int *piQuanIndex, void *pvDecMblk,
                   void *pvParamMblk);

/******************************************************************************
*
* Function:  VoxUnQuantSC6
*
* Action:    Unquantize parameters for SC6
*
* Input:     piQuanIndex -- quantization indices
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    pvParamMblk -- pointer to the quantized parameters
*
* Globals:   none
*
* Return:    void
*******************************************************************************/
void VoxUnQuantSC6(int *piQuanIndex,void *pvParamMblk);

#endif /* _CODER_H */

