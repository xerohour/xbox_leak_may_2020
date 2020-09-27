/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       LDPDAdef.h
*                                                                              
* Purpose:        definitions and structure for LDPDA related files.
*                                                                              
* Functions:      
*
* Author/Date:    Wei Wang, 12/97
*
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/LDPDAdef.h_v   1.1   02 Apr 1998 14:46:58   weiwang  $
******************************************************************************/

#ifndef _LDPDA_DEF_H_
#define _LDPDA_DEF_H_

#define PITCH_TABLE_SIZE      89  

#define EXTEND_CUT_LC   96   
#define FAR_EXTEND_CUT_LC   102

#define MAX_NPEAKS     (EXTEND_CUT_LC>>1)

#define NCAND   3 /* only 3 candidates: first one, high-possibility, last one */

#define FREQ_CUT_LC      76   /* 80 1200Hz;  was 64 (1000Hz) */
#define ENERGY_CUT_OFF  0.05F    /* 20% cut-off for the high end */


typedef struct
{
  float fLastP0;
  float fLastCost;
} LD_PDA_MBlk;

#endif
