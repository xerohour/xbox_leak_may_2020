#ifndef _NVDOUBLEBUF_H
#define _NVDOUBLEBUF_H
/**************************************************************************
** nvdoublebuf.h
**
** Export file for nvdoublebuf.c. Declare exported functions.
**
** Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
**
** THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
** NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
** IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
**
** FNicklisch 12.10.2000: New
**************************************************************************/

BOOL bDoublePumped(PPDEV ppdev, ULONG ulSurfaceOffset);
void NV_InitDoubleBufferMode(PDEV *ppdev, LONG NumBuffers, ULONG *MultiBufferOffsets);
void NV_DisableDoubleBufferMode(PPDEV  ppdev);
BOOL NV_AddDoubleBufferSurface(PPDEV ppdev, ULONG ulSurfaceOffset);
BOOL NV_RemoveDoubleBufferSurface(PPDEV ppdev, ULONG ulSurfaceOffset);
VOID NV_InitDoubleBufferSavedProcs(PPDEV ppdev);


#endif // _NVDOUBLEBUF_H
