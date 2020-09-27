#ifndef _WKSAPPSUPPORT_H
#define _WKSAPPSUPPORT_H
/**************************************************************************
** wksappsupport.h
**
** This module handles the ESC_NV_WKS_APP_SUPPORT escape to 
** support the workstation tools interface.
**
** Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
**
** THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
** NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
** IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
**
** FNicklisch 09.11.2000: New
**************************************************************************/

LONG lWksAppSupport(SURFOBJ *pso, 
                    ULONG cjIn, VOID *pvIn,
                    ULONG cjOut, VOID *pvOut);

#define WKS_ESC_SUCCEED      1
#define WKS_ESC_UNSUPPORTED  0
#define WKS_ESC_FAILED      -1

#endif // _WKSAPPSUPPORT_H
