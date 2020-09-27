/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       VoicDeci.h                                                            
*                                                                              
* Purpose:        voice probability calculation                                                             
*                                                                              
* Author/Date:    Original developped by Suat Yeldener
*                 Integrated by Rob Zopf Sept. 12/95.                                           
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/encode/VoicDeci.h_v   1.0   26 Jan 1998 10:55:02   johnp  $                                                                     
*******************************************************************************/
#ifndef VOICDECI_H
#define VOICDECI_H

#define CONSTANT4PV           (0.3096F*D_PI)

unsigned short VoxVocDecision(const float fPitch, void *PDA_mblk, float *pPv, 
                      float *pPower, float *pBandErr, unsigned int uiWinLen, short *Class);

unsigned short VoxInitVoicing(void **Voicing_mblk);
unsigned short VoxFreeVoicing(void **hVoicing_mblk);

#endif /* VOICEDECI_H */
