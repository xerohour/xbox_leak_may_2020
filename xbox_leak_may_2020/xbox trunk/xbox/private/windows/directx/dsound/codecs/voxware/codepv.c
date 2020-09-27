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
* Filename:        CodePv.c
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/CodePv.c_v   1.6   13 Apr 1998 16:17:40   weiwang  $
*
******************************************************************************/

#include <stdlib.h>
#include <assert.h>

/* Use scalar quantizer. ==> I need to redesign it use better condition */
#include "vLibQuan.h"

#include "quansc36.h"
#include "CodePv.h"

#include "xvocver.h"

/* 3 bits Pv + 1 bits frame-fill. */
#define LENGTH_PV_CB              (int)(1<<(BITS_PV_SC3-1))

static const float PvCodeBook[LENGTH_PV_CB] = {
  0.0F, 0.143F, 0.286F, 0.429F, 0.571F, 0.714F, 0.857F, 1.0F
};

#ifndef DECODE_ONLY
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

int QuanPvSC3(  float Pv_1,  float Pv_Mid, float Pv)
{
  int index, index_ff = 0;
  float t1, t2;

  /* scalar-quantize the end frame Pv */
  index = ScalarQuant(Pv, PvCodeBook, LENGTH_PV_CB );
  
  /* then do one bit frame-fill of the middle-frame Pv */
  t1 = Pv_1-Pv_Mid;
  t1 = t1*t1;

  t2 = PvCodeBook[index]-Pv_Mid;
  t2 = t2*t2;

  if (t2 < t1)
    index_ff = 1;

  /* pack the index */
  index += (index_ff<<(BITS_PV_SC3-1));

  return index;
}
#endif

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

void UnQuanPvSC3(  int index,  float Pv_1, float *Pv_Mid, float *Pv)
{
  int index1, index2;

  assert(Pv_Mid != NULL);
  assert(Pv != NULL);

  /* unpack the bits */
  index2 = index & (LENGTH_PV_CB);
  index1 = index - index2;
  
  /* unquan the 3-bit scalar quantizer */
  *Pv = ScalarLUT( PvCodeBook, LENGTH_PV_CB, index1);
  
  /* unquan the 1-bit frame-fill */
  if (index2 == 0)
    *Pv_Mid = Pv_1;
  else
    *Pv_Mid = *Pv;
}
