/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:                                                                    
*                                                                              
* Purpose:                                                                     
*                                                                              
* Functions:                                                                   
*                                                                              
* Author/Date:                                                                 
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
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/utils/param.c_v   1.4   01 Dec 1998 14:38:02   zopf  $
*******************************************************************************/
#include <stdlib.h>
#include <assert.h>

#include "model.h"
#include "VoxMem.h"

#include "param.h"
#include "vem2.h"
#include "vem2Prm.h"

#include "xvocver.h"

/*******************************************************************************
* Function:       VOXErr VoxInitParam(PARAM* pParam)
*
* Action:
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
unsigned short VoxInitParam(void **hParam, void *pVemMem)
{
   PARAM *pParam;
   void  *pVemMemHandle;

   if(VOX_MEM_INIT(pParam=*hParam,1,sizeof(PARAM))) 
      return 1;

   if(VOX_MEM_INIT(pParam->Amp,MAXHARM,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(pParam->Lsf,LPC_ORDER,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(pParam->Alpc,LPC_ORDER+1,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(pParam->Blpc,LPC_ORDER,sizeof(float))) 
      return 1;
   
   if(pVemMem) {
      pVemMemHandle = ((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler;
      vemRegisterArray(pVemMemHandle, pParam->Amp, VEM_FLOAT, MAXHARM, "amplitudes");
      vemRegisterConstShort(pVemMemHandle, (short)LPC_ORDER, "lpc order");

	  /* This change is for VoiceFont backward compatibility - see Wei or John      */
      /* vemRegisterArray(pVemMemHandle, pParam->Alpc, VEM_FLOAT, LPC_ORDER, "lpc");*/

	  vemRegisterArray(pVemMemHandle, pParam->Blpc, VEM_FLOAT, LPC_ORDER, "lpc");

      vemRegisterVariable(pVemMemHandle, &pParam->Pv, VEM_FLOAT, "pv");
      vemRegisterVariable(pVemMemHandle, &pParam->Pitch, VEM_FLOAT, "pitch");
      vemRegisterVariable(pVemMemHandle, &pParam->Energy, VEM_FLOAT, "resEnergy");  
   }

   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxFreeParam(PARAM* pParam)
*
* Action:
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
unsigned short VoxFreeParam(void **pParam)
{
   if(*pParam) {
      VOX_MEM_FREE(((PARAM*)*pParam)->Amp);
      VOX_MEM_FREE(((PARAM*)*pParam)->Lsf);
      VOX_MEM_FREE(((PARAM*)*pParam)->Alpc);
	  VOX_MEM_FREE(((PARAM*)*pParam)->Blpc);
      VOX_MEM_FREE(*pParam);
   }
   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxInitPrevFrame(PrevFrameParam *PrevFrameParam_mblk)
*
* Action:
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
unsigned short VoxInitPrevFrame(void **hPrevFrameParam_mblk)
{
   PrevFrameParam *PrevFrameParam_mblk;
	short i;

   if(VOX_MEM_INIT(PrevFrameParam_mblk=*hPrevFrameParam_mblk,1,sizeof(PrevFrameParam))) 
      return 1;

   if(VOX_MEM_INIT(PrevFrameParam_mblk->Amp,MAXHARM,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(PrevFrameParam_mblk->Lsf,LPC_ORDER,sizeof(float))) 
      return 1;

   PrevFrameParam_mblk->Pitch = INITIAL_PITCH;
   PrevFrameParam_mblk->Pv=0.0F;

  for(i=0;i<LPC_ORDER;i++) 
	PrevFrameParam_mblk->Lsf[i] = LSP_INITIAL_COND_DEL*i + LSP_INITIAL_COND_0;


   PrevFrameParam_mblk->Energy = 0.0F;

   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxFreePrevFrame(PrevFrameParam *PrevFrameParam_mblk)
*
* Action:
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
unsigned short VoxFreePrevFrame(void **hPrevFrameParam_mblk)
{
   PrevFrameParam *PrevFrameParam_mblk=(PrevFrameParam *)*hPrevFrameParam_mblk;

   if(*hPrevFrameParam_mblk) {
      VOX_MEM_FREE(PrevFrameParam_mblk->Amp);
      VOX_MEM_FREE(PrevFrameParam_mblk->Lsf);

      VOX_MEM_FREE(*hPrevFrameParam_mblk);
   }

   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxInitSubframe(SubFrameParam *SubFrameParam_mblk)
*
* Action:
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
unsigned short VoxInitSubframe(void **hSubFrameParam_mblk)
{

   SubFrameParam *SubFrameParam_mblk;

   if(VOX_MEM_INIT(SubFrameParam_mblk=*hSubFrameParam_mblk,1,sizeof(SubFrameParam))) 
      return 1;

   if(VOX_MEM_INIT(SubFrameParam_mblk->Amp,MAXHARM,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(SubFrameParam_mblk->Lsf,LPC_ORDER,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(SubFrameParam_mblk->Amp_1,MAXHARM,sizeof(float)))
      return 1;

   if(VOX_MEM_INIT(SubFrameParam_mblk->Lsf_1,LPC_ORDER,sizeof(float))) 
      return 1;

   if(VOX_MEM_INIT(SubFrameParam_mblk->AveLsf,LPC_ORDER,sizeof(float))) 
      return 1;

   SubFrameParam_mblk->frameLEN = FRAME_LEN;
   SubFrameParam_mblk->Pitch_1= 50.0F; /*INITIAL_PITCH; //### MUST FIX INITIAL PITCH */
   SubFrameParam_mblk->nSubs = SYNSUBFRAMES;
   SubFrameParam_mblk->WarpFrame = 0;
   SubFrameParam_mblk->totalWarpFrame = 1;
   SubFrameParam_mblk->Hangover = HANGOVER_TIME;
   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxFreeSubframe(SubFrameParam *SubFrameParam_mblk)
*
* Action:
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
unsigned short VoxFreeSubframe(void **hSubFrameParam_mblk)
{
   SubFrameParam *SubFrameParam_mblk=(SubFrameParam *)*hSubFrameParam_mblk;

   if(*hSubFrameParam_mblk) {
      VOX_MEM_FREE(SubFrameParam_mblk->Amp);
      VOX_MEM_FREE(SubFrameParam_mblk->Lsf);
      VOX_MEM_FREE(SubFrameParam_mblk->Amp_1);
      VOX_MEM_FREE(SubFrameParam_mblk->Lsf_1);
      VOX_MEM_FREE(SubFrameParam_mblk->AveLsf);

      VOX_MEM_FREE(*hSubFrameParam_mblk);
   }
   return 0;
}



