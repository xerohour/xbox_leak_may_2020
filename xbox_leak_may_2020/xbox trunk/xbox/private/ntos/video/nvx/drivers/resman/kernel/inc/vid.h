#ifndef _VID_H_
#define _VID_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
//
/****************************** Video Control ******************************\
*                                                                           *
* Module: VID.H                                                             *
*       Video Includes.                                                     *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Structures.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Macros.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

// video.c
V032 videoService(PHWINFO);

// videobj.c
RM_STATUS videoCreateObj(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS videoDestroyObj(VOID*, POBJECT);
V032 videoFromMemService(PHWINFO, POBJECT);
V032 videoOverlayService(PHWINFO, POBJECT);
VOID videoUpdateWindowStart(PHWINFO, S016, S016);

#endif // _VID_H_
