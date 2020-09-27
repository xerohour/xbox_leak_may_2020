/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvVideoPort.h                                                     *
*    Definition file for:                                                   *
*       vp.c                                                                *
*    Suggested new name:                                                    *
*       nvVideoPort.c                                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Paul van der Kouwe 06/16/99 - created.                              *
*                                                                           *
\***************************************************************************/

#ifndef __NVVIDEOPORT_H_
#define __NVVIDEOPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

// public
DWORD __stdcall CanCreateVideoPort32       (LPDDHAL_CANCREATEVPORTDATA);
DWORD __stdcall CreateVideoPort32          (LPDDHAL_CREATEVPORTDATA);
DWORD __stdcall FlipVideoPort32            (LPDDHAL_FLIPVPORTDATA);
DWORD __stdcall GetVideoPortBandwidth32    (LPDDHAL_GETVPORTBANDWIDTHDATA);
DWORD __stdcall GetVideoPortInputFormat32  (LPDDHAL_GETVPORTINPUTFORMATDATA);
DWORD __stdcall GetVideoPortOutputFormat32 (LPDDHAL_GETVPORTOUTPUTFORMATDATA);
DWORD __stdcall GetVideoPortField32        (LPDDHAL_GETVPORTFIELDDATA);
DWORD __stdcall GetVideoPortLine32         (LPDDHAL_GETVPORTLINEDATA);
DWORD __stdcall GetVideoPortConnectInfo    (LPDDHAL_GETVPORTCONNECTDATA);
DWORD __stdcall DestroyVideoPort32         (LPDDHAL_DESTROYVPORTDATA);
DWORD __stdcall GetVideoPortFlipStatus32   (LPDDHAL_GETVPORTFLIPSTATUSDATA);
DWORD __stdcall UpdateVideoPort32          (LPDDHAL_UPDATEVPORTDATA);
DWORD __stdcall WaitForVideoPortSync32     (LPDDHAL_WAITFORVPORTSYNCDATA);
DWORD __stdcall GetVideoSignalStatus32     (LPDDHAL_GETVPORTSIGNALDATA);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __NVVIDEOPORT_H_
