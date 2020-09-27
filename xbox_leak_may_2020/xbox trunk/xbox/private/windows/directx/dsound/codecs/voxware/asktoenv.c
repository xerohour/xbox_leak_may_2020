/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       AskToEnv.c
*                                                                              
* Purpose:        Compute magnitude and phase envelopes from arcsin of
*                   reflection coefficients.  
*                                                                              
* Functions:      VoxAskToEnv()
*                                                                              
* Author/Date:    Bob Dunn 1/98
*******************************************************************************
* Modifications:                                                               
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/AskToEnv.c_v   1.8   10 Apr 1998 16:47:34   bobd  $
******************************************************************************/
#include <stdlib.h>
#include <assert.h>
#include "VoxMem.h"
#include "codec.h"
#include "vLibTran.h"
#include "PostFilt.h"
#include "AskToEnv.h"

#include "xvocver.h"

/*******************************************************************************
*
* Function:  VoxAskToEnv()
*
* Action:    Get the spectrum envelope from ASKs. 
*
* Input:    *pfASK -- input ASKs
*           fLog2Gain -- Log2 of residuel gain
*           pfEnv -- pointer to the output envelope amplitudes
*           pfPhase -- pointer to the output phases
*
* Output:   pfEnv -- output envelope amplitudes
*           pfPhase -- the output phases
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

void VoxAskToEnv( STACK_R 
                  const float *pfASK, 
                  const float  fLog2Gain, 
                  float       *pfEnv, 
                  float       *pfPhase
                 )
{
STACK_INIT
   float  fCS[ORDER_CEPSTRAL];
   float  fSW[CSENV_NFFT+2];
   float  fKS[LPC_ORDER];
   float  fAS[LPC_ORDER+1];
STACK_INIT_END

   STACK_ATTACH(float *, fCS)
   STACK_ATTACH(float *, fSW)
   STACK_ATTACH(float *, fKS)
   STACK_ATTACH(float *, fAS)

   STACK_START

   assert((pfASK != NULL) && (pfEnv != NULL) && (pfPhase != NULL));

   /*-------------------------------------------------------------------
     Compute the predictor coefficients from the ASKs
   -------------------------------------------------------------------*/
   ASK2K(LPC_ORDER, pfASK, fKS );
   VoxKsToAs(fKS, fAS, LPC_ORDER);
 
   /*-------------------------------------------------------------------
     Compute amplitude and phase envelopes from cepstral coefficients
   -------------------------------------------------------------------*/
   VoxAsToCs(fLog2Gain, fAS, LPC_ORDER, fCS, ORDER_CEPSTRAL);
 
   VoxCsToEnv(fCS, ORDER_CEPSTRAL, CSENV_FFT_ORDER, fSW);

   VoxDemultiplex(fSW,  pfEnv, pfPhase, CSENV_NFFT_2);

   /*-------------------------------------------------------------------
     Postfilter the envelope.
   -------------------------------------------------------------------*/
   PostFilter(pfEnv, fLog2Gain, fKS);

   STACK_END
}

