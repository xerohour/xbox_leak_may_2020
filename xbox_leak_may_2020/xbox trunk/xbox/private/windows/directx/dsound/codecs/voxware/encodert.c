/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       EncodeRT.c
*
* Purpose:        Encode function which will analysis input speech 
*                 frame by frame and then store speech model parameters
*
* Functions:      VOXErr VoxEncode(short *pPCMBuffer, void *pMemBlkE);
*                 VOXErr VoxInitEncode(void **ppMemBlkE);
*                 VOXErr VoxFreeEncode(void *pMemBlkE);
*
* Author/Date:
*                 Modified by Rob Zopf, Rewritten by Wei Wang, 2/96
********************************************************************************
* Modifications:                                                               
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/encode/EncodeRT.c_v   1.13   04 Dec 1998 16:08:48   weiwang  $
*******************************************************************************/
#include <stdio.h>

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "VoxMem.h"
#include "model.h"
#include "vem2.h"
#include "vem2Prm.h"

#include "LPC.h"
#include "param.h"

#include "vLibSgPr.h"  /* InitKaiserWindow(), FreeKaiserWindow(), 
                          symWindowSignal(), NormSymWindowByPower() */
#include "vLibSnd.h"   /* InitSpCBM(), FreeSpCBM(), PutFrameSpCBM(),
                          GetFrameSpCBM() */
#include "vLibVec.h"   /* MagSq() */
#include "vLibTran.h"  /* VoxFftCenter(), VoxDemultiplex() */
#include "vLibFilt.h"  /* VoxInitPreProc(), VoxPreProcess(), VoxFreePreProc() */

#include "CorrPDA.h"
#include "VoicDeci.h"
#include "Refine.h"
#include "VAD2.h"
#include "EncodeRT.h"

#include "xvocver.h"

static short VoxGetWinLength(float pitch);

/*******************************************************************************
* Function:       VOXErr VoxEncode(short *pPCMBuffer, void *pMemBlkE)
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*                 Put correlator PDA with TIME_DOMAIN_PDA == 1
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxEncodeRT(STACK_R short *pPCMBuffer, void *hEncMemBlk)
{
   ENC_MEM_BLK *pEncMemBlk= (ENC_MEM_BLK *)hEncMemBlk;

   PARAM *Param_mblk = (PARAM *)pEncMemBlk->Param_mblk;
   
   STACK_INIT
   float SpBuf[FRAME_SIZE3];
   STACK_INIT_END
   float *SWR = pEncMemBlk->SWR;
   float *SWI = pEncMemBlk->SWI;
   short  UV;

   STACK_ATTACH(float *, SpBuf)

   float *win;        /* window pointer */
   float pitch, Pv, pdaDelaySize;
   int i;
   STACK_START
   

  /* pre-processing the speech using HPF */
  VoxPreProcess(pPCMBuffer, SpBuf, FRAME_LEN, pEncMemBlk->PreProc_mblk);

  /* put 8k speech to the circular buffer */
  PutFrameSpCBM(pEncMemBlk->CircBuf_mblk, 0, (const int) FRAME_LEN, SpBuf);
  
  /* pitch detection */
  pitch = VoxCorrPDA(STACK_S SpBuf, FRAME_LEN, pEncMemBlk->PDA_mblk,
                     &pdaDelaySize, &(Param_mblk->VAD),&UV);

  /***** decide the length and window by the pitch of the current frame *****/
  pEncMemBlk->Nwin = VoxGetWinLength(pitch);

  /**** take an frame of speech out and apply windowing ****/
  GetFrameSpCBM(pEncMemBlk->CircBuf_mblk, (const int) pdaDelaySize, 
                (const int) pEncMemBlk->Nwin, SpBuf);

  switch(pEncMemBlk->Nwin) {
  case FRAME_SIZE1:
      win=pEncMemBlk->win1;
      break;
   case FRAME_SIZE2:
      win=pEncMemBlk->win2;
      break;
   case FRAME_SIZE3:
      win=pEncMemBlk->win3;
      break;
   }
  
  /**** window signal from high end to low end ****/
  SymWindowSignal(SpBuf, win, SpBuf, pEncMemBlk->Nwin);
  
  /***************************************************/
  /*     1. calculate LPC                            */
  /***************************************************/

  VoxCalcLPC(STACK_S SpBuf, pEncMemBlk->Nwin, Param_mblk->Alpc, &Param_mblk->Energy,
	     (float)EXPAND_COEF,pEncMemBlk->dwLpc);


   /*****************************************************/
   /*    2. calculate pitch, pv and number of harmonics */
   /*****************************************************/
  {  
   STACK_INIT
   float fSW[NFFT+2];
   STACK_INIT_END
   STACK_ATTACH(float*,fSW)
   STACK_START

   /***** calculate the spectrum *****/

   VoxFftCenter( SpBuf, pEncMemBlk->Nwin, fSW, FFT_ORDER );
   VoxDemultiplex( fSW, SWR, SWI, NFFT_2 );

   STACK_END
  }

  {
   STACK_INIT
   float power[NFFT_2];
   float BandError[MAXHARM];
   STACK_INIT_END
   STACK_ATTACH(float*,power)
   STACK_ATTACH(float*,BandError)

   STACK_START

   /**** calculate the power structrum of the signal ****/
   MagSq( SWR, SWI, NFFT_2, power);
   
   VoxPitchRefine_TIME(&pitch, SWR, SWI, power, pEncMemBlk->Nwin, BandError);  

   /* find the voicing probability */
   VoxVocDecision(pitch, pEncMemBlk->Voicing_mblk, &Pv, power, BandError, pEncMemBlk->Nwin, &(Param_mblk->VAD));
   Pv = (float) PvRefine( Pv, pEncMemBlk->Pv_1, (float) ((CorrPDA_mblk *)pEncMemBlk->PDA_mblk)->pitch_1, 
                 ((CorrPDA_mblk *)pEncMemBlk->PDA_mblk)->cost_1, 
                 ((CorrPDA_mblk *)pEncMemBlk->PDA_mblk)->biasTab);

   if(UV!=ACTIVE_SPEECH)
      Pv=0;
   pEncMemBlk->Pv_1 = Pv;

   STACK_END
  }

  /*****************************************************/
  /*    3.  pack pitch, pv                             */
  /*****************************************************/
  if (Pv == 0)
    pitch = (float)PITCH_UV;

  Param_mblk->Pitch = pitch;
  Param_mblk->Pv = Pv;

  pEncMemBlk->uQPitch = pitch;

  /*****************************************************/
  /*    4. Perform backward compatibility operations   */
  /*          (talk to Wei or John)                    */
  /*****************************************************/
  
  for (i = 0; i < LPC_ORDER; i++)
       Param_mblk->Blpc[i] = -(Param_mblk->Alpc[i+1]);

  /* service the "post encode" state */
  vemServiceState(pEncMemBlk->vemKeyPostEncode);

  STACK_END
  return 0;
} /* Encode() */


/*******************************************************************************
* Function:       VOXErr VoxInitEncode(void **ppMemBlkE, void *pVemMem))
*
* Action:         Initializes encoder structure
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
*               Add initialization of CorrPDA_mblk.  W.W. 11/6/96.
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

unsigned short VoxInitEncodeRT(void **ppMemBlkE, void *pVemMem)
{

  ENC_MEM_BLK *pEncode_mblk;

  if(VOX_MEM_INIT(pEncode_mblk=*ppMemBlkE,1,sizeof(ENC_MEM_BLK))) 
      return 1;

  /****************************************************
  initialize circular buffer structure 
  ****************************************************/
  /* initialize the circular buffer manager */
  if (InitSpCBM((const int) CIRCBUF_LENGTH, &pEncode_mblk->CircBuf_mblk))
      return 1;
  
  /* initialize the pre-process structure */
  if (VoxInitPreProc(&pEncode_mblk->PreProc_mblk, PRE_PROC_COEF))
      return 1;

  /****************************************************
  initialize Voicing structure
  ****************************************************/
  if(VoxInitVoicing(&pEncode_mblk->Voicing_mblk)) 
      return 1;

  /****************************************************
   initialize correlation PDA structure
   ****************************************************/
  if(VoxInitCorrPitchEst(&pEncode_mblk->PDA_mblk)) 
      return 1;

  /****************************************************
  initialize FFT coefficients 
  ****************************************************/

  /*-------------------------------------------------------
    LOOK OUT!!! WHOA!!! NOW HOLD EVERYTHING!  This should
      not be frame to frame memory, but....these values
      are used in ResidAmp.c! That needs to be changed.
  -------------------------------------------------------*/
  if(VOX_MEM_INIT(pEncode_mblk->SWR,NFFT_2,sizeof(float))) 
     return 1;

  if(VOX_MEM_INIT(pEncode_mblk->SWI,NFFT_2,sizeof(float))) 
     return 1;

 
  /****************************************************
  initialize the kaiser window 
  initialize only for API codec. For DSP application, read from table
  ****************************************************/
  if(InitKaiserWindow(&pEncode_mblk->win1, FRAME_SIZE1, 6.0F))
     return 1;
  NormSymWindowByPower(pEncode_mblk->win1, FRAME_SIZE1);


  if(InitKaiserWindow(&pEncode_mblk->win2, FRAME_SIZE2, 6.0F))
     return 1;
  NormSymWindowByPower(pEncode_mblk->win2, FRAME_SIZE2);

  if(InitKaiserWindow(&pEncode_mblk->win3, FRAME_SIZE3, 6.0F))
     return 1;
  NormSymWindowByPower(pEncode_mblk->win3, FRAME_SIZE3);

  /****************************************************
  initialize parameter structure
  ****************************************************/
  if(VoxInitParam(&pEncode_mblk->Param_mblk, pVemMem)) 
      return 1;

  if(VOX_MEM_INIT(pEncode_mblk->dwLpc,LPC_ORDER+1,sizeof(double))) 
     return 1;

  /****************************************************
  attaching the Vem memory
  ****************************************************/
  pEncode_mblk->pVemMem=pVemMem;
  if(!(pEncode_mblk->vemKeyPostEncode=vemRegisterState(
     ((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, "post encode"))) 
  {
#if VEM_DEFINED == 1
     return 1;
#endif
  }

  if(!(pEncode_mblk->vemKeyPostCalcResAmps=vemRegisterState(
     ((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, "post calc res amps rt")))
  {
#if VEM_DEFINED == 1
     return 1;
#endif
  }

  vemRegisterConstShort(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, (short)RATE8K, "rate");
  vemRegisterConstShort(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, (short)MAXHARM, "max harmonics");
  vemRegisterConstShort(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, (short)FRAME_LEN, "frame length");
  vemRegisterVariable(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, 
     pEncode_mblk->dwLpc, VEM_FLOAT, "energy");

  
  return 0; 
}

/*******************************************************************************
* Function:       VOXErr VoxFreeEncode(void *pMemBlkE)
*
* Action:         Free's encoder structure
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
*                Add free CorrPDA_mblk.  W.W. 11/6/96.
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxFreeEncodeRT(void **hMemBlkE)
{
  ENC_MEM_BLK *pMemBlkE=(ENC_MEM_BLK *)*hMemBlkE;

  if (*hMemBlkE) {

     /**** free circular structure ****/
     FreeSpCBM(&pMemBlkE->CircBuf_mblk);

     /**** free preprocess structure ****/
     VoxFreePreProc(&pMemBlkE->PreProc_mblk); 

     /**** free Voicing structure ****/
     VoxFreeVoicing(&pMemBlkE->Voicing_mblk);

     /**** free CorrPDA structure ****/
     VoxFreeCorrPitchEst(&pMemBlkE->PDA_mblk);

     /**** free Param structure ****/
     VoxFreeParam(&pMemBlkE->Param_mblk);

     /**** free the kaiser windows ****/
     FreeKaiserWindow(&pMemBlkE->win1);
     FreeKaiserWindow(&pMemBlkE->win2);
     FreeKaiserWindow(&pMemBlkE->win3);

     VOX_MEM_FREE(pMemBlkE->SWR);    
     VOX_MEM_FREE(pMemBlkE->SWI);

     VOX_MEM_FREE(pMemBlkE->dwLpc);
     
#ifdef WIN32
     nspFreeFftResources();
#endif

     VOX_MEM_FREE(*hMemBlkE);
  }
  return 0;
}

static short VoxGetWinLength(float pitch)
{
  if(pitch> PITCH4FRAME_SIZE3) {   /* 60 & 261 window */
      return FRAME_SIZE3;
   }
   else {                                  
     if (pitch > PITCH4FRAME_SIZE2) { /* 43 & 241 window */
         return FRAME_SIZE2;
      }
     else {
       return FRAME_SIZE1;
     }
   }
}

