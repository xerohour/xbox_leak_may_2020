#ifndef _MP_H_
#define _MP_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
//
/****************************** MediaPort Control ***************************\
*                                                                           *
* Module: MP.H                                                              *
*       Mediaport Includes.                                                 *
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

//
// MediaPort context tree structure
//
typedef struct _def_mediaport_context
{ 
    U032                     Hwregs[50];
    PVIDEODECODEROBJECT      CurrentDecoder;
    PVIDEODECOMPRESSOROBJECT CurrentDecompressor;
} MEDIAPORTCONTEXT, * PMEDIAPORTCONTEXT;

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
RM_STATUS mpCreateObj(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS mpDestroyObj(VOID*, POBJECT);
RM_STATUS initMp(PHWINFO);
V032      mpService(PHWINFO);
RM_STATUS stateMp(PHWINFO, U032);

#endif // _MP_H_
