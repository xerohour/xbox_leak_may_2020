/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:    MSVqPred.h
*
* Purpose:     quantization coefficients using multi-stage predictive VQ
*
* Functions:   VoxMSVQEncPred
*              VoxMSVQDecPred
*
* Author/Date:     Bob Dunn 7/18/97
*******************************************************************************
*
* Modifications:   Write MSVQ encode and decode functions 
*                  Xiaoqin Sun   7/25/97
*
*                  Wei Wang, 1/30/98, Rewrite whole code and simplify the 
*                  initialization by adding "ASKCBSC3.h".
*
* Comments: 
*
* Concerns: the dimension for sub-vector is 2.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/MSVqPred.h_v   1.1   16 Mar 1998 15:58:08   weiwang  $
*
******************************************************************************/

#ifndef MSVQPRED_H
#define MSVQPRED_H


/******************************************************************************
*
* Function: VoxMSVQEncPred
*
* Action:   quantize vector using split and predicted 2-stage Rotation-Scaling VQ.
*
* Input:  pfCoef -- input vector
*         piIndices -- the pointer for the output indices.
*         iOrder -- order of the vector
*         fPv -- voicing probability.
*
* Output: piIndices -- quantization indices.
*
* Globals:   none
*
* Return:    none
*******************************************************************************/

void VoxMSVQEncPred(STACK_R float *pfCoef, int *piIndices, 
                    int iOrder, float fPv);

/******************************************************************************
*
* Function:  VoxMSVQDecPred
*
* Action:    unquantize the vector
*
* Input:   piIndices -- input quantization indices
*          pfCoef -- pointer for the output vector
*          iOrder -- the order for vector
*          fPv -- voicing probability
*
* Output:  pfCoef -- quantized vector
*
* Globals:   none
*
* Return:    none
*******************************************************************************/

void VoxMSVQDecPred(STACK_R const int *piIndices, float *pfCoef, 
                    int iOrder, float fPv);

#endif /* MSVQPRED_H */

