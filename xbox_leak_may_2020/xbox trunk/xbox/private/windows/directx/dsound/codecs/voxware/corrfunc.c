
/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:     corrfunc.c                        
*                                                                              
* Purpose:      Refine the coarse pitch estimate using correlation function
*                                                                              
* Functions:    void corrfunc( float pitch, float fHiEdge, float fRefinMax)
*                                                                              
* Author/Date:  Bob McAulay/ 1/97                                              
********************************************************************************
* Modifications: Xiaoqin Sun     6/97
*
*                Wei Wang  6/19/97: share the trigle table.
*                                                                              
* Comments:   Index of cosine table has been replaced by fix point
*             Pitch was timed by 2^12, and fPeakFrq was timed by 2^11, then 
*             down shift 23 bits                                                        
* 
* Concerns:   Though run on PC the refined pitch values with fixed point version 
*             is very similar with floating point version, it may have a potectial 
*             overflow problem on another type of machines (e.g. UNIX machine).
*             Suggestion ----test on UNIX machine
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/corrfunc.c_v   1.2   27 Mar 1998 16:58:18   weiwang  $
*******************************************************************************/
#include <math.h>
#include <assert.h>

#include "VoxMem.h"
#include "codec.h"
#include "corrfunc.h"
#include "trigSC.h"

#include "xvocver.h"

#define   ARG0                  (TRIG_SIZE>>FFT_ORDER)
#define   N_SHIFTS       (F0_SHIFT_BITS +F0_SHIFT_11B-LOG2_TRIG_SIZE+FFT_ORDER)
#define   OFFSET_ROUND          (1 << (N_SHIFTS-1))
#define   TRIG_TABLE_MASK       (TRIG_SIZE-1)

       
extern const float *cosTBL_32;

void corrfunc(float dpitch, float fpitch, 
              float fHiEdge, float *fRefinedMax,
              int iBpeaks, float *pfFltPwr, 
              float fFltEnergy, unsigned long *iPeakFrq,
              float *fRefinedPeriod)
{
  float fSum_k;

  int j;
  unsigned long lIndex;
  int iIndex;
  unsigned long ipitch;
  unsigned long mult_v1;

  /*** Refine the pitch about the previously estimated value ***/
  
  mult_v1= (unsigned long) (1<<F0_SHIFT_BITS );
  
  while (fpitch <= fHiEdge)
    { 
      ipitch =(unsigned long)(fpitch*mult_v1+0.5);
      fSum_k = fFltEnergy;

      for (j = 0; j < iBpeaks; j++)
        {  
          lIndex = (ipitch*iPeakFrq[j])+ OFFSET_ROUND;
          iIndex = (int)(lIndex >>N_SHIFTS) & TRIG_TABLE_MASK;
          fSum_k += pfFltPwr[j]*cosTBL_32[iIndex];
        }

      /*** Test for a correlation peak and store winning pitch period ***/
      if (fSum_k > *fRefinedMax)
        {
          *fRefinedMax = fSum_k;
          *fRefinedPeriod = fpitch;   /** 1 sample ahead **/
        }
      fpitch += dpitch; 
    }

 
} /* corrfunc.c*/
