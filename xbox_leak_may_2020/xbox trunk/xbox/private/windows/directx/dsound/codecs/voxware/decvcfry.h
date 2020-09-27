/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/DecVcFry.h_v   1.0   06 Feb 1998 10:06:40   johnp  $
*******************************************************************************/
#ifndef _DETECT_VOCAL_FRY_
#define _DETECT_VOCAL_FRY_

short Detect_VocalFry(STACK_R void *hParamMblk, void *hVocalFryMblk, 
                      float *fAS, int lpcorder);

/* update the vocal fry detector states */
void UpdateVF_States(void *hParamMblk, void *hVocalFryMblk );

unsigned short VoxInitVocalFry(void **hVocalFryMblk);

unsigned short VoxFreeVocalFry(void **hVocalFryMblk);

#endif /* _DETECT_VOCAL_FRY_ */

