// (C) Copyright NVIDIA Corporation Inc., 1996,1998. All rights reserved.

/********************************* Direct 3D *******************************\
*                                                                           *
* Module: DDHal.h                                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe          6/15/99 - Created                       *
*                                                                           *
\***************************************************************************/
#ifndef _DDHAL_H_
#define _DDHAL_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef WINNT
// public
FLATPTR EXTERN_DDAPI DDHAL32_VidMemAlloc (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, DWORD dwXDim, DWORD dwYDim);
void EXTERN_DDAPI DDHAL32_VidMemFree (LPDDRAWI_DIRECTDRAW_GBL pdrv, int heap, FLATPTR ptr);
#endif // WINNT

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _DDHAL_H_
