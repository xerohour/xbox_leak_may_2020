/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       EncodeRT.h
*
* Purpose:       Encode function which will analysis input speech
*                frame by frame and then store speech model parameters
*
*
* Author/Date:
*                 Modified by Rob Zopf, Rewritten by Wei Wang, 2/96
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/encode/EncodeRT.h_v   1.4   04 Dec 1998 16:09:18   weiwang  $
*******************************************************************************/

#ifndef ENCODERT_H
#define ENCODERT_H

#ifndef __VEM_KEY
#define __VEM_KEY
typedef void * VEM_KEY;
#endif /* __VEM_KEY */

/** Encoder main structure **/
typedef struct
{
  void* CircBuf_mblk;                /** in-data handling structure including preprocessing **/
  void* PreProc_mblk;                /** preprocess structure            **/

  void* Voicing_mblk;                /** PDA related structure           **/
  void* PDA_mblk;                    /** Cross-Correlation PDA structure **/
  void* hPreFiltMblk;                /** PreFilter memory                **/

  void* Param_mblk;

  /*** Memories needed dynamic allocated ***/
  float *SWR;                             /** FFT real part **/
  float *SWI;                             /** FFT imaginary part **/

  /*** Kaiser windows which can be precalculated or loaded from tables ***/
  float *win1;                            /** kaiser(221, 6) **/
  float *win2;                            /** kaiser(241, 6) **/
  float *win3;                            /** kaiser(261, 6) **/

  short Nwin;                             /** Analysis Window length used for Theo Amp Scale  **/
  float *dwLpc;                           /** need to keep here for VMG! **/

  /*float winScale; No longer needed IB 97/01/22 */

  float uQPitch;                             /** unquantized Pitch **/
  float Pv_1;                             /** Pv for Rob's Magic **/

  void *pVemMem;  /* attachment to Vem memory */
  /* enocoder states that the RT supports */
  VEM_KEY vemKeyPostEncode;
  VEM_KEY vemKeyPostCalcResAmps;

} ENC_MEM_BLK;

unsigned short VoxEncodeRT(STACK_R short *pPCMBuffer, void *hEncMemBlk);

unsigned short VoxInitEncodeRT(void **ppMemBlkE, void *pVemMem);

unsigned short VoxFreeEncodeRT(void **hMemBlkE);

void* voxReturnPDAStruct(void *pMemBlkE);

#endif /* ENCODE_H */

