/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* Filename: MidFrame.h
*
* Purpose: Calculate the middle-frame parameters.
*
* Author:  Wei Wang
*
* Date:    June 2, 1997.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/MidFrame.h_v   1.2   25 Mar 1998 18:12:36   bobd  $
*******************************************************************************/
#ifndef _MIDFRAME_H_
#define _MIDFRAME_H_

void MidFrameParam(STACK_R void *hEncMemBlk, void *hParamMblk, int iCodecDelay,
                           int iSCRate, float Cost1, int vfFlag);

#endif /* _MIDFRAME_H_ */

