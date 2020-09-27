/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:    ASKCBSC3.h
*
* Purpose:     structures and tables for MSVQPred.c
*
* Functions:   
*
* Author/Date:    Wei Wang, 02/09/98
*******************************************************************************
*
* Modifications:  
*
* Comments: 
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/ASKCBSC3.h_v   1.0   06 Feb 1998 10:11:26   johnp  $
*
******************************************************************************/

#ifndef _ASKCBSC3_H_
#define _ASKCBSC3_H_

/*************************************************************
  The structure for rotation-scaling 2 stages MSVQ codebook
  ************************************************************/
typedef struct {
  const int iStage1Size;
  const float *pfStage1CB;
  const float *pfRotScaleCB;

  const int iStage2Size;
  const float *pfStage2InCellCB;
  const float *pfStage2OutCellCB;

  const int iNumOfOutCell;
  const int *piIndexOfStage2OutCell;
} RS2StageVQ_MBlk;


/*************************************************************
  The structure for VQ codebook
  ************************************************************/
typedef struct {
  const int iCodebookSize;
  const float *fCodeBook;
} VQ2_MBlk;



/*--------------------------------------------
  Number of sub vectors
--------------------------------------------*/
#define SUB_DIMENSION  2
#define SUB_VECTORS (LPC_ORDER/SUB_DIMENSION)
#define MSVQ_PAIRS  4

/*--------------------------------------------------
  Number of candidates
  --------------------------------------------------*/
#define NUMCAND  2


/******************************************************************************
*
*  These numbers are hard wired for 12th order. 
*
******************************************************************************/

/*************************************************************
  Means 
  ************************************************************/
/* Voiced ASK Means */
static const float tab_ASK12_V_Means[] = {
#include "MASK12V.h"
};
/* Unvoiced ASK Means */
static const float tab_ASK12_UV_Means[] = {
#include "MASK12UV.h"
};

/*************************************************************
  Predictor 
  ************************************************************/
/* Voiced Codebook Predictors (2+4+...+10)*2 = 60 words */ 
static const float tab_ASK12_V_Predictors[] = {
#include "PASK12V.h"
};

static const float *ptab_ASK12_V_Pred[SUB_VECTORS] = {
  NULL,
  &(tab_ASK12_V_Predictors[0]), &(tab_ASK12_V_Predictors[2*2]),
  &(tab_ASK12_V_Predictors[(2+4)*2]), &(tab_ASK12_V_Predictors[(2+4+6)*2]),
  &(tab_ASK12_V_Predictors[(2+4+6+8)*2])
};

/* Unvoiced Codebook Predictors (2+4+...+10)*2 = 60 words */ 
static const float tab_ASK12_UV_Predictors[] = {
#include "PASK12UV.h"
};

static const float *ptab_ASK12_UV_Pred[SUB_VECTORS] = {
  NULL,
  &(tab_ASK12_UV_Predictors[0]), &(tab_ASK12_UV_Predictors[2*2]),
  &(tab_ASK12_UV_Predictors[(2+4)*2]), &(tab_ASK12_UV_Predictors[(2+4+6)*2]),
  &(tab_ASK12_UV_Predictors[(2+4+6+8)*2])
};


/*************************************************************
  1st pair (ASK[0-1]) codebooks -- voiced
  (1) Stage 1 codebook  (5bits)
  (2) Stage 1 rotation and scaling codebook (5bits)
  (3) Stage 2 inner cell codebook (5bits)
  (4) Stage 2 outter cell codebook (5bits)
  ************************************************************/
static const float tab_ASK0_1_V_CB[] = {
#include "ASK0_1V.h"
};

static const int tab_ASK0_1_V_OutCellIndex[] = {
#include "ASK0_1Vi.h"
};

static const RS2StageVQ_MBlk ASK0_1_V_mblk = {
  (1<<STAGE1_BITS_ASKS_0_1_SC36), 
  &(tab_ASK0_1_V_CB[0]),
  &(tab_ASK0_1_V_CB[SUB_DIMENSION*(1<<STAGE1_BITS_ASKS_0_1_SC36)]),
  (1<<STAGE2_BITS_ASKS_0_1_SC36), 
  &(tab_ASK0_1_V_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_0_1_SC36))]),
  &(tab_ASK0_1_V_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_0_1_SC36)+(1<<STAGE2_BITS_ASKS_0_1_SC36))]),
  sizeof(tab_ASK0_1_V_OutCellIndex)/sizeof(int),
  tab_ASK0_1_V_OutCellIndex
};


/*************************************************************
  1st pair (ASK[0-1]) codebooks -- unvoiced
  (1) Stage 1 codebook  (5bits)
  (2) Stage 1 rotation and scaling codebook (5bits)
  (3) Stage 2 inner cell codebook (5bits)
  (4) Stage 2 outter cell codebook (5bits)
  ************************************************************/
static const float tab_ASK0_1_UV_CB[] = {
#include "ASK0_1U.h"
};

static const int tab_ASK0_1_UV_OutCellIndex[] = {
#include "ASK0_1Ui.h"
};

static const RS2StageVQ_MBlk ASK0_1_UV_mblk = {
  (1<<STAGE1_BITS_ASKS_0_1_SC36), 
  &(tab_ASK0_1_UV_CB[0]),
  &(tab_ASK0_1_UV_CB[SUB_DIMENSION*(1<<STAGE1_BITS_ASKS_0_1_SC36)]),
  (1<<STAGE2_BITS_ASKS_0_1_SC36), 
  &(tab_ASK0_1_UV_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_0_1_SC36))]),
  &(tab_ASK0_1_UV_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_0_1_SC36)+(1<<STAGE2_BITS_ASKS_0_1_SC36))]),
  sizeof(tab_ASK0_1_UV_OutCellIndex)/sizeof(int),
  tab_ASK0_1_UV_OutCellIndex
};


/*************************************************************
  2nd pair (ASK[2-3]) codebooks
  (1) Stage 1 codebook  (5bits)
  (2) Stage 1 rotation and scaling codebook (5bits)
  (3) Stage 2 inner cell codebook (4bits)
  (4) Stage 2 outter cell codebook (4bits)
  ************************************************************/
static const float tab_ASK2_3_CB[] = {
#include "ASK2_3.h"
};

static const int tab_ASK2_3_OutCellIndex[] = {
#include "ASK2_3ix.h"
};

static const RS2StageVQ_MBlk ASK2_3_mblk = {
  (1<<STAGE1_BITS_ASKS_2_3_SC36), 
  &(tab_ASK2_3_CB[0]),
  &(tab_ASK2_3_CB[SUB_DIMENSION*(1<<STAGE1_BITS_ASKS_2_3_SC36)]),
  (1<<STAGE2_BITS_ASKS_2_3_SC36), 
  &(tab_ASK2_3_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_2_3_SC36))]),
  &(tab_ASK2_3_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_2_3_SC36)+(1<<STAGE2_BITS_ASKS_2_3_SC36))]),
  sizeof(tab_ASK2_3_OutCellIndex)/sizeof(int),
  tab_ASK2_3_OutCellIndex
};


/*************************************************************
  3rd pair (ASK[4-5]) codebooks
  (1) Stage 1 codebook  (5bits)
  (2) Stage 1 rotation and scaling codebook (5bits)
  (3) Stage 2 inner cell codebook (3bits)
  (4) Stage 2 outter cell codebook (3bits)
  ************************************************************/
static const float tab_ASK4_5_CB[] = {
#include "ASK4_5.h"
};

static const int tab_ASK4_5_OutCellIndex[] = {
#include "ASK4_5ix.h"
};

static const RS2StageVQ_MBlk ASK4_5_mblk = {
  (1<<STAGE1_BITS_ASKS_4_5_SC36), 
  &(tab_ASK4_5_CB[0]),
  &(tab_ASK4_5_CB[SUB_DIMENSION*(1<<STAGE1_BITS_ASKS_4_5_SC36)]),
  (1<<STAGE2_BITS_ASKS_4_5_SC36), 
  &(tab_ASK4_5_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_4_5_SC36))]),
  &(tab_ASK4_5_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_4_5_SC36)+(1<<STAGE2_BITS_ASKS_4_5_SC36))]),
  sizeof(tab_ASK4_5_OutCellIndex)/sizeof(int),
  tab_ASK4_5_OutCellIndex
};

/*************************************************************
  4th pair (ASK[6-7]) codebooks
  (1) Stage 1 codebook  (5bits)
  (2) Stage 1 rotation and scaling codebook (5bits)
  (3) Stage 2 inner cell codebook (2bits)
  (4) Stage 2 outter cell codebook (2bits)
  ************************************************************/
static const float tab_ASK6_7_CB[] = {
#include "ASK6_7.h"
};

static const int tab_ASK6_7_OutCellIndex[] = {
#include "ASK6_7ix.h"
};

static const RS2StageVQ_MBlk ASK6_7_mblk = {
  (1<<STAGE1_BITS_ASKS_6_7_SC36), 
  &(tab_ASK6_7_CB[0]),
  &(tab_ASK6_7_CB[SUB_DIMENSION*(1<<STAGE1_BITS_ASKS_6_7_SC36)]),
  (1<<STAGE2_BITS_ASKS_6_7_SC36), 
  &(tab_ASK6_7_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_6_7_SC36))]),
  &(tab_ASK6_7_CB[SUB_DIMENSION*(2*(1<<STAGE1_BITS_ASKS_6_7_SC36)+(1<<STAGE2_BITS_ASKS_6_7_SC36))]),
  sizeof(tab_ASK6_7_OutCellIndex)/sizeof(int),
  tab_ASK6_7_OutCellIndex
};



/*************************************************************
  Last 2 pairs (8-9, 10-11), single stage 5 bits VQ 
  ************************************************************/
static const float tab_ASK8_9_CB[SUB_DIMENSION*(1<<STAGE1_BITS_ASKS_8_9_SC36)]={
#include "ASK8_9.h"
};
static const float tab_ASK10_11_CB[SUB_DIMENSION*(1<<STAGE1_BITS_ASKS_10_11_SC36)]={
#include "ASK10_11.h"
};


static const VQ2_MBlk ASK8_9_mblk = {
  (1<<STAGE1_BITS_ASKS_8_9_SC36), 
  tab_ASK8_9_CB 
};

static const VQ2_MBlk ASK10_11_mblk = {
  (1<<STAGE1_BITS_ASKS_10_11_SC36), 
  tab_ASK10_11_CB 
};

static const VQ2_MBlk *pASK8_11_VQ[2] = {
  &ASK8_9_mblk,
  &ASK10_11_mblk
};


#endif
