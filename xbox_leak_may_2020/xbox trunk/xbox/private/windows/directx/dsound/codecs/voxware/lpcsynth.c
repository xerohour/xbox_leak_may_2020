/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       lpcSynth.c
*                                                                              
* Purpose:        Pass the excitation throught the lpc synthesis filter.
*                 Interpolate the filter for each subframe.
*                                                                              
* Functions:      VOXErr VoxInitLPCSyn(Synthesis *mySynthesis_mblk);
*                 VOXErr VoxFreeLPCSyn(Synthesis *mySynthesis_mblk);
*                 VOXErr VoxLPCSyn(float *lsp, short *SpeechOutBuf, 
*                                  float *exciteIN, Synthesis *mySynthesis_mblk,
*                                  short outputLEN);
*                                                                              
* Author/Date:    Rob Zopf   02/08/96
*
********************************************************************************
* Modifications:Ilan Berci/ 10/96 Removed global dependencies
*                                 Modularized code segments (Encapsulation)
*                                 Removed scope changes
*                                 Changed error code procedure
*                                 Removed useless code segments
*                                 General optimization
*                                                                              
* Comments:                                                                    
* 
* Concerns:        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*                  /   - NOTE : this buffer must be of length
*                  /     FRAMELEN+LPC_ORDER with the first sample
*                  /     of the excitation at exciteIN[LPC_ORDER].
*                  /     This is to allow for the filter memory.
*                  /
*                  /   - ANOTHER NOTE: this routine will spoil the data
*                  /     inside of exciteIN
*                  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/lpcSynth.c_v   1.4   20 Apr 1998 14:41:38   weiwang  $
*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "VoxMem.h"
#include "model.h"
#include "vLibTran.h" /* VoxLspToAsEven() */
#include "vLibVec.h"  /* DotProd()        */

#include "lpcSynth.h"

#include "xvocver.h"

static const float cfInvRate8k = 1.0F/RATE8K;

/*******************************************************************************
* Function:
*
* Action:
*
* Input:          lsp      : the quantized original lsp's
*           SpeechOutBuf   : the output buffer of length "FRAMELEN_29"
*               exciteIN   : the excitation
*                prevLSP   : the previous frame lsp's
*              interpLSP   : buffer for interpolating into
*               interLPC   : buffer for converting interpolated lsp's
*                 lpcMEM   : the memory of the synthesis filter
*
* Output:   SpeechOutBuf   : the synthetic speech
*                 lpcMEM   : the updated synthesis filter memory
*
* Globals:        none
*
* Return:        VOXErr : Voxware return code
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
unsigned short VoxLPCSyn( STACK_R 
                          float *lsp, 
                          float *exciteIN, 
                          void  *pSynthesis_mblk, 
                          short  outputLEN
                        )
{
   Synthesis *Synthesis_mblk = (Synthesis *)pSynthesis_mblk;
   float     *prevLSP        = Synthesis_mblk->prevLSP;

   STACK_INIT
   float    interpLSP[LPC_ORDER];
   float    interpLPC[LPC_ORDER+1];
   STACK_INIT_END
   STACK_ATTACH(float*,interpLSP)
   STACK_ATTACH(float*,interpLPC)
   float   *lpcMEM = Synthesis_mblk->lpcMEM;
   int      SubFrame, Samp, Index;
   float    InterpFactor;
   int      SampsDone;
   float    temp;
   float   *pexciteIN, *Offset_exciteIN;
   short    SubframeLen;
   short    TrueSubLen;
   float    deltaInterp;
   int      i;
   float   *pSpeech;

   STACK_START

 /*--------------------------------------------------
   Compute the number of lsf subframes we should use
   --------------------------------------------------*/
   TrueSubLen  = (short)(((float)outputLEN)*(1.0/(float)LPCSYN_SUBFRAMES));
   SubframeLen = TrueSubLen>>1;  /* first subframe is half */
   deltaInterp = (1.0F/((float)LPCSYN_SUBFRAMES));

 /*------------------------------------------------
   Copy the memory into the input buffer
   ------------------------------------------------*/
   memcpy(exciteIN, lpcMEM, LPC_ORDER*sizeof(*exciteIN));

 /*------------------------------------------------
   For each Subframe ...
      1. Interpolate the lsp's
      2. Convert lsp's to lpc's
      3. Filter 1 subframe worth of excitation
   ------------------------------------------------*/
   SampsDone=0;
   Offset_exciteIN = exciteIN; /* speeds stuff up below */
   InterpFactor = 0.0F;
   VoxLspToAsEven(prevLSP, LPC_ORDER, cfInvRate8k, interpLPC);

   for (SubFrame=0; SubFrame<=LPCSYN_SUBFRAMES; SubFrame++)
   {
      for (i=0;i<(LPC_ORDER>>1);i++)
      {
         temp = interpLPC[i+1];
         interpLPC[i+1] = -interpLPC[LPC_ORDER-i];
         interpLPC[LPC_ORDER-i] = -temp;
      }

      if (SubFrame == (LPCSYN_SUBFRAMES))
          SubframeLen = outputLEN - SampsDone;

      for (Samp = SampsDone; Samp<(SampsDone+SubframeLen); Samp++)
      {
         temp=0.0F;
         pexciteIN = Offset_exciteIN+Samp;

         temp = (float)DotProd(pexciteIN, interpLPC+1, LPC_ORDER);
         *(pexciteIN+LPC_ORDER)+=temp;
      }
      SampsDone+=SubframeLen;
      SubframeLen = TrueSubLen;

      InterpFactor += deltaInterp;
      if (SubFrame == (LPCSYN_SUBFRAMES-1))
      {
         VoxLspToAsEven(lsp, LPC_ORDER, cfInvRate8k, interpLPC);   
      }
      else
      {
         for (Index=0; Index<LPC_ORDER; Index++)
         {
            interpLSP[Index] = prevLSP[Index]+((lsp[Index]-prevLSP[Index])*InterpFactor);
         }

       /*-----------------------------------------------------
         Make sure that after interpolation, that the lsp's
         are seperated by at least LSP_SEPERATION_HZ Hertz
         -----------------------------------------------------*/
         for (Index=1; Index<LPC_ORDER; Index++)
            if ((interpLSP[Index] + LSP_SEPERATION_HZ)<interpLSP[Index-1])
               interpLSP[Index] = (float)(interpLSP[Index-1]+LSP_SEPERATION_HZ);

         VoxLspToAsEven(interpLSP, LPC_ORDER, cfInvRate8k, interpLPC);
      }
   }

 /*---------------------------------------------------------
   Update frame-to-frame memory of lsps, filter...
   ---------------------------------------------------------*/ 
   memcpy(Synthesis_mblk->prevLSP, lsp, LPC_ORDER*sizeof(*Synthesis_mblk->prevLSP));
   memcpy(Synthesis_mblk->lpcMEM, &(Offset_exciteIN[outputLEN]),
          sizeof(*Synthesis_mblk->lpcMEM)*LPC_ORDER);

 /*--------------------------------------------------------
   Check overflow before writing to the SHORT output buffer
   --------------------------------------------------------*/
   pSpeech = Offset_exciteIN + LPC_ORDER;
   for (Index=0; Index<outputLEN; Index++)
   {
      if (pSpeech[Index] > (float)SPEECH_CLIP_MAX)
         pSpeech[Index] = (float)SPEECH_CLIP_MAX;
      else if (pSpeech[Index] < (float)SPEECH_CLIP_MIN)
         pSpeech[Index] = (float)SPEECH_CLIP_MIN;
   }


   STACK_END
   return 0;
} 

/*******************************************************************************
* Function:       VOXErr VoxInitLPCSyn(Synthesis *Synthesis_mblk)
*
* Action:         Initializes the LPCSyn struct
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:         VOXErr : Voxware return code
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
unsigned short VoxInitLPCSyn(void **hSynthesis_mblk)
{
   Synthesis *Synthesis_mblk;
   short i;

   if(VOX_MEM_INIT(Synthesis_mblk=*hSynthesis_mblk,1,sizeof(Synthesis)))
      return 1;

   if(VOX_MEM_INIT(Synthesis_mblk->prevLSP,LPC_ORDER,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(Synthesis_mblk->lpcMEM,LPC_ORDER,sizeof(float))) 
      return 1;

   for(i=0;i<LPC_ORDER;i++) 
      (Synthesis_mblk->lpcMEM)[i]=0.0F;

      for(i=0;i<LPC_ORDER;i++) 
		Synthesis_mblk->prevLSP[i] = LSP_INITIAL_COND_DEL*i + LSP_INITIAL_COND_0;

   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxFreeLPCSyn(Synthesis *Synthesis_mblk)
*
* Action:         frees up the LPCSyn struct
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:        VOXErr : Voxware return code
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
unsigned short VoxFreeLPCSyn(void **hSynthesis_mblk)
{
   Synthesis *Synthesis_mblk=(Synthesis *)*hSynthesis_mblk;

   if(*hSynthesis_mblk) {
      VOX_MEM_FREE(Synthesis_mblk->prevLSP);
      VOX_MEM_FREE(Synthesis_mblk->lpcMEM);

      VOX_MEM_FREE(*hSynthesis_mblk);
   }

   return 0;
}


