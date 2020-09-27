/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       Sws.h
*                                                                              
* Purpose:        Sine-wave synthesis of harmonic amplitudes
*                                                                              
* Functions:      
*
*                                                                              
* Author/Date:    Rob Zopf   12/05/96
********************************************************************************
* Modifications:
*
*                                                                              
* Comments:                                                                    
* 
* Concerns:        
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/Sws.h_v   1.1   16 Feb 1998 12:22:50   bobd  $
*******************************************************************************/
#ifndef SWS_H
#define SWS_H

void VoxSws  (
          STACK_R
          void   *pSWS_mblk,
          float  *Amps,
          float  *Amps_1,
          float   Pitch,
          float   Pitch_1,
          float   Pv,
          float   Pv_1,
          short   frameLEN,
          short   nSubs,
          float  *outspeech
          );

unsigned short VoxInitSws(void **hSws);

unsigned short VoxFreeSws(void **hSws);

#endif /* SWS_H */

