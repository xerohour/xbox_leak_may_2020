/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       EncSC.c
*                                                                              
* Purpose:        Encode function which will analysis input speech 
*                 frame by frame and then store speech model parameters.
*                 There are two 10ms analysis frames for each 20ms outer
*                 transmit frame.
*                                                                              
* Functions:      VOXErr VoxEncodeSC(short *pPCMBuffer, void *pMemBlkE);
*                 VOXErr VoxInitEncodeSC(void **ppMemBlkE);
*                 VOXErr VoxFreeEncodeSC(void *pMemBlkE);
*                                                                              
* Author/Date:    Original developed by Suat Yeldner  1995
*                 Modified by Rob Zopf, Rewritten by Wei Wang, 2/96            
*                 Modified by Bob McAulay, 11/96
*                 Modified by Xiaoqin Sun,  10/97
*                 Modified by Wei Wang,  2/19/98 -- remove ParamMblk from 
*                                                     structure
********************************************************************************
* Modifications:                                                               
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/EncSC.c_v   1.14   14 Apr 1998 10:14:18   weiwang  $
******************************************************************************/
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "VoxMem.h"
#include "vem2.h"
#include "vem2Prm.h"

#include "vLibSnd.h"
#include "vLibSgPr.h"
#include "vLibTran.h"
#include "vLibSpch.h"
#include "vLibVec.h"
#include "vLibFilt.h"

#include "codec.h"
#include "quansc36.h"
#include "paramSC.h"
#include "STFTpks.h"
#include "Seevoc.h"
#include "EnvToAs.h"
#include "Voicing.h"
#include "STFTfaze.h"
#include "Window.h"
#include "LDPDA.h"
#include "MidFrame.h"
#include "rate.h"
#include "MidPitch.h"

#include "DecVcFry.h"
#include "EstOnset.h"
#include "EncSC.h"
#include "Lineintp.h"

#include "xvocver.h"


const float Kaiwin0[]={
#include "Kaiw291.h"
};

static float CoarsePitchAndFrameEnergy( STACK_R
                                        void  *hCircBufBlk,
                                        void  *hLdPdaBlk,
                                        float *fFrameEnergy
                                      );

static void PitchAdaptiveSpectrum( STACK_R
                                   float  fPitch,
                                   void  *hOnsetBlk,
                                   void  *hCircBufBlk,
                                   float *pfSWR,
                                   float *pfSWI,
                                   int   *piLength,
                                   int   *piWinShift
                                  );

static void OuterFrameParam( STACK_R
                             void        *hCircBufBlk,
                             float       *pfSWR,
                             float       *pfSWI,
                             PARAM_SC    *pParamMblk,
                             ENC_MEM_BLK *pEncMemBlk,
                             int          iLength,
                             int          iWinShift,
                             int         *piVFflag,
                             float       *pfCorrCost
                           );

/*******************************************************************************
* Function:       VOXErr VoxEncodeSC(short *pPCMBuffer, void *pMemBlkE, 
*                                     void *hParamMblk)
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:        VOXErr : Voxware return code
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
******************************************************************************/
unsigned short VoxEncodeSC( STACK_R 
                            short *pPCMBuffer, 
                            void  *hEncMemBlk, 
                            void  *hParamMblk
                          )
{
   ENC_MEM_BLK *pEncMemBlk   = (ENC_MEM_BLK *)hEncMemBlk;
   PARAM_SC    *pParamMblk   = (PARAM_SC *)hParamMblk;
   void        *hCircBufBlk = pEncMemBlk->CircBuf_mblk;
   SUBFRAME_PARAM_SC *pSubParam;


   int    iLength;              /* window length */
   int    vfFlag = 0;
   int    WinShift = 0;
   float  fCorrCost;
   
   pSubParam = &(pParamMblk->SubParam[PARAM_ENDFRAME]);

   /*--------------------------------------------------------------
     Test for a valid bitrate
   --------------------------------------------------------------*/
   if (!(pEncMemBlk->iSCTargetRate & pEncMemBlk->iSCRateValid))
      return 1;

   /*--------------------------------------------------------------
     Preprocess two 10ms frames of speech with a high-pass filter
       and store the result in the circular buffer.
   --------------------------------------------------------------*/
   {
      STACK_INIT
      float fSpBuf[FRAME_LENGTH_SC];
      STACK_INIT_END
 
      STACK_ATTACH(float *, fSpBuf)
 
      STACK_START

      VoxPreProcess( pPCMBuffer, fSpBuf, FRAME_LENGTH_SC, 
                     pEncMemBlk->PreProc_mblk );

      PutFrameSpCBM( hCircBufBlk, 0, FRAME_LENGTH_SC, fSpBuf );

      STACK_END
   }


   /*--------------------------------------------------------------
     Make a coarse pitch estimate and compute the frame energy
   --------------------------------------------------------------*/
   pSubParam->Pitch = CoarsePitchAndFrameEnergy( STACK_S
                                               hCircBufBlk,
                                               pEncMemBlk->LDPDA_mblk,
                                               &(pEncMemBlk->frameEnergy) );

   
   {
   STACK_INIT    /*%#  scratch memory #%*/
      float SWR[NFFT+2];
      float SWI[NFFT_2];
   STACK_INIT_END
 
      STACK_ATTACH(float *, SWR)
      STACK_ATTACH(float *, SWI)
 
      STACK_START

      /*--------------------------------------------------------------
        Compute the spectrum based on a pitch-adaptive Hamming
          window
      --------------------------------------------------------------*/
      PitchAdaptiveSpectrum( STACK_S pSubParam->Pitch,
                             pEncMemBlk->Onset_mblk, 
                             hCircBufBlk, SWR, SWI, &iLength, &WinShift );

      /*--------------------------------------------------------------
        Compute outer frame parameters (i.e. at 20 ms)
      --------------------------------------------------------------*/
      OuterFrameParam( STACK_S hCircBufBlk, SWR, SWI, pParamMblk, pEncMemBlk,
                               iLength, WinShift, &vfFlag, &fCorrCost);
      STACK_END
   }

   /*--------------------------------------------------------------
     Compute mid-frame parameters (i.e. at 10 ms)
   --------------------------------------------------------------*/
   MidFrameParam( STACK_S pEncMemBlk, hParamMblk, 
                  ENC_DELAY_SIZE+HALF_FRAME_LENGTH_SC, 
                  (short)pEncMemBlk->iSCTargetRate, fCorrCost, vfFlag );

   /*--------------------------------------------------------------
     update the states of vocal fry detector
   --------------------------------------------------------------*/
   UpdateVF_States((void *)pParamMblk, pEncMemBlk->VocalFry_mblk );

   /*--------------------------------------------------------------
     Save pitch and voicing estimates and the bit-rate
   --------------------------------------------------------------*/
   pEncMemBlk->uq_Pitch_1 = pSubParam->Pitch;
   pEncMemBlk->uq_Pv_1    = pSubParam->Pv;
   pEncMemBlk->iSCRate_1  = pEncMemBlk->iSCTargetRate;


   /*--------------------------------------------------------------
     Service the Voxware Extention Manager (VEM).
   --------------------------------------------------------------*/
   vemServiceState(pEncMemBlk->postEnc);

   return 0;

} /*VoxEncodeSC()*/


/******************************************************************************
* Function:       VOXErr VoxInitEncodeSC(void **ppMemBlkE, void *pVemMem)
*
* Action:         Initializes encoder structure
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:         VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*               Add initialization of PEA_mblk.  RJM 11/21/96.
*
* Comments:
*
* Concerns/TBD:
******************************************************************************/
unsigned short VoxInitEncodeSC3(void **hMemBlkE, void *pVemMem)
{

  ENC_MEM_BLK *pEncode_mblk;

   
  if(VOX_MEM_INIT(pEncode_mblk=*hMemBlkE,1,sizeof(ENC_MEM_BLK)))
     return 1;

  /****************************************************
   Ensure bit allocation is byte aligned
   ****************************************************/
  assert(!(TOTAL_BITS_SC3%8));

  /****************************************************
   Attaching the main vem mem handle
   ****************************************************/
  pEncode_mblk->pVemMem=pVemMem;


  /****************************************************
  initialize circular buffer structure
  ****************************************************/
  /* initialize the circular buffer manager */
  if(InitSpCBM(CIRCBUF_LENGTH, &pEncode_mblk->CircBuf_mblk))
      return 1;
 
 
  /****************************************************
  initialize the pre-process structure
  ****************************************************/
  if(VoxInitPreProc(&pEncode_mblk->PreProc_mblk, PREPROC_COEF))
      return 1;
   
 
  /****************************************************
  initialize PEA structure
  ****************************************************/
  pEncode_mblk->fVoicing_km1 = 0.0F;

  /****************************************************
  initialize LDPDA structure
  ****************************************************/
  VoxInitLDPDA(&pEncode_mblk->LDPDA_mblk);

  /****************************************************
    initialize vocal fry detector structure 
  ***************************************************/
  VoxInitVocalFry(&pEncode_mblk->VocalFry_mblk);

  /***************************************************
    Initialize onset detector structure 
  **************************************************/
  VoxInitEstOnset(&pEncode_mblk->Onset_mblk);

  /**** set the rate valid bit for SC_RATE_3K ****/
  pEncode_mblk->iSCRateValid = SC_RATE_3K;
  pEncode_mblk->iSCTargetRate = SC_RATE_3K;

  /***** Set the Cost for middle-frame ******/
  pEncode_mblk->lastCorrCost = 0.0F;

  /***** set the unquantized parameters of previous frame *****/
  pEncode_mblk->uq_Pv_1 = 0.0F;
  pEncode_mblk->uq_Pitch_1 = INITIAL_PITCH;

  /***** set the unquantized parameters of previous frame *****/
  pEncode_mblk->Pv_1 = 0.0F;
  pEncode_mblk->Pitch_1 = INITIAL_PITCH;

  vemRegisterVariable(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler,  
                      &pEncode_mblk->uq_Pv_1, VEM_FLOAT, "pv");

  vemRegisterVariable(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler,  
                      &pEncode_mblk->frameEnergy, VEM_FLOAT, "energy");
  
  pEncode_mblk->postEnc=vemRegisterState(((VEM_MAIN_STRUCT *)pVemMem)->pMemoryHandler, "post encode");

  return 0; 
}

unsigned short VoxInitEncodeSC6(void **hMemBlkE)
{
 
   ENC_MEM_BLK *pEncode_mblk=(ENC_MEM_BLK *)*hMemBlkE;


   /****************************************************
     Ensure bit allocation is byte aligned
   ****************************************************/
   assert( ADDITIONAL_BITS_SC6 == ADDITIONAL_BYTES_SC6*8 );

   /****************************************************
     check for a valid pointer
   ****************************************************/
   if (pEncode_mblk==NULL)
      return 1;

   /****************************************************
     make sure SC_RATE_3K was already initialized
   ****************************************************/
   if(!(pEncode_mblk->iSCRateValid & SC_RATE_3K))
      return 1;

  /**** set the rate valid bit for SC_RATE_6K ****/
  pEncode_mblk->iSCRateValid |= SC_RATE_6K;
  pEncode_mblk->iSCTargetRate = SC_RATE_6K;

  return 0;
}

/******************************************************************************
* Function:       VOXErr VoxFreeEncodeSC(void *pMemBlkE)
*
* Action:         Free's encoder structure
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:         VOXErr : Voxware return code
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
******************************************************************************/
unsigned short VoxFreeEncodeSC3(void **hMemBlkE)
{
  ENC_MEM_BLK *pEncode_mblk=(ENC_MEM_BLK*)*hMemBlkE;

   if (pEncode_mblk) {

     /**** free circular structure ****/
     FreeSpCBM(&pEncode_mblk->CircBuf_mblk);
 
     /**** free preprocess structure ****/
     VoxFreePreProc(&pEncode_mblk->PreProc_mblk);

     /**** free LDPDA structure *****/
     VoxFreeLDPDA(&pEncode_mblk->LDPDA_mblk);

     /**** free vocal fry structure ****/
     VoxFreeVocalFry(&pEncode_mblk->VocalFry_mblk);

     /**** free onset structure ****/
     VoxFreeEstOnset(&pEncode_mblk->Onset_mblk);
    
     VOX_MEM_FREE(pEncode_mblk);

#ifdef WIN32
     nspFreeFftResources();
#endif
  }
  return 0;
}

unsigned short VoxFreeEncodeSC6(void **hMemBlkE)
{
  ENC_MEM_BLK *pEncode_mblk=(ENC_MEM_BLK*)*hMemBlkE;

  if (pEncode_mblk) {
     /**** clear the rate valid bit for SC_RATE_6K ****/
     pEncode_mblk->iSCRateValid &= (~(SC_RATE_6K));

  }
  return 0;
}

/****************************************************************************/

/*********************************************************************
* voxSetEncode sets and returns the highest rate possible accroding to the
* bit field that was sent in     
**********************************************************************/
unsigned short voxSetEncodeRate(void *pMemBlkE, unsigned long wBitField)
{
   ENC_MEM_BLK *pEncode_mblk=(ENC_MEM_BLK*)pMemBlkE;
   unsigned short check=0;

   /* determine the highest rate in the field and set accordingly */
   if(wBitField&SC_RATE_6K) 
      if(pEncode_mblk->iSCRateValid&SC_RATE_6K) 
         return((unsigned short)(pEncode_mblk->iSCTargetRate=SC_RATE_6K));
      else
         check=1;

   if(wBitField&SC_RATE_3K||check) {
      if(pEncode_mblk->iSCRateValid&SC_RATE_3K) 
         return((unsigned short)(pEncode_mblk->iSCTargetRate=SC_RATE_3K));
      else
         return 0; /* couldn't find the rate */
   }
   return 0; /* couldn't find the bit rate */
}

int voxGetEncodeRate(const void *pMemBlkE)
{
   return(((ENC_MEM_BLK*)pMemBlkE)->iSCTargetRate);
}

int voxGetEncodeInitRate(const void *pMemBlkE)
{
   return(((ENC_MEM_BLK*)pMemBlkE)->iSCRateValid);
}

/******************************************************************************
*
* Function:  CoarsePitchAndFrameEnergy()
*
* Action:    Make a coarse estimate of the pitch and compute the power in
*              frame based on a fixed window.
*
* Input:     hCircBufBlk  - handle for the circular buffer structure
*            hLdPdaBlk    - handle for the LDPDA structure
*
* Output:    fFrameEnergy - the power in the speech for this frame
*
* Globals:   none
*
* Return:    float fPitch - the coarse pitch estimate
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
static float CoarsePitchAndFrameEnergy( STACK_R
                                        void  *hCircBufBlk,
                                        void  *hLdPdaBlk,
                                        float *fFrameEnergy
                                      )
{
   float  fPitch;
   float *pfSpBuf;
STACK_INIT
   float pfSW[NFFT+2];
STACK_INIT_END
 
   STACK_ATTACH(float *, pfSW)
 
   STACK_START

   assert((NFFT+2) >= PDA_FRAME_SIZE0);
   pfSpBuf = pfSW; /* make sure sharing is ok */

   /*-------------------------------------------------------------------
     Get a segment of speech from the circular buffer and window it
       with a 291 point Kaiser window.
   -------------------------------------------------------------------*/
   GetFrameSpCBM( hCircBufBlk, ENC_DELAY_SIZE, PDA_FRAME_SIZE0, pfSpBuf);
   SymWindowSignal( pfSpBuf, Kaiwin0, pfSpBuf, PDA_FRAME_SIZE0);

   /*-------------------------------------------------------------------
     Compute fFrameEnergy as the power of the windowed speech.
   -------------------------------------------------------------------*/
   *fFrameEnergy  = DotProd( pfSpBuf, pfSpBuf, PDA_FRAME_SIZE0);
   *fFrameEnergy *= INV_PEA_FRAME_SIZE0;

   /*------------------------------------------------------------
      Take the FFT of fSpBuf[] -- (in-place operation).
      The output is interleaved.
     ------------------------------------------------------------*/
   VoxFftCenter( pfSpBuf, PDA_FRAME_SIZE0, pfSW, FFT_ORDER);

   /*------------------------------------------------------------
      Coarse pitch estimation.
     ------------------------------------------------------------*/
    fPitch = VoxLDPDA(STACK_S pfSW, hLdPdaBlk);

   STACK_END

   return fPitch;

} /* CoarsePitchAndFrameEnergy() */

/******************************************************************************
*
* Function:  PitchAdaptiveSpectrum()
*
* Action:    Compute the spectrum of a the speech using a pitch-adaptive
*              Hamming window.  Shift the window in time (if necessary) to
*              account for onsets within the window.
*
* Input:     
*
* Output:    
*
* Globals:   none
*
* Return:    none
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

static void PitchAdaptiveSpectrum( STACK_R
                                   float  fPitch, 
                                   void  *hOnsetBlk,
                                   void  *hCircBufBlk,
                                   float *pfSWR, 
                                   float *pfSWI,
                                   int   *piLength, 
                                   int   *piWinShift 
                                  )
{
 
   int    iLength;

STACK_INIT
   float fSpBuf[PDA_FRAME_SIZE1];
STACK_INIT_END
 
   STACK_ATTACH(float *, fSpBuf)
 
   STACK_START

   /*-------------------------------------------------------------------
     Set pitch-adaptive window length.
   -------------------------------------------------------------------*/
   iLength = VoxAdaptiveWindow( fPitch );
 
   /*-------------------------------------------------------------------
     Compute an adjustment of the window location to account for
       onsets within the window.
   -------------------------------------------------------------------*/
   *piWinShift = EstOnset( hOnsetBlk, hCircBufBlk, ENC_DELAY_SIZE, iLength, 
                           fSpBuf);
 
   /*-------------------------------------------------------------------
     Get the speech
   -------------------------------------------------------------------*/
   assert((iLength>>1) <= (ENC_DELAY_SIZE+*piWinShift));
   GetFrameSpCBM( hCircBufBlk, ENC_DELAY_SIZE+*piWinShift, iLength, fSpBuf );
 
   /*-------------------------------------------------------------------
     Multiply by 2.0 because of complex analysis
   -------------------------------------------------------------------*/
   ScaleVector( fSpBuf, iLength, 2.0F, fSpBuf );
 
   /*-------------------------------------------------------------------
     Apply a unit area Hamming window (NOTE: the area of the window in
       HammingWindowNormSC() is 8% too big, but it is not changed for
       backward compatability with coding tables)
   -------------------------------------------------------------------*/
   HammingWindowNormSC(fSpBuf, iLength);
 
   /*-------------------------------------------------------------------
     Take the FFT of fSpBuf[].  The output is interleaved
       so demultiplex it.  (the real part is demultiplexed
       "in place")
   -------------------------------------------------------------------*/
   VoxFftCenter( fSpBuf, iLength, pfSWR, FFT_ORDER);
   VoxDemultiplex( pfSWR, pfSWR, pfSWI, NFFT_2);

   *piLength = iLength;

   STACK_END

} /* PitchAdaptiveSpectrum() */

/******************************************************************************
*
* Function:  OuterFrameParam()
*
* Action:    Compute the outer frame parameters:  pitch, voicing, gain, and
*              spectral envelope (arcsin of reflcetion coefficients).
*
* Input:
*
* Output:
*
* Globals:   none
*
* Return:    none
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

static void OuterFrameParam( STACK_R 
                             void        *hCircBufBlk, 
                             float       *pfSWR, 
                             float       *pfSWI, 
                             PARAM_SC    *pParamMblk,
                             ENC_MEM_BLK *pEncMemBlk,
                             int          iLength,
                             int          iWinShift,
                             int         *piVFflag,
                             float       *pfCorrCost 
                           )

{
   int   iPeakCount;
   float fPitchDFT;

   SUBFRAME_PARAM_SC *pSubParam;
   STACK_INIT
     float PeakPwr[MAXHARM];
     float PeakFrq[MAXHARM];
     float MagSq[NFFT_2];
     float fSpBuf[PDA_FRAME_SIZE0];
   STACK_INIT_END
    
   STACK_ATTACH(float *, PeakPwr)
   STACK_ATTACH(float *, PeakFrq)
   STACK_ATTACH(float *, MagSq)
   STACK_ATTACH(float *, fSpBuf)

   float *fAS = (float *)PeakPwr;        /***** fAS[LPC_ORDER_SC6+1]    *****/
   float *fKS = (float *)PeakFrq;        /***** fKS[LPC_ORDER_SC6]      *****/
   float *fLogSeeEnv = (float *)MagSq;   /***** share MagSq with the    *****/
                                         /***** memory fLogSeeEnv       *****/
   
   STACK_START

   assert(PDA_FRAME_SIZE0>=(2*MAXHARM)); /**** make sure the memory     *****/
                                         /**** sharing is ok            *****/
   assert(MAXHARM>=LPC_ORDER);           /**** make sure the memory     *****/
                                         /**** sharing is ok            *****/

   pSubParam = &(pParamMblk->SubParam[PARAM_ENDFRAME]);
      
   memset(PeakPwr, 0, MAXHARM*sizeof(float));
   memset(PeakFrq, 0, MAXHARM*sizeof(float));
    

   /*-------------------------------------------------------------------
     Compute the magnitude-squared STFT envelope and pick peaks
   -------------------------------------------------------------------*/
   VoxSTFTpeaks(STACK_S pfSWR, pfSWI, MagSq, PeakPwr, PeakFrq, &iPeakCount);

   /*-------------------------------------------------------------------
     Compute the flattop envelope of the squared STFT seevoc peaks
   -------------------------------------------------------------------*/
   fPitchDFT = (float)NFFT/pSubParam->Pitch;
   VoxSeevocFlattop(STACK_S MagSq, PeakPwr, PeakFrq, iPeakCount,fLogSeeEnv, 
                    fPitchDFT);
 
   /*-------------------------------------------------------------------
     Refine the pitch and compute the voicing probability
   -------------------------------------------------------------------*/
   VoxSTCVoicing(STACK_S &(pEncMemBlk->fVoicing_km1), fLogSeeEnv,
                 &(pSubParam->Pitch), 
                 &(pSubParam->Pv), 
                 PeakPwr, PeakFrq, iPeakCount);

   /*-------------------------------------------------------------------
     Interpolate the spectrum, fit a frequency-domain all-pole model,
       and compute the arcsin reflection coefficients.
   -------------------------------------------------------------------*/
   linearinterp (fPitchDFT, fLogSeeEnv, NFFT_2);

   LogEnvelopeToAs(STACK_S fLogSeeEnv, LPC_ORDER, &(pParamMblk->fLog2Gain),
                   fAS, fKS);

   K2ASK( LPC_ORDER,fKS, pParamMblk->pfASK );

   /*-------------------------------------------------------------------
     Compute a pitch correlation coefficient
   -------------------------------------------------------------------*/
   GetFrameSpCBM(hCircBufBlk, ENC_DELAY_SIZE+iWinShift, iLength, fSpBuf);

   ScaleVector( fSpBuf, iLength, 2.0F, fSpBuf );

   HammingWindowNormSC( fSpBuf, iLength);

   TDPitchRefine(fSpBuf, (short)iLength, pSubParam->Pitch,
                 pfCorrCost, 0.0F);
   
   /*-------------------------------------------------------------------
     Modify the voicing probability if it changes too sharply.
   -------------------------------------------------------------------*/
   pSubParam->Pv = ModifyPv(*pfCorrCost, pSubParam->Pv,
                            pEncMemBlk->uq_Pv_1,
                            pSubParam->Pitch,
                            pEncMemBlk->uq_Pitch_1);

   /*-------------------------------------------------------------------
     Detect the presence of vocal fry.  When it is present, cut the
       pitch period in half.
   -------------------------------------------------------------------*/
   *piVFflag = Detect_VocalFry( STACK_S (void *)pParamMblk,
                                pEncMemBlk->VocalFry_mblk, fAS, LPC_ORDER);

   if (*piVFflag == 1)
     pSubParam->Pitch *= 0.5F;

   STACK_END

   /*-------------------------------------------------------------------
     Compute the baseband measured phases for SC6.
   -------------------------------------------------------------------*/
   if (pEncMemBlk->iSCTargetRate==SC_RATE_6K)
   {
      /*** sample the 20ms STFT phase at the 20ms harmonics ***/
      VoxSTFTPhase(pfSWR, pfSWI, (int)SC6_PHASES,
                   pSubParam->pfVEVphase,
                   (float)NFFT/(pSubParam->Pitch),
                   (short)iWinShift);
   }

} /* OuterFrameParam() */   

