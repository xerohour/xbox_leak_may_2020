/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:     RefineC0.c                        
*                                                                              
* Purpose:      Refine the coarse pitch estimate using the comb-filter
*               correlation to account for the possibly different analysis
*               window used in computing the voicing probability.
*                                                                              
* Functions:    VOXErr VoxRefineC0 ( float fCoarsePeriod, float *pfFltPwr, 
*                      float *pfPeakFrq, int iBpeaks, float *pfRefinedPeriod,
*                      float *pfRefinedRho ) 
*                                                                              
* Author/Date:  Bob McAulay/ 1/97                                              
********************************************************************************
* Modifications: Xiaoqin Sun, nonlinear pitch searching range.
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/RefineC0.c_v   1.3   02 Apr 1998 14:49:58   weiwang  $
*******************************************************************************/
#include <math.h>
#include <assert.h>

#include "VoxMem.h"
#include "vLibQuan.h"
#include "vLibMath.h"

#include "RefineC0.h"
#include "codec.h"

#include "corrfunc.h"
#include "LDPDA.h"

#include "xvocver.h"


/******************************************************************************/
unsigned short VoxRefineC0 (STACK_R float fCoarsePeriod, float *pfFltPwr, float *pfPeakFrq, 
			     int iBpeaks, float *pfFltEnergy, float *pfRefinedPeriod,
			     float *pfRefinedRho ) 
{
  int j;
  float  fLoEdge, fHiEdge;
  float  fFltEnergy, fRefinedMax, fRefinedPeriod;
  float  *PitchTable;
  int  PitchTableSize;
  unsigned int iPitchIndex;
  float  deltapitch, dpitch;
  unsigned long mult_v2;

    
  STACK_INIT
  unsigned long iPeakFrq[MAXHARM];
  STACK_INIT_END
  STACK_ATTACH (unsigned long *, iPeakFrq) 

  STACK_START

  mult_v2= (unsigned long) (1<<F0_SHIFT_11B);
  
  for (j = 0; j < iBpeaks; j++)
	iPeakFrq[j]=(unsigned long)(pfPeakFrq[j]*mult_v2+0.5);

  /*** Get the pitch table ***/
  GetPitchTable(&PitchTable, &PitchTableSize);
 
  /*** Locate the search range for the refined pitch ***/

  fRefinedPeriod = fCoarsePeriod;

  /*search pitch index from the PitchFunction table*/
  iPitchIndex = ScalarQuant( fRefinedPeriod, PitchTable, PitchTableSize );
   
  /* Find the pitch difference */
  if( iPitchIndex >=1) 
    {
      deltapitch = PitchTable[iPitchIndex]-PitchTable[iPitchIndex-1];
      if (iPitchIndex < (unsigned int)(PitchTableSize -1)) 
	      deltapitch = 0.5F*(PitchTable[iPitchIndex+1]-PitchTable[iPitchIndex-1]);
    }
  else 
    {
      deltapitch = PitchTable[iPitchIndex+1]-PitchTable[iPitchIndex];
    }
  assert(deltapitch > 0);

  /*** Compute the energy in the flattened peaks ***/

  fFltEnergy = 1.E-10F;
  for (j = 0; j < iBpeaks; j++)
    fFltEnergy += pfFltPwr[j];

  fRefinedMax = 0.0F;

   
  /********************************************************************
    Find the best pitch candidates from the range: P-0.75dP:P+0.75dP
    *******************************************************************/
  /* get pitch step-size */
  dpitch =FRACTION1*deltapitch;

  /* find the pitch searching range */
  fLoEdge = fRefinedPeriod - SRANGE1*deltapitch;
  if (fLoEdge < (float) MIN_PITCH)
    fLoEdge = (float) MIN_PITCH;

  fHiEdge = fRefinedPeriod + SRANGE1*deltapitch;
  if (fHiEdge > (float) MAX_PITCH)
    fHiEdge = (float) MAX_PITCH;
  
  /* calculate the correlation function and get the best candidate */
  corrfunc( dpitch, fLoEdge, fHiEdge, &fRefinedMax, iBpeaks, pfFltPwr, fFltEnergy, 
	   iPeakFrq, &fRefinedPeriod);

  /********************************************************************
    Second run: test from fine resolution: P-0.25dP:P+0.25dP
    *******************************************************************/
  /* get pitch step-size */
  dpitch =FRACTION2*deltapitch;

  /* find the pitch searching range */
  fLoEdge = fRefinedPeriod - SRANGE2*deltapitch;
  if (fLoEdge < (float) MIN_PITCH)
    fLoEdge = (float) MIN_PITCH;
  fHiEdge = fRefinedPeriod + SRANGE2*deltapitch;
  if (fHiEdge > (float) MAX_PITCH)
    fHiEdge = (float) MAX_PITCH;
  
  /* calculate the correlation function and get the best candidate */
  corrfunc(dpitch, fLoEdge, fHiEdge, &fRefinedMax, iBpeaks, pfFltPwr, fFltEnergy, 
	   iPeakFrq, &fRefinedPeriod);

  /*** Compute the correlation coefficient and final refined pitch period ***/
 *pfFltEnergy = fFltEnergy;

 fRefinedMax = 0.5F * fRefinedMax;

 *pfRefinedRho = (fRefinedMax/fFltEnergy);

 *pfRefinedPeriod = fRefinedPeriod;
  
 STACK_END

 return 0;
 
} /*VoxRefineC0()*/
 
/***************************************************************************/ 
