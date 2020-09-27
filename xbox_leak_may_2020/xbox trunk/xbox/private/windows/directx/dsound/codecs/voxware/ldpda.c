/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       LDPDA.c
*                                                                              
* Purpose:        Estimate pitch using frequency domain cost function and back-
*                 ward tracking.
*                                                                              
* Functions:      VoxLDPDA(), GetPitchTable(),
*                 VoxInitLDPDA(), VoxFreeLDPDA()
*
* Author/Date:    Original developed by Gerard Aguilar   01/97
*                 Enhanced R&D by Wei Wang   04/97
*
********************************************************************************
* Modifications:
*                3/30/98, W.W. Rewrite whole file and use inter-leave format
*                spectrum. 
*  
*                05/97, W.W.
*                (1) Use longer table and change the boundary calculation. 
*                      (LDPDA.1)
*                (2) Change the boundary calculation using masker value and 
*                      use cosine table. (LDPDA.2).
*                (3) Use pitch table instead of fundamental frequency table.
*                (4) reduce table to 86 entries and expand the candidate from 
*                      8 to 160.
*                (5) add refinement for best candidates.
*                (6) when pitch is noise-like or (plusive), put pitch value 
*                      higher.
*                (7) put fix-point indeces for Calc_Cost_Func and VoicingLD.
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/LDPDA.c_v   1.3   08 Apr 1998 11:01:38   weiwang  $
******************************************************************************/
/*** system include files ***/
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/*** Research lib include files ***/
#include "vLibVec.h"
#include "vLibMath.h"

/*** codec related include files ***/
#include "VoxMem.h"
#include "codec.h"
#include "LDPDAdef.h"
#include "SelePeak.h"
#include "CompPow.h"
#include "PtchCand.h"
#include "VoicLD.h"
#include "LDPDA.h"

#include "xvocver.h"

/*--------------------------------------------------
   Local table -- pitch candidates
  --------------------------------------------------*/
const float CpfPitchTable[PITCH_TABLE_SIZE] =
{
#include "PitchTBL.h"
};

/*---------------------------------------------------
   Private functions
  ---------------------------------------------------*/
static 
int FindBaseBand(const float *pfPower, int iLength);

static
int FindBestCandidates(const float *pfPv, int iNumCand, int iHpIndex);

static
void UpdateLDPDA_MBlk(void *hLD_PDA_mblk, float fPitch, float fCost);


/******************************************************************************
* Function:  VoxLDPDA
*
* Action:    Estimate pitch value from frequency domain spectrum. This is 
*            the main function for coarse pitch calculation.
*
* Input:     pfSW -- input spectrum
*            hLD_PDA_mblk -- PDA structure
*
* Output:    none
*
* Globals:   none
*
* Return:    pitch value
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

float VoxLDPDA( STACK_R 
                const float *pfSW, 
                void        *hLD_PDA_mblk
              )

{
  /***** STACK memory ******/
STACK_INIT
  float pfPower[FAR_EXTEND_CUT_LC];
  float pfCandCost[NCAND];
  float pfPitchCand[NCAND];
  float pfPv[NCAND];
  int   piPeakFreq[MAX_NPEAKS];
STACK_INIT_END

  STACK_ATTACH(float *, pfPower)
  STACK_ATTACH(float *, pfCandCost)
  STACK_ATTACH(float *, pfPitchCand)
  STACK_ATTACH(float *, pfPv)
  STACK_ATTACH(int *, piPeakFreq)

  /***** regular varibles *****/
  int iBaseBand;    
  float fMaxPower;  
  int i;
  int iNumCand;
  int iHP_Index;
  float fPitch;

STACK_START
  /*-------------------------------------------
    (0) Check input arrays.
    -------------------------------------------*/
  assert(pfSW != NULL);
  assert(hLD_PDA_mblk != NULL);

  /*----------------------------------------------------------------------
    (1) compute the power spectrum of the STFT of the signal.
    Then find the maximum value.
    ----------------------------------------------------------------------*/
  MagSqIL(pfSW, FAR_EXTEND_CUT_LC, pfPower);

  FindMaxValueFlt(pfPower, FAR_EXTEND_CUT_LC, &i, &fMaxPower);
  assert(fMaxPower >= 0.0F);


  /*----------------------------------------------------------------------
    (2) Choose the baseband. Then select the peaks for pitch estimation.
    ----------------------------------------------------------------------*/
  iBaseBand = FindBaseBand(pfPower, EXTEND_CUT_LC);
  
  SelectPeaks(STACK_S pfPower, iBaseBand, piPeakFreq, fMaxPower); 


  /*----------------------------------------------------------------------
    (3) Normalize the power spectrum between 0 and 1. Then compress it to 
    get the magnitude spectrum by x^(1/8).
    ----------------------------------------------------------------------*/
  if (fMaxPower > 0.0F)
    fMaxPower = 1.0F/fMaxPower;

  for (i = iBaseBand-1; i >= 0; i--)
    {
      pfPower[i] *= fMaxPower;
      pfPower[i] = powerpoly_125(pfPower[i]);
   }

  /*----------------------------------------------------------------------
    (4) Select the good candidates and do pitch refinement.
    ----------------------------------------------------------------------*/
  iNumCand = SelectPitchCandidates(STACK_S pfPower, iBaseBand, piPeakFreq, 
                                   pfPitchCand, pfCandCost, 
                                   ((LD_PDA_MBlk *)hLD_PDA_mblk)->fLastP0, 
                                   ((LD_PDA_MBlk *)hLD_PDA_mblk)->fLastCost,
                                   &iHP_Index);

  
  /*----------------------------------------------------------------------
    (5) Find the estimated pitch from at most 3 candidates: 
    pfPitchCand[0], pfPitchCand[iHP_Index], pfPitchCand[iNumCand-1]
    using frequency-domain analysis-by-synthesis.
    ----------------------------------------------------------------------*/
  /* Re-calculate iBaseBand for voicingLD calculation. */
  iBaseBand = FindBand4VoicLD(iBaseBand, pfPitchCand[iNumCand-1]);
    
  /* do freqeuncy-domain analysis-by-synthesis voicing calculation */
  for (i = iNumCand-1; i >= 0; i--) 
    pfPv[i] = VoicingLD(pfPitchCand[i], pfSW, iBaseBand);

  /* do the final decision and pitch is pfPitchCand[iHP_Index] */
  iHP_Index = FindBestCandidates(pfPv, iNumCand, iHP_Index);

  fPitch = pfPitchCand[iHP_Index];
  
  /*----------------------------------------------------------------------
    (6) update the information.
    ----------------------------------------------------------------------*/
  UpdateLDPDA_MBlk(hLD_PDA_mblk, fPitch, pfCandCost[iHP_Index]);

STACK_END

  return fPitch;  
}


/******************************************************************************
* Function:  GetPitchTable()
*
* Action:    Export pitch table and table size.
*
* Input:     pfPitchTable -- pointer for output pitch table
*            piSize       -- pointer for output table size  
*
* Output:    pfPitchTable -- pitch table
*            piSize       -- table size
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

void GetPitchTable( float  **pfPitchTable, 
                    int    *piSize
                  )
{
  assert(pfPitchTable != NULL);
  assert(piSize != NULL);

  *pfPitchTable = (float *)CpfPitchTable;
  *piSize = PITCH_TABLE_SIZE;
}


/******************************************************************************
* Function:  VoxInitLDPDA()
*
* Action:    initialize LDPDA memory block.
*
* Input:     hLD_PDA_mblk -- pointer to LDPDA memory block
*
* Output:    hLD_PDA_mblk -- initialized LDPDA memory block
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

unsigned short VoxInitLDPDA(void **hLD_PDA_mblk)
{
  assert(hLD_PDA_mblk != NULL);

  /* initialize the structure */
  if(VOX_MEM_INIT(*hLD_PDA_mblk, 1, sizeof(LD_PDA_MBlk))) 
    return 1;

  ((LD_PDA_MBlk *)*hLD_PDA_mblk)->fLastP0 = 100.0F;
  ((LD_PDA_MBlk *)*hLD_PDA_mblk)->fLastCost = 0.0F;

  return 0;
}


/******************************************************************************
* Function:  VoxFreeLDPDA()
*
* Action:    free the memory which's allocated for LDPDA memory block.
*
* Input:     hLD_PDA_mblk -- pointer to LDPDA memory block
*
* Output:    hLD_PDA_mblk -- memory-freed LDPDA memory block
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/

unsigned short VoxFreeLDPDA(void **hLD_PDA_mblk)
{
  assert(hLD_PDA_mblk != NULL);

  if (*hLD_PDA_mblk) 
    VOX_MEM_FREE(*hLD_PDA_mblk);

  return 0;
}


/******************************************************************************
* Function:  FindBaseBand()
*
* Action:    Find the baseband length for cost function calculation
*
* Input:     pfPower  -- input power spectrum
*            iLength  -- length of power spectrum for calculation
*
* Output:    none
*
* Globals:   none
*
* Return:    baseband length
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

int FindBaseBand( const float *pfPower, 
                  int         iLength
                )
{
  float fEnergyThreshold;
  float fHighEndEnergy;
  int i;

  assert(pfPower != NULL);

  iLength --;
  /*--------------------------------------------------
    (1) Compute the energy cutoff threshold
    --------------------------------------------------*/
  fEnergyThreshold = 0.0F;
  for (i = iLength; i >= 0; i--)
    fEnergyThreshold += pfPower[i];
  fEnergyThreshold *= ENERGY_CUT_OFF;

  /*--------------------------------------------------
    (2) Find the cutoff BaseBand.
    --------------------------------------------------*/
  fHighEndEnergy = 0.0F;
  for (i = iLength; i>=0; i--)
    {
      fHighEndEnergy += pfPower[i];
      if (fHighEndEnergy >= fEnergyThreshold)
        break;
    }
  
  i += 2;
  if (i < FREQ_CUT_LC)
    i = FREQ_CUT_LC;

  return i;
}


/******************************************************************************
* Function:  FindBestCandidates()
*
* Action:    Decide the final pitch from Pv and other information.
*
* Input:     pfPv      -- input estimated voicing array
*            iNumCand  -- number of final candidates
*            iHpIndex  -- index for high possibility candidate
*
* Output:    none
*
* Globals:   none
*
* Return:    index for final pitch value
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
int FindBestCandidates( const float *pfPv, 
                        int          iNumCand, 
                        int          iHpIndex
                      )
{
  int iLastIndex = iNumCand - 1;
  int iBestIndex;
  const static float PV_TH[3] = {0.75F, 1.1F, 0.88F};

  assert(pfPv != NULL);

  /*------------------------------------------------------------
    Resolve most likely pitch candidate using the following 
    conditions:
    ------------------------------------------------------------*/
  iBestIndex = 0; 

  if (iHpIndex<(iLastIndex))    
    {      
      if ((iHpIndex == 0) || (pfPv[iHpIndex] >= (PV_TH[0]*pfPv[0])))
        {
          if (pfPv[iLastIndex] >= (PV_TH[1]*pfPv[iHpIndex]))
            iBestIndex = iLastIndex;
          else
            iBestIndex = iHpIndex;
        } 
    }
  else 
    {
      if (pfPv[iLastIndex] >= (PV_TH[2]*pfPv[0]))
        iBestIndex  = iLastIndex;
    }

  return iBestIndex;
}

/******************************************************************************
* Function:  UpdateLDPDA_MBlk()
*
* Action:    update the content of LDPDA memory block.
*
* Input:     hLD_PDA_mblk -- pointer to LDPDA memory block
*            fPitch        -- final pitch
*            fCost         -- cost value for the final pitch
*
* Output:    hLD_PDA_mblk -- updated LDPDA memory block
*
* Globals:   none
*
* Return:    none
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*
******************************************************************************/
static void UpdateLDPDA_MBlk( void  *hLD_PDA_mblk, 
                              float  fPitch, 
                              float  fCost
                            )
{
  assert(hLD_PDA_mblk != NULL);

  ((LD_PDA_MBlk *)hLD_PDA_mblk)->fLastP0 = fPitch;
  ((LD_PDA_MBlk *)hLD_PDA_mblk)->fLastCost = fCost;
}

