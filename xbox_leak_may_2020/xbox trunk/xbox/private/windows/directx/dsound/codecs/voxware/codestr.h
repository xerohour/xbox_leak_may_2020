/*****************************************************************************
 *                        Voxware Proprietary Material                       *
 *                        Copyright 1996, Voxware Inc.                       *
 *                        All Rights Reserved                                *
 *                                                                           *
 *                        DISTRIBUTION PROHIBITED without                    *
 *                        written authorization from Voxware.                *
 *****************************************************************************/

/******************************************************************************
* Filename:    CodeStr.h
*
* Purpose:     index structure which is referenced by Coder.c and PackSC.c
*
* Functions:   
*
* Author/Date:     Bob Dunn 7/18/97
*******************************************************************************
*
* Modifications: Cleaned up by Wei Wang Feb. 5, 1998
*
* Comments: 
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/quan/CodeStr.h_v   1.3   13 Apr 1998 16:17:42   weiwang  $
******************************************************************************/
#ifndef _CODESTR_H
#define _CODESTR_H

#include "codec.h"

#define INDEX_PITCH   0
#define INDEX_GAIN    1
#define INDEX_PV      2
#define INDEX_ASK     3     /* [LPC_ORDER] */
#define INDEX_RESERVE_BIT_SC3 (INDEX_ASK+LPC_ORDER)
#define INDEX_FLAG_BIT_SC3 (INDEX_ASK+LPC_ORDER+1)

#define TOTAL_SC3_QUAN_INDICES   (INDEX_FLAG_BIT_SC3+1)

#define INDEX_ASKRESI 0      /* [ASK_RES_ORDER] */
#define INDEX_PHASE   ASK_RES_ORDER    /* [SC6_PHASES] */
#define INDEX_RESERVE_BIT_SC6    (INDEX_PHASE+SC6_PHASES)
#define INDEX_FLAG_BIT_SC6       (INDEX_RESERVE_BIT_SC6+1)

#define EXTRA_SC6_QUAN_INDICES   (INDEX_FLAG_BIT_SC6+1)


#endif /*_CODESTR_H */

