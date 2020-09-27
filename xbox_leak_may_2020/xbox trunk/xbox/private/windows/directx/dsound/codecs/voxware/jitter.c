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
* Filename:     Jitter.c
*
* Purpose:      Add a pitch-dependent phase perturbation to account for
*               irregular glottal pulse timing.
*
* Functions:    PhaseJitter()
*
* Author/Date:  Bob Dunn 1/98 (original Bob McAulay)
*
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/Jitter.c_v   1.3   13 Apr 1998 18:26:16   weiwang  $
*
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "vLibMath.h"
#include "codec.h"
#include "Jitter.h"

#include "xvocver.h"

/*--------------------------------------------------------------------
  There is no phase jitter below fJitterCutoff
--------------------------------------------------------------------*/
#define JITTER_CUTOFF      (1500.0F/(float)RATE8K) /*(1500.0F*HZ2DFT) */
#define PERTURB_FACTOR     (2.0F*INVTWOPI)
#define PERTURB_SLOPE      (0.1F*INVTWOPI)

/*******************************************************************************
*
* Function:  PhaseJitter()
*
* Action:    Add a pitch-dependent phase perturbation to account for
*               irregular glottal pulse timing.
*
* Input:     float     fPitch   -> pitch period
*            float    *pfPhase     -> harmonic phases (in)
*            int       iBasePeaks  -> number of baseband measure phases
*            int       iHarmonics  -> number of harmonics
*
* Output:    float    *pfPhase     -> harmonic phases (out)
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


void PhaseJitter ( float     fPitch,
                   float    *pfPhase,
                   int       iBasePeaks,
                   int       iHarmonics
                 )
{
   int   n;
   int iJitterHarm;
   float fPerturb = PERTURB_FACTOR;

   assert(pfPhase != NULL);

   iJitterHarm = VoxFLOORpos(JITTER_CUTOFF * fPitch);

   if (iJitterHarm < iBasePeaks)
     fPerturb = PERTURB_FACTOR + (float)(iBasePeaks-iJitterHarm)*PERTURB_SLOPE;

   for (n = iBasePeaks; n < iHarmonics; n++)
     {
       if (n >= iJitterHarm)  /* increase perturbation factor */
         fPerturb += PERTURB_SLOPE;
       
       if (( n & 1) == 0)  /* only apply to odd harmonics */
         pfPhase[n] += fPerturb;
     }
} /* PhaseJitter */

