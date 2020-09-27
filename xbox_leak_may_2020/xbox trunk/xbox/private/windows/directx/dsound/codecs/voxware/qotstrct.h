/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        QoTstrct.h
*
* Purpose:         internal structure for VCI QOT
*
* Functions:       
*
* Author/Date:     Robert Zopf 08/18/97
*******************************************************************************
*
* Modifications: 
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/vci/QoTstrct.h_v   1.1   13 Feb 1998 10:42:46   weiwang  $
******************************************************************************/

#ifndef QOT_STRCT_H
#define QOT_STRCT_H

typedef struct
{
   float FrameStatus;
   short LastFrameUsed;
   unsigned long wNumOfBits;
   short SkippedFrames;
} VCI_QOT_BLOCK;


#endif /* QOT_STRCT_H */
