/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       SumSyn.c
*                                                                              
* Purpose:        Sine-wave synthesis of harmonic amplitudes with a sum of 
*                   sinusoids.
*
* Functions:  SumSyn()
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
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/SumSyn.c_v   1.2   02 Mar 1998 18:18:14   weiwang  $
*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "VoxMem.h"
#include "vLibVec.h" /* OlAdd() */
#include "model.h"

#include "SumSyn.h"

#include "xvocver.h"

void HarmonicSum (float *A,                 /* harmonic amplitudes            */
                  float *phi_o,             /* input phases (0-2PI)           */
                  float  w,                 /* current  fundamental (0-2PI)   */
                  short  startHarm,         /* first harm to synthesize(0..)  */
                  short  endHarm,           /* last harm (not including)      */
                  float *OutBuf,            /* output buffer                  */
                  short  L                  /* length of output buffer        */
                 );

typedef struct tagSumSynBlk {
   float BasePhase;
   float prevPhase[MAXHARM];
   float Mw_1;
   float Amp_1[MAXHARM];
   unsigned short nHarm_1;
} SumSynBlk;


/*******************************************************************************
* Function:       SumSyn()
*
* Action:         Sine-wave synthesis of harmonic amplitudes with a sum of 
*                   sinusoids
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		   VOXErr : Voxware return code
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
void SumSyn( STACK_R 
             void  *pvSumSynBlk, 
             float *Amp, 
             float  Pitch, 
             float  Pitch_1, 
             float *Phase, 
             float  multPitch, 
             short  pitch_mult, 
             short  nHarm, 
             short  vHarm, 
             short  frameLen, 
             float  interpFactor, 
             float *outSpeech
           )
{

   SumSynBlk *pSumSynBlk=(SumSynBlk *)pvSumSynBlk;
   STACK_INIT
   float  fSynBuf[MAXSYNLEN];  /* scratch */
   float  pfPrevSyn[MAXSYNLEN];  /* scratch */
   STACK_INIT_END
   STACK_ATTACH(float*,fSynBuf)
   STACK_ATTACH(float*,pfPrevSyn)
   short olLEN;
   short synLEN;
   float w, w_1;
   float Mw;
   float BaseStep;
   int i;
   STACK_START

   olLEN = (short)(interpFactor * frameLen);
   synLEN   = olLEN + ((frameLen-olLEN)>>1);
   w = (float)(D_PI/Pitch);
   w_1 = (float)(D_PI/Pitch_1);
   Mw = (float)(D_PI/multPitch);

   /*------------------------------------------------------------------------------------
      Synthesize the previous section of speech ...
     ------------------------------------------------------------------------------------*/
   HarmonicSum (pSumSynBlk->Amp_1, pSumSynBlk->prevPhase, pSumSynBlk->Mw_1, 0, 
      pSumSynBlk->nHarm_1, pfPrevSyn, synLEN );
     
   /*------------------------------------------------------------
      Set up the phases 
     ------------------------------------------------------------*/
   pSumSynBlk->BasePhase += (w+w_1)*0.5F*frameLen;
   pSumSynBlk->BasePhase  = (float)fmod(pSumSynBlk->BasePhase, D_PI);

   BaseStep = (float)(fmod((pSumSynBlk->BasePhase-synLEN*w), D_PI));
   BaseStep /=(float)pitch_mult;

   for (i=0;i<vHarm;i++)
   {
      Phase[i] = BaseStep*(i+1);
   }

   HarmonicSum (Amp, Phase, Mw, 0, nHarm, fSynBuf, synLEN );

   for (i=0; i<nHarm; i++)
      pSumSynBlk->prevPhase[i] = Phase[i] + Mw*synLEN*(i+1);

 /*----------------------------------------------------
     Do overlap-add of buffers ...
   ----------------------------------------------------*/
   OlAdd(pfPrevSyn, fSynBuf, (const int)olLEN, (const int)frameLen, outSpeech);

   memcpy(pSumSynBlk->Amp_1, Amp, sizeof(float)*MAXHARM);
   pSumSynBlk->nHarm_1 = nHarm;
   pSumSynBlk->Mw_1 = Mw;
   STACK_END
}

unsigned short InitSumSyn(void **hSumSynBlk)
{
   SumSynBlk *pSumSynBlk;

   if(VOX_MEM_INIT(pSumSynBlk=*hSumSynBlk,1,sizeof(SumSynBlk)))
      return 1;
   memset(pSumSynBlk->prevPhase,0,MAXHARM);
   memset(pSumSynBlk->Amp_1,0,MAXHARM);
   pSumSynBlk->BasePhase=0.0F;
   pSumSynBlk->Mw_1=0.0F;
   pSumSynBlk->nHarm_1=0;

   return 0;
}

unsigned short FreeSumSyn(void **hSumSynBlk)
{
   VOX_MEM_FREE(*hSumSynBlk);

   return 0;
}

void HarmonicSum (float *A,                 /* harmonic amplitudes            */
                  float *phi_o,             /* input phases (0-2PI)           */
                  float  w,                 /* current  fundamental (0-2PI)   */
                  short  startHarm,         /* first harm to synthesize(0..)  */
                  short  endHarm,           /* last harm (not including)      */
                  float *OutBuf,            /* output buffer                  */
                  short  L                  /* length of output buffer        */
                 )
{
   register int i;                          /* loop var                       */
   register int m;                          /* loop var                       */
   float        theta_t;                    /* theta(t)                       */
   float        dP;                         /* theta increment                */
   float        Am;                         /* mth amplitude                  */

   memset(OutBuf, 0, sizeof(float)*L);      /* set output buffer to 0         */
   dP     = startHarm*w;                    /* init. delta phase              */
   
   for (m=startHarm; m<endHarm; m++)        /* loop over selected harmonics   */
   {
      theta_t = phi_o[m];                   /* init. theta                    */
      dP     += w;                          /* inc. delta theta               */
      Am      = A[m];                       /* init. A(m)                     */

      for (i=0; i<L; i++)                   /* loop over all samples          */
      {
         OutBuf[i]  += (float)(Am*sin(theta_t));   /* harmonic sum            */
         theta_t    += dP;                         /* increment theta_t       */
      }                                            /* end for(i)              */
   }                                               /* end for(m)              */
} 
