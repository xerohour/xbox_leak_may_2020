/******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/******************************************************************************
* Filename:       EncSTC.h
*                                                                              
* Purpose:       Encode function which will analysis input speech 
*                frame by frame and then store speech model parameters 
*                                                                      
*                                                                              
* Author/Date:    Original developped by Suat Yeldner  1995
*                 Modified by Rob Zopf, Rewritten by Wei Wang, 2/96
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/EncSC.h_v   1.2   13 Apr 1998 16:17:14   weiwang  $                                                                     
******************************************************************************/

#ifndef ENCSC_H
#define ENCSC_H

#ifndef __VEM_KEY
#define __VEM_KEY
typedef void * VEM_KEY;
#endif /*  __VEM_KEY */

/** Encoder main structure **/
typedef struct
{
  void* CircBuf_mblk;       /** in-data handling structure                  **/
  void* PreProc_mblk;       /** data preprocessing structure                **/
  float fVoicing_km1;       /** RJM's PEA related structure                 **/
  void* pParamMblk;         /** current frame parameters                    **/
  /*  void* pParamMblk_1; */      /** previoud frame parameters                   **/

  void* LDPDA_mblk;         /** structure for LDPDA **/
  void* VocalFry_mblk;      /** structure for vocal-fry detector **/

  void* Onset_mblk;         /** structure for onset detector **/

  float lastCorrCost;       /** correlation coefficients of last frame for middle **/

  int   iSCRateValid;       /** valid bit rates to use                      **/
  int   iSCRate_1;          /** bit rate for previous frame                 **/
  int   iSCTargetRate;      /** user modified bit rate **/

  /*** the non-inter-frame memory for VEM access ***/
  float frameEnergy;

  /**** unquantized parameters ****/
  float uq_Pv_1;
  float uq_Pitch_1;

  /**** quantized parameters ****/
  float Pv_1;
  float Pitch_1;

  void *pVemMem;               /*** IB 97/08/06 main vem mem handle ***/

  VEM_KEY postEnc;

} ENC_MEM_BLK;


#define FIXED_BETA 10.0F  /** controls sidelobs of Kaiser fixed window...was 6.0 **/

#define ADAPTIVE_BETA 6.0F  /** controls sidelobs of Kaiser adaptive windows...was 5.0 **/


unsigned short VoxEncodeSC(STACK_R short *pPCMBuffer, void *hEncMemBlk, 
                           void *pParamMblk);

unsigned short VoxInitEncodeSC3(void **hMemBlkE, void *pVemMem);

unsigned short VoxFreeEncodeSC3(void **hMemBlkE);

unsigned short VoxInitEncodeSC6(void **hMemBlkE);

unsigned short VoxFreeEncodeSC6(void **hMemBlkE);

unsigned short voxSetEncodeRate(void *pMemBlkE, unsigned long wScale);
int voxGetEncodeRate(const void *pMemBlkE);
int voxGetEncodeInitRate(const void *pMemBlkE);

#endif /* ENCSC_H */

