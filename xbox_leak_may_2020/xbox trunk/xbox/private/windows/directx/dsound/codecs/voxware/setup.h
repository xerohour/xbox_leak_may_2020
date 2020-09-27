/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       Setup.h                                                              
*                                                                              
* Purpose:        Prepare all frame parameters for the current frame or subframe                                                             
*                                                                              
* Author/Date:    Rob Zopf   03/01/96                                                           
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/Setup.h_v   1.0   26 Jan 1998 10:52:10   johnp  $                                                                     
*******************************************************************************/
#ifndef SETUP_H
#define SETUP_H

unsigned short VoxSetup(STACK_R void *pvParam_mblk, void *pvSubFrameParam_mblk, void *pvPrevFrameParam_mblk, 
                float Warp, unsigned char *unpackFLAG);

void VoxUpdateSubframe(void *pvSubFrameParam_mblk);

#endif /* SETUP_H */

