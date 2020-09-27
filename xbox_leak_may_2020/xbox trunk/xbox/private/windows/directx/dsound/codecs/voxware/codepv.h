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
* Filename:        CodePv.h
*
* Purpose:         quantize / unquantize voicing vector
*
* Functions:   QuanPvSC3(), UnQuanPvSC3()
*
* Author/Date:     Wei Wang,   06/10/97
*
*******************************************************************************
*
* Modifications: Clean up the program by Wei Wang, 01/27/98
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/CodePv.h_v   1.4   13 Apr 1998 16:17:40   weiwang  $
*
******************************************************************************/

#ifndef _CODEPV_H_
#define _CODEPV_H_

/******************************************************************************
*
* Function:  int QuanPvSC3(  float Pv_1,  float Pv_Mid, float Pv)
*
* Action:    Quantize voicing for SC36
*
* Input:     Pv_1 -- voicing of the previous frame
*            Pv_Mid -- voicing of the middle frame
*            Pv     -- voicing of the end frame
*
* Output:    same as the return
*
* Globals:   none
*
* Return:    index from the voicing quantizer
******************************************************************************/

int QuanPvSC3(  float Pv_1,  float Pv_Mid, float Pv);


/******************************************************************************
*
* Function:  void UnQuanPvSC3( int index,  float Pv_1,
*                              float *Pv_Mid, float *Pv)
*
* Action:    Unquantize voicing for SC36
*
* Input:     index -- the quantization index for Pvs.
*            Pv_1 -- voicing of the previous frame
*            Pv_Mid -- pointer to the voicing of the middle frame
*            Pv     -- pointer to the voicing of the end frame
*
* Output:    Pv_Mid -- voicing of the middle frame
*            Pv     -- voicing of the end frame
*
* Globals:   none
*
* Return:    none
*******************************************************************************/

void UnQuanPvSC3(  int index,  float Pv_1, float *Pv_Mid, float *Pv);


#endif
