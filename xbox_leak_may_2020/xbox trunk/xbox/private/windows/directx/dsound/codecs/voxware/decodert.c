/*
 * File:     Decode.c
 *
 * Project:  RT29
 * *
 * 
 *
 * Written by Rob Zopf.
 *
 * Copyright 1996  Voxware, Inc.
 *
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/DecodeRT.c_v   1.5   01 Dec 1998 14:37:36   zopf  $
 *
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "VoxMem.h"
#include "model.h"
#include "vLibTran.h" /* VoxLspToAsEven() */

#include "vem2.h"
#include "vem2Prm.h"
#include "research.h"

#include "enhance.h"
#include "lpcSynth.h"
#include "Setup.h"
#include "OutBuf.h"
#include "param.h"
#include "Sws.h"
#include "GetnHarm.h"

#include "DecodeRT.h"

#include "xvocver.h"


#ifdef WIN32
#include "vLibTran.h"
#endif

static const float cfInvRate8k = 1.0F/RATE8K;
void SmoothSpectrumInNoise( float *CurrentLsf, float *AveLsf, int *Hangover, float Pv );

unsigned short VoxDecodeRT(STACK_R signed short *pPCMBuffer,void *pMemBlkD)
{
   DEC_MEM_BLK* pDecMemBlk=(DEC_MEM_BLK*)pMemBlkD;
   PARAM	*Param_mblk = (PARAM *)(pDecMemBlk->Param_mblk);
   Synthesis *Synthesis_mblk = (Synthesis *)(pDecMemBlk->Synthesis_mblk);
   SubFrameParam *SubFrameParam_mblk = (SubFrameParam *)(pDecMemBlk->SubFrameParam_mblk);
   PrevFrameParam *PrevFrameParam_mblk = (PrevFrameParam *)(pDecMemBlk->PrevFrameParam_mblk);
STACK_INIT
   float lpc[LPC_ORDER+1];
   float OutBufFloat[FRAME_LEN+LPC_ORDER];
STACK_INIT_END

STACK_ATTACH(float *, lpc)
STACK_ATTACH(float *, OutBufFloat)

   short nHarm;

STACK_START

/* service the "pre decode" state */
   vemServiceState(pDecMemBlk->vemKeyPreDecode);

   VoxSetup(STACK_S Param_mblk, SubFrameParam_mblk, PrevFrameParam_mblk, 
            pDecMemBlk->Warp, &(pDecMemBlk->unpackFLAG));

   nHarm = getnHarm(SubFrameParam_mblk->Pitch);
   if (nHarm > MAXHARM)
       nHarm = MAXHARM;

   SmoothSpectrumInNoise( SubFrameParam_mblk->Lsf, SubFrameParam_mblk->AveLsf, 
                        &(SubFrameParam_mblk->Hangover), SubFrameParam_mblk->Pv );

   /****** perceptually weight the spectrum using the lpc's *********/
   VoxLspToAsEven(SubFrameParam_mblk->Lsf, LPC_ORDER, cfInvRate8k, lpc);

   VoxEnhance(STACK_S lpc, SubFrameParam_mblk->Pv, SubFrameParam_mblk->Pitch, 
           nHarm, SubFrameParam_mblk->Amp);

   memset(OutBufFloat+LPC_ORDER, 0, sizeof(float)*SubFrameParam_mblk->frameLEN);


   /****** Calculate the unvoiced excitation with sine waves *******/
     VoxSws(
           STACK_S
           pDecMemBlk->Sws_mblk,
           SubFrameParam_mblk->Amp,
           SubFrameParam_mblk->Amp_1,
           SubFrameParam_mblk->Pitch,
           SubFrameParam_mblk->Pitch_1,
           SubFrameParam_mblk->Pv,
           SubFrameParam_mblk->Pv_1,
           SubFrameParam_mblk->frameLEN,
           SubFrameParam_mblk->nSubs,
           OutBufFloat+LPC_ORDER
         );
   /****** Do the synthesis using the unvoiced and voiced excitations ******/ 
   VoxLPCSyn(STACK_S SubFrameParam_mblk->Lsf, OutBufFloat, Synthesis_mblk, SubFrameParam_mblk->frameLEN);


   /****** Do the output buffer management *******/
   VoxOutputManager(pDecMemBlk->OutputBuffer_mblk,SubFrameParam_mblk->frameLEN, OutBufFloat+LPC_ORDER);

   /****** Output the speech from the double buffer to the API buffer ******/
   if (((OutputBuffer *)pDecMemBlk->OutputBuffer_mblk)->writeBuffer==1)
   {
      memcpy(pPCMBuffer, ((OutputBuffer *)(pDecMemBlk->OutputBuffer_mblk))->outBufferB1, sizeof(short)*FRAME_LEN);
      pDecMemBlk->writeFLAG=1;
   }
   else if (((OutputBuffer*)(pDecMemBlk->OutputBuffer_mblk))->writeBuffer==2)
   {
      memcpy(pPCMBuffer,((OutputBuffer *)(pDecMemBlk->OutputBuffer_mblk))->outBufferB2, sizeof(short)*FRAME_LEN);
      pDecMemBlk->writeFLAG=1;
   }
   else pDecMemBlk->writeFLAG=0;

   VoxUpdateSubframe(SubFrameParam_mblk);

   /* service the "post decode" state */
   vemServiceState(pDecMemBlk->vemKeyPostDecode);
STACK_END

return 0;
}

unsigned short VoxInitDecodeRT(void **ppMemBlkD, void *pVemMem)
{
   DEC_MEM_BLK *pDecode_mblk;

   if(VOX_MEM_INIT(pDecode_mblk=*ppMemBlkD,1,sizeof(DEC_MEM_BLK))) 
      return 1;

   /****************************************************
    Initialize the Sws block
    ****************************************************/
   if (VoxInitSws(&pDecode_mblk->Sws_mblk)) 
      return 1;

   /****************************************************
    Initialize the LPC Synthesis block
    ****************************************************/
   if (VoxInitLPCSyn(&pDecode_mblk->Synthesis_mblk))
      return 1;

   /****************************************************
    Initialize the Output Buffer block
    ****************************************************/
   if (VoxInitOutputBuffers(&pDecode_mblk->OutputBuffer_mblk)) 
      return 1;

   /****************************************************
    Initialize the subframe parameters block
    ****************************************************/
   if (VoxInitSubframe(&pDecode_mblk->SubFrameParam_mblk)) 
      return 1;

   /****************************************************
    Initialize the prev frame memory parameters block
    ****************************************************/
   if (VoxInitPrevFrame(&pDecode_mblk->PrevFrameParam_mblk)) 
      return 1;

   /****************************************************
    initialize parameter structure
    ****************************************************/
   if (VoxInitParam(&pDecode_mblk->Param_mblk, pVemMem)) 
      return 1;


   /****************************************************
    initialize the dropRate and fWarpedLengthFactor
    ****************************************************/
   pDecode_mblk->dropRate=1;
   pDecode_mblk->fWarpedLengthFactor=1.0F;
   pDecode_mblk->unpackFLAG=1;

   pDecode_mblk->pVemMem=pVemMem;

   if(!(pDecode_mblk->vemKeyPreDecode=vemRegisterState(
      ((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, "pre decode")))
      return 1;

   if(!(pDecode_mblk->vemKeyPostDecode=vemRegisterState(
      ((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, "post decode")))
      return 1;
   
   vemRegisterConstShort(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, 
      (short)RATE8K, "rate");
   vemRegisterConstShort(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, 
      (short)FRAME_LEN, "frame length");
   vemRegisterVariable(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, 
      &pDecode_mblk->writeFLAG, VEM_U_CHAR, "write flag");
      
   return 0;
}


unsigned short VoxFreeDecodeRT(void **hMemBlkD)
{
    DEC_MEM_BLK *pDecode_mblk=(DEC_MEM_BLK*)*hMemBlkD;
  
    if (*hMemBlkD) {

       VoxFreeSws(&pDecode_mblk->Sws_mblk);

       VoxFreeLPCSyn(&pDecode_mblk->Synthesis_mblk);

       VoxFreeOutputBuffers(&pDecode_mblk->OutputBuffer_mblk);

       VoxFreeSubframe(&pDecode_mblk->SubFrameParam_mblk);

       VoxFreePrevFrame(&pDecode_mblk->PrevFrameParam_mblk);

       VoxFreeParam(&pDecode_mblk->Param_mblk);

#ifdef WIN32
       nspFreeFftResources();
#endif

       VOX_MEM_FREE(*hMemBlkD);
    }
    return 0;
}



void SmoothSpectrumInNoise( float *CurrentLsf, float *AveLsf, int *Hangover, float Pv )
{
   int i;

   if (Pv >0.0F)
      *Hangover=HANGOVER_TIME;
   else
      (*Hangover)--;

   if (*Hangover == HANGOVER_TIME-1)
      memcpy(AveLsf, CurrentLsf, LPC_ORDER*sizeof(float));

   if (*Hangover < HANGOVER_TIME)
   {
      for (i=0;i<LPC_ORDER;i++)
         AveLsf[i] = AveLsf[i]*(1.0F-AVE_FACTOR) + CurrentLsf[i]*AVE_FACTOR;
   }

   if (*Hangover < 0)
   {
      *Hangover = 0;
      memcpy(CurrentLsf, AveLsf, LPC_ORDER*sizeof(float));
   }

}
