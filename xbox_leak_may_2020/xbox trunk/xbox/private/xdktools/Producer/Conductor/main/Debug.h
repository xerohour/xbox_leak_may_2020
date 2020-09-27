//
// Debug.h
// 
// Copyright (c) 1997-1998 Microsoft Corporation. All rights reserved.
//
#ifndef _DEBUG_H_
#define _DEBUG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <windows.h>

#undef assert

#ifdef _DEBUG

extern void DebugInit(void);
extern void DebugTrace(int iDebugLevel, LPSTR pstrFormat, ...);
extern void DebugAssert(LPSTR szExp, LPSTR szFile, ULONG ulLine);

#define Trace DebugTrace

#define assert(exp) (void)( (exp) || (DebugAssert(#exp, __FILE__, __LINE__), 0) )

#else

#define Trace
#define assert(exp)	((void)0)

#endif

#endif
