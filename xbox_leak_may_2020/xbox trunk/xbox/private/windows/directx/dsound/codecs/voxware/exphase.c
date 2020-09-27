/*******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
*******************************************************************************/

/*******************************************************************************
*
* Filename:     ExPhase.c
*
* Purpose:      Create voiced and measured phase components of the 
*                 excitation phase.
*
* Functions:    ExcitationPhase()
*
* Author/Date:  Bob McAulay, Spring 1997
*
********************************************************************************
*
* Modifications:  Code cleaning and such. Bob Dunn 12/97.
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/ExPhase.c_v   1.1   16 Mar 1998 10:33:36   weiwang  $
*
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "ExPhase.h"

#include "xvocver.h"

/*******************************************************************************
*
* Function:  ExcitationPhase()
*
* Action:    Create voiced and measured phase components of the
*                 excitation phase.
*
* Input:     float  fOnsetPhase -> the onset phase
*            float  fBeta       -> the beta factor
*            float *pfVEVPhase  -> measured phase residuals
*            float *pfMinPhase  -> minimum-phase phases
*            int    iNpeaks     -> total number of harmonics
*            int    iBasePeaks  -> number of measured phase residuals
*
* Output:    float *pfMinPhase  -> output phases with voiced and measured
*                                    phase components
*
* Globals:   none
*
* Return:    void
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void ExcitationPhase( float  fOnsetPhase,
                      float  fBeta,
                      const float *pfVEVPhase,
                      float *pfMinPhase,
                      int    iNpeaks,
                      int    iBasePeaks
                    )
{
   int   i;
   float fLinearPhase;
   
   assert((pfMinPhase != NULL) && (pfVEVPhase != NULL));

 /*--------------------------------------------------------------------------
   Compute the linear phase component with phase offset
 --------------------------------------------------------------------------*/
   fLinearPhase = fOnsetPhase + fBeta;
   for (i=0; i<iNpeaks; i++)
   {     
      pfMinPhase[i] += fLinearPhase;
      fLinearPhase += fOnsetPhase;
   }


 /*--------------------------------------------------------------------------
   Append the quantized phase residuals to the linear phase if iBasePeaks > 0
 --------------------------------------------------------------------------*/
   for (i=0; i<iBasePeaks; i++)
      pfMinPhase[i] += pfVEVPhase[i];

}

