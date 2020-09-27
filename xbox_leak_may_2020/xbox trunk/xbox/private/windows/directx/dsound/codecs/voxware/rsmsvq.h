/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:    RSMSVq.h
*
* Purpose:  quantize / unquantize vector using the split 2 Stage rotation and 
*           scaling VQ.
*
* Functions: RS2StageVQ2Enc and RS2StageVQ2Dec
*
* Author/Date: Original written by Xiaoqin Sun inside MSVqPred.c, 1997
*              Organize and Clean up code by Wei Wang, Feb. 4, 1998
*******************************************************************************
*
* Modifications: 
*
* Comments: The MSVQ uses two stage VQ. Each stage has two saparate codebooks 
*           for inner cells and outer cells.  Before searching second code books
*           residual vectors are rotated and scaled by multiplying a pretrained 
*           rotate-scaleer matrix. Each time, first stage codebook has two 
*           possible outputs (two-survivors) as candidates to be second codebook
*           entries to find the best ultimate output vector. 
*
* Concerns: Hard-wired only for dimension equal to 2
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/RSMSVq.h_v   1.1   26 Feb 1998 13:48:22   bobd  $
*
******************************************************************************/

#ifndef _RSMSVQ_H_
#define _RSMSVQ_H_

/******************************************************************************
*
* Function: RS2StageVQ2Enc
*
* Action: code vector using Rotation-Scaling 2 Stage VQ  
*
* Input:  piIndices -- pointer to the quantization indices
*         pfData -- input coefficients (or vector)
*         iNumCand -- number of M-Best candidates
*         iStage1Size -- size of the stage I codebook
*         pfStage1CB -- stage I codebook
*         pfRotScaleCB -- rotation-scaling codebook
*         iStage2Size -- size of the stage II codebook
*         pfStage2InCellCB -- stage II inner cell codebook
*         pfStage2OutCellCB -- stage II outer cell codebook
*         iNumOfOutCell -- number of outer cell in the stage I codebook
*         piIndexOfStage2OutCell -- the index for the outer cell in stage 
*                                     I codebook
*
* Output: piIndices -- quantization indices
*         pfData -- quantized coefficients 
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void RS2StageVQ2Enc( STACK_R
                     int         *piIndices,
                     float       *pfData,
                     int          iNumCand,
                     int          iStage1Size,
                     const float *pfStage1CB,
                     const float *pfRotScaleCB,
                     int          iStage2Size,
                     const float *pfStage2InCellCB,
                     const float *pfStage2OutCellCB,
                     int          iNumOfOutCell,
                     const int   *piIndexOfStage2OutCell
                   );

/******************************************************************************
*
* Function: RS2StageVQ2Dec
*
* Action: decode vector using Rotation-Scaling 2 Stage VQ  
*
* Input:  piIndices -- the quantization indices
*         pfData -- pointer to the output coefficients (or vector)
*         iStage1Size -- entries in stage 1 codebook
*         pfStage1CB -- stage I codebook
*         pfRotScaleCB -- rotation-scaling codebook
*         iStage2Size -- entries in stage II codebooks
*         pfStage2InCellCB -- stage II inner cell codebook
*         pfStage2OutCellCB -- stage II outer cell codebook
*         iNumOfOutCell -- number of outer cell in the stage I codebook
*         piIndexOfStage2OutCell -- the index for the outer cell in stage 
*                                      I codebook
*
* Output:   pfData -- quantized coefficients 
*
* Globals:   none
*
* Return:    none
*******************************************************************************/
void RS2StageVQ2Dec( STACK_R
                     const int   *piIndices,
                     float       *pfData,
                     int          iStage1Size,
                     const float *pfStage1CB,
                     const float *pfRotScaleCB,
                     int          iStage2Size,
                     const float *pfStage2InCellCB,
                     const float *pfStage2OutCellCB,
                     int          iNumOfOutCell,
                     const int   *piIndexOfStage2OutCell
                   );

#endif

