/*******************************************************************************
*                         Voxware Proprietary Material                         *
*                         Copyright 1997, Voxware, Inc                         *
*                             All Rights Reserved                              *
*                                                                              *
*                        DISTRIBUTION PROHIBITED without                       *
*                       written authorization from Voxware                     *
*******************************************************************************/
 
/*******************************************************************************
* Filename:       winscale.h
*
* Purpose:        window scale factors for different pitch values
*
* Functions:      
*
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
********************************************************************************
* Modifications:  change to fLog2(winscale) by Wei Wang, 4/98
*
* Comments:
*
* Concerns:  If not use linear-approximation fLog2, the table should be 
*            recalculated.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/tables/winscale.h_v   1.2   10 Apr 1998 11:52:28   weiwang  $
*******************************************************************************/
#ifndef WINSCALE_H
#define WINSCALE_H
3.7298565F,   /* fLog2(1.3838853e+001F), */
3.6622577F,   /* fLog2(1.3298062e+001F), */
3.5917907F,   /* fLog2(1.2734326e+001F), */
3.5180564F,   /* fLog2(1.2144450e+001F), */
3.4405527F,   /* fLog2(1.1524421e+001F), */
3.3586349F    /* fLog2(1.0869080e+001F) */
#endif
