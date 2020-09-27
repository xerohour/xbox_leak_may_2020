/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       DecSC.c
*                                                                              
* Purpose:        Decode speech model parameters and synthesize speech
*                 frame by frame and store in output buffer
*                                                                              
* Functions:      VoxDecodeSC(), VoxInitDecodeSC(), VoxFreeDecodeSC()
*                                                                              
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications: changed output buffer manager, Wei Wang, 2/18/98
*                remove paramMblk from decoder structure, 2/19/98, Wei Wang
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/DecSC.c_v   1.21   13 Apr 1998 16:16:58   weiwang  $
******************************************************************************/


#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "vLibTran.h"
#include "vLibVec.h"
#include "vLibMath.h"
#include "VoxMem.h"

#include "paramSC.h"
#include "codec.h"
#include "quansc36.h"
#include "SWSynth.h"
#include "rate.h"
#include "AskToEnv.h"
#include "HFRegen.h"
#include "OutBufSC.h"
#include "SubFmSyn.h"
#include "DecPriSC.h"
#include "DecSC.h"

#include "xvocver.h"
 
/*--------------------------------------------------
  Private functions
  --------------------------------------------------*/
static void DecFrameInit(DEC_MEM_BLK *pDecMemBlk );

static void DecFrameCleanUp(DEC_MEM_BLK *pDecMemBlk, 
                            PARAM_SC *pParamMblk, 
                            float *pfEnv, 
                            float *pfPhase);



/*******************************************************************************
*
* Function:  VoxDecodeSC()
*
* Action:    main decoder function
*
* Input:    pPCMBuffer   -- pointer to the output PCM buffer
*           hMemBlkD     -- the decoder memory block
*           hParamMblk   -- parameter memory block
*
* Output:   pPCMBuffer   -- output PCM buffer
*           pwPCMsamples -- number of samples in the output PCM buffer
*           hMemBlkD     -- updated decoder memory block
*
* Globals:   none
*
* Return:    none
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void VoxDecodeSC( STACK_R 
                  void           *hMemBlkD,
                  void           *hParamMblk,
                  short          *pPCMBuffer, 
                  unsigned short *pwPCMsamples 
                )
{
   DEC_MEM_BLK *pDecMemBlk=(DEC_MEM_BLK*)hMemBlkD;

   PARAM_SC  *pParamMblk   = (PARAM_SC *)hParamMblk;

   float *fSpeechBuf;

   /*---------------------------------------------------
     The arrays pfEnv[] and pfPhase[] require
       CSENV_NFFT_2+1 points in SetAmpAndPhase().
   ---------------------------------------------------*/
STACK_INIT
   float pfEnv[CSENV_NFFT_2+1];
   float pfPhase[CSENV_NFFT_2+1];
STACK_INIT_END
 
   STACK_ATTACH(float *, pfEnv)
   STACK_ATTACH(float *, pfPhase)
  
   int    iSynSamples = 0;

   STACK_START

   /*----------------------------------------------------------------------
     Initialize some things in the decoder block to prepare for synthesis
   ----------------------------------------------------------------------*/
   DecFrameInit( pDecMemBlk );

   /*----------------------------------------------------------------------
     Copy the content of pSaveBuffer to pPCMBuffer.
    ----------------------------------------------------------------------*/
   PreOutputManagerSC(pDecMemBlk->pfSaveBuffer, pDecMemBlk->iSaveSamples,
                      pPCMBuffer);

   fSpeechBuf = pDecMemBlk->pfSaveBuffer; /* Re-use pSaveBuffer for synthesis */

   /*--------------------------------------------------------------------
     Compute envelope and phase from arcsin reflection coefficients
   --------------------------------------------------------------------*/
   VoxAskToEnv(STACK_S pParamMblk->pfASK, pParamMblk->fLog2Gain,
               pfEnv, pfPhase);

   /*--------------------------------------------------------------------
     Synthesize 10 ms speech sub-frames (0, 1, or 2 may be synthesized)
   --------------------------------------------------------------------*/
   SubFrameSynth( STACK_S pDecMemBlk, (void *)(pParamMblk->SubParam),
                  fSpeechBuf, pfEnv, pfPhase,
                  &iSynSamples);

   /*----------------------------------------------------------------------
     Do output buffering and put output in pPCMBuffer
   ----------------------------------------------------------------------*/
   PostOutputManagerSC(pDecMemBlk->pfSaveBuffer, &(pDecMemBlk->iSaveSamples),
                       iSynSamples, pPCMBuffer, pwPCMsamples);

   /*----------------------------------------------------------------------
     Do book-keeping for next time
   ----------------------------------------------------------------------*/
   DecFrameCleanUp(pDecMemBlk, pParamMblk, pfEnv, pfPhase );

   STACK_END
}


/*******************************************************************************
*
* Function:  VoxInitDecodeSC3()
*
* Action:    initialize decoder structure for SC3
*
* Input:    ppMemBlkD -- pointer to the decoder memory block (NULL)
*
* Output:   ppMemBlkD -- initialized decoder memory block
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

unsigned short VoxInitDecodeSC3(void **ppMemBlkD)
{
   DEC_MEM_BLK *pDecode_mblk;

   if(VOX_MEM_INIT(pDecode_mblk=*ppMemBlkD,1,sizeof(DEC_MEM_BLK)))
      return 1;

   /****************************************************
    Ensure bit allocation is byte aligned
    ****************************************************/
   assert(TOTAL_BITS_SC3%8 == 0);

   /****************************************************
     Initialize Overlapadd 
     ***************************************************/
   if (VOX_MEM_INIT(pDecode_mblk->pWaveform_1, HALF_FRAME_LENGTH_SC, 
                    sizeof(float)))
       return 1;

   /****************************************************
    Initialize the Output Buffer block
    ****************************************************/
   if (VOX_MEM_INIT(pDecode_mblk->pfSaveBuffer, FRAME_LENGTH_SC, sizeof(float)))
      return 1;
   
   /****************************************************
    initialize HFRegen structure
    ****************************************************/
   if(VoxInitHFRegen(&pDecode_mblk->hHFRegenMblk))
      return 1;

   /****************************************************
    initialize the fWarpedLengthFactor
    ****************************************************/
   pDecode_mblk->fWarpedLengthFactor=1.0F;
   pDecode_mblk->fActualWarp=1.0F;
   pDecode_mblk->iNewFrame=1;
   pDecode_mblk->fSynTimeOffset=(float)HALF_FRAME_LENGTH_SC;

   pDecode_mblk->piFrameCount[0] = 1;   /* value of 1 assumes no warping */
   pDecode_mblk->piFrameCount[1] = 1;   /* value of 1 assumes no warping */

   /****************************************************
    Set the random seed for unvoiced phases 
    ****************************************************/
   pDecode_mblk->lUVPhaseRandSeed = 1;

   /****************************************************
    Set the rate valid flag
    ****************************************************/
   pDecode_mblk->iSCRateValid = SC_RATE_3K;
   pDecode_mblk->iSCTargetRate = SC_RATE_3K;
   pDecode_mblk->iSCRate_1 = SC_RATE_3K;

   /****************************************************
    Allocate space for phase and magnitude envelopes
    ****************************************************/
   if(VOX_MEM_INIT(pDecode_mblk->pfEnv_1,(CSENV_NFFT_2+1),
                   sizeof(float)))
      return 1;
 
   if(VOX_MEM_INIT(pDecode_mblk->pfPhase_1,(CSENV_NFFT_2+1),
                   sizeof(float)))
      return 1;

   /***************************************************
     Initialize the parameters of previous frame
    **************************************************/
   pDecode_mblk->Pitch_1_mid = INITIAL_PITCH;
   pDecode_mblk->PrevSubParam.Pitch = INITIAL_PITCH;

   return 0;
}

/*******************************************************************************
*
* Function:  VoxInitDecodeSC6()
*
* Action:    initialize decoder structure for SC6
*
* Input:    ppMemBlkD -- pointer to the decoder memory block
*
* Output:   ppMemBlkD -- initialized decoder memory block
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

unsigned short VoxInitDecodeSC6(void **ppMemBlkD)
{
   DEC_MEM_BLK *pDecode_mblk=(DEC_MEM_BLK *)*ppMemBlkD;

   /****************************************************
    Ensure bit allocation is byte aligned
    ****************************************************/
   assert( ADDITIONAL_BITS_SC6 == ADDITIONAL_BYTES_SC6*8 );

   /****************************************************
    check for valid pointer
    ****************************************************/
   if (pDecode_mblk==NULL)
      return 1;

   /****************************************************
    make sure SC_RATE_3K was already initialized
    ****************************************************/
   if(!(pDecode_mblk->iSCRateValid & SC_RATE_3K))
      return 1;


   /****************************************************
    Set the rate valid flag
    ****************************************************/
   pDecode_mblk->iSCRateValid |= SC_RATE_6K;
   pDecode_mblk->iSCTargetRate = SC_RATE_6K;
   pDecode_mblk->iSCRate_1 = SC_RATE_6K;

   return 0;
}

/*******************************************************************************
*
* Function:  VoxFreeDecodeSC3()
*
* Action:    free decoder structure for SC3
*
* Input:    hMemBlkD -- pointer to the decoder memory block
*
* Output:   hMemBlkD -- freed decoder memory block (NULL)
*
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

unsigned short VoxFreeDecodeSC3(void **hMemBlkD)
{
    DEC_MEM_BLK *pDecode_mblk=(DEC_MEM_BLK*)*hMemBlkD;

#ifdef WIN32
    nspFreeFftResources();
#endif

    if (pDecode_mblk) {

       VOX_MEM_FREE(pDecode_mblk->pWaveform_1);

       VOX_MEM_FREE(pDecode_mblk->pfSaveBuffer);

       VoxFreeHFRegen(&pDecode_mblk->hHFRegenMblk);

       VOX_MEM_FREE(pDecode_mblk->pfEnv_1);

       VOX_MEM_FREE(pDecode_mblk->pfPhase_1);

       VOX_MEM_FREE(pDecode_mblk);
    }
    return 0;
}

/*******************************************************************************
*
* Function:  VoxFreeDecodeSC6()
*
* Action:    free Decoder structure for SC6
*
* Input:    hMemBlkD -- pointer to the decoder memory block
*
* Output:   hMemBlkD -- modified decoder memory block with SC6 related fields
*                       freed.
* Globals:   none
*
* Return:    0: succeed, 1: failed
********************************************************************************
* Modifications:
*
* Comments:    Need call this function before call VoxFreeDecodeSC3().
*
* Concerns/TBD:
*******************************************************************************/

unsigned short VoxFreeDecodeSC6(void **hMemBlkD)
{
    DEC_MEM_BLK *pDecode_mblk=(DEC_MEM_BLK*)*hMemBlkD;
  
    if (pDecode_mblk) {

       /****************************************************
        clear the rate valid flag
        ****************************************************/
       pDecode_mblk->iSCRateValid &= (~(SC_RATE_6K));
    }

    return 0;
}



/*******************************************************************************
*
* Function:  DecFrameInit()
*
*******************************************************************************/

static void DecFrameInit( DEC_MEM_BLK *pDecMemBlk )
{
   /*-------------------------------------------------------------------------
     When decoding a new frame of data, the following must be initialized:
   -------------------------------------------------------------------------*/
   if (pDecMemBlk->iNewFrame)
   {
      /*----------------------------------------------
        Read the time warp factor and the number of
          frames skipped.
      ----------------------------------------------*/
      pDecMemBlk->fActualWarp          = pDecMemBlk->fWarpedLengthFactor;
      pDecMemBlk->iActualFramesSkipped = pDecMemBlk->iFramesSkipped;
   
      /*----------------------------------------------
        Determine if the phase offset should be 
          adjusted in HighFrequencyRegeneration()
          (i.e the value of iSetOffsetFlag)
      ----------------------------------------------*/
      if (pDecMemBlk->iActualFramesSkipped || (pDecMemBlk->iFramesRepeated
          && !(pDecMemBlk->iRepeatFrameFlag)))
         pDecMemBlk->iSetOffsetFlag = 1;
      else
         pDecMemBlk->iSetOffsetFlag = 0;
   
      /*----------------------------------------------
        Set the number of frame repeats
      ----------------------------------------------*/
      if (pDecMemBlk->iRepeatFrameFlag)
         pDecMemBlk->iFramesRepeated += 1;
      else
         pDecMemBlk->iFramesRepeated = 0;

      /*----------------------------------------------
        Reset these values
      ----------------------------------------------*/
      pDecMemBlk->iFramesSkipped = 0;
      pDecMemBlk->iRepeatFrameFlag = 0;
      pDecMemBlk->iNewFrame = 0;
   }
}


/*******************************************************************************
*
* Function:  DecFrameCleanUp()
*
*******************************************************************************/

static void DecFrameCleanUp(DEC_MEM_BLK *pDecMemBlk, PARAM_SC *pParamMblk,
                            float *pfEnv, float *pfPhase )
{
   int *piFrameCount;

   /*----------------------------------------------------------------------
     Save some things and set up for next time
   ----------------------------------------------------------------------*/
   if (pDecMemBlk->iNewFrame) 
   {
      /*----------------------------------------------------------------------
        Save/reset frame counters
      ----------------------------------------------------------------------*/
      piFrameCount = pDecMemBlk->piFrameCount;
     
      piFrameCount[0] = piFrameCount[2];
      piFrameCount[1] = piFrameCount[3];
      piFrameCount[2] = 0;
      piFrameCount[3] = 0;

      /*----------------------------------------------------------------------
        save amplitude and phase envelopes
      ----------------------------------------------------------------------*/
      memcpy( pDecMemBlk->pfEnv_1, pfEnv, (CSENV_NFFT_2+1)*sizeof(float));

      memcpy( pDecMemBlk->pfPhase_1, pfPhase, (CSENV_NFFT_2+1)*sizeof(float));
 
      /*----------------------------------------------------------------------
       copy the parameters
      ----------------------------------------------------------------------*/
      memcpy(&(pDecMemBlk->PrevSubParam), 
             &(pParamMblk->SubParam[PARAM_ENDFRAME]),
             sizeof(SUBFRAME_PARAM_SC));

      pDecMemBlk->Pitch_1_mid = pParamMblk->SubParam[PARAM_MIDFRAME].Pitch;
   }
}
