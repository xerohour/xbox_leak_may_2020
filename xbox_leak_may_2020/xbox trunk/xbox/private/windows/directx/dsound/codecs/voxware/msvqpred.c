/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:    MSVqPred.c
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/MSVqPred.c_v   1.5   08 Apr 1998 11:03:02   weiwang  $
*
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vLibQuan.h"
#include "VoxMem.h"

#include "codec.h"
#include "quansc36.h"
#include "CodeStr.h"

#include "RSMSVq.h"
#include "MSVqPred.h"

#include "ASKCBSC3.h"

#include "xvocver.h"

#define PV_THRESHOLD 0.8F

static void CompPredCoef(const float *fZMCoef, int iNumPredOrd, 
                         const float *tab_Pred, float *pfPredCoef, int iSubDim);

#ifndef DECODE_ONLY
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

void VoxMSVQEncPred(STACK_R float *pfCoef, int *piIndices, 
                    int iOrder, float fPv)
{
  float *tab_Means;
  float **tab_Pred;
  int iFirstLag;
  int i, j;

STACK_INIT
  float  pfZMCoef[LPC_ORDER];
  float  pfPredCoef[SUB_DIMENSION];
  RS2StageVQ_MBlk * ASKCB_mblk[MSVQ_PAIRS];

STACK_INIT_END
  STACK_ATTACH (float *, pfZMCoef)
  STACK_ATTACH (float *, pfPredCoef)
  STACK_ATTACH (RS2StageVQ_MBlk **, ASKCB_mblk)

STACK_START

  assert(iOrder == (SUB_VECTORS*SUB_DIMENSION));
  assert(SUB_DIMENSION == 2);
  assert((pfCoef != NULL) && (piIndices != NULL));

  /* choose the tables by comparing the voicing */
  if (fPv>PV_THRESHOLD)
    {
      tab_Means = (float *)tab_ASK12_V_Means;
      tab_Pred = (float **)ptab_ASK12_V_Pred;
      ASKCB_mblk[0] = (RS2StageVQ_MBlk *)&ASK0_1_V_mblk;
    } 
  else 
    {
      tab_Means = (float *)tab_ASK12_UV_Means;
      tab_Pred = (float **)ptab_ASK12_UV_Pred;
      ASKCB_mblk[0] = (RS2StageVQ_MBlk *)&ASK0_1_UV_mblk;
    }
   
   ASKCB_mblk[1] = (RS2StageVQ_MBlk *)&ASK2_3_mblk;
   ASKCB_mblk[2] = (RS2StageVQ_MBlk *)&ASK4_5_mblk;
   ASKCB_mblk[3] = (RS2StageVQ_MBlk *)&ASK6_7_mblk;


   /*--------------------------------------------
     Mean remover
     --------------------------------------------*/
   for (i=0; i<iOrder; i++)
     pfZMCoef[i] = pfCoef[i] - tab_Means[i];

   /*--------------------------------------------
     Encode each subvector
   --------------------------------------------*/
   iFirstLag = 0;

   for (i = 0; i < SUB_VECTORS; i++)
     {
       /*--------------------------------------------
         predict the ASK values and find the residual
         --------------------------------------------*/
       CompPredCoef(pfZMCoef, iFirstLag, tab_Pred[i], pfPredCoef, SUB_DIMENSION);

       pfZMCoef[iFirstLag] -= pfPredCoef[0];
       pfZMCoef[iFirstLag+1] -= pfPredCoef[1];
 
       if ( i < MSVQ_PAIRS) {
         /*--------------------------------------------
           Encode the ASK residual using RSMSVQ
           --------------------------------------------*/
         RS2StageVQ2Enc(STACK_S &(piIndices[iFirstLag]), &pfZMCoef[iFirstLag], 
                        NUMCAND, ASKCB_mblk[i]->iStage1Size, 
                        ASKCB_mblk[i]->pfStage1CB, ASKCB_mblk[i]->pfRotScaleCB,
                        ASKCB_mblk[i]->iStage2Size,
                        ASKCB_mblk[i]->pfStage2InCellCB, 
                        ASKCB_mblk[i]->pfStage2OutCellCB,
                        ASKCB_mblk[i]->iNumOfOutCell, 
                        ASKCB_mblk[i]->piIndexOfStage2OutCell);
       }
       else {
         /*--------------------------------------------------
           Encode the residual using 2-dimensional VQ 
           --------------------------------------------------*/
         j = i - MSVQ_PAIRS;
         piIndices[iFirstLag] = Dim2VectorQuantize(&pfZMCoef[iFirstLag],
                                      pASK8_11_VQ[j]->fCodeBook, 
                                      pASK8_11_VQ[j]->iCodebookSize);

         piIndices[iFirstLag+1] = 0;   /* single stage VQ */

        /*--------------------------------------------
          Decode the residual for the next sub-vector.
          --------------------------------------------*/
         Dim2VectorLUT( piIndices[iFirstLag], 
                        pASK8_11_VQ[j]->fCodeBook, 
                        pASK8_11_VQ[j]->iCodebookSize,
                        &pfZMCoef[iFirstLag]);
       }

       /*---------------------------------------------
         Compute the quantized coefficients.
        ---------------------------------------------*/
       pfZMCoef[iFirstLag++] += pfPredCoef[0];
       pfZMCoef[iFirstLag++] += pfPredCoef[1];
     }

   /*--------------------------------------------------
     add the mean vector
     --------------------------------------------------*/
   for (i = 0; i < iOrder; i++)
     pfCoef[i] = pfZMCoef[i] + tab_Means[i];
    
   STACK_END
}

#endif

/******************************************************************************
*
* Function:  VoxMSVQDecPred
*
* Action:    unquantize coefficients
*
* Input:   piIndices -- input quantization indices
*          pfCoef -- pointer for the output vector
*          iOrder -- the order for the vector
*          fPv -- voicing probability
*
* Output:  pfCoef -- quantized vector
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

void VoxMSVQDecPred(STACK_R const int *piIndices, float *pfCoef, 
                    int iOrder, float fPv)
{
  float *tab_Means;
  float **tab_Pred;
  int i, j; 
  int iFirstLag;

STACK_INIT
  float  pfZMCoef[LPC_ORDER];
  float  pfPredCoef[SUB_DIMENSION];
  RS2StageVQ_MBlk *ASKCB_mblk[MSVQ_PAIRS];

STACK_INIT_END
  STACK_ATTACH (float *, pfZMCoef)
  STACK_ATTACH (float *, pfPredCoef)
  STACK_ATTACH (RS2StageVQ_MBlk **, ASKCB_mblk)

STACK_START

  assert(iOrder == (SUB_VECTORS*SUB_DIMENSION));
  assert(SUB_DIMENSION == 2);
  assert((piIndices != NULL) && (pfCoef != NULL));

  /* choose the tables by comparing the voicing */
  if (fPv>PV_THRESHOLD)
    {
      tab_Means = (float *)tab_ASK12_V_Means;
      tab_Pred = (float **)ptab_ASK12_V_Pred;
      ASKCB_mblk[0] = (RS2StageVQ_MBlk *)&ASK0_1_V_mblk;
    } 
  else 
    {
      tab_Means = (float *)tab_ASK12_UV_Means;
      tab_Pred = (float **)ptab_ASK12_UV_Pred;
      ASKCB_mblk[0] = (RS2StageVQ_MBlk *)&ASK0_1_UV_mblk;
    }
   
  ASKCB_mblk[1] = (RS2StageVQ_MBlk *)&ASK2_3_mblk;
  ASKCB_mblk[2] = (RS2StageVQ_MBlk *)&ASK4_5_mblk;
  ASKCB_mblk[3] = (RS2StageVQ_MBlk *)&ASK6_7_mblk;


  /*--------------------------------------------
    Decode each subvector
   --------------------------------------------*/
  iFirstLag = 0;

  for (i = 0; i < SUB_VECTORS; i++)
     {
       if (i < MSVQ_PAIRS) {

         /*--------------------------------------------------
           Decode residual using RSMSVQ.
           --------------------------------------------------*/
         RS2StageVQ2Dec(STACK_S &(piIndices[iFirstLag]), 
                        &(pfZMCoef[iFirstLag]), 
                        ASKCB_mblk[i]->iStage1Size,
                        ASKCB_mblk[i]->pfStage1CB,
                        ASKCB_mblk[i]->pfRotScaleCB,
                        ASKCB_mblk[i]->iStage2Size,
                        ASKCB_mblk[i]->pfStage2InCellCB,
                        ASKCB_mblk[i]->pfStage2OutCellCB,
                        ASKCB_mblk[i]->iNumOfOutCell,
                        ASKCB_mblk[i]->piIndexOfStage2OutCell);
       }
       else {
          j = i-MSVQ_PAIRS;
         /*--------------------------------------------
           Decode the residual using 2-dimensional VQ
           --------------------------------------------*/
         Dim2VectorLUT( piIndices[iFirstLag], 
                        pASK8_11_VQ[j]->fCodeBook, 
                        pASK8_11_VQ[j]->iCodebookSize,
                        &pfZMCoef[iFirstLag]);
       }
         
       /*--------------------------------------------
         predict the ASK values
         --------------------------------------------*/
         CompPredCoef(pfZMCoef, iFirstLag, 
                      tab_Pred[i], pfPredCoef, SUB_DIMENSION);

       /*--------------------------------------------
         Add predicted value to decoded residual
         --------------------------------------------*/
        pfZMCoef[iFirstLag++] += pfPredCoef[0];
        pfZMCoef[iFirstLag++] += pfPredCoef[1];
     }

  /*--------------------------------------------
    Add the mean value to the ASKs
    --------------------------------------------*/
  for (i=0; i<iOrder; i++)
    pfCoef[i] = pfZMCoef[i] + tab_Means[i];

STACK_END
}

/******************************************************************************
*
* Function:  CompPredCoef
*
* Action:    Compute the predictor based on the previous coefficients.
*
* Input:     pfZMCoef -- input zero-mean coefficients.
*            iNumPredOrd -- the predictor order
*            tab_Pred -- the table for predictor
*            pfPredCoef -- the address for the output prediction coefficients
*            iSubDim -- the dimension for sub-vector
*
* Output:    pfPredCoef -- output prediction coefficients
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
* Concerns/TBD:   tab_Pred may be nil pointer when iNumPredOrd == 0.
******************************************************************************/
static void CompPredCoef(const float *pfZMCoef, int iNumPredOrd, 
                         const float *tab_Pred, 
                         float *pfPredCoef, int iSubDim)
{
  register int i, j;
  register float rPred;
  float *pfPredTab;

  assert(pfZMCoef != NULL && pfPredCoef != NULL);

  for (i = 0; i < iSubDim; i++) {
    rPred = 0.0F;
    pfPredTab = (float *)&(tab_Pred[i*iNumPredOrd]);
    for (j = 0; j < iNumPredOrd; j++)
      rPred += pfPredTab[j] * pfZMCoef[j];

    pfPredCoef[i] = rPred;
  }
}
