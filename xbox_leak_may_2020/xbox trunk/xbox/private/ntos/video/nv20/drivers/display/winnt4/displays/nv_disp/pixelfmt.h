#ifndef _PIXELFMT_H
#define _PIXELFMT_H
//******************************Module*Header***********************************
// Module Name: pixelfmt.h
//
// export header for pixelfmt.c
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

BOOL bOglStereoSurfacesAllowed(PPDEV ppdev);
BOOL bOglPfdCheckFlags(PPDEV ppdev, ULONG *pulPfdCheckFlags);
int cGetWindowFlippingControl(PPDEV ppdev);

#endif // _PIXELFMT_H
