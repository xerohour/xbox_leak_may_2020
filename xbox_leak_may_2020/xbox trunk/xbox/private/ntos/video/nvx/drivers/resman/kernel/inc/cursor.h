#ifndef _CURSOR_H_
#define _CURSOR_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1997. All rights reserved.
//
/******************************* Cursor Module ******************************\
*                                                                           *
* Module: cursor.h                                                          *
*       cursor image and DAC support routines.                              *                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Jim Putnam (putnam)    12/05/97 - genesis                           *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Function prototypes for the cursor class.
//
//---------------------------------------------------------------------------

RM_STATUS cursLutDacCreate(PCLASSOBJECT, POBJECT *);
RM_STATUS cursLutDacDestroy(PCLASSOBJECT);
RM_STATUS cursLutDacSetNotifyCtxDma(POBJECT, PMETHOD, U032, U032);
RM_STATUS cursLutDacSetNotify(POBJECT, PMETHOD, U032, U032);

RM_STATUS cursLutNtscCreate(PCLASSOBJECT, POBJECT *);
RM_STATUS cursLutNtscDestroy(PCLASSOBJECT);
RM_STATUS cursLutNtscSetNotifyCtxDma(POBJECT, PMETHOD, U032, U032);
RM_STATUS cursLutNtscSetNotify(POBJECT, PMETHOD, U032, U032);

RM_STATUS cursLutOvlDacCreate(PCLASSOBJECT, POBJECT *);
RM_STATUS cursLutOvlDacDestroy(PCLASSOBJECT);
RM_STATUS cursLutOvlDacSetNotifyCtxDma(POBJECT, PMETHOD, U032, U032);
RM_STATUS cursLutOvlDacSetNotify(POBJECT, PMETHOD, U032, U032);

#endif // _CURSOR_H
