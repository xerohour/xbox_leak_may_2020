/*
==============================================================================

    NVIDIA Corporation, (C) Copyright
    1996,1997,1998,1999. All rights reserved.

    nvpkrnl.h

    DDrawVPE-Miniport interface header

    Feb 10, 1999

==============================================================================
*/

#ifndef _NVPKRNL_H
#define _NVPKRNL_H

#include "nvprecomp.h"

// event indices
#define NVP_EVENT_MEDIAPORT 0
#define NVP_EVENT_OVERLAY1  1
#define NVP_EVENT_OVERLAY2  2
#define NVP_EVENT_MAX       3

typedef enum _NVP_STATUS {
    NVP_STATUS_SUCCESS  = 0x0,
    NVP_STATUS_FAILURE,
    NVP_STATUS_NOT_IMPLEMENTED,
    MVP_STATUS_INVALID_ARGUMENT
} NVP_STATUS;

// interface to Miniport
NVP_STATUS NVPInitialize(	GLOBALDATA *pDriverData, DWORD dwVPConnectionFlags, 
							LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
NVP_STATUS NVPUninitialize(GLOBALDATA *pDriverData);
NVP_STATUS NVPStartVideo(LPDDHAL_UPDATEVPORTDATA lpInput);
NVP_STATUS NVPStopVideo(GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
NVP_STATUS NVPUpdateVideo(LPDDHAL_UPDATEVPORTDATA lpInput);
NVP_STATUS NVPWaitForSync(GLOBALDATA *pDriverData, DWORD dwIndex, DWORD dwTimeOut);
NVP_STATUS NVPFlipVideoPort(LPDDHAL_FLIPVPORTDATA lpInput);

#ifdef VPEFSMIRROR

// [XW:09/27/2000] FS Mirror 
NVP_STATUS NVPFsMirrorEnable(GLOBALDATA *pDriverData);
NVP_STATUS NVPFsMirrorDisable(GLOBALDATA *pDriverData);

#endif

#ifdef __cplusplus
extern "C" {
#endif
DWORD __stdcall NVPUpdateOverlay(GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
BOOL  __stdcall NVPIsVPEEnabled(GLOBALDATA *pDriverData, DWORD);
DWORD __stdcall NVPCleanUp(GLOBALDATA *pDriverData, LPDDRAWI_DIRECTDRAW_GBL pDDGbl);
#ifdef __cplusplus
}
#endif



#endif
