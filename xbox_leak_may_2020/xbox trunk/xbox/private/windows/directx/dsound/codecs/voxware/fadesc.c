/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename: FadeSC.c
*                                                                              
* Purpose: Fade-In/ Fade-Out during frame loss conditions 
*                                                                              
* Functions: VoxFadeSC,  VoxinitFadeIO,  VoxfreeFadeIO,  
*         SetCurrentFrameState, GetCurrentFrameState, GetLastFrameState
*
* Author/Date: RZ/ 02-26-97
********************************************************************************
* Modifications: 07/30/97 RZ Adapted for SC codecs.
*                02/10/98 Wei Wang, cleaned up code
* Comments: 
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/FadeSC.c_v   1.6   13 Apr 1998 16:17:56   weiwang  $
* 
*******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "codec.h"
#include "VoxMem.h"
#include "paramSC.h"
#include "FadeSC.h"

#include "xvocver.h"

/*----------------------------------------------------------------------
  Definitions 
  ------------------------------------------------------------*/
#define SILENCE_PITCH              125.0F
#define RUNNING_AVE                0.97F
#define ENERGY_INCREASE            0.014355 /* log2(1.01F) */
#define FADEIN_LEN                 1 /* 3*/
#define INV_FADEIN_LEN             1.0F
#define INITIAL_BACKGROUND_ENERGY  0.0F; 


/*-----------------------------------------------------------------------------
  This is an array of initial values of arcsin reflection coefficients
-----------------------------------------------------------------------------*/
static const float fASKinit[LPC_ORDER] = {
-0.1329334F, 0.05232841F, -0.1199983F, -0.08337834F,
-0.1486588F, -0.1050886F, -0.08690762F, 0.0473017F,
-0.05771337F, -0.08524013F, 0.04383065F, 0.02077087F
};

/*------------------------------------------------------------
  Structure
  ------------------------------------------------------------*/
typedef struct tagFadeSC
{
  int CurrentFrameState;
  int LastFrameState;
  float  BackgroundEnergyEst;
  float  *BackgroundASK;
  int  FadeInCount;
} FadeSCstruct;


/*******************************************************************************
* Function:    void VoxFadeSC(void *pvFadeIOMemBlk, void *pvParamMemBlk)
*
* Action:      Perform FadeIn/ FadeOut during frame loss
*
* Input:  pvFadeSCMemBlk -- pointer to FadeSC memory block
*         pvParamMemBlk --  pointer to parameter memory block
* 
* Output: pvFadeSCMemBlk -- updated FadeSC memory block
*         pvParamMemBlk --  updated parameter memory block
*
* Globals:  none
*
* Return:   none
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
 
void VoxFadeSC(void *pvFadeSCMemBlk, void *pvParamMemBlk)
{
   PARAM_SC *Param_mblk = (PARAM_SC *)pvParamMemBlk;
   FadeSCstruct *FadeSC_mblk = (FadeSCstruct *)pvFadeSCMemBlk;
   SUBFRAME_PARAM_SC *pSubParamMid = &(Param_mblk->SubParam[PARAM_MIDFRAME]);
   SUBFRAME_PARAM_SC *pSubParamEnd = &(Param_mblk->SubParam[PARAM_ENDFRAME]);

   float FadeFactor;

   /*====================================================================
     Check to see if there should be silence/background noise injection.
     ====================================================================*/
   if (FadeSC_mblk->CurrentFrameState == SILENCE)
   {
    /*===================================================================
      Set the energy, pitch, pv, nad spectrum to either preset
      "noise" values, current noise estimates.
      ===================================================================*/
      Param_mblk->fLog2Gain     = FadeSC_mblk->BackgroundEnergyEst;
      pSubParamMid->Pitch = pSubParamEnd->Pitch = SILENCE_PITCH;
      pSubParamMid->Pv = pSubParamEnd->Pv = 0.0F;

      memcpy(Param_mblk->pfASK, FadeSC_mblk->BackgroundASK, 
             LPC_ORDER*sizeof(float));
   }
 /*========================================================================
   If we are not in silence, and the voicing is zero, we must update our
   background noise estimates of energy and ASKs.
   ========================================================================*/
   else if ((pSubParamMid->Pv == 0.0F)&&(pSubParamEnd->Pv == 0.0F))
   {
     memcpy(FadeSC_mblk->BackgroundASK, Param_mblk->pfASK, 
            LPC_ORDER*sizeof(float));

     if (Param_mblk->fLog2Gain < FadeSC_mblk->BackgroundEnergyEst)
       FadeSC_mblk->BackgroundEnergyEst = Param_mblk->fLog2Gain;
     else
       FadeSC_mblk->BackgroundEnergyEst += (float)ENERGY_INCREASE;
   }


 /*=========================================================================
   We want to fade in over a few frames.  Check to see if the codec must 
   fade in.  If it does, set the fade in length counting.
   =========================================================================*/
   if ((FadeSC_mblk->CurrentFrameState == NORMAL)
      &&(FadeSC_mblk->LastFrameState   == SILENCE))
   {
     FadeSC_mblk->FadeInCount = FADEIN_LEN;
   }

 /*=========================================================================
   Check if the decoder needs to fade in this frame
   =========================================================================*/
   if (FadeSC_mblk->FadeInCount > 0)
   {
      FadeSC_mblk->FadeInCount--;
      FadeFactor = 1.0F - (float)FadeSC_mblk->FadeInCount * INV_FADEIN_LEN;
      if (Param_mblk->fLog2Gain > FadeSC_mblk->BackgroundEnergyEst)
         Param_mblk->fLog2Gain = FadeSC_mblk->BackgroundEnergyEst + 
        (Param_mblk->fLog2Gain-FadeSC_mblk->BackgroundEnergyEst)*FadeFactor;
   }
  
   FadeSC_mblk->LastFrameState    = FadeSC_mblk->CurrentFrameState;
   FadeSC_mblk->CurrentFrameState = NORMAL;
}


/*******************************************************************************
* Function:  unsigned short VoxinitFadeIO(void **hFadeSCMemBlk)
*
* Action:    Initializes the LPCSyn struct
*
* Input:     hFadeSCMemBlk -- pointer to FadeSC memory block
*
* Output:    hFadeSCMemBlk -- memory allocated and initialized FadeSC memory 
*                               block
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
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
unsigned short VoxinitFadeSC(void **hFadeSCMemBlk)
{
   FadeSCstruct *FadeSC_mblk;

   if(VOX_MEM_INIT(FadeSC_mblk=*hFadeSCMemBlk,1,sizeof(FadeSCstruct))) 
      return 1;

   if(VOX_MEM_INIT(FadeSC_mblk->BackgroundASK,LPC_ORDER,sizeof(float))) 
      return 1;

   memcpy(FadeSC_mblk->BackgroundASK, fASKinit, LPC_ORDER*sizeof(float));
  
   FadeSC_mblk->FadeInCount         = 0; 
   FadeSC_mblk->CurrentFrameState   = NORMAL;
   FadeSC_mblk->LastFrameState      = NORMAL;
   FadeSC_mblk->BackgroundEnergyEst = INITIAL_BACKGROUND_ENERGY; 
 
  return 0;
}


/*******************************************************************************
* Function:  unsigned short VoxfreeFadeIO(void **hFadeSCMemBlk)
*
* Action:   free up fade SC memory block
*
* Input:   hFadeSCMemBlk -- pointer to FadeSC memory block
*
* Output:  hFadeSCMemBlk -- memory freed FadeSC structure
*
* Globals:  none
*
* Return:  0 -- succeed
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
unsigned short VoxfreeFadeSC(void **hFadeSCMemBlk)
{
   FadeSCstruct *FadeSC_mblk=(FadeSCstruct *)*hFadeSCMemBlk;

   if(*hFadeSCMemBlk) {
      VOX_MEM_FREE(FadeSC_mblk->BackgroundASK);
      VOX_MEM_FREE(*hFadeSCMemBlk);
   }
   return 0;
}


/*******************************************************************************
* Function:  SetCurrentFrameStateSC()
*
* Action:    Set the current frame state inside FadeSC structure
*
* Input:     pvFadeSCMemBlk -- FadeSC structure
*            iFrameFlag -- the frame state (either 0 or 1)
*
* Output:    pvFadeSCMemBlk -- the updated FadeSC structure
*
* Globals:   none
*
* Return:    none
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

void SetCurrentFrameStateSC(void *pvFadeSCMemBlk, const int iFrameFlag)
{
  ((FadeSCstruct *)pvFadeSCMemBlk)->CurrentFrameState = iFrameFlag;
}


/*******************************************************************************
* Function:  GetCurrentFrameStateSC()
*
* Action:    Get the current frame state from FadeSC structure
*
* Input:     pvFadeSCMemBlk -- FadeSC structure
*            
* Output:    same as return
*
* Globals:   none
*
* Return:    the current frame state from FadeSC structure
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

int GetCurrentFrameStateSC(const void *pvFadeSCMemBlk)
{
  return ((FadeSCstruct *)pvFadeSCMemBlk)->CurrentFrameState;
}

/*******************************************************************************
* Function:  GetLastFrameStateSC()
*
* Action:    Get the last frame state from FadeSC structure
*
* Input:     pvFadeSCMemBlk -- FadeSC structure
*            
* Output:    same as return
*
* Globals:   none
*
* Return:    the last frame state from FadeSC structure
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

int GetLastFrameStateSC(const void *pvFadeSCMemBlk)
{
  return ((FadeSCstruct *)pvFadeSCMemBlk)->LastFrameState;
}

/*******************************************************************************
* Function:  UpdateFadeStateSC()
*
* Action:    Update the state of the Fade routine for the case where the
*              the decoder requires multiple calls to decode one frame of 
*              parameters.  
*
* Input:     pvFadeSCMemBlk  -- fade memory block
*            DecReqDataFlag  -- flag from decoder if decoder needs new data.
*            
* Output:    none
*
* Globals:   none
*
* Return:    none
********************************************************************************
* Implementation/Detailed Description:
*              If the DecReqDataFlag is NOT true, then the decoder is still
*          working on the last frame.  When this happens CurrentFrameState
*          must be set to LastFrameState if LastFrameState is SILENCE.
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void UpdateFadeStateSC(const void *pvFadeSCMemBlk, unsigned char DecReqDataFlag)
{
  if (!DecReqDataFlag)
  {
     if (((FadeSCstruct *)pvFadeSCMemBlk)->LastFrameState == SILENCE)
        ((FadeSCstruct *)pvFadeSCMemBlk)->CurrentFrameState = SILENCE;
  }
}


