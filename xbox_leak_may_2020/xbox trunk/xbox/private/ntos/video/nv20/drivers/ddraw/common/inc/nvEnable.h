// (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvEnable.h                                                        *
*    Definition file for:                                                   *
*       enablec.c and disablec.c                                            *
*    Suggested new name for combined file:                                  *
*       nvEnable.c                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe          6/15/99 - Created                       *
*                                                                           *
\***************************************************************************/

#ifndef _NVENABLE_H_
#define _NVENABLE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// public
BOOL           nvEnable32                           (LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
BOOL __stdcall nvDisable32                          (void);
BOOL           nvEnableD3D                          (void);
BOOL           nvInitD3DObjects                     (void);
void           nvDisableD3D                         (void);
void           nvCreateVidHeapVarsAndPseudoNotifier (void);
BOOL           nvCreateDACObjects                   (DWORD dwChannel, DWORD dwHandle);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVENABLE_H_
