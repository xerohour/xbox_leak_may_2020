/******************************Module*Header***********************************\
* Module Name: precomp.h
*
* Common headers used throughout the display driver.  This entire include
* file will typically be pre-compiled.
*
* Copyright (c) 1993-1996 Microsoft Corporation
\******************************************************************************/

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#ifndef _WIN32_WINNT     // NT 3.5
#include <stdio.h>
#endif                  // NT 3.5
#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <windef.h>

#if _WIN32_WINNT < 0x0500
typedef  ULONG ULONG_PTR;
__inline ULONG PtrToUlong(PVOID p) { return((ULONG) p); }
#endif

#if _WIN32_WINNT >= 0x0500
#define  __NTDDKCOMP__
#if DX8
#include <d3d8.h>
#include <ddrawint.h>
#else
#include <d3d.h>
#endif
#endif

#include <winerror.h>
#include <wingdi.h>
#include <winddi.h>
#include <devioctl.h>
#include <ntddvdeo.h>
#include <ioaccess.h>
#ifndef _WIN32_WINNT    // If this is NT 3.5x
#include <windows.h>    // NT 3.5, need Win32 API Fct Protos
#endif                  // If this is >= NT 4.0
#include <gl\gl.h>

#include "lines.h"
#ifdef NVD3D
#include "nvProcMan.h"
    #include "dmemmgr.h"
#endif
#include "memstruc.h"
#include "nvos.h"
#include "nvntioctl.h"
#include "nvEscDef.h"
#if defined(_WIN64)
#include "nvEscDef32.h"
#endif

// This file used to include driver.h but that included (indirectly) nv32.h
// Some of the files in the winnt4/displays/nv_disp directory needed to use specialized (older)
// versions of nv32.h (eg: nv1_32.h, oldnv332.h).  To resolve this issue (for now)
// I've moved the inclusion of driver.h into each of the files that needed it.  This allows
// each to include any specialized version of the nv32 header as needed.  -mlavoie

#include "debug.h"
#include "machdep.h"
#include "nvMultiMon.h"
