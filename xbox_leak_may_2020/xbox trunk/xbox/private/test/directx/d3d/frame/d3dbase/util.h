/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    util.h

Author:

    Matt Bronder

Description:

    Utility functions.

*******************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

//******************************************************************************
// Debugging function prototypes
//******************************************************************************

#if defined(DEBUG) || defined(_DEBUG) || defined(DBG)
void                DebugString(LPCTSTR szFormat, ...);
BOOL                ResultFailed(HRESULT hr, LPCTSTR sz);
#else
inline void         DebugString(LPCTSTR szFormat, ...) {}
inline BOOL         ResultFailed(HRESULT hr, LPCTSTR sz) {return FALSE;};
#endif

void                OutputString(LPCTSTR szFormat, ...);

//******************************************************************************
// Memory allocation prototypes
//******************************************************************************

BOOL                CreateHeap();
void                ReleaseHeap();
LPVOID              MemAlloc(UINT uNumBytes);
void                MemFree(LPVOID pvMem);
LPVOID              MemAlloc32(UINT uNumBytes);
void                MemFree32(LPVOID pvMem);

//******************************************************************************
// Module instance
//******************************************************************************

void                RegisterInstance(HINSTANCE hInstance);
HINSTANCE           GetInstance();

//******************************************************************************
// Miscellaneous functions
//******************************************************************************

inline void         F2L(int* pint, float f) {
                        __asm fld f;
                        __asm mov edx, pint
                        __asm fistp [edx]
                    }

#endif // __UTIL_H__
