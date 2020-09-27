/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: ddVideo.h                                                         *
*    Definition file for:                                                   *
*       dvideo32.c                                                          *
*    Suggested new name:                                                    *
*       ddVideo.c                                                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe 06/16/99 - created.                              *
*                                                                           *
\***************************************************************************/

#ifndef _DDVIDEO_H_
#define _DDVIDEO_H_

#ifdef __cplusplus
extern "C" {
#endif

// public
DWORD __stdcall ConvertOverlay32                (LPDDRAWI_DDRAWSURFACE_LCL, BOOL waitForCompletion, LPDWORD lpDstYOffset);
DWORD __stdcall PreScaleOverlay32               (LPDDRAWI_DDRAWSURFACE_LCL);
DWORD __stdcall UpdateOverlay32                 (LPDDHAL_UPDATEOVERLAYDATA);
DWORD __stdcall SetOverlayPosition32            (LPDDHAL_SETOVERLAYPOSITIONDATA);
BOOL  __stdcall GetVideoScalerBandwidthStatus32 (DWORD, DWORD);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _DDVIDEO_H_
