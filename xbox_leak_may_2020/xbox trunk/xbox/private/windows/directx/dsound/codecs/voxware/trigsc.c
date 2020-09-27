/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:     trig.c
*                                                              
* Purpose:      Provide a global pointer to a sin and a cos table.
*                                                                          
*                                                                   
* Author/Date:  Rob Zopf        
*******************************************************************************
* Modifications:
*                                                
* Comments: The table MUST include the value at 2 PI!                                                                   
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/trigSC.c_v   1.3   16 Apr 1998 13:28:18   bobd  $
* 
******************************************************************************/
#include "codec.h"
#include "trigSC.h"

#include "xvocver.h"

/*-------------------------------------------------------
  WARNING!! In OverAdd.c it is assumed that the trig
   tables range from zero to and including 2 PI!
   This save computation in computing phase indices
   that are modulo 2 PI.  Be sure the table includes 
   the value at 2 PI.
-------------------------------------------------------*/

#if (LOG2_TRIG_SIZE==10)
const float trigTBL_32[] = 
{
#include "trgTblSC.h"
};
#endif

#if (LOG2_TRIG_SIZE==8)
const float trigTBL_32[] = 
{
#include "trgTbl8.h"
};
#endif

const float *sinTBL_32 = trigTBL_32;
const float *cosTBL_32 = trigTBL_32 + TRIG_SIZE/4;

const float fRadiansToIndex = (float)TRIG_SIZE/TWOPI;
