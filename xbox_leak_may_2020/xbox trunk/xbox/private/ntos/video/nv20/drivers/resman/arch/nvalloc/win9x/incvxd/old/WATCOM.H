#ifndef _WATCOM_H_
#define _WATCOM_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/*************************** OS Specific Module ****************************\
*                                                                           *
* Module: WATCOM.H                                                          *
*       WATCOM and Windows specific defines and pragmas.                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)     1/06/95 - copied from nvrm.h.          *
*                                                                           *
\***************************************************************************/
//
// Special pragmas for the WATCOM VxD build environment.
//
#ifdef _LGROUP
#pragma data_seg( "_LDATA", "CODE" )
#endif
#ifdef _IGROUP
#pragma data_seg( "_IDATA", "ICODE" )
#endif
#endif // _WATCOM_H_

