/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       DecPrivSC.h
*                                                                              
* Purpose:        The private structure for decoder. 
*                                                                              
* Functions:      no
*                                                                              
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications:  Remove ParamMblk to save memory. 
*
* Comments:  This is the private structure which can only be used for
*            DecSC.c and DecPlsSC.c
* 
* Concerns:  ParamMblk is removed however warping needs it. Instead, the 
*            current Vox Buffer is saved (packed bits) in VCI block. So 
*            warping can always work under quantization mode but not model 
*            mode (only encoder and decoder).
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/DecPriSC.h_v   1.7   22 Jun 1998 14:29:30   weiwang  $
******************************************************************************/
#ifndef DECPRISC_H
#define DECPRISC_H

/*---------------------------------------------------------------------
  Decoder Structure ...
  ---------------------------------------------------------------------*/
typedef struct tagDecode_mblk
{
  /* structure for VCI */
   void           *pFadeIOMemBlk;   /* Fade IO memory block                   */
   unsigned char  *pbSaveVoxBuffer; /* Save current Vox Buffer for later use  */

   void*     hHFRegenMblk;
   float*    pWaveform_1; /* overlapadd.. no need for a structure IB 97/05/15*/

   float*    pfSaveBuffer;  /* for output buffer manager */
   int       iSaveSamples;  /* for output buffer manager */

   int   iSCRateValid;       /** valid bit rates to use **/
   int   iSCRate_1;          /** bit rate for previous frame **/
   int   iSCTargetRate;      /** user modified bit rate **/

   long   lUVPhaseRandSeed;   /* random seed for unvoiced phases */

   float     *pfEnv_1;
   float     *pfPhase_1;

  /**** parameters of previous frame ****/
  float  Pitch_1_mid;
  SUBFRAME_PARAM_SC  PrevSubParam;

   float     fWarpedLengthFactor;
   float     fActualWarp;  
   int       iNewFrame;
   float     fSynTimeOffset;
   float     fPhaseOffset;
   int       iFramesSkipped;
   int       iActualFramesSkipped;
   int       iRepeatFrameFlag;
   int       iFramesRepeated;
   int       iSetOffsetFlag;

   int       piFrameCount[4];
} DEC_MEM_BLK;


#endif
