/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
*
* Filename: DecVcFry.c
*
* Purpose: Simple Vocal Fry detector -- only for pitch double case.
*
* Author:  Wei Wang
*
* Date:    July 11, 1997.
*
*******************************************************************************
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/DecVcFry.c_v   1.2   13 Apr 1998 16:17:08   weiwang  $
* 
*******************************************************************************/

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "vLibTran.h"

#include "codec.h"
#include "paramSC.h"
#include "VoxMem.h"
#include "DecVcFry.h"

#include "xvocver.h"

typedef struct
{
  float averagePitch;
  float confident;
  float *averageCepstrum;
} VocalFry_mblk;

static const float fCepWeight[VF_CEP_ORDER] = {
  2.552914F, 4.0F, 5.24264F,  6.19615F,
  6.795555F, 7.0F, 6.795555F, 6.1961524F,
  5.24264F,  4.0F, 2.552914F, 1.0F
};


static void Cepstrum4VocalFry(STACK_R float fGain, float *pfAS,
                              float *fAverageCepstrum,
                              float *dGain, float *dCep,
                              int lpcorder);

short Detect_VocalFry(STACK_R void *hParamMblk, void *hVocalFryMblk, 
                      float *fAS, int lpcorder)
{
  VocalFry_mblk *pVocalFryMblk = (VocalFry_mblk *)hVocalFryMblk;
  PARAM_SC *pParamMblk = (PARAM_SC *)hParamMblk;
  float dGain, dCep;
  float averagePitch = pVocalFryMblk->averagePitch;
  float confident = pVocalFryMblk->confident;
  float vfPitch;
  short VFflag = 0;
  SUBFRAME_PARAM_SC *pSubParam;

  assert(hParamMblk != NULL);

  pSubParam = &(pParamMblk->SubParam[PARAM_ENDFRAME]);

  /* Calculate the cepstrum coefficients to verify the decision */
  Cepstrum4VocalFry(STACK_S pParamMblk->fLog2Gain, fAS,
                    pVocalFryMblk->averageCepstrum, &dGain, &dCep, lpcorder);

  /* Find out when the pitch suddently doubled */
  if ((averagePitch < pSubParam->Pitch) && (pSubParam->Pv>=0.2F))
  {
    vfPitch = 0.5F*pSubParam->Pitch;      /* find half pitch */

    if ((averagePitch>(0.87F*vfPitch))&&(averagePitch<(1.15F*vfPitch)))
      VFflag = 1;
  }


  /* use dGain and dCep to eliminate the wrong decision */
  if ((VFflag == 1) && (dGain <= 2.0F)
      && (((dCep <= 0.5F) && (confident >= 3.5F))
      || ((dCep <= 0.4F)&&(confident>=2.5F))
      ||((dCep<=0.1)&&(confident>=1.5F))))
    VFflag = 1;
  else
    VFflag = 0;

  return VFflag;
}


/* update the vocal fry detector states */
void UpdateVF_States(void *hParamMblk, void *hVocalFryMblk )
{
  VocalFry_mblk *pVocalFryMblk = (VocalFry_mblk *)hVocalFryMblk;
  PARAM_SC *pParamMblk = (PARAM_SC *)hParamMblk;
  float averagePitch = pVocalFryMblk->averagePitch;
  float confident = pVocalFryMblk->confident;
  float vfPitch;
  SUBFRAME_PARAM_SC *pSubParam_Mid, *pSubParam_End;

  pSubParam_Mid = &(pParamMblk->SubParam[PARAM_MIDFRAME]);
  pSubParam_End = &(pParamMblk->SubParam[PARAM_ENDFRAME]);

  /* update the average pitch and confident */
  vfPitch = pSubParam_End->Pitch;

  if ((averagePitch>(0.8F*vfPitch))&&(averagePitch<(1.2F*vfPitch))) {
    averagePitch = 0.3F*averagePitch + 0.7F*vfPitch;
    confident += 1.0F;
  }
  else {
    if (pSubParam_End->Pv >= 0.3F) {  /*** probably voiced ****/
      averagePitch = vfPitch;
      confident = pSubParam_End->Pv;
    }
    else {
      averagePitch = VF_BAD_PITCH;
      confident = 0.0F;
    }
  }

  if ((averagePitch>(0.9F*pSubParam_Mid->Pitch))
      &&(averagePitch<(1.1F*pSubParam_Mid->Pitch)))
    confident += 1.0F;

  pVocalFryMblk->confident = confident;
  pVocalFryMblk->averagePitch = averagePitch;
}



/*
 * Cepstrum4VocalFry: calculate the difference of cepstrum and gain 
 */

static void Cepstrum4VocalFry(STACK_R float fGain, float *pfAS,
                              float *fAverageCepstrum,
                              float *dGain, float *dCep,
                              int lpcorder)
{
  STACK_INIT 
    float fCepstrum[VF_CEP_ORDER+1];
  STACK_INIT_END
  int i;
  register float dis;
  
  STACK_ATTACH(float *, fCepstrum)

  STACK_START

  /***** Change LPC coefficients to cepstrum *****/
  VoxAsToCs(fGain, pfAS, lpcorder, fCepstrum, VF_CEP_ORDER+1);

  /***** Compute the distortion *****/
  *dGain =(float)fabs(-fCepstrum[0] + fAverageCepstrum[0]);

  *dCep = 0.0F;
  for (i = 1; i <= VF_CEP_ORDER; i++) {
    fCepstrum[i] *= fCepWeight[i-1];

    dis = fCepstrum[i] - fAverageCepstrum[i];

    *dCep += dis * dis;
  }
  *dCep *= (float)INV_ORDER;


  /**** Get the average cepstrum ****/
  if (*dCep < 0.05F) {
    for (i = 0; i <= VF_CEP_ORDER; i++)
      fAverageCepstrum[i] = 0.9F*fCepstrum[i] + 0.1F*fAverageCepstrum[i];
  }
  else {
    for (i = 0; i <= VF_CEP_ORDER; i++)
      fAverageCepstrum[i] = fCepstrum[i];
  }

  STACK_END
}


/*
 * Initialize vocal fry structure 
 */
unsigned short VoxInitVocalFry(void **hVocalFryMblk)
{
  
  VocalFry_mblk *pVocalFryMblk;

  /**** initialize the structure pointer ****/
  if (VOX_MEM_INIT(pVocalFryMblk=*hVocalFryMblk, 1, sizeof(VocalFry_mblk)))
    return 1;

  /**** initialize the averageCepstrum array *****/
  if (VOX_MEM_INIT(pVocalFryMblk->averageCepstrum, VF_CEP_ORDER+1,
         sizeof(float)))
    return 1;
  
  
  /**** initialize the values ****/
  pVocalFryMblk->averagePitch = VF_BAD_PITCH;
  pVocalFryMblk->confident = 0.0F;
  memset(pVocalFryMblk->averageCepstrum, 0, sizeof(float)*(VF_CEP_ORDER+1));
  
  return 0;
}


/* 
 * free vocal fry structure 
 */
unsigned short VoxFreeVocalFry(void **hVocalFryMblk)
{
  VocalFry_mblk *pVocalFryMblk = (VocalFry_mblk *) *hVocalFryMblk;


  if (pVocalFryMblk) {

    /* free averageCepstrum */
    VOX_MEM_FREE(pVocalFryMblk->averageCepstrum);
    
    /* free pVocalFryMblk */
    VOX_MEM_FREE(pVocalFryMblk);

  }

  return 0;
}
