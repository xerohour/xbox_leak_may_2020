/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       Sws.c
*
* Purpose:        Sine-wave synthesis of harmonic amplitudes
*                                                                              
* Functions:      
*
*
* Author/Date:    Rob Zopf   12/05/96
********************************************************************************
* Modifications:
*
*                                                                              
* Comments:                                                                    
* 
* Concerns:        
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/Sws.c_v   1.4   20 Apr 1998 14:40:18   weiwang  $
*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "VoxMem.h"
#include "model.h"
#include "research.h"

#include "vLibMath.h"   /* nuRand()    */
#include "vLibSgPr.h"   /* SpecIntLn() */
#include "vLibVec.h"    /* L2Scale()   */
#include "vLibFilt.h"   /* PP_NUM_SAMPLES */

#include "HarmSynF.h"
#include "GetnHarm.h"
#include "SumSyn.h"
#include "trig.h"

#include "Sws.h"

#include "xvocver.h"

#define UV_SUPPRESSION_FACTOR 0.5F /* 0.5F */

typedef struct tagSWS {
   float           PitchCycle[EXP_SIZE+PP_NUM_SAMPLES];/* scratch memory */
   float           fResampRate;
   float           fNormPhase;
   float           fVoicePhase;
   void*           pvSumSynBlk;
   long            lNrSeed;               /* seed for random number generator */
} SWS;

/*******************************************************************************
* Function:  Sws()
*
* Action:    Sine-wave synthesis of harmonic amplitudes for both voiced and 
*            unvoiced portions of the spectrum in a single step.
*
* Input:     Amps          - the current frame harmonic amplitudes
*            Amps_1        - the previous frame harmonic amplitudes
*            Pitch         - the current frame pitch lag (time-domain)
*            Pitch_1       - the previous frame pitch lag (time-domain)
*            Pv            - the current frame voicing probability
*            Pv_1          - the previous frame voicing probability
*            frameLEN      - the current frame length
*            nSubs         - the number of sumbframes for the current frame
*            outspeech     - pointer to a vector of length frameLEN for 
*                              synthesis
*
* Output: 
*            outspeech     - the synthesized speech         
*
*
* Globals:        none
*
* Return:
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
void VoxSws  (
          STACK_R
          void   *pSWS_mblk,
          float  *Amps,
          float  *Amps_1,
          float   Pitch,
          float   Pitch_1,
          float   Pv,
          float   Pv_1,
          short   frameLEN,
          short   nSubs,
          float  *outspeech
          ) 
{
   short     subframeLEN, overlap; 
   short     pitch_mult, subnHarm, subvHarm;
   int       sub,i,j;
   float     interp, subPv, subPitch;
   float     multPitch, subPitch_1, uv_energy_desired;
   SWS *SWS_mblk= (SWS *)pSWS_mblk;
   short     tempvHarm;

   STACK_INIT
   float     Spec_sub[MAXHARM];
   int       inPhase[MAXHARM];
   STACK_INIT_END

   STACK_ATTACH(float*,Spec_sub)
   STACK_ATTACH(int*,inPhase)

   STACK_START

   subframeLEN  = (short) (frameLEN/nSubs);
   overlap      = (short)(OVERLAPFACTOR*subframeLEN);
   subPitch_1   = Pitch_1;

   if ((overlap&1) == 1)
      overlap--;

   for (sub=0; sub < nSubs; sub++)
   {
      {
         STACK_INIT
         float     Spec_sub_1[MAXHARM];
         STACK_INIT_END
         STACK_ATTACH(float*,Spec_sub_1)

         STACK_START

         if ((Pv > 0.0) && (Pv_1 > 0.0))
         {
            interp = ((float)(sub+1))/((float)nSubs);
         }
         else
         {
            interp = (sub < (nSubs>>1))?0.0F:1.0F;
         }
       /*---------------------------------
          Don't interpolate Pv
         ---------------------------------*/
         subPv = (sub < (nSubs>>1))?Pv_1:Pv;

       /*----------------------------------------------
         Calculate the pitch for this subframe ...
         ----------------------------------------------*/
         if (fabs((Pitch-Pitch_1)/Pitch) < PITCH_RANGE_FOR_INTERP)
            subPitch  = Pitch_1 + interp*(Pitch-Pitch_1);
         else
            subPitch = (sub < (nSubs>>1))?Pitch_1:Pitch;
         
         if (subPitch > MAX_PITCH)
            subPitch = (float)MAX_PITCH;
         if (subPitch < MIN_PITCH)
            subPitch = (float)MIN_PITCH;

       /*-----------------------------------------------
         Calculate the highest multiple of the pitch that
         is still less than or equal to MAX_PITCH
         -----------------------------------------------*/
         pitch_mult = (short)(((float)MAX_PITCH_MULT)/subPitch);
         if (pitch_mult==0)
            pitch_mult=1;

         multPitch  = subPitch*((float)pitch_mult);

       /*---------------------------------------------------
         Calcuate the number of harmonics for multPitch, and
         the number of voiced harmonics.  The true number
         of voiced harmonics will be the largest even value
         less than or equal to the value calculated divided
         by pitch_mult (since this is what we multiplied the
         true voiced pitch by.
         ---------------------------------------------------*/
         tempvHarm = getvHarm(subPitch, subPv);
         subvHarm  = pitch_mult*tempvHarm;
         subnHarm  = getnHarm(multPitch);

         if (subnHarm > MAXHARM)
            subnHarm=MAXHARM;
         if (subvHarm > MAXHARM)
            subvHarm=MAXHARM;

         for (i=0; i < subvHarm; i++)
            inPhase[i] = 0;

       /*----------------------------------------------------
          Patent Breaking Magical 7 ...
         ----------------------------------------------------*/
         if (tempvHarm > 0)
            inPhase[(((tempvHarm>>1)+1)*pitch_mult)-1] = 7; 

       /*-----------------------------------------------
         Calculate the harmonic amplitudes at the 
         current subframe pitch value...
         -----------------------------------------------*/
         SpecIntLn(Pitch_1, multPitch, (const int) subnHarm, Amps_1, 
                   Spec_sub_1, (const unsigned int) MAXHARM);
         SpecIntLn(Pitch, multPitch, (const int) subnHarm, Amps, 
                   Spec_sub, (const unsigned int) MAXHARM);

         for (i=0; i < subnHarm; i++)
            Spec_sub[i] = Spec_sub_1[i] + 
            interp*(Spec_sub[i]-Spec_sub_1[i]);

         STACK_END
      }

    /*-----------------------------------------------
      Set the unwanted harmonics to zero ...
      -----------------------------------------------*/
      for (i=0; i < pitch_mult-1; i++)
      {
         for (j=i; j < subvHarm; j+=pitch_mult)
         {
            Spec_sub[j] = 0.0F;
         }
      }

    /*-----------------------------------------------
      Calculate the desired unvoiced energy ...
      -----------------------------------------------*/
      uv_energy_desired = 0.0F;
      for (i=subvHarm+pitch_mult-1; i < subnHarm; i+=pitch_mult)
      {
         uv_energy_desired += (Spec_sub[i])*(Spec_sub[i]);
      }
    /*------------------------------------------------
      Rescale the unvoiced energy ...
      ------------------------------------------------*/
      L2Scale(&(Spec_sub[subvHarm]), (int) (subnHarm-subvHarm), 
              uv_energy_desired*UV_SUPPRESSION_FACTOR);

    /*------------------------------------------------
      Randomize the unvoiced phase ...
      ------------------------------------------------*/
      for (i=subvHarm; i < subnHarm; i++)
      {
         inPhase[i] = (int)((NuRand(&(SWS_mblk->lNrSeed)))&(TRIG_SIZE-1));
      }

#if USE_FHT_SYN==1
      HarmSynFHTFloat(STACK_S Spec_sub, inPhase, multPitch, subPitch, 
         subPitch_1, &SWS_mblk->fNormPhase, &SWS_mblk->fVoicePhase,
         &SWS_mblk->fResampRate, subnHarm, SWS_mblk->PitchCycle, 
         &(outspeech[sub*subframeLEN]), subframeLEN, (short)(overlap));
#else
{
   float Phase[MAXHARM];

   for (i=0; i<subnHarm; i++)
      Phase[i] = ((float)inPhase[i])*D_PI*( 1.0/(TRIG_SIZE) );

      SumSyn(STACK_S SWS_mblk->pvSumSynBlk, Spec_sub, subPitch, subPitch_1, 
         Phase, multPitch, pitch_mult, subnHarm, subvHarm, subframeLEN, 
         (float)OVERLAPFACTOR, &(outspeech[sub*subframeLEN]));
}
#endif

      subPitch_1 = subPitch;
   }
   STACK_END

}

unsigned short VoxInitSws(void **hSws) 
{
   SWS *pSws;
   int i;

   if (VOX_MEM_INIT(pSws=*hSws, 1, sizeof(SWS))) 
      return 1;

   if(InitSumSyn(&pSws->pvSumSynBlk))
      return 1;

   /* initializing static variables */
   for (i=0; i < EXP_SIZE+PP_NUM_SAMPLES; i++) 
      pSws->PitchCycle[i] = 0.0F;

   pSws->fResampRate = 0.0F;
   pSws->fNormPhase  = 0.0F;
   pSws->fVoicePhase = 0.0F;

   pSws->lNrSeed = 1L;  /* initial seed for random number generator */

   return 0;
}

unsigned short VoxFreeSws(void **hSws)
{
   FreeSumSyn(&(((SWS *)(*hSws))->pvSumSynBlk));

   VOX_MEM_FREE(*hSws);

   return 0;
}


