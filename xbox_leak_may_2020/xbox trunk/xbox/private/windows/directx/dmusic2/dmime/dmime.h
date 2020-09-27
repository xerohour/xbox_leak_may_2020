// Copyright (c) 1998-1999 Microsoft Corporation
// dmime.h
//
#ifndef _DMIME_H_
#define _DMIME_H_

// Must be before dmusicc.h, which includes dsound.h
//
#ifdef XBOX
#define NODSOUND
#include <xtl.h>
#else // XBOX
#include <windows.h>
#define COM_NO_WINDOWS_H
#include <objbase.h>
#endif // XBOX

#include <mmsystem.h>
#include <dsoundp.h>

#include "dmusicip.h"

#include <mmsystem.h>
#define RELEASE(x)	if( NULL != x ) { x->Release(); }

#ifdef __cplusplus
extern "C" {
#endif

#ifdef XBOX
extern long g_cComponent;
#define IncrementDLLCount() 
#define DecrementDLLCount()
#else
extern long g_cComponent;
#define IncrementDLLCount() InterlockedIncrement(&g_cComponent)
#define DecrementDLLCount() InterlockedDecrement(&g_cComponent)
extern bool g_fInitCS;
extern CRITICAL_SECTION g_CritSec;
#endif

#ifdef __cplusplus
}; /* extern "C" */
#endif
DEFINE_GUID(IID_IDirectMusicPerformanceStats, 0x9301e312, 0x1f22, 0x11d3, 0x82, 0x26, 0xd2, 0xfa, 0x76, 0x25, 0x5d, 0x47);
DEFINE_GUID(IID_IDirectMusicParamHook,0x58880561, 0x5481, 0x11d3, 0x9b, 0xd1, 0xc2, 0x9f, 0xc4, 0xd1, 0xe6, 0x35);
DEFINE_GUID(IID_IDirectMusicSetParamHook,0x679c4138, 0xc62e, 0x4147, 0xb2, 0xb4, 0x9d, 0x56, 0x9a, 0xcb, 0x25, 0x4c);

#endif // _DMIME_H_
