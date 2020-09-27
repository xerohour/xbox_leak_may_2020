/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       Refine.h                                                          
*                                                                              
* Purpose:        Performs spectrum reconstruction                                                             
*                                                                              
* Author/Date:    Original developped by Suat Yeldener   1995
*                 Written by Rob Zopf  Sept. 12/95
*                 Modified by Wei Wang,  2/28/96                                            
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/encode/Refine.h_v   1.0   26 Jan 1998 10:54:48   johnp  $                                                                     
*******************************************************************************/
#ifndef REFINE_H
#define REFINE_H


unsigned short VoxPitchRefine_TIME(float *const pitch, const float *const SWR, 
			   const float *const SWI, float *pPower, 
			   const unsigned int uiWinLen, float *BandError);

#endif /* REFINE_H */

