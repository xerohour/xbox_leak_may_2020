/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       UVPhase.c
*
* Purpose:        Add random phase to unvoiced harmonics.
*
* Functions:      UnvoicedPhase()
*
* Author/Date:    Bob Dunn 12/18/97
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/UVPhase.c_v   1.3   16 Mar 1998 15:16:16   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "vLibMath.h"

#include "codec.h"
#include "UVPhase.h"

#include "xvocver.h"

/*---------------------------------------------------------------
  Scale factor to generate uniform random number on [0,TWOPI]
---------------------------------------------------------------*/
static const float fPhaseScale = NURAND_SCALE*TWOPI;

/*******************************************************************************
*
* Function:  UnvoicedPhase()
*
* Action:    Add random phase to unvoiced harmonics.
*
* Input:     float *pfPhase     -> harmonic phases
*            int    iHarmonics  -> total number of harmonics
*            int    iVoicHarm   -> number of voiced harmonics
*            long  *plRandSeed --> address to the random seed
*
* Output:    float *pfPhase     -> harmonic phases
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

void UnvoicedPhase( float *pfPhase,
                    int    iHarmonics,
                    int    iVoicHarm,
                    long  *plRandSeed
                  )
{
   int   i;

   assert(pfPhase != NULL && plRandSeed != NULL);

   /*---------------------------------------------------------------
     Add a random phase above the cutoff for noise-like synthesis
   ---------------------------------------------------------------*/
   for (i=iVoicHarm; i<iHarmonics; i++) {
      pfPhase[i] = fPhaseScale*(float)NuRand(plRandSeed);
   }
}

