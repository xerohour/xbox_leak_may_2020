#ifndef _OGLFLIP_H
#define _OGLFLIP_H
//******************************Module*Header***********************************
// Module Name: oglflip.h
//
// Export file for maintenance code for OpenGL flipping.
//
// Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
//
// THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
// NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
// IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
//
//******************************************************************************

BOOL bOglPageFlipModeUpdate(PPDEV ppdev);
BOOL bOglPageFlipModeEnable(PPDEV ppdev, struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL bOglPageFlipModeDisable(PPDEV ppdev, struct _NV_OPENGL_CLIENT_INFO *clientInfo);
BOOL bOglPageFlipModeDisableAll(PPDEV ppdev);
BOOL bOglPageFlipModeRetryEnable(PPDEV ppdev);
ULONG cOglFlippingClients(PPDEV ppdev);

#endif // _OGLFLIP_H
