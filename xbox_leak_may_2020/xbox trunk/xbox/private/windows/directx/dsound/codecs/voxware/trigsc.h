/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:     trig.h
*
* Purpose:      Provide a global pointer to a sin and a cos table. 
*
*
*
* Author/Date:  Rob Zopf
*******************************************************************************
* Modifications:
*                                                
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/utils/trigSC.h_v   1.1   14 Apr 1998 15:33:18   weiwang  $
*
******************************************************************************/
#ifndef TRIG_SC_H
#define TRIG_SC_H

#define LOG2_TRIG_SIZE      8
#define TRIG_SIZE           (1<<LOG2_TRIG_SIZE)
#define TRIG_MASK           (TRIG_SIZE-1)

#endif /* TRIG_SC_H */
 
