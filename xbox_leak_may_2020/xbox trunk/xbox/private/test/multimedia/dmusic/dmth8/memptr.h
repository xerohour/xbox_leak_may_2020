//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       memptr.h
//
//--------------------------------------------------------------------------

//===========================================================================
// memptr.h
//
// History:
//  09/12/1997 - davidkl - created
//===========================================================================

#pragma once


#ifdef __cplusplus
extern "C"
{
#endif

//---------------------------------------------------------------------------

// BUGBUG - what is the "int 3" for alphas?
#define MEMJUNK                         0xCC

// Prototypes
HRESULT _stdcall helpMalloc(void **ppv, UINT uSize);
HRESULT _stdcall helpFree(void *pv);
BOOL _stdcall helpIsValidPtr(LPVOID ptr, UINT cb, BOOL fAllowNull);
BOOL _stdcall helpIsValidReadPtr(LPVOID ptr, UINT cb, BOOL fAllowNull);
BOOL _stdcall helpIsValidWritePtr(LPVOID ptr, UINT cb, BOOL fAllowNull);
LPVOID _stdcall helpGetBogusPointer(void);
void _stdcall helpFreeBogusPointer(LPVOID lpv);

//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif



























