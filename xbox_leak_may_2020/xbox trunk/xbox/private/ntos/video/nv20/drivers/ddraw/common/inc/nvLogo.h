/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvLogo.h                                                          *
*   NVIDIA Logo loading/drawing routines.                                   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe 05/26/99 - created.                              *
*                                                                           *
\***************************************************************************/

#ifndef _NVLOGO_H_
#define _NVLOGO_H_

#ifdef __cplusplus
extern "C" {
#endif

void nvLoadLogo (void);
void nvFreeLogo (void);
void nvDrawLogo (PNVD3DCONTEXT pContext, BOOL bClear);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVLOGO_H_
