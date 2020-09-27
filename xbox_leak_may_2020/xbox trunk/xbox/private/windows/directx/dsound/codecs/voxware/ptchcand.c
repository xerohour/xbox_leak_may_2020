/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       PtchCand.c
*                                                                              
* Purpose:        Select pitch candidates for further analysis.
*                                                                              
* Functions:      SelectPitchCandidates()
*
* Author/Date:    Original developed by Gerard Aguilar, 97.
*                 Enhanced and R&D by Wei Wang, 4/97.
*
********************************************************************************
* Modifications:  12/97, splitted to small functions by WW.
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/PtchCand.c_v   1.2   02 Apr 1998 14:46:58   weiwang  $
******************************************************************************/

/*** system include files ***/
#include <stdlib.h>
#include <assert.h>

/*** Research lib include files ***/
#include "vLibVec.h"

/*** codec related include files ***/
#include "VoxMem.h"
#include "LDPDAdef.h"
#include "PtchCand.h"
#include "CalcCost.h"

#include "xvocver.h"

/*--------------------------------------------------
   Local table -- pitch candidates
  --------------------------------------------------*/
extern const float CpfPitchTable[PITCH_TABLE_SIZE];

/*--------------------------------------------------
   local definitions
  --------------------------------------------------*/

#define SUB_MAX        5 
#define MINCON         1.0e-6F    
#define RIGHT_P0_SCALE   1.333F
#define LEFT_P0_SCALE    0.75F
#define COST_THRESHOLD    0.78F
#define HP_COST_THRESHOLD  0.84F
#define SEARCH_BOUND  (1.0F/1.2F)
#define LOW_THRESHOLD  0.2F
#define CONF_PITCH  30.0F
#define COST_THRESHOLD_BACK  0.72F

#define LD_TRUE  1
#define LD_FALSE 0

/* Macro for exchanging floating value */
#define ExchangeFltValue(x, y) {float a; a=x; x=y; y=a;}

/* Macro for computing the total cost of current frame
   and backward tracking frame */
#define CompBackCost(x_1, x) ((x_1 > x) ? (0.5F*(x_1+x)) : x)


/*--------------------------------------------------
  Private functions
  --------------------------------------------------*/
static 
int BackWard_Check(const float *pfCostFunc, float fLastP0, float fLastCost,
                   float *pfNewBackCost);

static 
int HarmThreshold(float *CostFunc, int *Ind_Cand, int *piHP_Index);

static 
void EliminateSmallP0(float *CostFunc, int *Ind_Cand, int *piNumCand, 
                      int *piHP_Index);

static 
void AddBackTrackCand(float *CostFunc, int iBackIndex, int *Ind_Cand,
                      int *piNumCand, int *piHP_Index);

static 
float RefineLDPitch(int iIndex, const int *piPeakFreq, const float *pfMag, 
                    int iBaseBand, float fNormEng, float *pfMaxCost);


/******************************************************************************
* Function:  SelectPitchCandidates()
*
* Action:    Select most possible candidates.
*
* Input:     pfMag       -- compressed magnitude spectrum
*            iBaseBand   -- length of base bandwidth
*            piPeakFreq  -- peak indices
*            pfPitchCand -- address to the output pitch candidates
*            pfCandCost  -- address to the cost values of selected candidates
*            fLastP0     -- previous pitch for backward tracking
*            flastCost   -- the cost value of previous pitch period
*            piHP_Index  -- address to the index of pfPitchCand to get the high-
*                           possibility pitch candidate
*
* Output:    pfPitchCand -- pitch periods of selected candidates
*            piHP_Index  -- index of pfPitchCand to get the high-
*                           possibility pitch candidate
*
* Globals:   none
*
* Return:    number of candidates
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/
int SelectPitchCandidates( STACK_R 
                           const float *pfMag, 
                           int         iBaseBand, 
                           const int   *piPeakFreq, 
                           float       *pfPitchCand, 
                           float       *pfCandCost, 
                           float       fLastP0,
                           float       fLastCost,
                           int         *piHP_Index
                         )
{
  int iBackIndex;
  float fNewBackCost;
  int iNumCand, iNumCand_1;
  int i, k;
  int iHP_Index;
  float fCost;
  int iIndex;
  float fNormEnergy;

STACK_INIT
  int piCandInd_0[SUB_MAX+1];
  float pfCostFunc[PITCH_TABLE_SIZE];
STACK_INIT_END

STACK_ATTACH(int *, piCandInd_0)
STACK_ATTACH(float *, pfCostFunc)

STACK_START

  assert(pfMag != NULL);
  assert(piPeakFreq != NULL);
  assert(pfPitchCand != NULL);
  assert(piHP_Index != NULL);

  /*----------------------------------------------------------------------
    (0) Compute normalization factor .
    ----------------------------------------------------------------------*/
  fNormEnergy = MINCON + DotProd(pfMag, pfMag, iBaseBand);
  
  fNormEnergy = 10.0F/fNormEnergy;

  /*----------------------------------------------------------------------
    (1) Compute the cost function (or likelyhood function) of all pitch
        candidates.
    ----------------------------------------------------------------------*/
  for (i = 0; i < PITCH_TABLE_SIZE; i++)
    pfCostFunc[i] = CalcCostFunc(CpfPitchTable[i], piPeakFreq, pfMag,
                                 iBaseBand, fNormEnergy);


  /*----------------------------------------------------------------------
    (2) Backward tracking and modify the cost function for pre-selection.
    ----------------------------------------------------------------------*/
  iBackIndex = BackWard_Check(pfCostFunc, fLastP0, fLastCost,
                              &fNewBackCost);


  /*----------------------------------------------------------------------
    (3) Replace the cost value of iBackIndex using fNewBackCost.
    Then extract good pitch candidates using progressive harmonic thresholding.
    Then change back to the original value.
    ----------------------------------------------------------------------*/

  /*----------------------------------------------------------------------
    Get the regular candidates using Harmonic Progressive thresholding.
    ----------------------------------------------------------------------*/
  ExchangeFltValue(fNewBackCost, pfCostFunc[iBackIndex]);

  iNumCand = HarmThreshold(pfCostFunc, piCandInd_0, &iHP_Index);
  
  /*----------------------------------------------------------------------
    Prevent pick up too small pitch candidate, -- important for the plusive
    speech.
    ----------------------------------------------------------------------*/
  EliminateSmallP0(pfCostFunc, piCandInd_0, &iNumCand, &iHP_Index);


  /*----------------------------------------------------------------------
    Add the backward-tracked candidate.
    ----------------------------------------------------------------------*/
  AddBackTrackCand(pfCostFunc, iBackIndex, piCandInd_0, &iNumCand, &iHP_Index);

  ExchangeFltValue(fNewBackCost, pfCostFunc[iBackIndex]);

  
  /*----------------------------------------------------------------------
    (4) Refine the pitch and only choose at most 3 candidates for later
    calculateion.
    ----------------------------------------------------------------------*/
  k = 0;  
  iNumCand_1 = iNumCand-1;
  for (i = 0; i < iNumCand; i++)
    {
      if ((i != 0) && (i != (iNumCand_1)) && (i != iHP_Index))
        continue;

      /*** find the new high possibility index ***/
      if (i == iHP_Index)
        *piHP_Index = k;

      iIndex = piCandInd_0[i];

      /*** find the cost value ***/
      fCost = pfCostFunc[iIndex];

      /*** refine pitch value ***/
      pfPitchCand[k] = RefineLDPitch(iIndex, piPeakFreq, pfMag,
                                     iBaseBand, fNormEnergy, &fCost);

      /*** put back the backward tracking information ***/
      if (iIndex == iBackIndex)
        fCost = CompBackCost(fLastCost, fCost);

      /*** Save to New arrays ***/
      pfCandCost[k] = fCost;

      k++;
    } 

  iNumCand = k;

STACK_END

  return iNumCand;
}


/******************************************************************************
* Function:  BackWard_Check()
*
* Action:    find potential pitch candidate by backward tracking
*
* Input:     pfCostFunc    -- cost values of all pitch candidates (from table)
*            fLastP0       -- pitch period of last frame
*            fLastCost     -- cost value of last frame pitch period
*            pfNewBackCost -- array to cost value from backward tracking
*
* Output:    pfNewBackCost -- modified cost value for the pitch from backward
*                             tracking
*
* Globals:   none
*
* Return:    pitch index from backward tracking
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

static 
int BackWard_Check( const float *pfCostFunc, 
                    float        fLastP0, 
                    float        fLastCost,
                    float       *pfNewBackCost
                  )
{
  float fRightP0, fLeftP0;
  int i;
  float fMaxCost;
  int iBackIndex;

  assert(pfCostFunc != NULL);
  assert(pfNewBackCost != NULL);

  /* Find the boundaries */
  fRightP0 = fLastP0*RIGHT_P0_SCALE;
  fLeftP0 = fLastP0*LEFT_P0_SCALE;

  /* find the left boundary */
  i = 0;
  while (CpfPitchTable[i] < fLeftP0) 
     i++;

  /* Find the candidate with the maximum cost value from the 
     backward tracking */
  fMaxCost = pfCostFunc[i];
  iBackIndex = i;

  do {
    if (fMaxCost < pfCostFunc[i])
      {
        fMaxCost = pfCostFunc[i];
        iBackIndex = i;
      }
    i++;
  } while((i < PITCH_TABLE_SIZE) && (CpfPitchTable[i] <= fRightP0));
  
  /* Now compute the total cost value (combine the cost from the last 
     frame and the current frame) */
  *pfNewBackCost = CompBackCost(fLastCost, fMaxCost);

  return iBackIndex;
}


/******************************************************************************
* Function:  HarmThreshold()
*
* Action:    Get selected candidates using Harmonic Progressive Thresholding
*            algorithm
*
* Input:     pfCostFunc  -- cost values of all pitch candidates (from table)
*            piIndCand   -- address to the indices of selected candidates
*            piHPIndex   -- address to the high-possibility candidate 
*
* Output:    piIndCand -- the indices of selected candidates
*            piHPIndex -- high-possibility candidate
*
* Globals:   none
*
* Return:    number of selected candidates
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/
static 
int HarmThreshold( float   *pfCostFunc, 
                   int     *piIndCand, 
                   int     *piHPIndex
                 )
{
  float fMaxCost;
  int iPitchIndex;
  float fP0;
  int iHPIndex;
  float fLocalMaxCost;
  int iLocalMaxInd;
  int iLocalLength;
  int iNumCand;


  assert(pfCostFunc != NULL);
  assert(piIndCand != NULL);
  assert(piHPIndex != NULL);

  /*--------------------------------------------------
    Initializing ...
    --------------------------------------------------*/
  iHPIndex = 0;
  iNumCand = 0;
  iPitchIndex = PITCH_TABLE_SIZE-1;   /* the end point index */
  fMaxCost = -1.0F;

  while (iNumCand < SUB_MAX)
    { 
      /*--------------------------------------------------
        Find the local maxima.
        --------------------------------------------------*/
      iLocalLength = iPitchIndex+1;
      FindMaxValueFlt(pfCostFunc, iLocalLength, &iLocalMaxInd,
                      &fLocalMaxCost);

      /*--------------------------------------------------
        Only choose the candidate > threshold.
        --------------------------------------------------*/
      if (fLocalMaxCost < (COST_THRESHOLD*fMaxCost)) 
         break;

      /*--------------------------------------------------
        Fill up the local candidate.
        --------------------------------------------------*/
      piIndCand[iNumCand] = iLocalMaxInd;

      /*--------------------------------------------------
        Find the high probability candidate.
        --------------------------------------------------*/
      if (fLocalMaxCost >= (HP_COST_THRESHOLD*fMaxCost))
        iHPIndex = iNumCand;

      /*--------------------------------------------------
        Replace global maxima with the local one.
        --------------------------------------------------*/
      fMaxCost = fLocalMaxCost;

      /*--------------------------------------------------
        Update ... 
        --------------------------------------------------*/
      iNumCand++;

      /*--------------------------------------------------
        Find the next search range 
        --------------------------------------------------*/
      iPitchIndex = iLocalMaxInd;
      fP0 = CpfPitchTable[iPitchIndex]*SEARCH_BOUND;  
      if (fP0 < CpfPitchTable[0])   /* don't go over table length */
         break;

      while (CpfPitchTable[iPitchIndex] > fP0)  
         iPitchIndex--;

      /*--------------------------------------------------
        Pass a valley to make sure the new candidate is
        a peak.
        --------------------------------------------------*/
      while ((iPitchIndex >= 0)&&
             (pfCostFunc[iPitchIndex] < pfCostFunc[iPitchIndex+1]))
          iPitchIndex --;         

      if (iPitchIndex<0) break;
    }

  *piHPIndex = iHPIndex;

  return iNumCand;
}



/******************************************************************************
* Function:  EliminateSmallP0()
*
* Action:    To prevent pick up too small pitch period as candidates (high 
*            frequency pitch). This is important to get better quality for
*            plusive speech.
*
* Input:     pfCostFunc  -- cost function for all candidates in pitch table
*            piIndCand   -- indices for seleted pitch candidates
*            piNumCand   -- the number of seleted pitch candidates
*            piHPIndex   -- the index of high-possibility pitch candidate
*
* Output:    piIndCand   -- updated indices for seleted pitch candidates
*            piNumCand   -- updated number of seleted pitch candidates
*            piHPIndex   -- updated index for high-possiblity pitch candidate
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/
static 
void EliminateSmallP0( float  *pfCostFunc, 
                       int    *piIndCand, 
                       int    *piNumCand, 
                       int    *piHPIndex
                     )
{
  float fMaxCost;
  int iPitchIndex;
  int iLocalMaxInd;
  float fOrgP0, fTryP0;
  int iHPIndex;
  int iHitFlag;
  float fHitThreshold;
  int iNumP0Index;
  int iNumCand;

  assert(pfCostFunc != NULL);
  assert(piIndCand != NULL);
  assert(piNumCand != NULL);
  assert(piHPIndex != NULL);

  iHPIndex = *piHPIndex;
  iNumCand = *piNumCand;

  /*--------------------------------------------------
    Get the pitch value of the smallest candidate.
    --------------------------------------------------*/
  iPitchIndex = piIndCand[iNumCand-1];
  fOrgP0 = CpfPitchTable[iPitchIndex];

  /*--------------------------------------------------
    Only search if Pitch < 30.0F
    --------------------------------------------------*/
  if (fOrgP0 <= CONF_PITCH)
    {
      /*--------------------------------------------------
        Set the cost threshold.
        --------------------------------------------------*/
      iHitFlag = LD_FALSE;
      fHitThreshold = pfCostFunc[iPitchIndex] * LOW_THRESHOLD;

      /*--------------------------------------------------
        check the cost values around 2*P0.
        --------------------------------------------------*/
      fTryP0 = fOrgP0 * 2.0F;

      while(CpfPitchTable[++iPitchIndex] < fTryP0);

      if ((pfCostFunc[iPitchIndex-1]<fHitThreshold)
          && (pfCostFunc[iPitchIndex]<fHitThreshold))
        iHitFlag = LD_TRUE;


      /*--------------------------------------------------
        check the cost values around 3*P0.
        -------------------------------------------------*/
      fTryP0 = fOrgP0 * 3.0F;

      while(CpfPitchTable[++iPitchIndex] < fTryP0);

      if ((pfCostFunc[iPitchIndex-1]<fHitThreshold)
          && (pfCostFunc[iPitchIndex]<fHitThreshold))
        iHitFlag = LD_TRUE;

      
      /*--------------------------------------------------
        Re-select the pitch candidate if the conditions
        are failed.
        --------------------------------------------------*/
      if (iHitFlag == LD_TRUE) {
        if (iNumCand == LD_TRUE) 
          {
            /*--------------------------------------------------
              if there is only one candidate, replace it using 
              the largest cost from [3*P0 MAX_PITCH].
              --------------------------------------------------*/
            iNumP0Index = PITCH_TABLE_SIZE-iPitchIndex;
            FindMaxValueFlt(&(pfCostFunc[iPitchIndex]), iNumP0Index,
                            &iLocalMaxInd, &fMaxCost);
            piIndCand[0] = iLocalMaxInd + iPitchIndex;
          }
        else 
          {
            /*--------------------------------------------------
              if there are more than one candidate, remove the
              smallest pitch candidate.
              --------------------------------------------------*/
            iNumCand--;
            
            /*--------------------------------------------------
              if the removed one is with high-possibility,
              reset the HP candidate with the largest cost value.
              --------------------------------------------------*/
            if (iHPIndex == iNumCand)
              iHPIndex = 0;
          }
      }
    }

  *piHPIndex = iHPIndex;
  *piNumCand = iNumCand;
}



/******************************************************************************
* Function:  AddBackTrackCand()
*
* Action:    Add backward tracked pitch to the seleted pitch candidates list
*
* Input:     pfCostFunc  -- cost function for all candidates in pitch table
*            iBackIndex  -- backward tracked pitch index
*            piIndCand   -- indices for seleted pitch candidates
*            piNumCand   -- the number of seleted pitch candidates
*            piHPIndex   -- the index of high-possibility pitch candidate
*
* Output:    piIndCand   -- updated indices for seleted pitch candidates
*            piNumCand   -- updated number of seleted pitch candidates
*            piHPIndex   -- updated index for high-possiblity pitch candidate
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

static 
void AddBackTrackCand( float  *pfCostFunc, 
                       int    iBackIndex, 
                       int    *piIndCand,
                       int    *piNumCand, 
                       int    *piHPIndex
                      )
{
  float fMaxCost;
  int iHPIndex;
  int iHitFlag;
  float fHitThreshold;
  int iNumCand;
  int iInsertPoint;
  float fBackCost;
  int i;

  assert(pfCostFunc != NULL);
  assert(piIndCand != NULL);
  assert(piNumCand != NULL);
  assert(piHPIndex != NULL);
  
  iHPIndex = *piHPIndex;
  iNumCand = *piNumCand;

  /*--------------------------------------------------
    Search whether the backward tracked candidate
    is in the list. And find the location 
    where BackIndex>Ind_Cand[i].
    --------------------------------------------------*/
  iHitFlag = LD_FALSE;
  for (i = 0; i < iNumCand; i++) {
    if (iBackIndex == piIndCand[i])
       iHitFlag = LD_TRUE;
    if (iBackIndex > piIndCand[i]) 
       break;
  }
  iInsertPoint = i;

  /*-----------------------------------------------------
    If the backward-tracked candidate is an extra one ...
    -----------------------------------------------------*/
  if (iHitFlag == LD_FALSE) 
    {
      /*--------------------------------------------------
        Find the thresholds
        --------------------------------------------------*/
      fBackCost = pfCostFunc[iBackIndex];
      fHitThreshold = COST_THRESHOLD_BACK*pfCostFunc[piIndCand[0]];
      fMaxCost = HP_COST_THRESHOLD*pfCostFunc[piIndCand[iHPIndex]];

      /*--------------------------------------------------
        If the new candidate is smaller than whole selected
        candidatesthe, or if it's small than the 
        high-possibility one, add it to the list.
        --------------------------------------------------*/
      if ((iInsertPoint==iNumCand && fBackCost>=fHitThreshold)
          || (iInsertPoint>iHPIndex && fBackCost>=fMaxCost))
        {
          /*----------------------------------------
            Shift the indeces.
            ----------------------------------------*/
          for (i = iNumCand; i > iInsertPoint; i--) 
            piIndCand[i] = piIndCand[i-1];
        
          /*----------------------------------------
            Insert the new candidate.
            ----------------------------------------*/
          piIndCand[iInsertPoint] = iBackIndex;

          /*----------------------------------------
            Test and reset the high-possibility 
            index.
            ----------------------------------------*/
          if (fBackCost>=fMaxCost)
            iHPIndex = iInsertPoint;

          /*----------------------------------------
            Increase the number of candidates.
            ----------------------------------------*/
          iNumCand++;
        }
    }

  /*--------------------------------------------------
    Return the HP_Index and number of candidates.
    --------------------------------------------------*/
  *piHPIndex = iHPIndex;
  *piNumCand = iNumCand;

}

/******************************************************************************
* Function:  RefineLDPitch()
*
* Action:    Get the refined pitch by choosing the maximum likely-hood functions
*            from two neighbor pitches.
*
* Input:     iIndex      -- index of the pitch who needs to be refined
*            piPeakFreq  -- peaks' indices 
*            pfMag       -- compressed magnitude spectrum
*            iBaseBand   -- length of base-band
*            fNormEng    -- energy normalization factor
*            pfMaxCost   -- address to the output maximum cost value
*
* Output:    pfMaxCost   -- cost value of the refined pitch
*
* Globals:   none
*
* Return:    refined pitch value
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

static
float RefineLDPitch( int         iIndex,
                     const int   *piPeakFreq, 
                     const float *pfMag, 
                     int         iBaseBand,
                     float       fNormEng, 
                     float       *pfMaxCost
                   )
{
  float fBestPitch;
  float fPitch;
  float fCost;
  float pfPitchCand[2];
  int k, i;

  assert(piPeakFreq != NULL);
  assert(pfMag != NULL);
  assert(pfMaxCost != NULL);

  /* get the pitch candidates */
  fBestPitch = CpfPitchTable[iIndex];

  k = 0;
  /* find pitch period on the left side of input candidate */
  if (iIndex > 0)
    pfPitchCand[k++] = 0.5F*(fBestPitch + CpfPitchTable[iIndex-1]);
  iIndex++;
  /* find pitch period on the right side of input candidate */
  if (iIndex < PITCH_TABLE_SIZE)
    pfPitchCand[k++] = 0.5F*(fBestPitch + CpfPitchTable[iIndex]);


  /* compute the cost values of two refined pitch period. 
     And find the maximum among the original and the two refined ones. */
  for (i = 0; i < k; i++)
    {
      fPitch = pfPitchCand[i];
      fCost = CalcCostFunc(fPitch, piPeakFreq, pfMag, 
                           iBaseBand, fNormEng);
      if (fCost > *pfMaxCost) {
        fBestPitch = fPitch;
        *pfMaxCost = fCost;
      }
    }

  return fBestPitch;
}


