/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       lpcSynth.h                                                                 
*                                                                              
* Purpose:        Pass the excitation throught the lpc synthesis filter.
*                 Interpolate the filter for each subframe.                                                             
*                                                                              
* Author/Date:    Rob Zopf  02/08/96                                                            
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/lpcSynth.h_v   1.0   26 Jan 1998 10:52:18   johnp  $                                                                     
*******************************************************************************/
#ifndef LPCSYNTH_H
#define LPCSYNTH_H

typedef struct tagSynthesis
{
   float        *prevLSP;
   float        *lpcMEM;
} Synthesis;

unsigned short VoxInitLPCSyn(void **mySynthesis_mblk);

unsigned short VoxFreeLPCSyn(void **mySynthesis_mblk);

unsigned short VoxLPCSyn(STACK_R float *lsp, float *exciteIN, void *mySynthesis_mblk, short outputLEN);

#endif /* LPCSYNTH_H */

