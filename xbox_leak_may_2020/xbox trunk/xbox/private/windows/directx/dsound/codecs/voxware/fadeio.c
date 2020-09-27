/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename: FadeIO.c                                                                    
*                                                                              
* Purpose: Fade-In/ Fade-Out during frame loss conditions                                                                    
*                                                                              
* Functions:                                                                   
*                                                                              
* Author/Date: RZ/ 02-26-97                                                                
********************************************************************************
* Modifications:
*                   
* Comments:                                                                    
* 
* Concerns:
*
* 
*******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "model.h"
#include "VoxMem.h"
#include "param.h"
#include "GetnHarm.h"
#include "FadeIO.h"

#include "xvocver.h"

/*******************************************************************************
* Function:    void VoxFadeIO(void *pvFadeIOMemBlk, void *pvParamMemBlk)
*
* Action: Perform FadeIn/ FadeOut during frame loss
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
 
void VoxFadeIO(void *pvFadeIOMemBlk, void *pvParamMemBlk)
{
   PARAM *Param_mblk = (PARAM *)pvParamMemBlk;
   FadeIOstruct *FadeIO_mblk = (FadeIOstruct *)pvFadeIOMemBlk;

   short nHarm;
   int   i;
   float AmpLevel;
   float FadeFactor;

   /*====================================================================
     Check to see if there should be silence/background noise injection.
     ====================================================================*/
   if (FadeIO_mblk->CurrentFrameState == SILENCE)
   {
    /*===================================================================
      Set the energy, pitch, pv, amplitudes, and lsfs to either preset
      "noise" values, current noise estimates.
      ===================================================================*/
      Param_mblk->Energy = FadeIO_mblk->BackgroundEnergyEst;
      Param_mblk->Pitch  = SILENCE_PITCH;
      Param_mblk->Pv     = 0.0F;
      nHarm              = getnHarm(Param_mblk->Pitch);
      AmpLevel           = (float)sqrt(2.0F*Param_mblk->Energy/nHarm);

      for (i=0; i< nHarm; i++)
      {
         Param_mblk->Amp[i] = AmpLevel;
      }
      for (; i<MAXHARM; i++)
      {
         Param_mblk->Amp[i] = 0.0F;
      }

      for (i=0; i<LPC_ORDER; i++)
      {
         Param_mblk->Lsf[i] = FadeIO_mblk->BackgroundLsf[i];
      }
      for (i=1;i<LPC_ORDER;i++)
      {
         if (Param_mblk->Lsf[i]<(Param_mblk->Lsf[i-1]+LSP_SEPERATION_HZ))
         {
            Param_mblk->Lsf[i] = Param_mblk->Lsf[i-1]+LSP_SEPERATION_HZ;
         }
      }  
   }
 /*========================================================================
   If we are not in silence, and the voicing is zero, we must update our
   background noise estimates of energy and lsfs.
   ========================================================================*/
   else if (Param_mblk->Pv == 0.0F)
   {
      for (i=0; i<LPC_ORDER; i++)  /* running average of silence lsfs  */
      {
         FadeIO_mblk->BackgroundLsf[i] = (float)((1.0-RUNNING_AVE)*Param_mblk->Lsf[i] 
            +RUNNING_AVE*FadeIO_mblk->BackgroundLsf[i]);
      }
      if (Param_mblk->Energy < FadeIO_mblk->BackgroundEnergyEst)
      {
         FadeIO_mblk->BackgroundEnergyEst = Param_mblk->Energy;
      }
      else
      {
         FadeIO_mblk->BackgroundEnergyEst *= ENERGY_INCREASE;
      }
   }


 /*=========================================================================
   We want to fade in over a few frames.  Check to see if the codec must 
   fade in.  If it does, set the fade in length counting.
   =========================================================================*/
   if ((FadeIO_mblk->CurrentFrameState == NORMAL)
      &&(FadeIO_mblk->LastFrameState   == SILENCE))
   {
      FadeIO_mblk->FadeInCount = FADEIN_LEN;
   }

 /*=========================================================================
   Check if the decoder needs to fade in this frame
   =========================================================================*/
   if (FadeIO_mblk->FadeInCount > 0)
   {
      FadeIO_mblk->FadeInCount--;
      FadeFactor = (FADEIN_LEN-FadeIO_mblk->FadeInCount)*(1.0F/((float)FADEIN_LEN));
      for (i=0;i<MAXHARM;i++)
      {
         Param_mblk->Amp[i] *= FadeFactor;
      }
   }
  
   FadeIO_mblk->LastFrameState    = FadeIO_mblk->CurrentFrameState;
   FadeIO_mblk->CurrentFrameState = NORMAL;
}


/*******************************************************************************
* Function:       VOXErr VoxinitFadeIO(void **hFadeIOMemBlk)
*
* Action:         Initializes the LPCSyn struct
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
unsigned short VoxinitFadeIO(void **hFadeIOMemBlk)
{
   FadeIOstruct *FadeIO_mblk;
   short i;

   if(VOX_MEM_INIT(FadeIO_mblk=*hFadeIOMemBlk,1,sizeof(FadeIOstruct))) 
      return 1;

   if(VOX_MEM_INIT(FadeIO_mblk->BackgroundLsf,LPC_ORDER,sizeof(float))) 
      return 1;

  for(i=0;i<LPC_ORDER;i++) 
	FadeIO_mblk->BackgroundLsf[i] = LSP_INITIAL_COND_DEL*i + LSP_INITIAL_COND_0;

   FadeIO_mblk->FadeInCount         = 0; 
   FadeIO_mblk->CurrentFrameState   = NORMAL;
   FadeIO_mblk->LastFrameState      = NORMAL;
   FadeIO_mblk->BackgroundEnergyEst = INITIAL_BACKGROUND_ENERGY;


   return 0;
}


/*******************************************************************************
* Function:       VOXErr VoxfreeFadeIO(void **hFadeIOMemBlk)
*
* Action:         frees up the LPCSyn struct
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
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short VoxfreeFadeIO(void **hFadeIOMemBlk)
{
   FadeIOstruct *FadeIO_mblk=(FadeIOstruct *)*hFadeIOMemBlk;

   if(*hFadeIOMemBlk) {
      VOX_MEM_FREE(FadeIO_mblk->BackgroundLsf);
      VOX_MEM_FREE(*hFadeIOMemBlk);
   }
   return 0;
}
