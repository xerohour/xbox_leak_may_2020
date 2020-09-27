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
* Filename: MidFrame.c
*
* Purpose: Calculate the middle-frame parameters.
*
* Author:  Wei Wang
*
* Date:    June 2, 1997.
*
*******************************************************************************
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/MidFrame.c_v   1.11   14 Apr 1998 10:14:18   weiwang  $
*
******************************************************************************/
#include <math.h>
#include <assert.h>

#include "vLibSnd.h"
#include "vLibVec.h"
#include "vLibSgPr.h"
#include "vLibMath.h"

#include "codec.h"
#include "VoxMem.h"
#include "paramSC.h"
#include "dft.h"
#include "Window.h"
#include "MidPitch.h"
#include "EncSC.h"
#include "rate.h"

#include "MidFrame.h"

#include "xvocver.h"

static void GetWindowedSpeech(float pitch, void *hCircBufBlk, 
                              float *pfSpBuf, int *piLength, int iDelay);

static float MidFramePv(float Pv_1, float Pv1, float Cost1, 
                        float Cost0, float *Cost_1);

static void MidFramePhases(STACK_R float *SpBuf, int iLength, float fPitch, 
                           float *pfPhases);


/******************************************************************************
*
* Function: MidFrameParam()
*
******************************************************************************/
void MidFrameParam(STACK_R void *hEncMemBlk, void *hParamMblk,
                   int iCodecDelay, int iSCRate, float Cost1, 
                   int vfFlag)
{
  ENC_MEM_BLK *pEncMemBlk = (ENC_MEM_BLK *)hEncMemBlk;
  PARAM_SC *pParamMblk    = (PARAM_SC *)hParamMblk;

  int   iLength;
  float fCoarsePitchPeriod;
  float fCorrCostMid;
  SUBFRAME_PARAM_SC *pSubParam_Mid, *pSubParam_End;
  
  STACK_INIT
  float fSpBuf[PDA_FRAME_SIZE1];
  STACK_INIT_END
 
  STACK_ATTACH(float *, fSpBuf)

  STACK_START

  pSubParam_Mid = &(pParamMblk->SubParam[PARAM_MIDFRAME]);
  pSubParam_End = &(pParamMblk->SubParam[PARAM_ENDFRAME]);

  /*-----------------------------------------------------------------------
    Interpolate for a coarse estimate of the mid-frame pitch
  -----------------------------------------------------------------------*/
  fCoarsePitchPeriod = MidEstPitch( pEncMemBlk->uq_Pitch_1, 
                                    pSubParam_End->Pitch,
                                    pEncMemBlk->uq_Pv_1, 
                                    pSubParam_End->Pv);
  
  /*-----------------------------------------------------------------------
    If this is vocal fry, then the pitch has been modified and we want to
      undo this.
  -----------------------------------------------------------------------*/
  if (vfFlag == 1) 
    fCoarsePitchPeriod *= 2.0F;

  /*-----------------------------------------------------------------------
    Get windowed mikd-frame speech
  -----------------------------------------------------------------------*/
  GetWindowedSpeech( fCoarsePitchPeriod, pEncMemBlk->CircBuf_mblk, fSpBuf, 
                     &iLength, iCodecDelay);

  /*-----------------------------------------------------------------------
    Refine the pitch using autocorrelation calculation.
  -----------------------------------------------------------------------*/
  pSubParam_Mid->Pitch = TDPitchRefine( fSpBuf, (short)iLength,
                                        fCoarsePitchPeriod, 
                                        &fCorrCostMid, 0.1F);


  /*-----------------------------------------------------------------------
    If this is vocal fry, the reapply the vocal fry pitch period halving
  -----------------------------------------------------------------------*/
  if (vfFlag == 1) 
    pSubParam_Mid->Pitch *= 0.5F;

  /*-----------------------------------------------------------------------
    If encoding SC6, compute the mid-frame measured phases with a DFT.
  -----------------------------------------------------------------------*/
  if (iSCRate==SC_RATE_6K) 
    MidFramePhases( STACK_S fSpBuf, iLength, pSubParam_Mid->Pitch,
                    pSubParam_Mid->pfVEVphase);

  /*-----------------------------------------------------------------------
    Compute mid-frame voicing probability.
  -----------------------------------------------------------------------*/
  pSubParam_Mid->Pv = MidFramePv( pEncMemBlk->uq_Pv_1, 
                                  pSubParam_End->Pv,
                                  Cost1, fCorrCostMid, 
                                  &(pEncMemBlk->lastCorrCost));

  STACK_END

}
    

/******************************************************************************
*
* Function: MidFramePv()
*
* Action:   Find the middle-frame voicing
*
******************************************************************************/
static float MidFramePv(float Pv_1, float Pv1, float Cost1, 
                        float Cost0, float *Cost_1)
{
  float Pv;
  

  /******************************************************************
    calculate the cost value of end-frame.
    *****************************************************************/
  /********* Make the decision. ***********/
  if (*Cost_1 < 0.1F)    *Cost_1 = 0.1F;
  if (Cost1 < 0.1F)       Cost1 = 0.1F;

  if (fabs(Pv1 - Pv_1) < 0.15F)
    Pv = 0.5F*(Pv1+Pv_1);
  else 
  {
    if (Cost0 < 0.35F)     /* noise-like */
    {
      if (Pv1 > Pv_1) 
        Pv = Pv_1 * Cost0 / *Cost_1;
      else
        Pv = Pv1 * Cost0 / Cost1;
    } else {
      if (Pv1 > Pv_1) 
        Pv = Pv1 * Cost0 / Cost1;
      else
        Pv = Pv_1 * Cost0 / *Cost_1;
    }
  }

  if (Pv < 0)    
     Pv = 0.0F;
  if (Pv > 1)    
     Pv = 1.0F;

  /******* Replace last frame's cost value with this frame's one. ******/
  *Cost_1 = Cost1;

  return Pv;
}

/******************************************************************************
*
* Function: GetWindowedSpeech()
*
******************************************************************************/

static void GetWindowedSpeech( float fPitch, void *hCircBufBlk, 
                               float *pfSpBuf, int *piLength, int iDelay)
{
  /****** Get the window length and window the signal ******/
  *piLength = VoxAdaptiveWindow( fPitch );

  /**** Take the middle frame speech out ****/
  GetFrameSpCBM( hCircBufBlk, iDelay, *piLength, pfSpBuf );

  /**** window the speech signal ****/
  HammingWindowNormSC( pfSpBuf, *piLength );
}

/******************************************************************************
*
* Function: MidFramePhases()
*
* Action:   calculate middle-frame Phases using DFT or FFT
*
******************************************************************************/

static void MidFramePhases(STACK_R float *SpBuf, int iLength, float fPitch, 
                           float *pfPhases)
{
  int   i;
  float fSTFTphase;
  float fRadianPitch;

STACK_INIT
  float SWR[SC6_PHASES];
  float SWI[SC6_PHASES];
STACK_INIT_END

STACK_ATTACH(float *, SWR)
STACK_ATTACH(float *, SWI)

STACK_START

  /*---------------------------------------------------------------
    Compute the pitch in radians
  ---------------------------------------------------------------*/
  fRadianPitch = TWOPI/fPitch;

  /*---------------------------------------------------------------
    Calculate the DFT at the pitch harmonics. (NOTE: It is possible
      that fRadianPitch*SC6_PHASES is greater than PI.  If this 
      happens we are computing the DFT at frequencies beyond the 
      Nyquist rate.  This is okay, because the DFT will just
      return the aliased values and they will not be used by 
      the decoder.  The other alternative is to compute the number
      of harmonics, but that increases code size.)
  ---------------------------------------------------------------*/
  VoxDFT(SpBuf, iLength, fRadianPitch, SC6_PHASES, SWR, SWI);

  /*---------------------------------------------------------------
    Calculate the phases
  ---------------------------------------------------------------*/
  for (i = 0; i < SC6_PHASES; i++) 
  {
    fSTFTphase = VoxATAN2(SWI[i], SWR[i]);
    if (fSTFTphase < 0.0F)
      fSTFTphase += TWOPI;
    pfPhases[i] = fSTFTphase;
  }

STACK_END
}



