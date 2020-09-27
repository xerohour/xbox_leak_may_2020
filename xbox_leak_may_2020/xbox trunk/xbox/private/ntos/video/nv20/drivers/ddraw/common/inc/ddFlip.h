/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: ddFlip.h                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe 06/16/99 - created.                              *
*                                                                           *
\***************************************************************************/

#ifndef _DDFLIP_H_
#define _DDFLIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FLIP_ANYSURFACE         0xFFFFFFF7         // fpVidMem parameter for updateFlipStatus

// prototypes
HRESULT __stdcall getFlipStatusPrimary (FLATPTR fpVidMem);
HRESULT __stdcall getFlipStatusOverlay (FLATPTR fpVidMem);
HRESULT __stdcall getFlipStatus        (FLATPTR fpVidMem, DWORD dwType);
DWORD   __stdcall GetFlipStatus32       (LPDDHAL_GETFLIPSTATUSDATA lpGetFlipStatus);
DWORD   __stdcall Flip32                (LPDDHAL_FLIPDATA pfd);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _DDFLIP_H_
