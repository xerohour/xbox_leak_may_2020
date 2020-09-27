/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:        Coder.c
*
* Purpose:         Code parameters for both SC3 and SC6
*
* Functions:   VoxQuantSC3, VoxQuantSC6, VoxUnQuantSC3 and VoxUnQuantSC6,
*              VoxInitQuantizer and VoxFreeQuantizer
*
* Author/Date:     Bob Dunn 1/22/97
*******************************************************************************
*
* Modifications:  Clean up the code by Wei Wang, 1/27/98
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/Coder.c_v   1.9   13 Apr 1998 16:17:44   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "VoxMem.h"
#include "codec.h"
#include "paramSC.h"
#include "CodeStr.h"
#include "SVqPhase.h"
#include "QP0SC3.h"
#include "quansc36.h"
#include "CodeGain.h"
#include "CodePv.h"
#include "MSVqPred.h"
#include "VqResi.h"
#include "Coder.h"

#ifndef DECODE_ONLY
#include "EncSC.h"
#endif

#include "DecPlsSC.h"

#include "xvocver.h"


#ifndef DECODE_ONLY
/******************************************************************************
*
* Function:  VoxQuantSC3
*
* Action:    Quantize parameters for SC3
*
* Input:     STACK_R -- the pointer to the Voxware stack 
*            piQuanIndex -- quantization indices
*            pvParamMblk_1 -- parameter structure of previous frame
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    piQuanIndex -- the updated quantization indices
*            pvParamMblk -- quantized parameters (except gain)
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxQuantSC3(STACK_R int *piQuanIndex, void *pvEncMblk,
                 void *pvParamMblk)
{
   ENC_MEM_BLK *pEncMblk = (ENC_MEM_BLK *)pvEncMblk;
   PARAM_SC *pParamMblk   = (PARAM_SC *)pvParamMblk;
   SUBFRAME_PARAM_SC *pSubParam_Mid, *pSubParam_End;


   assert((piQuanIndex != NULL) && (pvEncMblk != NULL));
   assert(pvParamMblk != NULL);

   pSubParam_Mid = &(pParamMblk->SubParam[PARAM_MIDFRAME]); 
   pSubParam_End = &(pParamMblk->SubParam[PARAM_ENDFRAME]); 

   /*--------------------------------------------------------
     Quantize VOICING and Get the quantized voicing.
     Quantize voicing vector on outer frame boundary
     -----------------------------------------------------*/
   piQuanIndex[INDEX_PV] = QuanPvSC3(pEncMblk->Pv_1, pSubParam_Mid->Pv,
                                     pSubParam_End->Pv);
   UnQuanPvSC3(piQuanIndex[INDEX_PV], pEncMblk->Pv_1,
               &(pSubParam_Mid->Pv), &(pSubParam_End->Pv));

 
   /*--------------------------------------------------------
     Quantize PITCH and Get the quantized pitch
     Quantize pitch on outer frame boundary
     -----------------------------------------------------*/
   piQuanIndex[INDEX_PITCH] = VoxQuanPitchSC3(pSubParam_End->Pitch);
   pSubParam_End->Pitch = VoxUnQuanPitchSC3(piQuanIndex[INDEX_PITCH]);

  
   /*------------------------------------------------------------------------
     Get the quantized the middle frame pitch:
     No frame-fill bits: Use voicing-dependent, 20ms  pitch interpolation
     -------------------------------------------------------------------------*/
   pSubParam_Mid->Pitch = UnQuanMidPitchSC3(pEncMblk->Pitch_1,
                                            pSubParam_End->Pitch,
                                            pEncMblk->Pv_1,
                                            pSubParam_End->Pv);
   
   /*--------------------------------------------------------
     Quantize GAIN
     Quantize gain on outer frame boundary
     -----------------------------------------------------*/
   piQuanIndex[INDEX_GAIN] = QuanGainSC3(pParamMblk->fLog2Gain);
  
   
   /*--------------------------------------------------------
     Preserve the unquantized ASKs.
     --------------------------------------------------------*/
   memcpy(pParamMblk->pfASKunquant, pParamMblk->pfASK, LPC_ORDER*sizeof(float));

   /*-----------------------------------------------------
     Quantize ASKs on outer frame boundary and Get the
     quantized ASKs.
     -----------------------------------------------------*/
   VoxMSVQEncPred( STACK_S pParamMblk->pfASK,
                   &(piQuanIndex[INDEX_ASK]),
                   LPC_ORDER,
                   pSubParam_End->Pv);

   /*-----------------------------------------------------
      Update the memory 
     ----------------------------------------------------*/
    pEncMblk->Pv_1 = pSubParam_End->Pv;
    pEncMblk->Pitch_1 = pSubParam_End->Pitch;
}



/******************************************************************************
*
* Function:  VoxQuantSC6
*
* Action:    Quantize parameters for SC6
*
* Input:     piQuanIndex -- pointer to quantization indices
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    piQuanIndex -- the updated quantization indices
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxQuantSC6(int *piQuanIndex, void *pvParamMblk)
{
   PARAM_SC    *pParamMblk   = (PARAM_SC *)pvParamMblk;
   SUBFRAME_PARAM_SC *pSubParam_Mid, *pSubParam_End;

   assert((piQuanIndex != NULL) && (pvParamMblk != NULL));


   pSubParam_Mid = &(pParamMblk->SubParam[PARAM_MIDFRAME]); 
   pSubParam_End = &(pParamMblk->SubParam[PARAM_ENDFRAME]); 

   /*--------------------------------------------------------
     Quantize Phase
     --------------------------------------------------------*/
   /*-----------------------------------------------------
     Compute the predicted phase based on the quantized pitch
     -----------------------------------------------------*/
   VoxEncPhasePrediction(pSubParam_Mid->Pitch, pSubParam_End->Pitch,
                         pSubParam_Mid->pfVEVphase, 
                         pSubParam_End->pfVEVphase);
  
   /*-----------------------------------------------------
     Vector quantize mid-frame and outer frame Phases
     -----------------------------------------------------*/
   VoxSVQEncPhase(pSubParam_Mid->pfVEVphase, pSubParam_End->pfVEVphase, 
                  &(piQuanIndex[INDEX_PHASE]));
  
   /*--------------------------------------------------------
     Quantize the residue ASKs
     --------------------------------------------------------*/
   VoxASKResiEnc(pParamMblk->pfASK, pParamMblk->pfASKunquant, 
                 &(piQuanIndex[INDEX_ASKRESI]));
}
#endif

/******************************************************************************
*
* Function:  VoxUnQuantSC3
*
* Action:    Unquantize parameters for SC3
*
* Input:     STACK_R -- stack memory pointer
*            piQuanIndex -- quantization indices
*            pvParamMblk_1 -- parameter structure of previous frame
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    pvParamMblk -- quantized parameters 
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxUnQuantSC3(STACK_R int *piQuanIndex, void *pvDecMblk,
                   void *pvParamMblk)
{
   PARAM_SC *pParamMblk   = (PARAM_SC *)pvParamMblk;
   float Pv_1;
   float Pitch_1;
   SUBFRAME_PARAM_SC *pSubParam_Mid, *pSubParam_End;

   assert((piQuanIndex != NULL) && (pvDecMblk != NULL));
   assert(pvParamMblk != NULL);

   pSubParam_Mid = &(pParamMblk->SubParam[PARAM_MIDFRAME]); 
   pSubParam_End = &(pParamMblk->SubParam[PARAM_ENDFRAME]); 

   Pv_1    = VoxGetDecPrevPvSC(pvDecMblk);
   Pitch_1 = VoxGetDecPrevPitchSC(pvDecMblk);

   /*--------------------------------------------------------
     UnQuantize VOICING
     Since Pv was used to quantize mid-frame pitch,
     it must be unquantized first
     --------------------------------------------------------*/
   /*-------------------------------------------------------
   Unquantize voicing vector of two subframes 
   -------------------------------------------------------*/
   UnQuanPvSC3(piQuanIndex[INDEX_PV], Pv_1,
               &(pSubParam_Mid->Pv), &(pSubParam_End->Pv));

   /*--------------------------------------------------------
     UnQuantize PITCH
     --------------------------------------------------------*/
   /*--------------------------------------------------------
     UnQuantize pitch on 20ms outer frame boundary
     --------------------------------------------------------*/
   pSubParam_End->Pitch = VoxUnQuanPitchSC3(piQuanIndex[INDEX_PITCH]);

   /*--------------------------------------------------------
     No frame-fill bits: Use voicing-dependent, 20ms pitch interpolation
     --------------------------------------------------------*/
   pSubParam_Mid->Pitch = UnQuanMidPitchSC3(Pitch_1,
                                            pSubParam_End->Pitch,
                                            Pv_1,
                                            pSubParam_End->Pv);

   /*--------------------------------------------------------
     UnQuantize GAIN
     --------------------------------------------------------*/
   /*--------------------------------------------------------
     UnQuantize gain on outer frame boundary
     --------------------------------------------------------*/
   pParamMblk->fLog2Gain = UnQuanGainSC3(piQuanIndex[INDEX_GAIN]);


   /*--------------------------------------------------------
     UnQuantize ASK
     --------------------------------------------------------*/

   /*--------------------------------------------------------
     UnQuantize ASKs on outer frame boundary
     --------------------------------------------------------*/
   VoxMSVQDecPred( STACK_S &(piQuanIndex[INDEX_ASK]), pParamMblk->pfASK,
                   LPC_ORDER, pSubParam_End->Pv);
}


/******************************************************************************
*
* Function:  VoxUnQuantSC6
*
* Action:    Unquantize parameters for SC6
*
* Input:     piQuanIndex -- quantization indices
*            pvParamMblk -- parameter structure of the current frame
*
* Output:    pvParamMblk -- pointer to the quantized parameters
*
* Globals:   none
*
* Return:    void
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
******************************************************************************/

void VoxUnQuantSC6(int *piQuanIndex,void *pvParamMblk)
{
   PARAM_SC *pParamMblk   = (PARAM_SC *)pvParamMblk;
   SUBFRAME_PARAM_SC *pSubParam_Mid, *pSubParam_End;

   assert((piQuanIndex != NULL) && (pvParamMblk != NULL));

   pSubParam_Mid = &(pParamMblk->SubParam[PARAM_MIDFRAME]); 
   pSubParam_End = &(pParamMblk->SubParam[PARAM_ENDFRAME]); 

   /*--------------------------------------------------------
     UnQuantize Phase
     --------------------------------------------------------*/
   /*--------------------------------------------------------
     UnQuantize mid-frame and outer frame Phase Vectors
     --------------------------------------------------------*/
   VoxSVQDecPhase(&(piQuanIndex[INDEX_PHASE]), pSubParam_Mid->pfVEVphase,
                  pSubParam_End->pfVEVphase);

   /*-----------------------------------------------------
     Compute the mid-frame phase from the quantized phase prediction
     -----------------------------------------------------*/
   VoxDecPhasePrediction(pSubParam_Mid->Pitch, pSubParam_End->Pitch,
                         pSubParam_Mid->pfVEVphase,
                         pSubParam_End->pfVEVphase);

   /*--------------------------------------------------------
     UnQuantize ASK residuals
     --------------------------------------------------------*/
   VoxASKResiDec(pParamMblk->pfASK, &(piQuanIndex[INDEX_ASKRESI]));
}
