/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:    RSMSVq.c
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/RSMSVq.c_v   1.4   08 Apr 1998 11:03:04   weiwang  $
*
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "VoxMem.h"
#include "vLibQuan.h"
#include "RSMSVq.h"

#include "xvocver.h"

#define MAX_NUMCAND   2
#undef FLT_MAX
#define FLT_MAX       3.40282E+38F
#define VQ2_DIMENSION 2

#ifndef DECODE_ONLY
static void VQ2mbest(STACK_R const float *pfData, const float *pfCB, 
                     int iCBSize, int *piMBestCand,  int iMBest);

static void RotScaleVQ2(const float *pfIn, const float *pfRSMatrix,
                        int iIndex, float *pfOut);

static float CompDistVQ2(const float *pfIn, const float *pfRef);
#endif

static void DeRotScaleVQ2(const float *pfIn, const float *pfRSMatrix,
                          int iIndex, float *pfOut);

static int SelectOutCellCB(const int *piIndexOfOutCell,
                           int iNumOfOutCell, 
                           int iIndex);

#ifndef DECODE_ONLY
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
* Concerns/TBD: only for 2-dimensional VQ
******************************************************************************/

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
                   )
{
  int i;
  int iBestCand;
  int iIndex;
  float fDis;
  float fMinDis;
  const float *pfStage2CB;
STACK_INIT
  int piMBestCand[MAX_NUMCAND];
  float pfStage1Out[VQ2_DIMENSION];
  float pfRes[VQ2_DIMENSION];
  float pfStage2In[VQ2_DIMENSION];
  float pfQuanVec[VQ2_DIMENSION];
STACK_INIT_END

  STACK_ATTACH(int *, piMBestCand)
  STACK_ATTACH(float *, pfStage1Out)
  STACK_ATTACH(float *, pfRes)
  STACK_ATTACH(float *, pfStage2In)
  STACK_ATTACH(float *, pfQuanVec)

STACK_START
  
  assert(iNumCand <= MAX_NUMCAND);
  assert((piIndices != NULL) && (pfData != NULL));
  assert((pfStage1CB != NULL) && (pfRotScaleCB != NULL));
  assert((pfStage2InCellCB != NULL) && (pfStage2OutCellCB != NULL));
  assert(piIndexOfStage2OutCell != NULL);

  /*--------------------------------------------------
    initializethe  minimum distortion.
    --------------------------------------------------*/
  fMinDis = FLT_MAX;

  /*--------------------------------------------------
    Find the M-Best candidates
    --------------------------------------------------*/
  VQ2mbest(STACK_S pfData, pfStage1CB, iStage1Size, piMBestCand, iNumCand);


  /*--------------------------------------------------
    Check each candidates for the next stage 
    --------------------------------------------------*/
   for(i=0; i<iNumCand; i++)
     {
       iBestCand = piMBestCand[i];

       /*--------------------------------------------------
         Find the quantized vector from stage I VQ
         --------------------------------------------------*/
       Dim2VectorLUT(iBestCand, pfStage1CB, iStage1Size, pfStage1Out);

       /*--------------------------------------------------
         Find the residual and do rotation-scaling to 
         normalize the residual vector.
         --------------------------------------------------*/
       pfRes[0] = pfData[0] - pfStage1Out[0];
       pfRes[1] = pfData[1] - pfStage1Out[1];

       RotScaleVQ2(pfRes, pfRotScaleCB, iBestCand, pfStage2In);

       /*--------------------------------------------------
        Initialize the codebook for stage II -- from
        inner cell or outer cell
        --------------------------------------------------*/
       if (SelectOutCellCB(piIndexOfStage2OutCell, iNumOfOutCell, 
                           iBestCand) == 1)
          pfStage2CB = pfStage2OutCellCB;
       else
          pfStage2CB = pfStage2InCellCB;


       /*--------------------------------------------------
        Quantize the residual using stage II codebook.
        --------------------------------------------------*/
       iIndex = Dim2VectorQuantize(pfStage2In, pfStage2CB, iStage2Size);

       /*--------------------------------------------------
        Now, get the quantized stage II vector.
        --------------------------------------------------*/
       Dim2VectorLUT(iIndex, pfStage2CB, iStage2Size, pfStage2In);

       /*--------------------------------------------------
         undo rotation and scaling 
        --------------------------------------------------*/
       DeRotScaleVQ2(pfStage2In, pfRotScaleCB, iBestCand, pfRes);

       /*--------------------------------------------------
        Get the fully quantized vector by adding the
        residual vector.
        --------------------------------------------------*/
       pfStage1Out[0] += pfRes[0];
       pfStage1Out[1] += pfRes[1];

       /*--------------------------------------------------
        Compute the distortion
        --------------------------------------------------*/
       fDis = CompDistVQ2(pfData, pfStage1Out);

       /*--------------------------------------------------
         Find the minimum distation
        --------------------------------------------------*/
       if (fDis < fMinDis) 
         {
           fMinDis = fDis;
           piIndices[0] = iBestCand;
           piIndices[1] = iIndex;
           pfQuanVec[0] = pfStage1Out[0];
           pfQuanVec[1] = pfStage1Out[1];
         }
     } 

   /*--------------------------------------------------
     Replace pfData with pfQuanVec
     --------------------------------------------------*/
   pfData[0] = pfQuanVec[0];
   pfData[1] = pfQuanVec[1];
   
STACK_END
}
#endif

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
*                                     I codebook
*
* Output:   pfData -- quantized coefficients 
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
* Concerns/TBD: only for 2-dimensional VQ
******************************************************************************/

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
                   )
{
  const float *pfStage2CB;
STACK_INIT
  float pfStage2In[VQ2_DIMENSION];
  float pfRes[VQ2_DIMENSION];
STACK_INIT_END

  STACK_ATTACH(float *, pfStage2In)  
  STACK_ATTACH(float *, pfRes)

STACK_START

  assert((piIndices != NULL) && (pfData != NULL));
  assert((pfStage1CB != NULL) && (pfRotScaleCB != NULL));
  assert((pfStage2InCellCB != NULL) && (pfStage2OutCellCB != NULL));
  assert(piIndexOfStage2OutCell != NULL);
 
  /*--------------------------------------------------
    Decode the Stage I vector
    --------------------------------------------------*/
  Dim2VectorLUT(piIndices[0], pfStage1CB, iStage1Size, pfData);

  /*--------------------------------------------------
    Find the stage II codebook
    --------------------------------------------------*/
  if (SelectOutCellCB(piIndexOfStage2OutCell, iNumOfOutCell,
                      piIndices[0]) == 1)
    pfStage2CB = pfStage2OutCellCB;
  else
    pfStage2CB = pfStage2InCellCB;

  /*--------------------------------------------------
    Decode the stage II vector
    --------------------------------------------------*/
  Dim2VectorLUT(piIndices[1], pfStage2CB, iStage2Size, pfStage2In);

  /*--------------------------------------------------
    undo rotation and scaling 
    --------------------------------------------------*/
  DeRotScaleVQ2(pfStage2In, pfRotScaleCB, piIndices[0], pfRes);

  /*--------------------------------------------------
    Add two stage vectors together
    --------------------------------------------------*/
  pfData[0] += pfRes[0];
  pfData[1] += pfRes[1];

STACK_END
}

#ifndef DECODE_ONLY
/******************************************************************************
*
* Function: VQ2mbest
*
* Action: Find the M-Best indices 
*
* Input:  pfData -- input vector
*         pfCB -- codebook
*         iCBSize -- codebook size
*         piMBestCand -- pointer to the indices for the M-Best candidates
*         iMBest -- number of best candidates
*
* Output:   piMBestCand -- indices for the M-Best candidates
*
* Globals:   none
*
* Return:    none
*******************************************************************************/

static void VQ2mbest( STACK_R 
                      const float *pfData, 
                      const float *pfCB, 
                      int    iCBSize, 
                      int    *piMBestCand, 
                      int    iMBest
                    )
{
  int i, j;
  float fDis;

STACK_INIT
  float pfMinDis[MAX_NUMCAND];
STACK_INIT_END

  STACK_ATTACH (float *, pfMinDis)

STACK_START

  assert(iMBest <= MAX_NUMCAND);
  assert((pfData != NULL) && (pfCB != NULL));
  assert(piMBestCand != NULL);

  /*--------------------------------------------------
    Initialize piMBestCand and pfMinDis 
    --------------------------------------------------*/
  for(i = 0; i < iMBest; i++) 
    {
      piMBestCand[i] = 0;
      pfMinDis[i] = FLT_MAX;
    }

  /*--------------------------------------------------
    Search each entry of the codebook, and find the
    M-Best candidates 
    --------------------------------------------------*/
  for (i = 0; i < iCBSize; i++)
    {
      /*--------------------------------------------------
        Compute the distance between the input vector and
        each code-vector.
        --------------------------------------------------*/
      fDis = CompDistVQ2(pfData, pfCB);
      pfCB += 2;
      
      /*--------------------------------------------------
        If the error for this entry is less than the worst
        retained candidate so far, keep it. 
        Note that the error list is maintained in order 
        of best (min error) to worst.
        --------------------------------------------------*/
      if (fDis < pfMinDis[iMBest-1])
        { 
          for(j=iMBest-1; ((j>0)&&(fDis<=pfMinDis[j-1])); j--)
            {
              pfMinDis[j] = pfMinDis[j-1];
              piMBestCand[j] =piMBestCand[j-1];
            }
             
          pfMinDis[j] = fDis;
          piMBestCand[j] = i;
        }
    } 

STACK_END
}


/******************************************************************************
*
* Function: RotScaleVQ2
*
* Action: Rotate and scale an input vector
*
* Input:  pfIn -- input vector
*         pfRSMatrix -- rotation and scaling matrix
*         iIndex -- index for the rotation-scaling vector entry
*         pfOut -- the pointer to the output vector
*
* Output: pfOut -- rotated and scaled vector
*
* Globals:   none
*
* Return:    none
*******************************************************************************/

static void RotScaleVQ2( const float *pfIn, 
                         const float *pfRSMatrix,
                         int    iIndex, 
                         float  *pfOut
                       )
{
  const float *pfRSVec = &(pfRSMatrix[iIndex<<1]);

  assert((pfIn != NULL) && (pfRSMatrix != NULL) && (pfOut != NULL));

  pfOut[0]=  pfRSVec[0] * pfIn[0] + pfRSVec[1] * pfIn[1];
  pfOut[1]= -pfRSVec[1] * pfIn[0] + pfRSVec[0] * pfIn[1];
}    
#endif

/******************************************************************************
*
* Function: DeRotScaleVQ2()
*
* Action: undo rotation and scaling
*
* Input:  pfIn -- input vector
*         pfRSMatrix -- rotation and scaling matrix
*         iIndex -- index for the rotation-scaling vector entry
*         pfOut -- the pointer to the output vector
*
* Output: pfOut -- output vector
*
* Globals:   none
*
* Return:    none
*******************************************************************************/

static void DeRotScaleVQ2( const float *pfIn, 
                           const float *pfRSMatrix,
                           int    iIndex, 
                           float  *pfOut
                         )
{
  float fScale;
  const float *pfRSVec = &(pfRSMatrix[iIndex<<1]);

  assert((pfIn != NULL) && (pfRSMatrix != NULL) && (pfOut != NULL));

  fScale = pfRSVec[0]*pfRSVec[0] + pfRSVec[1]*pfRSVec[1];
  fScale = 1.0F / fScale;
    
  pfOut[0] = fScale * (pfIn[0]*pfRSVec[0] - pfIn[1]*pfRSVec[1]);
  pfOut[1] = fScale * (pfIn[1]*pfRSVec[0] + pfIn[0]*pfRSVec[1]);
}


/******************************************************************************
*
* Function: SelectOutCellCB
*
* Action: find out whether the code-vector is an outer cell
*
* Input:  piIndexOfOutCell -- indices for the outer cell
*         iNumOfOutCell -- number of outer cells
*         iIndex -- the input index for an unknown cell (or code-vector)
*
* Output: none
*
* Globals:   none
*
* Return:    1: outer cell
*            0: inner cell
*       
*******************************************************************************/

static int SelectOutCellCB( const int *piIndexOfOutCell,
                            int  iNumOfOutCell, 
                            int  iIndex
                          )
{
  int flag = 0;
  int i;

  assert(piIndexOfOutCell != NULL);

  for(i=0; i<iNumOfOutCell; i++)
    {
      if (piIndexOfOutCell[i] == iIndex)
        {
          flag = 1;
          break;
        }
    }

  return flag;
}

#ifndef DECODE_ONLY
/******************************************************************************
*
* Function: CompDistVQ2
*
* Action: compute the distortion between 2-dimensional vectors
*
* Input:  pfIn -- input vector
*         pfRef -- reference vector
*
* Output: none
*
* Globals:   none
*
* Return:    distortion between pfIn and pfRef
*       
*******************************************************************************/
     
static float CompDistVQ2(const float *pfIn, const float *pfRef)
{
  float fErr;
  float fDis;

  assert((pfIn != NULL) && (pfRef != NULL));

  fErr = pfIn[0] - pfRef[0];
  fDis = fErr*fErr;

  fErr = pfIn[1] - pfRef[1];
  fDis += fErr*fErr;

  return fDis;
}


#endif
