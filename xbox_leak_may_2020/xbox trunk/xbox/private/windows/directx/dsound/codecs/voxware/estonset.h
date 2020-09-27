/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/EstOnset.h_v   1.0   06 Feb 1998 10:06:46   johnp  $
*******************************************************************************/
#ifndef _ESTONSET_H_
#define _ESTONSET_H_

int EstOnset(void *pOnsetMblk, void *pCircBufMblk, int codec_delay_size,
               int winLength, float *SpBuf);

short VoxInitEstOnset(void **hOnsetMblk);

short VoxFreeEstOnset(void **hOnsetMblk);

#endif /* _ESTONSET_H_ */

