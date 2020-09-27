/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:   Quan12.c                                                                  
*                                                                              
* Purpose:    Quan/Dequan functions for VR12                                                                  
*                                                                               
* Functions:  VoxQuanVR12(), VoxUnQuanVR12(), VoxInitQuanVR12(), VoxFreeQuanVR12() 
*                                                                              
* Author/Date:                                                                 
********************************************************************************
* Modifications:
*                   
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   P:/r_and_d/archives/realtime/quan/Quan12.c_v   1.5   01 Dec 1998 12:56:38   zopf  $
*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "model.h"
#include "research.h"
#include "vem2.h"
#include "vem2Prm.h"

#include "quanvr12.h" 
#include "VoxMem.h"
#include "vLibTran.h" /* VoxAsToLspEven() */
#include "vLibMath.h"

#include "param.h"

#include "QTable12.h"
#include "QEnrgy12.h"
#include "QPitch12.h"
#include "QPvVR12.h"
#include "LspMSVQ.h"
#include "GetnHarm.h"
#include "Quan12.h"

#include "xvocver.h"

/*******************************************************************************
* Function:       VOXErr VoxInitQuanVR12(void *QUAN_mblk)
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
unsigned short VoxQuanVR12(void *pvQuanMemBlk, void *pvParamMemBlk) 
{
   PARAM *Param_mblk = (PARAM *)pvParamMemBlk;
   QUAN_12 *Quan_mblk = (QUAN_12*)pvQuanMemBlk;

   /*************************************************
              Set the Class 
    *************************************************/
   if (Param_mblk->VAD == 0)
      Quan_mblk->Class = SILENCE_FRAME;
   else if (Param_mblk->Pv<=PV_RESET)
      Quan_mblk->Class = UV_FRAME;   
   else if (Param_mblk->Pv<PV_V_BOUND)
      Quan_mblk->Class = MIX_FRAME;
   else 
      Quan_mblk->Class = V_FRAME;

   if (Quan_mblk->Class==UV_FRAME)
   {

      /***** convert lpc to lsp *****/
      VoxAsToLspEven(Param_mblk->Alpc, LPC_ORDER, RATE8K, Param_mblk->Lsf);

      VoxLspMSVQ (Param_mblk->Lsf, Param_mblk->Alpc+1, Quan_mblk->Lsf_INDEX, Quan_mblk->MSVQ_struct,
                  NUMSTAGES_LSPVQ_VR12_UNVOICED, NUMCAND_LSPVQ_VR12, LPC_ORDER, TABLESIZE_LSPVQ_VR12,
                  Quan_mblk->Lsf_MSVQ_TABLE);

      VoxQuanEnergyVR12(Param_mblk->Energy, Quan_mblk->Energy_TABLE_UnVoiced, 
                   Quan_mblk->Energy_SIZE_UnVoiced, &(Quan_mblk->Energy_INDEX) );

   }

   if (Quan_mblk->Class==MIX_FRAME)
   {

      Quan_mblk->Pitch_INDEX = VoxQuanPitchVR12(Param_mblk->Pitch);

      /***** convert lpc to lsp *****/
      VoxAsToLspEven(Param_mblk->Alpc, LPC_ORDER, RATE8K, Param_mblk->Lsf);

      VoxLspMSVQ (Param_mblk->Lsf, Param_mblk->Alpc+1, Quan_mblk->Lsf_INDEX, Quan_mblk->MSVQ_struct,
                  NUMSTAGES_LSPVQ_VR12_MIXED, NUMCAND_LSPVQ_VR12, LPC_ORDER, TABLESIZE_LSPVQ_VR12,
                  Quan_mblk->Lsf_MSVQ_TABLE);

      Quan_mblk->Pv_INDEX = VoxQuanPvVR12(Param_mblk->Pv);

      VoxQuanEnergyVR12(Param_mblk->Energy, Quan_mblk->Energy_TABLE_Voiced, 
                   Quan_mblk->Energy_SIZE_Voiced, &(Quan_mblk->Energy_INDEX) );
   }

   if (Quan_mblk->Class==V_FRAME)
   {

      Quan_mblk->Pitch_INDEX = VoxQuanPitchVR12(Param_mblk->Pitch);

      /***** convert lpc to lsp *****/
      VoxAsToLspEven(Param_mblk->Alpc, LPC_ORDER, RATE8K, Param_mblk->Lsf);

      VoxLspMSVQ (Param_mblk->Lsf, Param_mblk->Alpc+1, Quan_mblk->Lsf_INDEX, Quan_mblk->MSVQ_struct,
                  NUMSTAGES_LSPVQ_VR12_VOICED, NUMCAND_LSPVQ_VR12, LPC_ORDER, TABLESIZE_LSPVQ_VR12,
                  Quan_mblk->Lsf_MSVQ_TABLE);

      VoxQuanEnergyVR12(Param_mblk->Energy, Quan_mblk->Energy_TABLE_Voiced, 
                   Quan_mblk->Energy_SIZE_Voiced, &(Quan_mblk->Energy_INDEX) );
   }

   vemServiceState(Quan_mblk->vemKeyPostQuan);
 
   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxUnQuanVR12(void *pvDecMemBlk)
*
* Action:  Unquantize the frame parameters for VR12.
*
* Input:  *pvQuanMemBlk - the quantization memory block
*
* Output: *pvParamMemBlk - the Param block filled in with the unquantized values.
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications: 10/27/98 - background noise estimation improvements - RZ
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
#define  BCKGRND_RND_HZ       50.0F   /* maximum amount to randomize the lsfs */
#define  BCKGRND_MULT_FACTOR  1.01F   /* increase noise estimate              */
#define  BCKGRND_ADD_FACTOR   5.0F    /* increase noise estimate              */
#define  BCKGRND_FADE_FACTOR  0.99F   /* fade to silence during bckgrnd noise */
#define  INIT_CLASS           999     /* used to avoid first frame energy     */
#define  LONGTERM_AVE         0.6F    /* running average of lsfs              */

unsigned short VoxUnQuanVR12(void *pvQuanMemBlk, void *pvParamMemBlk)
{
   PARAM *Param_mblk  = (PARAM *)pvParamMemBlk;
   QUAN_12 *Quan_mblk = (QUAN_12 *)pvQuanMemBlk;
   short   nHarm;
   float   scale;
   int     i;

   if (Quan_mblk->Class==SILENCE_FRAME)
   {
      Param_mblk->Pv    = 0.0F;
      Param_mblk->Pitch = (float)PITCH_UV;

      if (Quan_mblk->PrevClass!=SILENCE_FRAME)
      {
         /**************************************************************************/
         /* Calculate a running average of the lsfs, and the energy for use        */
         /* during the voice inactive frames ...                                   */
         /**************************************************************************/
         for (i=0;i<LPC_ORDER;i++)
         {
            Quan_mblk->UVLsf[i] = LONGTERM_AVE*Quan_mblk->UVLsf[i] + 
                                 (1.0F-LONGTERM_AVE)*Quan_mblk->PrevLsf[i];
         }
      }

      Param_mblk->Energy = Quan_mblk->BckGrndEnergyEst;

      /**********************************************************************
       Add a random component to the background noise fill spectrum so that
       we don't repeat the exact same spectrum over and over again.
       **********************************************************************/
      for (i=0;i<LPC_ORDER;i++)
         Param_mblk->Lsf[i] = Quan_mblk->UVLsf[i] +
                              (((NuRand(&(Quan_mblk->Seed))*NURAND_SCALE)-0.5F)*2.0F)*BCKGRND_RND_HZ;

      /**********************************************************************
                             Stability check
       **********************************************************************/
      for (i=1;i<LPC_ORDER;i++)
         if (Param_mblk->Lsf[i] < Param_mblk->Lsf[i-1]+LSP_SEPERATION_HZ)
             Param_mblk->Lsf[i] = Param_mblk->Lsf[i-1]+LSP_SEPERATION_HZ;
   } /* end SILENCE_FRAME */

   if (Quan_mblk->Class==UV_FRAME)
   {
      Param_mblk->Pitch = (float)PITCH_UV;
      Param_mblk->Pv    = 0.0F;
      Param_mblk->Energy= VoxUnQuanEnergyVR12(Quan_mblk->Energy_TABLE_UnVoiced,  
                          Quan_mblk->Energy_SIZE_UnVoiced, Quan_mblk->Energy_INDEX);
      VoxDecLspMSVQ (Quan_mblk->Lsf_INDEX, Param_mblk->Lsf, Quan_mblk->Lsf_MSVQ_TABLE, 
                  LPC_ORDER, NUMSTAGES_LSPVQ_VR12_UNVOICED);
   }

   if (Quan_mblk->Class==MIX_FRAME)
   {
      /***** Unquantize Pitch *****/
      Param_mblk->Pitch = VoxLutPitchVR12(Quan_mblk->Pitch_INDEX);

      /***** Unquantize Pv    *****/
      Param_mblk->Pv = VoxUnQuanPvVR12( Quan_mblk->Pv_INDEX );

      /***** Unquantize Energy ****/
      Param_mblk->Energy= VoxUnQuanEnergyVR12(Quan_mblk->Energy_TABLE_Voiced,  Quan_mblk->Energy_SIZE_Voiced,
                                         Quan_mblk->Energy_INDEX);

      VoxDecLspMSVQ (Quan_mblk->Lsf_INDEX, Param_mblk->Lsf, Quan_mblk->Lsf_MSVQ_TABLE, 
                  LPC_ORDER, NUMSTAGES_LSPVQ_VR12_MIXED);
   }

   if (Quan_mblk->Class==V_FRAME)
   {
      /***** Unquantize Pitch *****/
      Param_mblk->Pitch = VoxLutPitchVR12(Quan_mblk->Pitch_INDEX);

      /***** Unquantize Pv    *****/
      Param_mblk->Pv = (float)PV_V;

      /***** Unquantize Energy ****/
      Param_mblk->Energy= VoxUnQuanEnergyVR12(Quan_mblk->Energy_TABLE_Voiced,  Quan_mblk->Energy_SIZE_Voiced,
                                         Quan_mblk->Energy_INDEX);

      VoxDecLspMSVQ (Quan_mblk->Lsf_INDEX, Param_mblk->Lsf, Quan_mblk->Lsf_MSVQ_TABLE, 
                  LPC_ORDER, NUMSTAGES_LSPVQ_VR12_MIXED);
   }

   /*******************************************************************
    Calculate the harmonic amplitudes
    *******************************************************************/
   nHarm = getnHarm(Param_mblk->Pitch);
   scale = (float)sqrt(2.0*Param_mblk->Energy/nHarm);
   for (i=0; i< nHarm; i++)
      Param_mblk->Amp[i] = scale;
   for (i=nHarm; i< MAXHARM; i++)
      Param_mblk->Amp[i] = 0.0F; 
 
   /*******************************************************************
    Update the background noise estimate.  First, fade to silence 
    during background noise.  Second, update the background noise 
    estimate based on the frame energy and our current estimate
    *******************************************************************/
   if (Quan_mblk->Class==SILENCE_FRAME)
   {
      Quan_mblk->BckGrndEnergyEst *= BCKGRND_FADE_FACTOR;
   }
   else
   {
      Quan_mblk->BckGrndEnergyEst = (Quan_mblk->BckGrndEnergyEst*BCKGRND_MULT_FACTOR) + BCKGRND_ADD_FACTOR;
      if (Quan_mblk->PrevClass!= INIT_CLASS)  /* first frame is always low energy due to zero-padding */
         if (Param_mblk->Energy < Quan_mblk->BckGrndEnergyEst)
            Quan_mblk->BckGrndEnergyEst = Param_mblk->Energy;
   }

   /**********************************************************************
    Update our memory 
    **********************************************************************/
   Quan_mblk->PrevClass = Quan_mblk->Class;
   memcpy(Quan_mblk->PrevLsf, Param_mblk->Lsf, sizeof(float)*LPC_ORDER);

   vemServiceState(Quan_mblk->vemKeyPostUnQuan);
    
   return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxInitQuanVR12(QUAN_12* Quan_mblk)
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

unsigned short VoxInitQuanVR12(void **hQuan_mblk, void *pVemMem)
{
  QUAN_12 *Quan_mblk;

  if(VOX_MEM_INIT(Quan_mblk=*hQuan_mblk,1,sizeof(QUAN_12))) 
     return 1;

  /**** initialize Pv table ****/
  Quan_mblk->Pv_INDEX = 0;
  Quan_mblk->Pv_SIZE = (1<<PV_BITS_VR12_MIXED); 
  Quan_mblk->Pv_TABLE = voxQuanGetPvTableVR12();

  /**** initialize pitch table ****/
  Quan_mblk->Pitch_INDEX = 0;
  Quan_mblk->Pitch_SIZE = (1<<F0_BITS_VR12_VOICED);
  Quan_mblk->Pitch_TABLE = NULL;    /* calculate in the function */

  /***** initialize Energy table *****/
  Quan_mblk->Energy_INDEX = 0;
  Quan_mblk->Energy_SIZE_Voiced = ((short)1<<GAIN_BITS_VR12_VOICED);
  Quan_mblk->Energy_TABLE_Voiced = voxQuanGetEnergyTableVR12_Voiced();
  Quan_mblk->Energy_SIZE_UnVoiced = ((short)1<<GAIN_BITS_VR12_UNVOICED);
  Quan_mblk->Energy_TABLE_UnVoiced = voxQuanGetEnergyTableVR12_UnVoiced();


  if(VOX_MEM_INIT(Quan_mblk->Lsf_INDEX, NUMSTAGES_LSPVQ_VR12_VOICED,sizeof(unsigned short))) 
     return 1;

  if(voxQuanFillLsfMSVQTableVR12(&Quan_mblk->Lsf_MSVQ_TABLE)) 
      return 1;
  
  if (VoxInitMSVQ(&(Quan_mblk->MSVQ_struct)))
     return 1;

  if(VOX_MEM_INIT(Quan_mblk->PrevLsf, LPC_ORDER,sizeof(float))) 
     return 1;

  if(VOX_MEM_INIT(Quan_mblk->UVLsf, LPC_ORDER,sizeof(float))) 
     return 1;

  Quan_mblk->PrevLsf[0] = INITIAL_LSF_0;
  Quan_mblk->PrevLsf[1] = INITIAL_LSF_1;
  Quan_mblk->PrevLsf[2] = INITIAL_LSF_2;
  Quan_mblk->PrevLsf[3] = INITIAL_LSF_3;
  Quan_mblk->PrevLsf[4] = INITIAL_LSF_4;
  Quan_mblk->PrevLsf[5] = INITIAL_LSF_5;
  Quan_mblk->PrevLsf[6] = INITIAL_LSF_6;
  Quan_mblk->PrevLsf[7] = INITIAL_LSF_7;
  Quan_mblk->PrevLsf[8] = INITIAL_LSF_8;
  Quan_mblk->PrevLsf[9] = INITIAL_LSF_9;

  Quan_mblk->UVLsf[0] = INITIAL_LSF_0;
  Quan_mblk->UVLsf[1] = INITIAL_LSF_1;
  Quan_mblk->UVLsf[2] = INITIAL_LSF_2;
  Quan_mblk->UVLsf[3] = INITIAL_LSF_3;
  Quan_mblk->UVLsf[4] = INITIAL_LSF_4;
  Quan_mblk->UVLsf[5] = INITIAL_LSF_5;
  Quan_mblk->UVLsf[6] = INITIAL_LSF_6;
  Quan_mblk->UVLsf[7] = INITIAL_LSF_7;
  Quan_mblk->UVLsf[8] = INITIAL_LSF_8;
  Quan_mblk->UVLsf[9] = INITIAL_LSF_9;

  Quan_mblk->PrevClass        = INIT_CLASS;
  Quan_mblk->Seed             = 55;   /* some non-zero number */
  Quan_mblk->BckGrndEnergyEst = 1000000.0F;  /* some large number */  

  Quan_mblk->pVemMem=pVemMem;
  if(!(Quan_mblk->vemKeyPostQuan=vemRegisterState(
     ((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler,"post quan")))
  {
#if VEM_DEFINED == 1
     return 1;
#endif
  }

  if(!(Quan_mblk->vemKeyPostUnQuan=vemRegisterState(
     ((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler,"post unquan")))
  {
#if VEM_DEFINED == 1
     return 1;
#endif
  }
  vemRegisterConstShort(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, 
     (short)QUANT_MAX_PITCH_VR, "max pitch");

  vemRegisterConstShort(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, 
     (short)QUANT_MIN_PITCH_VR, "min pitch");

  return 0;
}

/*******************************************************************************
* Function:       VOXErr VoxFreeQuanVR12(void *QUAN_mblk)
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
unsigned short VoxFreeQuanVR12(void **hQuan_mblk)
{
  QUAN_12* pQuan=(QUAN_12 *)*hQuan_mblk;
  if(*hQuan_mblk) {
     /***** free LSF Table *****/
     VOX_MEM_FREE(pQuan->Lsf_INDEX);
     VOX_MEM_FREE(pQuan->PrevLsf);
     VOX_MEM_FREE(pQuan->UVLsf);
     VOX_MEM_FREE_CONST(pQuan->Lsf_MSVQ_TABLE);  
     
     VoxFreeMSVQ(&(pQuan->MSVQ_struct));

     VOX_MEM_FREE(*hQuan_mblk);
  }

  return 0;
}
