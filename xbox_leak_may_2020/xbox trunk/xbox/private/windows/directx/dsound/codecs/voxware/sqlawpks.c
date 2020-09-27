/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       SqLawPks.c                        
*                                                                              
* Purpose:        This module computes the sine-wave amplitudes and frequencies
*                 for the sine-wave representation of the square-law output
*                                                                              
* Functions:      VoxSquareLawPeaks()
*
* Author/Date:    Bob McAulay   1/97
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/SqLawPks.c_v   1.1   11 Apr 1998 15:18:30   bobd  $
*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "vLibMath.h"
#include "codec.h"
#include "SqLawPks.h"

#include "xvocver.h"

/*------------------------------------------------------------
  Set the maximum number of difference frequencies
------------------------------------------------------------*/
#define MAX_SPAN  2
 
/*------------------------------------------------------------
    Eliminate square-law peaks below this threshold.
------------------------------------------------------------*/
#define MIN_NU_THRESHOLD    0.05F

/*------------------------------------------------------------
  Do not eliminate low level peaks unless there are at least
   MIN_NU_PEAKCOUNT new peaks.
------------------------------------------------------------*/
#define MIN_NU_PEAKCOUNT    10
 

/******************************************************************************
*
* Function:  VoxSquareLawPeaks()
*
* Action:    This module computes the sine-wave amplitudes and frequencies
*              for the sine-wave representation of the square-law output
*
* Input:     float *pfFltAmp       -> peak amplitudes
*            float *pfPkFrq        -> peak frequencies (in DFT pts)
*            int    iBasePeakCount -> number of peaks
*            float  fLoBandEdge    -> maximum difference frequency (in DFT pts)
*
* Output:    float *pfNuAmp        -> new peak amplitudes
*            float *pfNuFrq        -> new peak frequencies (in DFT pts)
*            int   *piNuPeaks      -> number of new peaks
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
void VoxSquareLawPeaks ( float *pfFltAmp,
                         float *pfPkFrq,
                         int    iBasePeakCount,
                         float  fLoBandEdge,
                         float *pfNuAmp,
                         float *pfNuFrq,
                         int   *piNuPeaks
                       )
{

   int   j, k;
   int   iPks;
   int   iNuPeakCount;


   /*-----------------------------------------------------------
     Compute the square-law peaks.
   -----------------------------------------------------------*/
   iNuPeakCount = 0;

   /*-----------------------------------------------------------
     Include the original peaks.
   -----------------------------------------------------------*/
   iPks = VoxMIN( iBasePeakCount, MAXHARM );
   for (k = 0; k < iPks; k++)
   {
      pfNuAmp[iNuPeakCount] = pfFltAmp[k];
      pfNuFrq[iNuPeakCount] = pfPkFrq[k];
      iNuPeakCount ++;
   }

   /*-----------------------------------------------------------
     Compute peaks from the first difference.
   -----------------------------------------------------------*/
   iPks = VoxMIN( iBasePeakCount-1, MAXHARM-iNuPeakCount );
   for (k = 0; k < iPks; k++)
   {
      pfNuAmp[iNuPeakCount] = pfFltAmp[k+1] * pfFltAmp[k];
      pfNuFrq[iNuPeakCount] = pfPkFrq[k+1]  - pfPkFrq[k];   
      iNuPeakCount ++;
   }

   /*-----------------------------------------------------------
     Compute peaks from the second difference.
   -----------------------------------------------------------*/
   iPks = VoxMIN( iBasePeakCount-2, MAXHARM-iNuPeakCount );
   for (k = 0; k < iPks; k++)
   {
      pfNuAmp[iNuPeakCount] = pfFltAmp[k+2] * pfFltAmp[k];
      pfNuFrq[iNuPeakCount] = pfPkFrq[k+2]  - pfPkFrq[k];   
      iNuPeakCount ++;
   }

   *piNuPeaks = iNuPeakCount;

   /*-----------------------------------------------------------
     Delete low-level peaks if there are enough of them.
   -----------------------------------------------------------*/
   if (iNuPeakCount >= MIN_NU_PEAKCOUNT)
   {
      j = 0;
      for (k = 0; k < iNuPeakCount; k++)
      {
         if (pfNuAmp[k] > MIN_NU_THRESHOLD)
         {
            pfNuAmp[j] = pfNuAmp[k];
            pfNuFrq[j] = pfNuFrq[k];
            j ++;
         }
      }
      *piNuPeaks = j;
   }

} /*VoxSquareLawPeaks()*/


