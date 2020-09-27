/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

	CPUDetect.cpp

Abstract:

	Several CPU detection utilities.

Author:

    Joseph Wu (sjwu@microsoft.com)          05/98
    Bruce Line (blin@microsoft.com)         05/98
    Ming-Chieh Lee (mingcl@microsoft.com)   06/98

Revision History:

*************************************************************************/

#ifndef __CPUDETECT_H_
#define __CPUDETECT_H_

#ifdef __cplusplus
extern "C" {
#endif

// HongCho: It's better for these "support" functions to be defined regardless
//          of the platform.  If not supported either because of the architecture
//          or the API, the default return value should be just FALSE.
int g_SupportMMX (void);
int g_SupportSSE1 (void);
int g_SupportSSE2 (void);
int g_SupportMVI (void);
int g_SupportCMOV (void);


#ifdef _M_IX86
int cpuidIsIntelP5 (void); // To detect Intel P5
int g_CPUClock (void);
#endif // _M_IX86

#ifdef _Embedded_x86
int cpuidIsIntelP5 (void); // to detect 486, P5, or P6
#endif

#ifdef macintosh
int g_SupportAltiVec(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __CPUDETECT_H_
