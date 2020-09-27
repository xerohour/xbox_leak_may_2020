/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*                                                                              *
*******************************************************************************/
 
/*******************************************************************************
*
* File: vLibMacr.h
*
* Purpose:   provide macros for Voxware Research library
*
* Functions:  SafeFree()
*
* Author/Date: Wei Wang, 3/6/98
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vLibMacr.h_v   1.0   06 Mar 1998 16:56:40   weiwang  $
******************************************************************************/

#ifndef _VLIBMACR_H_
#define _VLIBMACR_H_

#define SafeFree(ptr)  if(ptr) {free((void *)ptr); ptr=NULL;}

#endif
